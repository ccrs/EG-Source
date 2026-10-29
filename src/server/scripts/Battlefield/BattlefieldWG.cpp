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

#include "BattlefieldWG.h"
#include "Creature.h"
#include "GameObject.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldStatePackets.h"
#include <vector>

static std::vector<BattlefieldBuildingInfo> const wintergraspBuildingInfo =
{
    { WORLDSTATE_WINTERGRASP_FORTRESS_GATE,            GO_WINTERGRASP_FORTRESS_GATE,            BATTLEFIELD_BUILDING_TYPE_DOOR },
    { WORLDSTATE_WINTERGRASP_FORTRESS_VAULT_GATE,      GO_WINTERGRASP_FORTRESS_VAULT_GATE,      BATTLEFIELD_BUILDING_TYPE_DOOR_LAST },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_1,          GO_WINTERGRASP_FORTRESS_WALL_1,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_2,          GO_WINTERGRASP_FORTRESS_WALL_2,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_3,          GO_WINTERGRASP_FORTRESS_WALL_3,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_4,          GO_WINTERGRASP_FORTRESS_WALL_4,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_5,          GO_WINTERGRASP_FORTRESS_WALL_5,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_6,          GO_WINTERGRASP_FORTRESS_WALL_6,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_7,          GO_WINTERGRASP_FORTRESS_WALL_7,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_8,          GO_WINTERGRASP_FORTRESS_WALL_8,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_9,          GO_WINTERGRASP_FORTRESS_WALL_9,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_10,         GO_WINTERGRASP_FORTRESS_WALL_10,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_11,         GO_WINTERGRASP_FORTRESS_WALL_11,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_12,         GO_WINTERGRASP_FORTRESS_WALL_12,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_13,         GO_WINTERGRASP_FORTRESS_WALL_13,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_14,         GO_WINTERGRASP_FORTRESS_WALL_14,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_15,         GO_WINTERGRASP_FORTRESS_WALL_15,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_16,         GO_WINTERGRASP_FORTRESS_WALL_16,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_17,         GO_WINTERGRASP_FORTRESS_WALL_17,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_18,         GO_WINTERGRASP_FORTRESS_WALL_18,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_19,         GO_WINTERGRASP_FORTRESS_WALL_19,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_20,         GO_WINTERGRASP_FORTRESS_WALL_20,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_1, GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1, BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_2, GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2, BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_3, GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3, BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_1,         GO_WINTERGRASP_FORTRESS_TOWER_1,         BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_2,         GO_WINTERGRASP_FORTRESS_TOWER_2,         BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_3,         GO_WINTERGRASP_FORTRESS_TOWER_3,         BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_4,         GO_WINTERGRASP_FORTRESS_TOWER_4,         BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_TOWER_1,                  GO_WINTERGRASP_TOWER_1,                  BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_TOWER_2,                  GO_WINTERGRASP_TOWER_2,                  BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_TOWER_3,                  GO_WINTERGRASP_TOWER_3,                  BATTLEFIELD_BUILDING_TYPE_TOWER }
};

static std::vector<BattlefieldGraveyardInfo> const wintergraspGraveyardInfo =
{
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_NE, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NE, GOSSIP_WINTERGRASP_GRAVEYARD_NE, SPELL_WINTERGRASP_TELEPORT_SUNKEN_RING },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_NW, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NW, GOSSIP_WINTERGRASP_GRAVEYARD_NW, SPELL_WINTERGRASP_TELEPORT_BROKEN_TEMPLE },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_SE, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SE,         GOSSIP_WINTERGRASP_GRAVEYARD_SE, SPELL_WINTERGRASP_TELEPORT_EASTSPARK_FACTORY_GRAVEYARD },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_SW, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SW,         GOSSIP_WINTERGRASP_GRAVEYARD_SW, SPELL_WINTERGRASP_TELEPORT_WESTSPARK_FACTORY_GRAVEYARD },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_KEEP,        WORLDSAFELOC_WINTERGRASP_FORTRESS_EAST,            GOSSIP_WINTERGRASP_GRAVEYARD_KEEP, SPELL_WINTERGRASP_TELEPORT_FORTRESS_GRAVEYARD },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_HORDE,       WORLDSAFELOC_WINTERGRASP_HORDE_STARTING_AREA,      GOSSIP_WINTERGRASP_GRAVEYARD_HORDE, SPELL_WINTERGRASP_TELEPORT_HORDE_LANDING_ZONE },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_ALLIANCE,    WORLDSAFELOC_WINTERGRASP_ALLIANCE_STARTING_AREA,   GOSSIP_WINTERGRASP_GRAVEYARD_ALLIANCE, SPELL_WINTERGRASP_TELEPORT_ALLIANCE_LANDING_ZONE }
};

