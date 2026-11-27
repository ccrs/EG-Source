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

#include "ScriptMgr.h"
#include "CommonHelpers.h"
#include "GameObject.h"
#include "Map.h"
#include "MotionMaster.h"
#include "MovementTypedefs.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "StringFormat.h"
#include "TaskScheduler.h"
#include "TemporarySummon.h"
#include "WaypointManager.h"
#include "WorldStatePackets.h"
#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

enum BattleForTheUndercityMisc
{
    MAP_EASTERN_KINGDOMS = 0,
    ZONE_TIRISFAL_GLADES = 85,
    ZONE_UNDERCITY = 1497,
    PHASE_HORDE = 64,
    PHASE_ALLIANCE = 128,

    QUEST_BATTLE_FOR_THE_UNDERCITY_HORDE = 13267,
    QUEST_FATE_UP_AGAINST_YOUR_WILL = 13369,

    NPC_THRALL = 31650,
    NPC_SYLVANAS = 31651,
    NPC_VARIMATHRAS = 31565,
    NPC_BLIGHT_ABERRATION = 31844,
    NPC_KHANOK = 32160,
    NPC_LEGION_WORLD_PORTAL = 32232,
    NPC_PERFIDIOUS_DREADLORD = 32391,
    NPC_INVISIBLE_STALKER_TESLA = 31653,
    NPC_CAVE_IN_DUMMY = 32200,
    NPC_DISTANT_VOICE = 32277,
    NPC_VARIAN_WRYNN_HORDE_RUN = 32303,
    NPC_JAINA_PROUDMOORE_HORDE_RUN = 31418,
    NPC_STORMWIND_ELITE_HORDE_RUN = 31639,
    NPC_SAURFANG = 32315,
    NPC_VARIAN_WRYNN = 32401,
    NPC_JAINA_PROUDMOORE = 32402,
    NPC_ENGORGED_BLIGHT_WORM = 32483,
    NPC_GRAND_APOTHECARY_PUTRESS = 31530,
    NPC_FAILED_EXPERIMENT = 32519,
    NPC_THRALL_THRONE = 32518,
    NPC_SYLVANAS_THRONE = 32365,
    NPC_WARSONG_BATTLEGUARD_THRONE = 32510,
    NPC_SYLVANAS_ORGRIMMAR = 31419,
    NPC_KORKRON_ELITE_ORGRIMMAR = 32367,

    GO_THRONE_ROOM_CAVE_IN_1 = 193628,
    GO_THRONE_ROOM_CAVE_IN_2 = 193629,

    SPELL_WARCHIEFS_BLESSING = 59647,
    SPELL_WARCHIEFS_BLESSING_PLAYER = 59641,
    SPELL_WARCHIEFS_BLESSING_AURA = 64670,
    SPELL_HEROIC_VANGUARD = 59506,
    SPELL_CALL_OF_AIR = 59898,
    SPELL_CYCLONE_FALL = 59892,
    SPELL_TIDAL_WAVE = 59627,
    SPELL_LAVA_BURST = 59519,
    SPELL_CHAIN_LIGHTNING = 59517,
    SPELL_THUNDER = 59507,
    SPELL_FORCE_OF_NATURE = 59537,
    SPELL_CALL_OF_EARTH = 60207,
    SPELL_SHOOT = 20463,
    SPELL_SHRIEK_OF_THE_HIGHBORNE = 59514,
    SPELL_MULTI_SHOT = 59515,
    SPELL_HEARTSTRIKE = 59790,
    SPELL_ASSASSINATE_BOARD = 60045,
    SPELL_ASSASSINATE_KILL = 60046,
    SPELL_ASCENSION = 60461,
    SPELL_SONG_OF_SYLVANAS = 59756,
    SPELL_TELEPORT_VISUAL_ONLY = 41232,
    SPELL_TELEPORT_VISUAL_EVIL = 52233,
    SPELL_LEGION_PORTAL = 59680,
    SPELL_AURA_OF_VARIMATHRAS = 60289,
    SPELL_MIGHT_OF_VARIMATHRAS = 59424,
    SPELL_SHADOW_BOLT_VOLLEY = 20741,
    SPELL_DRAIN_LIFE = 17238,
    SPELL_CARRION_SWARM = 59434,
    SPELL_SHADOW_NOVA = 59435,
    SPELL_OPENING_LEGION_PORTALS = 60298,
    SPELL_PORTAL_COLLAPSE = 60285,
    SPELL_HALL_BLOWN_UP = 60101,
    SPELL_ROLL_PUTRESS_HEAD = 60496,
    SPELL_SEVERED_HEAD_OF_PUTRESS = 60498,
    SPELL_DEEP_FREEZE_STUN = 60511,
    SPELL_GREATER_MASS_TELEPORT_DUMMY = 60516,
    SPELL_WHIRLWIND = 41056,
    SPELL_STRENGTH_OF_WRYNN = 60964,
    SPELL_HEROIC_VANGUARD_WRYNN = 60966,
    SPELL_IMPROVED_BRILLIANCE_AURA = 61011,
    SPELL_FIREBALL = 20692,
    SPELL_FIREBALL_FAST = 20678,
    SPELL_FIRE_BLAST = 20679,
    SPELL_BLIZZARD = 20680,
    SPELL_SUMMON_WATER_ELEMENTALS = 61128,
    SPELL_DEEP_FREEZE_CREDIT = 61224,
    SPELL_GREATER_MASS_TELEPORT = 61225,
    SPELL_PUTRESS_CASTING_STATE = 59447,
    SPELL_PUTRESS_TESLA_AURA = 59457,
    SPELL_BLIGHT_OVERLOAD = 61181,
    SPELL_THROW_BLIGHT_BARREL = 59460,
    SPELL_UNHOLY_FRENZY = 60300,
    SPELL_WARCHIEFS_BLESSING_THRONE = 61212,
    SPELL_BLIGHT_EXPLOSION = 61126,
    SPELL_BLIGHT_EMPOWERMENT = 59449,
    SPELL_AIR_REVENANT_ENTRANCE = 55761,

    EVENT_CALL_OF_AIR = 20512,

    SPAWN_GROUP_HORDE_COURTYARD = 1029,
    SPAWN_GROUP_HORDE_LOWER_CITY = 1030,
    SPAWN_GROUP_HORDE_CYCLONES = 1031,
    SPAWN_GROUP_HORDE_INNER_SANCTUM = 1032,
    SPAWN_GROUP_HORDE_ROYAL_QUARTER = 1033,
    SPAWN_GROUP_ALLIANCE_ENTRANCE = 1034,
    SPAWN_GROUP_ALLIANCE_SEWERS = 1035,
    SPAWN_GROUP_ALLIANCE_APOTHECARIUM = 1036,
    SPAWN_GROUP_ALLIANCE_THRONE = 1037,
    SPAWN_GROUP_HORDE_COURTYARD_BANNERS = 1038,
    SPAWN_GROUP_HORDE_THRONE_PORTAL = 1039,
    SPAWN_GROUP_HORDE_INNER_SANCTUM_BANNERS = 1040,

    SUMMON_GROUP_COURTYARD_WAVE_FIRST = 0,
    SUMMON_GROUP_COURTYARD_WAVE_LAST = 22,
    SUMMON_GROUP_INNER_SANCTUM_WAVE_FIRST = 23,
    SUMMON_GROUP_INNER_SANCTUM_WAVE_LAST = 77,
    SUMMON_GROUP_ROYAL_QUARTER_WAVE_FIRST = 78,
    SUMMON_GROUP_ROYAL_QUARTER_WAVE_LAST = 91,
    SUMMON_GROUP_BLIGHT_ABERRATION = 92,
    SUMMON_GROUP_KHANOK = 93,
    SUMMON_GROUP_VARIMATHRAS_COURTYARD = 94,
    SUMMON_GROUP_VARIMATHRAS_INNER_SANCTUM = 95,
    SUMMON_GROUP_VARIMATHRAS_HALL = 96,
    SUMMON_GROUP_VARIMATHRAS_ROYAL_QUARTER = 97,
    SUMMON_GROUP_VARIAN_WRYNN_HORDE_RUN = 98,
    SUMMON_GROUP_JAINA_PROUDMOORE_HORDE_RUN = 99,
    SUMMON_GROUP_STORMWIND_ELITE_FIRST = 100,
    SUMMON_GROUP_STORMWIND_ELITE_LAST = 108,
    SUMMON_GROUP_SAURFANG = 109,
    SUMMON_GROUP_DISTANT_VOICE = 110,
    SUMMON_GROUP_GREAT_WIND_VORTEX_1 = 111,
    SUMMON_GROUP_GREAT_WIND_VORTEX_2 = 112,
    SUMMON_GROUP_BATTLEGUARD_MARCH_FIRST = 113,
    SUMMON_GROUP_BATTLEGUARD_MARCH_LAST = 122,
    SUMMON_GROUP_DEATHGUARD_ELITE_1 = 123,
    SUMMON_GROUP_DEATHGUARD_ELITE_2 = 124,
    SUMMON_GROUP_DEATHGUARD_ELITE_3 = 125,
    SUMMON_GROUP_DEATHGUARD_ELITE_4 = 126,
    SUMMON_GROUP_DEATHGUARD_ELITE_5 = 127,
    SUMMON_GROUP_DEATHGUARD_ELITE_6 = 128,
    SUMMON_GROUP_BATTLEGUARD_REINFORCEMENT_1 = 129,
    SUMMON_GROUP_BATTLEGUARD_REINFORCEMENT_2 = 130,

    SUMMON_GROUP_SEWER_POOL_WAVE_FIRST = 0,
    SUMMON_GROUP_SEWER_POOL_WAVE_LAST = 27,
    SUMMON_GROUP_APOTHECARIUM_WAVE_FIRST = 28,
    SUMMON_GROUP_APOTHECARIUM_WAVE_LAST = 65,
    SUMMON_GROUP_ENGORGED_BLIGHT_WORM = 66,
    SUMMON_GROUP_STORMWIND_ELITES = 67,
    SUMMON_GROUP_GRAND_APOTHECARY_PUTRESS = 68,
    SUMMON_GROUP_THRONE_HORDE = 69,
    SUMMON_GROUP_PERFIDIOUS_DREADLORDS = 70,

    SUMMON_GROUP_JAINA_ORGRIMMAR = 0,

    PATH_THRALL = 3165000,
    PATH_THRALL_DESCENT = 3165001,
    PATH_SYLVANAS = 3165100,
    PATH_GREAT_WIND_VORTEX_1 = 3178200,
    PATH_GREAT_WIND_VORTEX_2 = 3178201,
    PATH_VARIAN_WRYNN_HORDE_RUN = 3230300,
    PATH_JAINA_PROUDMOORE_HORDE_RUN = 3141800,
    PATH_SAURFANG = 3231500,
    PATH_BATTLEGUARD_MARCH = 3173900,
    PATH_BATTLEGUARD_COURTYARD = 3173910,
    PATH_BATTLEGUARD_REINFORCEMENT = 3173920,
    PATH_STORMWIND_ELITE_FORMATION = 3163900,
    PATH_VARIMATHRAS_RETREAT = 3156500,
    PATH_VARIAN_WRYNN = 3240100,
    PATH_JAINA_PROUDMOORE = 3240200,
    PATH_JAINA_PROUDMOORE_ORGRIMMAR = 3236400,
    PATH_SYLVANAS_ORGRIMMAR = 3141900,

    POINT_THRALL_GATE = 3,
    POINT_THRALL_COURTYARD_ENTRANCE = 10,
    POINT_THRALL_COURTYARD = 14,
    POINT_THRALL_COURTYARD_SECURED = 16,
    POINT_THRALL_ELEVATOR = 34,
    POINT_THRALL_LOWER_CITY = 38,
    POINT_THRALL_INNER_SANCTUM = 57,
    POINT_THRALL_ROYAL_QUARTER_APPROACH = 63,
    POINT_THRALL_COLLAPSED_HALL = 65,
    POINT_THRALL_THRONE_ROOM = 74,
    POINT_THRALL_SYLVANAS_PLATFORM = 75,
    POINT_THRALL_ALLIANCE_HORNS = 77,
    POINT_THRALL_ALLIANCE_INTRUSION = 78,
    POINT_THRALL_LAMENT = 79,

    POINT_SYLVANAS_GATE = 3,
    POINT_SYLVANAS_COURTYARD_ENTRANCE = 9,
    POINT_SYLVANAS_COURTYARD = 10,
    POINT_SYLVANAS_ELEVATOR = 29,
    POINT_SYLVANAS_SHAFT = 30,
    POINT_SYLVANAS_LOWER_CITY = 34,
    POINT_SYLVANAS_INNER_SANCTUM = 51,
    POINT_SYLVANAS_ROYAL_QUARTER_APPROACH = 68,
    POINT_SYLVANAS_COLLAPSED_HALL = 71,
    POINT_SYLVANAS_THRONE_ROOM = 89,
    POINT_SYLVANAS_PLATFORM = 90,

    POINT_VARIAN_SEWERS_ENTRANCE = 4,
    POINT_VARIAN_SEWER_POOL = 17,
    POINT_VARIAN_ENGORGED_BLIGHT_WORM = 18,
    POINT_VARIAN_CANALS = 29,
    POINT_VARIAN_KHANOK = 30,
    POINT_VARIAN_MAIN_CHAMBER = 35,
    POINT_VARIAN_DREADLORDS = 37,
    POINT_VARIAN_THERE = 44,
    POINT_VARIAN_NOWHERE_TO_RUN = 45,
    POINT_VARIAN_PUTRESS = 46,
    POINT_VARIAN_WHAT_SAY_YOU_NOW = 47,
    POINT_VARIAN_LOOK_AROUND = 48,
    POINT_VARIAN_THRONE_HALL = 73,

    POINT_JAINA_SEWERS_ENTRANCE = 4,
    POINT_JAINA_SEWER_POOL = 21,
    POINT_JAINA_CANALS = 33,
    POINT_JAINA_MAIN_CHAMBER = 39,
    POINT_JAINA_PUTRESS = 50,
    POINT_JAINA_APOTHECARIUM = 51,

    POINT_ANCHOR = 1,
    POINT_DESCENT,
    POINT_PORTAL,

    SAY_THRALL_HEROES = 0,
    SAY_THRALL_GATHER,
    SAY_THRALL_BLOOD_AND_THUNDER,
    SAY_THRALL_GREAT_CONFLICT,
    SAY_THRALL_GREAT_SPIRITS,
    SAY_THRALL_SONG_OF_WAR,
    SAY_THRALL_BATTLE_BEGINS,
    SAY_THRALL_ONWARD,
    SAY_THRALL_CALL_OF_AIR,
    SAY_THRALL_ANSWER_TO_THE_ELEMENTS,
    SAY_THRALL_TIDAL_WAVE,
    SAY_THRALL_ATTACK,
    SAY_THRALL_COURTYARD_SECURED,
    SAY_THRALL_ELEVATOR_DESTROYED,
    SAY_THRALL_AIR_SPIRIT,
    SAY_THRALL_CYCLONES,
    SAY_THRALL_INNER_SANCTUM_BREACHED,
    SAY_THRALL_LEAD_THE_WAY,
    SAY_THRALL_IS_THAT_ALL,
    SAY_THRALL_WE_COME_FOR_YOU,
    SAY_THRALL_COWARD,
    SAY_THRALL_EARTH_SPIRITS,
    SAY_THRALL_LETS_GO_QUICKLY,
    SAY_THRALL_WHAT_IS_THIS,
    SAY_THRALL_VICTORIOUS,
    SAY_THRALL_LOKTAR,
    SAY_THRALL_JOIN_ME,
    SAY_THRALL_WE_SHALL,
    SAY_THRALL_ALLIANCE_HORNS,
    SAY_THRALL_IT_ENDS,
    SAY_THRALL_ALL_IS_LOST,
    SAY_THRALL_IF_ONLY_YOU_WERE_HERE,
    SAY_THRALL_NODS,
    SAY_THRALL_GOOD_TO_HAVE_YOU_BACK,

