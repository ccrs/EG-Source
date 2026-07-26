#include "CustomFunctions.h"
#include "InstanceSaveMgr.h"
#include "InstanceScript.h"
#include "CellImpl.h"
#include "Containers.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "DBCStores.h"
#include "GenericMovementGenerator.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "LFGMgr.h"
#include "LFGQueue.h"
#include "Map.h"
#include "MapManager.h"
#include "MotionMaster.h"
#include "MovementInfo.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "Log.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SmartAI.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
#include "Transport.h"
#include "Unit.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "World.h"
#include "WorldSession.h"
#include <unordered_set>


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

void Creature::LoadCreaturesAddonAuras()
{
    CreatureAddon const* creatureAddon = GetCreatureAddon();
    if (!creatureAddon || creatureAddon->auras.empty())
        return;

    for (std::vector<uint32>::const_iterator itr = creatureAddon->auras.begin(); itr != creatureAddon->auras.end(); ++itr)
    {
        SpellInfo const* AdditionalSpellInfo = sSpellMgr->GetSpellInfo(*itr);
        if (!AdditionalSpellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Creature {} has wrong spell {} defined in `auras` field.", GetGUID().ToString(), *itr);
            continue;
        }

        // skip already applied aura
        if (HasAura(*itr))
            continue;

        AddAura(*itr, this);
        TC_LOG_DEBUG("entities.unit", "Spell: {} added to creature {}", *itr, GetGUID().ToString());
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
        Position safePos = pos;
        bool const posValid = !(std::fabs(pos.GetPositionX()) < 0.01f && std::fabs(pos.GetPositionY()) < 0.01f) && pos.GetPositionZ() > INVALID_HEIGHT;
        if (!posValid)
        {
            TC_LOG_WARN("entities.vehicle", "Unit {} vehicle exit pos invalid ({:.2f}, {:.2f}, {:.2f}); falling back to passenger pos ({:.2f}, {:.2f}, {:.2f})",
                GetGUID().ToString(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), GetPositionX(), GetPositionY(), GetPositionZ());
            safePos = GetPosition();
        }

        float const vehicleCollisionHeight = vehicle->GetBase()->GetCollisionHeight();

        if (!IsAlive() && GetTypeId() == TYPEID_PLAYER)
        {
            float const startHeight = safePos.GetPositionZ() + vehicleCollisionHeight;
            float groundHeight = startHeight;
            float const waterOrGroundLevel = GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), safePos.GetPositionX(), safePos.GetPositionY(), startHeight, &groundHeight, false, DEFAULT_COLLISION_HEIGHT, 1000.0f);

            if (waterOrGroundLevel > INVALID_HEIGHT)
            {
                Position groundPos(safePos.GetPositionX(), safePos.GetPositionY(), groundHeight, safePos.GetOrientation());
                DisableSpline();
                GetMotionMaster()->InterruptOnTeleport();
                SendTeleportPacket(groundPos);
                UpdatePosition(groundPos, true);
                UpdateObjectVisibility();
            }
        }
        else
        {
            bool const serverDrivenFallEligible = !IsAlive() || (GetTypeId() == TYPEID_UNIT && !CanFly());

            std::function<void(Movement::MoveSplineInit&)> initializer = [safePos, vehicleCollisionHeight, serverDrivenFallEligible, this](Movement::MoveSplineInit& init)
            {
                float const startHeight = safePos.GetPositionZ() + vehicleCollisionHeight;
                float groundHeight = startHeight;
                float const waterOrGroundLevel = GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), safePos.GetPositionX(), safePos.GetPositionY(), startHeight, &groundHeight, false, DEFAULT_COLLISION_HEIGHT, 1000.0f);

                float const dropTolerance = 10.0f;
                bool const groundFound = waterOrGroundLevel > INVALID_HEIGHT;
                bool const safePlacement = groundFound && (safePos.GetPositionZ() - waterOrGroundLevel) <= dropTolerance;

                if (safePlacement)
                    init.MoveTo(safePos.GetPositionX(), safePos.GetPositionY(), groundHeight + 0.25f, false);
                else if (groundFound && serverDrivenFallEligible)
                {
                    init.SetFall();
                    init.MoveTo(safePos.GetPositionX(), safePos.GetPositionY(), groundHeight, false);
                }
                else
                    init.MoveTo(safePos.GetPositionX(), safePos.GetPositionY(), safePos.GetPositionZ(), false);

                init.SetFacing(safePos.GetOrientation());
                init.SetTransportExit();
            };

            GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, EVENT_VEHICLE_EXIT);
            movement->Priority = MOTION_PRIORITY_HIGHEST;
            movement->Mode = MOTION_MODE_OVERRIDE;
            if (serverDrivenFallEligible)
                movement->AddFlag(MOVEMENTGENERATOR_FLAG_PERSIST_ON_DEATH);
            GetMotionMaster()->Add(movement);
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

    if (IsAlive() && !despawn && params.Evade && GetTypeId() == TYPEID_UNIT)
    {
        Creature* toCreature = ToCreature();
        toCreature->SetSpawnHealth();
        toCreature->LoadCreaturesAddon();
        if (toCreature->IsVehicle())
            toCreature->GetVehicleKit()->Reset(true);
    }
}

