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
#include "AreaBoundary.h"
#include "Creature.h"
#include "CreatureTextMgr.h"
#include "GameObject.h"
#include "Map.h"
#include "MapUtils.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "TemporarySummon.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldStatePackets.h"
#include <algorithm>
#include <array>
#include <unordered_map>
#include <vector>

static WorldLocation const WintergraspDefenderBattleLocation(MAPID_WINTERGRASP, 5333.0f, 2842.0f, 410.0f, 0.0f);
static WorldLocation const WintergraspAllianceAttackerBattleLocation(MAPID_WINTERGRASP, 5095.0f, 2180.0f, 366.0f, 0.0f);
static WorldLocation const WintergraspHordeAttackerBattleLocation(MAPID_WINTERGRASP, 5025.857f, 3674.629f, 362.737f, 0.0f);
static WorldLocation const WintergraspKickLocation(MAPID_WINTERGRASP, 5748.0f, 2698.0f, 697.4f, 0.0f);
static RectangleBoundary const WintergraspTitanRelicChamberBoundary(5400.0f, 5490.0f, 2803.0f, 2878.0f);
static ZRangeBoundary const WintergraspTitanRelicChamberHeightBoundary(415.0f, 435.0f);

static std::vector<BattlefieldObjectInfo> const WintergraspZoneObjectInfos =
{
    { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_FORTRESS_TELEPORTERS },
    { PVP_TEAM_HORDE, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_OPPOSITE_TEAM, SPAWNGROUP_WINTERGRASP_TITAN_RELIC_HORDE },
    { PVP_TEAM_ALLIANCE, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_OPPOSITE_TEAM, SPAWNGROUP_WINTERGRASP_TITAN_RELIC_ALLIANCE },
    { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_FORTRESS_CANNONS }
};

static std::vector<BattlefieldBuildingInfo> const WintergraspBuildingInfo =
{
    { BUILDING_FORTRESS_GATE, WORLDSTATE_WINTERGRASP_FORTRESS_GATE, BATTLEFIELD_BUILDING_TYPE_DOOR },
    { BUILDING_FORTRESS_VAULT_GATE, WORLDSTATE_WINTERGRASP_FORTRESS_VAULT_GATE, BATTLEFIELD_BUILDING_TYPE_DOOR_LAST },
    { BUILDING_FORTRESS_WALL_1, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_1, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_2, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_2, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_3, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_3, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_4, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_4, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_5, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_5, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_6, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_6, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_7, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_7, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_8, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_8, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_9, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_9, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_10, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_10, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_11, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_11, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_12, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_12, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_13, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_13, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_14, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_14, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_15, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_15, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_16, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_16, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_17, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_17, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_18, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_18, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_19, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_19, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_WALL_20, WORLDSTATE_WINTERGRASP_FORTRESS_WALL_20, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_INTERIOR_WALL_1, WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_1, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_INTERIOR_WALL_2, WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_2, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_INTERIOR_WALL_3, WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_3, BATTLEFIELD_BUILDING_TYPE_WALL },
    { BUILDING_FORTRESS_TOWER_1, WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_1, BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { BUILDING_FORTRESS_TOWER_2, WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_2, BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { BUILDING_FORTRESS_TOWER_3, WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_3, BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { BUILDING_FORTRESS_TOWER_4, WORLDSTATE_WINTERGRASP_FORTRESS_TOWER_4, BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER },
    { BUILDING_TOWER_1, WORLDSTATE_WINTERGRASP_TOWER_1, BATTLEFIELD_BUILDING_TYPE_TOWER },
    { BUILDING_TOWER_2, WORLDSTATE_WINTERGRASP_TOWER_2, BATTLEFIELD_BUILDING_TYPE_TOWER },
    { BUILDING_TOWER_3, WORLDSTATE_WINTERGRASP_TOWER_3, BATTLEFIELD_BUILDING_TYPE_TOWER }
};

static std::unordered_map<uint8 /*buildingId*/, std::vector<BattlefieldObjectInfo>> const WintergraspBuildingObjectInfos =
{
    { BUILDING_FORTRESS_GATE, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_GATE } }, { PVP_TEAM_HORDE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_FORTRESS_GATE_GUARDS_HORDE }, { PVP_TEAM_ALLIANCE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_FORTRESS_GATE_GUARDS_ALLIANCE } } },
    { BUILDING_FORTRESS_VAULT_GATE, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_VAULT_GATE } } } },
    { BUILDING_FORTRESS_WALL_1, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_1 } } } },
    { BUILDING_FORTRESS_WALL_2, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_2 } } } },
    { BUILDING_FORTRESS_WALL_3, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_3 } } } },
    { BUILDING_FORTRESS_WALL_4, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_4 } } } },
    { BUILDING_FORTRESS_WALL_5, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_5 } } } },
    { BUILDING_FORTRESS_WALL_6, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_6 } } } },
    { BUILDING_FORTRESS_WALL_7, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_7 } } } },
    { BUILDING_FORTRESS_WALL_8, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_8 } } } },
    { BUILDING_FORTRESS_WALL_9, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_9 } } } },
    { BUILDING_FORTRESS_WALL_10, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_10 } } } },
    { BUILDING_FORTRESS_WALL_11, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_11 } } } },
    { BUILDING_FORTRESS_WALL_12, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_12 } } } },
    { BUILDING_FORTRESS_WALL_13, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_13 } } } },
    { BUILDING_FORTRESS_WALL_14, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_14 } } } },
    { BUILDING_FORTRESS_WALL_15, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_15 } } } },
    { BUILDING_FORTRESS_WALL_16, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_16 } } } },
    { BUILDING_FORTRESS_WALL_17, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_17 } } } },
    { BUILDING_FORTRESS_WALL_18, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_18 } } } },
    { BUILDING_FORTRESS_WALL_19, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_19 } } } },
    { BUILDING_FORTRESS_WALL_20, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_WALL_20 } } } },
    { BUILDING_FORTRESS_INTERIOR_WALL_1, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1 } } } },
    { BUILDING_FORTRESS_INTERIOR_WALL_2, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2 } } } },
    { BUILDING_FORTRESS_INTERIOR_WALL_3, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3 } } } },
    { BUILDING_FORTRESS_TOWER_1, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_TOWER_1 } }, { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED, SPAWNGROUP_WINTERGRASP_FORTRESS_TOWER_1_CANNONS } } },
    { BUILDING_FORTRESS_TOWER_2, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_TOWER_2 } }, { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED, SPAWNGROUP_WINTERGRASP_FORTRESS_TOWER_2_CANNONS } } },
    { BUILDING_FORTRESS_TOWER_3, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_TOWER_3 } }, { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED, SPAWNGROUP_WINTERGRASP_FORTRESS_TOWER_3_CANNONS } } },
    { BUILDING_FORTRESS_TOWER_4, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FORTRESS_TOWER_4 } }, { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED, SPAWNGROUP_WINTERGRASP_FORTRESS_TOWER_4_CANNONS } } },
    { BUILDING_TOWER_1, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_TOWER_1 } }, { PVP_TEAM_HORDE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_TOWER_1_GUARDS_HORDE }, { PVP_TEAM_ALLIANCE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_TOWER_1_GUARDS_ALLIANCE }, { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED, SPAWNGROUP_WINTERGRASP_TOWER_1_CANNONS } } },
    { BUILDING_TOWER_2, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_TOWER_2 } }, { PVP_TEAM_HORDE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_TOWER_2_GUARDS_HORDE }, { PVP_TEAM_ALLIANCE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_TOWER_2_GUARDS_ALLIANCE }, { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED, SPAWNGROUP_WINTERGRASP_TOWER_2_CANNONS } } },
    { BUILDING_TOWER_3, { { PVP_TEAM_NEUTRAL, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_TOWER_3 } }, { PVP_TEAM_HORDE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_TOWER_3_GUARDS_HORDE }, { PVP_TEAM_ALLIANCE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_TOWER_3_GUARDS_ALLIANCE }, { PVP_TEAM_NEUTRAL, BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE, BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED, SPAWNGROUP_WINTERGRASP_TOWER_3_CANNONS } } },
    { BUILDING_WORKSHOP_NE, { { PVP_TEAM_HORDE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_NE_HORDE } }, { PVP_TEAM_ALLIANCE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_NE_ALLIANCE } } } },
    { BUILDING_WORKSHOP_NW, { { PVP_TEAM_HORDE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_NW_HORDE } }, { PVP_TEAM_ALLIANCE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_NW_ALLIANCE } } } },
    { BUILDING_WORKSHOP_SE, { { PVP_TEAM_HORDE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_SE_HORDE } }, { PVP_TEAM_ALLIANCE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_SE_ALLIANCE } }, { PVP_TEAM_HORDE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_WORKSHOP_SE_GUARDS_HORDE }, { PVP_TEAM_ALLIANCE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_WORKSHOP_SE_GUARDS_ALLIANCE } } },
    { BUILDING_WORKSHOP_SW, { { PVP_TEAM_HORDE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_SW_HORDE } }, { PVP_TEAM_ALLIANCE, TYPEID_GAMEOBJECT, { GO_WINTERGRASP_FACTORY_BANNER_SW_ALLIANCE } }, { PVP_TEAM_HORDE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_WORKSHOP_SW_GUARDS_HORDE }, { PVP_TEAM_ALLIANCE, BATTLEFIELD_OBJECT_INFO_PHASE_ANY, BATTLEFIELD_OBJECT_INFO_FLAG_NONE, SPAWNGROUP_WINTERGRASP_WORKSHOP_SW_GUARDS_ALLIANCE } } }
};