BattlefieldWintergrasp::BattlefieldWintergrasp() : Battlefield(BATTLEFIELD_BATTLEID_WINTERGRASP, BATTLEFIELD_ZONEID_WINTERGRASP)
{
}

BattlefieldWintergrasp::~BattlefieldWintergrasp()
{
}

bool BattlefieldWintergrasp::SetupBattlefield()
{
    if (!Battlefield::SetupBattlefield())
        return false;

    SetMapId(MAPID_WINTERGRASP);

    uint64 hordeDefender = sWorld->getWorldState(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER);
    Values.Set(BATTLEFIELD_VALUE_STATE_CONTROLLING_TEAM, hordeDefender != 0 ? PVP_TEAM_HORDE : PVP_TEAM_ALLIANCE);

    for (auto itr = wintergraspBuildingInfo.begin(); itr != wintergraspBuildingInfo.end(); ++itr)
    {
        WintergraspBuildingPointer building = std::make_unique<WintergraspBuilding>(this, *itr);
        building->InitializeState();
        _buildings.emplace(itr->Info.GetNeutralObjectEntry(), std::move(building));
    }

    for (auto itr = wintergraspGraveyardInfo.begin(); itr != wintergraspGraveyardInfo.end(); ++itr)
    {
        WintergraspGraveyardPointer graveyard = std::make_unique<WintergraspGraveyard>(this, *itr);
        graveyard->InitializeState();
        EmplaceGraveyard(itr->Id, std::move(graveyard));
    }

    Values.Set<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_WAR, Minutes(sWorld->getIntConfig(CONFIG_WINTERGRASP_BATTLETIME)));
    Values.Set<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_WAR_START, Minutes(sWorld->getIntConfig(CONFIG_WINTERGRASP_NOBATTLETIME)));
    Values.Set(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION, WINTERGRASP_PREPARATION_DURATION);
    Values.Set<Seconds>(BATTLEFIELD_VALUE_DELAY_PHASE_WAR_RESTART, Minutes(sWorld->getIntConfig(CONFIG_WINTERGRASP_RESTART_AFTER_CRASH)));
    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START, time_t(sWorld->getWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE)));
    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END, time_t(sWorld->getWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_END)));
    InitializePhaseTimes();
    return true;
}

void BattlefieldWintergrasp::ChangeTeams(PvPTeamId newControllingTeam)
{
    Battlefield::ChangeTeams(newControllingTeam);

    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, GetControllingTeam() == PVP_TEAM_HORDE ? 1 : 0);
    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, GetControllingTeam() == PVP_TEAM_ALLIANCE ? 1 : 0);

    for (WintergraspBuildingContainer::value_type& buildingPair : _buildings)
        buildingPair.second->InitializeState();

    QueueEvent<BattlefieldZoneRefreshEvent>();
}

bool BattlefieldWintergrasp::SetPhase(BattlefieldPhase phase)
{
    BattlefieldPhase const oldPhase = GetPhase();
    if (!Battlefield::SetPhase(phase))
        return false;

    if (phase == oldPhase)
        return true;

    switch (phase)
    {
        case BATTLEFIELD_PHASE_PREPARATION:
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, 1);
            break;
        case BATTLEFIELD_PHASE_WAR:
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_CONTROL, WINTERGRASP_CONTROL_WAR);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, 0);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, 0);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_WAR_TIMER, 1);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_COOLDOWN, 0);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, 1);
            break;
        case BATTLEFIELD_PHASE_IDLE:
        {
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, 0);
            if (oldPhase != BATTLEFIELD_PHASE_WAR)
                break;

            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_COOLDOWN, 1);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_WAR_TIMER, 0);

            BattlefieldBattleResult const result = Values.Get<BattlefieldBattleResult>(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT);
            if (result != BATTLEFIELD_BATTLE_RESULT_NONE)
            {
                bool const captured = result == BATTLEFIELD_BATTLE_RESULT_CAPTURED;
                uint32 counterWorldState = 0;
                if (GetControllingTeam() == PVP_TEAM_HORDE)
                    counterWorldState = captured ? WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_HORDE : WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_HORDE;
                else
                    counterWorldState = captured ? WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_ALLIANCE : WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_ALLIANCE;

                uint32 const count = uint32(sWorld->getWorldState(counterWorldState)) + 1;
                sWorld->setWorldState(counterWorldState, count);
                SendWorldStateToAll(counterWorldState, count);
            }

            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, GetControllingTeam() == PVP_TEAM_HORDE ? 1 : 0);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, GetControllingTeam() == PVP_TEAM_ALLIANCE ? 1 : 0);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_CONTROL, GetControlState());
            break;
        }
        default:
            break;
    }
    return true;
}

