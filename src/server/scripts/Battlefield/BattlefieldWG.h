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

#ifndef TRINITY_BATTLEFIELD_WG_
#define TRINITY_BATTLEFIELD_WG_

#include "Battlefield.h"
#include "BattlefieldEntities.h"
#include <memory>
#include <unordered_map>

enum WintergraspNPCEntries
{
    NPC_WINTERGRASP_TOWER_CANNON = 28366,
    NPC_WINTERGRASP_GUARD_HORDE = 30739,
    NPC_WINTERGRASP_GUARD_ALLIANCE = 30740,
    NPC_WINTERGRASP_STALKER = 27869,

    NPC_WINTERGRASP_VIERON_BLAZEFEATHER = 31102,
    NPC_WINTERGRASP_STONE_GUARD_MUKAR = 32296, // <WINTERGRASP QUARTERMASTER>
    NPC_WINTERGRASP_HOODOO_MASTER_FU_JIN = 31101, // <MASTER HEXXER>
    NPC_WINTERGRASP_CHAMPION_ROS_SLAI = 39173, // <WINTERGRASP QUARTERMASTER>
    NPC_WINTERGRASP_COMMANDER_DARDOSH = 31091,
    NPC_WINTERGRASP_TACTICAL_OFFICER_KILRATH = 31151,
    NPC_WINTERGRASP_SIEGESMITH_STRONGHOOF = 31106,
    NPC_WINTERGRASP_PRIMALIST_MULFORT = 31053,
    NPC_WINTERGRASP_LIEUTENANT_MURP = 31107,

    NPC_WINTERGRASP_BOWYER_RANDOLPH = 31052,
    NPC_WINTERGRASP_KNIGHT_DAMERON = 32294, // <WINTERGRASP QUARTERMASTER>
    NPC_WINTERGRASP_SORCERESS_KAYLANA = 31051, // <ENCHANTRESS>
    NPC_WINTERGRASP_MARSHAL_MAGRUDER = 39172, // <WINTERGRASP QUARTERMASTER>
    NPC_WINTERGRASP_COMMANDER_ZANNETH = 31036,
    NPC_WINTERGRASP_TACTICAL_OFFICER_AHBRAMIS = 31153,
    NPC_WINTERGRASP_SIEGE_MASTER_STOUTHANDLE = 31108,
    NPC_WINTERGRASP_ANCHORITE_TESSA = 31054,
    NPC_WINTERGRASP_SENIOR_DEMOLITIONIST_LEGOSO = 31109,

    NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE = 31841, // Horde spirit guide for Wintergrasp
    NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE = 31842, // Alliance spirit guide for Wintergrasp

    NPC_WINTERGRASP_GOBLIN_MECHANIC = 30400,
    NPC_WINTERGRASP_GNOMISH_ENGINEER = 30499,
    NPC_WINTERGRASP_CONTROL_ARMS = 27852,
    NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE = 28312,
    NPC_WINTERGRASP_SIEGE_ENGINE_HORDE = 32627,
    NPC_WINTERGRASP_SIEGE_TURRET_HORDE = 32629,
    NPC_WINTERGRASP_SIEGE_TURRET_ALLIANCE = 28319,
    NPC_WINTERGRASP_CATAPULT = 27881,
    NPC_WINTERGRASP_DEMOLISHER = 28094,

    NPC_WINTERGRASP_PVP_KILL_HORDE = 39019,
    NPC_WINTERGRASP_PVP_KILL_ALLIANCE = 31086
};

enum WintergraspGossipText
{
    GOSSIP_WINTERGRASP_GRAVEYARD_NE = 20071,
    GOSSIP_WINTERGRASP_GRAVEYARD_NW = 20072,
    GOSSIP_WINTERGRASP_GRAVEYARD_SE = 20074,
    GOSSIP_WINTERGRASP_GRAVEYARD_SW = 20073,
    GOSSIP_WINTERGRASP_GRAVEYARD_KEEP = 20070,
    GOSSIP_WINTERGRASP_GRAVEYARD_HORDE = 20075,
    GOSSIP_WINTERGRASP_GRAVEYARD_ALLIANCE = 20076
};