static std::unordered_map<uint8 /*buildingId*/, uint8> const WintergraspTowerTexts =
{
    { BUILDING_TOWER_1, SAY_WINTERGRASP_WESTERN_TOWER_DAMAGED },
    { BUILDING_TOWER_2, SAY_WINTERGRASP_SOUTHERN_TOWER_DAMAGED },
    { BUILDING_TOWER_3, SAY_WINTERGRASP_EASTERN_TOWER_DAMAGED },
    { BUILDING_FORTRESS_TOWER_1, SAY_WINTERGRASP_NORTH_WESTERN_KEEP_TOWER_DAMAGED },
    { BUILDING_FORTRESS_TOWER_2, SAY_WINTERGRASP_SOUTH_WESTERN_KEEP_TOWER_DAMAGED },
    { BUILDING_FORTRESS_TOWER_3, SAY_WINTERGRASP_SOUTH_EASTERN_KEEP_TOWER_DAMAGED },
    { BUILDING_FORTRESS_TOWER_4, SAY_WINTERGRASP_NORTH_EASTERN_KEEP_TOWER_DAMAGED }
};

static std::vector<BattlefieldCapturePointInfo> const WintergraspCapturePointInfo =
{
    { BUILDING_WORKSHOP_NE, WORLDSTATE_WINTERGRASP_WORKSHOP_NE, GRAVEYARD_WORKSHOP_NE, true, false, true },
    { BUILDING_WORKSHOP_NW, WORLDSTATE_WINTERGRASP_WORKSHOP_NW, GRAVEYARD_WORKSHOP_NW, true, false, true },
    { BUILDING_WORKSHOP_SE, WORLDSTATE_WINTERGRASP_WORKSHOP_SE, GRAVEYARD_WORKSHOP_SE, true, true, false },
    { BUILDING_WORKSHOP_SW, WORLDSTATE_WINTERGRASP_WORKSHOP_SW, GRAVEYARD_WORKSHOP_SW, true, true, false },
    { BUILDING_WORKSHOP_KEEP_W, WORLDSTATE_WINTERGRASP_WORKSHOP_KEEP_W, 0, false, false, false },
    { BUILDING_WORKSHOP_KEEP_E, WORLDSTATE_WINTERGRASP_WORKSHOP_KEEP_E, 0, false, false, false }
};

