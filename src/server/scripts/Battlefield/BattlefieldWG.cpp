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
#include "CreatureTextMgr.h"
#include "Map.h"
#include "MapUtils.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldStatePackets.h"
#include <unordered_map>
#include <vector>

static WorldLocation const WintergraspDefenderBattleLocation(MAPID_WINTERGRASP, 5333.0f, 2842.0f, 410.0f, 0.0f);
static WorldLocation const WintergraspAllianceAttackerBattleLocation(MAPID_WINTERGRASP, 5095.0f, 2180.0f, 366.0f, 0.0f);
static WorldLocation const WintergraspHordeAttackerBattleLocation(MAPID_WINTERGRASP, 5025.857f, 3674.629f, 362.737f, 0.0f);
static WorldLocation const WintergraspKickLocation(MAPID_WINTERGRASP, 5748.0f, 2698.0f, 697.4f, 0.0f);

static std::vector<BattlefieldBuildingInfo> const WintergraspBuildingInfo =
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
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_1,         GO_WINTERGRASP_FORTRESS_TOWER_1,         BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_2,         GO_WINTERGRASP_FORTRESS_TOWER_2,         BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_3,         GO_WINTERGRASP_FORTRESS_TOWER_3,         BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_4,         GO_WINTERGRASP_FORTRESS_TOWER_4,         BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { WORLDSTATE_WINTERGRASP_TOWER_1,                  GO_WINTERGRASP_TOWER_1,                  BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_TOWER_2,                  GO_WINTERGRASP_TOWER_2,                  BATTLEFIELD_BUILDING_TYPE_TOWER },
    { WORLDSTATE_WINTERGRASP_TOWER_3,                  GO_WINTERGRASP_TOWER_3,                  BATTLEFIELD_BUILDING_TYPE_TOWER }
};

static std::unordered_map<uint32 /*entry*/, uint8> const WintergraspTowerTexts =
{
    { GO_WINTERGRASP_TOWER_1, SAY_WINTERGRASP_WESTERN_TOWER_DAMAGED },
    { GO_WINTERGRASP_TOWER_2, SAY_WINTERGRASP_SOUTHERN_TOWER_DAMAGED },
    { GO_WINTERGRASP_TOWER_3, SAY_WINTERGRASP_EASTERN_TOWER_DAMAGED },
    { GO_WINTERGRASP_FORTRESS_TOWER_1, SAY_WINTERGRASP_NORTH_WESTERN_KEEP_TOWER_DAMAGED },
    { GO_WINTERGRASP_FORTRESS_TOWER_2, SAY_WINTERGRASP_SOUTH_WESTERN_KEEP_TOWER_DAMAGED },
    { GO_WINTERGRASP_FORTRESS_TOWER_3, SAY_WINTERGRASP_SOUTH_EASTERN_KEEP_TOWER_DAMAGED },
    { GO_WINTERGRASP_FORTRESS_TOWER_4, SAY_WINTERGRASP_NORTH_EASTERN_KEEP_TOWER_DAMAGED }
};

static std::vector<BattlefieldGraveyardInfo> const WintergraspGraveyardInfo =
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

    for (auto itr = WintergraspBuildingInfo.begin(); itr != WintergraspBuildingInfo.end(); ++itr)
    {
        WintergraspBuildingPointer building = std::make_unique<WintergraspBuilding>(this, *itr);
        building->InitializeState();
        EmplaceBuilding(std::move(building));
    }

    for (auto itr = WintergraspGraveyardInfo.begin(); itr != WintergraspGraveyardInfo.end(); ++itr)
    {
        WintergraspGraveyardPointer graveyard = std::make_unique<WintergraspGraveyard>(this, *itr);
        graveyard->InitializeState();
        EmplaceGraveyard(itr->Id, std::move(graveyard));
    }

    Values.Set<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_BATTLE, Minutes(sWorld->getIntConfig(CONFIG_WINTERGRASP_BATTLETIME)));
    Values.Set<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_BATTLE_START, Minutes(sWorld->getIntConfig(CONFIG_WINTERGRASP_NOBATTLETIME)));
    Values.Set(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION, WINTERGRASP_PREPARATION_DURATION);
    Values.Set(BATTLEFIELD_VALUE_LIMIT_TEAM_PLAYERS, sWorld->getIntConfig(CONFIG_WINTERGRASP_PLR_MAX));
    Values.Set(BATTLEFIELD_VALUE_LIMIT_PLAYER_LEVEL, sWorld->getIntConfig(CONFIG_WINTERGRASP_PLR_MIN_LVL));
    Values.Set<Seconds>(BATTLEFIELD_VALUE_DELAY_PHASE_BATTLE_RESTART, Minutes(sWorld->getIntConfig(CONFIG_WINTERGRASP_RESTART_AFTER_CRASH)));
    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START, time_t(sWorld->getWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE)));
    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END, time_t(sWorld->getWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_END)));
    InitializePhaseTimes();
    return true;
}

