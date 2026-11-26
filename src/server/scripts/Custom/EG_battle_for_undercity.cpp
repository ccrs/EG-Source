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
#include "GameObject.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TaskScheduler.h"
#include "TemporarySummon.h"
#include "WaypointManager.h"
#include "WorldStatePackets.h"
#include <algorithm>
#include <array>
#include <functional>
#include <list>
#include <map>
#include <set>
#include <utility>

enum BattleForTheUndercityMisc
{
    MAP_EASTERN_KINGDOMS = 0,
    ZONE_TIRISFAL_GLADES = 85,
    ZONE_UNDERCITY = 1497,
    PHASE_HORDE = 64,
    PHASE_ALLIANCE = 128,

    QUEST_BATTLE_FOR_THE_UNDERCITY_HORDE = 13267,
    QUEST_FATE_UP_AGAINST_YOUR_WILL = 13369,

    NPC_THRALL = 32518,
    NPC_SYLVANAS = 32365,
    NPC_WARSONG_BATTLEGUARD = 31739,
    NPC_VARIMATHRAS = 31565,
    NPC_VARIMATHRAS_PORTAL = 31811,
    NPC_BLIGHT_ABERRATION = 31844,
    NPC_TREACHEROUS_GUARDIAN = 31532,
    NPC_BLIGHT_DOCTOR = 31516,
    NPC_APOTHECARY_CHEMIST = 31482,
    NPC_BLIGHT_SLINGER = 31526,
    NPC_FELGUARD_MARAUDER = 31527,
    NPC_PERFIDIOUS_DREADLORD = 31531,
    NPC_RAVISHING_BETRAYER = 31529,
    NPC_PLAGUED_FELBEAST = 31528,
    NPC_DOOMGUARD_PILLAGER = 32159,
    NPC_KHANOK = 32511,
    NPC_LEGION_INVADER = 32269,
    NPC_LEGION_DREADWHISPERER = 32270,
    NPC_LEGION_OVERLORD = 32271,
    NPC_VARIAN_WRYNN_HORDE_RUN = 32303,
    NPC_JAINA_PROUDMOORE_HORDE_RUN = 31418,
    NPC_STORMWIND_ELITE = 32387,
    NPC_SAURFANG = 32315,
    NPC_VARIAN_WRYNN = 32401,
    NPC_JAINA_PROUDMOORE = 32402,
    NPC_ENGORGED_BLIGHT_WORM = 32483,
    NPC_GRAND_APOTHECARY_PUTRESS = 31530,
    NPC_CRAZED_APOTHECARY_GENERATOR = 36212,
    NPC_WARSONG_BATTLEGUARD_THRONE = 32510,
    NPC_FAILED_EXPERIMENT = 32519,
    NPC_SYLVANAS_ORGRIMMAR = 31419,
    NPC_KORKRON_ELITE_ORGRIMMAR = 32367,

    GO_BLOCKED_PASSAGE = 194935,

    SPELL_WARCHIEFS_BLESSING = 64670,
    SPELL_CALL_OF_AIR = 59898,
    SPELL_TIDAL_WAVE = 59627,
    SPELL_HEROIC_VANGUARD = 59506,
    SPELL_CHAIN_LIGHTNING = 59517,
    SPELL_LAVA_BURST = 59519,
    SPELL_THUNDER = 59507,
    SPELL_LEGION_PORTAL = 59680,
    SPELL_SONG_OF_SYLVANAS = 59756,
    SPELL_BLACK_ARROW = 59712,
    SPELL_SHOOT = 59710,
    SPELL_MULTI_SHOT = 59713,
    SPELL_SHRIEK_OF_THE_HIGHBORNE = 59514,
    SPELL_CALL_OF_EARTH = 60207,
    SPELL_AURA_OF_VARIMATHRAS = 60289,
    SPELL_OPENING_LEGION_PORTALS = 60224,
    SPELL_LEAP_TO_PLATFORM = 56347,
    SPELL_DEEP_FREEZE_STUN = 60511,
    SPELL_GREATER_MASS_TELEPORT_DUMMY = 60516,
    SPELL_STRENGTH_OF_WRYNN = 60964,
    SPELL_IMPROVED_BRILLIANCE_AURA = 61011,
    SPELL_WHIRLWIND = 41056,
    SPELL_HEROIC_LEAP = 57793,
    SPELL_FIREBALL = 20692,
    SPELL_BLIZZARD = 20680,
    SPELL_SUMMON_WATER_ELEMENTALS = 20681,
    SPELL_INGEST = 61123,
    SPELL_INGEST_VEHICLE = 61124,
    SPELL_BLIGHT_BREATH = 61125,
    SPELL_PUTRESS_CASTING_STATE = 59447,
    SPELL_BLIGHT_EMPOWERMENT = 59449,
    SPELL_BLIGHT_OVERLOAD = 61181,
    SPELL_DEEP_FREEZE_CREDIT = 61224,
    SPELL_GREATER_MASS_TELEPORT = 61225,
    SPELL_AIR_REVENANT_ENTRANCE = 55761,

    EVENT_CALL_OF_AIR = 20512,

    SPAWN_GROUP_HORDE_RUN_ACTORS = 1029,
    SPAWN_GROUP_HORDE_STAGING = 1030,
    SPAWN_GROUP_HORDE_COURTYARD_BANNERS = 1031,
    SPAWN_GROUP_HORDE_BLOCKED_PASSAGES = 1032,

    SUMMON_GROUP_GREAT_WIND_VORTEX_1 = 0,
    SUMMON_GROUP_GREAT_WIND_VORTEX_2 = 1,
    SUMMON_GROUP_COURTYARD_DEFENDERS = 2,
    SUMMON_GROUP_VARIMATHRAS_COURTYARD = 3,
    SUMMON_GROUP_COURTYARD_WAVE = 4,
    SUMMON_GROUP_BLIGHT_ABERRATION = 5,
    SUMMON_GROUP_COURTYARD_BATTLEGUARDS = 6,
    SUMMON_GROUP_CAVE_IN_DUMMIES = 7,
    SUMMON_GROUP_SHAFT_BATTLEGUARDS = 8,
    SUMMON_GROUP_LOWER_CITY_WAVE_1 = 9,
    SUMMON_GROUP_LOWER_CITY_WAVE_2 = 10,
    SUMMON_GROUP_LOWER_CITY_WAVE_3 = 11,
    SUMMON_GROUP_VARIMATHRAS_INNER_SANCTUM = 12,
    SUMMON_GROUP_INNER_SANCTUM_WAVE_1 = 13,
    SUMMON_GROUP_INNER_SANCTUM_WAVE_2 = 14,
    SUMMON_GROUP_DOOMGUARD_PILLAGER = 15,
    SUMMON_GROUP_KHANOK = 16,
    SUMMON_GROUP_BATTLEGUARD_CHARGE = 17,
    SUMMON_GROUP_VARIMATHRAS_ROYAL_QUARTER = 18,
    SUMMON_GROUP_LEGION_PORTALS = 19,
    SUMMON_GROUP_ROYAL_QUARTER_WAVE_1 = 20,
    SUMMON_GROUP_ROYAL_QUARTER_WAVE_2 = 21,
    SUMMON_GROUP_ALLIANCE_INTRUSION = 22,
    SUMMON_GROUP_SAURFANG = 23,

    SUMMON_GROUP_SEWERS_GUARDIAN = 0,
    SUMMON_GROUP_SEWERS_WAVE_1 = 1,
    SUMMON_GROUP_SEWERS_WAVE_2 = 2,
    SUMMON_GROUP_SEWERS_WAVE_3 = 3,
    SUMMON_GROUP_SEWERS_WAVE_4 = 4,
    SUMMON_GROUP_SEWERS_WAVE_5 = 5,
    SUMMON_GROUP_SEWERS_BOSSES = 6,
    SUMMON_GROUP_STORMWIND_ELITE_1 = 7,
    SUMMON_GROUP_APOTHECARIUM_GUARDIANS = 11,
    SUMMON_GROUP_PERFIDIOUS_DREADLORD = 12,
    SUMMON_GROUP_FAILED_EXPERIMENTS = 13,
    SUMMON_GROUP_THRONE_HORDE = 14,
    SUMMON_GROUP_THRONE_ALLIANCE = 15,

    SUMMON_GROUP_JAINA_ORGRIMMAR = 0,

    PATH_THRALL = 3251800,
    PATH_SYLVANAS_GATE = 3236500,
    PATH_SYLVANAS_COURTYARD = 3236501,
    PATH_GREAT_WIND_VORTEX_1 = 3178200,
    PATH_GREAT_WIND_VORTEX_2 = 3178201,
    PATH_BATTLEGUARD_CHARGE = 3173900,
    PATH_VARIAN_WRYNN_HORDE_RUN = 3230300,
    PATH_JAINA_PROUDMOORE_HORDE_RUN = 3141800,
    PATH_SAURFANG = 3231500,
    PATH_VARIAN_WRYNN = 3240100,
    PATH_JAINA_PROUDMOORE = 3240200,
    PATH_JAINA_PROUDMOORE_CANALS = 3240201,
    PATH_JAINA_PROUDMOORE_APOTHECARIUM = 3240202,
    PATH_JAINA_PROUDMOORE_THRONE_HALL = 3240203,
    PATH_JAINA_PROUDMOORE_THRONE = 3240204,
    PATH_STORMWIND_ELITE_1 = 3238700,
    PATH_PERFIDIOUS_DREADLORD = 3239100,
    PATH_JAINA_PROUDMOORE_ORGRIMMAR = 3236400,
    PATH_SYLVANAS_ORGRIMMAR = 3141900,

    POINT_THRALL_GATE = 2,
    POINT_THRALL_COURTYARD_ENTRANCE = 11,
    POINT_THRALL_COURTYARD = 13,
    POINT_THRALL_COURTYARD_SECURED = 14,
    POINT_THRALL_ELEVATOR = 34,
    POINT_THRALL_SHAFT_BOTTOM = 35,
    POINT_THRALL_INNER_SANCTUM = 36,
    POINT_THRALL_LOWER_CITY = 46,
    POINT_THRALL_LOWER_CITY_AMBUSH_1 = 57,
    POINT_THRALL_LOWER_CITY_AMBUSH_2 = 61,
    POINT_THRALL_KHANOK = 65,
    POINT_THRALL_KHANOK_DEFEATED = 66,
    POINT_THRALL_ROYAL_QUARTER_APPROACH = 75,
    POINT_THRALL_COLLAPSED_HALL = 81,
    POINT_THRALL_THRONE_ROOM = 104,
    POINT_THRALL_VICTORY = 109,
    POINT_THRALL_SYLVANAS_PLATFORM = 113,
    POINT_THRALL_ALLIANCE_HORNS = 117,
    POINT_THRALL_LAMENT = 118,
    POINT_THRALL_SAURFANG = 120,

