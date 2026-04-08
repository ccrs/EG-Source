#include "CustomFunctions.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Map.h"
#include "MotionMaster.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Player.h"
#include "SmartAI.h"
#include "TemporarySummon.h"
#include "Unit.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "World.h"


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

Item* Player::GetWeaponForDamageMods(WeaponAttackType attackType) const
{
    uint8 slot;
    switch (attackType)
    {
        case BASE_ATTACK:   slot = EQUIPMENT_SLOT_MAINHAND; break;
        case OFF_ATTACK:    slot = EQUIPMENT_SLOT_OFFHAND;  break;
        case RANGED_ATTACK: slot = EQUIPMENT_SLOT_RANGED;   break;
        default: return nullptr;
    }

    Item* item = GetUseableItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!item || item->GetTemplate()->Class != ITEM_CLASS_WEAPON)
        return nullptr;

    if (item->IsBroken())
        return nullptr;

    return item;
}

void Unit::ExitVehicleHandling(Vehicle* vehicle, Position const& pos, UnitVehicleExitParameters params)
{
    if (params.ExitSpline && IsInWorld() && !m_Events.HasEventType(EventType::EVENT_TYPE_VEHICLE_JOIN))
    {
        if (IsAlive())
        {
            std::function<void(Movement::MoveSplineInit&)> initializer = [=, this, vehicleCollisionHeight = vehicle->GetBase()->GetCollisionHeight()](Movement::MoveSplineInit& init)
            {
                float height = pos.GetPositionZ() + vehicleCollisionHeight;

                // Creatures without inhabit type air should begin falling after exiting the vehicle
                if (GetTypeId() == TYPEID_UNIT && !CanFly() && height > GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ() + vehicleCollisionHeight, &height))
                    init.SetFall();

                init.MoveTo(pos.GetPositionX(), pos.GetPositionY(), height, false);
                init.SetFacing(pos.GetOrientation());
                init.SetTransportExit();
            };
            GetMotionMaster()->LaunchMoveSpline(std::move(initializer), EVENT_VEHICLE_EXIT, MOTION_PRIORITY_HIGHEST, EFFECT_MOTION_TYPE, MOTION_MODE_OVERRIDE);
        }
    }

    if (Player* player = ToPlayer())
    {
        player->SetCanTeleport(true);
        if (params.ResummonPet && !m_Events.HasEventType(EventType::EVENT_TYPE_VEHICLE_JOIN))
            player->ResummonPetTemporaryUnSummonedIfAny();
    }

    bool despawn = false;
    if (params.Despawn)
    {
        if (vehicle->GetBase()->HasUnitTypeMask(UNIT_MASK_MINION) && vehicle->GetBase()->GetTypeId() == TYPEID_UNIT)
            if (((Minion*)vehicle->GetBase())->GetOwner() == this)
                vehicle->GetBase()->ToCreature()->DespawnOrUnsummon(vehicle->GetDespawnDelay());

        if (HasUnitTypeMask(UNIT_MASK_ACCESSORY))
        {
            // Vehicle just died, we die too
            if (vehicle->GetBase()->getDeathState() == JUST_DIED)
                setDeathState(JUST_DIED);
            // If for other reason we as minion are exiting the vehicle (ejected, master dismounted) - unsummon
            else
                ToTempSummon()->UnSummon(2000); // Approximation
            despawn = true;
        }
    }
    if (!despawn && params.Evade && GetTypeId() == TYPEID_UNIT)
    {
        Creature* toCreature = ToCreature();
        toCreature->SetSpawnHealth();
        toCreature->LoadCreaturesAddon();
        if (toCreature->IsVehicle())
            toCreature->GetVehicleKit()->Reset(true);
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

    Optional<ChaseRange> chaseRange = _combatChaseParameters.CombatDistance ? ChaseRange(_combatChaseParameters.CombatDistance) : Optional<ChaseRange>{ };
    Optional<ChaseAngle> chaseAngle = _combatChaseParameters.CombatAngle ? ChaseAngle(_combatChaseParameters.CombatAngle) : Optional<ChaseAngle>{ };
    me->GetMotionMaster()->MoveChase(me->GetVictim(), chaseRange, chaseAngle);
}