    SAY_SYLVANAS_BEGINS_TO_SING = 0,
    SAY_SYLVANAS_SHAFT_TRAPPED,
    SAY_SYLVANAS_BEAUTIFUL_CITY,
    SAY_SYLVANAS_REDEMPTION,
    SAY_SYLVANAS_ROYAL_QUARTER_THIS_WAY,
    SAY_SYLVANAS_BARK,
    SAY_SYLVANAS_DARK_MAGIC,
    SAY_SYLVANAS_ROYAL_QUARTER_AHEAD,
    SAY_SYLVANAS_FATE_OF_ALL_ENEMIES,

    SAY_VARIMATHRAS_WELCOME = 0,
    SAY_VARIMATHRAS_MINION,
    SAY_VARIMATHRAS_ENOUGH,
    SAY_VARIMATHRAS_CLEVER_GIRL,
    SAY_VARIMATHRAS_BROTHERS_HUNGRY,
    SAY_VARIMATHRAS_BRING_DOWN_THE_HALLS,
    SAY_VARIMATHRAS_WELCOME_TO_YOUR_FUTURE,
    SAY_VARIMATHRAS_TOO_LONG,
    SAY_VARIMATHRAS_NEED_MORE_TIME,
    SAY_VARIMATHRAS_SUCH_POWER,
    SAY_VARIMATHRAS_WILL_NOT_FAIL,
    SAY_VARIMATHRAS_CANNOT_HOLD,
    SAY_VARIMATHRAS_THOUSAND_PARDONS,
    SAY_VARIMATHRAS_YEARS_WASTED,

    SAY_BLIGHT_ABERRATION_RELEASED = 0,
    SAY_BLIGHT_ABERRATION_ROAR,

    SAY_KHANOK_APPEARS = 0,
    SAY_KHANOK_NO_HOPE,

    SAY_DISTANT_VOICE_FAILED = 0,

    SAY_BATTLEGUARD_FOR_THE_HORDE = 1,

    SAY_VARIAN_HORDE_RUN_AWAY_TOO_LONG = 0,
    SAY_VARIAN_HORDE_RUN_MAKE_THINGS_RIGHT,
    SAY_VARIAN_HORDE_RUN_WAITED_LONG,
    SAY_VARIAN_HORDE_RUN_IT_ENDS_NOW,
    SAY_VARIAN_HORDE_RUN_ATTACK,

    SAY_JAINA_HORDE_RUN_VARIAN_NO = 5,
    SAY_JAINA_HORDE_RUN_DID_NOT_HAVE_TO_BE,

    SAY_SAURFANG_WHAT_HE_WOULD_DO = 0,
    SAY_SAURFANG_LEAD_YOUR_PEOPLE,
    SAY_SAURFANG_LETS_GO_HOME,

    SAY_VARIAN_HEROES = 0,
    SAY_VARIAN_HIDDEN_INSIDE,
    SAY_VARIAN_SOON_WE_MARCH,
    SAY_VARIAN_HOUR_OF_REDEMPTION,
    SAY_VARIAN_MARCH_BEGINS,
    SAY_VARIAN_DESTINY_AWAITS,
    SAY_VARIAN_DESCENT_BEGINS,
    SAY_VARIAN_STAND_YOUR_GROUND,
    SAY_VARIAN_FALL_IN,
    SAY_VARIAN_ABERRATIONS,
    SAY_VARIAN_HORDE_IN_FORCE,
    SAY_VARIAN_STAY_ALERT,
    SAY_VARIAN_ONWARD,
    SAY_VARIAN_MAIN_CHAMBER,
    SAY_VARIAN_THERE,
    SAY_VARIAN_NOWHERE_TO_RUN,
    SAY_VARIAN_JUSTICE_SERVED,
    SAY_VARIAN_WHAT_SAY_YOU_NOW,
    SAY_VARIAN_LOOK_AROUND,
    SAY_VARIAN_HOW_MUCH_LONGER,
    SAY_VARIAN_HORDES_WORLD,
    SAY_VARIAN_EVIL_IN_ORCS,
    SAY_VARIAN_THRALL_HERE,
    SAY_VARIAN_END_THIS_NOW,
    SAY_VARIAN_LOKTAR_OGAR,
    SAY_VARIAN_TO_THE_THRONE_ROOM,
    SAY_VARIAN_AWAY_TOO_LONG,
    SAY_VARIAN_MAKE_THINGS_RIGHT,
    SAY_VARIAN_WAITED_LONG,
    SAY_VARIAN_IT_ENDS_NOW,
    SAY_VARIAN_ATTACK,

    SAY_JAINA_RIGHT_AWAY = 0,
    SAY_JAINA_CORPSES_EVERYWHERE,
    SAY_JAINA_WONT_HELP_YOU,
    SAY_JAINA_VARIAN_NO,
    SAY_JAINA_DID_NOT_HAVE_TO_BE,

    SAY_ENGORGED_BLIGHT_WORM_UNLEASHED = 0,

    SAY_PUTRESS_DO_NOT_LET_THEM_PASS = 0,
    SAY_PUTRESS_KILL_THEM,
    SAY_PUTRESS_STARE_INTO_THE_ABYSS,
    SAY_PUTRESS_NO_MATCH,
    SAY_PUTRESS_CANNOT_BE_HAPPENING,
    SAY_PUTRESS_WILL_NOT_END,
    SAY_PUTRESS_POWER_WASHES,
    SAY_PUTRESS_MORE_SUFFERING,

    SAY_THRALL_THRONE_VICTORIOUS = 0,
    SAY_THRALL_THRONE_LOKTAR,

    SAY_THRALL_ORGRIMMAR_STAND_DOWN = 0,
    SAY_THRALL_ORGRIMMAR_JAINA,
    SAY_THRALL_ORGRIMMAR_BETRAYAL,
    SAY_THRALL_ORGRIMMAR_UNDERCITY_LOST,
    SAY_THRALL_ORGRIMMAR_LAY_SIEGE,
    SAY_THRALL_ORGRIMMAR_DIVIDED_FORCES,
    SAY_THRALL_ORGRIMMAR_MAKE_THIS_RIGHT,
    SAY_THRALL_ORGRIMMAR_PREPARE_TRANSPORT,

    SAY_JAINA_ORGRIMMAR_WHAT_HAPPENED = 0,
    SAY_JAINA_ORGRIMMAR_DELIVER_INFORMATION,
    SAY_JAINA_ORGRIMMAR_BOLVAR,
    SAY_JAINA_ORGRIMMAR_RAGE,
    SAY_JAINA_ORGRIMMAR_FAREWELL,

    SAY_SYLVANAS_ORGRIMMAR_TRUTH = 0,
    SAY_SYLVANAS_ORGRIMMAR_UPRISING,

    GOSSIP_MENU_THRALL = 57026,
    GOSSIP_MENU_VARIAN = 10194,
    GOSSIP_OPTION_START = 0,
    NPC_TEXT_THRALL_READY = 14050,
    NPC_TEXT_THRALL_BUSY = 14051,
    NPC_TEXT_VARIAN_READY = 14140,
    NPC_TEXT_VARIAN_BUSY = 14142,

    WORLD_STATE_HORDE_BATTLE_BEGUN = 3875,
    WORLD_STATE_HORDE_COUNTDOWN = 3876,
    WORLD_STATE_HORDE_COUNTDOWN_MINUTES = 3877,
    WORLD_STATE_HORDE_ENEMY_CONTROLS_UNDERCITY = 3878,
    WORLD_STATE_HORDE_RUN_ACTIVE = 3879,
    WORLD_STATE_HORDE_COURTYARD_PROGRESS = 3881,
    WORLD_STATE_HORDE_INNER_SANCTUM_PROGRESS = 3882,
    WORLD_STATE_HORDE_ROYAL_QUARTER_PROGRESS = 3884,
    WORLD_STATE_HORDE_COURTYARD_FIGHT = 3885,
    WORLD_STATE_HORDE_COURTYARD_CONTROLLED = 3886,
    WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT = 3887,
    WORLD_STATE_HORDE_INNER_SANCTUM_CONTROLLED = 3888,
    WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT = 3889,
    WORLD_STATE_HORDE_ROYAL_QUARTER_CONTROLLED = 3890,
    WORLD_STATE_HORDE_RESET_MINUTES = 3893,
    WORLD_STATE_HORDE_RESET_COUNTDOWN = 3894,

    WORLD_STATE_ALLIANCE_COUNTDOWN_MINUTES = 3957,
    WORLD_STATE_ALLIANCE_COUNTDOWN = 3958,
    WORLD_STATE_ALLIANCE_SEWERS_PROGRESS = 3961,
    WORLD_STATE_ALLIANCE_SEWERS_FIGHT = 3962,
    WORLD_STATE_ALLIANCE_ENEMY_CONTROLS_UNDERCITY = 3963,
    WORLD_STATE_ALLIANCE_SEWERS_SECURED = 3964,
    WORLD_STATE_ALLIANCE_RUN_ACTIVE = 3965,
    WORLD_STATE_ALLIANCE_MANHUNT_BEGUN = 3966,
    WORLD_STATE_ALLIANCE_APOTHECARIUM_PROGRESS = 3969,
    WORLD_STATE_ALLIANCE_APOTHECARIUM_SECURED = 3971,
    WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT = 3972,

    DATA_BATTLE_STATE = 1,
    DATA_THRALL,
    DATA_SYLVANAS,
    DATA_VARIMATHRAS,
    DATA_VARIAN_WRYNN_HORDE_RUN,
    DATA_JAINA_PROUDMOORE_HORDE_RUN,
    DATA_SAURFANG,
    DATA_VARIAN_WRYNN,
    DATA_JAINA_PROUDMOORE,
    DATA_GRAND_APOTHECARY_PUTRESS,
    DATA_THRALL_THRONE,
    DATA_HALL_COLLAPSE,
    DATA_HALL_REOPEN,
    DATA_LEGION_PORTALS_OPEN,
    DATA_LEGION_PORTALS_COLLAPSE,
    DATA_RELEASE_POINT,
    DATA_THRONE_DEFENDERS,
    DATA_RESET_RUN,

    ACTION_COUNTDOWN = 1,
    ACTION_BATTLE_BEGUN,
    ACTION_COURTYARD_CONTROLLED,
    ACTION_INNER_SANCTUM_CONTROLLED,
    ACTION_ROYAL_QUARTER_CONTROLLED,
    ACTION_MANHUNT,
    ACTION_SEWERS_SECURED,
    ACTION_APOTHECARIUM_SECURED,
    ACTION_START_PATH,
    ACTION_SYLVANAS_SING,
    ACTION_SYLVANAS_DESCENT,
    ACTION_SYLVANAS_ASSASSINATE,
    ACTION_SYLVANAS_ASCENSION,
    ACTION_JAINA_BRILLIANCE,
    ACTION_JAINA_THRONE,
    ACTION_VARIMATHRAS_LEAVE,
    ACTION_VARIMATHRAS_RETREAT,
    ACTION_VARIMATHRAS_ATTACK,
    ACTION_PUTRESS_ATTACK,

    BATTLE_STATE_IDLE = 0,
    BATTLE_STATE_COUNTDOWN,
    BATTLE_STATE_BEGUN,
    BATTLE_STATE_COURTYARD_FIGHT,
    BATTLE_STATE_COURTYARD_CONTROLLED,
    BATTLE_STATE_INNER_SANCTUM_FIGHT,
    BATTLE_STATE_INNER_SANCTUM_CONTROLLED,
    BATTLE_STATE_ROYAL_QUARTER_FIGHT,
    BATTLE_STATE_ROYAL_QUARTER_CONTROLLED,
    BATTLE_STATE_SEWERS_FIGHT,
    BATTLE_STATE_SEWERS_SECURED,
    BATTLE_STATE_APOTHECARIUM_FIGHT,
    BATTLE_STATE_APOTHECARIUM_SECURED,

    GROUP_COUNTDOWN = 1,
    GROUP_EMPTY_RUN,
    GROUP_COMBAT
};

class EG_zone_undercity_battle : public WorldZoneScript
{
protected:
    EG_zone_undercity_battle(char const* name, uint32 phaseMask, std::map<uint32, uint32> idleWorldStates) : WorldZoneScript(name, MAP_EASTERN_KINGDOMS, { ZONE_TIRISFAL_GLADES, ZONE_UNDERCITY }, phaseMask), BattleState(BATTLE_STATE_IDLE), _idleWorldStates(idleWorldStates), _worldStates(std::move(idleWorldStates)), _cleanupPending(false) { }

public:
    void OnCreatureCreate(Creature* creature) override
    {
        if (TempSummon const* summon = creature->ToTempSummon())
            if (summon->GetSummonerGUID().IsCreatureOrVehicle())
                _summonGUIDs.insert(creature->GetGUID());
    }

    void OnCreatureRemove(Creature* creature) override
    {
        _summonGUIDs.erase(creature->GetGUID());
    }

    uint32 GetData(uint32 type) const override
    {
        return type == DATA_BATTLE_STATE ? BattleState : 0;
    }

    void OnPlayerEnter(Player* /*player*/, uint32 /*zoneId*/) override
    {
        Scheduler.CancelGroup(GROUP_EMPTY_RUN);
    }

    void OnPlayerLeave(Player* /*player*/, uint32 /*zoneId*/) override
    {
        if (BattleState == BATTLE_STATE_IDLE || !GetPlayers().empty())
            return;

        Scheduler.Schedule(10min, GROUP_EMPTY_RUN, [this](TaskContext /*context*/)
        {
            EndRun();
        });
    }

    void FillInitialWorldStates(Player* /*player*/, WorldPackets::WorldState::InitWorldStates& packet) override
    {
        for (auto const& [worldStateId, value] : _worldStates)
            packet.Worldstates.emplace_back(worldStateId, value);
    }

    void SendRemoveWorldStates(Player* player) override
    {
        for (auto const& worldState : _worldStates)
            player->SendUpdateWorldState(worldState.first, 0);
    }

    void OnUpdate(Map* map, uint32 diff) override
    {
        Scheduler.Update(diff);

        if (_cleanupPending)
        {
            _cleanupPending = false;

            GuidUnorderedSet const summonGUIDs = std::move(_summonGUIDs);
            _summonGUIDs.clear();
            for (ObjectGuid const& guid : summonGUIDs)
                if (Creature* summon = map->GetCreature(guid))
                    summon->DespawnOrUnsummon();

            OnRunEnd(map);
        }

        if (_changedWorldStates.empty())
            return;

        for (ObjectGuid const& guid : GetPlayers())
            if (Player* player = ObjectAccessor::GetPlayer(map, guid))
                for (uint32 worldStateId : _changedWorldStates)
                    player->SendUpdateWorldState(worldStateId, _worldStates[worldStateId]);

        _changedWorldStates.clear();
    }

protected:
    void SetWorldState(uint32 worldStateId, uint32 value)
    {
        auto itr = _worldStates.find(worldStateId);
        if (itr == _worldStates.end() || itr->second == value)
            return;

        itr->second = value;
        _changedWorldStates.insert(worldStateId);
    }