    POINT_VARIAN_SEWERS_ENTRANCE = 2,
    POINT_VARIAN_SEWERS = 38,
    POINT_VARIAN_CANALS = 45,
    POINT_VARIAN_KHANOK = 46,
    POINT_VARIAN_PUTRESS_WARNING = 48,
    POINT_VARIAN_MAIN_CHAMBER = 50,
    POINT_VARIAN_THERE = 63,
    POINT_VARIAN_PUTRESS = 65,
    POINT_VARIAN_APOTHECARIUM_SECURED = 66,
    POINT_VARIAN_THRONE_HALL = 87,
    POINT_VARIAN_THRONE = 88,

    SAY_THRALL_PREPARATION_1 = 2,
    SAY_THRALL_PREPARATION_2,
    SAY_THRALL_PREPARATION_3,
    SAY_THRALL_PREPARATION_4,
    SAY_THRALL_PREPARATION_5,
    SAY_THRALL_PREPARATION_6,
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

    SAY_BATTLEGUARD_BURN = 0,
    SAY_BATTLEGUARD_PUTRESS,
    SAY_BATTLEGUARD_FOR_THE_HORDE,

    SAY_SYLVANAS_SHAFT_TRAPPED = 1,
    SAY_SYLVANAS_BEAUTIFUL_CITY,
    SAY_SYLVANAS_REDEMPTION,
    SAY_SYLVANAS_ROYAL_QUARTER_THIS_WAY,
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

    SAY_VARIAN_HORDE_RUN_AWAY_TOO_LONG = 0,
    SAY_VARIAN_HORDE_RUN_MAKE_THINGS_RIGHT,
    SAY_VARIAN_HORDE_RUN_WAITED_LONG,
    SAY_VARIAN_HORDE_RUN_IT_ENDS_NOW,

    SAY_JAINA_HORDE_RUN_VARIAN_NO = 5,
    SAY_JAINA_HORDE_RUN_DID_NOT_HAVE_TO_BE,

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

    SAY_PUTRESS_DO_NOT_LET_THEM_PASS = 0,
    SAY_PUTRESS_KILL_THEM,
    SAY_PUTRESS_STARE_INTO_THE_ABYSS,
    SAY_PUTRESS_NO_MATCH,
    SAY_PUTRESS_CANNOT_BE_HAPPENING,
    SAY_PUTRESS_WILL_NOT_END,
    SAY_PUTRESS_POWER_WASHES,
    SAY_PUTRESS_MORE_SUFFERING,

    SAY_THRALL_ALLIANCE_RUN_VICTORIOUS = 0,
    SAY_THRALL_ALLIANCE_RUN_LOKTAR,

    SAY_KHANOK_NO_HOPE = 0,

    SAY_SAURFANG_WHAT_HE_WOULD_DO = 0,
    SAY_SAURFANG_LEAD_YOUR_PEOPLE,
    SAY_SAURFANG_LETS_GO_HOME,

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

    DATA_RUN_STATE = 1,
    DATA_THRALL,
    DATA_VARIAN_WRYNN,
    DATA_SYLVANAS,
    DATA_VARIMATHRAS,
    DATA_VARIMATHRAS_PORTAL,
    DATA_VARIAN_WRYNN_HORDE_RUN,
    DATA_JAINA_PROUDMOORE_HORDE_RUN,
    DATA_SAURFANG,
    DATA_WARSONG_BATTLEGUARD,
    DATA_BLOCKED_PASSAGE_1,
    DATA_BLOCKED_PASSAGE_2,
    DATA_BLOCKED_PASSAGE_3,
    DATA_THRALL_ALLIANCE_RUN,
    DATA_JAINA_PROUDMOORE,
    DATA_GRAND_APOTHECARY_PUTRESS,
    DATA_KHANOK,

    ACTION_ONWARD = 1,
    ACTION_COURTYARD_CONTROLLED,
    ACTION_INNER_SANCTUM_CONTROLLED,
    ACTION_ROYAL_QUARTER_CONTROLLED,
    ACTION_MANHUNT,
    ACTION_SEWERS_SECURED,
    ACTION_APOTHECARIUM_SECURED,
    ACTION_JAINA_FOLLOW,
    ACTION_JAINA_HOLD,
    ACTION_SYLVANAS_GATE,
    ACTION_SYLVANAS_COURTYARD,
    ACTION_SYLVANAS_FOLLOW,

    RUN_STATE_IDLE = 0,
    RUN_STATE_COUNTDOWN,
    RUN_STATE_BEGUN,
    RUN_STATE_COURTYARD_FIGHT,
    RUN_STATE_COURTYARD_CONTROLLED,
    RUN_STATE_INNER_SANCTUM_FIGHT,
    RUN_STATE_INNER_SANCTUM_CONTROLLED,
    RUN_STATE_ROYAL_QUARTER_FIGHT,
    RUN_STATE_ROYAL_QUARTER_CONTROLLED,
    RUN_STATE_SEWERS_FIGHT,
    RUN_STATE_SEWERS_SECURED,
    RUN_STATE_APOTHECARIUM_FIGHT,
    RUN_STATE_APOTHECARIUM_SECURED,

    GROUP_COUNTDOWN = 1,
    GROUP_EMPTY_RUN,
    GROUP_COMBAT,
    GROUP_INNER_SANCTUM_WAVES,
    GROUP_ROYAL_QUARTER_WAVES,
    GROUP_SEWERS_WAVES
};