enum WintergraspGameObjects
{
    GO_WINTERGRASP_FORTRESS_WALL_1 = 190219,
    GO_WINTERGRASP_FORTRESS_WALL_2 = 190220,
    GO_WINTERGRASP_FORTRESS_WALL_3 = 190369,
    GO_WINTERGRASP_FORTRESS_WALL_4 = 190370,
    GO_WINTERGRASP_FORTRESS_WALL_5 = 190371,
    GO_WINTERGRASP_FORTRESS_WALL_6 = 190372,
    GO_WINTERGRASP_FORTRESS_WALL_7 = 190374,
    GO_WINTERGRASP_FORTRESS_GATE = 190375,
    GO_WINTERGRASP_FORTRESS_WALL_8 = 190376,
    GO_WINTERGRASP_FORTRESS_WALL_9 = 191795,
    GO_WINTERGRASP_FORTRESS_WALL_10 = 191796,
    GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1 = 191797,
    GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2 = 191798,
    GO_WINTERGRASP_FORTRESS_WALL_11 = 191799,
    GO_WINTERGRASP_FORTRESS_WALL_12 = 191800,
    GO_WINTERGRASP_FORTRESS_WALL_13 = 191801,
    GO_WINTERGRASP_FORTRESS_WALL_14 = 191802,
    GO_WINTERGRASP_FORTRESS_WALL_15 = 191803,
    GO_WINTERGRASP_FORTRESS_WALL_16 = 191804,
    GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3 = 191805,
    GO_WINTERGRASP_FORTRESS_WALL_17 = 191806,
    GO_WINTERGRASP_FORTRESS_WALL_18 = 191807,
    GO_WINTERGRASP_FORTRESS_WALL_19 = 191808,
    GO_WINTERGRASP_FORTRESS_WALL_20 = 191809,
    GO_WINTERGRASP_FORTRESS_VAULT_GATE = 191810,
    GO_WINTERGRASP_FORTRESS_TELEPORTER_HORDE = 190763,
    GO_WINTERGRASP_FORTRESS_TELEPORTER_ALLIANCE = 191575,
    GO_WINTERGRASP_FORTRESS_TELEPORTER_VEHICLE = 192951
};

enum WintergraspSpawnGroups
{
    SPAWNGROUP_WINTERGRASP_FORTRESS_TELEPORTERS = 100,
};

enum WintergraspSpells
{
    // Wartime auras
    SPELL_WINTERGRASP_RECRUIT = 37795,
    SPELL_WINTERGRASP_CORPORAL = 33280,
    SPELL_WINTERGRASP_LIEUTENANT = 55629,
    SPELL_WINTERGRASP_TENACITY = 58549,
    SPELL_WINTERGRASP_TENACITY_VEHICLE = 59911,
    SPELL_WINTERGRASP_TOWER_CONTROL = 62064,
    SPELL_WINTERGRASP_SPIRITUAL_IMMUNITY = 58729,
    SPELL_WINTERGRASP_GREAT_HONOR = 58555,
    SPELL_WINTERGRASP_GREATER_HONOR = 58556,
    SPELL_WINTERGRASP_GREATEST_HONOR = 58557,
    SPELL_WINTERGRASP_ALLIANCE_FLAG = 14268,
    SPELL_WINTERGRASP_HORDE_FLAG = 14267,
    SPELL_WINTERGRASP_GRAB_PASSENGER = 61178,

    // Reward spells
    SPELL_WINTERGRASP_VICTORY_REWARD = 56902,
    SPELL_WINTERGRASP_DEFEAT_REWARD = 58494,
    SPELL_WINTERGRASP_DAMAGED_TOWER = 59135,
    SPELL_WINTERGRASP_DESTROYED_TOWER = 59136,
    SPELL_WINTERGRASP_DAMAGED_BUILDING = 59201,
    SPELL_WINTERGRASP_INTACT_BUILDING = 59203,