void Unit::InterruptSpellsCastedOnMe(bool killDelayed, bool interruptFriendlySpells)
{
    UnitList targets;
    Trinity::AnyUnitInObjectRangeCheck u_check(this, 100.0f);
    Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(this, targets, u_check);
    Cell::VisitAllObjects(this, searcher, GetMap()->GetVisibilityRange());

    for (const auto& target : targets)
    {
        if (target->GetVictim() == this)
        {
            if (Player* player = target->ToPlayer())
            {
                player->ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                player->InterruptSpell(CURRENT_MELEE_SPELL);
                player->SendMeleeAttackStop(this);
                player->SendAttackSwingCancelAttack();
            }
            else
            {
                target->InterruptSpell(CURRENT_MELEE_SPELL);
                target->SendMeleeAttackStop(this);
            }
        }

        if (!interruptFriendlySpells && IsFriendlyTo(target))
            continue;

        for (uint32 itr = CURRENT_FIRST_NON_MELEE_SPELL; itr < CURRENT_MAX_SPELL; ++itr)
            if (Spell* spell = target->GetCurrentSpell(CurrentSpellTypes(itr)))
                if (spell->m_targets.GetUnitTargetGUID() == GetGUID())
                    if (killDelayed || (spell->getState() == SPELL_STATE_PREPARING && spell->GetTimer()) || itr == CURRENT_CHANNELED_SPELL)
                        target->InterruptSpell(CurrentSpellTypes(itr), true);

        if (!killDelayed)
            continue;

        std::multimap<uint64, BasicEvent*> toIterate = target->m_Events.GetEvents();
        for (auto itr = toIterate.begin(); itr != toIterate.end(); ++itr)
            if (itr->second->Type == EventType::EVENT_TYPE_SPELL)
                if (SpellEvent* event = dynamic_cast<SpellEvent*>(itr->second))
                    if (event->GetSpell()->m_targets.GetUnitTargetGUID() == GetGUID())
                        if (event->GetSpell()->getState() != SPELL_STATE_FINISHED)
                            event->GetSpell()->cancel();
    }
}

/*static*/ float Vehicle::GetSeatOrientationOffset(VehicleSeatEntry const* seatInfo, VehicleSeatAddon const* seatAddon)
{
    if (seatAddon)
        return seatAddon->SeatOrientationOffset;

    if (seatInfo && std::isfinite(seatInfo->PassengerYaw))
        return seatInfo->PassengerYaw;

    return 0.0f;
}