    void StartCountdown(uint32 minutesWorldStateId, std::function<void()> const& finish)
    {
        SetWorldState(minutesWorldStateId, 5);
        Scheduler.Schedule(62500ms, GROUP_COUNTDOWN, [this, minutesWorldStateId, finish](TaskContext context)
        {
            SetWorldState(minutesWorldStateId, _worldStates[minutesWorldStateId] - 1);
            if (_worldStates[minutesWorldStateId])
                context.Repeat();
            else
                finish();
        });
    }

    void EndRun()
    {
        BattleState = BATTLE_STATE_IDLE;
        Scheduler.CancelAll();
        _cleanupPending = true;

        for (auto const& [worldStateId, value] : _idleWorldStates)
            SetWorldState(worldStateId, value);
    }

    void ResetLeader(Map* map, ObjectGuid const& guid)
    {
        if (Creature* leader = map->GetCreature(guid))
            leader->DespawnOrUnsummon(0s, 5s);
    }

    virtual void OnRunEnd(Map* /*map*/) { }

    uint32 BattleState;
    TaskScheduler Scheduler;

private:
    std::map<uint32, uint32> const _idleWorldStates;
    std::map<uint32, uint32> _worldStates;
    std::set<uint32> _changedWorldStates;
    GuidUnorderedSet _summonGUIDs;
    bool _cleanupPending;
};

class EG_zone_undercity_battle_horde : public EG_zone_undercity_battle
{
public:
    EG_zone_undercity_battle_horde() : EG_zone_undercity_battle("EG_zone_undercity_battle_horde", PHASE_HORDE, { { WORLD_STATE_HORDE_BATTLE_BEGUN, 0 }, { WORLD_STATE_HORDE_COUNTDOWN, 0 }, { WORLD_STATE_HORDE_COUNTDOWN_MINUTES, 0 }, { WORLD_STATE_HORDE_ENEMY_CONTROLS_UNDERCITY, 1 }, { WORLD_STATE_HORDE_RUN_ACTIVE, 0 }, { WORLD_STATE_HORDE_COURTYARD_PROGRESS, 0 }, { WORLD_STATE_HORDE_INNER_SANCTUM_PROGRESS, 0 }, { WORLD_STATE_HORDE_ROYAL_QUARTER_PROGRESS, 0 }, { WORLD_STATE_HORDE_COURTYARD_FIGHT, 0 }, { WORLD_STATE_HORDE_COURTYARD_CONTROLLED, 0 }, { WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 0 }, { WORLD_STATE_HORDE_INNER_SANCTUM_CONTROLLED, 0 }, { WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT, 0 }, { WORLD_STATE_HORDE_ROYAL_QUARTER_CONTROLLED, 0 }, { WORLD_STATE_HORDE_RESET_MINUTES, 0 }, { WORLD_STATE_HORDE_RESET_COUNTDOWN, 0 } }), _runStartPending(false), _beginPending(false), _creditPending(false) { }

    void OnCreatureCreate(Creature* creature) override
    {
        EG_zone_undercity_battle::OnCreatureCreate(creature);

        switch (creature->GetEntry())
        {
            case NPC_THRALL:
                _thrallGUID = creature->GetGUID();
                break;
            case NPC_SYLVANAS:
                _sylvanasGUID = creature->GetGUID();
                break;
            case NPC_VARIMATHRAS:
                _varimathrasGUID = creature->GetGUID();
                break;
            case NPC_VARIAN_WRYNN_HORDE_RUN:
                _varianGUID = creature->GetGUID();
                break;
            case NPC_JAINA_PROUDMOORE_HORDE_RUN:
                _jainaGUID = creature->GetGUID();
                break;
            case NPC_SAURFANG:
                _saurfangGUID = creature->GetGUID();
                break;
            case NPC_LEGION_WORLD_PORTAL:
                _legionPortalGUIDs.insert(creature->GetGUID());
                break;
            case NPC_CAVE_IN_DUMMY:
                _caveInDummyGUIDs.insert(creature->GetGUID());
                break;
            default:
                break;
        }
    }

    void OnCreatureRemove(Creature* creature) override
    {
        EG_zone_undercity_battle::OnCreatureRemove(creature);

        ObjectGuid const guid = creature->GetGUID();
        _legionPortalGUIDs.erase(guid);
        _caveInDummyGUIDs.erase(guid);
        if (guid == _thrallGUID)
        {
            _thrallGUID.Clear();
            if (BattleState != BATTLE_STATE_IDLE)
                EndRun();
        }
        else if (guid == _sylvanasGUID)
            _sylvanasGUID.Clear();
        else if (guid == _varimathrasGUID)
            _varimathrasGUID.Clear();
        else if (guid == _varianGUID)
            _varianGUID.Clear();
        else if (guid == _jainaGUID)
            _jainaGUID.Clear();
        else if (guid == _saurfangGUID)
            _saurfangGUID.Clear();
    }

    void OnGameObjectCreate(GameObject* go) override
    {
        if (go->GetEntry() == GO_THRONE_ROOM_CAVE_IN_1 || go->GetEntry() == GO_THRONE_ROOM_CAVE_IN_2)
            _caveInGUIDs.insert(go->GetGUID());
    }

    void OnGameObjectRemove(GameObject* go) override
    {
        _caveInGUIDs.erase(go->GetGUID());
    }

    void OnUnitDeath(Unit* unit) override
    {
        switch (unit->GetEntry())
        {
            case NPC_THRALL:
                if (BattleState != BATTLE_STATE_IDLE)
                    EndRun();
                break;
            case NPC_BLIGHT_ABERRATION:
                if (BattleState != BATTLE_STATE_COURTYARD_FIGHT)
                    break;

                SetData(DATA_BATTLE_STATE, BATTLE_STATE_COURTYARD_CONTROLLED);
                unit->GetMap()->SpawnGroupSpawn(SPAWN_GROUP_HORDE_COURTYARD_BANNERS, true);
                if (Creature* thrall = unit->GetMap()->GetCreature(_thrallGUID))
                    thrall->AI()->DoAction(ACTION_COURTYARD_CONTROLLED);
                break;
            case NPC_KHANOK:
                if (BattleState != BATTLE_STATE_INNER_SANCTUM_FIGHT)
                    break;

                SetData(DATA_BATTLE_STATE, BATTLE_STATE_INNER_SANCTUM_CONTROLLED);
                unit->GetMap()->SpawnGroupSpawn(SPAWN_GROUP_HORDE_INNER_SANCTUM_BANNERS, true);
                if (Creature* thrall = unit->GetMap()->GetCreature(_thrallGUID))
                    thrall->AI()->DoAction(ACTION_INNER_SANCTUM_CONTROLLED);
                break;
            case NPC_VARIMATHRAS:
                if (BattleState != BATTLE_STATE_ROYAL_QUARTER_FIGHT)
                    break;

                SetData(DATA_BATTLE_STATE, BATTLE_STATE_ROYAL_QUARTER_CONTROLLED);
                if (Creature* thrall = unit->GetMap()->GetCreature(_thrallGUID))
                    thrall->AI()->DoAction(ACTION_ROYAL_QUARTER_CONTROLLED);
                break;
            default:
                break;
        }
    }

    ObjectGuid GetGuidData(uint32 type) const override
    {
        switch (type)
        {
            case DATA_THRALL:
                return _thrallGUID;
            case DATA_SYLVANAS:
                return _sylvanasGUID;
            case DATA_VARIMATHRAS:
                return _varimathrasGUID;
            case DATA_VARIAN_WRYNN_HORDE_RUN:
                return _varianGUID;
            case DATA_JAINA_PROUDMOORE_HORDE_RUN:
                return _jainaGUID;
            case DATA_SAURFANG:
                return _saurfangGUID;
            default:
                return ObjectGuid::Empty;
        }
    }

    void ProcessEvent(WorldObject* obj, uint32 eventId, WorldObject* /*invoker*/) override
    {
        if (eventId != EVENT_CALL_OF_AIR || !obj || obj->GetGUID() != _thrallGUID || BattleState != BATTLE_STATE_COURTYARD_CONTROLLED)
            return;

        obj->GetMap()->SpawnGroupSpawn(SPAWN_GROUP_HORDE_CYCLONES, true);
        std::list<Player*> players;
        obj->GetPlayerListInGrid(players, 50.0f);
        for (Player* player : players)
            if (HasPlayer(player->GetGUID()))
                player->CastSpell(player, SPELL_CYCLONE_FALL, true);
    }

    void SetData(uint32 type, uint32 value) override
    {
        switch (type)
        {
            case DATA_HALL_COLLAPSE:
            case DATA_HALL_REOPEN:
            case DATA_LEGION_PORTALS_OPEN:
            case DATA_LEGION_PORTALS_COLLAPSE:
                _pendingEvents.emplace_back(type, value);
                return;
            case DATA_BATTLE_STATE:
                break;
            default:
                return;
        }

        switch (value)
        {
            case BATTLE_STATE_IDLE:
                if (BattleState != BATTLE_STATE_IDLE)
                    EndRun();
                break;
            case BATTLE_STATE_COUNTDOWN:
                if (BattleState != BATTLE_STATE_IDLE)
                    break;

                BattleState = BATTLE_STATE_COUNTDOWN;
                _runStartPending = true;
                SetWorldState(WORLD_STATE_HORDE_RUN_ACTIVE, 1);
                SetWorldState(WORLD_STATE_HORDE_COUNTDOWN, 1);
                StartCountdown(WORLD_STATE_HORDE_COUNTDOWN_MINUTES, [this]()
                {
                    BattleState = BATTLE_STATE_BEGUN;
                    _beginPending = true;
                    SetWorldState(WORLD_STATE_HORDE_COUNTDOWN, 0);
                    SetWorldState(WORLD_STATE_HORDE_BATTLE_BEGUN, 1);
                });
                break;
            case BATTLE_STATE_COURTYARD_FIGHT:
                if (BattleState != BATTLE_STATE_BEGUN)
                    break;

                BattleState = BATTLE_STATE_COURTYARD_FIGHT;
                SetWorldState(WORLD_STATE_HORDE_BATTLE_BEGUN, 0);
                SetWorldState(WORLD_STATE_HORDE_ENEMY_CONTROLS_UNDERCITY, 0);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_FIGHT, 1);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_PROGRESS, 1);
                break;
            case BATTLE_STATE_COURTYARD_CONTROLLED:
                if (BattleState != BATTLE_STATE_COURTYARD_FIGHT)
                    break;

                BattleState = BATTLE_STATE_COURTYARD_CONTROLLED;
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_FIGHT, 0);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_CONTROLLED, 1);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_PROGRESS, 2);
                break;
            case BATTLE_STATE_INNER_SANCTUM_FIGHT:
                if (BattleState != BATTLE_STATE_COURTYARD_CONTROLLED)
                    break;

                BattleState = BATTLE_STATE_INNER_SANCTUM_FIGHT;
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 1);
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_PROGRESS, 1);
                break;
            case BATTLE_STATE_INNER_SANCTUM_CONTROLLED:
                if (BattleState != BATTLE_STATE_INNER_SANCTUM_FIGHT)
                    break;

                BattleState = BATTLE_STATE_INNER_SANCTUM_CONTROLLED;
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 0);
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_CONTROLLED, 1);
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_PROGRESS, 2);
                break;
            case BATTLE_STATE_ROYAL_QUARTER_FIGHT:
                if (BattleState != BATTLE_STATE_INNER_SANCTUM_CONTROLLED)
                    break;

                BattleState = BATTLE_STATE_ROYAL_QUARTER_FIGHT;
                SetWorldState(WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT, 1);
                SetWorldState(WORLD_STATE_HORDE_ROYAL_QUARTER_PROGRESS, 1);
                break;
            case BATTLE_STATE_ROYAL_QUARTER_CONTROLLED:
                if (BattleState != BATTLE_STATE_ROYAL_QUARTER_FIGHT)
                    break;

                BattleState = BATTLE_STATE_ROYAL_QUARTER_CONTROLLED;
                SetWorldState(WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT, 0);
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_CONTROLLED, 0);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_CONTROLLED, 0);
                SetWorldState(WORLD_STATE_HORDE_ROYAL_QUARTER_CONTROLLED, 1);
                SetWorldState(WORLD_STATE_HORDE_ROYAL_QUARTER_PROGRESS, 2);
                SetWorldState(WORLD_STATE_HORDE_RESET_COUNTDOWN, 1);
                _creditPending = true;
                StartCountdown(WORLD_STATE_HORDE_RESET_MINUTES, [this]()
                {
                    EndRun();
                });
                break;
            default:
                break;
        }
    }

    void OnUpdate(Map* map, uint32 diff) override
    {
        EG_zone_undercity_battle::OnUpdate(map, diff);

        if (_runStartPending)
        {
            _runStartPending = false;
            for (uint32 groupId : { SPAWN_GROUP_HORDE_COURTYARD, SPAWN_GROUP_HORDE_LOWER_CITY, SPAWN_GROUP_HORDE_INNER_SANCTUM, SPAWN_GROUP_HORDE_ROYAL_QUARTER })
                map->SpawnGroupSpawn(groupId, true);
            if (Creature* thrall = map->GetCreature(_thrallGUID))
                thrall->AI()->DoAction(ACTION_COUNTDOWN);
        }

        if (_beginPending)
        {
            _beginPending = false;
            if (Creature* thrall = map->GetCreature(_thrallGUID))
                thrall->AI()->DoAction(ACTION_BATTLE_BEGUN);
        }

        std::vector<std::pair<uint32, uint32>> const pendingEvents = std::move(_pendingEvents);
        _pendingEvents.clear();
        for (auto const& [event, value] : pendingEvents)
        {
            switch (event)
            {
                case DATA_HALL_COLLAPSE:
                    for (ObjectGuid const& guid : _caveInDummyGUIDs)
                        if (Creature* dummy = map->GetCreature(guid))
                            dummy->CastSpell(dummy, SPELL_HALL_BLOWN_UP);
                    break;
                case DATA_HALL_REOPEN:
                    for (ObjectGuid const& guid : _caveInGUIDs)
                        if (GameObject* caveIn = map->GetGameObject(guid))
                            caveIn->ResetDoorOrButton();
                    break;
                case DATA_LEGION_PORTALS_OPEN:
                    for (ObjectGuid const& guid : _legionPortalGUIDs)
                        if (Creature* portal = map->GetCreature(guid))
                            portal->CastSpell(nullptr, SPELL_OPENING_LEGION_PORTALS);
                    break;
                case DATA_LEGION_PORTALS_COLLAPSE:
                    for (ObjectGuid const& guid : _legionPortalGUIDs)
                        if (Creature* portal = map->GetCreature(guid))
                            if (portal->HasStringId(Trinity::StringFormat("undercity_legion_portal_{}", value)))
                                portal->CastSpell(nullptr, SPELL_PORTAL_COLLAPSE);
                    break;
                default:
                    break;
            }
        }

        if (!_creditPending)
            return;

        _creditPending = false;
        for (ObjectGuid const& guid : GetPlayers())
            if (Player* player = ObjectAccessor::GetPlayer(map, guid))
                if (player->GetZoneId() == ZONE_UNDERCITY && player->GetQuestStatus(QUEST_BATTLE_FOR_THE_UNDERCITY_HORDE) == QUEST_STATUS_INCOMPLETE)
                    player->AreaExploredOrEventHappens(QUEST_BATTLE_FOR_THE_UNDERCITY_HORDE);
    }