class EG_zone_undercity_battle : public WorldZoneScript
{
protected:
    EG_zone_undercity_battle(char const* name, uint32 phaseMask, std::map<uint32, uint32> idleWorldStates) : WorldZoneScript(name, MAP_EASTERN_KINGDOMS, { ZONE_TIRISFAL_GLADES, ZONE_UNDERCITY }, phaseMask), RunState(RUN_STATE_IDLE), _idleWorldStates(idleWorldStates), _worldStates(std::move(idleWorldStates)), _cleanupPending(false) { }

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
        return type == DATA_RUN_STATE ? RunState : 0;
    }

    void OnPlayerEnter(Player* /*player*/, uint32 /*zoneId*/) override
    {
        _scheduler.CancelGroup(GROUP_EMPTY_RUN);
    }

    void OnPlayerLeave(Player* /*player*/, uint32 /*zoneId*/) override
    {
        if (RunState == RUN_STATE_IDLE || !GetPlayers().empty())
            return;

        _scheduler.Schedule(10min, GROUP_EMPTY_RUN, [this](TaskContext /*context*/)
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
        _scheduler.Update(diff);

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
        _scheduler.Schedule(62s, GROUP_COUNTDOWN, [this, minutesWorldStateId, finish](TaskContext context)
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
        RunState = RUN_STATE_IDLE;
        _scheduler.CancelAll();
        _cleanupPending = true;

        for (auto const& [worldStateId, value] : _idleWorldStates)
            SetWorldState(worldStateId, value);
    }

    virtual void OnRunEnd(Map* /*map*/) { }

    uint32 RunState;

private:
    std::map<uint32, uint32> const _idleWorldStates;
    std::map<uint32, uint32> _worldStates;
    std::set<uint32> _changedWorldStates;
    GuidUnorderedSet _summonGUIDs;
    TaskScheduler _scheduler;
    bool _cleanupPending;
};

class EG_zone_undercity_battle_horde : public EG_zone_undercity_battle
{
public:
    EG_zone_undercity_battle_horde() : EG_zone_undercity_battle("EG_zone_undercity_battle_horde", PHASE_HORDE, { { WORLD_STATE_HORDE_BATTLE_BEGUN, 0 }, { WORLD_STATE_HORDE_COUNTDOWN, 0 }, { WORLD_STATE_HORDE_COUNTDOWN_MINUTES, 0 }, { WORLD_STATE_HORDE_ENEMY_CONTROLS_UNDERCITY, 1 }, { WORLD_STATE_HORDE_RUN_ACTIVE, 0 }, { WORLD_STATE_HORDE_COURTYARD_PROGRESS, 0 }, { WORLD_STATE_HORDE_INNER_SANCTUM_PROGRESS, 0 }, { WORLD_STATE_HORDE_ROYAL_QUARTER_PROGRESS, 0 }, { WORLD_STATE_HORDE_COURTYARD_FIGHT, 0 }, { WORLD_STATE_HORDE_COURTYARD_CONTROLLED, 0 }, { WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 0 }, { WORLD_STATE_HORDE_INNER_SANCTUM_CONTROLLED, 0 }, { WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT, 0 }, { WORLD_STATE_HORDE_ROYAL_QUARTER_CONTROLLED, 0 }, { WORLD_STATE_HORDE_RESET_MINUTES, 0 }, { WORLD_STATE_HORDE_RESET_COUNTDOWN, 0 } }), _runStartPending(false), _onwardPending(false), _creditPending(false) { }

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
            case NPC_VARIMATHRAS_PORTAL:
                if (TempSummon const* summon = creature->ToTempSummon())
                    if (summon->GetSummonerGUID() == _varimathrasGUID)
                        _varimathrasPortalGUID = creature->GetGUID();
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
            case NPC_WARSONG_BATTLEGUARD:
                if (!creature->IsSummon() && _battleguardGUID.IsEmpty())
                    _battleguardGUID = creature->GetGUID();
                break;
            default:
                break;
        }
    }

    void OnCreatureRemove(Creature* creature) override
    {
        EG_zone_undercity_battle::OnCreatureRemove(creature);

        ObjectGuid const guid = creature->GetGUID();
        if (guid == _thrallGUID)
        {
            _thrallGUID.Clear();
            if (RunState != RUN_STATE_IDLE)
                EndRun();
        }
        else if (guid == _sylvanasGUID)
            _sylvanasGUID.Clear();
        else if (guid == _varimathrasGUID)
            _varimathrasGUID.Clear();
        else if (guid == _varimathrasPortalGUID)
            _varimathrasPortalGUID.Clear();
        else if (guid == _varianGUID)
            _varianGUID.Clear();
        else if (guid == _jainaGUID)
            _jainaGUID.Clear();
        else if (guid == _saurfangGUID)
            _saurfangGUID.Clear();
        else if (guid == _battleguardGUID)
            _battleguardGUID.Clear();
    }

    void OnGameObjectCreate(GameObject* go) override
    {
        if (go->GetEntry() != GO_BLOCKED_PASSAGE)
            return;

        GameObjectData const* data = go->GetGameObjectData();
        if (!data || !data->spawnGroupData || data->spawnGroupData->groupId != SPAWN_GROUP_HORDE_BLOCKED_PASSAGES)
            return;

        auto slot = std::find(_blockedPassageGUIDs.begin(), _blockedPassageGUIDs.end(), ObjectGuid::Empty);
        if (slot != _blockedPassageGUIDs.end())
            *slot = go->GetGUID();
    }

    void OnGameObjectRemove(GameObject* go) override
    {
        auto slot = std::find(_blockedPassageGUIDs.begin(), _blockedPassageGUIDs.end(), go->GetGUID());
        if (slot != _blockedPassageGUIDs.end())
            slot->Clear();
    }

    void OnUnitDeath(Unit* unit) override
    {
        switch (unit->GetEntry())
        {
            case NPC_THRALL:
                if (RunState != RUN_STATE_IDLE)
                    EndRun();
                break;
            case NPC_BLIGHT_ABERRATION:
                if (RunState != RUN_STATE_COURTYARD_FIGHT)
                    break;

                SetData(DATA_RUN_STATE, RUN_STATE_COURTYARD_CONTROLLED);
                unit->GetMap()->SpawnGroupSpawn(SPAWN_GROUP_HORDE_COURTYARD_BANNERS, true);
                if (Creature* thrall = unit->GetMap()->GetCreature(_thrallGUID))
                    thrall->AI()->DoAction(ACTION_COURTYARD_CONTROLLED);
                break;
            case NPC_KHANOK:
                if (RunState != RUN_STATE_INNER_SANCTUM_FIGHT)
                    break;

                SetData(DATA_RUN_STATE, RUN_STATE_INNER_SANCTUM_CONTROLLED);
                if (Creature* thrall = unit->GetMap()->GetCreature(_thrallGUID))
                    thrall->AI()->DoAction(ACTION_INNER_SANCTUM_CONTROLLED);
                break;
            case NPC_VARIMATHRAS:
                if (RunState != RUN_STATE_ROYAL_QUARTER_FIGHT)
                    break;

                SetData(DATA_RUN_STATE, RUN_STATE_ROYAL_QUARTER_CONTROLLED);
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
            case DATA_VARIMATHRAS_PORTAL:
                return _varimathrasPortalGUID;
            case DATA_VARIAN_WRYNN_HORDE_RUN:
                return _varianGUID;
            case DATA_JAINA_PROUDMOORE_HORDE_RUN:
                return _jainaGUID;
            case DATA_SAURFANG:
                return _saurfangGUID;
            case DATA_WARSONG_BATTLEGUARD:
                return _battleguardGUID;
            case DATA_BLOCKED_PASSAGE_1:
            case DATA_BLOCKED_PASSAGE_2:
            case DATA_BLOCKED_PASSAGE_3:
                return _blockedPassageGUIDs[type - DATA_BLOCKED_PASSAGE_1];
            default:
                return ObjectGuid::Empty;
        }
    }

    void ProcessEvent(WorldObject* obj, uint32 eventId, WorldObject* /*invoker*/) override
    {
        if (eventId != EVENT_CALL_OF_AIR || !obj || obj->GetGUID() != _thrallGUID)
            return;

        Creature* thrall = obj->ToCreature();
        switch (RunState)
        {
            case RUN_STATE_BEGUN:
                for (auto const& [summonGroup, path] : { std::pair<uint8, uint32>(SUMMON_GROUP_GREAT_WIND_VORTEX_1, PATH_GREAT_WIND_VORTEX_1), std::pair<uint8, uint32>(SUMMON_GROUP_GREAT_WIND_VORTEX_2, PATH_GREAT_WIND_VORTEX_2) })
                {
                    std::list<TempSummon*> vortices;
                    thrall->SummonCreatureGroup(summonGroup, &vortices);
                    for (TempSummon* vortex : vortices)
                        vortex->GetMotionMaster()->MovePath(path, false);
                }
                break;
            case RUN_STATE_COURTYARD_CONTROLLED:
                thrall->SummonCreatureGroup(SUMMON_GROUP_CAVE_IN_DUMMIES);
                break;
            default:
                break;
        }
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type != DATA_RUN_STATE)
            return;

        switch (value)
        {
            case RUN_STATE_IDLE:
                if (RunState != RUN_STATE_IDLE)
                    EndRun();
                break;
            case RUN_STATE_COUNTDOWN:
                if (RunState != RUN_STATE_IDLE)
                    break;

                RunState = RUN_STATE_COUNTDOWN;
                _runStartPending = true;
                SetWorldState(WORLD_STATE_HORDE_RUN_ACTIVE, 1);
                SetWorldState(WORLD_STATE_HORDE_COUNTDOWN, 1);
                StartCountdown(WORLD_STATE_HORDE_COUNTDOWN_MINUTES, [this]()
                {
                    RunState = RUN_STATE_BEGUN;
                    _onwardPending = true;
                    SetWorldState(WORLD_STATE_HORDE_COUNTDOWN, 0);
                    SetWorldState(WORLD_STATE_HORDE_BATTLE_BEGUN, 1);
                });
                break;
            case RUN_STATE_COURTYARD_FIGHT:
                if (RunState != RUN_STATE_BEGUN)
                    break;

                RunState = RUN_STATE_COURTYARD_FIGHT;
                SetWorldState(WORLD_STATE_HORDE_BATTLE_BEGUN, 0);
                SetWorldState(WORLD_STATE_HORDE_ENEMY_CONTROLS_UNDERCITY, 0);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_FIGHT, 1);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_PROGRESS, 1);
                break;
            case RUN_STATE_COURTYARD_CONTROLLED:
                if (RunState != RUN_STATE_COURTYARD_FIGHT)
                    break;

                RunState = RUN_STATE_COURTYARD_CONTROLLED;
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_FIGHT, 0);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_CONTROLLED, 1);
                SetWorldState(WORLD_STATE_HORDE_COURTYARD_PROGRESS, 2);
                break;
            case RUN_STATE_INNER_SANCTUM_FIGHT:
                if (RunState != RUN_STATE_COURTYARD_CONTROLLED)
                    break;

                RunState = RUN_STATE_INNER_SANCTUM_FIGHT;
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 1);
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_PROGRESS, 1);
                break;
            case RUN_STATE_INNER_SANCTUM_CONTROLLED:
                if (RunState != RUN_STATE_INNER_SANCTUM_FIGHT)
                    break;

                RunState = RUN_STATE_INNER_SANCTUM_CONTROLLED;
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 0);
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_CONTROLLED, 1);
                SetWorldState(WORLD_STATE_HORDE_INNER_SANCTUM_PROGRESS, 2);
                break;
            case RUN_STATE_ROYAL_QUARTER_FIGHT:
                if (RunState != RUN_STATE_INNER_SANCTUM_CONTROLLED)
                    break;

                RunState = RUN_STATE_ROYAL_QUARTER_FIGHT;
                SetWorldState(WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT, 1);
                SetWorldState(WORLD_STATE_HORDE_ROYAL_QUARTER_PROGRESS, 1);
                break;
            case RUN_STATE_ROYAL_QUARTER_CONTROLLED:
                if (RunState != RUN_STATE_ROYAL_QUARTER_FIGHT)
                    break;

                RunState = RUN_STATE_ROYAL_QUARTER_CONTROLLED;
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
            map->SpawnGroupDespawn(SPAWN_GROUP_HORDE_STAGING);
            map->SpawnGroupSpawn(SPAWN_GROUP_HORDE_RUN_ACTORS, true);
        }

        if (_onwardPending)
        {
            _onwardPending = false;
            if (Creature* thrall = map->GetCreature(_thrallGUID))
                thrall->AI()->DoAction(ACTION_ONWARD);
        }

        if (!_creditPending)
            return;

        _creditPending = false;

        GuidUnorderedSet const players = GetPlayers();
        for (ObjectGuid const& guid : players)
            if (Player* player = ObjectAccessor::GetPlayer(map, guid))
                if (player->GetZoneId() == ZONE_UNDERCITY && player->GetQuestStatus(QUEST_BATTLE_FOR_THE_UNDERCITY_HORDE) == QUEST_STATUS_INCOMPLETE)
                    player->AreaExploredOrEventHappens(QUEST_BATTLE_FOR_THE_UNDERCITY_HORDE);
    }

private:
    void OnRunEnd(Map* map) override
    {
        _runStartPending = false;
        _onwardPending = false;
        _creditPending = false;
        map->SpawnGroupDespawn(SPAWN_GROUP_HORDE_RUN_ACTORS);
        map->SpawnGroupDespawn(SPAWN_GROUP_HORDE_COURTYARD_BANNERS);
        map->SpawnGroupDespawn(SPAWN_GROUP_HORDE_BLOCKED_PASSAGES);
        map->SpawnGroupSpawn(SPAWN_GROUP_HORDE_STAGING, true);
    }

    ObjectGuid _thrallGUID;
    ObjectGuid _sylvanasGUID;
    ObjectGuid _varimathrasGUID;
    ObjectGuid _varimathrasPortalGUID;
    ObjectGuid _varianGUID;
    ObjectGuid _jainaGUID;
    ObjectGuid _saurfangGUID;
    ObjectGuid _battleguardGUID;
    std::array<ObjectGuid, 3> _blockedPassageGUIDs;
    bool _runStartPending;
    bool _onwardPending;
    bool _creditPending;
};