static std::unordered_map<uint8 /*buildingId*/, uint8> const WintergraspWorkshopTexts =
{
    { BUILDING_WORKSHOP_NE, SAY_WINTERGRASP_SUNKEN_RING_ATTACKED_ALLIANCE },
    { BUILDING_WORKSHOP_NW, SAY_WINTERGRASP_BROKEN_TEMPLE_ATTACKED_ALLIANCE },
    { BUILDING_WORKSHOP_SE, SAY_WINTERGRASP_EASTSPARK_ATTACKED_ALLIANCE },
    { BUILDING_WORKSHOP_SW, SAY_WINTERGRASP_WESTSPARK_ATTACKED_ALLIANCE }
};

static std::unordered_map<uint32 /*areaId*/, uint8 /*buildingId*/> const WintergraspWorkshopAreas =
{
    { AREA_THE_SUNKEN_RING, BUILDING_WORKSHOP_NE },
    { AREA_THE_BROKEN_TEMPLE, BUILDING_WORKSHOP_NW },
    { AREA_THE_CHILLED_QUAGMIRE, BUILDING_WORKSHOP_NW },
    { AREA_EASTPARK_WORKSHOP, BUILDING_WORKSHOP_SE },
    { AREA_WESTPARK_WORKSHOP, BUILDING_WORKSHOP_SW }
};

static std::vector<BattlefieldGraveyardInfo> const WintergraspGraveyardInfo =
{
    { GRAVEYARD_WORKSHOP_NE, 0, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NE, GOSSIP_WINTERGRASP_GRAVEYARD_NE, SPELL_WINTERGRASP_TELEPORT_SUNKEN_RING },
    { GRAVEYARD_WORKSHOP_NW, 0, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NW, GOSSIP_WINTERGRASP_GRAVEYARD_NW, SPELL_WINTERGRASP_TELEPORT_BROKEN_TEMPLE },
    { GRAVEYARD_WORKSHOP_SE, 0, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SE, GOSSIP_WINTERGRASP_GRAVEYARD_SE, SPELL_WINTERGRASP_TELEPORT_EASTSPARK_FACTORY_GRAVEYARD },
    { GRAVEYARD_WORKSHOP_SW, 0, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SW, GOSSIP_WINTERGRASP_GRAVEYARD_SW, SPELL_WINTERGRASP_TELEPORT_WESTSPARK_FACTORY_GRAVEYARD },
    { GRAVEYARD_KEEP, 0, WORLDSAFELOC_WINTERGRASP_FORTRESS_EAST, GOSSIP_WINTERGRASP_GRAVEYARD_KEEP, SPELL_WINTERGRASP_TELEPORT_FORTRESS_GRAVEYARD },
    { GRAVEYARD_HORDE, 0, WORLDSAFELOC_WINTERGRASP_HORDE_STARTING_AREA, GOSSIP_WINTERGRASP_GRAVEYARD_HORDE, SPELL_WINTERGRASP_TELEPORT_HORDE_LANDING_ZONE },
    { GRAVEYARD_ALLIANCE, 0, WORLDSAFELOC_WINTERGRASP_ALLIANCE_STARTING_AREA, GOSSIP_WINTERGRASP_GRAVEYARD_ALLIANCE, SPELL_WINTERGRASP_TELEPORT_ALLIANCE_LANDING_ZONE }
};

static std::unordered_map<uint8 /*graveyardId*/, std::vector<BattlefieldObjectInfo>> const WintergraspGraveyardObjectInfos =
{
    { GRAVEYARD_KEEP, { { PVP_TEAM_HORDE, TYPEID_UNIT, { NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_TAUNKA_SPIRIT_GUIDE_KEEP } }, { PVP_TEAM_ALLIANCE, TYPEID_UNIT, { NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_DWARVEN_SPIRIT_GUIDE_KEEP } } } },
    { GRAVEYARD_WORKSHOP_NE, { { PVP_TEAM_HORDE, TYPEID_UNIT, { NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_TAUNKA_SPIRIT_GUIDE_WORKSHOP_NE } }, { PVP_TEAM_ALLIANCE, TYPEID_UNIT, { NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_DWARVEN_SPIRIT_GUIDE_WORKSHOP_NE } } } },
    { GRAVEYARD_WORKSHOP_NW, { { PVP_TEAM_HORDE, TYPEID_UNIT, { NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_TAUNKA_SPIRIT_GUIDE_WORKSHOP_NW } }, { PVP_TEAM_ALLIANCE, TYPEID_UNIT, { NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_DWARVEN_SPIRIT_GUIDE_WORKSHOP_NW } } } },
    { GRAVEYARD_WORKSHOP_SW, { { PVP_TEAM_HORDE, TYPEID_UNIT, { NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_TAUNKA_SPIRIT_GUIDE_WORKSHOP_SW } }, { PVP_TEAM_ALLIANCE, TYPEID_UNIT, { NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_DWARVEN_SPIRIT_GUIDE_WORKSHOP_SW } } } },
    { GRAVEYARD_WORKSHOP_SE, { { PVP_TEAM_HORDE, TYPEID_UNIT, { NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_TAUNKA_SPIRIT_GUIDE_WORKSHOP_SE } }, { PVP_TEAM_ALLIANCE, TYPEID_UNIT, { NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_DWARVEN_SPIRIT_GUIDE_WORKSHOP_SE } } } },
    { GRAVEYARD_HORDE, { { PVP_TEAM_HORDE, TYPEID_UNIT, { NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_TAUNKA_SPIRIT_GUIDE_HORDE_LANDING } } } },
    { GRAVEYARD_ALLIANCE, { { PVP_TEAM_ALLIANCE, TYPEID_UNIT, { NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE }, { SPAWNID_WINTERGRASP_DWARVEN_SPIRIT_GUIDE_ALLIANCE_LANDING } } } }
};