private:
    void OnRunEnd(Map* map) override
    {
        _runStartPending = false;
        _beginPending = false;
        _creditPending = false;
        _pendingEvents.clear();
        for (uint32 groupId : { SPAWN_GROUP_HORDE_COURTYARD, SPAWN_GROUP_HORDE_LOWER_CITY, SPAWN_GROUP_HORDE_CYCLONES, SPAWN_GROUP_HORDE_INNER_SANCTUM, SPAWN_GROUP_HORDE_ROYAL_QUARTER, SPAWN_GROUP_HORDE_COURTYARD_BANNERS, SPAWN_GROUP_HORDE_THRONE_PORTAL, SPAWN_GROUP_HORDE_INNER_SANCTUM_BANNERS })
            map->SpawnGroupDespawn(groupId);
        for (ObjectGuid const& guid : _caveInGUIDs)
            if (GameObject* caveIn = map->GetGameObject(guid))
                caveIn->ResetDoorOrButton();
        ResetLeader(map, _thrallGUID);
        ResetLeader(map, _sylvanasGUID);
    }

    ObjectGuid _thrallGUID;
    ObjectGuid _sylvanasGUID;
    ObjectGuid _varimathrasGUID;
    ObjectGuid _varianGUID;
    ObjectGuid _jainaGUID;
    ObjectGuid _saurfangGUID;
    GuidUnorderedSet _legionPortalGUIDs;
    GuidUnorderedSet _caveInDummyGUIDs;
    GuidUnorderedSet _caveInGUIDs;
    std::vector<std::pair<uint32, uint32>> _pendingEvents;
    bool _runStartPending;
    bool _beginPending;
    bool _creditPending;
};

class EG_zone_undercity_battle_alliance : public EG_zone_undercity_battle
{
public:
    EG_zone_undercity_battle_alliance() : EG_zone_undercity_battle("EG_zone_undercity_battle_alliance", PHASE_ALLIANCE, { { WORLD_STATE_ALLIANCE_COUNTDOWN_MINUTES, 0 }, { WORLD_STATE_ALLIANCE_COUNTDOWN, 0 }, { WORLD_STATE_ALLIANCE_SEWERS_PROGRESS, 0 }, { WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 0 }, { WORLD_STATE_ALLIANCE_ENEMY_CONTROLS_UNDERCITY, 1 }, { WORLD_STATE_ALLIANCE_SEWERS_SECURED, 0 }, { WORLD_STATE_ALLIANCE_RUN_ACTIVE, 0 }, { WORLD_STATE_ALLIANCE_MANHUNT_BEGUN, 0 }, { WORLD_STATE_ALLIANCE_APOTHECARIUM_PROGRESS, 0 }, { WORLD_STATE_ALLIANCE_APOTHECARIUM_SECURED, 0 }, { WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT, 0 } }), _runStartPending(false), _manhuntPending(false), _teslaPending(false), _defendersPending(false) { }

    void OnCreatureCreate(Creature* creature) override
    {
        EG_zone_undercity_battle::OnCreatureCreate(creature);

        switch (creature->GetEntry())
        {
            case NPC_VARIAN_WRYNN:
                _varianGUID = creature->GetGUID();
                break;
            case NPC_JAINA_PROUDMOORE:
                _jainaGUID = creature->GetGUID();
                break;
            case NPC_GRAND_APOTHECARY_PUTRESS:
                _putressGUID = creature->GetGUID();
                _teslaPending = true;
                break;
            case NPC_THRALL_THRONE:
                _thrallGUID = creature->GetGUID();
                break;
            case NPC_INVISIBLE_STALKER_TESLA:
                _teslaGUIDs.insert(creature->GetGUID());
                break;
            case NPC_WARSONG_BATTLEGUARD_THRONE:
                if (!creature->IsSummon())
                    _battleguardGUIDs.insert(creature->GetGUID());
                break;
            default:
                break;
        }
    }

    void OnCreatureRemove(Creature* creature) override
    {
        EG_zone_undercity_battle::OnCreatureRemove(creature);

        ObjectGuid const guid = creature->GetGUID();
        _teslaGUIDs.erase(guid);
        _battleguardGUIDs.erase(guid);
        if (guid == _varianGUID)
        {
            _varianGUID.Clear();
            if (BattleState != BATTLE_STATE_IDLE)
                EndRun();
        }
        else if (guid == _jainaGUID)
            _jainaGUID.Clear();
        else if (guid == _putressGUID)
            _putressGUID.Clear();
        else if (guid == _thrallGUID)
            _thrallGUID.Clear();
    }

    void OnUnitDeath(Unit* unit) override
    {
        switch (unit->GetEntry())
        {
            case NPC_VARIAN_WRYNN:
                if (BattleState != BATTLE_STATE_IDLE)
                    EndRun();
                break;
            case NPC_ENGORGED_BLIGHT_WORM:
                if (BattleState != BATTLE_STATE_SEWERS_FIGHT)
                    break;

                SetData(DATA_BATTLE_STATE, BATTLE_STATE_SEWERS_SECURED);
                if (Creature* varian = unit->GetMap()->GetCreature(_varianGUID))
                    varian->AI()->DoAction(ACTION_SEWERS_SECURED);
                break;
            case NPC_GRAND_APOTHECARY_PUTRESS:
                if (BattleState != BATTLE_STATE_APOTHECARIUM_FIGHT)
                    break;

                SetData(DATA_BATTLE_STATE, BATTLE_STATE_APOTHECARIUM_SECURED);
                if (Creature* varian = unit->GetMap()->GetCreature(_varianGUID))
                    varian->AI()->DoAction(ACTION_APOTHECARIUM_SECURED);
                break;
            default:
                break;
        }
    }

    ObjectGuid GetGuidData(uint32 type) const override
    {
        switch (type)
        {
            case DATA_VARIAN_WRYNN:
                return _varianGUID;
            case DATA_JAINA_PROUDMOORE:
                return _jainaGUID;
            case DATA_GRAND_APOTHECARY_PUTRESS:
                return _putressGUID;
            case DATA_THRALL_THRONE:
                return _thrallGUID;
            default:
                return ObjectGuid::Empty;
        }
    }

    void OnUpdate(Map* map, uint32 diff) override
    {
        EG_zone_undercity_battle::OnUpdate(map, diff);

        if (_runStartPending)
        {
            _runStartPending = false;
            for (uint32 groupId : { SPAWN_GROUP_ALLIANCE_ENTRANCE, SPAWN_GROUP_ALLIANCE_SEWERS, SPAWN_GROUP_ALLIANCE_APOTHECARIUM })
                map->SpawnGroupSpawn(groupId, true);
            if (Creature* varian = map->GetCreature(_varianGUID))
                varian->AI()->DoAction(ACTION_COUNTDOWN);
        }

        if (_teslaPending)
        {
            _teslaPending = false;
            for (ObjectGuid const& guid : _teslaGUIDs)
                if (Creature* tesla = map->GetCreature(guid))
                    tesla->CastSpell(nullptr, SPELL_BLIGHT_EMPOWERMENT);
        }

        if (_defendersPending)
        {
            _defendersPending = false;
            Creature* varian = map->GetCreature(_varianGUID);
            for (ObjectGuid const& guid : _battleguardGUIDs)
            {
                Creature* battleguard = map->GetCreature(guid);
                if (!battleguard || !varian)
                    continue;

                battleguard->SetImmuneToAll(false);
                battleguard->SetReactState(REACT_AGGRESSIVE);
                battleguard->AI()->AttackStart(varian);
            }
        }

        if (!_manhuntPending)
            return;

        _manhuntPending = false;
        if (Creature* varian = map->GetCreature(_varianGUID))
            varian->AI()->DoAction(ACTION_MANHUNT);
    }

    void SetData(uint32 type, uint32 value) override
    {
        switch (type)
        {
            case DATA_THRONE_DEFENDERS:
                _defendersPending = true;
                return;
            case DATA_RESET_RUN:
                Scheduler.Schedule(11500ms, [this](TaskContext /*context*/)
                {
                    EndRun();
                });
                return;
            case DATA_BATTLE_STATE:
                break;
            default:
                return;
        }

        switch (value)
        {
            case BATTLE_STATE_IDLE:
                if (BattleState != BATTLE_STATE_IDLE)
                    EndRun();
                break;
            case BATTLE_STATE_COUNTDOWN:
                if (BattleState != BATTLE_STATE_IDLE)
                    break;

                BattleState = BATTLE_STATE_COUNTDOWN;
                _runStartPending = true;
                SetWorldState(WORLD_STATE_ALLIANCE_RUN_ACTIVE, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_COUNTDOWN, 1);
                StartCountdown(WORLD_STATE_ALLIANCE_COUNTDOWN_MINUTES, [this]()
                {
                    BattleState = BATTLE_STATE_BEGUN;
                    _manhuntPending = true;
                    SetWorldState(WORLD_STATE_ALLIANCE_COUNTDOWN, 0);
                    SetWorldState(WORLD_STATE_ALLIANCE_MANHUNT_BEGUN, 1);
                });
                break;
            case BATTLE_STATE_SEWERS_FIGHT:
                if (BattleState != BATTLE_STATE_BEGUN)
                    break;

                BattleState = BATTLE_STATE_SEWERS_FIGHT;
                SetWorldState(WORLD_STATE_ALLIANCE_MANHUNT_BEGUN, 0);
                SetWorldState(WORLD_STATE_ALLIANCE_ENEMY_CONTROLS_UNDERCITY, 0);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_PROGRESS, 1);
                break;
            case BATTLE_STATE_SEWERS_SECURED:
                if (BattleState != BATTLE_STATE_SEWERS_FIGHT)
                    break;

                BattleState = BATTLE_STATE_SEWERS_SECURED;
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 0);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_SECURED, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_PROGRESS, 2);
                break;
            case BATTLE_STATE_APOTHECARIUM_FIGHT:
                if (BattleState != BATTLE_STATE_SEWERS_SECURED)
                    break;

                BattleState = BATTLE_STATE_APOTHECARIUM_FIGHT;
                SetWorldState(WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_APOTHECARIUM_PROGRESS, 1);
                break;
            case BATTLE_STATE_APOTHECARIUM_SECURED:
                if (BattleState != BATTLE_STATE_APOTHECARIUM_FIGHT)
                    break;

                BattleState = BATTLE_STATE_APOTHECARIUM_SECURED;
                SetWorldState(WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT, 0);
                SetWorldState(WORLD_STATE_ALLIANCE_APOTHECARIUM_SECURED, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_APOTHECARIUM_PROGRESS, 2);
                break;
            default:
                break;
        }
    }

private:
    void OnRunEnd(Map* map) override
    {
        _runStartPending = false;
        _manhuntPending = false;
        _teslaPending = false;
        _defendersPending = false;
        for (uint32 groupId : { SPAWN_GROUP_ALLIANCE_ENTRANCE, SPAWN_GROUP_ALLIANCE_SEWERS, SPAWN_GROUP_ALLIANCE_APOTHECARIUM, SPAWN_GROUP_ALLIANCE_THRONE })
            map->SpawnGroupDespawn(groupId);
        ResetLeader(map, _varianGUID);
        ResetLeader(map, _jainaGUID);
    }

    ObjectGuid _varianGUID;
    ObjectGuid _jainaGUID;
    ObjectGuid _putressGUID;
    ObjectGuid _thrallGUID;
    GuidUnorderedSet _teslaGUIDs;
    GuidUnorderedSet _battleguardGUIDs;
    bool _runStartPending;
    bool _manhuntPending;
    bool _teslaPending;
    bool _defendersPending;
};

struct EG_npc_undercity_leader : public ScriptedAI
{
    EG_npc_undercity_leader(Creature* creature, uint32 gossipMenu, uint32 readyText, uint32 busyText) : ScriptedAI(creature), Summons(me), HasAnchor(false), _gossipMenu(gossipMenu), _readyText(readyText), _busyText(busyText), _pathStarted(false)
    {
        Combat.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    bool OnGossipHello(Player* player) override
    {
        bool const idle = GetBattleState() == BATTLE_STATE_IDLE;

        player->PrepareGossipMenu(me, _gossipMenu, true);
        if (!idle)
            player->PlayerTalkClass->GetGossipMenu().ClearMenu();

        SendGossipMenuFor(player, idle ? _readyText : _busyText, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        if (!zoneScript || menuId != _gossipMenu || gossipListId != GOSSIP_OPTION_START || !player->PlayerTalkClass->GetGossipMenu().GetItem(gossipListId))
            return false;

        CloseGossipMenuFor(player);
        zoneScript->SetData(DATA_BATTLE_STATE, BATTLE_STATE_COUNTDOWN);
        return true;
    }

    void JustSummoned(Creature* summon) override
    {
        Summons.Summon(summon);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        Summons.Despawn(summon);
    }

    void WaypointPathEnded(uint32 /*nodeId*/, uint32 /*pathId*/) override
    {
        _pathStarted = false;
        PathEnded();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (me->IsEngaged() && me->HasReactState(REACT_AGGRESSIVE) && me->CanStartAttack(who, false))
            me->EngageWithTarget(who);

        ScriptedAI::MoveInLineOfSight(who);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        Combat.CancelGroup(GROUP_COMBAT);
        me->AttackStop();
        me->GetThreatManager().ClearAllThreat();
        EngagementOver();
        me->GetMotionMaster()->Remove(CHASE_MOTION_TYPE);
        if (HasAnchor)
            me->GetMotionMaster()->MovePoint(POINT_ANCHOR, Anchor, true, Anchor.GetOrientation());
    }

    void UpdateAI(uint32 diff) override
    {
        if (_pathStarted && me->GetMotionMaster()->GetCurrentMovementGeneratorType(MOTION_SLOT_DEFAULT) != WAYPOINT_MOTION_TYPE)
        {
            _pathStarted = false;
            if (ZoneScript* zoneScript = me->GetZoneScript())
                zoneScript->SetData(DATA_BATTLE_STATE, BATTLE_STATE_IDLE);
            return;
        }

        Scheduler.Update(diff);
        _waves.Update(diff);

        if (!UpdateVictim())
            return;

        Combat.Update(diff);
        DoMeleeAttackIfReady();
    }

protected:
    Creature* GetActor(uint32 type) const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(type)) : nullptr;
    }

    uint32 GetBattleState() const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? zoneScript->GetData(DATA_BATTLE_STATE) : uint32(BATTLE_STATE_IDLE);
    }

    virtual void PathEnded() { }

    void StartPath(uint32 pathId)
    {
        _pathStarted = true;
        me->GetMotionMaster()->MovePath(pathId, false);
    }

    void StartWaves(uint8 firstGroup, uint8 lastGroup, Milliseconds interval)
    {
        _waves.CancelAll();
        _waves.Schedule(0s, [this, firstGroup, lastGroup, interval](TaskContext context)
        {
            SummonWave(uint8(firstGroup + context.GetRepeatCounter() % (lastGroup - firstGroup + 1)));
            context.Repeat(interval);
        });
    }

    void SummonWave(uint8 summonGroup)
    {
        std::list<TempSummon*> attackers;
        me->SummonCreatureGroup(summonGroup, &attackers);
        for (TempSummon* attacker : attackers)
        {
            _waveGUIDs.push_back(attacker->GetGUID());
            attacker->AI()->AttackStart(me);
            me->EngageWithTarget(attacker);
        }
    }

    void StopWaves()
    {
        _waves.CancelAll();
        for (ObjectGuid const& guid : _waveGUIDs)
            if (Creature* attacker = ObjectAccessor::GetCreature(*me, guid))
                if (attacker->IsAlive())
                    attacker->DespawnOrUnsummon();
        _waveGUIDs.clear();
    }

    void ActorTalk(uint32 actorData, uint8 textGroup)
    {
        if (Creature* actor = GetActor(actorData))
            actor->AI()->Talk(textGroup);
    }

    SummonList Summons;
    TaskScheduler Scheduler;
    TaskScheduler Combat;
    Position Anchor;
    bool HasAnchor;