class EG_zone_undercity_battle_alliance : public EG_zone_undercity_battle
{
public:
    EG_zone_undercity_battle_alliance() : EG_zone_undercity_battle("EG_zone_undercity_battle_alliance", PHASE_ALLIANCE, { { WORLD_STATE_ALLIANCE_COUNTDOWN_MINUTES, 0 }, { WORLD_STATE_ALLIANCE_COUNTDOWN, 0 }, { WORLD_STATE_ALLIANCE_SEWERS_PROGRESS, 0 }, { WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 0 }, { WORLD_STATE_ALLIANCE_ENEMY_CONTROLS_UNDERCITY, 1 }, { WORLD_STATE_ALLIANCE_SEWERS_SECURED, 0 }, { WORLD_STATE_ALLIANCE_RUN_ACTIVE, 0 }, { WORLD_STATE_ALLIANCE_MANHUNT_BEGUN, 0 }, { WORLD_STATE_ALLIANCE_APOTHECARIUM_PROGRESS, 0 }, { WORLD_STATE_ALLIANCE_APOTHECARIUM_SECURED, 0 }, { WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT, 0 } }), _manhuntPending(false) { }

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
                break;
            case NPC_KHANOK:
                _khanokGUID = creature->GetGUID();
                break;
            case NPC_THRALL:
                _thrallGUID = creature->GetGUID();
                break;
            default:
                break;
        }
    }

    void OnCreatureRemove(Creature* creature) override
    {
        EG_zone_undercity_battle::OnCreatureRemove(creature);

        ObjectGuid const guid = creature->GetGUID();
        if (guid == _varianGUID)
        {
            _varianGUID.Clear();
            if (RunState != RUN_STATE_IDLE)
                EndRun();
        }
        else if (guid == _jainaGUID)
            _jainaGUID.Clear();
        else if (guid == _putressGUID)
            _putressGUID.Clear();
        else if (guid == _khanokGUID)
            _khanokGUID.Clear();
        else if (guid == _thrallGUID)
            _thrallGUID.Clear();
    }

    void OnUnitDeath(Unit* unit) override
    {
        switch (unit->GetEntry())
        {
            case NPC_VARIAN_WRYNN:
                if (RunState != RUN_STATE_IDLE)
                    EndRun();
                break;
            case NPC_ENGORGED_BLIGHT_WORM:
                if (RunState != RUN_STATE_SEWERS_FIGHT)
                    break;

                SetData(DATA_RUN_STATE, RUN_STATE_SEWERS_SECURED);
                if (Creature* varian = unit->GetMap()->GetCreature(_varianGUID))
                    varian->AI()->DoAction(ACTION_SEWERS_SECURED);
                break;
            case NPC_GRAND_APOTHECARY_PUTRESS:
                if (RunState != RUN_STATE_APOTHECARIUM_FIGHT)
                    break;

                SetData(DATA_RUN_STATE, RUN_STATE_APOTHECARIUM_SECURED);
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
            case DATA_KHANOK:
                return _khanokGUID;
            case DATA_THRALL_ALLIANCE_RUN:
                return _thrallGUID;
            default:
                return ObjectGuid::Empty;
        }
    }

    void OnUpdate(Map* map, uint32 diff) override
    {
        EG_zone_undercity_battle::OnUpdate(map, diff);

        if (!_manhuntPending)
            return;

        _manhuntPending = false;
        if (Creature* varian = map->GetCreature(_varianGUID))
            varian->AI()->DoAction(ACTION_MANHUNT);
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type != DATA_RUN_STATE)
            return;

        switch (value)
        {
            case RUN_STATE_IDLE:
                if (RunState != RUN_STATE_IDLE)
                    EndRun();
                break;
            case RUN_STATE_COUNTDOWN:
                if (RunState != RUN_STATE_IDLE)
                    break;

                RunState = RUN_STATE_COUNTDOWN;
                SetWorldState(WORLD_STATE_ALLIANCE_RUN_ACTIVE, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_COUNTDOWN, 1);
                StartCountdown(WORLD_STATE_ALLIANCE_COUNTDOWN_MINUTES, [this]()
                {
                    RunState = RUN_STATE_BEGUN;
                    _manhuntPending = true;
                    SetWorldState(WORLD_STATE_ALLIANCE_COUNTDOWN, 0);
                    SetWorldState(WORLD_STATE_ALLIANCE_MANHUNT_BEGUN, 1);
                });
                break;
            case RUN_STATE_SEWERS_FIGHT:
                if (RunState != RUN_STATE_BEGUN)
                    break;

                RunState = RUN_STATE_SEWERS_FIGHT;
                SetWorldState(WORLD_STATE_ALLIANCE_MANHUNT_BEGUN, 0);
                SetWorldState(WORLD_STATE_ALLIANCE_ENEMY_CONTROLS_UNDERCITY, 0);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_PROGRESS, 1);
                break;
            case RUN_STATE_SEWERS_SECURED:
                if (RunState != RUN_STATE_SEWERS_FIGHT)
                    break;

                RunState = RUN_STATE_SEWERS_SECURED;
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 0);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_SECURED, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_SEWERS_PROGRESS, 2);
                break;
            case RUN_STATE_APOTHECARIUM_FIGHT:
                if (RunState != RUN_STATE_SEWERS_SECURED)
                    break;

                RunState = RUN_STATE_APOTHECARIUM_FIGHT;
                SetWorldState(WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT, 1);
                SetWorldState(WORLD_STATE_ALLIANCE_APOTHECARIUM_PROGRESS, 1);
                break;
            case RUN_STATE_APOTHECARIUM_SECURED:
                if (RunState != RUN_STATE_APOTHECARIUM_FIGHT)
                    break;

                RunState = RUN_STATE_APOTHECARIUM_SECURED;
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
        _manhuntPending = false;
        for (ObjectGuid const& guid : { _varianGUID, _jainaGUID })
            if (Creature* leader = map->GetCreature(guid))
                leader->DespawnOrUnsummon(0s, 5s);
    }

    ObjectGuid _varianGUID;
    ObjectGuid _jainaGUID;
    ObjectGuid _putressGUID;
    ObjectGuid _khanokGUID;
    ObjectGuid _thrallGUID;
    bool _manhuntPending;
};

