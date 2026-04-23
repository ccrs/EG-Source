/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FollowMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "Optional.h"
#include "PathGenerator.h"
#include "Pet.h"
#include "Player.h"
#include "Unit.h"
#include "Util.h"

static void DoMovementInform(Unit* owner, Unit* target)
{
    if (owner->GetTypeId() != TYPEID_UNIT)
        return;

    if (CreatureAI* AI = owner->ToCreature()->AI())
        AI->MovementInform(FOLLOW_MOTION_TYPE, target->GetGUID().GetCounter());
}

FollowMovementGenerator::FollowMovementGenerator(Unit* target, float range, ChaseAngle angle, Optional<bool> run/* = {}*/) : AbstractFollower(ASSERT_NOTNULL(target)), _range(range), _angle(angle), _checkTimer(CHECK_INTERVAL), _relocationCooldown(0), _run(run)
{
    Mode = MOTION_MODE_DEFAULT;
    Priority = MOTION_PRIORITY_NORMAL;
    Flags = MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING;
    BaseUnitState = UNIT_STATE_FOLLOW;
}
FollowMovementGenerator::~FollowMovementGenerator() = default;

static bool PositionOkay(Unit* owner, Unit* target, float range, Optional<ChaseAngle> angle = {})
{
    if (owner->GetExactDistSq(target) > square(owner->GetCombatReach() + target->GetCombatReach() + range))
        return false;

    return !angle || angle->IsAngleOkay(target->GetRelativeAngle(owner));
}

static bool AngleOkayRelaxed(ChaseAngle const& angle, float relativeAngle, float tolerance)
{
    if (angle.IsAngleOkay(relativeAngle))
        return true;

    float const diffUpper = Position::NormalizeOrientation(relativeAngle - angle.UpperBound());
    float const diffLower = Position::NormalizeOrientation(angle.LowerBound() - relativeAngle);
    float const nearestDiff = diffUpper < diffLower ? diffUpper : diffLower;

    return nearestDiff <= tolerance;
}

static bool PositionOkayRelaxed(Unit* owner, Unit* target, float range, ChaseAngle const& angle)
{
    if (owner->GetExactDistSq(target) > square(owner->GetCombatReach() + target->GetCombatReach() + range))
        return false;

    return AngleOkayRelaxed(angle, target->GetRelativeAngle(owner), 0.15f);
}

static bool PositionOkayStrict(Unit* owner, Unit* target, float range, ChaseAngle const& angle)
{
    if (owner->GetExactDistSq(target) > square(owner->GetCombatReach() + target->GetCombatReach() + range))
        return false;

    return angle.IsAngleOkay(target->GetRelativeAngle(owner));
}

static float GetStableFollowDistance(float range)
{
    if (range <= 0.0f)
        return 0.0f;

    float const inset = FOLLOW_RANGE_TOLERANCE * 0.5f;

    if (range <= inset)
        return range;

    return range - inset;
}

bool FollowMovementGenerator::Initialize(Unit* owner)
{
    RemoveFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING | MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    AddFlag(MOVEMENTGENERATOR_FLAG_INITIALIZED | MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);

    UpdatePetSpeed(owner);
    _path = nullptr;
    _lastTargetPosition.reset();
    _checkTimer.Reset(0);
    _relocationCooldown.Reset(0s);
    return false;
}

bool FollowMovementGenerator::Reset(Unit* owner)
{
    RemoveFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);

    return Initialize(owner);
}

