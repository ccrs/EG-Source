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

#include "ChaseMovementGenerator.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "G3DPosition.hpp"
#include "Map.h"
#include "MotionMaster.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "PathGenerator.h"
#include "SmartAI.h"
#include "Unit.h"
#include "Util.h"

static bool PositionOkay(Unit* owner, Unit* target, Optional<float> minDistance, Optional<float> maxDistance, Optional<ChaseAngle> angle)
{
    float const distSq = owner->GetExactDistSq(target);
    if (minDistance && distSq < square(*minDistance))
        return false;
    if (maxDistance && distSq > square(*maxDistance))
        return false;
    if (angle && !angle->IsAngleOkay(target->GetRelativeAngle(owner)))
        return false;
    if (!owner->IsWithinLOSInMap(target))
        return false;
    return true;
}

static void DoMovementInform(Unit* owner, Unit* target)
{
    if (owner->GetTypeId() != TYPEID_UNIT)
        return;

    if (CreatureAI* AI = owner->ToCreature()->AI())
        AI->MovementInform(CHASE_MOTION_TYPE, target->GetGUID().GetCounter());
}

static bool ShouldRandomizeChaseStopDistance(Unit* owner, Unit* target, Optional<ChaseRange> const& range, float minTarget, float maxTarget)
{
    // Do not randomize default melee chase.
    if (!range)
        return false;

    float const meleeRange = owner->GetMeleeRange(target);
    if (maxTarget <= meleeRange + 5.0f)
        return false;

    float const availableRoom = maxTarget - minTarget;
    if (availableRoom <= 6.0f)
        return false;

    return true;
}

static float SelectRandomizedChaseStopDistance(Unit* owner, Unit* target, Optional<ChaseRange> const& range, float minTarget, float maxTarget)
{
    if (!ShouldRandomizeChaseStopDistance(owner, target, range, minTarget, maxTarget))
        return maxTarget;

    float const availableRoom = maxTarget - minTarget;
    float lowerDistance = minTarget + availableRoom * 0.35f;
    float upperDistance = minTarget + availableRoom * 0.80f;
    if (lowerDistance < minTarget + 2.0f)
        lowerDistance = minTarget + 2.0f;
    if (upperDistance > maxTarget - 2.0f)
        upperDistance = maxTarget - 2.0f;
    if (upperDistance <= lowerDistance)
        return maxTarget;

    return frand(lowerDistance, upperDistance);
}

static bool IsValidChaseStopDistance(float distance, float minTarget, float maxTarget)
{
    return distance > 0.0f && distance >= minTarget && distance <= maxTarget;
}

static constexpr float FACING_BROADCAST_THRESHOLD = 0.0873f; // 5 degrees

static void BroadcastFacingIfNeeded(Unit* owner, Unit* target, Optional<float>& lastBroadcastedAngle)
{
    if (owner->HasUnitState(UNIT_STATE_CANNOT_TURN))
        return;

    float const currentAngle = owner->GetAbsoluteAngle(target);
    bool sendFacingUpdate = !lastBroadcastedAngle;
    if (!sendFacingUpdate)
    {
        float delta = Position::NormalizeOrientation(currentAngle - *lastBroadcastedAngle);
        if (delta > float(M_PI))
            delta -= 2.0f * float(M_PI);
        sendFacingUpdate = std::fabs(delta) > FACING_BROADCAST_THRESHOLD;
    }
    if (sendFacingUpdate)
    {
        lastBroadcastedAngle = currentAngle;
        Movement::MoveSplineInit facingInit(owner);
        facingInit.MoveTo(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), false);
        facingInit.SetFacing(currentAngle);
        facingInit.Launch();
        owner->DisableSpline();
    }
}

ChaseMovementGenerator::ChaseMovementGenerator(Unit* target, Optional<ChaseRange> range, Optional<ChaseAngle> angle) : AbstractFollower(ASSERT_NOTNULL(target)), _range(range), _angle(angle), _rangeCheckTimer(0), _relocationCooldown(0)
{
    Mode = MOTION_MODE_DEFAULT;
    Priority = MOTION_PRIORITY_NORMAL;
    Flags = MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING;
    BaseUnitState = UNIT_STATE_CHASE;
}
ChaseMovementGenerator::~ChaseMovementGenerator() = default;

bool ChaseMovementGenerator::Initialize(Unit* owner)
{
    RemoveFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING | MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    AddFlag(MOVEMENTGENERATOR_FLAG_INITIALIZED | MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);

    _path = nullptr;
    _lastTargetPosition.reset();
    _lastBroadcastedFacingAngle.reset();
    _rangeCheckTimer.Reset(0);
    _relocationCooldown.Reset(0s);
    _currentChaseStopDistance = 0.0f;
    owner->StopMoving();
    return false;
}