struct WintergraspRankInfo
{
    uint32 SpellId;
    uint8 PromotionStacks;
    uint32 PromotionSpellId;
    uint8 PromotionText;
};

static std::array<WintergraspRankInfo, 2> const WintergraspRanks =
{
    {
        { SPELL_WINTERGRASP_RECRUIT, WINTERGRASP_RECRUIT_PROMOTION_STACKS, SPELL_WINTERGRASP_CORPORAL, SAY_WINTERGRASP_RANK_CORPORAL },
        { SPELL_WINTERGRASP_CORPORAL, WINTERGRASP_CORPORAL_PROMOTION_STACKS, SPELL_WINTERGRASP_LIEUTENANT, SAY_WINTERGRASP_RANK_FIRST_LIEUTENANT }
    }
};

static std::array<std::array<uint32, 4>, PVP_TEAMS_COUNT> const WintergraspTenacityHonorSpells =
{
    {
        { SPELL_WINTERGRASP_HONORABLE, SPELL_WINTERGRASP_GREAT_HONOR, SPELL_WINTERGRASP_GREATER_HONOR, SPELL_WINTERGRASP_GREATEST_HONOR },
        { SPELL_WINTERGRASP_LOK_REGAR, SPELL_WINTERGRASP_LOK_NARASH, SPELL_WINTERGRASP_LOKTAR, SPELL_WINTERGRASP_LOKTAR_OGAR }
    }
};

WintergraspZone::WintergraspZone(BattlefieldWintergrasp* wintergrasp) : BattlefieldZone(wintergrasp), _wintergrasp(wintergrasp)
{
}

void WintergraspZone::OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo)
{
    BattlefieldZone::OnObjectCreate(object, objectInfo);

    if (Creature* cannon = object->ToCreature())
        if (cannon->GetEntry() == NPC_WINTERGRASP_TOWER_CANNON)
            cannon->SetFaction(_wintergrasp->GetFaction(GetDisplayedPvPTeamId()));
}

std::vector<BattlefieldObjectInfo> const* WintergraspZone::GetObjectInfos() const
{
    return &WintergraspZoneObjectInfos;
}

WintergraspBuilding::WintergraspBuilding(BattlefieldWintergrasp* wintergrasp, BattlefieldBuildingInfo const info) : BattlefieldDestructibleBuilding(wintergrasp, info), _wintergrasp(wintergrasp)
{
}

void WintergraspBuilding::OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo)
{
    BattlefieldDestructibleBuilding::OnObjectCreate(object, objectInfo);

    if (Creature* cannon = object->ToCreature())
        if (cannon->GetEntry() == NPC_WINTERGRASP_TOWER_CANNON)
            cannon->SetFaction(_wintergrasp->GetFaction(GetDisplayedPvPTeamId()));
}

std::vector<BattlefieldObjectInfo> const* WintergraspBuilding::GetObjectInfos() const
{
    return Trinity::Containers::MapGetValuePtr(WintergraspBuildingObjectInfos, Info.Id);
}

bool WintergraspBuilding::Damage()
{
    if (!BattlefieldDestructibleBuilding::Damage())
        return false;

    if (_wintergrasp->IsBattleTime())
        SendText(false);

    return true;
}

bool WintergraspBuilding::Destroy()
{
    if (!BattlefieldDestructibleBuilding::Destroy())
        return false;

    if (BuildingType == BATTLEFIELD_BUILDING_TYPE_DOOR_LAST)
        _wintergrasp->HandleVaultGateDestroyed();

    if (!_wintergrasp->IsBattleTime())
        return true;

    SendText(true);
    if (BuildingType == BATTLEFIELD_BUILDING_TYPE_TOWER)
        _wintergrasp->HandleSouthernTowerDestroyed();

    return true;
}

void WintergraspBuilding::SendText(bool destroyed) const
{
    if (uint8 const* textGroup = Trinity::Containers::MapGetValuePtr(WintergraspTowerTexts, Info.Id))
        _wintergrasp->SendStalkerText(uint8(*textGroup + (destroyed ? 1 : 0)));
}

WintergraspCapturePoint::WintergraspCapturePoint(BattlefieldWintergrasp* wintergrasp, BattlefieldCapturePointInfo const info) : BattlefieldCapturePoint(wintergrasp, info), _wintergrasp(wintergrasp)
{
}

std::vector<BattlefieldObjectInfo> const* WintergraspCapturePoint::GetObjectInfos() const
{
    return Trinity::Containers::MapGetValuePtr(WintergraspBuildingObjectInfos, Info.Id);
}

void WintergraspCapturePoint::SetOwner(PvPTeamId pvpTeamId)
{
    BattlefieldCapturePoint::SetOwner(pvpTeamId);

    _wintergrasp->SendVehicleLimits();
}

void WintergraspCapturePoint::Attack(PvPTeamId pvpTeamId)
{
    BattlefieldCapturePoint::Attack(pvpTeamId);

    SendText(pvpTeamId, false);
}

void WintergraspCapturePoint::Capture(PvPTeamId pvpTeamId)
{
    BattlefieldCapturePoint::Capture(pvpTeamId);

    SendText(pvpTeamId, true);
}

void WintergraspCapturePoint::SendText(PvPTeamId pvpTeamId, bool captured) const
{
    if (uint8 const* textGroup = Trinity::Containers::MapGetValuePtr(WintergraspWorkshopTexts, Info.Id))
        _wintergrasp->SendStalkerText(uint8(*textGroup + (pvpTeamId == PVP_TEAM_HORDE ? 2 : 0) + (captured ? 1 : 0)));
}

WintergraspGraveyard::WintergraspGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info) : BattlefieldGraveyard(battlefield, info)
{
}

std::vector<BattlefieldObjectInfo> const* WintergraspGraveyard::GetObjectInfos() const
{
    return Trinity::Containers::MapGetValuePtr(WintergraspGraveyardObjectInfos, Info.Id);
}