private:
    uint32 const _gossipMenu;
    uint32 const _readyText;
    uint32 const _busyText;
    TaskScheduler _waves;
    GuidVector _waveGUIDs;
    bool _pathStarted;
};

// 31650 - Thrall
struct EG_npc_undercity_thrall : public EG_npc_undercity_leader
{
    EG_npc_undercity_thrall(Creature* creature) : EG_npc_undercity_leader(creature, GOSSIP_MENU_THRALL, NPC_TEXT_THRALL_READY, NPC_TEXT_THRALL_BUSY), _descentStep(0) { }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_COUNTDOWN:
                Scheduler.Schedule(1200ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_HEROES);
                })
                .Schedule(7700ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_GATHER);
                })
                .Schedule(129400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_BLOOD_AND_THUNDER);
                })
                .Schedule(148600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_GREAT_CONFLICT);
                });
                break;
            case ACTION_BATTLE_BEGUN:
                Scheduler.Schedule(4500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_GREAT_SPIRITS);
                })
                .Schedule(15900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_SONG_OF_WAR);
                })
                .Schedule(28900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_BATTLE_BEGINS);
                })
                .Schedule(31900ms, [this](TaskContext /*context*/)
                {
                    me->CastSpell(me, SPELL_WARCHIEFS_BLESSING);
                    me->CastSpell(me, SPELL_WARCHIEFS_BLESSING_AURA);
                })
                .Schedule(40200ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_ONWARD);
                    me->ReplaceAllNpcFlags(UNIT_NPC_FLAG_NONE);
                    me->SetImmuneToNPC(false);
                    StartPath(PATH_THRALL);
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_START_PATH);
                    for (uint8 i = 0; i <= SUMMON_GROUP_BATTLEGUARD_MARCH_LAST - SUMMON_GROUP_BATTLEGUARD_MARCH_FIRST; ++i)
                    {
                        std::list<TempSummon*> battleguards;
                        me->SummonCreatureGroup(SUMMON_GROUP_BATTLEGUARD_MARCH_FIRST + i, &battleguards);
                        for (TempSummon* battleguard : battleguards)
                        {
                            _battleguardGUIDs.push_back(battleguard->GetGUID());
                            battleguard->GetMotionMaster()->MovePath(PATH_BATTLEGUARD_MARCH + i, false);
                        }
                    }
                });
                break;
            case ACTION_COURTYARD_CONTROLLED:
                StopWaves();
                HasAnchor = false;
                ReleaseSylvanas(POINT_SYLVANAS_COURTYARD);
                me->ResumeMovement();
                for (size_t i = 0; i < _battleguardGUIDs.size(); ++i)
                    if (Creature* battleguard = ObjectAccessor::GetCreature(*me, _battleguardGUIDs[i]))
                        battleguard->GetMotionMaster()->MovePath(PATH_BATTLEGUARD_COURTYARD + i, false);
                break;
            case ACTION_INNER_SANCTUM_CONTROLLED:
                StopWaves();
                me->SummonCreatureGroup(SUMMON_GROUP_BATTLEGUARD_REINFORCEMENT_1);
                Scheduler.Schedule(9700ms, [this](TaskContext context)
                {
                    std::list<TempSummon*> battleguards;
                    me->SummonCreatureGroup(SUMMON_GROUP_BATTLEGUARD_REINFORCEMENT_2, &battleguards);
                    if (battleguards.empty())
                        return;

                    for (TempSummon* battleguard : battleguards)
                        battleguard->GetMotionMaster()->MovePath(PATH_BATTLEGUARD_REINFORCEMENT, false);
                    context.Schedule(8700ms, [this, battleguardGUID = battleguards.front()->GetGUID()](TaskContext /*context*/)
                    {
                        if (Creature* battleguard = ObjectAccessor::GetCreature(*me, battleguardGUID))
                            battleguard->AI()->Talk(SAY_BATTLEGUARD_FOR_THE_HORDE);
                    });
                })
                .Schedule(26s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_IS_THAT_ALL);
                })
                .Schedule(32400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_WE_COME_FOR_YOU);
                })
                .Schedule(38800ms, [this](TaskContext /*context*/)
                {
                    HasAnchor = false;
                    ReleaseSylvanas(POINT_SYLVANAS_INNER_SANCTUM);
                    me->ResumeMovement();
                });
                break;
            case ACTION_ROYAL_QUARTER_CONTROLLED:
                StopWaves();
                Scheduler.Schedule(2600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_VICTORIOUS);
                })
                .Schedule(7500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_LOKTAR);
                })
                .Schedule(13900ms, [this](TaskContext /*context*/)
                {
                    HasAnchor = false;
                    me->ResumeMovement();
                });
                break;
            default:
                break;
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
            case POINT_THRALL_GATE:
                me->PauseMovement();
                me->Dismount();
                if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                    sylvanas->Dismount();
                Scheduler.Schedule(3s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_CALL_OF_AIR);
                })
                .Schedule(3500ms, [this](TaskContext context)
                {
                    std::vector<std::pair<ObjectGuid, uint32>> vortices;
                    for (auto const& [summonGroup, path] : { std::pair<uint8, uint32>(SUMMON_GROUP_GREAT_WIND_VORTEX_1, PATH_GREAT_WIND_VORTEX_1), std::pair<uint8, uint32>(SUMMON_GROUP_GREAT_WIND_VORTEX_2, PATH_GREAT_WIND_VORTEX_2) })
                    {
                        std::list<TempSummon*> summons;
                        me->SummonCreatureGroup(summonGroup, &summons);
                        for (TempSummon* vortex : summons)
                            vortices.emplace_back(vortex->GetGUID(), path);
                    }

                    context.Schedule(2600ms, [this, vortices](TaskContext /*context*/)
                    {
                        for (auto const& [guid, path] : vortices)
                            if (Creature* vortex = ObjectAccessor::GetCreature(*me, guid))
                                vortex->GetMotionMaster()->MovePath(path, false);
                    });
                })
                .Schedule(14200ms, [this](TaskContext /*context*/)
                {
                    ReleaseSylvanas(POINT_SYLVANAS_GATE);
                    me->ResumeMovement();
                });
                break;
            case POINT_THRALL_COURTYARD_ENTRANCE:
                me->PauseMovement();
                Scheduler.Schedule(1400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_ANSWER_TO_THE_ELEMENTS);
                })
                .Schedule(7800ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIMATHRAS_COURTYARD);
                })
                .Schedule(10700ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_WELCOME);
                })
                .Schedule(17200ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_MINION);
                })
                .Schedule(26900ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_ENOUGH);
                })
                .Schedule(33300ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->CastSpell(varimathras, SPELL_LEGION_PORTAL);
                })
                .Schedule(46200ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->DoAction(ACTION_VARIMATHRAS_LEAVE);
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_BATTLE_STATE, BATTLE_STATE_COURTYARD_FIGHT);
                })
                .Schedule(48400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_TIDAL_WAVE);
                    me->CastSpell(nullptr, SPELL_TIDAL_WAVE);
                })
                .Schedule(56500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_ATTACK);
                    Anchor.Relocate(1798.851f, 254.913f, 59.989f, 3.464667f);
                    HasAnchor = true;
                    StartWaves(SUMMON_GROUP_COURTYARD_WAVE_FIRST, SUMMON_GROUP_COURTYARD_WAVE_LAST, 10400ms);
                    ReleaseSylvanas(POINT_SYLVANAS_COURTYARD_ENTRANCE);
                    me->ResumeMovement();
                })
                .Schedule(59900ms, [this](TaskContext /*context*/)
                {
                    me->CastSpell(me, SPELL_HEROIC_VANGUARD);
                })
                .Schedule(68200ms, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_SING);
                })
                .Schedule(240100ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_BLIGHT_ABERRATION);
                });
                break;
            case POINT_THRALL_COURTYARD:
                if (GetBattleState() == BATTLE_STATE_COURTYARD_FIGHT)
                    me->PauseMovement();
                break;
            case POINT_THRALL_COURTYARD_SECURED:
                me->PauseMovement();
                Scheduler.Schedule(300ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_COURTYARD_SECURED);
                })
                .Schedule(6700ms, [this](TaskContext /*context*/)
                {
                    me->ResumeMovement();
                });
                break;
            case POINT_THRALL_ELEVATOR:
                me->PauseMovement();
                Scheduler.Schedule(200ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_ELEVATOR_DESTROYED);
                })
                .Schedule(16s, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_SYLVANAS, SAY_SYLVANAS_SHAFT_TRAPPED);
                })
                .Schedule(26s, [this](TaskContext /*context*/)
                {
                    me->CastSpell(me, SPELL_CALL_OF_AIR);
                })
                .Schedule(26200ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_AIR_SPIRIT);
                    ReleaseSylvanas(POINT_SYLVANAS_ELEVATOR);
                })
                .Schedule(42400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_CYCLONES);
                })
                .Schedule(48800ms, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_DESCENT);
                    _descentStep = 0;
                    DescentJump();
                })
                .Schedule(73200ms, [this](TaskContext /*context*/)
                {
                    WaypointPath const* path = sWaypointMgr->GetPath(PATH_THRALL_DESCENT);
                    if (path && !path->nodes.empty() && _descentStep < path->nodes.size())
                        me->NearTeleportTo(path->nodes.back().x, path->nodes.back().y, path->nodes.back().z, me->GetOrientation());
                    Talk(SAY_THRALL_INNER_SANCTUM_BREACHED);
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_BATTLE_STATE, BATTLE_STATE_INNER_SANCTUM_FIGHT);
                    ReleaseSylvanas(POINT_SYLVANAS_SHAFT);
                    me->ResumeMovement();
                });
                break;
            case POINT_THRALL_LOWER_CITY:
                me->PauseMovement();
                Scheduler.Schedule(7100ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_SYLVANAS, SAY_SYLVANAS_BEAUTIFUL_CITY);
                })
                .Schedule(20100ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_SYLVANAS, SAY_SYLVANAS_REDEMPTION);
                })
                .Schedule(31s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_LEAD_THE_WAY);
                })
                .Schedule(40800ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_SYLVANAS, SAY_SYLVANAS_ROYAL_QUARTER_THIS_WAY);
                })
                .Schedule(52500ms, [this](TaskContext /*context*/)
                {
                    ReleaseSylvanas(POINT_SYLVANAS_LOWER_CITY);
                    me->ResumeMovement();
                })
                .Schedule(96300ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_DEATHGUARD_ELITE_1);
                })
                .Schedule(121100ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_DEATHGUARD_ELITE_2);
                });
                break;
            case POINT_THRALL_INNER_SANCTUM:
                me->PauseMovement();
                Anchor.Relocate(1589.82f, 380.491f, -62.178f, 1.448623f);
                HasAnchor = true;
                Scheduler.Schedule(16600ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_SYLVANAS, SAY_SYLVANAS_DARK_MAGIC);
                })
                .Schedule(17600ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIMATHRAS_INNER_SANCTUM);
                })
                .Schedule(20700ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_CLEVER_GIRL);
                })
                .Schedule(27s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->CastSpell(varimathras, SPELL_LEGION_PORTAL);
                })
                .Schedule(27200ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_BROTHERS_HUNGRY);
                    StartWaves(SUMMON_GROUP_INNER_SANCTUM_WAVE_FIRST, SUMMON_GROUP_INNER_SANCTUM_WAVE_LAST, 5700ms);
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_ASSASSINATE);
                })
                .Schedule(30300ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->DoAction(ACTION_VARIMATHRAS_LEAVE);
                })
                .Schedule(74300ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_DEATHGUARD_ELITE_3);
                })
                .Schedule(196300ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_KHANOK);
                });
                break;
            case POINT_THRALL_ROYAL_QUARTER_APPROACH:
                me->PauseMovement();
                me->SummonCreatureGroup(SUMMON_GROUP_DEATHGUARD_ELITE_4);
                Scheduler.Schedule(27s, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_DEATHGUARD_ELITE_5);
                })
                .Schedule(44600ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_SYLVANAS, SAY_SYLVANAS_ROYAL_QUARTER_AHEAD);
                })
                .Schedule(51600ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIMATHRAS_HALL);
                })
                .Schedule(53s, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_BRING_DOWN_THE_HALLS);
                })
                .Schedule(56200ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->DoAction(ACTION_VARIMATHRAS_RETREAT);
                })
                .Schedule(56600ms, [this](TaskContext /*context*/)
                {
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_HALL_COLLAPSE, 0);
                })
                .Schedule(72100ms, [this](TaskContext /*context*/)
                {
                    ReleaseSylvanas(POINT_SYLVANAS_ROYAL_QUARTER_APPROACH);
                    me->ResumeMovement();
                });
                break;
            case POINT_THRALL_COLLAPSED_HALL:
                me->PauseMovement();
                Scheduler.Schedule(3900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_COWARD);
                })
                .Schedule(13400ms, [this](TaskContext /*context*/)
                {
                    me->CastSpell(nullptr, SPELL_CALL_OF_EARTH);
                })
                .Schedule(13600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_EARTH_SPIRITS);
                })
                .Schedule(21600ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIMATHRAS_ROYAL_QUARTER);
                })
                .Schedule(26100ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->CastSpell(varimathras, SPELL_AURA_OF_VARIMATHRAS);
                })
                .Schedule(27900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_LETS_GO_QUICKLY);
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_BATTLE_STATE, BATTLE_STATE_ROYAL_QUARTER_FIGHT);
                    ReleaseSylvanas(POINT_SYLVANAS_COLLAPSED_HALL);
                    me->ResumeMovement();
                })
                .Schedule(29100ms, [this](TaskContext /*context*/)
                {
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_LEGION_PORTALS_OPEN, 0);
                });
                break;
            case POINT_THRALL_THRONE_ROOM:
                me->PauseMovement();
                Anchor.Relocate(1301.873f, 359.193f, -67.202f, 4.438606f);
                HasAnchor = true;
                Scheduler.Schedule(1s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_WHAT_IS_THIS);
                })
                .Schedule(4400ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_WELCOME_TO_YOUR_FUTURE);
                })
                .Schedule(5100ms, [this](TaskContext context)
                {
                    SummonWave(uint8(SUMMON_GROUP_ROYAL_QUARTER_WAVE_FIRST + context.GetRepeatCounter()));
                    if (context.GetRepeatCounter() < uint32(SUMMON_GROUP_ROYAL_QUARTER_WAVE_LAST - SUMMON_GROUP_ROYAL_QUARTER_WAVE_FIRST))
                        context.Repeat(12s);
                })
                .Schedule(62100ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_DEATHGUARD_ELITE_6);
                })
                .Schedule(69200ms, [this](TaskContext /*context*/)
                {
                    CollapsePortal(1);
                })
                .Schedule(76700ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_TOO_LONG);
                })
                .Schedule(123s, [this](TaskContext /*context*/)
                {
                    CollapsePortal(2);
                })
                .Schedule(130200ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_NEED_MORE_TIME);
                })
                .Schedule(164600ms, [this](TaskContext /*context*/)
                {
                    CollapsePortal(3);
                })
                .Schedule(170800ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_SUCH_POWER);
                })
                .Schedule(195900ms, [this](TaskContext /*context*/)
                {
                    CollapsePortal(4);
                })
                .Schedule(203200ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_WILL_NOT_FAIL);
                })
                .Schedule(216800ms, [this](TaskContext /*context*/)
                {
                    CollapsePortal(5);
                })
                .Schedule(224300ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_CANNOT_HOLD);
                })
                .Schedule(227200ms, [this](TaskContext /*context*/)
                {
                    CollapsePortal(6);
                })
                .Schedule(227300ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_DISTANT_VOICE);
                })
                .Schedule(235600ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIMATHRAS, SAY_VARIMATHRAS_THOUSAND_PARDONS);
                })
                .Schedule(239100ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->DoAction(ACTION_VARIMATHRAS_ATTACK);
                });
                break;
            case POINT_THRALL_SYLVANAS_PLATFORM:
                me->PauseMovement();
                Scheduler.Schedule(9100ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_JOIN_ME);
                })
                .Schedule(12100ms, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_ASCENSION);
                })
                .Schedule(20500ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_SYLVANAS, SAY_SYLVANAS_FATE_OF_ALL_ENEMIES);
                })
                .Schedule(25400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_WE_SHALL);
                    me->ResumeMovement();
                })
                .Schedule(33400ms, [this](TaskContext /*context*/)
                {
                    ReleaseSylvanas(POINT_SYLVANAS_PLATFORM);
                });
                break;
            case POINT_THRALL_ALLIANCE_HORNS:
                me->PauseMovement();
                Scheduler.Schedule(1500ms, [this](TaskContext context)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIAN_WRYNN_HORDE_RUN);
                    std::vector<std::pair<ObjectGuid, uint32>> elites;
                    for (uint8 i = 0; i <= SUMMON_GROUP_STORMWIND_ELITE_LAST - SUMMON_GROUP_STORMWIND_ELITE_FIRST; ++i)
                    {
                        std::list<TempSummon*> summons;
                        me->SummonCreatureGroup(SUMMON_GROUP_STORMWIND_ELITE_FIRST + i, &summons);
                        for (TempSummon* elite : summons)
                            elites.emplace_back(elite->GetGUID(), PATH_STORMWIND_ELITE_FORMATION + i);
                    }

                    context.Schedule(11400ms, [this, elites](TaskContext /*context*/)
                    {
                        if (Creature* varian = GetActor(DATA_VARIAN_WRYNN_HORDE_RUN))
                            varian->GetMotionMaster()->MovePath(PATH_VARIAN_WRYNN_HORDE_RUN, false);
                        for (auto const& [guid, path] : elites)
                            if (Creature* elite = ObjectAccessor::GetCreature(*me, guid))
                                elite->GetMotionMaster()->MovePath(path, false);
                    });
                })
                .Schedule(2500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varian = GetActor(DATA_VARIAN_WRYNN_HORDE_RUN))
                        varian->CastSpell(nullptr, SPELL_ROLL_PUTRESS_HEAD);
                })
                .Schedule(7500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_ALLIANCE_HORNS);
                    me->ResumeMovement();
                });
                break;
            case POINT_THRALL_ALLIANCE_INTRUSION:
                me->PauseMovement();
                Scheduler.Schedule(13300ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIAN_WRYNN_HORDE_RUN, SAY_VARIAN_HORDE_RUN_AWAY_TOO_LONG);
                })
                .Schedule(30700ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIAN_WRYNN_HORDE_RUN, SAY_VARIAN_HORDE_RUN_MAKE_THINGS_RIGHT);
                })
                .Schedule(47s, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIAN_WRYNN_HORDE_RUN, SAY_VARIAN_HORDE_RUN_WAITED_LONG);
                })
                .Schedule(63200ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIAN_WRYNN_HORDE_RUN, SAY_VARIAN_HORDE_RUN_IT_ENDS_NOW);
                })
                .Schedule(74500ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_VARIAN_WRYNN_HORDE_RUN, SAY_VARIAN_HORDE_RUN_ATTACK);
                    for (ObjectGuid const& guid : Summons)
                    {
                        Creature* soldier = ObjectAccessor::GetCreature(*me, guid);
                        if (!soldier || (soldier->GetEntry() != NPC_VARIAN_WRYNN_HORDE_RUN && soldier->GetEntry() != NPC_STORMWIND_ELITE_HORDE_RUN))
                            continue;

                        soldier->SetImmuneToAll(false);
                        soldier->SetReactState(REACT_AGGRESSIVE);
                        soldier->AI()->AttackStart(me);
                    }
                })
                .Schedule(76500ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_JAINA_PROUDMOORE_HORDE_RUN);
                })
                .Schedule(78600ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_JAINA_PROUDMOORE_HORDE_RUN, SAY_JAINA_HORDE_RUN_VARIAN_NO);
                })
                .Schedule(83300ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE_HORDE_RUN))
                        jaina->CastSpell(jaina, SPELL_DEEP_FREEZE_STUN);
                })
                .Schedule(84100ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varian = GetActor(DATA_VARIAN_WRYNN_HORDE_RUN))
                        varian->CastSpell(varian, SPELL_WHIRLWIND);
                })
                .Schedule(87900ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_JAINA_PROUDMOORE_HORDE_RUN, SAY_JAINA_HORDE_RUN_DID_NOT_HAVE_TO_BE);
                })
                .Schedule(91400ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE_HORDE_RUN))
                        jaina->CastSpell(jaina, SPELL_GREATER_MASS_TELEPORT_DUMMY);
                })
                .Schedule(104400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_IT_ENDS);
                    me->ResumeMovement();
                });
                break;
            case POINT_THRALL_LAMENT:
                me->PauseMovement();
                Scheduler.Schedule(5100ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_ALL_IS_LOST);
                })
                .Schedule(19600ms, [this](TaskContext /*context*/)
                {
                    me->ResumeMovement();
                });
                break;
            default:
                break;
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != EFFECT_MOTION_TYPE || pointId != POINT_DESCENT)
            return;

        ++_descentStep;
        Scheduler.Schedule(1200ms, [this](TaskContext /*context*/)
        {
            DescentJump();
        });
    }

    void JustSummoned(Creature* summon) override
    {
        EG_npc_undercity_leader::JustSummoned(summon);

        switch (summon->GetEntry())
        {
            case NPC_BLIGHT_ABERRATION:
                summon->AI()->Talk(SAY_BLIGHT_ABERRATION_RELEASED);
                Scheduler.Schedule(7900ms, [this, aberrationGUID = summon->GetGUID()](TaskContext /*context*/)
                {
                    Creature* aberration = ObjectAccessor::GetCreature(*me, aberrationGUID);
                    if (!aberration)
                        return;

                    aberration->AI()->Talk(SAY_BLIGHT_ABERRATION_ROAR);
                    aberration->SetImmuneToAll(false);
                    aberration->AI()->AttackStart(me);
                    me->EngageWithTarget(aberration);
                });
                break;
            case NPC_KHANOK:
                summon->AI()->Talk(SAY_KHANOK_APPEARS);
                Scheduler.Schedule(9300ms, [this, khanokGUID = summon->GetGUID()](TaskContext /*context*/)
                {
                    Creature* khanok = ObjectAccessor::GetCreature(*me, khanokGUID);
                    if (!khanok)
                        return;

                    khanok->AI()->Talk(SAY_KHANOK_NO_HOPE);
                    khanok->SetImmuneToAll(false);
                    khanok->AI()->AttackStart(me);
                    me->EngageWithTarget(khanok);
                });
                break;
            case NPC_DISTANT_VOICE:
                summon->AI()->Talk(SAY_DISTANT_VOICE_FAILED);
                break;
            case NPC_VARIAN_WRYNN_HORDE_RUN:
            case NPC_STORMWIND_ELITE_HORDE_RUN:
                summon->SetImmuneToAll(true);
                summon->SetReactState(REACT_PASSIVE);
                break;
            case NPC_JAINA_PROUDMOORE_HORDE_RUN:
                summon->SetImmuneToAll(true);
                summon->SetReactState(REACT_PASSIVE);
                summon->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE_HORDE_RUN, false);
                break;
            case NPC_SAURFANG:
            {
                WaypointPath const* path = sWaypointMgr->GetPath(PATH_SAURFANG);
                if (!path || path->nodes.empty())
                    break;

                WaypointNode const& node = path->nodes.front();
                summon->GetMotionMaster()->MoveJump(node.x, node.y, node.z, summon->GetOrientation(), 12.0f, Movement::gravity * summon->GetExactDist(node.x, node.y, node.z) / 24.0f);
                Scheduler.Schedule(2300ms, [this, saurfangGUID = summon->GetGUID()](TaskContext /*context*/)
                {
                    if (Creature* saurfang = ObjectAccessor::GetCreature(*me, saurfangGUID))
                        saurfang->GetMotionMaster()->MovePath(PATH_SAURFANG, false);
                });
                break;
            }
            default:
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Combat.Schedule(7300ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_LAVA_BURST);
            context.Repeat(7300ms, 19500ms);
        })
        .Schedule(8100ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_CHAIN_LIGHTNING);
            context.Repeat(8100ms, 21100ms);
        })
        .Schedule(15700ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastSelf(SPELL_THUNDER);
            context.Repeat(15700ms, 46500ms);
        })
        .Schedule(68s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_FORCE_OF_NATURE);
            context.Repeat(68s, 106s);
        });
    }