bool ChaseMovementGenerator::Reset(Unit* owner)
{
    RemoveFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);

    return Initialize(owner);
}

bool ChaseMovementGenerator::Update(Unit* owner, uint32 diff)
{
    // owner might be dead or gone (can we even get nullptr here?)
    if (!owner || !owner->IsAlive())
        return false;

    // our target might have gone away
    Unit* const target = GetTarget();
    if (!target || !target->IsInWorld())
        return false;

    // the owner might be unable to move (rooted or casting), or we have lost the target, pause movement
    if (owner->HasUnitState(UNIT_STATE_NOT_MOVE) || owner->IsMovementPreventedByCasting() || _HasLostTarget(owner, target))
    {
        owner->StopMoving();
        _lastTargetPosition.reset();
        _lastBroadcastedFacingAngle.reset();
        _currentChaseStopDistance = 0.0f;
        if (Creature* cOwner = owner->ToCreature())
            cOwner->SetCannotReachTarget(false);
        return true;
    }

    bool const useChaseAngle = _UseChaseAngle(owner, target);
    float const hitboxSum = owner->GetCombatReach() + target->GetCombatReach();
    float const minRange = _range ? _range->MinRange + hitboxSum : CONTACT_DISTANCE;
    float const minTarget = (_range ? _range->MinTolerance : 0.0f) + hitboxSum;
    float const maxRange = _range ? _range->MaxRange + hitboxSum : owner->GetMeleeRange(target); // melee range already includes hitboxes
    float const maxTarget = _range ? _range->MaxTolerance + hitboxSum : CONTACT_DISTANCE + hitboxSum;
    Optional<ChaseAngle> angle = useChaseAngle ? _angle : Optional<ChaseAngle>();

    // Decoupled from range-check timer: sync facing every tick when settled so slight owner displacement still updates server orientation.
    bool syncFacingOrientation = !owner->HasUnitState(UNIT_STATE_CHASE_MOVE);
    _relocationCooldown.Update(diff);
    _rangeCheckTimer.Update(diff);
    if (_rangeCheckTimer.Passed())
    {
        _rangeCheckTimer.Reset(RANGE_CHECK_INTERVAL);

        // Avoid relying on stale/uninitialized _movingTowards when not actively chasing.
        bool const rangeCheckMovingTowards = owner->HasUnitState(UNIT_STATE_CHASE_MOVE) ? _movingTowards : !owner->IsInDist(target, maxRange);
        if (rangeCheckMovingTowards)
        {
            if (!IsValidChaseStopDistance(_currentChaseStopDistance, minTarget, maxTarget))
                _currentChaseStopDistance = SelectRandomizedChaseStopDistance(owner, target, _range, minTarget, maxTarget);
        }
        else
            _currentChaseStopDistance = 0.0f;

        float const activeMovingTowardsStopDistance = rangeCheckMovingTowards ? _currentChaseStopDistance : maxTarget;
        if (HasFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED) && PositionOkay(owner, target, rangeCheckMovingTowards ? Optional<float>() : minTarget, rangeCheckMovingTowards ? activeMovingTowardsStopDistance : Optional<float>(), angle))
        {
            RemoveFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);
            _path = nullptr;
            _currentChaseStopDistance = 0.0f;
            if (Creature* cOwner = owner->ToCreature())
                cOwner->SetCannotReachTarget(false);
            owner->StopMoving();
            owner->ClearUnitState(UNIT_STATE_CHASE_MOVE);
            owner->SetInFront(target);
            DoMovementInform(owner, target);
            return true;
        }

        Position const currentTargetPosition = target->GetPosition();
        bool const targetPositionChanged = !_lastTargetPosition || !currentTargetPosition.IsInDist(_lastTargetPosition.value(), 0.01f);
        bool const chaseAngleModeChanged = useChaseAngle != _useChaseAngle;
        bool const relocationCooldownExpired = _relocationCooldown.Expired();

        // Reconsider movement if:
        // - the target moved,
        // - the chase angle mode changed,
        // - or a previously-blocked relocation cooldown has expired.
        if (targetPositionChanged || chaseAngleModeChanged || relocationCooldownExpired)
        {
            bool const alreadyMoving = owner->HasUnitState(UNIT_STATE_CHASE_MOVE);
            bool const positionInvalid = !PositionOkay(owner, target, minRange, maxRange, angle);
            if (alreadyMoving || positionInvalid)
            {
                Creature* const cOwner = owner->ToCreature();
                // can we get to the target?
                if (cOwner && !target->isInAccessiblePlaceFor(cOwner))
                {
                    cOwner->SetCannotReachTarget(true);
                    cOwner->StopMoving();
                    _path = nullptr;
                    _relocationCooldown.Reset(0s);
                    _currentChaseStopDistance = 0.0f;
                    _lastTargetPosition = currentTargetPosition;
                    _useChaseAngle = useChaseAngle;
                    return true;
                }

                // figure out which way we want to move
                _movingTowards = !owner->IsInDist(target, maxRange);

                bool const angleMismatch = angle && !angle->IsAngleOkay(target->GetRelativeAngle(owner));
                bool const closeRangeRelocation = !_movingTowards;

                // Only optional close-range relocation should be throttled.
                // Do not throttle:
                // - normal chasing toward target,
                // - angle correction,
                // - repathing while already moving.
                bool const shouldThrottleRelocation = closeRangeRelocation && !angleMismatch && !alreadyMoving;
                if (shouldThrottleRelocation && !_relocationCooldown.Passed())
                {
                    owner->SetInFront(target);
                    BroadcastFacingIfNeeded(owner, target, _lastBroadcastedFacingAngle);
                    _lastTargetPosition = currentTargetPosition;
                    _useChaseAngle = useChaseAngle;
                    return true;
                }

                // make a new path if we have to...
                if (!_path)
                    _path = std::make_unique<PathGenerator>(owner);

                Position destination;
                bool shortenPath = false;

                // If we are moving toward a target with a large explicit chase range,
                // pick a randomized stop distance inside the max chase distance.
                float desiredTargetDistance = minTarget;
                if (_movingTowards)
                {
                    if (!IsValidChaseStopDistance(_currentChaseStopDistance, minTarget, maxTarget))
                        _currentChaseStopDistance = SelectRandomizedChaseStopDistance(owner, target, _range, minTarget, maxTarget);

                    desiredTargetDistance = _currentChaseStopDistance;
                }
                else
                    _currentChaseStopDistance = 0.0f;

                float const calculationDistance = desiredTargetDistance - hitboxSum;
                float const calculationAngle = angle ? target->ToAbsoluteAngle(angle->RelativeAngle) : target->GetAbsoluteAngle(owner);
                // if we want to move toward the target and there's no fixed angle...
                if (_movingTowards && !angle)
                {
                    // ...we'll pathfind to the center, then shorten the path
                    target->GetPosition(destination.m_positionX, destination.m_positionY, destination.m_positionZ);
                    shortenPath = true;
                }
                else // otherwise, we fall back to nearpoint finding
                    target->GetNearPoint(owner, destination.m_positionX, destination.m_positionY, destination.m_positionZ, calculationDistance, calculationAngle);

                if (owner->IsHovering())
                    owner->UpdateAllowedPositionZ(destination.m_positionX, destination.m_positionY, destination.m_positionZ);
                else if (owner->IsFlying()
                    && owner->GetTypeId() == TYPEID_UNIT
                    && owner->ToCreature()->HasStoredMovementFlag(MOVEMENTFLAG_HOVER)
                    && owner->GetFloatValue(UNIT_FIELD_HOVERHEIGHT)
                    && !owner->ToCreature()->IsInAir(destination, owner->GetMap()->GetHeight(owner->GetPhaseMask(), destination))
                )
                {
                    target->GetNearPoint(owner, destination.m_positionX, destination.m_positionY, destination.m_positionZ, calculationDistance, calculationAngle);
                    shortenPath = false;
                }

                if (owner->GetTypeId() == TYPEID_UNIT && owner->IsAIEnabled() && !dynamic_cast<SmartAI*>(owner->ToCreature()->AI()) && !owner->IsWithinLOSInMap(&destination, target))
                {
                    target->GetNearPoint(owner, destination.m_positionX, destination.m_positionY, destination.m_positionZ, calculationDistance / 2.f, calculationAngle);
                    shortenPath = false;
                    if (!owner->IsWithinLOSInMap(&destination, target))
                    {
                        target->GetPosition(destination.m_positionX, destination.m_positionY, destination.m_positionZ);
                        shortenPath = true;
                    }
                }

                bool const success = _path->CalculatePath(destination.m_positionX, destination.m_positionY, destination.m_positionZ, owner->CanFly());
                if (!success || (_path->GetPathType() & (PATHFIND_NOPATH /* | PATHFIND_INCOMPLETE*/)))
                {
                    if (cOwner)
                        cOwner->SetCannotReachTarget(true);
                    owner->StopMoving();
                    _currentChaseStopDistance = 0.0f;

                    // Path failure should not consume the long 20-30s relocation cooldown.
                    // Use a short retry delay only for close-range relocation to avoid path spam.
                    if (closeRangeRelocation)
                        _relocationCooldown.Reset(2s);
                    else
                        _relocationCooldown.Reset(0s);

                    _lastTargetPosition = currentTargetPosition;
                    _useChaseAngle = useChaseAngle;
                    return true;
                }

                if (shortenPath)
                    _path->ShortenPathUntilDist(PositionToVector3(target), desiredTargetDistance);

                if (cOwner)
                    cOwner->SetCannotReachTarget(false);

                bool walk = false;
                if (cOwner && !cOwner->IsPet())
                {
                    switch (cOwner->GetMovementTemplate().GetChase())
                    {
                        case CreatureChaseMovementType::CanWalk:
                            walk = owner->IsWalking();
                            break;
                        case CreatureChaseMovementType::AlwaysWalk:
                            walk = true;
                            break;
                        default:
                            break;
                    }
                }

                _lastBroadcastedFacingAngle.reset();
                owner->AddUnitState(UNIT_STATE_CHASE_MOVE);
                AddFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);

                Movement::MoveSplineInit init(owner);
                init.MovebyPath(_path->GetPath());
                init.SetWalk(walk);
                init.SetFacing(target);
                init.Launch();

                // Store the randomized stop distance only while moving toward the target.
                // This prevents the range-check block from stopping the spline early at maxTarget.
                _currentChaseStopDistance = _movingTowards ? desiredTargetDistance : 0.0f;

                // Commit target state only after this movement decision was actually handled.
                _lastTargetPosition = currentTargetPosition;
                _useChaseAngle = useChaseAngle;

                // Only successful optional close-range relocation starts the long cooldown.
                if (shouldThrottleRelocation)
                    _relocationCooldown.Reset(randtime(20s, 30s));
                else if (_movingTowards || angleMismatch)
                    _relocationCooldown.Reset(0s);

                return true;
            }

            // Target/angle change was seen, but no movement correction was needed.
            _lastTargetPosition = currentTargetPosition;
            _useChaseAngle = useChaseAngle;
        }
        syncFacingOrientation = true;
        if (!owner->HasUnitState(UNIT_STATE_CHASE_MOVE))
            BroadcastFacingIfNeeded(owner, target, _lastBroadcastedFacingAngle);
    }

    // if we're done moving, we want to clean up
    if (owner->HasUnitState(UNIT_STATE_CHASE_MOVE) && owner->movespline->Finalized())
    {
        RemoveFlag(MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);
        _path = nullptr;
        _currentChaseStopDistance = 0.0f;
        if (Creature* cOwner = owner->ToCreature())
            cOwner->SetCannotReachTarget(false);
        owner->ClearUnitState(UNIT_STATE_CHASE_MOVE);
        owner->SetInFront(target);
        _lastBroadcastedFacingAngle.reset();
        BroadcastFacingIfNeeded(owner, target, _lastBroadcastedFacingAngle);
        DoMovementInform(owner, target);
    }

    if (syncFacingOrientation)
        owner->SetInFront(target);
    return true;
}