void WintergraspGraveyard::InitializeState()
{
    switch (Info.Id)
    {
        case GRAVEYARD_KEEP:
            SetOwner(Battle->GetControllingTeam());
            break;
        case GRAVEYARD_HORDE:
            SetOwner(PVP_TEAM_HORDE);
            break;
        case GRAVEYARD_ALLIANCE:
            SetOwner(PVP_TEAM_ALLIANCE);
            break;
        default:
            break;
    }
}

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
    EmplaceEntity<BattlefieldZone>(std::make_unique<WintergraspZone>(this));

    for (auto itr = WintergraspBuildingInfo.begin(); itr != WintergraspBuildingInfo.end(); ++itr)
    {
        WintergraspBuildingPointer building = std::make_unique<WintergraspBuilding>(this, *itr);
        building->InitializeState();
        EmplaceEntity<BattlefieldBuilding>(std::move(building));
    }

    for (auto itr = WintergraspGraveyardInfo.begin(); itr != WintergraspGraveyardInfo.end(); ++itr)
    {
        WintergraspGraveyardPointer graveyard = std::make_unique<WintergraspGraveyard>(this, *itr);
        graveyard->InitializeState();
        EmplaceEntity<BattlefieldGraveyard>(std::move(graveyard));
    }

    for (auto itr = WintergraspCapturePointInfo.begin(); itr != WintergraspCapturePointInfo.end(); ++itr)
    {
        WintergraspCapturePointPointer capturePoint = std::make_unique<WintergraspCapturePoint>(this, *itr);
        capturePoint->InitializeState();
        EmplaceEntity<BattlefieldBuilding>(std::move(capturePoint));
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
    SendWorldStateToMap(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, GetControllingTeam() == PVP_TEAM_HORDE ? 1 : 0);
    SendWorldStateToMap(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, GetControllingTeam() == PVP_TEAM_ALLIANCE ? 1 : 0);
    if (IsEnabled())
        SendWorldStateToMap(WORLDSTATE_WINTERGRASP_CONTROL, GetControlState());

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
            SendStalkerText(SAY_WINTERGRASP_BATTLE_ABOUT_TO_BEGIN);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, 1);
            break;
        case BATTLEFIELD_PHASE_BATTLE:
            SendStalkerText(SAY_WINTERGRASP_BATTLE_BEGIN);
            SetKeepCollisionsOpen(false);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_CONTROL, WINTERGRASP_CONTROL_BATTLE);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_ALLIANCE_DEFENDER, 0);
            SendWorldStateToMap(WORLDSTATE_WINTERGRASP_HORDE_DEFENDER, 0);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_BATTLE_TIMER, 1);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_SHOW_COOLDOWN, 0);
            SendWorldStateToAll(WORLDSTATE_WINTERGRASP_ICON_ACTIVE, 1);
            break;
        case BATTLEFIELD_PHASE_IDLE:
        {
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
                SendStalkerText(uint8((GetControllingTeam() == PVP_TEAM_HORDE ? SAY_WINTERGRASP_FORTRESS_DEFENDED_HORDE : SAY_WINTERGRASP_FORTRESS_DEFENDED_ALLIANCE) + (captured ? 1 : 0)));
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

void BattlefieldWintergrasp::HandlePlayerEnterZone(Player* player)
{
    Battlefield::HandlePlayerEnterZone(player);

    if (IsPlayerInBattle(player) || HasPendingBattleResult(player))
        return;

    RemoveRankAuras(player);
    SetTenacity(player, 0);
}

void BattlefieldWintergrasp::HandleKill(Player* killer, Unit* victim)
{
    Battlefield::HandleKill(killer, victim);

    if (killer == victim || !IsBattleTime())
        return;

    if (Player* playerVictim = victim->ToPlayer())
    {
        if (playerVictim->GetTeamId() == killer->GetTeamId())
            return;
    }
    else
    {
        switch (victim->GetEntry())
        {
            case NPC_WINTERGRASP_CATAPULT:
            case NPC_WINTERGRASP_DEMOLISHER:
            case NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE:
            case NPC_WINTERGRASP_SIEGE_ENGINE_HORDE:
            case NPC_WINTERGRASP_GUARD_HORDE:
            case NPC_WINTERGRASP_GUARD_ALLIANCE:
            case NPC_WINTERGRASP_TOWER_CANNON:
                break;
            default:
                return;
        }
    }

    PromotePlayer(killer);
    for (ObjectGuid playerGuid : GetZonePlayers())
    {
        Player* player = ObjectAccessor::GetPlayer(*killer, playerGuid);
        if (player && player != killer && player->GetTeamId() == killer->GetTeamId() && player->GetExactDist2d(victim) <= WINTERGRASP_RANK_CREDIT_DISTANCE)
            PromotePlayer(player);
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

    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_VEHICLE_HORDE, GetVehicleCount(PVP_TEAM_HORDE));
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_MAX_VEHICLE_HORDE, GetVehicleLimit(PVP_TEAM_HORDE));
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_VEHICLE_ALLIANCE, GetVehicleCount(PVP_TEAM_ALLIANCE));
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_MAX_VEHICLE_ALLIANCE, GetVehicleLimit(PVP_TEAM_ALLIANCE));
    packet.Worldstates.emplace_back(WORLDSTATE_WINTERGRASP_CAPTURE_NEUTRAL_PERCENT, WINTERGRASP_CAPTURE_NEUTRAL_PERCENT);
}

void BattlefieldWintergrasp::SendGlobalWorldStates(Player const* player)
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