// 32518 - Thrall
struct EG_npc_undercity_thrall : public EscortAI
{
    EG_npc_undercity_thrall(Creature* creature) : EscortAI(creature), _summons(me), _courtyardAttack(false)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void JustAppeared() override
    {
        EscortAI::JustAppeared();

        _scheduler.Schedule(177s, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_PREPARATION_1);
        })
        .Schedule(183s, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_PREPARATION_2);
        })
        .Schedule(197s, [this](TaskContext /*context*/)
        {
            if (Creature* battleguard = GetActor(DATA_WARSONG_BATTLEGUARD))
                battleguard->AI()->Talk(SAY_BATTLEGUARD_BURN);
        })
        .Schedule(216s, [this](TaskContext /*context*/)
        {
            if (Creature* battleguard = GetActor(DATA_WARSONG_BATTLEGUARD))
                battleguard->AI()->Talk(SAY_BATTLEGUARD_PUTRESS);
        })
        .Schedule(241s, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_PREPARATION_3);
        })
        .Schedule(255s, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_PREPARATION_4);
        })
        .Schedule(269s, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_PREPARATION_5);
        })
        .Schedule(279s, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_PREPARATION_6);
        })
        .Schedule(294s, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_BATTLE_BEGINS);
        })
        .Schedule(300s, [this](TaskContext context)
        {
            if (!me->HasAura(SPELL_WARCHIEFS_BLESSING))
                DoCastSelf(SPELL_WARCHIEFS_BLESSING);
            context.Repeat(10s);
        });
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ONWARD:
                Talk(SAY_THRALL_ONWARD);
                if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                    sylvanas->AI()->DoAction(ACTION_SYLVANAS_GATE);
                LoadPath(PATH_THRALL);
                SetDespawnAtEnd(false);
                SetDespawnAtFar(false);
                Start(true);
                break;
            case ACTION_COURTYARD_CONTROLLED:
                _summons.DespawnEntry(NPC_TREACHEROUS_GUARDIAN);
                _summons.DespawnEntry(NPC_BLIGHT_DOCTOR);
                _summons.DespawnEntry(NPC_APOTHECARY_CHEMIST);
                _summons.DespawnEntry(NPC_BLIGHT_SLINGER);
                me->SummonCreatureGroup(SUMMON_GROUP_COURTYARD_BATTLEGUARDS);
                if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                    sylvanas->AI()->DoAction(ACTION_SYLVANAS_FOLLOW);
                SetEscortPaused(false);
                break;
            case ACTION_INNER_SANCTUM_CONTROLLED:
                _scheduler.CancelGroup(GROUP_INNER_SANCTUM_WAVES);
                for (uint32 entry : { NPC_FELGUARD_MARAUDER, NPC_PERFIDIOUS_DREADLORD, NPC_TREACHEROUS_GUARDIAN, NPC_BLIGHT_DOCTOR, NPC_APOTHECARY_CHEMIST, NPC_RAVISHING_BETRAYER, NPC_PLAGUED_FELBEAST, NPC_DOOMGUARD_PILLAGER })
                    _summons.DespawnEntry(entry);
                if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                    sylvanas->AI()->DoAction(ACTION_SYLVANAS_FOLLOW);
                SetEscortPaused(false);
                break;
            case ACTION_ROYAL_QUARTER_CONTROLLED:
                _scheduler.CancelGroup(GROUP_ROYAL_QUARTER_WAVES);
                for (uint32 entry : { NPC_VARIMATHRAS_PORTAL, NPC_LEGION_INVADER, NPC_LEGION_DREADWHISPERER, NPC_LEGION_OVERLORD })
                    _summons.DespawnEntry(entry);
                SetEscortPaused(false);
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
                SetEscortPaused(true);
                me->Dismount();
                _scheduler.Schedule(1s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->Dismount();
                })
                .Schedule(4s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_CALL_OF_AIR);
                })
                .Schedule(8s, [this](TaskContext /*context*/)
                {
                    DoCastSelf(SPELL_CALL_OF_AIR);
                })
                .Schedule(14s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_COURTYARD);
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_COURTYARD_ENTRANCE:
                SetEscortPaused(true);
                me->SummonCreatureGroup(SUMMON_GROUP_COURTYARD_DEFENDERS);
                Talk(SAY_THRALL_ANSWER_TO_THE_ELEMENTS);
                _scheduler.Schedule(6s, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIMATHRAS_COURTYARD);
                })
                .Schedule(9s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->Talk(SAY_VARIMATHRAS_WELCOME);
                })
                .Schedule(14s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->Talk(SAY_VARIMATHRAS_MINION);
                })
                .Schedule(23s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->Talk(SAY_VARIMATHRAS_ENOUGH);
                })
                .Schedule(30s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->CastSpell(varimathras, SPELL_LEGION_PORTAL);
                })
                .Schedule(43s, [this](TaskContext /*context*/)
                {
                    Creature* varimathras = GetActor(DATA_VARIMATHRAS);
                    if (!varimathras)
                        return;

                    if (Creature* portal = GetActor(DATA_VARIMATHRAS_PORTAL))
                    {
                        varimathras->GetMotionMaster()->MovePoint(0, portal->GetPosition());
                        portal->DespawnOrUnsummon(6s);
                    }
                    varimathras->DespawnOrUnsummon(3s);
                })
                .Schedule(44s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_TIDAL_WAVE);
                    DoCastSelf(SPELL_TIDAL_WAVE);
                })
                .Schedule(49s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_ATTACK);
                    _courtyardAttack = true;
                    for (ObjectGuid const& guid : _summons)
                    {
                        if (Creature* defender = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (defender->GetEntry() == NPC_TREACHEROUS_GUARDIAN || defender->GetEntry() == NPC_BLIGHT_DOCTOR || defender->GetEntry() == NPC_APOTHECARY_CHEMIST)
                            {
                                defender->SetImmuneToAll(false);
                                defender->AI()->AttackStart(me);
                            }
                        }
                    }

                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_FOLLOW);
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_COURTYARD_FIGHT);
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_COURTYARD:
                SetEscortPaused(true);
                DoCastSelf(SPELL_HEROIC_VANGUARD, true);
                _scheduler.Schedule(0s, [this](TaskContext context)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_COURTYARD_WAVE);
                    if (context.GetRepeatCounter() < 16)
                        context.Repeat(10s);
                    else
                        context.Schedule(2s, [this](TaskContext /*context*/)
                        {
                            me->SummonCreatureGroup(SUMMON_GROUP_BLIGHT_ABERRATION);
                        });
                });
                break;
            case POINT_THRALL_COURTYARD_SECURED:
                SetEscortPaused(true);
                Talk(SAY_THRALL_COURTYARD_SECURED);
                _scheduler.Schedule(5s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_ELEVATOR:
                SetEscortPaused(true);
                Talk(SAY_THRALL_ELEVATOR_DESTROYED);
                _scheduler.Schedule(10s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->Talk(SAY_SYLVANAS_SHAFT_TRAPPED);
                })
                .Schedule(13s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_AIR_SPIRIT);
                    DoCastSelf(SPELL_CALL_OF_AIR);
                })
                .Schedule(29s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_CYCLONES);
                })
                .Schedule(33s, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_SHAFT_BATTLEGUARDS);

                    WaypointPath const* path = sWaypointMgr->GetPath(PATH_THRALL);
                    if (!path)
                        return;

                    auto bottom = std::find_if(path->nodes.begin(), path->nodes.end(), [](WaypointNode const& node)
                    {
                        return node.id == POINT_THRALL_SHAFT_BOTTOM;
                    });

                    Position const landing(bottom->x, bottom->y, bottom->z);
                    me->GetMotionMaster()->MoveJump(landing, 40.0f, 40.0f);
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->GetMotionMaster()->MoveJump(landing, 40.0f, 40.0f);
                })
                .Schedule(37s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_FOLLOW);
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_INNER_SANCTUM:
                Talk(SAY_THRALL_INNER_SANCTUM_BREACHED);
                if (ZoneScript* zoneScript = me->GetZoneScript())
                    zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_INNER_SANCTUM_FIGHT);
                break;
            case POINT_THRALL_LOWER_CITY:
                SetEscortPaused(true);
                if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                    sylvanas->AI()->Talk(SAY_SYLVANAS_BEAUTIFUL_CITY);
                _scheduler.Schedule(5s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->Talk(SAY_SYLVANAS_REDEMPTION);
                })
                .Schedule(13s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_LEAD_THE_WAY);
                })
                .Schedule(18s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->Talk(SAY_SYLVANAS_ROYAL_QUARTER_THIS_WAY);
                })
                .Schedule(23s, [this](TaskContext context)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_LOWER_CITY_WAVE_1);
                    if (context.GetRepeatCounter() < 2)
                        context.Repeat(5s);
                })
                .Schedule(43s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_LOWER_CITY_AMBUSH_1:
                SetEscortPaused(true);
                _scheduler.Schedule(3s, [this](TaskContext context)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_LOWER_CITY_WAVE_2);
                    if (context.GetRepeatCounter() < 1)
                        context.Repeat(5s);
                })
                .Schedule(18s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_LOWER_CITY_AMBUSH_2:
                SetEscortPaused(true);
                _scheduler.Schedule(0s, [this](TaskContext context)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_LOWER_CITY_WAVE_3);
                    if (context.GetRepeatCounter() < 1)
                        context.Repeat(6s);
                })
                .Schedule(16s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_KHANOK:
                SetEscortPaused(true);
                _scheduler.Schedule(10s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->Talk(SAY_SYLVANAS_DARK_MAGIC);
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIMATHRAS_INNER_SANCTUM);
                })
                .Schedule(15s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                        varimathras->AI()->Talk(SAY_VARIMATHRAS_CLEVER_GIRL);
                })
                .Schedule(20s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                    {
                        varimathras->AI()->Talk(SAY_VARIMATHRAS_BROTHERS_HUNGRY);
                        varimathras->CastSpell(varimathras, SPELL_LEGION_PORTAL);
                    }
                })
                .Schedule(26s, [this](TaskContext /*context*/)
                {
                    Creature* varimathras = GetActor(DATA_VARIMATHRAS);
                    if (!varimathras)
                        return;

                    if (Creature* portal = GetActor(DATA_VARIMATHRAS_PORTAL))
                    {
                        varimathras->GetMotionMaster()->MovePoint(0, portal->GetPosition());
                        portal->DespawnOrUnsummon(3s);
                    }
                    varimathras->DespawnOrUnsummon(3s);
                })
                .Schedule(28s, GROUP_INNER_SANCTUM_WAVES, [this](TaskContext context)
                {
                    switch (context.GetRepeatCounter() % 3)
                    {
                        case 0:
                            me->SummonCreatureGroup(SUMMON_GROUP_INNER_SANCTUM_WAVE_1);
                            break;
                        case 1:
                            me->SummonCreatureGroup(SUMMON_GROUP_INNER_SANCTUM_WAVE_2);
                            break;
                        default:
                            me->SummonCreatureGroup(SUMMON_GROUP_DOOMGUARD_PILLAGER);
                            break;
                    }

                    if (context.GetRepeatCounter() < 17)
                        context.Repeat(8s);
                })
                .Schedule(172s, GROUP_INNER_SANCTUM_WAVES, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_KHANOK);
                })
                .Schedule(182s, GROUP_INNER_SANCTUM_WAVES, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_INNER_SANCTUM_WAVE_1);
                })
                .Schedule(187s, GROUP_INNER_SANCTUM_WAVES, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_INNER_SANCTUM_WAVE_2);
                })
                .Schedule(192s, GROUP_INNER_SANCTUM_WAVES, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_DOOMGUARD_PILLAGER);
                });
                break;
            case POINT_THRALL_KHANOK_DEFEATED:
                SetEscortPaused(true);
                Talk(SAY_THRALL_IS_THAT_ALL);
                _scheduler.Schedule(7s, [this](TaskContext context)
                {
                    std::list<TempSummon*> battleguards;
                    me->SummonCreatureGroup(SUMMON_GROUP_BATTLEGUARD_CHARGE, &battleguards);
                    for (TempSummon* battleguard : battleguards)
                    {
                        battleguard->AI()->Talk(SAY_BATTLEGUARD_FOR_THE_HORDE);
                        battleguard->GetMotionMaster()->MovePath(PATH_BATTLEGUARD_CHARGE, false);
                    }

                    if (context.GetRepeatCounter() < 8)
                        context.Repeat(350ms);
                })
                .Schedule(13s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_WE_COME_FOR_YOU);
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_ROYAL_QUARTER_APPROACH:
                SetEscortPaused(true);
                if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                    sylvanas->AI()->Talk(SAY_SYLVANAS_ROYAL_QUARTER_AHEAD);
                _scheduler.Schedule(8s, [this](TaskContext /*context*/)
                {
                    me->GetMap()->SpawnGroupSpawn(SPAWN_GROUP_HORDE_BLOCKED_PASSAGES, true);
                    me->SummonCreatureGroup(SUMMON_GROUP_LEGION_PORTALS);

                    std::list<TempSummon*> varimathras;
                    me->SummonCreatureGroup(SUMMON_GROUP_VARIMATHRAS_ROYAL_QUARTER, &varimathras);
                    for (TempSummon* demon : varimathras)
                    {
                        demon->CastSpell(me, SPELL_AURA_OF_VARIMATHRAS);
                        demon->CastSpell(me, SPELL_OPENING_LEGION_PORTALS);
                        demon->AI()->Talk(SAY_VARIMATHRAS_BRING_DOWN_THE_HALLS);
                    }
                })
                .Schedule(13s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_COLLAPSED_HALL:
                SetEscortPaused(true);
                _scheduler.Schedule(3s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_COWARD);
                })
                .Schedule(15s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_EARTH_SPIRITS);
                    DoCastSelf(SPELL_CALL_OF_EARTH);
                })
                .Schedule(26s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_LETS_GO_QUICKLY);
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_ROYAL_QUARTER_FIGHT);
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_FOLLOW);
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_THRONE_ROOM:
                SetEscortPaused(true);
                Talk(SAY_THRALL_WHAT_IS_THIS);
                _scheduler.Schedule(5s, [this](TaskContext /*context*/)
                {
                    if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                    {
                        varimathras->AI()->Talk(SAY_VARIMATHRAS_WELCOME_TO_YOUR_FUTURE);
                        varimathras->CastSpell(me, SPELL_OPENING_LEGION_PORTALS);
                    }
                })
                .Schedule(8s, GROUP_ROYAL_QUARTER_WAVES, [this](TaskContext context)
                {
                    uint32 const step = context.GetRepeatCounter();
                    switch (step % 3)
                    {
                        case 0:
                            me->SummonCreatureGroup(SUMMON_GROUP_ROYAL_QUARTER_WAVE_1);
                            break;
                        case 1:
                            me->SummonCreatureGroup(SUMMON_GROUP_ROYAL_QUARTER_WAVE_2);
                            break;
                        default:
                        {
                            Creature* portal = nullptr;
                            for (ObjectGuid const& guid : _summons)
                            {
                                Creature* summon = ObjectAccessor::GetCreature(*me, guid);
                                if (summon && summon->GetEntry() == NPC_VARIMATHRAS_PORTAL)
                                {
                                    portal = summon;
                                    break;
                                }
                            }

                            if (portal)
                            {
                                me->SummonCreature(NPC_LEGION_OVERLORD, portal->GetPosition(), TEMPSUMMON_DEAD_DESPAWN);
                                portal->DespawnOrUnsummon();
                            }

                            if (step / 3 < 5)
                                if (Creature* varimathras = GetActor(DATA_VARIMATHRAS))
                                    varimathras->AI()->Talk(SAY_VARIMATHRAS_TOO_LONG + step / 3);
                            break;
                        }
                    }

                    if (step < 17)
                        context.Repeat(10s);
                    else
                        context.Schedule(5s, [this](TaskContext /*context*/)
                        {
                            Creature* varimathras = GetActor(DATA_VARIMATHRAS);
                            if (!varimathras)
                                return;

                            varimathras->RemoveAurasDueToSpell(SPELL_AURA_OF_VARIMATHRAS);
                            varimathras->RemoveAurasDueToSpell(SPELL_OPENING_LEGION_PORTALS);
                            varimathras->SetImmuneToAll(false);
                            varimathras->SetReactState(REACT_AGGRESSIVE);
                            varimathras->SetHomePosition(me->GetPosition());
                            varimathras->AI()->Talk(SAY_VARIMATHRAS_THOUSAND_PARDONS);
                            varimathras->AI()->AttackStart(me);
                            AttackStart(varimathras);
                        });
                });
                break;
            case POINT_THRALL_VICTORY:
                SetEscortPaused(true);
                Talk(SAY_THRALL_VICTORIOUS);
                _scheduler.Schedule(5s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_LOKTAR);
                })
                .Schedule(7s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_SYLVANAS_PLATFORM:
                SetEscortPaused(true);
                _scheduler.Schedule(8s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        me->SetFacingToObject(sylvanas);
                    Talk(SAY_THRALL_JOIN_ME);
                })
                .Schedule(11s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->CastSpell(sylvanas, SPELL_LEAP_TO_PLATFORM);
                })
                .Schedule(13s, [this](TaskContext /*context*/)
                {
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                    {
                        sylvanas->AI()->Talk(SAY_SYLVANAS_FATE_OF_ALL_ENEMIES);
                        sylvanas->SetFacingToObject(me);
                        me->SetFacingToObject(sylvanas);
                    }
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                })
                .Schedule(16s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_WE_SHALL);
                    if (Creature* sylvanas = GetActor(DATA_SYLVANAS))
                        sylvanas->AI()->DoAction(ACTION_SYLVANAS_FOLLOW);
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_ALLIANCE_HORNS:
                SetEscortPaused(true);
                Talk(SAY_THRALL_ALLIANCE_HORNS);
                _scheduler.Schedule(3s, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_ALLIANCE_INTRUSION);
                })
                .Schedule(9s, [this](TaskContext /*context*/)
                {
                    if (Creature* varian = GetActor(DATA_VARIAN_WRYNN_HORDE_RUN))
                        varian->AI()->Talk(SAY_VARIAN_HORDE_RUN_AWAY_TOO_LONG);
                })
                .Schedule(24s, [this](TaskContext /*context*/)
                {
                    if (Creature* varian = GetActor(DATA_VARIAN_WRYNN_HORDE_RUN))
                        varian->AI()->Talk(SAY_VARIAN_HORDE_RUN_MAKE_THINGS_RIGHT);
                })
                .Schedule(39s, [this](TaskContext /*context*/)
                {
                    if (Creature* varian = GetActor(DATA_VARIAN_WRYNN_HORDE_RUN))
                        varian->AI()->Talk(SAY_VARIAN_HORDE_RUN_WAITED_LONG);
                })
                .Schedule(55500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* varian = GetActor(DATA_VARIAN_WRYNN_HORDE_RUN))
                        varian->AI()->Talk(SAY_VARIAN_HORDE_RUN_IT_ENDS_NOW);
                })
                .Schedule(61500ms, [this](TaskContext /*context*/)
                {
                    for (ObjectGuid const& guid : _summons)
                    {
                        Creature* soldier = ObjectAccessor::GetCreature(*me, guid);
                        if (!soldier || (soldier->GetEntry() != NPC_VARIAN_WRYNN_HORDE_RUN && soldier->GetEntry() != NPC_STORMWIND_ELITE))
                            continue;

                        soldier->SetImmuneToNPC(false);
                        soldier->SetReactState(REACT_AGGRESSIVE);
                        soldier->AI()->AttackStart(me);
                    }
                })
                .Schedule(67500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE_HORDE_RUN))
                    {
                        jaina->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE_HORDE_RUN, false);
                        jaina->AI()->Talk(SAY_JAINA_HORDE_RUN_VARIAN_NO);
                    }
                })
                .Schedule(75500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE_HORDE_RUN))
                    {
                        jaina->CastSpell(jaina, SPELL_DEEP_FREEZE_STUN);
                        jaina->AI()->Talk(SAY_JAINA_HORDE_RUN_DID_NOT_HAVE_TO_BE);
                    }
                })
                .Schedule(80500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE_HORDE_RUN))
                        jaina->CastSpell(jaina, SPELL_GREATER_MASS_TELEPORT_DUMMY);
                })
                .Schedule(85500ms, [this](TaskContext /*context*/)
                {
                    _summons.DespawnEntry(NPC_JAINA_PROUDMOORE_HORDE_RUN);
                })
                .Schedule(88500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_IT_ENDS);
                    SetEscortPaused(false);
                });
                break;
            case POINT_THRALL_LAMENT:
                Talk(SAY_THRALL_ALL_IS_LOST);
                break;
            case POINT_THRALL_SAURFANG:
                SetEscortPaused(true);
                Talk(SAY_THRALL_IF_ONLY_YOU_WERE_HERE);
                me->SetStandState(UNIT_STAND_STATE_SIT);
                _scheduler.Schedule(3s, [this](TaskContext /*context*/)
                {
                    me->SummonCreatureGroup(SUMMON_GROUP_SAURFANG);
                })
                .Schedule(10s, [this](TaskContext /*context*/)
                {
                    if (Creature* saurfang = GetActor(DATA_SAURFANG))
                    {
                        saurfang->AI()->Talk(SAY_SAURFANG_WHAT_HE_WOULD_DO);
                        saurfang->SetStandState(UNIT_STAND_STATE_SIT);
                    }
                })
                .Schedule(16s, [this](TaskContext /*context*/)
                {
                    if (Creature* saurfang = GetActor(DATA_SAURFANG))
                        saurfang->AI()->Talk(SAY_SAURFANG_LEAD_YOUR_PEOPLE);
                })
                .Schedule(22s, [this](TaskContext /*context*/)
                {
                    if (Creature* saurfang = GetActor(DATA_SAURFANG))
                        saurfang->AI()->Talk(SAY_SAURFANG_LETS_GO_HOME);
                })
                .Schedule(27s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_THRALL_NODS);
                })
                .Schedule(32s, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                    Talk(SAY_THRALL_GOOD_TO_HAVE_YOU_BACK);
                });
                break;
            default:
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);

        switch (summon->GetEntry())
        {
            case NPC_TREACHEROUS_GUARDIAN:
            case NPC_BLIGHT_DOCTOR:
            case NPC_APOTHECARY_CHEMIST:
                if (_courtyardAttack)
                    summon->AI()->AttackStart(me);
                else
                    summon->SetImmuneToAll(true);
                break;
            case NPC_BLIGHT_SLINGER:
                summon->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
                summon->SetImmuneToNPC(true);
                break;
            case NPC_VARIMATHRAS:
                summon->SetImmuneToAll(true);
                summon->SetReactState(REACT_PASSIVE);
                break;
            case NPC_BLIGHT_ABERRATION:
                AttackStart(summon);
                break;
            case NPC_KHANOK:
                summon->SetImmuneToAll(false);
                summon->AI()->Talk(SAY_KHANOK_NO_HOPE);
                AttackStart(summon);
                break;
            case NPC_VARIAN_WRYNN_HORDE_RUN:
                summon->SetImmuneToAll(true);
                summon->SetReactState(REACT_PASSIVE);
                summon->SetEmoteState(EMOTE_STATE_READY2H);
                summon->GetMotionMaster()->MovePath(PATH_VARIAN_WRYNN_HORDE_RUN, false);
                break;
            case NPC_STORMWIND_ELITE:
                summon->SetImmuneToAll(true);
                summon->SetReactState(REACT_PASSIVE);
                summon->SetEmoteState(EMOTE_STATE_READY2H);
                break;
            case NPC_JAINA_PROUDMOORE_HORDE_RUN:
                summon->SetImmuneToAll(true);
                summon->SetReactState(REACT_PASSIVE);
                break;
            case NPC_SAURFANG:
                summon->GetMotionMaster()->MovePath(PATH_SAURFANG, false);
                break;
            default:
                break;
        }
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        _summons.Despawn(summon);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(5s, 8s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_CHAIN_LIGHTNING);
            context.Repeat(5s, 8s);
        })
        .Schedule(3s, 5s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_LAVA_BURST);
            context.Repeat(3s, 5s);
        })
        .Schedule(15s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastSelf(SPELL_THUNDER);
            context.Repeat(15s);
        });
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelGroup(GROUP_COMBAT);
        EscortAI::EnterEvadeMode(why);
    }

    void UpdateEscortAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    Creature* GetActor(uint32 type) const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(type)) : nullptr;
    }

    SummonList _summons;
    TaskScheduler _scheduler;
    bool _courtyardAttack;
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

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(5s, 8s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_CHAIN_LIGHTNING);
            context.Repeat(5s, 8s);
        })
        .Schedule(3s, 5s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_LAVA_BURST);
            context.Repeat(3s, 5s);
        })
        .Schedule(15s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_THUNDER);
            context.Repeat(15s);
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
struct EG_npc_undercity_sylvanas : public ScriptedAI
{
    EG_npc_undercity_sylvanas(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_SYLVANAS_GATE:
                me->GetMotionMaster()->MovePath(PATH_SYLVANAS_GATE, false);
                break;
            case ACTION_SYLVANAS_COURTYARD:
                me->GetMotionMaster()->MovePath(PATH_SYLVANAS_COURTYARD, false);
                break;
            case ACTION_SYLVANAS_FOLLOW:
                me->SetImmuneToNPC(false);
                if (Unit* thrall = GetThrall())
                    me->GetMotionMaster()->MoveFollow(thrall, 1.0f, ChaseAngle(float(M_PI_2)));
                break;
            default:
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCastSelf(SPELL_SONG_OF_SYLVANAS);
        _scheduler.Schedule(6s, 9s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BLACK_ARROW);
            context.Repeat(6s, 9s);
        })
        .Schedule(5s, 10s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHOOT);
            context.Repeat(5s, 10s);
        })
        .Schedule(10s, 13s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_MULTI_SHOT);
            context.Repeat(10s, 13s);
        })
        .Schedule(3s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHRIEK_OF_THE_HIGHBORNE);
            context.Repeat(3s);
        })
        .Schedule(10s, [this](TaskContext context)
        {
            if (!me->HasAura(SPELL_SONG_OF_SYLVANAS))
                DoCastSelf(SPELL_SONG_OF_SYLVANAS);
            context.Repeat(10s);
        });
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelAll();

        Unit* thrall = GetThrall();
        if (!thrall)
        {
            ScriptedAI::EnterEvadeMode(why);
            return;
        }

        if (!_EnterEvadeMode(why))
            return;

        me->GetMotionMaster()->MoveFollow(thrall, 1.0f, ChaseAngle(float(M_PI_2)));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
        {
            if (Unit* thrall = GetThrall())
                if (Unit* victim = thrall->GetVictim())
                    AttackStart(victim);
            return;
        }

        _scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