protected:
    void PathEnded() override
    {
        Scheduler.Schedule(200ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_IF_ONLY_YOU_WERE_HERE);
        })
        .Schedule(1200ms, [this](TaskContext /*context*/)
        {
            me->SummonCreatureGroup(SUMMON_GROUP_SAURFANG);
            me->GetMap()->SpawnGroupSpawn(SPAWN_GROUP_HORDE_THRONE_PORTAL, true);
        })
        .Schedule(8800ms, [this](TaskContext /*context*/)
        {
            ActorTalk(DATA_SAURFANG, SAY_SAURFANG_WHAT_HE_WOULD_DO);
        })
        .Schedule(15300ms, [this](TaskContext /*context*/)
        {
            ActorTalk(DATA_SAURFANG, SAY_SAURFANG_LEAD_YOUR_PEOPLE);
        })
        .Schedule(21800ms, [this](TaskContext /*context*/)
        {
            ActorTalk(DATA_SAURFANG, SAY_SAURFANG_LETS_GO_HOME);
        })
        .Schedule(26700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_NODS);
        })
        .Schedule(31100ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_GOOD_TO_HAVE_YOU_BACK);
            me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
        });
    }

private:
    void ReleaseSylvanas(uint32 pointId)
    {
        if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
            sylvanas->AI()->SetData(DATA_RELEASE_POINT, pointId);
    }

    void CollapsePortal(uint32 step)
    {
        if (ZoneScript* zoneScript = me->GetZoneScript())
            zoneScript->SetData(DATA_LEGION_PORTALS_COLLAPSE, step);
    }

    void DescentJump()
    {
        WaypointPath const* path = sWaypointMgr->GetPath(PATH_THRALL_DESCENT);
        if (!path || _descentStep >= path->nodes.size())
            return;

        WaypointNode const& node = path->nodes[_descentStep];
        me->GetMotionMaster()->MoveJump(node.x, node.y, node.z, me->GetOrientation(), 8.0f, Movement::gravity * me->GetExactDist(node.x, node.y, node.z) / 16.0f, POINT_DESCENT);
    }

    GuidVector _battleguardGUIDs;
    uint32 _descentStep;
};

struct EG_npc_undercity_companion : public ScriptedAI
{
    EG_npc_undercity_companion(Creature* creature, uint32 pathId, uint32 leaderData, std::set<uint32> stops) : ScriptedAI(creature), _pathId(pathId), _leaderData(leaderData), _stops(std::move(stops)), _releasedPoint(0), _pausedPoint(0)
    {
        Combat.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void DoAction(int32 action) override
    {
        if (action != ACTION_START_PATH)
            return;

        me->SetImmuneToNPC(false);
        me->GetMotionMaster()->MovePath(_pathId, false);
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type != DATA_RELEASE_POINT)
            return;

        _releasedPoint = std::max(_releasedPoint, value);
        if (!_pausedPoint || _pausedPoint > _releasedPoint)
            return;

        _pausedPoint = 0;
        me->ResumeMovement();
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (!_stops.contains(waypointId) || waypointId <= _releasedPoint)
            return;

        _pausedPoint = waypointId;
        me->PauseMovement();
    }

    void AttackStart(Unit* victim) override
    {
        AttackStartCaster(victim, 25.0f);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (me->IsEngaged() && me->HasReactState(REACT_AGGRESSIVE) && me->CanStartAttack(who, false))
            me->EngageWithTarget(who);

        ScriptedAI::MoveInLineOfSight(who);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        Combat.CancelAll();
        me->AttackStop();
        me->GetThreatManager().ClearAllThreat();
        EngagementOver();
        me->GetMotionMaster()->Remove(CHASE_MOTION_TYPE);
    }

    void UpdateAI(uint32 diff) override
    {
        Scheduler.Update(diff);

        if (!UpdateVictim())
        {
            Creature* leader = GetActor(_leaderData);
            Unit* victim = leader && me->IsWithinDist(leader, 40.0f) ? leader->GetVictim() : nullptr;
            if (victim && me->IsValidAttackTarget(victim))
                AttackStart(victim);
            return;
        }

        Combat.Update(diff);
        DoMeleeAttackIfReady();
    }

protected:
    Creature* GetActor(uint32 type) const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(type)) : nullptr;
    }

    uint32 GetBattleState() const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? zoneScript->GetData(DATA_BATTLE_STATE) : uint32(BATTLE_STATE_IDLE);
    }

    TaskScheduler Scheduler;
    TaskScheduler Combat;

private:
    uint32 const _pathId;
    uint32 const _leaderData;
    std::set<uint32> const _stops;
    uint32 _releasedPoint;
    uint32 _pausedPoint;
};

// 31651 - Lady Sylvanas Windrunner
struct EG_npc_undercity_sylvanas : public EG_npc_undercity_companion
{
    EG_npc_undercity_sylvanas(Creature* creature) : EG_npc_undercity_companion(creature, PATH_SYLVANAS, DATA_THRALL, { POINT_SYLVANAS_GATE, POINT_SYLVANAS_COURTYARD_ENTRANCE, POINT_SYLVANAS_COURTYARD, POINT_SYLVANAS_ELEVATOR, POINT_SYLVANAS_SHAFT, POINT_SYLVANAS_LOWER_CITY, POINT_SYLVANAS_INNER_SANCTUM, POINT_SYLVANAS_ROYAL_QUARTER_APPROACH, POINT_SYLVANAS_COLLAPSED_HALL, POINT_SYLVANAS_THRONE_ROOM, POINT_SYLVANAS_PLATFORM }), _descentPending(false) { }

    void WaypointReached(uint32 waypointId, uint32 pathId) override
    {
        EG_npc_undercity_companion::WaypointReached(waypointId, pathId);
        if (waypointId == POINT_SYLVANAS_SHAFT && _descentPending)
            Descend();
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_SYLVANAS_SING:
                Talk(SAY_SYLVANAS_BEGINS_TO_SING);
                me->CastSpell(me, SPELL_SONG_OF_SYLVANAS);
                break;
            case ACTION_SYLVANAS_DESCENT:
                if (me->GetCurrentWaypointInfo().first == POINT_SYLVANAS_SHAFT)
                    Descend();
                else
                    _descentPending = true;
                break;
            case ACTION_SYLVANAS_ASSASSINATE:
                Scheduler.Schedule(2500ms, [this](TaskContext context)
                {
                    if (GetBattleState() != BATTLE_STATE_INNER_SANCTUM_FIGHT)
                        return;

                    if (!me->GetVehicle() && !me->HasUnitState(UNIT_STATE_CASTING))
                    {
                        me->CastSpell(nullptr, SPELL_ASSASSINATE_BOARD);
                        context.Schedule(1s, [this](TaskContext /*context*/)
                        {
                            if (me->GetVehicle())
                                me->CastSpell(nullptr, SPELL_ASSASSINATE_KILL);
                        });
                    }

                    context.Repeat(5s);
                });
                break;
            case ACTION_SYLVANAS_ASCENSION:
                SetData(DATA_RELEASE_POINT, POINT_SYLVANAS_THRONE_ROOM);
                me->CastSpell(nullptr, SPELL_ASCENSION);
                break;
            default:
                EG_npc_undercity_companion::DoAction(action);
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Combat.Schedule(2100ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHOOT);
            context.Repeat(2100ms, 17300ms);
        })
        .Schedule(8100ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHRIEK_OF_THE_HIGHBORNE);
            context.Repeat(8100ms, 28300ms);
        })
        .Schedule(7300ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_MULTI_SHOT);
            context.Repeat(7300ms, 34900ms);
        })
        .Schedule(18300ms, GROUP_COMBAT, [this](TaskContext context)
        {
            if (GetBattleState() == BATTLE_STATE_COURTYARD_FIGHT)
                me->CastSpell(nullptr, SPELL_HEARTSTRIKE);
            context.Repeat(18300ms, 39300ms);
        })
        .Schedule(20s, 40s, GROUP_COMBAT, [this](TaskContext context)
        {
            if (GetBattleState() == BATTLE_STATE_COURTYARD_CONTROLLED || GetBattleState() == BATTLE_STATE_INNER_SANCTUM_FIGHT)
                Talk(SAY_SYLVANAS_BARK);
            context.Repeat(20s, 40s);
        });
    }