void ChaseMovementGenerator::Deactivate(Unit* owner)
{
    AddFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED);
    RemoveFlag(MOVEMENTGENERATOR_FLAG_TRANSITORY | MOVEMENTGENERATOR_FLAG_INFORM_ENABLED);
    owner->ClearUnitState(UNIT_STATE_CHASE_MOVE);
    _path = nullptr;
    if (Creature* cOwner = owner->ToCreature())
        cOwner->SetCannotReachTarget(false);
}

void ChaseMovementGenerator::Finalize(Unit* owner, bool active, bool/* movementInform*/)
{
    AddFlag(MOVEMENTGENERATOR_FLAG_FINALIZED);
    if (active)
    {
        owner->ClearUnitState(UNIT_STATE_CHASE_MOVE);
        if (Creature* cOwner = owner->ToCreature())
            cOwner->SetCannotReachTarget(false);
    }
}

bool ChaseMovementGenerator::_HasLostTarget(Unit* owner, Unit* target)
{
    if (CheckLostTarget)
        return owner->GetVictim() != target;
    return false;
}

bool ChaseMovementGenerator::_UseChaseAngle(Unit* owner, Unit* target)
{
    if (ForceAngle)
        return true;

    if (!CheckLostTarget)
        return false;

    MovementGeneratorType targetMovementType = target->GetMotionMaster()->GetCurrentMovementGeneratorType();
    if (targetMovementType == CHASE_MOTION_TYPE)
        if (ChaseMovementGenerator* movement = dynamic_cast<ChaseMovementGenerator*>(target->GetMotionMaster()->GetCurrentMovementGenerator()))
            return movement->GetTarget() != owner;

    if (targetMovementType == IDLE_MOTION_TYPE || targetMovementType == RANDOM_MOTION_TYPE || targetMovementType == WAYPOINT_MOTION_TYPE)
        return target->GetVictim() != owner;

    return false;
}