private:
    Unit* GetThrall() const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(DATA_THRALL)) : nullptr;
    }

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

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCastSelf(SPELL_SONG_OF_SYLVANAS);
        _scheduler.Schedule(6s, 9s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BLACK_ARROW);
            context.Repeat(6s, 9s);
        })
        .Schedule(5s, 10s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHOOT);
            context.Repeat(5s, 10s);
        })
        .Schedule(10s, 13s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_MULTI_SHOT);
            context.Repeat(10s, 13s);
        })
        .Schedule(3s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHRIEK_OF_THE_HIGHBORNE);
            context.Repeat(3s);
        })
        .Schedule(10s, [this](TaskContext context)
        {
            if (!me->HasAura(SPELL_SONG_OF_SYLVANAS))
                DoCastSelf(SPELL_SONG_OF_SYLVANAS);
            context.Repeat(10s);
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

// 31650 - Thrall
struct EG_npc_undercity_thrall_staging : public ScriptedAI
{
    EG_npc_undercity_thrall_staging(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        bool const idle = zoneScript && zoneScript->GetData(DATA_RUN_STATE) == RUN_STATE_IDLE;

        player->PrepareGossipMenu(me, GOSSIP_MENU_THRALL, true);
        if (!idle)
            player->PlayerTalkClass->GetGossipMenu().ClearMenu();

        SendGossipMenuFor(player, idle ? NPC_TEXT_THRALL_READY : NPC_TEXT_THRALL_BUSY, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        if (!zoneScript || menuId != GOSSIP_MENU_THRALL || gossipListId != GOSSIP_OPTION_START || !player->PlayerTalkClass->GetGossipMenu().GetItem(gossipListId))
            return false;

        CloseGossipMenuFor(player);
        zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_COUNTDOWN);
        return true;
    }
};

// 32401 - King Varian Wrynn
struct EG_npc_undercity_varian : public EscortAI
{
    EG_npc_undercity_varian(Creature* creature) : EscortAI(creature), _summons(me)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_MANHUNT:
                Talk(SAY_VARIAN_MARCH_BEGINS);
                _scheduler.Schedule(10s, [this](TaskContext context)
                {
                    if (!me->HasAura(SPELL_STRENGTH_OF_WRYNN))
                        DoCastSelf(SPELL_STRENGTH_OF_WRYNN);
                    context.Repeat(10s);
                })
                .Schedule(13s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_DESTINY_AWAITS);
                })
                .Schedule(14s, [this](TaskContext /*context*/)
                {
                    LoadPath(PATH_VARIAN_WRYNN);
                    SetDespawnAtEnd(false);
                    SetDespawnAtFar(false);
                    Start(true);
                })
                .Schedule(15500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                    {
                        jaina->AI()->DoAction(ACTION_JAINA_HOLD);
                        jaina->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE, false);
                    }
                });
                break;
            case ACTION_SEWERS_SECURED:
                Talk(SAY_VARIAN_FALL_IN);
                for (uint8 i = 0; i < 4; ++i)
                {
                    std::list<TempSummon*> elites;
                    me->SummonCreatureGroup(SUMMON_GROUP_STORMWIND_ELITE_1 + i, &elites);
                    for (TempSummon* elite : elites)
                        elite->GetMotionMaster()->MovePath(PATH_STORMWIND_ELITE_1 + i, false);
                }
                _scheduler.Schedule(10s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_ABERRATIONS);
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->DoAction(ACTION_JAINA_FOLLOW);
                })
                .Schedule(15s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case ACTION_APOTHECARIUM_SECURED:
                _summons.DespawnEntry(NPC_CRAZED_APOTHECARY_GENERATOR);
                _summons.DespawnEntry(NPC_FAILED_EXPERIMENT);
                Talk(SAY_VARIAN_JUSTICE_SERVED);
                _scheduler.Schedule(4s, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        me->SetFacingToObject(putress);
                    Talk(SAY_VARIAN_WHAT_SAY_YOU_NOW);
                })
                .Schedule(8s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_LOOK_AROUND);
                    SetEscortPaused(false);
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
                SetEscortPaused(true);
                Talk(SAY_VARIAN_DESCENT_BEGINS);
                for (uint8 summonGroup : { SUMMON_GROUP_SEWERS_GUARDIAN, SUMMON_GROUP_SEWERS_WAVE_1, SUMMON_GROUP_SEWERS_WAVE_2, SUMMON_GROUP_SEWERS_WAVE_3 })
                    me->SummonCreatureGroup(summonGroup);
                _scheduler.Schedule(9500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->Talk(SAY_JAINA_RIGHT_AWAY);
                })
                .Schedule(11500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->CastSpell(jaina, SPELL_IMPROVED_BRILLIANCE_AURA);
                })
                .Schedule(12500ms, [this](TaskContext /*context*/)
                {
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_SEWERS_FIGHT);
                    SetEscortPaused(false);
                })
                .Schedule(13500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->DoAction(ACTION_JAINA_FOLLOW);
                });
                break;
            case POINT_VARIAN_SEWERS:
                SetEscortPaused(true);
                if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                    jaina->AI()->DoAction(ACTION_JAINA_HOLD);
                Talk(SAY_VARIAN_STAND_YOUR_GROUND);
                _scheduler.Schedule(3500ms, GROUP_SEWERS_WAVES, [this](TaskContext context)
                {
                    me->SummonCreatureGroup(context.GetRepeatCounter() == 1 || context.GetRepeatCounter() == 4 ? SUMMON_GROUP_SEWERS_WAVE_5 : SUMMON_GROUP_SEWERS_WAVE_4);
                    if (context.GetRepeatCounter() < 4)
                        context.Repeat(15s);
                })
                .Schedule(78500ms, GROUP_SEWERS_WAVES, [this](TaskContext /*context*/)
                {
                    std::list<TempSummon*> bosses;
                    me->SummonCreatureGroup(SUMMON_GROUP_SEWERS_BOSSES, &bosses);

                    auto putress = std::find_if(bosses.begin(), bosses.end(), [](TempSummon const* summon)
                    {
                        return summon->GetEntry() == NPC_GRAND_APOTHECARY_PUTRESS;
                    });

                    for (TempSummon* boss : bosses)
                    {
                        switch (boss->GetEntry())
                        {
                            case NPC_ENGORGED_BLIGHT_WORM:
                                boss->AI()->AttackStart(me);
                                AttackStart(boss);
                                break;
                            case NPC_GRAND_APOTHECARY_PUTRESS:
                                boss->SetImmuneToAll(true);
                                boss->SetReactState(REACT_PASSIVE);
                                boss->CastSpell(boss, SPELL_PUTRESS_CASTING_STATE);
                                break;
                            case NPC_KHANOK:
                                boss->SetCorpseDelay(HOUR);
                                boss->setDeathState(JUST_DIED);
                                break;
                            case NPC_CRAZED_APOTHECARY_GENERATOR:
                                boss->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                                boss->SetImmuneToAll(true);
                                if (putress != bosses.end())
                                    boss->CastSpell(*putress, SPELL_BLIGHT_EMPOWERMENT);
                                break;
                            default:
                                break;
                        }
                    }
                });
                break;
            case POINT_VARIAN_CANALS:
                SetEscortPaused(true);
                if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                {
                    jaina->AI()->DoAction(ACTION_JAINA_HOLD);
                    jaina->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE_CANALS, false);
                    jaina->AI()->Talk(SAY_JAINA_CORPSES_EVERYWHERE);
                }
                _scheduler.Schedule(10s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_VARIAN_KHANOK:
                SetEscortPaused(true);
                if (Creature* khanok = GetActor(DATA_KHANOK))
                    me->SetFacingToObject(khanok);
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                _scheduler.Schedule(1s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HORDE_IN_FORCE);
                })
                .Schedule(13s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_STAY_ALERT);
                })
                .Schedule(23s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_ONWARD);
                })
                .Schedule(24500ms, [this](TaskContext /*context*/)
                {
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_APOTHECARIUM_FIGHT);
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        putress->AI()->Talk(SAY_PUTRESS_DO_NOT_LET_THEM_PASS);
                    me->SummonCreatureGroup(SUMMON_GROUP_APOTHECARIUM_GUARDIANS);
                })
                .Schedule(27500ms, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->DoAction(ACTION_JAINA_FOLLOW);
                    SetEscortPaused(false);
                });
                break;
            case POINT_VARIAN_PUTRESS_WARNING:
                if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                    putress->AI()->Talk(SAY_PUTRESS_KILL_THEM);
                break;
            case POINT_VARIAN_MAIN_CHAMBER:
                SetEscortPaused(true);
                Talk(SAY_VARIAN_MAIN_CHAMBER);
                _scheduler.Schedule(0s, [this](TaskContext context)
                {
                    std::list<TempSummon*> dreadlords;
                    me->SummonCreatureGroup(SUMMON_GROUP_PERFIDIOUS_DREADLORD, &dreadlords);
                    for (TempSummon* dreadlord : dreadlords)
                        dreadlord->GetMotionMaster()->MovePath(PATH_PERFIDIOUS_DREADLORD, false);

                    if (context.GetRepeatCounter() < 10)
                        context.Repeat(350ms);
                    else
                        context.Schedule(350ms, [this](TaskContext /*context*/)
                        {
                            SetEscortPaused(false);
                        });
                });
                break;
            case POINT_VARIAN_THERE:
                SetEscortPaused(true);
                Talk(SAY_VARIAN_THERE);
                if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                {
                    jaina->AI()->DoAction(ACTION_JAINA_HOLD);
                    jaina->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE_APOTHECARIUM, false);
                }
                _scheduler.Schedule(5s, [this](TaskContext /*context*/)
                {
                    SetEscortPaused(false);
                });
                break;
            case POINT_VARIAN_PUTRESS:
                SetEscortPaused(true);
                Talk(SAY_VARIAN_NOWHERE_TO_RUN);
                _scheduler.Schedule(4s, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        putress->AI()->Talk(SAY_PUTRESS_STARE_INTO_THE_ABYSS);
                    me->SummonCreatureGroup(SUMMON_GROUP_FAILED_EXPERIMENTS);
                })
                .Schedule(11500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        putress->AI()->Talk(SAY_PUTRESS_NO_MATCH);
                })
                .Schedule(19s, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        putress->AI()->Talk(SAY_PUTRESS_CANNOT_BE_HAPPENING);
                    me->SummonCreatureGroup(SUMMON_GROUP_FAILED_EXPERIMENTS);
                })
                .Schedule(26500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        putress->AI()->Talk(SAY_PUTRESS_WILL_NOT_END);
                })
                .Schedule(36500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                        putress->AI()->Talk(SAY_PUTRESS_POWER_WASHES);
                    me->SummonCreatureGroup(SUMMON_GROUP_FAILED_EXPERIMENTS);
                })
                .Schedule(44s, [this](TaskContext /*context*/)
                {
                    if (Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS))
                    {
                        putress->AI()->Talk(SAY_PUTRESS_MORE_SUFFERING);
                        putress->CastSpell(putress, SPELL_BLIGHT_OVERLOAD);
                    }
                })
                .Schedule(44500ms, [this](TaskContext /*context*/)
                {
                    Creature* putress = GetActor(DATA_GRAND_APOTHECARY_PUTRESS);
                    if (!putress)
                        return;

                    putress->RemoveAurasDueToSpell(SPELL_PUTRESS_CASTING_STATE);
                    putress->SetImmuneToAll(false);
                    putress->SetReactState(REACT_AGGRESSIVE);
                    putress->AI()->AttackStart(me);
                    AttackStart(putress);
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->DoAction(ACTION_JAINA_FOLLOW);
                });
                break;
            case POINT_VARIAN_APOTHECARIUM_SECURED:
                SetEscortPaused(true);
                _scheduler.Schedule(4s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HOW_MUCH_LONGER);
                })
                .Schedule(11500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_HORDES_WORLD);
                })
                .Schedule(19s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_EVIL_IN_ORCS);
                    std::list<TempSummon*> horde;
                    me->SummonCreatureGroup(SUMMON_GROUP_THRONE_HORDE, &horde);
                    for (TempSummon* defender : horde)
                    {
                        defender->SetImmuneToAll(true);
                        defender->SetReactState(REACT_PASSIVE);
                        defender->SetEmoteState(EMOTE_STATE_READY2H);
                        if (defender->GetEntry() == NPC_THRALL)
                            defender->CastSpell(defender, SPELL_WARCHIEFS_BLESSING);
                    }
                })
                .Schedule(29s, [this](TaskContext /*context*/)
                {
                    if (Creature* thrall = GetActor(DATA_THRALL_ALLIANCE_RUN))
                        thrall->AI()->Talk(SAY_THRALL_ALLIANCE_RUN_VICTORIOUS);
                })
                .Schedule(32s, [this](TaskContext /*context*/)
                {
                    if (Creature* thrall = GetActor(DATA_THRALL_ALLIANCE_RUN))
                        thrall->AI()->Talk(SAY_THRALL_ALLIANCE_RUN_LOKTAR);
                })
                .Schedule(37s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_THRALL_HERE);
                })
                .Schedule(40s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_END_THIS_NOW);
                })
                .Schedule(41500ms, [this](TaskContext /*context*/)
                {
                    me->SetImmuneToNPC(true);
                    SetEscortPaused(false);
                })
                .Schedule(42s, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->AI()->Talk(SAY_JAINA_WONT_HELP_YOU);
                });
                break;
            case POINT_VARIAN_THRONE_HALL:
                Talk(SAY_VARIAN_LOKTAR_OGAR);
                SetEscortPaused(true);
                _scheduler.Schedule(10s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_TO_THE_THRONE_ROOM);
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                    {
                        jaina->AI()->DoAction(ACTION_JAINA_HOLD);
                        jaina->SetImmuneToAll(true);
                        jaina->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE_THRONE_HALL, false);
                    }
                    SetEscortPaused(false);
                });
                break;
            case POINT_VARIAN_THRONE:
                SetEscortPaused(true);
                Talk(SAY_VARIAN_AWAY_TOO_LONG);
                {
                    std::list<TempSummon*> alliance;
                    me->SummonCreatureGroup(SUMMON_GROUP_THRONE_ALLIANCE, &alliance);
                    for (TempSummon* elite : alliance)
                    {
                        elite->SetImmuneToAll(true);
                        elite->SetEmoteState(EMOTE_STATE_READY2H);
                    }
                }
                _scheduler.Schedule(15s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_MAKE_THINGS_RIGHT);
                })
                .Schedule(30s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_WAITED_LONG);
                })
                .Schedule(46500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_IT_ENDS_NOW);
                })
                .Schedule(52500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_VARIAN_ATTACK);
                    me->SetImmuneToAll(false);

                    std::vector<Creature*> elites;
                    std::vector<Creature*> battleguards;
                    for (ObjectGuid const& guid : _summons)
                    {
                        Creature* summon = ObjectAccessor::GetCreature(*me, guid);
                        if (!summon)
                            continue;

                        switch (summon->GetEntry())
                        {
                            case NPC_THRALL:
                            case NPC_SYLVANAS:
                                summon->SetImmuneToNPC(false);
                                summon->SetReactState(REACT_AGGRESSIVE);
                                summon->AI()->AttackStart(me);
                                break;
                            case NPC_STORMWIND_ELITE:
                                if (summon->IsImmuneToNPC())
                                    elites.push_back(summon);
                                break;
                            case NPC_WARSONG_BATTLEGUARD_THRONE:
                                battleguards.push_back(summon);
                                break;
                            default:
                                break;
                        }
                    }

                    for (size_t i = 0; i < elites.size() && i < battleguards.size(); ++i)
                    {
                        elites[i]->SetImmuneToNPC(false);
                        battleguards[i]->SetImmuneToNPC(false);
                        battleguards[i]->SetReactState(REACT_AGGRESSIVE);
                        elites[i]->AI()->AttackStart(battleguards[i]);
                        battleguards[i]->AI()->AttackStart(elites[i]);
                    }
                })
                .Schedule(58500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                    {
                        jaina->GetMotionMaster()->MovePath(PATH_JAINA_PROUDMOORE_THRONE, false);
                        jaina->AI()->Talk(SAY_JAINA_VARIAN_NO);
                    }
                })
                .Schedule(66500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                    {
                        jaina->CastSpell(jaina, SPELL_DEEP_FREEZE_CREDIT);
                        jaina->AI()->Talk(SAY_JAINA_DID_NOT_HAVE_TO_BE);
                    }
                })
                .Schedule(71500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* jaina = GetActor(DATA_JAINA_PROUDMOORE))
                        jaina->CastSpell(jaina, SPELL_GREATER_MASS_TELEPORT);
                })
                .Schedule(77s, [this](TaskContext /*context*/)
                {
                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_IDLE);
                });
                break;
            default:
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        _summons.Despawn(summon);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(20s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastSelf(SPELL_WHIRLWIND);
            context.Repeat(20s);
        })
        .Schedule(15s, 30s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_HEROIC_LEAP);
            context.Repeat(15s, 30s);
        });
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelGroup(GROUP_COMBAT);
        EscortAI::EnterEvadeMode(why);
    }

    void UpdateEscortAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    bool OnGossipHello(Player* player) override
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        bool const idle = zoneScript && zoneScript->GetData(DATA_RUN_STATE) == RUN_STATE_IDLE;

        player->PrepareGossipMenu(me, GOSSIP_MENU_VARIAN, true);
        if (!idle)
            player->PlayerTalkClass->GetGossipMenu().ClearMenu();

        SendGossipMenuFor(player, idle ? NPC_TEXT_VARIAN_READY : NPC_TEXT_VARIAN_BUSY, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        if (!zoneScript || menuId != GOSSIP_MENU_VARIAN || gossipListId != GOSSIP_OPTION_START || !player->PlayerTalkClass->GetGossipMenu().GetItem(gossipListId))
            return false;

        CloseGossipMenuFor(player);
        if (zoneScript->GetData(DATA_RUN_STATE) != RUN_STATE_IDLE)
            return true;

        zoneScript->SetData(DATA_RUN_STATE, RUN_STATE_COUNTDOWN);
        _scheduler.Schedule(250s, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_HEROES);
        })
        .Schedule(260s, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_HIDDEN_INSIDE);
        })
        .Schedule(270s, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_SOON_WE_MARCH);
        })
        .Schedule(290s, [this](TaskContext /*context*/)
        {
            Talk(SAY_VARIAN_HOUR_OF_REDEMPTION);
        });
        return true;
    }