private:
    void Descend()
    {
        _descentPending = false;
        me->CastSpell(me, SPELL_TELEPORT_VISUAL_ONLY);
        me->NearTeleportTo(1532.84f, 241.1f, -41.38f, me->GetOrientation());
    }

    bool _descentPending;
};

// 31565 - Varimathras
struct EG_npc_undercity_varimathras : public ScriptedAI
{
    EG_npc_undercity_varimathras(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void IsSummonedBy(WorldObject* /*summoner*/) override
    {
        me->SetImmuneToAll(true);
        me->SetReactState(REACT_PASSIVE);
    }

    void JustSummoned(Creature* summon) override
    {
        _portalGUID = summon->GetGUID();
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_VARIMATHRAS_LEAVE:
                if (Creature* portal = ObjectAccessor::GetCreature(*me, _portalGUID))
                    me->GetMotionMaster()->MovePoint(POINT_PORTAL, portal->GetPosition());
                me->DespawnOrUnsummon(3500ms);
                break;
            case ACTION_VARIMATHRAS_RETREAT:
                me->GetMotionMaster()->MovePath(PATH_VARIMATHRAS_RETREAT, false);
                me->DespawnOrUnsummon(3700ms);
                break;
            case ACTION_VARIMATHRAS_ATTACK:
            {
                me->RemoveAurasDueToSpell(SPELL_AURA_OF_VARIMATHRAS);
                me->SetImmuneToAll(false);
                me->SetReactState(REACT_AGGRESSIVE);
                ZoneScript* zoneScript = me->GetZoneScript();
                if (Creature* thrall = zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(DATA_THRALL)) : nullptr)
                    AttackStart(thrall);
                break;
            }
            default:
                break;
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE || pointId != POINT_PORTAL)
            return;

        me->CastSpell(me, SPELL_TELEPORT_VISUAL_EVIL);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->CastSpell(me, SPELL_MIGHT_OF_VARIMATHRAS);
        _scheduler.Schedule(14600ms, [this](TaskContext context)
        {
            DoCastSelf(SPELL_SHADOW_BOLT_VOLLEY);
            context.Repeat(14600ms, 27800ms);
        })
        .Schedule(14600ms, [this](TaskContext context)
        {
            DoCastVictim(SPELL_DRAIN_LIFE);
            context.Repeat(14600ms, 27600ms);
        })
        .Schedule(33800ms, [this](TaskContext context)
        {
            DoCastVictim(SPELL_CARRION_SWARM);
            context.Repeat(33800ms, 35200ms);
        })
        .Schedule(27500ms, [this](TaskContext context)
        {
            DoCastSelf(SPELL_SHADOW_NOVA);
            context.Repeat(27500ms, 89100ms);
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_VARIMATHRAS_YEARS_WASTED);
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelAll();
        ScriptedAI::EnterEvadeMode(why);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

private:
    TaskScheduler _scheduler;
    ObjectGuid _portalGUID;
};

// 32518 - Thrall
struct EG_npc_undercity_thrall_alliance_run : public ScriptedAI
{
    EG_npc_undercity_thrall_alliance_run(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        DoAddEvent(14700ms, new Trinity::Helpers::Events::GenericEvent(me, [](WorldObject* owner)
        {
            owner->CastSpell(owner, SPELL_WARCHIEFS_BLESSING_THRONE);
            return true;
        }));
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(7300ms, [this](TaskContext context)
        {
            DoCastVictim(SPELL_LAVA_BURST);
            context.Repeat(7300ms, 19500ms);
        });
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelAll();
        ScriptedAI::EnterEvadeMode(why);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

private:
    TaskScheduler _scheduler;
};

// 32365 - Lady Sylvanas Windrunner
struct EG_npc_undercity_sylvanas_alliance_run : public ScriptedAI
{
    EG_npc_undercity_sylvanas_alliance_run(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void AttackStart(Unit* victim) override
    {
        AttackStartCaster(victim, 25.0f);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(0s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHOOT);
            context.Repeat(2100ms, 17300ms);
        })
        .Schedule(7300ms, [this](TaskContext context)
        {
            DoCastVictim(SPELL_MULTI_SHOT);
            context.Repeat(7300ms, 34900ms);
        })
        .Schedule(8100ms, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHRIEK_OF_THE_HIGHBORNE);
            context.Repeat(8100ms, 28300ms);
        });
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelAll();
        ScriptedAI::EnterEvadeMode(why);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

private:
    TaskScheduler _scheduler;
};

// 32401 - King Varian Wrynn
struct EG_npc_undercity_varian : public EG_npc_undercity_leader
{
    EG_npc_undercity_varian(Creature* creature) : EG_npc_undercity_leader(creature, GOSSIP_MENU_VARIAN, NPC_TEXT_VARIAN_READY, NPC_TEXT_VARIAN_BUSY), _waitingForDreadlords(false) { }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_COUNTDOWN:
                Scheduler.Schedule(1200ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HEROES);
                })
                .Schedule(9300ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HIDDEN_INSIDE);
                })
                .Schedule(124s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_SOON_WE_MARCH);
                })
                .Schedule(186600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HOUR_OF_REDEMPTION);
                });
                break;
            case ACTION_MANHUNT:
                Talk(SAY_VARIAN_MARCH_BEGINS);
                Scheduler.Schedule(4800ms, [this](TaskContext /*context*/)
                {
                    me->CastSpell(me, SPELL_STRENGTH_OF_WRYNN);
                })
                .Schedule(12900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_DESTINY_AWAITS);
                    me->ReplaceAllNpcFlags(UNIT_NPC_FLAG_NONE);
                    me->SetImmuneToNPC(false);
                    StartPath(PATH_VARIAN_WRYNN);
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->DoAction(ACTION_START_PATH);
                });
                break;
            case ACTION_SEWERS_SECURED:
                StopWaves();
                Scheduler.Schedule(400ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_GRAND_APOTHECARY_PUTRESS);
                })
                .Schedule(3s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_FALL_IN);
                    me->SummonCreatureGroup(SUMMON_GROUP_STORMWIND_ELITES);
                })
                .Schedule(9500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_ABERRATIONS);
                })
                .Schedule(17600ms, [this](TaskContext /*context*/)
                {
                    ReleaseJaina(POINT_JAINA_SEWER_POOL);
                    me->ResumeMovement();
                });
                break;
            case ACTION_APOTHECARIUM_SECURED:
                StopWaves();
                ReleaseJaina(POINT_JAINA_PUTRESS);
                Scheduler.Schedule(2300ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_JUSTICE_SERVED);
                })
                .Schedule(7s, [this](TaskContext /*context*/)
                {
                    me->ResumeMovement();
                });
                break;
            default:
                break;
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
            case POINT_VARIAN_SEWERS_ENTRANCE:
                me->PauseMovement();
                Scheduler.Schedule(7s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_DESCENT_BEGINS);
                    me->CastSpell(me, SPELL_HEROIC_VANGUARD_WRYNN);
                })
                .Schedule(15900ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_JAINA_PROUDMOORE, SAY_JAINA_RIGHT_AWAY);
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_BATTLE_STATE, BATTLE_STATE_SEWERS_FIGHT);
                })
                .Schedule(18200ms, [this](TaskContext /*context*/)
                {
                    ReleaseJaina(POINT_JAINA_SEWERS_ENTRANCE);
                    me->ResumeMovement();
                })
                .Schedule(19s, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->DoAction(ACTION_JAINA_BRILLIANCE);
                });
                break;
            case POINT_VARIAN_SEWER_POOL:
                me->PauseMovement();
                Scheduler.Schedule(1300ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_STAND_YOUR_GROUND);
                    StartWaves(SUMMON_GROUP_SEWER_POOL_WAVE_FIRST, SUMMON_GROUP_SEWER_POOL_WAVE_LAST, 5400ms);
                })
                .Schedule(185300ms, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_ENGORGED_BLIGHT_WORM);
                    me->ResumeMovement();
                });
                break;
            case POINT_VARIAN_ENGORGED_BLIGHT_WORM:
                if (GetBattleState() == BATTLE_STATE_SEWERS_FIGHT)
                    me->PauseMovement();
                break;
            case POINT_VARIAN_CANALS:
                me->PauseMovement();
                Scheduler.Schedule(2s, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_JAINA_PROUDMOORE, SAY_JAINA_CORPSES_EVERYWHERE);
                })
                .Schedule(12500ms, [this](TaskContext /*context*/)
                {
                    me->ResumeMovement();
                });
                break;
            case POINT_VARIAN_KHANOK:
                me->PauseMovement();
                Scheduler.Schedule(2900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HORDE_IN_FORCE);
                })
                .Schedule(12600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_STAY_ALERT);
                })
                .Schedule(20700ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_ONWARD);
                })
                .Schedule(22600ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_DO_NOT_LET_THEM_PASS);
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_BATTLE_STATE, BATTLE_STATE_APOTHECARIUM_FIGHT);
                })
                .Schedule(23900ms, [this](TaskContext /*context*/)
                {
                    ReleaseJaina(POINT_JAINA_CANALS);
                    me->ResumeMovement();
                });
                break;
            case POINT_VARIAN_MAIN_CHAMBER:
            {
                me->PauseMovement();
                ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_KILL_THEM);
                std::list<TempSummon*> dreadlords;
                me->SummonCreatureGroup(SUMMON_GROUP_PERFIDIOUS_DREADLORDS, &dreadlords);
                for (TempSummon* dreadlord : dreadlords)
                    dreadlord->AI()->AttackStart(me);
                Scheduler.Schedule(3900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_MAIN_CHAMBER);
                })
                .Schedule(10200ms, [this](TaskContext /*context*/)
                {
                    ReleaseJaina(POINT_JAINA_MAIN_CHAMBER);
                    me->ResumeMovement();
                });
                break;
            }
            case POINT_VARIAN_DREADLORDS:
                if (std::none_of(Summons.begin(), Summons.end(), [this](ObjectGuid const& guid)
                {
                    Creature* summon = ObjectAccessor::GetCreature(*me, guid);
                    return summon && summon->GetEntry() == NPC_PERFIDIOUS_DREADLORD && summon->IsAlive();
                }))
                    break;

                _waitingForDreadlords = true;
                me->PauseMovement();
                Scheduler.Schedule(3min, [this](TaskContext /*context*/)
                {
                    if (!_waitingForDreadlords)
                        return;

                    _waitingForDreadlords = false;
                    me->ResumeMovement();
                });
                break;
            case POINT_VARIAN_THERE:
                me->PauseMovement();
                Scheduler.Schedule(2700ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_THERE);
                })
                .Schedule(5800ms, [this](TaskContext /*context*/)
                {
                    me->ResumeMovement();
                });
                break;
            case POINT_VARIAN_NOWHERE_TO_RUN:
                me->PauseMovement();
                Scheduler.Schedule(1600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_NOWHERE_TO_RUN);
                    StartWaves(SUMMON_GROUP_APOTHECARIUM_WAVE_FIRST, SUMMON_GROUP_APOTHECARIUM_WAVE_LAST, 5700ms);
                })
                .Schedule(6400ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_STARE_INTO_THE_ABYSS);
                })
                .Schedule(14500ms, [this](TaskContext /*context*/)
                {
                    me->ResumeMovement();
                })
                .Schedule(23700ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_NO_MATCH);
                })
                .Schedule(44600ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_CANNOT_BE_HAPPENING);
                })
                .Schedule(66400ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_WILL_NOT_END);
                })
                .Schedule(77700ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_POWER_WASHES);
                })
                .Schedule(84200ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_GRAND_APOTHECARY_PUTRESS, SAY_PUTRESS_MORE_SUFFERING);
                })
                .Schedule(90200ms, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        putress->AI()->DoAction(ACTION_PUTRESS_ATTACK);
                });
                break;
            case POINT_VARIAN_PUTRESS:
                if (GetBattleState() == BATTLE_STATE_APOTHECARIUM_FIGHT)
                    me->PauseMovement();
                break;
            case POINT_VARIAN_WHAT_SAY_YOU_NOW:
                me->PauseMovement();
                Scheduler.Schedule(2600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_WHAT_SAY_YOU_NOW);
                })
                .Schedule(9s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_LOOK_AROUND);
                    me->ResumeMovement();
                });
                break;
            case POINT_VARIAN_LOOK_AROUND:
                me->PauseMovement();
                Scheduler.Schedule(9400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HOW_MUCH_LONGER);
                })
                .Schedule(15900ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HORDES_WORLD);
                })
                .Schedule(24s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_EVIL_IN_ORCS);
                })
                .Schedule(32s, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_THRONE_HORDE);
                    me->GetMap()->SpawnGroupSpawn(SPAWN_GROUP_ALLIANCE_THRONE, true);
                })
                .Schedule(37400ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_THRALL_THRONE, SAY_THRALL_THRONE_VICTORIOUS);
                })
                .Schedule(42300ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_THRALL_THRONE, SAY_THRALL_THRONE_LOKTAR);
                })
                .Schedule(48400ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_THRALL_HERE);
                })
                .Schedule(51600ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_END_THIS_NOW);
                })
                .Schedule(54800ms, [this](TaskContext /*context*/)
                {
                    me->SetImmuneToNPC(true);
                    me->ResumeMovement();
                })
                .Schedule(56100ms, [this](TaskContext /*context*/)
                {
                    ActorTalk(DATA_JAINA_PROUDMOORE, SAY_JAINA_WONT_HELP_YOU);
                });
                break;
            case POINT_VARIAN_THRONE_HALL:
                me->PauseMovement();
                Scheduler.Schedule(10s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_LOKTAR_OGAR);
                })
                .Schedule(23s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_TO_THE_THRONE_ROOM);
                    me->ResumeMovement();
                });
                break;
            default:
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        EG_npc_undercity_leader::JustSummoned(summon);

        if (summon->GetEntry() != NPC_ENGORGED_BLIGHT_WORM)
            return;

        Scheduler.Schedule(4700ms, [this, wormGUID = summon->GetGUID()](TaskContext /*context*/)
        {
            if (Creature* worm = ObjectAccessor::GetCreature(*me, wormGUID))
                worm->AI()->Talk(SAY_ENGORGED_BLIGHT_WORM_UNLEASHED);
        });
        summon->AI()->AttackStart(me);
        me->EngageWithTarget(summon);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() != NPC_PERFIDIOUS_DREADLORD || !_waitingForDreadlords)
            return;

        if (std::any_of(Summons.begin(), Summons.end(), [this](ObjectGuid const& guid)
        {
            Creature* dreadlord = ObjectAccessor::GetCreature(*me, guid);
            return dreadlord && dreadlord->GetEntry() == NPC_PERFIDIOUS_DREADLORD && dreadlord->IsAlive();
        }))
            return;

        _waitingForDreadlords = false;
        me->ResumeMovement();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Combat.Schedule(8100ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastSelf(SPELL_WHIRLWIND);
            context.Repeat(8100ms, 16900ms);
        });
    }