float Vehicle::GetSeatOrientationOffsetForPassenger(Unit const* passenger) const
{
    return GetSeatOrientationOffset(GetSeatForPassenger(passenger), GetSeatAddonForSeatOfPassenger(passenger));
}

bool Vehicle::NormalizePassengerMovementInfo(Unit const* passenger, MovementInfo& movementInfo) const
{
    SeatMap::const_iterator seat = Seats.end();

    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
    {
        if (itr->second.Passenger.Guid == passenger->GetGUID())
        {
            seat = itr;
            break;
        }
    }

    if (seat == Seats.end())
        return false;

    VehicleSeatEntry const* seatInfo = seat->second.SeatInfo;
    VehicleSeatAddon const* seatAddon = seat->second.SeatAddon;

    if (!seatAddon && (seatInfo->Flags & VEHICLE_SEAT_FLAG_ALLOW_TURNING))
    {
        static std::unordered_set<uint32> warnedIds;
        if (warnedIds.insert(seatInfo->ID).second)
            TC_LOG_WARN("vehicles", "VehicleSeat ID {} has VEHICLE_SEAT_FLAG_ALLOW_TURNING but no VehicleSeatAddon DB row; orientation offset defaults to 0.0f", seatInfo->ID);
    }

    float localX = seatInfo->AttachmentOffset.X;
    float localY = seatInfo->AttachmentOffset.Y;
    float localZ = seatInfo->AttachmentOffset.Z;
    // EG - sniff-derived seat position override (see VehicleSeatAddon::AttachmentOffset)
    if (seatAddon && seatAddon->AttachmentOffset)
    {
        localX = seatAddon->AttachmentOffset->GetPositionX();
        localY = seatAddon->AttachmentOffset->GetPositionY();
        localZ = seatAddon->AttachmentOffset->GetPositionZ();
    }
    float localO = GetSeatOrientationOffset(seatInfo, seatAddon);

    // For turning seats, preserve only the passenger's local orientation.
    // Never preserve client-sent local x/y/z.
    if (seatInfo->Flags & VEHICLE_SEAT_FLAG_ALLOW_TURNING)
    {
        if (movementInfo.HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
            localO = movementInfo.transport.pos.GetOrientation();
        else
            localO = passenger->m_movementInfo.transport.pos.GetOrientation();
    }

    localO = Position::NormalizeOrientation(localO);

    float worldX = localX;
    float worldY = localY;
    float worldZ = localZ;
    float worldO = localO;

    CalculatePassengerPosition(worldX, worldY, worldZ, &worldO);

    if (!Trinity::IsValidMapCoord(worldX, worldY, worldZ, worldO))
        return false;

    movementInfo.AddMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
    movementInfo.transport.guid = GetBase()->GetGUID();
    movementInfo.transport.seat = seat->first;
    movementInfo.transport.time = 0;
    movementInfo.transport.pos.Relocate(localX, localY, localZ, localO);
    movementInfo.pos.Relocate(worldX, worldY, worldZ, worldO);

    return true;
}

Unit* WorldObject::DoFindLowestHPFriendlyInRange(FriendlySearchOptions options) const
{
    std::vector<Unit*> potentialFriendlies;
    EG::AnyFriendlyUnitInObjectRangeCheck checker(this, options);
    Trinity::UnitListSearcher<EG::AnyFriendlyUnitInObjectRangeCheck> searcher(this, potentialFriendlies, checker);
    Cell::VisitAllObjects(this, searcher, options.Range);
    if (potentialFriendlies.empty())
        return nullptr;

    Unit* unit = nullptr;
    float hp = 100.f;
    Trinity::Containers::RandomShuffle(potentialFriendlies);
    for (Unit* potential : potentialFriendlies)
    {
        if (potential->GetHealthPct() < hp)
        {
            unit = potential;
            hp = unit->GetHealthPct();
        }
    }
    if (!unit)
        unit = *potentialFriendlies.begin();

    return unit;
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
        && !unit->IsCritter()
        && ((_source->IsInCombat() && unit->IsInCombat()) || (!_source->IsInCombat()))
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _range)
        && _source->IsWithinLOSInMap(unit)
        && unit->GetHealthPct() < _hp
    )
    {
        _hp = unit->GetHealthPct();
        return true;
    }

    return false;
}