    // Other spells
    SPELL_WINTERGRASP_TELEPORT_BRIDGE = 59096,
    SPELL_WINTERGRASP_TELEPORT_FORTRESS = 60035,
    SPELL_WINTERGRASP_TELEPORT_DALARAN = 53360,
    SPELL_WINTERGRASP_VICTORY_AURA = 60044,
    SPELL_WINTERGRASP_WINTERGRASP_WATER = 36444,
    SPELL_WINTERGRASP_ESSENCE_OF_WINTERGRASP = 58045,
    SPELL_WINTERGRASP_ESSENCE_OF_WINTERGRASP_NORTHREND = 57940,
    SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA = 58730,
    SPELL_WINTERGRASP_BATTLEGROUND_DAMPENING = 74411,

    // Phasing spells
    SPELL_WINTERGRASP_HORDE_CONTROLS_FACTORY_PHASE_SHIFT = 56618, // ADDS PHASE 16
    SPELL_WINTERGRASP_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT = 56617, // ADDS PHASE 32
    SPELL_WINTERGRASP_HORDE_CONTROL_PHASE_SHIFT = 55773, // ADDS PHASE 64
    SPELL_WINTERGRASP_ALLIANCE_CONTROL_PHASE_SHIFT = 55774  // ADDS PHASE 128
};

enum WintergraspWorldstates
{
    WORLDSTATE_WINTERGRASP_FORTRESS_GATE = 3763,
    WORLDSTATE_WINTERGRASP_FORTRESS_VAULT_GATE = 3773,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_1 = 3749,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_2 = 3750,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_3 = 3753,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_4 = 3758,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_5 = 3754,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_6 = 3757,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_7 = 3755,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_8 = 3756,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_9 = 3764,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_10 = 3772,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_11 = 3762,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_12 = 3766,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_13 = 3770,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_14 = 3751,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_15 = 3752,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_16 = 3767,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_17 = 3769,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_18 = 3759,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_19 = 3760,
    WORLDSTATE_WINTERGRASP_FORTRESS_WALL_20 = 3761,
    WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_1 = 3765,
    WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_2 = 3771,
    WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_3 = 3768,

    WORLDSTATE_WINTERGRASP_WORKSHOP_NE = 3701,
    WORLDSTATE_WINTERGRASP_WORKSHOP_NW = 3700,
    WORLDSTATE_WINTERGRASP_WORKSHOP_SE = 3703,
    WORLDSTATE_WINTERGRASP_WORKSHOP_SW = 3702,
    WORLDSTATE_WINTERGRASP_WORKSHOP_KEEP_W = 3698,
    WORLDSTATE_WINTERGRASP_WORKSHOP_KEEP_E = 3699,

    WORLDSTATE_WINTERGRASP_VEHICLE_HORDE = 3490,
    WORLDSTATE_WINTERGRASP_MAX_VEHICLE_HORDE = 3491,
    WORLDSTATE_WINTERGRASP_VEHICLE_ALLIANCE = 3680,
    WORLDSTATE_WINTERGRASP_MAX_VEHICLE_ALLIANCE = 3681,

    WORLDSTATE_WINTERGRASP_DEFENDER_HORDE = 3802,
    WORLDSTATE_WINTERGRASP_DEFENDER_ALLIANCE = 3803,

    WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_HORDE = 4022,
    WORLDSTATE_WINTERGRASP_TIMES_ATTACKED_ALLIANCE = 4023,
    WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_HORDE = 4024,
    WORLDSTATE_WINTERGRASP_TIMES_DEFENDED_ALLIANCE = 4025,

    WORLDSTATE_WINTERGRASP_SHOW_WAR_TIMER = 3710,
    WORLDSTATE_WINTERGRASP_TIME_TO_END = 3781,
    WORLDSTATE_WINTERGRASP_SHOW_NOWAR_TIMER = 3801,
    WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE = 4354
};