void BattlefieldWintergrasp::SendPhaseTimes()
{
    Battlefield::SendPhaseTimes();

    if (!IsEnabled())
        return;

    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE, uint64(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START)));
    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint64(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END)));
    SendWorldStateToAll(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START)));
    SendWorldStateToMap(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END)));
}

void BattlefieldWintergrasp::OnCreatureCreate(Creature* object)
{
    switch (object->GetEntry())
    {
        case NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE:
        case NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE:
            if (uint8 graveyardId = GetWintergraspGraveyardId(object))
                if (BattlefieldGraveyard* graveyard = GetGraveyard(graveyardId))
                    graveyard->OnObjectCreate(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnCreatureRemove(Creature* object)
{
    switch (object->GetEntry())
    {
        case NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE:
        case NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE:
            if (uint8 graveyardId = GetWintergraspGraveyardId(object))
                if (BattlefieldGraveyard* graveyard = GetGraveyard(graveyardId))
                    graveyard->OnObjectRemove(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnGameObjectCreate(GameObject* object)
{
    switch (object->GetEntry())
    {
        case GO_WINTERGRASP_FORTRESS_GATE:
        case GO_WINTERGRASP_FORTRESS_VAULT_GATE:
        case GO_WINTERGRASP_FORTRESS_WALL_1:
        case GO_WINTERGRASP_FORTRESS_WALL_2:
        case GO_WINTERGRASP_FORTRESS_WALL_3:
        case GO_WINTERGRASP_FORTRESS_WALL_4:
        case GO_WINTERGRASP_FORTRESS_WALL_5:
        case GO_WINTERGRASP_FORTRESS_WALL_6:
        case GO_WINTERGRASP_FORTRESS_WALL_7:
        case GO_WINTERGRASP_FORTRESS_WALL_8:
        case GO_WINTERGRASP_FORTRESS_WALL_9:
        case GO_WINTERGRASP_FORTRESS_WALL_10:
        case GO_WINTERGRASP_FORTRESS_WALL_11:
        case GO_WINTERGRASP_FORTRESS_WALL_12:
        case GO_WINTERGRASP_FORTRESS_WALL_13:
        case GO_WINTERGRASP_FORTRESS_WALL_14:
        case GO_WINTERGRASP_FORTRESS_WALL_15:
        case GO_WINTERGRASP_FORTRESS_WALL_16:
        case GO_WINTERGRASP_FORTRESS_WALL_17:
        case GO_WINTERGRASP_FORTRESS_WALL_18:
        case GO_WINTERGRASP_FORTRESS_WALL_19:
        case GO_WINTERGRASP_FORTRESS_WALL_20:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3:
        case GO_WINTERGRASP_FORTRESS_TOWER_1:
        case GO_WINTERGRASP_FORTRESS_TOWER_2:
        case GO_WINTERGRASP_FORTRESS_TOWER_3:
        case GO_WINTERGRASP_FORTRESS_TOWER_4:
        case GO_WINTERGRASP_TOWER_1:
        case GO_WINTERGRASP_TOWER_2:
        case GO_WINTERGRASP_TOWER_3:
            if (auto it = _buildings.find(object->GetEntry()); it != _buildings.end() && it->second)
                it->second->OnObjectCreate(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnGameObjectRemove(GameObject* object)
{
    switch (object->GetEntry())
    {
        case GO_WINTERGRASP_FORTRESS_GATE:
        case GO_WINTERGRASP_FORTRESS_VAULT_GATE:
        case GO_WINTERGRASP_FORTRESS_WALL_1:
        case GO_WINTERGRASP_FORTRESS_WALL_2:
        case GO_WINTERGRASP_FORTRESS_WALL_3:
        case GO_WINTERGRASP_FORTRESS_WALL_4:
        case GO_WINTERGRASP_FORTRESS_WALL_5:
        case GO_WINTERGRASP_FORTRESS_WALL_6:
        case GO_WINTERGRASP_FORTRESS_WALL_7:
        case GO_WINTERGRASP_FORTRESS_WALL_8:
        case GO_WINTERGRASP_FORTRESS_WALL_9:
        case GO_WINTERGRASP_FORTRESS_WALL_10:
        case GO_WINTERGRASP_FORTRESS_WALL_11:
        case GO_WINTERGRASP_FORTRESS_WALL_12:
        case GO_WINTERGRASP_FORTRESS_WALL_13:
        case GO_WINTERGRASP_FORTRESS_WALL_14:
        case GO_WINTERGRASP_FORTRESS_WALL_15:
        case GO_WINTERGRASP_FORTRESS_WALL_16:
        case GO_WINTERGRASP_FORTRESS_WALL_17:
        case GO_WINTERGRASP_FORTRESS_WALL_18:
        case GO_WINTERGRASP_FORTRESS_WALL_19:
        case GO_WINTERGRASP_FORTRESS_WALL_20:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3:
        case GO_WINTERGRASP_FORTRESS_TOWER_1:
        case GO_WINTERGRASP_FORTRESS_TOWER_2:
        case GO_WINTERGRASP_FORTRESS_TOWER_3:
        case GO_WINTERGRASP_FORTRESS_TOWER_4:
        case GO_WINTERGRASP_TOWER_1:
        case GO_WINTERGRASP_TOWER_2:
        case GO_WINTERGRASP_TOWER_3:
            if (auto it = _buildings.find(object->GetEntry()); it != _buildings.end() && it->second)
                it->second->OnObjectRemove(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    bool const warTime = IsWarTime();
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, !warTime && GetControllingTeam() == PVP_TEAM_HORDE ? 1 : 0); // 3802
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, !warTime && GetControllingTeam() == PVP_TEAM_ALLIANCE ? 1 : 0); // 3803

    if (IsEnabled())
    {
        packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_CONTROL, GetControlState()); // 3804
        packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_SHOW_WAR_TIMER, warTime ? 1 : 0); // 3710
        packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END))); // 3781
    }

    for (WintergraspBuildingContainer::value_type const& building : _buildings)
        building.second->FillInitialWorldStates(packet);
}

void BattlefieldWintergrasp::SendGlobalWorldStates(Player const* player) const
{
    if (!IsEnabled())
        return;

    bool const warTime = IsWarTime();
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_SHOW_WAR_TIMER, warTime ? 1 : 0); // 3710
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_SHOW_COOLDOWN, warTime ? 0 : 1); // 3801
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START))); // 4354
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, IsInPreparation() || warTime ? 1 : 0); // 4375
    for (uint32 worldState : { WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_HORDE, WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_ALLIANCE, WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_HORDE, WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_ALLIANCE })
        player->SendUpdateWorldState(worldState, uint32(sWorld->getWorldState(worldState)));

    if (player->GetMapId() != MAPID_WINTERGRASP)
        return;

    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END))); // 3781
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, !warTime && GetControllingTeam() == PVP_TEAM_HORDE ? 1 : 0); // 3802
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, !warTime && GetControllingTeam() == PVP_TEAM_ALLIANCE ? 1 : 0); // 3803
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_CONTROL, GetControlState()); // 3804
}