bool FollowMovementGenerator::Update(Unit* owner, uint32 diff)
{
    // owner might be dead or gone
    if (!owner || !owner->IsAlive())
        return false;

    // our target might have gone away
    Unit* const target = GetTarget();
    if (!target || !target->IsInWorld())
        return false;

    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE) || owner->IsMovementPreventedByCasting())
    {
        _path = nullptr;
        owner->StopMoving();
        _lastTargetPosition.reset();
        _relocationCooldown.Reset(0s);
        return true;
    }

    if (!_relocationCooldown.Passed())
        _relocationCooldown.Update(diff);

    _checkTimer.Update(diff);
    if (_checkTimer.Passed())
    {
        _checkTimer.Reset(CHECK_INTERVAL);

        float const acceptableRange = _range + FOLLOW_RANGE_TOLERANCE;
        float const curAngle = target->GetRelativeAngle(owner);
        bool const relaxedPositionOkay = PositionOkayRelaxed(owner, target, acceptableRange, _angle);

        /*
         * Final movement completion must use strict angle validation.
         * The relaxed angle is only anti-jitter hysteresis and must not be allowed
         * to complete the movement or send MovementInform.
         */
        if (PositionOkayStrict(owner, target, acceptableRange, _angle))
        {
            if (owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE))
            {
                owner->StopMoving();
                owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
            }

            if (HasFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED))
            {
                RemoveFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);
                DoMovementInform(owner, target);
            }

            _path = nullptr;
            _lastTargetPosition.reset();
            return true;
        }

        Position const currentTargetPosition = target->GetPosition();
        bool const angleOkayStrict = _angle.IsAngleOkay(curAngle);
        bool const angleNeedsCorrection = !angleOkayStrict;
        bool const distanceNeedsCorrection = !PositionOkay(owner, target, acceptableRange);
        bool const relocationCooldownActive = _relocationCooldown.GetExpiry() != 0s;

        /*
         * Close angle-only correction can still be throttled to reduce jitter,
         * but it must not be considered "done" unless the strict angle is valid.
         */
        bool const closeAngleOnlyCorrection = !distanceNeedsCorrection && angleNeedsCorrection;
        if (closeAngleOnlyCorrection && relocationCooldownActive && !_relocationCooldown.Passed())
            return true;

        /*
         * Reconsider movement if:
         * - target moved,
         * - strict angle is invalid,
         * - distance is outside accepted follow range,
         * - or a previously-blocked close relocation cooldown expired.
         */
        if (!_lastTargetPosition || !currentTargetPosition.IsInDist(_lastTargetPosition.value(), 0.5f) || angleNeedsCorrection || distanceNeedsCorrection || (relocationCooldownActive && _relocationCooldown.Passed()))
        {
            if (distanceNeedsCorrection || angleNeedsCorrection || owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE))
            {
                if (!_path)
                    _path = std::make_unique<PathGenerator>(owner);

                float x, y, z;
                float tAngle;
                if (angleOkayStrict)
                    tAngle = curAngle;
                else
                    tAngle = _angle.RelativeAngle;

                float const desiredFollowDistance = GetStableFollowDistance(_range);
                float const effectiveFollowDistance = closeAngleOnlyCorrection ? _range : desiredFollowDistance;
                target->GetNearPoint(owner, x, y, z, effectiveFollowDistance, target->ToAbsoluteAngle(tAngle));

                Position const destination(x, y, z, target->GetOrientation());
                float const destinationRelativeAngle = target->GetRelativeAngle(&destination);
                if (!_angle.IsAngleOkay(destinationRelativeAngle) && angleOkayStrict)
                    target->GetNearPoint(owner, x, y, z, effectiveFollowDistance, target->ToAbsoluteAngle(_angle.RelativeAngle));

                // pets are allowed to "cheat" on pathfinding when following their master
                bool allowShortcut = false;
                if (!owner->GetCharmerOrOwnerGUID().IsEmpty() && target->GetGUID() == owner->GetCharmerOrOwnerGUID())
                    allowShortcut = true;
                else if (owner->IsPet() && owner->ToPet()->GetOwner()->GetGUID() == target->GetGUID())
                    allowShortcut = true;

                bool success = _path->CalculatePath(x, y, z, allowShortcut);
                bool noPathFound = success && (_path->GetPathType() & PATHFIND_NOPATH);
                if ((!success && !allowShortcut) || (success && !allowShortcut && noPathFound))
                {
                    owner->StopMoving();

                    // Short retry delay. Avoid repeated path spam near the target.
                    if (closeAngleOnlyCorrection)
                        _relocationCooldown.Reset(1s);
                    else
                        _relocationCooldown.Reset(0s);

                    _lastTargetPosition = currentTargetPosition;
                    return true;
                }

                owner->AddUnitState(UNIT_STATE_FOLLOW_MOVE);
                AddFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);

                Movement::MoveSplineInit init(owner);
                if (success && !noPathFound)
                    init.MovebyPath(_path->GetPath());
                else
                    init.MoveTo(x, y, z, false, true);

                init.SetWalk(_run.has_value() ? !_run.value() : owner->IsWalking());
                init.SetFacing(target->GetOrientation());
                init.Launch();

                _lastTargetPosition = currentTargetPosition;

                /*
                 * Only throttle optional close angle relocation.
                 *
                 * Since angleNeedsCorrection is now strict, this cooldown applies when:
                 * - distance is okay,
                 * - angle is strict-invalid,
                 * - and we just launched an angle correction.
                 */
                if (closeAngleOnlyCorrection)
                    _relocationCooldown.Reset(randtime(2s, 4s));
                else
                    _relocationCooldown.Reset(0s);

                return true;
            }

            _lastTargetPosition = currentTargetPosition;
        }

        /*
         * If relaxed is okay but strict is not, we intentionally do not finish.
         * This avoids declaring the follow movement complete at a final invalid angle.
         */
        if (relaxedPositionOkay)
            return true;
    }

    if (owner->HasUnitState(UNIT_STATE_FOLLOW_MOVE) && owner->movespline->Finalized())
    {
        /*
         * Do not blindly inform on finalized spline if the target moved and the final
         * position is no longer strictly valid.
         */
        if (!PositionOkayStrict(owner, target, _range + FOLLOW_RANGE_TOLERANCE, _angle))
        {
            _path = nullptr;
            owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
            AddFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);
            _lastTargetPosition.reset();
            return true;
        }

        RemoveFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);
        _path = nullptr;
        owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
        _lastTargetPosition.reset();
        DoMovementInform(owner, target);
    }

    return true;
}

void FollowMovementGenerator::Deactivate(Unit* owner)
{
    AddFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    RemoveFlag(MOVEMENTGENERATOR_FLAG_TRANSITORY | MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);
    owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
}

void FollowMovementGenerator::Finalize(Unit* owner, bool active, bool/* movementInform*/)
{
    AddFlag(MOVEMENTGENERATOR_FLAG_FINALIZED);
    if (active)
    {
        owner->ClearUnitState(UNIT_STATE_FOLLOW_MOVE);
        UpdatePetSpeed(owner);
    }
}

void FollowMovementGenerator::UpdatePetSpeed(Unit* owner)
{
    if (Pet* oPet = owner->ToPet())
    {
        if (!GetTarget() || GetTarget()->GetGUID() == owner->GetOwnerGUID())
        {
            oPet->UpdateSpeed(MOVE_RUN);
            oPet->UpdateSpeed(MOVE_WALK);
            oPet->UpdateSpeed(MOVE_SWIM);
        }
    }
}