enum WintergraspGraveyardIds : uint8
{
    GRAVEYARD_WORKSHOP_NE = 1,
    GRAVEYARD_WORKSHOP_NW,
    GRAVEYARD_WORKSHOP_SE,
    GRAVEYARD_WORKSHOP_SW,
    GRAVEYARD_KEEP,
    GRAVEYARD_HORDE,
    GRAVEYARD_ALLIANCE
};

enum WintergraspAreaIds
{
    AREA_WINTERGRASP_FORTRESS = 4575,
    AREA_THE_SUNKEN_RING = 4538,
    AREA_THE_BROKEN_TEMPLE = 4539,
    AREA_WESTPARK_WORKSHOP = 4611,
    AREA_EASTPARK_WORKSHOP = 4612,
    AREA_WINTERGRASP = 4197,
    AREA_THE_CHILLED_QUAGMIRE = 4589
};

enum WintergraspWorldSafeLocs
{
    WORLDSAFELOC_WINTERGRASP_FORTRESS_EAST = 1285,
    WORLDSAFELOC_WINTERGRASP_FORTRESS_WEST = 1328,
    WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NE = 1329,
    WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NW = 1330,
    WORLDSAFELOC_WINTERGRASP_HORDE_STARTING_AREA = 1331,
    WORLDSAFELOC_WINTERGRASP_ALLIANCE_STARTING_AREA = 1332,
    WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SE = 1333,
    WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SW = 1334,
    WORLDSAFELOC_WINTERGRASP_FORTRESS_GRAVEYARD_INDOORS = 1474
};

class Creature;
class GameObject;
class WorldObject;

class WintergraspBuilding : public BattlefieldBuilding
{
public:
    explicit WintergraspBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info);
    ~WintergraspBuilding() { }

    void OnObjectCreate(WorldObject* object) override;
    void OnObjectRemove(WorldObject* object) override;
};

class WintergraspCapturePoint : public BattlefieldCapturePoint
{
public:
    explicit WintergraspCapturePoint(Battlefield* battlefield, BattlefieldEntityInfo const info);
    ~WintergraspCapturePoint() { }

    void OnObjectCreate(WorldObject* object) override;
    void OnObjectRemove(WorldObject* object) override;
};

class WintergraspGraveyard : public BattlefieldGraveyard
{
public:
    explicit WintergraspGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info);
    ~WintergraspGraveyard() { }

    void OnObjectCreate(WorldObject* object) override;
    void OnObjectRemove(WorldObject* object) override;
};

class TC_GAME_API BattlefieldWintergrasp : public Battlefield
{
public:
    explicit BattlefieldWintergrasp();
    ~BattlefieldWintergrasp();

    void OnCreatureCreate(Creature* object) override;
    void OnCreatureRemove(Creature* object) override;
    void OnGameObjectCreate(GameObject* object) override;
    void OnGameObjectRemove(GameObject* object) override;
    void SendGlobalWorldStates(Player const* player) const override;

    bool IsFlyingMountAllowed() const override { return !IsEnabled() || (IsEnabled() && !IsWarTime()); }
    bool IsSpellAreaAllowed(uint32 spellId, Player const* player, uint32 newArea) const override;

private:
    typedef std::unique_ptr<WintergraspBuilding> WintergraspBuildingPointer;
    typedef std::unordered_map<uint32 /*entry*/, WintergraspBuildingPointer> WintergraspBuildingContainer;
    typedef std::unique_ptr<WintergraspCapturePoint> WintergraspCapturePointPointer;
    typedef std::unordered_map<uint32 /*entry*/, WintergraspCapturePointPointer> WintergraspCapturePointContainer;
    typedef std::unique_ptr<WintergraspGraveyard> WintergraspGraveyardPointer;

    BattlefieldWintergrasp(BattlefieldWintergrasp const&) = delete;
    BattlefieldWintergrasp& operator=(BattlefieldWintergrasp const&) = delete;

    uint8 GetWintergraspGraveyardId(Creature* creature) const;

    WintergraspBuildingContainer _buildings;
};

#endif