bool EG::AnyFriendlyUnitInObjectRangeCheck::operator()(Unit const* unit) const
{
    if (_options.Alive && !unit->IsAlive())
        return false;

    if (_options.IncludeSelf && _source == unit)
        return true;

    if (unit->IsCritter())
        return false;

    if (!_options.ExcludedEntries.empty() && _options.ExcludedEntries.contains(unit->GetEntry()))
        return false;

    if (((_options.PlayerOnly && unit->IsPlayer()) || !_options.PlayerOnly)
        && (!_source->IsUnit() || (_source->ToUnit()->IsInCombat() && unit->IsInCombat()) || (!_source->ToUnit()->IsInCombat()))
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _options.Range)
        && _source->IsWithinLOSInMap(unit)
    )
        return true;

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

bool WorldSession::NormalizeTransportMovementInfo(Unit* mover, MovementInfo& movementInfo)
{
    Player* plrMover = mover->ToPlayer();

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
    {
        Transport* transport = nullptr;
        Transport* currentTransport = plrMover ? plrMover->GetTransport() : nullptr;

        if (plrMover)
        {
            transport = plrMover->GetMap()->GetTransport(movementInfo.transport.guid);

            if (!transport && currentTransport && currentTransport->GetGUID() == movementInfo.transport.guid)
                transport = currentTransport;
        }
        else if (Transport* moverTransport = mover->GetTransport())
        {
            if (moverTransport->GetGUID() == movementInfo.transport.guid)
            {
                movementInfo.transport.guid = moverTransport->GetGUID();
                movementInfo.transport.pos  = mover->m_movementInfo.transport.pos;

                float wx = movementInfo.transport.pos.GetPositionX();
                float wy = movementInfo.transport.pos.GetPositionY();
                float wz = movementInfo.transport.pos.GetPositionZ();
                float wo = movementInfo.transport.pos.GetOrientation();
                moverTransport->CalculatePassengerPosition(wx, wy, wz, &wo);
                movementInfo.pos.Relocate(wx, wy, wz, wo);
                return true;
            }
        }

        float localX = movementInfo.transport.pos.GetPositionX();
        float localY = movementInfo.transport.pos.GetPositionY();
        float localZ = movementInfo.transport.pos.GetPositionZ();

        // Reject bogus transport offsets before changing transport membership.
        if (std::fabs(localX) > 75.0f || std::fabs(localY) > 75.0f || std::fabs(localZ) > 75.0f)
            return false;

        if (transport)
        {
            float worldX = movementInfo.transport.pos.GetPositionX();
            float worldY = movementInfo.transport.pos.GetPositionY();
            float worldZ = movementInfo.transport.pos.GetPositionZ();
            float worldO = movementInfo.transport.pos.GetOrientation();

            transport->CalculatePassengerPosition(worldX, worldY, worldZ, &worldO);

            // Reject invalid derived world coordinates before changing transport membership.
            if (!Trinity::IsValidMapCoord(worldX, worldY, worldZ, worldO))
                return false;

            if (plrMover)
            {
                if (!currentTransport)
                    transport->AddPassenger(plrMover);
                else if (currentTransport != transport)
                {
                    currentTransport->RemovePassenger(plrMover);
                    transport->AddPassenger(plrMover);
                }
            }

            movementInfo.transport.guid = transport->GetGUID();
            movementInfo.pos.Relocate(worldX, worldY, worldZ, worldO);
        }
        else
        {
            // No server-side Transport object for this GUID.
            // This can still be a non-map GAMEOBJECT_TYPE_TRANSPORT, so preserve that behavior.
            bool validGameObjectTransport = false;

            if (GameObject* go = mover->GetMap()->GetGameObject(movementInfo.transport.guid))
                validGameObjectTransport = go->GetGoType() == GAMEOBJECT_TYPE_TRANSPORT;

            if (plrMover && currentTransport && currentTransport->GetGUID() != movementInfo.transport.guid)
                currentTransport->RemovePassenger(plrMover);

            if (!validGameObjectTransport)
            {
                movementInfo.RemoveMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
                movementInfo.transport.Reset();
            }
        }
    }
    else if (plrMover && plrMover->GetTransport())
    {
        plrMover->GetTransport()->RemovePassenger(plrMover);
        movementInfo.transport.Reset();
    }

    return true;
}

