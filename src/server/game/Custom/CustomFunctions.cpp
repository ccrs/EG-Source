#include "CustomFunctions.h"
#include "CreatureAI.h"
#include "Map.h"
#include "MotionMaster.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "SmartAI.h"
#include "Unit.h"
#include "World.h"
#include "Creature.h"


void Creature::ProcessDelayedLOSEntries()
{
    if (_LOSQueue.empty() || !IsAlive() || HasUnitState(UNIT_STATE_SIGHTLESS) || !IsAIEnabled())
    {
        _LOSQueue = {};
        _uniqueLOSEntries.clear();
        _moveInLOSLockStatus = LOS_LOCK_NONE;
        return;
    }

    _moveInLOSLockStatus = LOS_LOCK_PROCESSING;
    for (auto itr = _LOSQueue.begin(); itr != _LOSQueue.end();)
    {
        if (Unit* current = ObjectAccessor::GetUnit(*this, *itr))
        {
            if (Creature* currentCreature = current->ToCreature())
                if (currentCreature->GetLOSLockStatus() == LOS_LOCK_SPAWN)
                {
                    ++itr;
                    continue;
                }

            if (current->IsAlive() && !current->IsInFlight() && CanSeeOrDetect(current, false, true))
                AI()->MoveInLineOfSight(current);
        }
        _uniqueLOSEntries.erase(*itr);
        itr = _LOSQueue.erase(itr);
    }

    _moveInLOSLockStatus = LOS_LOCK_NONE;
}

void Creature::InsertLOSEntry(ObjectGuid guid)
{
    if (!_uniqueLOSEntries.contains(guid))
    {
        _uniqueLOSEntries.insert(guid);
        _LOSQueue.push_back(guid);
    }
}

void WorldObject::GetNearPoint2D(WorldObject const* searcher, Position const* reference, float& x, float& y, float distance, float absAngle) const
{
    float effectiveReach = GetCombatReach();

    if (searcher)
    {
        effectiveReach += searcher->GetCombatReach();

        if (this != searcher)
        {
            float myHover = 0.0f, searcherHover = 0.0f;
            if (Unit const* unit = ToUnit())
                myHover = unit->GetHoverOffset();
            if (Unit const* searchUnit = searcher->ToUnit())
                searcherHover = searchUnit->GetHoverOffset();

            float hoverDelta = myHover - searcherHover;
            if (hoverDelta != 0.0f)
                effectiveReach = std::sqrt(std::max(effectiveReach * effectiveReach - hoverDelta * hoverDelta, 0.0f));
        }
    }

    x = reference->GetPositionX() + (effectiveReach + distance) * std::cos(absAngle);
    y = reference->GetPositionY() + (effectiveReach + distance) * std::sin(absAngle);

    Trinity::NormalizeMapCoord(x);
    Trinity::NormalizeMapCoord(y);
}

void WorldObject::GetNearPoint(WorldObject const* searcher, Position const* reference, float& x, float& y, float& z, float distance2d, float absAngle) const
{
    GetNearPoint2D(searcher, reference, x, y, distance2d, absAngle);
    z = reference->GetPositionZ();
    (searcher ? searcher : this)->UpdateAllowedPositionZ(x, y, z);

    // if detection disabled, return first point
    if (!sWorld->getBoolConfig(CONFIG_DETECT_POS_COLLISION))
        return;

    // return if the point is already in LoS
    Position destination(x, y, z);
    if (IsWithinLOSInMap(&destination, searcher))
        return;

    // remember first point
    float first_x = x;
    float first_y = y;
    float first_z = z;

    // loop in a circle to look for a point in LoS using small steps
    for (float angle = float(M_PI) / 8; angle < float(M_PI) * 2; angle += float(M_PI) / 8)
    {
        GetNearPoint2D(searcher, reference, x, y, distance2d, absAngle + angle);
        z = reference->GetPositionZ();
        (searcher ? searcher : this)->UpdateAllowedPositionZ(x, y, z);
        destination.Relocate(x, y, z);
        if (IsWithinLOSInMap(&destination, searcher))
            return;
    }

    // still not in LoS, give up and return first position found
    x = first_x;
    y = first_y;
    z = first_z;
}

bool WorldObject::IsWithinLOSInMap(Position const* reference, WorldObject const* obj, LineOfSightChecks checks, VMAP::ModelIgnoreFlags ignoreFlags) const
{
    if (!IsInMap(obj))
        return false;

    float ox, oy, oz;
    if (obj->GetTypeId() == TYPEID_PLAYER)
    {
        obj->GetPosition(ox, oy, oz);
        oz += GetCollisionHeight();
    }
    else
        obj->GetHitSpherePointFor({ reference->GetPositionX(), reference->GetPositionY(), reference->GetPositionZ() + GetCollisionHeight() }, ox, oy, oz);

    float x, y, z;
    if (GetTypeId() == TYPEID_PLAYER)
    {
        reference->GetPosition(x, y, z);
        z += GetCollisionHeight();
    }
    else
        GetHitSpherePointFor({ reference->GetPositionX(), reference->GetPositionY(), reference->GetPositionZ() + obj->GetCollisionHeight() }, x, y, z);

    return GetMap()->isInLineOfSight(x, y, z, ox, oy, oz, GetPhaseMask(), checks, ignoreFlags);
}

bool EG::MostHPMissingFriendlyUnitInRangeSearcher::operator()(Unit* unit)
{
    if (_includeSelf && _source == unit && unit->GetHealthPct() < _hp)
    {
        _hp = unit->GetHealthPct();
        return true;
    }

    if (((_playerOnly && unit->IsPlayer()) || !_playerOnly)
        && unit->IsAlive()
        && unit->IsInCombat()
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _range)
        && unit->GetHealthPct() < _hp
    )
    {
        _hp = unit->GetHealthPct();
        return true;
    }

    return false;
}

void SmartAI::SetCombatMovement()
{
    if (!me->IsAlive() || !CanCombatMove() || !me->GetVictim())
        return;

    Optional<ChaseRange> chaseRange = _combatDistance ? ChaseRange(_combatDistance) : Optional<ChaseRange>{ };
    me->GetMotionMaster()->MoveChase(me->GetVictim(), chaseRange);
}