void BattlefieldWintergrasp::OnCreatureCreate(Creature* object)
{
    Battlefield::OnCreatureCreate(object);

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
        case NPC_WINTERGRASP_CATAPULT:
        case NPC_WINTERGRASP_DEMOLISHER:
        case NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE:
        case NPC_WINTERGRASP_SIEGE_ENGINE_HORDE:
            if (TempSummon* summon = object->ToTempSummon())
                if (Player* builder = ObjectAccessor::GetPlayer(*object, summon->GetSummonerGUID()))
                    AddVehicle(object->GetGUID(), PvPTeamIdByTeamId(builder->GetTeamId()));
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnCreatureRemove(Creature* object)
{
    Battlefield::OnCreatureRemove(object);

    switch (object->GetEntry())
    {
        case NPC_WINTERGRASP_CATAPULT:
        case NPC_WINTERGRASP_DEMOLISHER:
        case NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE:
        case NPC_WINTERGRASP_SIEGE_ENGINE_HORDE:
            RemoveVehicle(object->GetGUID());
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnGameObjectCreate(GameObject* object)
{
    Battlefield::OnGameObjectCreate(object);

    switch (object->GetEntry())
    {
        case GO_WINTERGRASP_TITAN_RELIC_HORDE:
            _titanRelicGUIDs[PVP_TEAM_HORDE] = object->GetGUID();
            if (IsVaultGateDestroyed())
                object->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
            break;
        case GO_WINTERGRASP_TITAN_RELIC_ALLIANCE:
            _titanRelicGUIDs[PVP_TEAM_ALLIANCE] = object->GetGUID();
            if (IsVaultGateDestroyed())
                object->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
            break;
        case GO_WINTERGRASP_KEEP_COLLISION_WALL:
            _keepCollisionWallGUID = object->GetGUID();
            if (IsVaultGateDestroyed())
                object->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_WINTERGRASP_KEEP_DOOR_COLLISION:
            _keepDoorCollisionGUID = object->GetGUID();
            if (IsVaultGateDestroyed())
                object->SetGoState(GO_STATE_ACTIVE);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnUnitDeath(Unit* unit)
{
    Battlefield::OnUnitDeath(unit);

    RemoveVehicle(unit->GetGUID());
}

void BattlefieldWintergrasp::ProcessEvent(WorldObject* object, uint32 eventId)
{
    Battlefield::ProcessEvent(object, eventId);

    if (eventId == EVENT_WINTERGRASP_TITAN_RELIC)
        QueueEvent<BattlefieldBattleCaptureEvent>();
}

void BattlefieldWintergrasp::HandleSouthernTowerDestroyed()
{
    uint32 towers = 0;
    uint32 destroyedTowers = 0;
    for (auto const& [_, building] : GetEntities<BattlefieldBuilding>())
    {
        if (building->BuildingType != BATTLEFIELD_BUILDING_TYPE_TOWER)
            continue;

        ++towers;
        if (building->GetDestructibleState() == GO_DESTRUCTIBLE_DESTROYED)
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

void BattlefieldWintergrasp::HandleVaultGateDestroyed()
{
    SetKeepCollisionsOpen(true);
    for (ObjectGuid relicGuid : _titanRelicGUIDs)
        if (GameObject* relic = GetGameObject(relicGuid))
            relic->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
}

void BattlefieldWintergrasp::SendStalkerText(uint8 textGroup, WorldObject const* whisperTarget /*= nullptr*/)
{
    Map* battleMap = GetBattleMap();
    CreatureData const* stalkerData = sObjectMgr->GetCreatureData(SPAWNID_WINTERGRASP_INVISIBLE_STALKER);
    if (!battleMap || !stalkerData)
        return;

    battleMap->LoadGrid(stalkerData->spawnPoint.GetPositionX(), stalkerData->spawnPoint.GetPositionY());
    if (Creature* stalker = GetCreature(_stalkerGUID))
        sCreatureTextMgr->SendChat(stalker, textGroup, whisperTarget);
}

void BattlefieldWintergrasp::SendVehicleLimits()
{
    SendWorldStateToMap(WORLDSTATE_WINTERGRASP_MAX_VEHICLE_HORDE, GetVehicleLimit(PVP_TEAM_HORDE));
    SendWorldStateToMap(WORLDSTATE_WINTERGRASP_MAX_VEHICLE_ALLIANCE, GetVehicleLimit(PVP_TEAM_ALLIANCE));
}

bool BattlefieldWintergrasp::CanUseTitanRelic(Player const* player) const
{
    return IsVaultGateDestroyed() && player->GetTeamId() == GetAttackingTeamId() && IsPlayerInBattle(player);
}

bool BattlefieldWintergrasp::CanBuildVehicle(PvPTeamId pvpTeamId) const
{
    return IsBattleTime() && GetVehicleCount(pvpTeamId) < GetVehicleLimit(pvpTeamId);
}

bool BattlefieldWintergrasp::IsSpellAreaAllowed(uint32 spellId, Player const* player, uint32 newArea) const
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
        case SPELL_WINTERGRASP_HORDE_CONTROLS_FACTORY_PHASE_SHIFT:
        {
            uint8 const* buildingId = Trinity::Containers::MapGetValuePtr(WintergraspWorkshopAreas, newArea);
            BattlefieldBuilding const* workshop = buildingId ? GetEntity<BattlefieldBuilding>(*buildingId) : nullptr;
            return workshop && workshop->GetPvPTeamId() == (spellId == SPELL_WINTERGRASP_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT ? PVP_TEAM_ALLIANCE : PVP_TEAM_HORDE);
        }
        case SPELL_WINTERGRASP_ALLIANCE_CONTROL_PHASE_SHIFT:
            return GetControllingTeamId() == TEAM_ALLIANCE;
        case SPELL_WINTERGRASP_HORDE_CONTROL_PHASE_SHIFT:
            return GetControllingTeamId() == TEAM_HORDE;
        default:
            break;
    }

    return true;
}

void BattlefieldWintergrasp::ClearBattleStatus()
{
    Battlefield::ClearBattleStatus();

    DespawnVehicles();
}

void BattlefieldWintergrasp::OnPlayerJoinBattle(Player* player)
{
    RemoveRankAuras(player);
    player->CastSpell(player, SPELL_WINTERGRASP_RECRUIT, true);
}

void BattlefieldWintergrasp::OnPlayerLeaveBattle(Player* player)
{
    RemoveRankAuras(player);
    SetTenacity(player, 0);
    for (Unit const* vehicleBase = player->GetVehicleBase(); vehicleBase; vehicleBase = vehicleBase->GetVehicleBase())
    {
        bool const builtVehicle = std::any_of(_vehicleGUIDs.begin(), _vehicleGUIDs.end(), [vehicleBase](GuidUnorderedSet const& vehicleGuids) -> bool
        {
            return vehicleGuids.contains(vehicleBase->GetGUID());
        });

        if (builtVehicle)
        {
            player->ExitVehicle();
            break;
        }
    }
}

void BattlefieldWintergrasp::OnPlayerBattleResult(Player* player)
{
    Optional<time_t> const battleJoinTime = GetBattleJoinTime(player);
    if (!battleJoinTime || Seconds(Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_START) - *battleJoinTime) < 90s)
        return;

    PvPTeamId const pvpTeamId = PvPTeamIdByTeamId(player->GetTeamId());
    bool const defender = pvpTeamId == Values.Get<PvPTeamId>(BATTLEFIELD_VALUE_STATE_BATTLE_DEFENDER);
    uint32 intactBuildings = 0;
    uint32 damagedBuildings = 0;
    for (uint8 workshopId : { BUILDING_WORKSHOP_NE, BUILDING_WORKSHOP_NW })
        if (BattlefieldBuilding const* workshop = GetEntity<BattlefieldBuilding>(workshopId); workshop && workshop->GetPvPTeamId() == pvpTeamId)
            ++intactBuildings;

    for (auto const& [_, building] : GetEntities<BattlefieldBuilding>())
    {
        if (building->BuildingType != (defender ? BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER : BATTLEFIELD_BUILDING_TYPE_TOWER))
            continue;

        if (building->GetDestructibleState() == GO_DESTRUCTIBLE_INTACT)
            ++intactBuildings;
        else if (defender && building->GetDestructibleState() == GO_DESTRUCTIBLE_DAMAGED)
            ++damagedBuildings;
    }

    for (uint32 i = 0; i < intactBuildings; ++i)
        player->CastSpell(player, SPELL_WINTERGRASP_INTACT_BUILDING, true);

    for (uint32 i = 0; i < damagedBuildings; ++i)
        player->CastSpell(player, SPELL_WINTERGRASP_DAMAGED_BUILDING, true);

    player->CastSpell(player, defender != (Values.Get<BattlefieldBattleResult>(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT) == BATTLEFIELD_BATTLE_RESULT_CAPTURED) ? SPELL_WINTERGRASP_VICTORY_REWARD : SPELL_WINTERGRASP_DEFEAT_REWARD, true);
}

void BattlefieldWintergrasp::OnPeriodicUpdate(std::vector<Player*> const& players, Milliseconds /*diff*/)
{
    TeamId tenacityTeamId = TEAM_NEUTRAL;
    uint32 tenacityStack = 0;
    uint32 const allianceCount = GetBattlePlayerCount(TEAM_ALLIANCE);
    uint32 const hordeCount = GetBattlePlayerCount(TEAM_HORDE);
    if (IsBattleTime() && allianceCount && hordeCount && allianceCount != hordeCount)
    {
        uint32 const smallCount = std::min(allianceCount, hordeCount);
        uint32 const bigCount = std::max(allianceCount, hordeCount);
        tenacityStack = std::min<uint32>((bigCount - smallCount) * 4 / smallCount, sSpellMgr->AssertSpellInfo(SPELL_WINTERGRASP_TENACITY)->StackAmount);
        tenacityTeamId = allianceCount < hordeCount ? TEAM_ALLIANCE : TEAM_HORDE;
    }

    GuidUnorderedSet tenacityUnitGUIDs;
    for (Player* player : players)
    {
        uint32 const stack = player->GetTeamId() == tenacityTeamId ? tenacityStack : 0;
        SetTenacity(player, stack);
        if (!stack)
            continue;

        for (Unit* controlled : player->m_Controlled)
        {
            if (controlled->IsPlayer() || controlled->IsTotem())
                continue;

            if (!controlled->HasAura(SPELL_WINTERGRASP_TENACITY_VEHICLE, player->GetGUID()))
                controlled->RemoveAurasDueToSpell(SPELL_WINTERGRASP_TENACITY_VEHICLE);

            SetTenacityAura(player, controlled, SPELL_WINTERGRASP_TENACITY_VEHICLE, stack);
            tenacityUnitGUIDs.insert(controlled->GetGUID());
        }
    }

    Map* battleMap = GetBattleMap();
    if (battleMap)
    {
        for (ObjectGuid unitGuid : _tenacityUnitGUIDs)
        {
            if (!tenacityUnitGUIDs.contains(unitGuid))
                if (Creature* unit = unitGuid.IsPet() ? battleMap->GetPet(unitGuid) : battleMap->GetCreature(unitGuid))
                    unit->RemoveAurasDueToSpell(SPELL_WINTERGRASP_TENACITY_VEHICLE);
        }
    }

    _tenacityUnitGUIDs = std::move(tenacityUnitGUIDs);

    if (!battleMap || !IsBattleTime() || IsVaultGateDestroyed())
        return;

    TeamId const attackingTeamId = GetAttackingTeamId();
    for (ObjectGuid playerGuid : GetZonePlayers())
    {
        Player* player = ObjectAccessor::GetPlayer(battleMap, playerGuid);
        if (!player || player->GetTeamId() != attackingTeamId || player->IsGameMaster() || player->IsInFlight() || player->IsBeingTeleported())
            continue;

        if (WintergraspTitanRelicChamberBoundary.IsWithinBoundary(player) && WintergraspTitanRelicChamberHeightBoundary.IsWithinBoundary(player))
            TeleportPlayerToBattle(player);
    }
}

void BattlefieldWintergrasp::SetKeepCollisionsOpen(bool open)
{
    for (ObjectGuid collisionGuid : { _keepCollisionWallGUID, _keepDoorCollisionGUID })
        if (GameObject* collision = GetGameObject(collisionGuid))
            collision->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
}

void BattlefieldWintergrasp::RemoveRankAuras(Player* player)
{
    for (uint32 spellId : { SPELL_WINTERGRASP_RECRUIT, SPELL_WINTERGRASP_CORPORAL, SPELL_WINTERGRASP_LIEUTENANT })
        player->RemoveAurasDueToSpell(spellId);
}

void BattlefieldWintergrasp::PromotePlayer(Player* player)
{
    if (!IsPlayerInBattle(player))
        return;

    for (WintergraspRankInfo const& rank : WintergraspRanks)
    {
        Aura* aura = player->GetAura(rank.SpellId);
        if (!aura)
            continue;

        player->CastSpell(player, rank.SpellId, true);
        if (aura->GetStackAmount() >= rank.PromotionStacks)
        {
            player->RemoveAurasDueToSpell(rank.SpellId);
            player->CastSpell(player, rank.PromotionSpellId, true);
            SendStalkerText(rank.PromotionText, player);
        }
        return;
    }
}

void BattlefieldWintergrasp::SendVehicleCount(PvPTeamId pvpTeamId)
{
    SendWorldStateToMap(pvpTeamId == PVP_TEAM_HORDE ? WORLDSTATE_WINTERGRASP_VEHICLE_HORDE : WORLDSTATE_WINTERGRASP_VEHICLE_ALLIANCE, GetVehicleCount(pvpTeamId));
}

void BattlefieldWintergrasp::AddVehicle(ObjectGuid vehicleGuid, PvPTeamId pvpTeamId)
{
    if (_vehicleGUIDs[pvpTeamId].insert(vehicleGuid).second)
        SendVehicleCount(pvpTeamId);
}

void BattlefieldWintergrasp::RemoveVehicle(ObjectGuid vehicleGuid)
{
    for (PvPTeamId pvpTeamId : { PVP_TEAM_HORDE, PVP_TEAM_ALLIANCE })
    {
        if (!_vehicleGUIDs[pvpTeamId].erase(vehicleGuid))
            continue;

        SendVehicleCount(pvpTeamId);
        return;
    }
}

void BattlefieldWintergrasp::DespawnVehicles()
{
    for (GuidUnorderedSet const& vehicleGuids : _vehicleGUIDs)
    {
        GuidVector const guids(vehicleGuids.begin(), vehicleGuids.end());
        for (ObjectGuid vehicleGuid : guids)
        {
            RemoveVehicle(vehicleGuid);
            if (Creature* vehicle = GetCreature(vehicleGuid))
                vehicle->DespawnOrUnsummon();
        }
    }
}

void BattlefieldWintergrasp::SetTenacity(Player* player, uint32 stack)
{
    SetTenacityAura(player, player, SPELL_WINTERGRASP_TENACITY, stack);
    SetTenacityAura(player, player, SPELL_WINTERGRASP_TENACITY_HEALING, stack);

    std::array<uint32, 4> const& honorSpells = WintergraspTenacityHonorSpells[player->GetTeamId()];
    uint32 const honorSpellId = stack ? honorSpells[std::min<size_t>(stack, honorSpells.size()) - 1] : 0;
    for (uint32 spellId : honorSpells)
        if (spellId != honorSpellId)
            player->RemoveAurasDueToSpell(spellId);

    if (!honorSpellId)
        return;

    if (Aura* honorAura = player->GetAura(honorSpellId))
    {
        if (honorAura->GetDuration() * 2 < honorAura->GetMaxDuration())
            honorAura->RefreshDuration();
    }
    else
        player->CastSpell(player, honorSpellId, true);
}

void BattlefieldWintergrasp::SetTenacityAura(Unit* caster, Unit* target, uint32 spellId, uint32 stack)
{
    Aura* aura = target->GetAura(spellId, caster->GetGUID());
    if (!stack)
    {
        if (aura)
            aura->Remove();

        return;
    }

    if (!aura || aura->GetStackAmount() != stack)
        caster->SetAuraStack(spellId, target, stack);
}

uint32 BattlefieldWintergrasp::GetControlState() const
{
    if (IsBattleTime())
        return WINTERGRASP_CONTROL_BATTLE;

    return GetControllingTeam() == PVP_TEAM_HORDE ? WINTERGRASP_CONTROL_HORDE : WINTERGRASP_CONTROL_ALLIANCE;
}

uint32 BattlefieldWintergrasp::GetVehicleCount(PvPTeamId pvpTeamId) const
{
    return uint32(_vehicleGUIDs[pvpTeamId].size());
}

uint32 BattlefieldWintergrasp::GetVehicleLimit(PvPTeamId pvpTeamId) const
{
    uint32 limit = 0;
    for (auto const& [_, building] : GetEntities<BattlefieldBuilding>())
        if (building->BuildingType == BATTLEFIELD_BUILDING_TYPE_WORKSHOP && building->GetPvPTeamId() == pvpTeamId)
            limit += WINTERGRASP_VEHICLES_PER_WORKSHOP;

    return limit;
}

bool BattlefieldWintergrasp::IsVaultGateDestroyed() const
{
    BattlefieldBuilding const* vaultGate = GetEntity<BattlefieldBuilding>(BUILDING_FORTRESS_VAULT_GATE);
    return vaultGate && vaultGate->GetDestructibleState() == GO_DESTRUCTIBLE_DESTROYED;
}

class BattlefieldWintergraspScript : public BattlefieldScript
{
public:
    BattlefieldWintergraspScript() : BattlefieldScript("battlefield_wg") { }

    Battlefield* GetBattlefield() const override
    {
        return new BattlefieldWintergrasp();
    }
};

void AddSCBattlefieldWintergraspScript()
{
    new BattlefieldWintergraspScript();
}