void InstanceScript::ForceRespawnQueuedCreaturesByEntry(std::vector<uint32> const& entries)
{
    std::vector<RespawnInfo const*> respawnData;
    instance->GetRespawnInfo(respawnData, SPAWN_TYPEMASK_CREATURE);
    for (RespawnInfo const* info : respawnData)
    {
        bool matched = false;
        for (uint32 entry : entries)
            if (info->entry == entry) { matched = true; break; }
        if (!matched)
            continue;

        bool aliveExists = false;
        std::vector<Creature*> deadCopies;
        auto bounds = instance->GetCreatureBySpawnIdStore().equal_range(info->spawnId);
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
        {
            Creature* creature = itr->second;
            if (!creature)
                continue;
            if (creature->IsAlive())
                aliveExists = true;
            else
                deadCopies.push_back(creature);
        }

        for (Creature* creature : deadCopies)
            creature->RemoveCorpse(false, true);

        if (!aliveExists)
            instance->Respawn(info->type, info->spawnId);
    }
}

bool lfg::LFGQueue::CompatibleKeyContainsGuid(std::string const& key, ObjectGuid guid)
{
    std::string const wanted = std::to_string(guid.GetRawValue());
    std::string::size_type begin = 0;

    while (begin <= key.size())
    {
        std::string::size_type end = key.find('|', begin);
        std::string token = end == std::string::npos ? key.substr(begin) : key.substr(begin, end - begin);

        if (token == wanted)
            return true;

        if (end == std::string::npos)
            break;

        begin = end + 1;
    }

    return false;
}

uint8 lfg::LFGQueue::CountGuidsInCompatibleKey(std::string const& key)
{
    if (key.empty())
        return 0;

    return uint8(std::count(key.begin(), key.end(), '|') + 1);
}