private:
    Creature* GetActor(uint32 type) const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(type)) : nullptr;
    }

    SummonList _summons;
    TaskScheduler _scheduler;
};

// 32402 - Lady Jaina Proudmoore
struct EG_npc_undercity_jaina : public ScriptedAI
{
    EG_npc_undercity_jaina(Creature* creature) : ScriptedAI(creature), _following(false)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_JAINA_FOLLOW:
                _following = true;
                me->SetImmuneToNPC(false);
                me->SetReactState(REACT_AGGRESSIVE);
                if (Unit* varian = GetVarian())
                    me->GetMotionMaster()->MoveFollow(varian, 5.0f, ChaseAngle(float(M_PI)));
                break;
            case ACTION_JAINA_HOLD:
                _following = false;
                me->GetMotionMaster()->MoveIdle();
                break;
            default:
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(1s, GROUP_COMBAT, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                DoCast(target, SPELL_FIREBALL);
            context.Repeat(3s);
        })
        .Schedule(8s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BLIZZARD);
            context.Repeat(15s);
        })
        .Schedule(30s, GROUP_COMBAT, [this](TaskContext context)
        {
            DoCastSelf(SPELL_SUMMON_WATER_ELEMENTALS);
            context.Repeat(90s);
        });
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelGroup(GROUP_COMBAT);

        Unit* varian = GetVarian();
        if (!_following || !varian)
        {
            ScriptedAI::EnterEvadeMode(why);
            return;
        }

        if (!_EnterEvadeMode(why))
            return;

        me->GetMotionMaster()->MoveFollow(varian, 5.0f, ChaseAngle(float(M_PI)));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
        {
            if (_following)
                if (Unit* varian = GetVarian())
                    if (Unit* victim = varian->GetVictim())
                        AttackStart(victim);
            return;
        }

        _scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