uint8 BattlefieldWintergrasp::GetWintergraspGraveyardId(Creature* creature) const
{
    switch (creature->GetSpawnId())
    {
        case 88315: // Horde base spirit
            return GRAVEYARD_HORDE;
        case 88321: // Alliance base spirit
            return GRAVEYARD_ALLIANCE;
        case 88319: // Broken Temple graveyard areaId != Broken Temple workshop areaId
        case 88313:
            return GRAVEYARD_WORKSHOP_NW;
        default:
            switch (creature->GetAreaId())
            {
                case AREA_WINTERGRASP_FORTRESS:
                    return GRAVEYARD_KEEP;
                case AREA_THE_SUNKEN_RING:
                    return GRAVEYARD_WORKSHOP_NE;
                case AREA_THE_BROKEN_TEMPLE:
                    return GRAVEYARD_WORKSHOP_NW;
                case AREA_WESTPARK_WORKSHOP:
                    return GRAVEYARD_WORKSHOP_SW;
                case AREA_EASTPARK_WORKSHOP:
                    return GRAVEYARD_WORKSHOP_SE;
                default:
                    break;
            }
            break;
    }

    return 0;
}

bool BattlefieldWintergrasp::IsSpellAreaAllowed(uint32 spellId, Player const* player, uint32 /*newArea*/) const
{
    if (!player)
        return false;

    switch (spellId)
    {
        case SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA:
            if (IsFlyingMountAllowed() || (!player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) && !player->HasAuraType(SPELL_AURA_FLY)))
                return false;
            break;
        case SPELL_WINTERGRASP_ESSENCE_OF_WINTERGRASP:
            return !IsWarTime();
        case SPELL_WINTERGRASP_ESSENCE_OF_WINTERGRASP_NORTHREND:
            return false;
        case SPELL_WINTERGRASP_BATTLEGROUND_DAMPENING:
            return IsEnabled() && IsWarTime();
        case SPELL_WINTERGRASP_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT:
            return false;
        case SPELL_WINTERGRASP_HORDE_CONTROLS_FACTORY_PHASE_SHIFT:
            return false;
        case SPELL_WINTERGRASP_ALLIANCE_CONTROL_PHASE_SHIFT:
            return GetControllingTeamId() == TEAM_ALLIANCE;
        case SPELL_WINTERGRASP_HORDE_CONTROL_PHASE_SHIFT:
            return GetControllingTeamId() == TEAM_HORDE;
        default:
            break;
    }

    return true;
}