lfg::LFGMgr::LfgBindCleanupResult lfg::LFGMgr::TryClearOldLfgGroupBind(Group* group, uint32 lfgDungeonId, uint32 oldInstanceId)
{
    if (!group || !group->isLFGGroup() || !lfgDungeonId)
        return LfgBindCleanupResult::NoBind;

    LFGDungeonData const* dungeon = GetLFGDungeon(lfgDungeonId);
    if (!dungeon)
        return LfgBindCleanupResult::NoBind;

    MapEntry const* mapEntry = sMapStore.LookupEntry(dungeon->map);
    if (!mapEntry || !mapEntry->IsDungeon())
        return LfgBindCleanupResult::NoBind;

    // LFG requeue must never become a way to bypass raid or protected instance locks.
    if (mapEntry->IsRaid())
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing old group bind for raid map {}", dungeon->map);
        return LfgBindCleanupResult::Protected;
    }

    Difficulty difficulty = Difficulty(dungeon->difficulty);
    GetDownscaledMapDifficultyData(dungeon->map, difficulty);

    // Match existing reset-all semantics: do not clear heroic/protected dungeon binds here.
    if (difficulty > DUNGEON_DIFFICULTY_NORMAL)
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing old group bind for heroic/protected dungeon map {} difficulty {}",
            dungeon->map, uint32(difficulty));
        return LfgBindCleanupResult::Protected;
    }

    InstanceGroupBind* bind = group->GetBoundInstance(difficulty, dungeon->map);
    if (!bind || !bind->save)
        return LfgBindCleanupResult::NoBind;

    // Only clear the exact copy the player left - a faster member may already have rebound the group to a fresh one.
    if (oldInstanceId && bind->save->GetInstanceId() != oldInstanceId)
        return LfgBindCleanupResult::NoBind;

    // Permanent binds are authoritative locks and must not be bypassed by LFG requeue.
    if (bind->perm)
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing permanent group bind group={} map={} difficulty={} instance={}",
            group->GetGUID().ToString(), dungeon->map, uint32(difficulty), bind->save->GetInstanceId());
        return LfgBindCleanupResult::Protected;
    }

    // Non-resettable saves are protected by the instance system.
    if (!bind->save->CanReset())
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing non-resettable group bind group={} map={} difficulty={} instance={}",
            group->GetGUID().ToString(), dungeon->map, uint32(difficulty), bind->save->GetInstanceId());
        return LfgBindCleanupResult::Protected;
    }

    uint32 const instanceId = bind->save->GetInstanceId();

    if (Map* boundMap = sMapMgr->FindMap(dungeon->map, instanceId))
        if (boundMap->IsDungeon())
            static_cast<InstanceMap*>(boundMap)->Reset(INSTANCE_RESET_RESPAWN_DELAY);

    group->UnbindInstance(dungeon->map, uint8(difficulty), false);

    TC_LOG_DEBUG("lfg.teleport", "LFG requeue: cleared resettable non-permanent group bind group={} map={} difficulty={} instance={}",
        group->GetGUID().ToString(), dungeon->map, uint32(difficulty), instanceId);

    return LfgBindCleanupResult::Cleared;
}

void lfg::LFGMgr::ProcessPendingTeleportIns(time_t currTime)
{
    for (std::map<ObjectGuid, PendingTeleportInData>::iterator itr = PendingTeleportInStore.begin(); itr != PendingTeleportInStore.end();)
    {
        ObjectGuid guid = itr->first;
        time_t expireTime = itr->second.ExpireTime;
        uint32 previousDungeonId = itr->second.PreviousDungeonId;
        uint32 oldInstanceId = itr->second.OldInstanceId;

        Player* player = ObjectAccessor::FindConnectedPlayer(guid);
        if (!player)
        {
            if (currTime >= expireTime)
                itr = PendingTeleportInStore.erase(itr);
            else
                ++itr;

            continue;
        }

        if (player->IsBeingTeleported())
        {
            if (currTime >= expireTime)
                itr = PendingTeleportInStore.erase(itr);
            else
                ++itr;

            continue;
        }

        Group* group = player->GetGroup();
        if (!group || !group->isLFGGroup())
        {
            itr = PendingTeleportInStore.erase(itr);
            continue;
        }

        LFGDungeonData const* dungeon = GetLFGDungeon(GetDungeon(group->GetGUID()));
        if (!dungeon)
        {
            itr = PendingTeleportInStore.erase(itr);
            continue;
        }

        if (player->GetMapId() == uint32(dungeon->map) && player->GetMap()->IsDungeon())
        {
            if (currTime >= expireTime)
            {
                TC_LOG_DEBUG("lfg.teleport", "Deferred LFG teleport for player {} expired while still inside map {}", player->GetName(), dungeon->map);
                itr = PendingTeleportInStore.erase(itr);
            }
            else
                ++itr;

            continue;
        }

        if (previousDungeonId)
            TryClearOldLfgGroupBind(group, previousDungeonId, oldInstanceId);

        itr = PendingTeleportInStore.erase(itr);
        TeleportPlayer(player, false, false, true, previousDungeonId);
    }
}