private:
    Unit* GetVarian() const
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        return zoneScript ? ObjectAccessor::GetCreature(*me, zoneScript->GetGuidData(DATA_VARIAN_WRYNN)) : nullptr;
    }

    TaskScheduler _scheduler;
    bool _following;
};

// 32483 - Engorged Blight Worm
struct EG_npc_undercity_blight_worm : public ScriptedAI
{
    EG_npc_undercity_blight_worm(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void Reset() override
    {
        _scheduler.CancelAll();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(2s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                DoCast(target, SPELL_INGEST);
            context.Repeat(20s);
        })
        .Schedule(750ms, [this](TaskContext context)
        {
            DoCastSelf(SPELL_BLIGHT_BREATH);
            context.Repeat(15s);
        });
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
                jaina->GetMotionMaster()->MovePoint(0, jaina->GetHomePosition());
        })
        .Schedule(126700ms, [this](TaskContext /*context*/)
        {
            Talk(SAY_THRALL_ORGRIMMAR_PREPARE_TRANSPORT);
        })
        .Schedule(127700ms, [this](TaskContext /*context*/)
        {
            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanasGuid))
                sylvanas->AI()->EnterEvadeMode();
        })
        .Schedule(170s, [this](TaskContext /*context*/)
        {
            _sceneInProgress = false;
        });
    }

    void JustSummoned(Creature* summon) override
    {
        _jainaGuid = summon->GetGUID();
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
    GuidVector _korkronGuids;
    bool _sceneInProgress;
};

// 60207 - Call of Earth
class EG_spell_undercity_call_of_earth : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_call_of_earth);

    void HandleAfterCast()
    {
        ZoneScript* zoneScript = GetCaster()->GetZoneScript();
        if (!zoneScript)
            return;

        for (uint32 type : { DATA_BLOCKED_PASSAGE_1, DATA_BLOCKED_PASSAGE_2, DATA_BLOCKED_PASSAGE_3 })
            if (GameObject* passage = ObjectAccessor::GetGameObject(*GetCaster(), zoneScript->GetGuidData(type)))
                passage->UseDoorOrButton();
    }

    void Register() override
    {
        AfterCast += SpellCastFn(EG_spell_undercity_call_of_earth::HandleAfterCast);
    }
};

// 60516 - Greater Mass Teleport
class EG_spell_undercity_greater_mass_teleport : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_greater_mass_teleport);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
            target->DespawnOrUnsummon(1s);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_greater_mass_teleport::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 61123 - Ingest
class EG_spell_undercity_ingest : public SpellScript
{
    PrepareSpellScript(EG_spell_undercity_ingest);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ uint32(spellInfo->GetEffect(EFFECT_0).CalcValue()) });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_undercity_ingest::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_EG_battle_for_undercity()
{
    RegisterCreatureAI(EG_npc_undercity_thrall);
    RegisterCreatureAI(EG_npc_undercity_thrall_alliance_run);
    RegisterCreatureAI(EG_npc_undercity_sylvanas);
    RegisterCreatureAI(EG_npc_undercity_sylvanas_alliance_run);
    RegisterCreatureAI(EG_npc_undercity_thrall_staging);
    RegisterCreatureAI(EG_npc_undercity_varian);
    RegisterCreatureAI(EG_npc_undercity_jaina);
    RegisterCreatureAI(EG_npc_undercity_blight_worm);
    RegisterCreatureAI(EG_npc_undercity_thrall_orgrimmar);
    RegisterSpellScript(EG_spell_undercity_call_of_earth);
    RegisterSpellScript(EG_spell_undercity_ingest);
    RegisterSpellScript(EG_spell_undercity_greater_mass_teleport);
    new EG_zone_undercity_battle_horde();
    new EG_zone_undercity_battle_alliance();
}