void BattlefieldWintergrasp::ChangeTeam(PvPTeamId newControllingTeam)
{
    Battlefield::ChangeTeam(newControllingTeam);

    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, GetControllingTeam() == PVP_TEAM_HORDE ? 1 : 0);
    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, GetControllingTeam() == PVP_TEAM_ALLIANCE ? 1 : 0);
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
        case BATTLEFIELD_PHASE_BATTLE:
            if (Map* battleMap = GetBattleMap())
                if (CreatureData const* stalkerData = sObjectMgr->GetCreatureData(SPAWNID_WINTERGRASP_INVISIBLE_STALKER))
                    battleMap->LoadGrid(stalkerData->spawnPoint.GetPositionX(), stalkerData->spawnPoint.GetPositionY());

            if (Creature* stalker = GetCreature(_stalkerGUID))
                stalker->setActive(true);

            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_CONTROL, WINTERGRASP_CONTROL_BATTLE);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, 0);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, 0);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_BATTLE_TIMER, 1);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_COOLDOWN, 0);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, 1);
            break;
        case BATTLEFIELD_PHASE_IDLE:
        {
            if (Creature* stalker = GetCreature(_stalkerGUID))
                stalker->setActive(false);

            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, 0);
            if (oldPhase != BATTLEFIELD_PHASE_BATTLE)
                break;

            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_COOLDOWN, 1);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_BATTLE_TIMER, 0);

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

    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE, uint64(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START)));
    sWorld->setWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint64(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END)));
    SendWorldStateToAll(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START)));
    SendWorldStateToMap(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END)));
}

bool BattlefieldWintergrasp::TeleportPlayerToBattle(Player* player)
{
    Battlefield::TeleportPlayerToBattle(player);

    if (player->GetTeamId() == GetControllingTeamId())
        return player->TeleportTo(WintergraspDefenderBattleLocation);

    if (player->GetTeamId() == TEAM_ALLIANCE)
        return player->TeleportTo(WintergraspAllianceAttackerBattleLocation);

    return player->TeleportTo(WintergraspHordeAttackerBattleLocation);
}

void BattlefieldWintergrasp::TeleportPlayerOutOfBattle(Player* player)
{
    Battlefield::TeleportPlayerOutOfBattle(player);

    player->TeleportTo(WintergraspKickLocation);
}

uint32 BattlefieldWintergrasp::GetFaction(PvPTeamId pvpTeamId) const
{
    switch (pvpTeamId)
    {
        case PVP_TEAM_HORDE:
            return FACTION_HORDE_GENERIC_WG;
        case PVP_TEAM_ALLIANCE:
            return FACTION_ALLIANCE_GENERIC_WG;
        default:
            return 0;
    }
}

void BattlefieldWintergrasp::OnCreatureCreate(Creature* object)
{
    switch (object->GetEntry())
    {
        case NPC_WINTERGRASP_INVISIBLE_STALKER:
            if (object->GetSpawnId() == SPAWNID_WINTERGRASP_INVISIBLE_STALKER)
                _stalkerGUID = object->GetGUID();
            break;
        case NPC_WINTERGRASP_COMMANDER_ZANNETH:
            if (object->GetSpawnId() == SPAWNID_WINTERGRASP_COMMANDER_ZANNETH_KEEP)
                _commanderGUIDs[PVP_TEAM_ALLIANCE] = object->GetGUID();
            break;
        case NPC_WINTERGRASP_COMMANDER_DARDOSH:
            if (object->GetSpawnId() == SPAWNID_WINTERGRASP_COMMANDER_DARDOSH_KEEP)
                _commanderGUIDs[PVP_TEAM_HORDE] = object->GetGUID();
            break;
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

void BattlefieldWintergrasp::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    Battlefield::FillInitialWorldStates(packet);

    bool const warTime = IsBattleTime();
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, !warTime && GetControllingTeam() == PVP_TEAM_HORDE ? 1 : 0); // 3802
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, !warTime && GetControllingTeam() == PVP_TEAM_ALLIANCE ? 1 : 0); // 3803

    if (IsEnabled())
    {
        packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_CONTROL, GetControlState()); // 3804
        packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_SHOW_BATTLE_TIMER, warTime ? 1 : 0); // 3710
        packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END))); // 3781
    }
}