uint32 BattlefieldWintergrasp::GetControlState() const
{
    if (IsWarTime())
        return WINTERGRASP_CONTROL_WAR;

    return GetControllingTeam() == PVP_TEAM_HORDE ? WINTERGRASP_CONTROL_HORDE : WINTERGRASP_CONTROL_ALLIANCE;
}

WintergraspBuilding::WintergraspBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info) : BattlefieldBuilding(battlefield, info)
{
}

void WintergraspBuilding::OnObjectCreate(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldBuilding::OnObjectCreate(object);
}

void WintergraspBuilding::OnObjectRemove(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldBuilding::OnObjectRemove(object);
}

void WintergraspBuilding::InitializeState()
{
    switch (Info.WorldState)
    {
        case WORLDSTATE_WINTERGRASP_TOWER_1:
        case WORLDSTATE_WINTERGRASP_TOWER_2:
        case WORLDSTATE_WINTERGRASP_TOWER_3:
            State = Battle->GetControllingTeam() == PVP_TEAM_HORDE ? BATTLEFIELD_BUILDING_STATE_ALLIANCE_INTACT : BATTLEFIELD_BUILDING_STATE_HORDE_INTACT;
            break;
        default:
            State = Battle->GetControllingTeam() == PVP_TEAM_HORDE ? BATTLEFIELD_BUILDING_STATE_HORDE_INTACT : BATTLEFIELD_BUILDING_STATE_ALLIANCE_INTACT;
            break;
    }
    SaveWorldState();
}

WintergraspCapturePoint::WintergraspCapturePoint(Battlefield* battlefield, BattlefieldEntityInfo const info) : BattlefieldCapturePoint(battlefield, info)
{
}

void WintergraspCapturePoint::OnObjectCreate(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldCapturePoint::OnObjectCreate(object);
}

void WintergraspCapturePoint::OnObjectRemove(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldCapturePoint::OnObjectRemove(object);
}

void WintergraspCapturePoint::InitializeState()
{
}

WintergraspGraveyard::WintergraspGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info) : BattlefieldGraveyard(battlefield, info)
{
    if (Id == GRAVEYARD_WORKSHOP_NW)
        SpellAreaForced = true;
}

void WintergraspGraveyard::OnObjectCreate(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_UNIT)
        return;

    BattlefieldGraveyard::OnObjectCreate(object);
}

void WintergraspGraveyard::OnObjectRemove(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_UNIT)
        return;

    BattlefieldGraveyard::OnObjectRemove(object);
}

void WintergraspGraveyard::InitializeState()
{
    switch (Id)
    {
        case GRAVEYARD_WORKSHOP_NE:
        case GRAVEYARD_WORKSHOP_NW:
        case GRAVEYARD_WORKSHOP_SE:
        case GRAVEYARD_WORKSHOP_SW:
            State = BATTLEFIELD_GRAVEYARD_STATE_NEUTRAL;
            break;
        case GRAVEYARD_KEEP:
            State = Battle->GetControllingTeam() == PVP_TEAM_HORDE ? BATTLEFIELD_GRAVEYARD_STATE_HORDE : BATTLEFIELD_GRAVEYARD_STATE_ALLIANCE;
            break;
        case GRAVEYARD_HORDE:
            State = BATTLEFIELD_GRAVEYARD_STATE_HORDE;
            break;
        case GRAVEYARD_ALLIANCE:
            State = BATTLEFIELD_GRAVEYARD_STATE_ALLIANCE;
            break;
        default:
            break;
    }
}

class BattlefieldWintergraspScript : public BattlefieldScript
{
public:
    BattlefieldWintergraspScript() : BattlefieldScript("battlefield_wg") {}

    Battlefield* GetBattlefield() const override
    {
        return new BattlefieldWintergrasp();
    }
};

void AddSCBattlefieldWintergraspScript() {
    new BattlefieldWintergraspScript();
}