protected:
    void PathEnded() override
    {
        Scheduler.Schedule(800ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_AWAY_TOO_LONG);
        })
        .Schedule(17100ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_MAKE_THINGS_RIGHT);
        })
        .Schedule(30100ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_WAITED_LONG);
        })
        .Schedule(46300ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_IT_ENDS_NOW);
        })
        .Schedule(59300ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_ATTACK);
            me->SetImmuneToAll(false);
            for (ObjectGuid const& guid : Summons)
            {
                Creature* defender = ObjectAccessor::GetCreature(*me, guid);
                if (!defender || (defender->GetEntry() != NPC_THRALL_THRONE && defender->GetEntry() != NPC_SYLVANAS_THRONE))
                    continue;

                defender->SetImmuneToAll(false);
                defender->SetReactState(REACT_AGGRESSIVE);
                defender->AI()->AttackStart(me);
            }

            if (ZoneScript* zoneScript = me->GetZoneScript())
                zoneScript->SetData(DATA_THRONE_DEFENDERS, 0);
        })
        .Schedule(67s, [this](TaskContext /*context*/)
        {
            ActorTalk(DATA_JAINA_PROUDMOORE, SAY_JAINA_VARIAN_NO);
        })
        .Schedule(71800ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                jaina->AI()->DoAction(ACTION_JAINA_THRONE);
        })
        .Schedule(73400ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                jaina->CastSpell(jaina, SPELL_DEEP_FREEZE_CREDIT);
        })
        .Schedule(79900ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                jaina->CastSpell(jaina, SPELL_GREATER_MASS_TELEPORT);
            if (ZoneScript* zoneScript = me->GetZoneScript())
                zoneScript->SetData(DATA_RESET_RUN, 0);
        })
        .Schedule(80s, [this](TaskContext /*context*/)
        {
            ActorTalk(DATA_JAINA_PROUDMOORE, SAY_JAINA_DID_NOT_HAVE_TO_BE);
        });
    }

private:
    void ReleaseJaina(uint32 pointId)
    {
        if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
            jaina->AI()->SetData(DATA_RELEASE_POINT, pointId);
    }

    bool _waitingForDreadlords;
};

// 32402 - Lady Jaina Proudmoore
struct EG_npc_undercity_jaina : public EG_npc_undercity_companion
{
    EG_npc_undercity_jaina(Creature* creature) : EG_npc_undercity_companion(creature, PATH_JAINA_PROUDMOORE, DATA_VARIAN_WRYNN, { POINT_JAINA_SEWERS_ENTRANCE, POINT_JAINA_SEWER_POOL, POINT_JAINA_CANALS, POINT_JAINA_MAIN_CHAMBER, POINT_JAINA_PUTRESS, POINT_JAINA_APOTHECARIUM }) { }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_JAINA_BRILLIANCE:
                me->CastSpell(me, SPELL_IMPROVED_BRILLIANCE_AURA);
                break;
            case ACTION_JAINA_THRONE:
            {
                EnterEvadeMode(EVADE_REASON_OTHER);
                me->SetImmuneToAll(true);
                me->SetReactState(REACT_PASSIVE);
                WaypointPath const* path = sWaypointMgr->GetPath(PATH_JAINA_PROUDMOORE);
                if (!path || path->nodes.empty())
                    break;

                WaypointNode const& node = path->nodes.back();
                me->NearTeleportTo(node.x, node.y, node.z, me->GetOrientation());
                break;
            }
            default:
                EG_npc_undercity_companion::DoAction(action);
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Combat.Schedule(1500ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_FIREBALL);
            context.Repeat(1500ms, 11500ms);
        })
        .Schedule(4800ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_FIREBALL_FAST);
            context.Repeat(4800ms, 11400ms);
        })
        .Schedule(6400ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_FIRE_BLAST);
            context.Repeat(6400ms, 21200ms);
        })
        .Schedule(9800ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BLIZZARD);
            context.Repeat(9800ms, 25800ms);
        })
        .Schedule(21100ms, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastSelf(SPELL_SUMMON_WATER_ELEMENTALS);
            context.Repeat(21100ms, 25900ms);
        });
    }
};

// 31530 - Grand Apothecary Putress
struct EG_npc_undercity_putress : public ScriptedAI
{
    EG_npc_undercity_putress(Creature* creature) : ScriptedAI(creature)
    {
        _combat.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void IsSummonedBy(WorldObject* /*summoner*/) override
    {
        me->SetImmuneToAll(true);
        me->SetReactState(REACT_PASSIVE);
        me->CastSpell(me, SPELL_PUTRESS_CASTING_STATE);
        DoAddEvent(2s, new Trinity::Helpers::Events::GenericEvent(me, [](WorldObject* owner)
        {
            owner->CastSpell(owner, SPELL_PUTRESS_TESLA_AURA);
            return true;
        }));
    }

    void DoAction(int32 action) override
    {
        if (action != ACTION_PUTRESS_ATTACK)
            return;

        me->RemoveAurasDueToSpell(SPELL_PUTRESS_CASTING_STATE);
        me->CastSpell(me, SPELL_BLIGHT_OVERLOAD);
        me->SetImmuneToAll(false);
        me->SetReactState(REACT_AGGRESSIVE);
        ZoneScript* zoneScript = me->GetZoneScript();
        if (Creature* varian = zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(DATA_VARIAN_WRYNN)) : nullptr)
            AttackStart(varian);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _combat.Schedule(4s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_THROW_BLIGHT_BARREL);
            context.Repeat(14200ms, 23s);
        })
        .Schedule(94s, [this](TaskContext context)
        {
            if (Creature* experiment = me->FindNearestCreatureWithOptions(40.0f, { .CreatureId = NPC_FAILED_EXPERIMENT, .IsAlive = true }))
                DoCast(experiment, SPELL_UNHOLY_FRENZY);
            context.Repeat(11400ms, 19400ms);
        });
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _combat.CancelAll();
        ScriptedAI::EnterEvadeMode(why);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _combat.Update(diff);
        DoMeleeAttackIfReady();
    }

private:
    TaskScheduler _combat;
};

// 32483 - Engorged Blight Worm
struct EG_npc_undercity_blight_worm : public ScriptedAI
{
    EG_npc_undercity_blight_worm(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
    }

    void IsSummonedBy(WorldObject* /*summoner*/) override
    {
        me->CastSpell(me, SPELL_BLIGHT_EXPLOSION);
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

// 32363 - Thrall
struct EG_npc_undercity_thrall_orgrimmar : public ScriptedAI
{
    EG_npc_undercity_thrall_orgrimmar(Creature* creature) : ScriptedAI(creature), _sceneInProgress(false) { }

    void MoveInLineOfSight(Unit* who) override
    {
        ScriptedAI::MoveInLineOfSight(who);

        Player* player = who->ToPlayer();
        if (_sceneInProgress || me->IsEngaged() || !player || me->IsHostileTo(player) || player->GetQuestStatus(QUEST_FATE_UP_AGAINST_YOUR_WILL) != QUEST_STATUS_COMPLETE || !me->IsWithinDistInMap(player, 50.0f) || !me->IsWithinLOSInMap(player))
            return;

        _sceneInProgress = true;
        me->SummonCreatureGroup(SUMMON_GROUP_JAINA_ORGRIMMAR);
        Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid);

        std::list<Creature*> guards;
        me->GetCreatureListWithOptionsInGrid(guards, 50.0f, { .CreatureId = NPC_KORKRON_ELITE_ORGRIMMAR });
        _korkronGuids.clear();
        for (Creature* guard : guards)
        {
            _korkronGuids.push_back(guard->GetGUID());
            guard->SetWalk(false);
            if (jaina)
                guard->GetMotionMaster()->MovePoint(0, jaina->GetPosition());
        }

        if (jaina)
            jaina->SetWalk(true);

        if (Creature* sylvanas = me->FindNearestCreatureWithOptions(50.0f, { .CreatureId = NPC_SYLVANAS_ORGRIMMAR }))
        {
            _sylvanasGuid = sylvanas->GetGUID();
            _sylvanasHome = sylvanas->GetHomePosition();
            sylvanas->SetWalk(true);
            sylvanas->SetStandState(UNIT_STAND_STATE_STAND);
        }

        Talk(SAY_THRALL_ORGRIMMAR_STAND_DOWN);

        _scheduler.Schedule(1700ms, [this](TaskContext /*context*/)
        {
            for (ObjectGuid const& guid : _korkronGuids)
                if (Creature* guard = ObjectAccessor::GetCreature(*me, guid))
                    guard->AI()->EnterEvadeMode();
        })
        .Schedule(5700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_JAINA);
        })
        .Schedule(6700ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid))
                jaina->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE_ORGRIMMAR, false);
        })
        .Schedule(10700ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid))
                jaina->AI()->Talk(SAY_JAINA_ORGRIMMAR_WHAT_HAPPENED);
        })
        .Schedule(16700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_BETRAYAL);
        })
        .Schedule(22700ms, [this](TaskContext /*context*/)
        {
            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGuid))
                sylvanas->GetMotionMaster()->MovePath(PATH_SYLVANAS_ORGRIMMAR, false);
        })
        .Schedule(27700ms, [this](TaskContext /*context*/)
        {
            Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGuid);
            Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid);
            if (sylvanas && jaina)
                sylvanas->SetFacingToObject(jaina);
        })
        .Schedule(28700ms, [this](TaskContext /*context*/)
        {
            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGuid))
                sylvanas->AI()->Talk(SAY_SYLVANAS_ORGRIMMAR_TRUTH);
        })
        .Schedule(38700ms, [this](TaskContext /*context*/)
        {
            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGuid))
                sylvanas->AI()->Talk(SAY_SYLVANAS_ORGRIMMAR_UPRISING);
        })
        .Schedule(57700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_UNDERCITY_LOST);
        })
        .Schedule(62700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_LAY_SIEGE);
        })
        .Schedule(71700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_DIVIDED_FORCES);
        })
        .Schedule(78700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_MAKE_THIS_RIGHT);
        })
        .Schedule(84700ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid))
                jaina->AI()->Talk(SAY_JAINA_ORGRIMMAR_DELIVER_INFORMATION);
        })
        .Schedule(88700ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid))
                jaina->AI()->Talk(SAY_JAINA_ORGRIMMAR_BOLVAR);
        })
        .Schedule(104700ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid))
                jaina->AI()->Talk(SAY_JAINA_ORGRIMMAR_RAGE);
        })
        .Schedule(115700ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid))
                jaina->AI()->Talk(SAY_JAINA_ORGRIMMAR_FAREWELL);
        })
        .Schedule(123700ms, [this](TaskContext /*context*/)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jainaGuid))
                jaina->GetMotionMaster()->MovePoint(0, _jainaHome);
        })
        .Schedule(126700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_PREPARE_TRANSPORT);
        })
        .Schedule(127700ms, [this](TaskContext /*context*/)
        {
            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGuid))
            {
                sylvanas->SetHomePosition(_sylvanasHome);
                sylvanas->AI()->EnterEvadeMode();
            }
        })
        .Schedule(170s, [this](TaskContext /*context*/)
        {
            _sceneInProgress = false;
        });
    }

    void JustSummoned(Creature* summon) override
    {
        _jainaGuid = summon->GetGUID();
        _jainaHome = summon->GetPosition();
        summon->CastSpell(summon, SPELL_AIR_REVENANT_ENTRANCE);
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    TaskScheduler _scheduler;
    ObjectGuid _jainaGuid;
    ObjectGuid _sylvanasGuid;
    Position _jainaHome;
    Position _sylvanasHome;
    GuidVector _korkronGuids;
    bool _sceneInProgress;
};

// 59647 - Warchief's Blessing
class EG_spell_undercity_warchiefs_blessing : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_warchiefs_blessing);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WARCHIEFS_BLESSING_PLAYER });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* player = GetHitPlayer())
            player->CastSpell(player, SPELL_WARCHIEFS_BLESSING_PLAYER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_warchiefs_blessing::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60101 - Hall Blown Up
class EG_spell_undercity_hall_blown_up : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_hall_blown_up);

    void HandleActivateObject(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        if (GameObject* caveIn = GetHitGObj())
            caveIn->UseDoorOrButton(0, false, GetCaster());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_hall_blown_up::HandleActivateObject, EFFECT_1, SPELL_EFFECT_ACTIVATE_OBJECT);
    }
};

// 60104 - Grip of Darkness
class EG_spell_undercity_grip_of_darkness : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_grip_of_darkness);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        WorldLocation const* dest = GetHitDest();
        if (!dest)
            return;

        Unit* target = GetHitUnit();
        target->GetMotionMaster()->MoveJump(*dest, 50.0f, Movement::gravity * target->GetExactDist(*dest) / 100.0f);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_grip_of_darkness::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60207 - Call of Earth
class EG_spell_undercity_call_of_earth : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_call_of_earth);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (ZoneScript* zoneScript = GetCaster()->GetZoneScript())
            zoneScript->SetData(DATA_HALL_REOPEN, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_call_of_earth::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60285 - Portal Collapse
class EG_spell_undercity_portal_collapse : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_portal_collapse);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_OPENING_LEGION_PORTALS, GetCaster()->GetGUID());
        if (Creature* portal = GetCaster()->ToCreature())
            portal->DespawnOrUnsummon(1s, 1h);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_portal_collapse::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60496 - Roll Putress Head
class EG_spell_undercity_roll_putress_head : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_roll_putress_head);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SEVERED_HEAD_OF_PUTRESS });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SEVERED_HEAD_OF_PUTRESS, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_roll_putress_head::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60516 - Greater Mass Teleport
class EG_spell_undercity_greater_mass_teleport : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_greater_mass_teleport);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->DespawnOrUnsummon(1700ms);
    }

    void HandleAfterCast()
    {
        if (Creature* caster = GetCaster()->ToCreature())
            caster->DespawnOrUnsummon(6100ms);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_greater_mass_teleport::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        AfterCast += SpellCastFn(EG_spell_undercity_greater_mass_teleport::HandleAfterCast);
    }
};

void AddSC_EG_battle_for_undercity()
{
    RegisterCreatureAI(EG_npc_undercity_thrall);
    RegisterCreatureAI(EG_npc_undercity_sylvanas);
    RegisterCreatureAI(EG_npc_undercity_varimathras);
    RegisterCreatureAI(EG_npc_undercity_thrall_alliance_run);
    RegisterCreatureAI(EG_npc_undercity_sylvanas_alliance_run);
    RegisterCreatureAI(EG_npc_undercity_varian);
    RegisterCreatureAI(EG_npc_undercity_jaina);
    RegisterCreatureAI(EG_npc_undercity_putress);
    RegisterCreatureAI(EG_npc_undercity_blight_worm);
    RegisterCreatureAI(EG_npc_undercity_thrall_orgrimmar);
    RegisterSpellScript(EG_spell_undercity_warchiefs_blessing);
    RegisterSpellScript(EG_spell_undercity_hall_blown_up);
    RegisterSpellScript(EG_spell_undercity_grip_of_darkness);
    RegisterSpellScript(EG_spell_undercity_call_of_earth);
    RegisterSpellScript(EG_spell_undercity_portal_collapse);
    RegisterSpellScript(EG_spell_undercity_roll_putress_head);
    RegisterSpellScript(EG_spell_undercity_greater_mass_teleport);
    new EG_zone_undercity_battle_horde();
    new EG_zone_undercity_battle_alliance();
}