void BattlefieldWintergrasp::SendGlobalWorldStates(Player const* player) const
{
    if (!IsEnabled())
        return;

    bool const warTime = IsBattleTime();
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_SHOW_BATTLE_TIMER, warTime ? 1 : 0); // 3710
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_SHOW_COOLDOWN, warTime ? 0 : 1); // 3801
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START))); // 4354
    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, IsInPreparation() || warTime ? 1 : 0); // 4375
    for (uint32 worldState : { WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_HORDE, WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_ALLIANCE, WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_HORDE, WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_ALLIANCE })
        player->SendUpdateWorldState(worldState, uint32(sWorld->getWorldState(worldState)));

    if (player->GetMapId() != MAPID_WINTERGRASP)
        return;

    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_END, uint32(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END))); // 3781
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

void BattlefieldWintergrasp::SendStalkerText(uint8 textGroup) const
{
    if (Creature* stalker = GetCreature(_stalkerGUID))
        sCreatureTextMgr->SendChat(stalker, textGroup);
}

void BattlefieldWintergrasp::HandleSouthernTowerDestroyed()
{
    uint32 towers = 0;
    uint32 destroyedTowers = 0;
    for (BattlefieldBuildingContainer::value_type const& pair : GetBuildings())
    {
        if (pair.second->BuildingType != BATTLEFIELD_BUILDING_TYPE_TOWER)
            continue;

        ++towers;
        if (pair.second->GetDestructibleState() == GO_DESTRUCTIBLE_DESTROYED)
            ++destroyedTowers;
    }

    uint8 textGroup = SAY_WINTERGRASP_COMMANDER_SECOND_TOWER_FALLEN;
    if (destroyedTowers == towers)
        textGroup = SAY_WINTERGRASP_COMMANDER_ALL_TOWERS_FALLEN;
    else if (destroyedTowers == 1)
        textGroup = SAY_WINTERGRASP_COMMANDER_FIRST_TOWER_FALLEN;

    if (Creature* commander = GetCreature(_commanderGUIDs[GetControllingTeam()]))
        sCreatureTextMgr->SendChat(commander, textGroup);

    if (destroyedTowers == towers)
        SetPhaseTime(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END) - WINTERGRASP_SOUTHERN_TOWERS_TIME_REDUCTION.count());
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
            return !IsBattleTime();
        case SPELL_WINTERGRASP_ESSENCE_OF_WINTERGRASP_NORTHREND:
            return false;
        case SPELL_WINTERGRASP_BATTLEGROUND_DAMPENING:
            return IsEnabled() && IsBattleTime();
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
    if (IsBattleTime())
        return WINTERGRASP_CONTROL_BATTLE;

    return GetControllingTeam() == PVP_TEAM_HORDE ? WINTERGRASP_CONTROL_HORDE : WINTERGRASP_CONTROL_ALLIANCE;
}

WintergraspBuilding::WintergraspBuilding(BattlefieldWintergrasp* wintergrasp, BattlefieldBuildingInfo const info) : BattlefieldBuilding(wintergrasp, info), _wintergrasp(wintergrasp)
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

bool WintergraspBuilding::Damage()
{
    if (!BattlefieldBuilding::Damage())
        return false;

    if (_wintergrasp->IsBattleTime())
        SendText(false);

    return true;
}

bool WintergraspBuilding::Destroy()
{
    if (!BattlefieldBuilding::Destroy())
        return false;

    if (!_wintergrasp->IsBattleTime())
        return true;

    SendText(true);
    if (BuildingType == BATTLEFIELD_BUILDING_TYPE_TOWER)
        _wintergrasp->HandleSouthernTowerDestroyed();

    return true;
}

void WintergraspBuilding::SendText(bool destroyed) const
{
    if (uint8 const* textGroup = Trinity::Containers::MapGetValuePtr(WintergraspTowerTexts, Info.GetNeutralObjectEntry()))
        _wintergrasp->SendStalkerText(uint8(*textGroup + (destroyed ? 1 : 0)));
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
