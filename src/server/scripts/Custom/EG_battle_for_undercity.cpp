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
#include "MapManager.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "PetDefines.h"
#include "Player.h"
#include "Random.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "SpawnData.h"
#include "SpellScript.h"
#include "TaskScheduler.h"
#include "WorldSession.h"
#include "WorldStatePackets.h"
#include <unordered_map>

enum UndercityBattleMisc
{
    MAP_EASTERN_KINGDOMS = 0,
    ZONE_TIRISFAL_GLADES = 85,
    ZONE_UNDERCITY = 1497,
    PHASE_HORDE = 64,
    PHASE_ALLIANCE = 128,

    QUEST_BATTLE_HORDE = 13267,
    QUEST_BATTLE_ALLIANCE = 13377,

    ACTION_START_BATTLE = 1,

    DATA_ACTOR_DIED = 1,

    SPAWN_GROUP_ALLIANCE_BLIGHT_WORM = 1003,
    SPAWN_GROUP_ALLIANCE_SEWER_REINFORCEMENTS,
    SPAWN_GROUP_ALLIANCE_APOTHECARIUM,
    SPAWN_GROUP_ALLIANCE_THRONE_HORDE,
    SPAWN_GROUP_ALLIANCE_THRONE_ALLIANCE,
    SPAWN_GROUP_ALLIANCE_WAVE_ORIGINS,
    SPAWN_GROUP_HORDE_LEADERS,
    SPAWN_GROUP_HORDE_GATE_BLIGHT,
    SPAWN_GROUP_HORDE_VORTICES,
    SPAWN_GROUP_HORDE_COURTYARD_DEFENDERS,
    SPAWN_GROUP_HORDE_VARIMATHRAS_COURTYARD,
    SPAWN_GROUP_HORDE_BLIGHT_ABERRATION,
    SPAWN_GROUP_HORDE_COURTYARD_SECURED,
    SPAWN_GROUP_HORDE_ELEVATOR_SHAFT,
    SPAWN_GROUP_HORDE_VARIMATHRAS_SANCTUM,
    SPAWN_GROUP_HORDE_KHANOK,
    SPAWN_GROUP_HORDE_VARIMATHRAS_THRONE,
    SPAWN_GROUP_HORDE_THRONE_ALLIANCE,
    SPAWN_GROUP_HORDE_SAURFANG,
    SPAWN_GROUP_HORDE_DISTANT_VOICE,
    SPAWN_GROUP_HORDE_WAVE_ORIGINS,

    SPAWN_ORIGIN_ALLIANCE_SEWER_MOUTH = 1300141,
    SPAWN_ORIGIN_ALLIANCE_SEWER_PACK_1,
    SPAWN_ORIGIN_ALLIANCE_SEWER_PACK_2,
    SPAWN_ORIGIN_ALLIANCE_SEWER_PACK_3,
    SPAWN_ORIGIN_ALLIANCE_SEWER_HOLD_LEFT,
    SPAWN_ORIGIN_ALLIANCE_SEWER_HOLD_RIGHT,
    SPAWN_ORIGIN_ALLIANCE_SEWER_HOLD_GUARDIANS,
    SPAWN_ORIGIN_ALLIANCE_APOTHECARIUM_GUARDIANS,
    SPAWN_ORIGIN_ALLIANCE_APOTHECARIUM_DREADLORDS,
    SPAWN_ORIGIN_ALLIANCE_EXPERIMENTS_1,
    SPAWN_ORIGIN_ALLIANCE_EXPERIMENTS_2,
    SPAWN_ORIGIN_ALLIANCE_EXPERIMENTS_3,
    SPAWN_ORIGIN_ALLIANCE_EXPERIMENTS_4,
    SPAWN_ORIGIN_HORDE_COURTYARD_GUARDIANS = 1300308,
    SPAWN_ORIGIN_HORDE_COURTYARD_DOCTORS,
    SPAWN_ORIGIN_HORDE_COURTYARD_CHEMISTS,
    SPAWN_ORIGIN_HORDE_SANCTUM_TOP,
    SPAWN_ORIGIN_HORDE_SANCTUM_LEFT,
    SPAWN_ORIGIN_HORDE_SANCTUM_RIGHT,
    SPAWN_ORIGIN_HORDE_KHANOK_LEFT,
    SPAWN_ORIGIN_HORDE_KHANOK_RIGHT,
    SPAWN_ORIGIN_HORDE_KHANOK_ABOVE,
    SPAWN_ORIGIN_HORDE_BATTLEGUARD_CHARGE,
    SPAWN_ORIGIN_HORDE_THRONE_LEFT,
    SPAWN_ORIGIN_HORDE_THRONE_RIGHT,

    POINT_WRYNN_STAGING = 0,
    POINT_WRYNN_SEWER_ENTRANCE = 2,
    POINT_WRYNN_SEWER_HOLD = 38,
    POINT_WRYNN_APOTHECARIUM_APPROACH = 45,
    POINT_WRYNN_KHANOK_CORPSE = 46,
    POINT_WRYNN_PUTRESS_TAUNT = 48,
    POINT_WRYNN_APOTHECARIUM_TRASH = 50,
    POINT_WRYNN_PUTRESS_PLATFORM = 63,
    POINT_WRYNN_PUTRESS = 65,
    POINT_WRYNN_PUTRESS_DEFEATED = 66,
    POINT_WRYNN_THRONE_ROOM_DOOR = 87,
    POINT_WRYNN_THRONE = 88,

    POINT_THRALL_STAGING = 1,
    POINT_THRALL_GATE = 2,
    POINT_THRALL_COURTYARD_ENTRANCE = 11,
    POINT_THRALL_COURTYARD = 13,
    POINT_THRALL_COURTYARD_CLEARED = 14,
    POINT_THRALL_ELEVATOR = 34,
    POINT_THRALL_SANCTUM_ENTRANCE = 36,
    POINT_THRALL_SANCTUM_TOP = 46,
    POINT_THRALL_SANCTUM_LEFT = 57,
    POINT_THRALL_SANCTUM_RIGHT = 61,
    POINT_THRALL_KHANOK = 65,
    POINT_THRALL_KHANOK_DEFEATED = 66,
    POINT_THRALL_PASSAGE_APPROACH = 75,
    POINT_THRALL_BLOCKED_PASSAGE = 81,
    POINT_THRALL_VARIMATHRAS = 104,
    POINT_THRALL_VARIMATHRAS_DEFEATED = 109,
    POINT_THRALL_THRONE_STEPS = 113,
    POINT_THRALL_ALLIANCE_ARRIVES = 117,
    POINT_THRALL_THRONE_APPROACH = 118,
    POINT_THRALL_THRONE = 120,

    NPC_SYLVANAS_STAGING = 31651,
    NPC_THRALL = 32518,
    NPC_SYLVANAS = 32365,
    NPC_WRYNN = 32401,
    NPC_JAINA = 32402,
    NPC_SAURFANG = 32315,
    NPC_STORMWIND_ELITE = 32387,
    NPC_WARSONG_BATTLEGUARD = 31739,
    NPC_WARSONG_BATTLEGUARD_THRONE = 32510,
    NPC_PUTRESS = 31530,
    NPC_FAILED_EXPERIMENT = 32519,
    NPC_APOTHECARY_GENERATOR = 36212,
    NPC_BLIGHT_WORM = 32483,
    NPC_KHANOK = 32511,
    NPC_VARIMATHRAS = 31565,
    NPC_VARIMATHRAS_PORTAL = 31811,
    NPC_DISTANT_VOICE = 32277,
    NPC_GREAT_WIND_VORTEX = 31782,
    NPC_TIDAL_WAVE = 31765,
    NPC_CAVE_IN_DUMMY = 32200,
    NPC_PLAGUE_TRIGGER = 31576,
    NPC_SLINGER_TRIGGER = 31577,
    NPC_BLIGHT_ABERRATION = 31844,
    NPC_BLIGHT_SLINGER = 31526,
    NPC_DOOMGUARD_PILLAGER = 32159,
    NPC_LEGION_OVERLORD = 32271,
    NPC_LEGION_INVADER = 32269,
    NPC_LEGION_DREADWHISPERER = 32270,
    NPC_TREACHEROUS_GUARDIAN_H = 31532,
    NPC_PERFIDIOUS_DREADLORD_H = 31531,
    NPC_PLAGUED_FELBEAST_H = 31528,
    NPC_FELGUARD_MARAUDER_H = 31527,
    NPC_RAVISHING_BETRAYER_H = 31529,
    NPC_APOTHECARY_CHEMIST_H = 31482,
    NPC_BLIGHT_DOCTOR_H = 31516,
    NPC_TREACHEROUS_GUARDIAN_A = 32390,
    NPC_PERFIDIOUS_DREADLORD_A = 32391,
    NPC_PLAGUED_FELBEAST_A = 32392,
    NPC_FELGUARD_MARAUDER_A = 32393,
    NPC_RAVISHING_BETRAYER_A = 32394,
    NPC_APOTHECARY_CHEMIST_A = 32395,
    NPC_APOTHECARY_COLLABORATOR_A = 32396,
    NPC_BLIGHT_DOCTOR_A = 32397,

    GO_BLOCKED_PASSAGE = 194935,

    NPC_TEXT_THRALL_READY = 14050,
    NPC_TEXT_THRALL_BUSY = 14051,
    NPC_TEXT_WRYNN_READY = 14140,
    NPC_TEXT_WRYNN_BUSY = 14142,
    GOSSIP_MENU_WRYNN = 10194,
    GOSSIP_MENU_THRALL = 57026,
    GOSSIP_OPTION_START = 0,

    PATH_WRYNN = 3240100,
    PATH_THRALL = 3251800,
    PATH_VORTEX_LEFT = 316880,
    PATH_VORTEX_RIGHT = 3168800,
    PATH_BATTLEGUARD_CHARGE = 3173900,
    PATH_DOOMGUARD_PILLAGER = 3215900,

    SOUND_PUTRESS_LAUGH = 16920,
    SOUND_THRALL_THRONE_1 = 16212,
    SOUND_THRALL_THRONE_2 = 16214,

    WORLD_STATE_ALLIANCE_COUNTDOWN = 3958,
    WORLD_STATE_ALLIANCE_SEWERS_FIGHT = 3962,
    WORLD_STATE_ALLIANCE_SEWERS_DONE = 3964,
    WORLD_STATE_ALLIANCE_STARTS = 3966,
    WORLD_STATE_ALLIANCE_APOTHECARIUM_DONE = 3971,
    WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT = 3972,
    WORLD_STATE_HORDE_START = 3875,
    WORLD_STATE_HORDE_COUNTDOWN = 3876,
    WORLD_STATE_HORDE_COURTYARD_FIGHT = 3885,
    WORLD_STATE_HORDE_COURTYARD_DONE = 3886,
    WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT = 3887,
    WORLD_STATE_HORDE_INNER_SANCTUM_DONE = 3888,
    WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT = 3889,
    WORLD_STATE_HORDE_ROYAL_QUARTER_DONE = 3890,

    SPELL_WRYNN_BUFF = 60964,
    SPELL_JAINA_BUFF = 61011,
    SPELL_THRALL_BUFF = 64670,
    SPELL_SYLVANAS_BUFF = 59756,
    SPELL_WHIRLWIND = 41056,
    SPELL_HEROIC_LEAP = 57793,
    SPELL_THUNDER = 59507,
    SPELL_FIREBALL = 20692,
    SPELL_BLIZZARD = 20680,
    SPELL_SUMMON_WATER_ELEMENTALS = 20681,
    SPELL_DEEP_FREEZE = 61224,
    SPELL_CHAIN_LIGHTNING = 59517,
    SPELL_LAVA_BURST = 59519,
    SPELL_HEROIC_VANGUARD = 59506,
    SPELL_CALL_OF_AIR = 59898,
    SPELL_CALL_OF_EARTH = 60207,
    SPELL_TIDAL_WAVE_SUMMON = 59627,
    SPELL_TIDAL_WAVE = 59635,
    SPELL_BLACK_ARROW = 59712,
    SPELL_SHOT = 59710,
    SPELL_MULTI_SHOT = 59713,
    SPELL_SHRIEK_OF_THE_HIGHBORNE = 59514,
    SPELL_INGEST = 61123,
    SPELL_INGEST_TRIGGER = 61124,
    SPELL_BLIGHT_BREATH = 61125,
    SPELL_BLIGHT_EMPOWERMENT = 59449,
    SPELL_BLIGHT_OVERLOAD = 61181,
    SPELL_THROW_BLIGHT_BARREL = 59460,
    SPELL_PUTRESS_CASTING_STATE = 59447,
    SPELL_VARIMATHRAS_PORTAL = 68424,
    SPELL_CARRION_SWARM = 59434,
    SPELL_DRAIN_LIFE = 17238,
    SPELL_MIGHT_OF_VARIMATHRAS = 59424,
    SPELL_SHADOW_BOLT_VOLLEY = 20741,
    SPELL_AURA_OF_VARIMATHRAS = 60289,
    SPELL_OPENING_LEGION_PORTALS = 60224,
    SPELL_VEIL_OF_SHADOW = 69633,
    SPELL_BLIGHT_BOMB = 48211,

    GROUP_IDLE = 1,
    GROUP_EMPTY,

    SAY_WRYNN_PREP_1 = 0,
    SAY_WRYNN_PREP_2,
    SAY_WRYNN_PREP_3,
    SAY_WRYNN_PREP_4,
    SAY_WRYNN_PREP_5,
    SAY_WRYNN_PREP_6,
    SAY_WRYNN_SEWERS_1,
    SAY_WRYNN_SEWERS_2,
    SAY_WRYNN_SEWERS_3,
    SAY_WRYNN_SEWERS_4,
    SAY_WRYNN_APOTHECARIUM_1,
    SAY_WRYNN_APOTHECARIUM_2,
    SAY_WRYNN_APOTHECARIUM_3,
    SAY_WRYNN_APOTHECARIUM_4,
    SAY_WRYNN_APOTHECARIUM_5,
    SAY_WRYNN_APOTHECARIUM_6,
    SAY_WRYNN_APOTHECARIUM_7,
    SAY_WRYNN_APOTHECARIUM_8,
    SAY_WRYNN_APOTHECARIUM_9,
    SAY_WRYNN_APOTHECARIUM_10,
    SAY_WRYNN_APOTHECARIUM_11,
    SAY_WRYNN_APOTHECARIUM_12,
    SAY_WRYNN_THRONE_1,
    SAY_WRYNN_THRONE_2,
    SAY_WRYNN_THRONE_3,
    SAY_WRYNN_THRONE_4,
    SAY_WRYNN_THRONE_5,
    SAY_WRYNN_THRONE_6,
    SAY_WRYNN_THRONE_7,
    SAY_WRYNN_THRONE_8,
    SAY_WRYNN_THRONE_9,

    SAY_JAINA_SEWERS = 0,
    SAY_JAINA_APOTHECARIUM,
    SAY_JAINA_THRONE_1,
    SAY_JAINA_THRONE_2,
    SAY_JAINA_THRONE_3,

    SAY_THRALL_ALLIANCE_THRONE_1 = 0,
    SAY_THRALL_ALLIANCE_THRONE_2,
    SAY_THRALL_PREP_1,
    SAY_THRALL_PREP_2,
    SAY_THRALL_PREP_3,
    SAY_THRALL_PREP_4,
    SAY_THRALL_PREP_5,
    SAY_THRALL_PREP_6,
    SAY_THRALL_PREP_7,
    SAY_THRALL_PREP_8,
    SAY_THRALL_COURTYARD_1,
    SAY_THRALL_COURTYARD_2,
    SAY_THRALL_COURTYARD_3,
    SAY_THRALL_COURTYARD_4,
    SAY_THRALL_COURTYARD_5,
    SAY_THRALL_ELEVATOR_1,
    SAY_THRALL_ELEVATOR_2,
    SAY_THRALL_ELEVATOR_3,
    SAY_THRALL_SANCTUM_1,
    SAY_THRALL_SANCTUM_2,
    SAY_THRALL_SANCTUM_3,
    SAY_THRALL_SANCTUM_4,
    SAY_THRALL_SANCTUM_5,
    SAY_THRALL_SANCTUM_6,
    SAY_THRALL_SANCTUM_7,
    SAY_THRALL_THRONE_1,
    SAY_THRALL_THRONE_2,
    SAY_THRALL_THRONE_3,
    SAY_THRALL_THRONE_4,
    SAY_THRALL_THRONE_5,
    SAY_THRALL_THRONE_6,
    SAY_THRALL_THRONE_7,
    SAY_THRALL_THRONE_8,
    SAY_THRALL_THRONE_9,
    SAY_THRALL_THRONE_10,
    SAY_THRALL_THRONE_11,

    SAY_SYLVANAS_COURTYARD = 0,
    SAY_SYLVANAS_ELEVATOR,
    SAY_SYLVANAS_SANCTUM_1,
    SAY_SYLVANAS_SANCTUM_2,
    SAY_SYLVANAS_SANCTUM_3,
    SAY_SYLVANAS_SANCTUM_4,
    SAY_SYLVANAS_SANCTUM_5,
    SAY_SYLVANAS_THRONE,

    SAY_PUTRESS_1 = 0,
    SAY_PUTRESS_2,
    SAY_PUTRESS_3,
    SAY_PUTRESS_4,
    SAY_PUTRESS_5,
    SAY_PUTRESS_6,
    SAY_PUTRESS_7,
    SAY_PUTRESS_8,

    SAY_VARIMATHRAS_INTRO_1 = 0,
    SAY_VARIMATHRAS_INTRO_2,
    SAY_VARIMATHRAS_INTRO_3,
    SAY_VARIMATHRAS_SANCTUM_1,
    SAY_VARIMATHRAS_SANCTUM_2,
    SAY_VARIMATHRAS_CLOSE_DOOR,
    SAY_VARIMATHRAS_THRONE_1,
    SAY_VARIMATHRAS_THRONE_2,
    SAY_VARIMATHRAS_THRONE_3,
    SAY_VARIMATHRAS_THRONE_4,
    SAY_VARIMATHRAS_THRONE_5,
    SAY_VARIMATHRAS_THRONE_6,
    SAY_VARIMATHRAS_ATTACK,
    SAY_VARIMATHRAS_DEATH,

    SAY_BATTLEGUARD_BURN = 0,
    SAY_BATTLEGUARD_PUTRESS,
    SAY_BATTLEGUARD_FOR_THE_HORDE,

    SAY_SAURFANG_ARRIVAL_1 = 0,
    SAY_SAURFANG_ARRIVAL_2,
    SAY_SAURFANG_ARRIVAL_3,

    SAY_SUMMONED = 0
};

class UndercityBattleMgr
{
public:
    static UndercityBattleMgr* instance()
    {
        static UndercityBattleMgr instance;
        return &instance;
    }

    bool IsRunning(TeamId team) const
    {
        return _battles[team].Running;
    }

    bool IsLeader(TeamId team, ObjectGuid const& guid) const
    {
        return _battles[team].Running && _battles[team].Leader == guid;
    }

    bool CanStart(Player const* player, TeamId team) const
    {
        Battle const& battle = _battles[team];
        return !battle.Running && player->GetTeamId() == team && player->GetQuestStatus(battle.Quest) == QUEST_STATUS_INCOMPLETE;
    }

    GuidSet const& GetParticipants(TeamId team) const
    {
        return _battles[team].Participants;
    }

    void Start(TeamId team)
    {
        _battles[team].Running = true;
        RestartIdleTimer(team);
    }

    void SetLeader(TeamId team, ObjectGuid const& leader)
    {
        _battles[team].Leader = leader;
    }

    void End(TeamId team, Milliseconds delay = 0s)
    {
        Battle& battle = _battles[team];
        if (delay == 0s)
            battle.Ended = true;
        else
        {
            battle.Timers.Schedule(delay, [&battle](TaskContext /*context*/)
            {
                battle.Ended = true;
            });
        }
    }

    void RestartIdleTimer(TeamId team)
    {
        Battle& battle = _battles[team];
        battle.Timers.CancelGroup(GROUP_IDLE).Schedule(20min, GROUP_IDLE, [&battle](TaskContext /*context*/)
        {
            battle.Ended = true;
        });
    }

    void AddStaticActor(TeamId team, ObjectGuid const& guid)
    {
        _battles[team].StaticActors.push_back(guid);
    }

    void AddHiddenActor(TeamId team, ObjectGuid const& guid)
    {
        _battles[team].HiddenActors.push_back(guid);
    }

    void AddSummon(TeamId team, ObjectGuid const& guid)
    {
        _battles[team].Summons.push_back(guid);
    }

    void SpawnGroup(TeamId team, Map* map, uint32 groupId)
    {
        for (auto const& pair : sObjectMgr->GetSpawnMetadataForGroup(groupId))
            if (SpawnData const* data = pair.second->ToSpawnData())
                map->LoadGrid(data->spawnPoint.GetPositionX(), data->spawnPoint.GetPositionY());

        _battles[team].SpawnGroups.push_back(groupId);
        map->SpawnGroupSpawn(groupId, true);
    }

    void SetWorldState(TeamId team, uint32 worldState, uint32 value)
    {
        Battle& battle = _battles[team];
        battle.WorldStates[worldState] = value;
        for (ObjectGuid const& guid : battle.Participants)
            if (Player* player = ObjectAccessor::FindPlayer(guid))
                player->SendUpdateWorldState(worldState, value);
    }

    void PlaySoundForParticipants(TeamId team, WorldObject* source, uint32 soundId)
    {
        for (ObjectGuid const& guid : _battles[team].Participants)
            if (Player* player = ObjectAccessor::GetPlayer(*source, guid))
                source->PlayDirectSound(soundId, player);
    }

    void CompleteQuest(TeamId team, WorldObject const* center)
    {
        Battle const& battle = _battles[team];
        for (ObjectGuid const& guid : battle.Participants)
            if (Player* player = ObjectAccessor::GetPlayer(*center, guid))
                if (player->GetQuestStatus(battle.Quest) == QUEST_STATUS_INCOMPLETE && player->IsWithinDist(center, 100.0f))
                    player->AreaExploredOrEventHappens(battle.Quest);
    }

    void AddZonePlayer(ObjectGuid const& guid)
    {
        _zonePlayers.insert(guid);
    }

    void RemoveZonePlayer(ObjectGuid const& guid)
    {
        _zonePlayers.erase(guid);
    }

    void FillInitialWorldStates(Player const* player, WorldPackets::WorldState::InitWorldStates& packet) const
    {
        for (Battle const& battle : _battles)
            if (battle.Running && (player->GetPhaseMask() & battle.Phase) && player->GetTeamId() == battle.Team)
                for (std::pair<uint32 const, uint32> const& worldState : battle.WorldStates)
                    packet.Worldstates.emplace_back(worldState.first, worldState.second);
    }

    void HandleCreatureDeath(Creature const* creature) const
    {
        Battle const& battle = _battles[(creature->GetPhaseMask() & PHASE_HORDE) ? TEAM_HORDE : TEAM_ALLIANCE];
        if (!battle.Running)
            return;

        if (Creature* leader = ObjectAccessor::GetCreature(*creature, battle.Leader))
            leader->AI()->SetData(DATA_ACTOR_DIED, creature->GetEntry());
    }

    void Update(uint32 diff)
    {
        for (Battle& battle : _battles)
            if (battle.Running)
                battle.Timers.Update(diff);

        _scheduler.Update(diff);
    }

private:
    struct Battle
    {
        TeamId Team;
        uint32 Phase;
        uint32 Quest;
        bool Running = false;
        bool Ended = false;
        bool Empty = false;
        ObjectGuid Leader;
        GuidVector StaticActors;
        GuidVector HiddenActors;
        GuidVector Summons;
        std::vector<uint32> SpawnGroups;
        GuidSet Participants;
        std::unordered_map<uint32, uint32> WorldStates;
        TaskScheduler Timers;
    };

    UndercityBattleMgr() : _battles{ { TEAM_ALLIANCE, PHASE_ALLIANCE, QUEST_BATTLE_ALLIANCE }, { TEAM_HORDE, PHASE_HORDE, QUEST_BATTLE_HORDE } }
    {
        _scheduler.Schedule(1s, [this](TaskContext context)
        {
            context.Repeat(1s);

            Map* map = sMapMgr->FindBaseNonInstanceMap(MAP_EASTERN_KINGDOMS);
            if (!map)
                return;

            for (Battle& battle : _battles)
            {
                if (!battle.Running)
                    continue;

                GuidSet participants;
                for (ObjectGuid const& guid : _zonePlayers)
                    if (Player* player = map->GetPlayer(guid))
                        if (player->IsInWorld() && (player->GetPhaseMask() & battle.Phase) && player->GetTeamId() == battle.Team)
                            participants.insert(guid);

                for (ObjectGuid const& guid : participants)
                    if (!battle.Participants.count(guid))
                        if (Player* player = map->GetPlayer(guid))
                            for (std::pair<uint32 const, uint32> const& worldState : battle.WorldStates)
                                player->SendUpdateWorldState(worldState.first, worldState.second);

                for (ObjectGuid const& guid : battle.Participants)
                    if (!participants.count(guid))
                        if (Player* player = map->GetPlayer(guid))
                            for (std::pair<uint32 const, uint32> const& worldState : battle.WorldStates)
                                player->SendUpdateWorldState(worldState.first, 0);

                battle.Participants = std::move(participants);
                if (battle.Participants.empty() != battle.Empty)
                {
                    battle.Empty = !battle.Empty;
                    battle.Timers.CancelGroup(GROUP_EMPTY);
                    if (battle.Empty)
                    {
                        battle.Timers.Schedule(10min, GROUP_EMPTY, [&battle](TaskContext /*context*/)
                        {
                            battle.Ended = true;
                        });
                    }
                }

                Creature* leader = map->GetCreature(battle.Leader);
                if (!battle.Ended && leader && leader->IsAlive())
                    continue;

                for (ObjectGuid const& guid : battle.Participants)
                    if (Player* player = map->GetPlayer(guid))
                        for (std::pair<uint32 const, uint32> const& worldState : battle.WorldStates)
                            player->SendUpdateWorldState(worldState.first, 0);

                for (uint32 groupId : battle.SpawnGroups)
                    map->SpawnGroupDespawn(groupId, true);

                for (ObjectGuid const& guid : battle.Summons)
                    if (Creature* creature = map->GetCreature(guid))
                        creature->DespawnOrUnsummon();

                for (ObjectGuid const& guid : battle.StaticActors)
                {
                    if (Creature* actor = map->GetCreature(guid))
                    {
                        actor->setActive(false);
                        actor->AI()->InitializeAI();
                        actor->DespawnOrUnsummon(0s, 30s);
                    }
                }

                for (ObjectGuid const& guid : battle.HiddenActors)
                    if (Creature* actor = map->GetCreature(guid))
                        actor->SetVisible(true);

                battle.Timers.CancelAll();
                battle.Running = false;
                battle.Ended = false;
                battle.Empty = false;
                battle.Leader.Clear();
                battle.StaticActors.clear();
                battle.HiddenActors.clear();
                battle.Summons.clear();
                battle.SpawnGroups.clear();
                battle.Participants.clear();
                battle.WorldStates.clear();
            }
        });
    }

    UndercityBattleMgr(UndercityBattleMgr const&) = delete;
    UndercityBattleMgr& operator=(UndercityBattleMgr const&) = delete;

    Battle _battles[PVP_TEAMS_COUNT];
    GuidSet _zonePlayers;
    TaskScheduler _scheduler;
};

#define sUndercityBattleMgr UndercityBattleMgr::instance()

static FindCreatureOptions const GateTriggers = { .CreatureIds = { NPC_PLAGUE_TRIGGER, NPC_SLINGER_TRIGGER } };
static FindCreatureOptions const CourtyardDefenders = { .CreatureIds = { NPC_TREACHEROUS_GUARDIAN_H, NPC_BLIGHT_DOCTOR_H, NPC_APOTHECARY_CHEMIST_H }, .IsAlive = true };

struct npc_eg_undercity_leaderAI : public EscortAI
{
    npc_eg_undercity_leaderAI(Creature* creature, TeamId team) : EscortAI(creature), _team(team), _stepping(false), _step(0)
    {
        _scheduler.SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void InitializeAI() override
    {
        EscortAI::InitializeAI();
        _steps.CancelAll();
        _stepping = false;
        _step = 0;
    }

    void JustSummoned(Creature* summon) override
    {
        if (IsLeader())
            sUndercityBattleMgr->AddSummon(_team, summon->GetGUID());
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (IsLeader())
            sUndercityBattleMgr->End(_team);
    }

    void UpdateEscortAI(uint32 diff) override
    {
        _steps.Update(diff);

        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this] { DoMeleeAttackIfReady(); });
    }

protected:
    bool IsLeader() const
    {
        return sUndercityBattleMgr->IsLeader(_team, me->GetGUID());
    }

    void Next(Milliseconds delay)
    {
        ++_step;
        if (_stepping)
            ScheduleStep(delay);
    }

    void StartStepping()
    {
        if (_stepping)
            return;

        _stepping = true;
        ScheduleStep(0s);
    }

    void Hold()
    {
        SetEscortPaused(true);
        StartStepping();
    }

    Position WaveOrigin(uint32 spawnId) const
    {
        if (CreatureData const* data = sObjectMgr->GetCreatureData(spawnId))
            return data->spawnPoint;

        return me->GetPosition();
    }

    void EngageWith(Creature* attacker, Unit* victim)
    {
        AddThreat(victim, 100.0f, attacker);
        attacker->AI()->AttackStart(victim);
    }

    TeamId const _team;
    TaskScheduler _scheduler;
    bool _stepping;
    uint32 _step;

private:
    virtual void RunStep() = 0;

    void ScheduleStep(Milliseconds delay)
    {
        _steps.Schedule(delay, [this](TaskContext /*context*/)
        {
            if (!IsLeader())
                return;

            sUndercityBattleMgr->RestartIdleTimer(_team);
            RunStep();
        });
    }

    TaskScheduler _steps;
};

struct npc_eg_undercity_wrynn : public npc_eg_undercity_leaderAI
{
    npc_eg_undercity_wrynn(Creature* creature) : npc_eg_undercity_leaderAI(creature, TEAM_ALLIANCE) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SYLVANAS_BUFF, true);
        _scheduler.CancelAll().Schedule(2s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_WRYNN_BUFF);
            context.Repeat(10s);
        }).Schedule(2s, [this](TaskContext context)
        {
            if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                if (!jaina->IsEngaged() && !jaina->IsImmuneToNPC())
                    jaina->AI()->AttackStart(me->GetVictim());
            DoCastSelf(SPELL_THUNDER);
            context.Repeat(2s);
        }).Schedule(5s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_WHIRLWIND);
            context.Repeat(20s);
        }).Schedule(10s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_HEROIC_LEAP);
            context.Repeat(15s, 30s);
        });
    }

    bool OnGossipHello(Player* player) override
    {
        if (!(me->GetPhaseMask() & PHASE_ALLIANCE))
        {
            CloseGossipMenuFor(player);
            return true;
        }

        bool const running = sUndercityBattleMgr->IsRunning(_team);
        if (running)
            InitGossipMenuFor(player, GOSSIP_MENU_WRYNN);
        else
            player->PrepareGossipMenu(me, GOSSIP_MENU_WRYNN, true);

        SendGossipMenuFor(player, running ? NPC_TEXT_WRYNN_BUSY : NPC_TEXT_WRYNN_READY, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        CloseGossipMenuFor(player);
        if (menuId != GOSSIP_MENU_WRYNN || gossipListId != GOSSIP_OPTION_START || !(me->GetPhaseMask() & PHASE_ALLIANCE) || !sUndercityBattleMgr->CanStart(player, _team) || me->IsEngaged())
            return true;

        Creature* jaina = me->FindNearestCreature(NPC_JAINA, 50.0f);
        if (!jaina)
            return true;

        _jaina = jaina->GetGUID();
        sUndercityBattleMgr->Start(_team);
        sUndercityBattleMgr->SetLeader(_team, me->GetGUID());
        sUndercityBattleMgr->AddStaticActor(_team, me->GetGUID());
        sUndercityBattleMgr->AddStaticActor(_team, _jaina);

        me->setActive(true);
        jaina->setActive(true);
        LoadPath(PATH_WRYNN);
        Start(true);
        SetDespawnAtEnd(false);
        return true;
    }

    void JustSummoned(Creature* summon) override
    {
        npc_eg_undercity_leaderAI::JustSummoned(summon);

        switch (summon->GetEntry())
        {
            case NPC_TREACHEROUS_GUARDIAN_A:
            case NPC_PERFIDIOUS_DREADLORD_A:
            case NPC_PLAGUED_FELBEAST_A:
            case NPC_FELGUARD_MARAUDER_A:
            case NPC_RAVISHING_BETRAYER_A:
            case NPC_APOTHECARY_CHEMIST_A:
            case NPC_APOTHECARY_COLLABORATOR_A:
            case NPC_BLIGHT_DOCTOR_A:
            case NPC_FAILED_EXPERIMENT:
                summon->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                summon->SetImmuneToAll(false);
                break;
            default:
                break;
        }
    }

    void SetData(uint32 id, uint32 value) override
    {
        if (id != DATA_ACTOR_DIED || !IsLeader())
            return;

        switch (value)
        {
            case NPC_BLIGHT_WORM:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_SEWERS_DONE, 1);
                StartStepping();
                break;
            case NPC_PUTRESS:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_APOTHECARIUM_DONE, 1);
                StartStepping();
                break;
            default:
                break;
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (!IsLeader())
            return;

        switch (waypointId)
        {
            case POINT_WRYNN_STAGING:
            case POINT_WRYNN_SEWER_ENTRANCE:
            case POINT_WRYNN_KHANOK_CORPSE:
            case POINT_WRYNN_APOTHECARIUM_TRASH:
            case POINT_WRYNN_PUTRESS_PLATFORM:
            case POINT_WRYNN_PUTRESS:
            case POINT_WRYNN_PUTRESS_DEFEATED:
            case POINT_WRYNN_THRONE_ROOM_DOOR:
            case POINT_WRYNN_THRONE:
                Hold();
                break;
            case POINT_WRYNN_SEWER_HOLD:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->GetMotionMaster()->Clear();
                    jaina->SetImmuneToNPC(false);
                    jaina->SetReactState(REACT_AGGRESSIVE);
                }
                Hold();
                break;
            case POINT_WRYNN_APOTHECARIUM_APPROACH:
                Hold();
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->GetMotionMaster()->Clear();
                    jaina->GetMotionMaster()->MovePoint(0, 1594.92f, 422.44f, -46.38f);
                }
                break;
            case POINT_WRYNN_PUTRESS_TAUNT:
                if (Creature* putress = me->FindNearestCreature(NPC_PUTRESS, 300.0f))
                    putress->AI()->Talk(SAY_PUTRESS_2);
                sUndercityBattleMgr->PlaySoundForParticipants(_team, me, SOUND_PUTRESS_LAUGH);
                break;
            default:
                break;
        }
    }

    void RunStep() override
    {
        // one Perfidious Dreadlord every 350 ms
        if (_step >= 32 && _step <= 42)
        {
            if (_step == 32)
                Talk(SAY_WRYNN_APOTHECARIUM_4);
            if (Creature* dreadlord = me->SummonCreature(NPC_PERFIDIOUS_DREADLORD_A, WaveOrigin(SPAWN_ORIGIN_ALLIANCE_APOTHECARIUM_DREADLORDS), TEMPSUMMON_DEAD_DESPAWN))
            {
                dreadlord->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                dreadlord->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                dreadlord->GetMotionMaster()->MovePoint(0, me->GetPosition());
            }
            Next(350ms);
            return;
        }

        switch (_step)
        {
            case 0:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_COUNTDOWN, 1);
                Talk(SAY_WRYNN_PREP_1);
                Next(10s);
                break;
            case 1:
                Talk(SAY_WRYNN_PREP_2);
                Next(10s);
                break;
            case 2:
                Talk(SAY_WRYNN_PREP_3);
                Next(20s);
                break;
            case 3:
                Talk(SAY_WRYNN_PREP_4);
                Next(20s);
                break;
            case 4:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_COUNTDOWN, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_STARTS, 1);
                Talk(SAY_WRYNN_PREP_5);
                Next(10s);
                break;
            case 5:
                DoCastSelf(SPELL_WRYNN_BUFF);
                Next(3s);
                break;
            case 6:
                Talk(SAY_WRYNN_PREP_6);
                Next(1s);
                break;
            case 7:
                SetEscortPaused(false);
                Next(1500ms);
                break;
            case 8:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                    jaina->GetMotionMaster()->MoveFollow(me, 5.0f, PET_FOLLOW_ANGLE);
                _stepping = false;
                Next(0s);
                break;
            case 9:
                Talk(SAY_WRYNN_SEWERS_1);
                me->SummonCreature(NPC_TREACHEROUS_GUARDIAN_A, WaveOrigin(SPAWN_ORIGIN_ALLIANCE_SEWER_MOUTH), TEMPSUMMON_DEAD_DESPAWN);
                SummonPack(SPAWN_ORIGIN_ALLIANCE_SEWER_PACK_1, { NPC_PERFIDIOUS_DREADLORD_A, NPC_BLIGHT_DOCTOR_A, NPC_BLIGHT_DOCTOR_A, NPC_BLIGHT_DOCTOR_A, NPC_PLAGUED_FELBEAST_A, NPC_PLAGUED_FELBEAST_A, NPC_PLAGUED_FELBEAST_A, NPC_RAVISHING_BETRAYER_A, NPC_RAVISHING_BETRAYER_A, NPC_RAVISHING_BETRAYER_A, NPC_APOTHECARY_COLLABORATOR_A, NPC_APOTHECARY_COLLABORATOR_A, NPC_APOTHECARY_COLLABORATOR_A }, false);
                SummonPack(SPAWN_ORIGIN_ALLIANCE_SEWER_PACK_2, { NPC_APOTHECARY_COLLABORATOR_A, NPC_BLIGHT_DOCTOR_A, NPC_PLAGUED_FELBEAST_A, NPC_RAVISHING_BETRAYER_A }, false);
                SummonPack(SPAWN_ORIGIN_ALLIANCE_SEWER_PACK_3, { NPC_TREACHEROUS_GUARDIAN_A, NPC_APOTHECARY_CHEMIST_A, NPC_APOTHECARY_CHEMIST_A, NPC_BLIGHT_DOCTOR_A, NPC_BLIGHT_DOCTOR_A }, false);
                Next(9500ms);
                break;
            case 10:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                    jaina->AI()->Talk(SAY_JAINA_SEWERS);
                Next(2s);
                break;
            case 11:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                    jaina->CastSpell(jaina, SPELL_JAINA_BUFF);
                Next(1s);
                break;
            case 12:
                SetEscortPaused(false);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_STARTS, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_SEWERS_FIGHT, 1);
                Next(1s);
                break;
            case 13:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->GetMotionMaster()->MoveFollow(me, 5.0f, PET_FOLLOW_ANGLE);
                    jaina->SetReactState(REACT_AGGRESSIVE);
                    jaina->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_ACTIVE);
                }
                _stepping = false;
                Next(0s);
                break;
            case 14:
                Talk(SAY_WRYNN_SEWERS_2);
                Next(3500ms);
                break;
            case 15:
            case 17:
            case 18:
                SummonPack(_step == 15 || _step == 18 ? SPAWN_ORIGIN_ALLIANCE_SEWER_HOLD_LEFT : SPAWN_ORIGIN_ALLIANCE_SEWER_HOLD_RIGHT, { NPC_BLIGHT_DOCTOR_A, NPC_APOTHECARY_CHEMIST_A, NPC_RAVISHING_BETRAYER_A }, true);
                Next(15s);
                break;
            case 16:
            case 19:
                SummonPack(SPAWN_ORIGIN_ALLIANCE_SEWER_HOLD_GUARDIANS, { NPC_TREACHEROUS_GUARDIAN_A }, true);
                Next(15s);
                break;
            case 20:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_ALLIANCE_BLIGHT_WORM);
                if (Creature* worm = me->FindNearestCreature(NPC_BLIGHT_WORM, 100.0f))
                {
                    worm->SetImmuneToAll(false);
                    EngageWith(worm, me);
                    AddThreat(worm, 100.0f);
                    if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                        AddThreat(worm, 100.0f, jaina);
                }
                _stepping = false;
                Next(0s);
                break;
            case 21:
                Talk(SAY_WRYNN_SEWERS_3);
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_ALLIANCE_SEWER_REINFORCEMENTS);
                Next(10s);
                break;
            case 22:
                Talk(SAY_WRYNN_SEWERS_4);
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->GetMotionMaster()->Clear();
                    jaina->GetMotionMaster()->MoveFollow(me, 1.0f, 0.0f);
                }
                Next(5s);
                break;
            case 23:
            case 25:
            case 43:
            case 45:
                SetEscortPaused(false);
                _stepping = false;
                Next(0s);
                break;
            case 24:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                    jaina->AI()->Talk(SAY_JAINA_APOTHECARIUM);
                Next(10s);
                break;
            case 26:
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                Next(1s);
                break;
            case 27:
                Talk(SAY_WRYNN_APOTHECARIUM_1);
                Next(12s);
                break;
            case 28:
                Talk(SAY_WRYNN_APOTHECARIUM_2);
                Next(10s);
                break;
            case 29:
                Talk(SAY_WRYNN_APOTHECARIUM_3);
                Next(1500ms);
                break;
            case 30:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_ALLIANCE_APOTHECARIUM_FIGHT, 1);
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_ALLIANCE_APOTHECARIUM);
                if (Creature* putress = me->FindNearestCreature(NPC_PUTRESS, 300.0f))
                {
                    putress->CastSpell(putress, SPELL_PUTRESS_CASTING_STATE);
                    putress->AI()->Talk(SAY_PUTRESS_1);
                    std::list<Creature*> generators;
                    putress->GetCreatureListWithEntryInGrid(generators, NPC_APOTHECARY_GENERATOR, 30.0f);
                    for (Creature* generator : generators)
                        generator->CastSpell(putress, SPELL_BLIGHT_EMPOWERMENT);
                }
                sUndercityBattleMgr->PlaySoundForParticipants(_team, me, SOUND_PUTRESS_LAUGH);
                for (uint8 i = 0; i < 12; ++i)
                {
                    Position pos = WaveOrigin(SPAWN_ORIGIN_ALLIANCE_APOTHECARIUM_GUARDIANS);
                    pos.m_positionX += frand(0.0f, 13.0f);
                    pos.m_positionY += frand(0.0f, 13.0f);
                    me->SummonCreature(NPC_TREACHEROUS_GUARDIAN_A, pos, TEMPSUMMON_DEAD_DESPAWN);
                }
                Next(3s);
                break;
            case 31:
                me->SetStandState(UNIT_STAND_STATE_STAND);
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                    jaina->GetMotionMaster()->MoveFollow(me, 1.0f, 0.0f);
                SetEscortPaused(false);
                _stepping = false;
                Next(0s);
                break;
            case 44:
                Talk(SAY_WRYNN_APOTHECARIUM_5);
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->GetMotionMaster()->Clear();
                    jaina->GetMotionMaster()->MovePoint(0, 1423.19f, 412.73f, -84.6f);
                }
                Next(5s);
                break;
            case 46:
                Talk(SAY_WRYNN_APOTHECARIUM_6);
                Next(4s);
                break;
            case 47:
            case 48:
            case 49:
            case 50:
            case 51:
                if (Creature* putress = me->FindNearestCreature(NPC_PUTRESS, 300.0f))
                    putress->AI()->Talk(uint8(SAY_PUTRESS_3 + (_step - 47)));
                if (_step != 48 && _step != 50)
                {
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        Position pos = WaveOrigin(SPAWN_ORIGIN_ALLIANCE_EXPERIMENTS_1 + i);
                        pos.m_positionX += frand(0.0f, 5.0f);
                        pos.m_positionY += frand(0.0f, 5.0f);
                        if (Creature* experiment = me->SummonCreature(NPC_FAILED_EXPERIMENT, pos, TEMPSUMMON_DEAD_DESPAWN))
                            experiment->GetMotionMaster()->MovePoint(0, me->GetPosition());
                    }
                }
                Next(_step == 50 ? 10000ms : 7500ms);
                break;
            case 52:
                if (Creature* putress = me->FindNearestCreature(NPC_PUTRESS, 300.0f))
                {
                    putress->AI()->Talk(SAY_PUTRESS_8);
                    putress->CastSpell(putress, SPELL_BLIGHT_OVERLOAD);
                }
                Next(500ms);
                break;
            case 53:
                if (Creature* putress = me->FindNearestCreature(NPC_PUTRESS, 300.0f))
                {
                    putress->SetImmuneToAll(false);
                    putress->RemoveAurasDueToSpell(SPELL_PUTRESS_CASTING_STATE);
                    EngageWith(putress, me);
                    AddThreat(putress, 100.0f);
                }
                _stepping = false;
                Next(0s);
                break;
            case 54:
                Talk(SAY_WRYNN_APOTHECARIUM_7);
                Next(4s);
                break;
            case 55:
                Talk(SAY_WRYNN_APOTHECARIUM_8);
                Next(4s);
                break;
            case 56:
                Talk(SAY_WRYNN_APOTHECARIUM_9);
                SetEscortPaused(false);
                _stepping = false;
                Next(0s);
                break;
            case 57:
                Next(4s);
                break;
            case 58:
                Talk(SAY_WRYNN_APOTHECARIUM_10);
                Next(7500ms);
                break;
            case 59:
                Talk(SAY_WRYNN_APOTHECARIUM_11);
                Next(7500ms);
                break;
            case 60:
                Talk(SAY_WRYNN_APOTHECARIUM_12);
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_ALLIANCE_THRONE_HORDE);
                if (Creature* thrall = me->FindNearestCreature(NPC_THRALL, 150.0f))
                    thrall->CastSpell(thrall, SPELL_THRALL_BUFF);
                Next(10s);
                break;
            case 61:
                if (Creature* thrall = me->FindNearestCreature(NPC_THRALL, 150.0f))
                    thrall->AI()->Talk(SAY_THRALL_ALLIANCE_THRONE_1);
                sUndercityBattleMgr->PlaySoundForParticipants(_team, me, SOUND_THRALL_THRONE_1);
                Next(3s);
                break;
            case 62:
                if (Creature* thrall = me->FindNearestCreature(NPC_THRALL, 150.0f))
                    thrall->AI()->Talk(SAY_THRALL_ALLIANCE_THRONE_2);
                sUndercityBattleMgr->PlaySoundForParticipants(_team, me, SOUND_THRALL_THRONE_2);
                Next(5s);
                break;
            case 63:
                Talk(SAY_WRYNN_THRONE_1);
                Next(3s);
                break;
            case 64:
                Talk(SAY_WRYNN_THRONE_2);
                Next(1500ms);
                break;
            case 65:
                SetEscortPaused(false);
                Next(250ms);
                break;
            case 66:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                    jaina->AI()->Talk(SAY_JAINA_THRONE_1);
                me->SetImmuneToNPC(true);
                _stepping = false;
                Next(0s);
                break;
            case 67:
                Talk(SAY_WRYNN_THRONE_3);
                Next(10s);
                break;
            case 68:
                Talk(SAY_WRYNN_THRONE_4);
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->GetMotionMaster()->MovePoint(0, 1311.93f, 394.38f, -63.25f);
                    jaina->SetImmuneToAll(true);
                }
                SetEscortPaused(false);
                _stepping = false;
                Next(0s);
                break;
            case 69:
                Talk(SAY_WRYNN_THRONE_5);
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_ALLIANCE_THRONE_ALLIANCE);
                Next(15s);
                break;
            case 70:
                Talk(SAY_WRYNN_THRONE_6);
                Next(15s);
                break;
            case 71:
                Talk(SAY_WRYNN_THRONE_7);
                Next(16500ms);
                break;
            case 72:
                Talk(SAY_WRYNN_THRONE_8);
                Next(6s);
                break;
            case 73:
                Talk(SAY_WRYNN_THRONE_9);
                me->SetImmuneToAll(false);
                for (uint32 entry : { NPC_THRALL, NPC_SYLVANAS })
                {
                    if (Creature* enemy = me->FindNearestCreature(entry, 150.0f))
                    {
                        enemy->SetReactState(REACT_AGGRESSIVE);
                        enemy->SetImmuneToNPC(false);
                        enemy->SetImmuneToPC(true);
                        EngageWith(enemy, me);
                        AddThreat(enemy, 100.0f);
                    }
                }
                {
                    std::list<Creature*> elites;
                    std::list<Creature*> guards;
                    me->GetCreatureListWithEntryInGrid(elites, NPC_STORMWIND_ELITE, 100.0f);
                    me->GetCreatureListWithEntryInGrid(guards, NPC_WARSONG_BATTLEGUARD_THRONE, 100.0f);
                    for (std::list<Creature*>::iterator elite = elites.begin(), guard = guards.begin(); elite != elites.end() && guard != guards.end(); ++elite, ++guard)
                    {
                        (*elite)->SetReactState(REACT_AGGRESSIVE);
                        (*guard)->SetReactState(REACT_AGGRESSIVE);
                        (*elite)->SetImmuneToAll(false);
                        (*guard)->SetImmuneToAll(false);
                        (*guard)->SetImmuneToPC(true);
                        EngageWith(*elite, *guard);
                        AddThreat(*elite, 100.0f, *guard);
                    }
                }
                Next(6s);
                break;
            case 74:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->GetMotionMaster()->MovePoint(0, 1300.75f, 347.39f, -65.02f);
                    jaina->AI()->Talk(SAY_JAINA_THRONE_2);
                }
                Next(8s);
                break;
            case 75:
                if (Creature* jaina = ObjectAccessor::GetCreature(*me, _jaina))
                {
                    jaina->CastSpell(jaina, SPELL_DEEP_FREEZE);
                    jaina->AI()->Talk(SAY_JAINA_THRONE_3);
                }
                Next(5s);
                break;
            case 76:
                sUndercityBattleMgr->CompleteQuest(_team, me);
                for (ObjectGuid const& guid : sUndercityBattleMgr->GetParticipants(_team))
                    if (Player* player = ObjectAccessor::GetPlayer(*me, guid))
                        if (player->IsAlive() && player->GetQuestStatus(QUEST_BATTLE_ALLIANCE) == QUEST_STATUS_COMPLETE && player->IsWithinDist(me, 100.0f))
                            player->TeleportTo(MAP_EASTERN_KINGDOMS, -8445.213867f, 337.384277f, 121.746056f, 5.401534f);
                _stepping = false;
                sUndercityBattleMgr->End(_team);
                break;
            default:
                break;
        }
    }

private:
    void SummonPack(uint32 originSpawnId, std::initializer_list<uint32> entries, bool charge)
    {
        for (uint8 i = 0; i < 12; ++i)
        {
            Position pos = WaveOrigin(originSpawnId);
            pos.m_positionX += frand(-5.0f, 5.0f);
            pos.m_positionY += frand(-5.0f, 5.0f);
            if (Creature* summon = me->SummonCreature(*(entries.begin() + urand(0, uint32(entries.size()) - 1)), pos, TEMPSUMMON_DEAD_DESPAWN))
                if (charge)
                    summon->GetMotionMaster()->MovePoint(0, me->GetPosition());
        }
    }

    ObjectGuid _jaina;
};

struct npc_eg_undercity_jaina : public ScriptedAI
{
    npc_eg_undercity_jaina(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_THRALL_BUFF, true);
        me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SYLVANAS_BUFF, true);
        _scheduler.CancelAll().Schedule(1s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                DoCast(target, SPELL_FIREBALL);
            context.Repeat(3s);
        }).Schedule(8s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BLIZZARD);
            context.Repeat(15s);
        }).Schedule(30s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_SUMMON_WATER_ELEMENTALS);
            context.Repeat(90s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this] { DoMeleeAttackIfReady(); });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_eg_undercity_blight_worm : public ScriptedAI
{
    npc_eg_undercity_blight_worm(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        _scheduler.SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void Reset() override
    {
        _scheduler.CancelAll().Schedule(750ms, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BLIGHT_BREATH);
            context.Repeat(15s);
        }).Schedule(2s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                DoCast(target, SPELL_INGEST);
            context.Repeat(20s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this] { DoMeleeAttackIfReady(); });
    }

private:
    TaskScheduler _scheduler;
};

// 61123 - Ingest
class spell_eg_undercity_blight_worm_ingest : public SpellScript
{
    PrepareSpellScript(spell_eg_undercity_blight_worm_ingest);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_INGEST_TRIGGER });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetCaster(), SPELL_INGEST_TRIGGER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_eg_undercity_blight_worm_ingest::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

struct npc_eg_undercity_putress : public ScriptedAI
{
    npc_eg_undercity_putress(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void Reset() override
    {
        _scheduler.CancelAll().Schedule(5s, 10s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f))
                DoCast(target, SPELL_THROW_BLIGHT_BARREL);
            context.Repeat(8s, 12s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this] { DoMeleeAttackIfReady(); });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_eg_undercity_thrall : public npc_eg_undercity_leaderAI
{
    npc_eg_undercity_thrall(Creature* creature) : npc_eg_undercity_leaderAI(creature, TEAM_HORDE) { }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SYLVANAS_BUFF, true);
        _scheduler.CancelAll().Schedule(2s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_THRALL_BUFF);
            context.Repeat(10s);
        }).Schedule(2s, [this](TaskContext context)
        {
            if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                if (!sylvanas->IsEngaged() && !sylvanas->IsImmuneToNPC())
                    sylvanas->AI()->AttackStart(me->GetVictim());
            context.Repeat(2s);
        }).Schedule(3s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_CHAIN_LIGHTNING);
            context.Repeat(3s);
        }).Schedule(5s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_LAVA_BURST);
            context.Repeat(5s);
        }).Schedule(8s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_THUNDER);
            context.Repeat(8s);
        });
    }

    void DoAction(int32 action) override
    {
        if (action != ACTION_START_BATTLE || !IsLeader())
            return;

        if (Creature* sylvanas = me->FindNearestCreature(NPC_SYLVANAS, 30.0f))
        {
            _sylvanas = sylvanas->GetGUID();
            sylvanas->setActive(true);
        }

        me->setActive(true);
        sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_GATE_BLIGHT);
        LoadPath(PATH_THRALL);
        Start(true);
        SetDespawnAtEnd(false);
    }

    void JustSummoned(Creature* summon) override
    {
        npc_eg_undercity_leaderAI::JustSummoned(summon);
        if (!IsLeader())
            return;

        switch (summon->GetEntry())
        {
            case NPC_TIDAL_WAVE:
                summon->CastSpell(summon, SPELL_TIDAL_WAVE, true);
                summon->GetMotionMaster()->MovePoint(0, 1735.79f, 238.951f, 62.796f);
                summon->DespawnOrUnsummon(20s);
                break;
            case NPC_WARSONG_BATTLEGUARD:
                summon->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SYLVANAS_BUFF, true);
                summon->SetEmoteState(EMOTE_STATE_READY2H);
                break;
            case NPC_TREACHEROUS_GUARDIAN_H:
            case NPC_BLIGHT_DOCTOR_H:
            case NPC_APOTHECARY_CHEMIST_H:
                summon->ApplySpellImmune(0, IMMUNITY_ID, SPELL_THRALL_BUFF, true);
                summon->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SYLVANAS_BUFF, true);
                [[fallthrough]];
            case NPC_LEGION_INVADER:
            case NPC_LEGION_DREADWHISPERER:
            case NPC_LEGION_OVERLORD:
            case NPC_FELGUARD_MARAUDER_H:
            case NPC_PERFIDIOUS_DREADLORD_H:
            case NPC_RAVISHING_BETRAYER_H:
            case NPC_PLAGUED_FELBEAST_H:
                summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                _trash.push_back(summon->GetGUID());
                EngageWith(summon, me);
                AddThreat(summon, 100.0f);
                break;
            case NPC_DOOMGUARD_PILLAGER:
                _trash.push_back(summon->GetGUID());
                break;
            default:
                break;
        }
    }

    void SetData(uint32 id, uint32 value) override
    {
        if (id != DATA_ACTOR_DIED || !IsLeader())
            return;

        switch (value)
        {
            case NPC_BLIGHT_ABERRATION:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_COURTYARD_FIGHT, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_COURTYARD_DONE, 1);
                StartStepping();
                break;
            case NPC_KHANOK:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_INNER_SANCTUM_DONE, 1);
                DespawnTrash();
                SylvanasFollow();
                SetEscortPaused(false);
                break;
            case NPC_VARIMATHRAS:
            {
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_ROYAL_QUARTER_DONE, 1);
                DespawnTrash();
                std::list<Creature*> portals;
                me->GetCreatureListWithEntryInGrid(portals, NPC_VARIMATHRAS_PORTAL, 200.0f);
                for (Creature* portal : portals)
                    portal->DespawnOrUnsummon();
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_DISTANT_VOICE);
                if (Creature* voice = me->FindNearestCreature(NPC_DISTANT_VOICE, 100.0f))
                {
                    voice->AI()->Talk(SAY_SUMMONED);
                    voice->DespawnOrUnsummon(20s);
                }
                SetEscortPaused(false);
                break;
            }
            default:
                break;
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        if (!IsLeader())
            return;

        switch (waypointId)
        {
            case POINT_THRALL_STAGING:
            case POINT_THRALL_GATE:
            case POINT_THRALL_COURTYARD_ENTRANCE:
            case POINT_THRALL_COURTYARD:
            case POINT_THRALL_COURTYARD_CLEARED:
            case POINT_THRALL_ELEVATOR:
            case POINT_THRALL_SANCTUM_TOP:
            case POINT_THRALL_SANCTUM_LEFT:
            case POINT_THRALL_SANCTUM_RIGHT:
            case POINT_THRALL_KHANOK:
            case POINT_THRALL_KHANOK_DEFEATED:
            case POINT_THRALL_PASSAGE_APPROACH:
            case POINT_THRALL_BLOCKED_PASSAGE:
            case POINT_THRALL_VARIMATHRAS:
            case POINT_THRALL_VARIMATHRAS_DEFEATED:
            case POINT_THRALL_THRONE_STEPS:
            case POINT_THRALL_ALLIANCE_ARRIVES:
            case POINT_THRALL_THRONE:
                Hold();
                break;
            case POINT_THRALL_SANCTUM_ENTRANCE:
                Talk(SAY_THRALL_SANCTUM_1);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_INNER_SANCTUM_FIGHT, 1);
                break;
            case POINT_THRALL_THRONE_APPROACH:
                Talk(SAY_THRALL_THRONE_8);
                break;
            default:
                break;
        }
    }

    void RunStep() override
    {
        if (_step >= 28 && _step <= 44)
        {
            if (_step == 28)
            {
                DoCastSelf(SPELL_HEROIC_VANGUARD, true);
                std::list<Creature*> defenders;
                me->GetCreatureListWithOptionsInGrid(defenders, 150.0f, CourtyardDefenders);
                for (Creature* defender : defenders)
                {
                    defender->SetImmuneToAll(false);
                    defender->ApplySpellImmune(0, IMMUNITY_ID, SPELL_THRALL_BUFF, true);
                    defender->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SYLVANAS_BUFF, true);
                    _trash.push_back(defender->GetGUID());
                    EngageWith(defender, me);
                }
            }
            SummonPack(9, { { NPC_TREACHEROUS_GUARDIAN_H, SPAWN_ORIGIN_HORDE_COURTYARD_GUARDIANS }, { NPC_BLIGHT_DOCTOR_H, SPAWN_ORIGIN_HORDE_COURTYARD_DOCTORS }, { NPC_APOTHECARY_CHEMIST_H, SPAWN_ORIGIN_HORDE_COURTYARD_CHEMISTS } }, 5.0f);
            Next(_step == 44 ? 2s : 10s);
            return;
        }

        if (_step >= 76 && _step <= 93)
        {
            SummonSanctumWave((_step - 76) % 3);
            Next(8s);
            return;
        }

        if (_step >= 99 && _step <= 107)
        {
            if (Creature* guard = me->SummonCreature(NPC_WARSONG_BATTLEGUARD, WaveOrigin(SPAWN_ORIGIN_HORDE_BATTLEGUARD_CHARGE), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240s))
            {
                guard->AI()->Talk(SAY_BATTLEGUARD_FOR_THE_HORDE);
                guard->GetMotionMaster()->MovePath(PATH_BATTLEGUARD_CHARGE, false);
            }
            Next(_step == 107 ? 3000ms : 350ms);
            return;
        }

        if (_step >= 119 && _step <= 136)
        {
            // every third step a portal is consumed and releases an Overlord
            uint32 const cycle = (_step - 119) % 3;
            if (cycle == 0)
                SummonPack(3, { { NPC_LEGION_INVADER, SPAWN_ORIGIN_HORDE_THRONE_LEFT }, { NPC_LEGION_DREADWHISPERER, SPAWN_ORIGIN_HORDE_THRONE_RIGHT } }, 5.0f);
            else if (cycle == 1)
                SummonPack(3, { { NPC_LEGION_INVADER, SPAWN_ORIGIN_HORDE_THRONE_RIGHT }, { NPC_LEGION_DREADWHISPERER, SPAWN_ORIGIN_HORDE_THRONE_LEFT } }, 5.0f);
            else
            {
                if (Creature* portal = me->FindNearestCreature(NPC_VARIMATHRAS_PORTAL, 300.0f))
                {
                    portal->DespawnOrUnsummon();
                    SummonPack(1, { { NPC_LEGION_OVERLORD, SPAWN_ORIGIN_HORDE_THRONE_RIGHT } }, 5.0f);
                }
                if (_step < 136)
                    if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
                        varimathras->AI()->Talk(uint8(SAY_VARIMATHRAS_THRONE_2 + (_step - 121) / 3));
            }

            Next(_step == 136 ? 5s : 10s);
            return;
        }

        switch (_step)
        {
            case 0:
                Next(3s);
                break;
            case 1:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_COUNTDOWN, 1);
                Talk(SAY_THRALL_PREP_1);
                Next(6s);
                break;
            case 2:
                Talk(SAY_THRALL_PREP_2);
                Next(14s);
                break;
            case 3:
            case 4:
                if (Creature* guard = me->FindNearestCreature(NPC_WARSONG_BATTLEGUARD, 100.0f))
                    guard->AI()->Talk(_step == 3 ? SAY_BATTLEGUARD_BURN : SAY_BATTLEGUARD_PUTRESS);
                Next(_step == 3 ? 19s : 25s);
                break;
            case 5:
                Talk(SAY_THRALL_PREP_3);
                Next(14s);
                break;
            case 6:
                Talk(SAY_THRALL_PREP_4);
                Next(14s);
                break;
            case 7:
                Talk(SAY_THRALL_PREP_5);
                Next(10s);
                break;
            case 8:
                Talk(SAY_THRALL_PREP_6);
                Next(15s);
                break;
            case 9:
                Talk(SAY_THRALL_PREP_7);
                Next(6s);
                break;
            case 10:
                DoCastSelf(SPELL_THRALL_BUFF);
                Next(10s);
                break;
            case 11:
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_COUNTDOWN, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_START, 1);
                Talk(SAY_THRALL_PREP_8);
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    sylvanas->GetMotionMaster()->MoveFollow(me, 1.0f, float(M_PI) * 0.1f);
                SetEscortPaused(false);
                _stepping = false;
                Next(0s);
                break;
            case 12:
                me->Dismount();
                Next(1s);
                break;
            case 13:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    sylvanas->Dismount();
                Next(3s);
                break;
            case 14:
                Talk(SAY_THRALL_COURTYARD_1);
                Next(4s);
                break;
            case 15:
                DoCastSelf(SPELL_CALL_OF_AIR);
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_VORTICES);
                {
                    std::list<Creature*> vortices;
                    me->GetCreatureListWithEntryInGrid(vortices, NPC_GREAT_WIND_VORTEX, 150.0f);
                    uint32 pathId = PATH_VORTEX_LEFT;
                    for (Creature* vortex : vortices)
                    {
                        vortex->GetMotionMaster()->MovePath(pathId, false);
                        vortex->DespawnOrUnsummon(30s);
                        pathId = PATH_VORTEX_RIGHT;
                    }
                }
                Next(5s);
                break;
            case 16:
                {
                    std::list<Creature*> triggers;
                    me->GetCreatureListWithEntryInGrid(triggers, NPC_PLAGUE_TRIGGER, 50.0f);
                    for (Creature* trigger : triggers)
                        trigger->DespawnOrUnsummon();
                }
                SetEscortPaused(false);
                Next(3s);
                break;
            case 17:
                _stepping = false;
                Next(0s);
                break;
            case 18:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_COURTYARD_DEFENDERS);
                Talk(SAY_THRALL_COURTYARD_2);
                Next(6s);
                break;
            case 19:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_VARIMATHRAS_COURTYARD);
                Next(3s);
                break;
            case 20:
            case 21:
            case 22:
                if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
                    varimathras->AI()->Talk(uint8(SAY_VARIMATHRAS_INTRO_1 + (_step - 20)));
                Next(_step == 20 ? 5s : _step == 21 ? 9s : 7s);
                break;
            case 23:
                Next(1s);
                break;
            case 24:
            case 74:
                if (Creature* portal = me->FindNearestCreature(NPC_VARIMATHRAS_PORTAL, 300.0f))
                    portal->CastSpell(portal, SPELL_VARIMATHRAS_PORTAL);
                Next(_step == 24 ? 12s : 4s);
                break;
            case 25:
                VarimathrasLeaves(1804.559f, 235.504f, 62.753f, 6s);
                Next(1s);
                break;
            case 26:
                Talk(SAY_THRALL_COURTYARD_3);
                DoCastSelf(SPELL_TIDAL_WAVE_SUMMON);
                {
                    std::list<Creature*> triggers;
                    me->GetCreatureListWithOptionsInGrid(triggers, 250.0f, GateTriggers);
                    for (Creature* trigger : triggers)
                        trigger->DespawnOrUnsummon();
                }
                Next(5s);
                break;
            case 27:
                Talk(SAY_THRALL_COURTYARD_4);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_START, 0);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_COURTYARD_FIGHT, 1);
                Resume();
                break;
            case 45:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_BLIGHT_ABERRATION);
                if (Creature* aberration = me->FindNearestCreature(NPC_BLIGHT_ABERRATION, 150.0f))
                {
                    aberration->SetHomePosition(me->GetPosition());
                    aberration->AI()->Talk(SAY_SUMMONED);
                    EngageWith(aberration, me);
                    AddThreat(aberration, 100.0f);
                }
                _stepping = false;
                Next(0s);
                break;
            case 46:
                DespawnTrash();
                {
                    std::list<Creature*> slingers;
                    me->GetCreatureListWithEntryInGrid(slingers, NPC_BLIGHT_SLINGER, 200.0f);
                    for (Creature* slinger : slingers)
                        slinger->DespawnOrUnsummon();
                }
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_COURTYARD_SECURED);
                Resume();
                break;
            case 47:
                Talk(SAY_THRALL_COURTYARD_5);
                Next(5s);
                break;
            case 48:
            case 140:
                SetEscortPaused(false);
                _stepping = false;
                Next(0s);
                break;
            case 49:
                Talk(SAY_THRALL_ELEVATOR_1);
                Next(10s);
                break;
            case 50:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    sylvanas->AI()->Talk(SAY_SYLVANAS_ELEVATOR);
                Next(3s);
                break;
            case 51:
                Talk(SAY_THRALL_ELEVATOR_2);
                DoCastSelf(SPELL_CALL_OF_AIR);
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_ELEVATOR_SHAFT);
                {
                    // kept until the run ends and kept active so late arrivals can still descend the shaft
                    std::list<Creature*> dummies;
                    me->GetCreatureListWithEntryInGrid(dummies, NPC_CAVE_IN_DUMMY, 150.0f);
                    for (Creature* dummy : dummies)
                        dummy->setActive(true);
                }
                Next(16s);
                break;
            case 52:
                Talk(SAY_THRALL_ELEVATOR_3);
                Next(4s);
                break;
            case 53:
                me->GetMotionMaster()->MoveJump(1542.196f, 241.254f, -41.36f, 3.276f, 40.0f, 40.0f);
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    sylvanas->GetMotionMaster()->MoveJump(1543.511f, 236.552f, -41.36f, 3.05f, 40.0f, 40.0f);
                Next(4s);
                break;
            case 54:
            case 62:
            case 66:
            case 69:
            case 111:
                Resume();
                break;
            case 55:
            case 56:
            case 58:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    sylvanas->AI()->Talk(_step == 55 ? SAY_SYLVANAS_SANCTUM_1 : _step == 56 ? SAY_SYLVANAS_SANCTUM_2 : SAY_SYLVANAS_SANCTUM_3);
                Next(_step == 56 ? 8s : 5s);
                break;
            case 57:
                Talk(SAY_THRALL_SANCTUM_2);
                Next(5s);
                break;
            case 59:
            case 60:
            case 61:
                SummonPack(3, { { NPC_TREACHEROUS_GUARDIAN_H, SPAWN_ORIGIN_HORDE_SANCTUM_TOP }, { NPC_BLIGHT_DOCTOR_H, SPAWN_ORIGIN_HORDE_SANCTUM_TOP }, { NPC_APOTHECARY_CHEMIST_H, SPAWN_ORIGIN_HORDE_SANCTUM_TOP } }, 2.0f);
                Next(_step == 61 ? 10s : 5s);
                break;
            case 63:
                Next(3s);
                break;
            case 64:
            case 65:
                SummonPack(3, { { NPC_TREACHEROUS_GUARDIAN_H, SPAWN_ORIGIN_HORDE_SANCTUM_LEFT }, { NPC_FELGUARD_MARAUDER_H, SPAWN_ORIGIN_HORDE_SANCTUM_LEFT } }, 5.0f);
                Next(_step == 64 ? 5s : 10s);
                break;
            case 67:
            case 68:
                SummonPack(3, { { NPC_FELGUARD_MARAUDER_H, SPAWN_ORIGIN_HORDE_SANCTUM_RIGHT }, { NPC_PERFIDIOUS_DREADLORD_H, SPAWN_ORIGIN_HORDE_SANCTUM_RIGHT } }, 5.0f);
                Next(_step == 67 ? 6s : 10s);
                break;
            case 70:
                Next(10s);
                break;
            case 71:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    sylvanas->AI()->Talk(SAY_SYLVANAS_SANCTUM_4);
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_VARIMATHRAS_SANCTUM);
                Next(5s);
                break;
            case 72:
                if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
                    varimathras->AI()->Talk(SAY_VARIMATHRAS_SANCTUM_1);
                Next(5s);
                break;
            case 73:
                if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
                    varimathras->AI()->Talk(SAY_VARIMATHRAS_SANCTUM_2);
                Next(2s);
                break;
            case 75:
                VarimathrasLeaves(1596.642f, 429.811f, -46.3429f, 3s);
                Next(2s);
                break;
            case 94:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_KHANOK);
                if (Creature* khanok = me->FindNearestCreature(NPC_KHANOK, 150.0f))
                {
                    khanok->SetImmuneToAll(false);
                    khanok->SetHomePosition(me->GetPosition());
                    khanok->AI()->Talk(SAY_SUMMONED);
                    EngageWith(khanok, me);
                    AddThreat(khanok, 100.0f);
                }
                Next(10s);
                break;
            case 95:
            case 96:
                SummonSanctumWave(_step - 95);
                Next(5s);
                break;
            case 97:
                SummonSanctumWave(2);
                _stepping = false;
                Next(0s);
                break;
            case 98:
                Talk(SAY_THRALL_SANCTUM_3);
                Next(7s);
                break;
            case 108:
                Talk(SAY_THRALL_SANCTUM_4);
                Resume();
                break;
            case 109:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    sylvanas->AI()->Talk(SAY_SYLVANAS_SANCTUM_5);
                Next(8s);
                break;
            case 110:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_VARIMATHRAS_THRONE);
                if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
                {
                    varimathras->CastSpell(varimathras, SPELL_AURA_OF_VARIMATHRAS);
                    varimathras->CastSpell(varimathras, SPELL_OPENING_LEGION_PORTALS);
                    varimathras->AI()->Talk(SAY_VARIMATHRAS_CLOSE_DOOR);
                }
                Next(5s);
                break;
            case 112:
                Next(3s);
                break;
            case 113:
                Talk(SAY_THRALL_SANCTUM_5);
                Next(12s);
                break;
            case 114:
                Talk(SAY_THRALL_SANCTUM_6);
                DoCastSelf(SPELL_CALL_OF_EARTH);
                Next(6s);
                break;
            case 115:
                {
                    std::list<GameObject*> passages;
                    me->GetGameObjectListWithEntryInGrid(passages, GO_BLOCKED_PASSAGE, 80.0f);
                    for (GameObject* passage : passages)
                        passage->UseDoorOrButton();
                }
                Next(5s);
                break;
            case 116:
                Talk(SAY_THRALL_SANCTUM_7);
                sUndercityBattleMgr->SetWorldState(_team, WORLD_STATE_HORDE_ROYAL_QUARTER_FIGHT, 1);
                Resume();
                break;
            case 117:
                Talk(SAY_THRALL_THRONE_1);
                Next(5s);
                break;
            case 118:
                if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
                {
                    varimathras->AI()->Talk(SAY_VARIMATHRAS_THRONE_1);
                    varimathras->CastSpell(varimathras, SPELL_OPENING_LEGION_PORTALS);
                }
                Next(3s);
                break;
            case 137:
                if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
                {
                    varimathras->SetImmuneToAll(false);
                    varimathras->RemoveAurasDueToSpell(SPELL_AURA_OF_VARIMATHRAS);
                    varimathras->RemoveAurasDueToSpell(SPELL_OPENING_LEGION_PORTALS);
                    varimathras->AI()->Talk(SAY_VARIMATHRAS_ATTACK);
                    varimathras->SetHomePosition(me->GetPosition());
                    EngageWith(varimathras, me);
                    AddThreat(varimathras, 100.0f);
                    AttackStart(varimathras);
                }
                _stepping = false;
                Next(0s);
                break;
            case 138:
                Talk(SAY_THRALL_THRONE_2);
                Next(5s);
                break;
            case 139:
                Talk(SAY_THRALL_THRONE_3);
                Next(2s);
                break;
            case 141:
                Next(8s);
                break;
            case 142:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                    me->SetFacingToObject(sylvanas);
                Talk(SAY_THRALL_THRONE_4);
                Next(3s);
                break;
            case 143:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                {
                    sylvanas->GetMotionMaster()->Clear();
                    sylvanas->GetMotionMaster()->MoveJump(1289.48f, 314.33f, -57.32f, 1.03f, 20.0f, 20.0f);
                }
                Next(10s);
                break;
            case 144:
                if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
                {
                    sylvanas->AI()->Talk(SAY_SYLVANAS_THRONE);
                    me->SetFacingToObject(sylvanas);
                    sylvanas->SetFacingToObject(me);
                }
                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                Next(3s);
                break;
            case 145:
                Talk(SAY_THRALL_THRONE_5);
                Resume();
                break;
            case 146:
                Talk(SAY_THRALL_THRONE_6);
                Next(3s);
                break;
            case 147:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_THRONE_ALLIANCE);
                if (Creature* wrynn = me->FindNearestCreature(NPC_WRYNN, 150.0f))
                {
                    // the template gives every Wrynn a gossip flag, this copy is an actor only
                    wrynn->ReplaceAllNpcFlags(UNIT_NPC_FLAG_NONE);
                    wrynn->GetMotionMaster()->MovePoint(0, 1302.543f, 359.472f, -67.295f);
                }
                Next(6s);
                break;
            case 148:
            case 149:
            case 150:
            case 151:
                if (Creature* wrynn = me->FindNearestCreature(NPC_WRYNN, 150.0f))
                    wrynn->AI()->Talk(uint8(SAY_WRYNN_THRONE_5 + (_step - 148)));
                Next(_step == 150 ? 16500ms : _step == 151 ? 6000ms : 15000ms);
                break;
            case 152:
                me->SetImmuneToAll(false);
                if (Creature* wrynn = me->FindNearestCreature(NPC_WRYNN, 150.0f))
                {
                    wrynn->SetImmuneToNPC(false);
                    wrynn->SetImmuneToPC(true);
                    wrynn->SetReactState(REACT_AGGRESSIVE);
                    EngageWith(wrynn, me);
                    AddThreat(wrynn, 100.0f);
                }
                {
                    std::list<Creature*> elites;
                    me->GetCreatureListWithEntryInGrid(elites, NPC_STORMWIND_ELITE, 150.0f);
                    for (Creature* elite : elites)
                    {
                        elite->SetImmuneToAll(false);
                        elite->SetReactState(REACT_AGGRESSIVE);
                        EngageWith(elite, me);
                    }
                }
                Next(6s);
                break;
            case 153:
                if (Creature* jaina = me->FindNearestCreature(NPC_JAINA, 150.0f))
                {
                    jaina->GetMotionMaster()->MovePoint(0, 1300.75f, 347.39f, -65.02f);
                    jaina->AI()->Talk(SAY_JAINA_THRONE_2);
                }
                Next(8s);
                break;
            case 154:
                if (Creature* jaina = me->FindNearestCreature(NPC_JAINA, 150.0f))
                {
                    jaina->CastSpell(jaina, SPELL_DEEP_FREEZE);
                    jaina->AI()->Talk(SAY_JAINA_THRONE_3);
                }
                Next(5s);
                break;
            case 155:
                me->GetMap()->SpawnGroupDespawn(SPAWN_GROUP_HORDE_THRONE_ALLIANCE, true);
                Next(8s);
                break;
            case 156:
                Talk(SAY_THRALL_THRONE_7);
                SetEscortPaused(false);
                _stepping = false;
                Next(0s);
                break;
            case 157:
                Talk(SAY_THRALL_THRONE_9);
                me->SetStandState(UNIT_STAND_STATE_SIT);
                Next(3s);
                break;
            case 158:
                sUndercityBattleMgr->SpawnGroup(_team, me->GetMap(), SPAWN_GROUP_HORDE_SAURFANG);
                if (Creature* saurfang = me->FindNearestCreature(NPC_SAURFANG, 150.0f))
                {
                    saurfang->SetWalk(true);
                    saurfang->GetMotionMaster()->MovePoint(0, 1300.862f, 353.670f, -66.187f);
                }
                Next(7s);
                break;
            case 159:
            case 160:
            case 161:
                if (Creature* saurfang = me->FindNearestCreature(NPC_SAURFANG, 150.0f))
                {
                    saurfang->AI()->Talk(uint8(SAY_SAURFANG_ARRIVAL_1 + (_step - 159)));
                    if (_step == 159)
                        saurfang->SetStandState(UNIT_STAND_STATE_SIT);
                }
                if (_step == 161)
                    sUndercityBattleMgr->CompleteQuest(_team, me);
                Next(_step == 161 ? 5s : 6s);
                break;
            case 162:
                Talk(SAY_THRALL_THRONE_10);
                Next(5s);
                break;
            case 163:
                Talk(SAY_THRALL_THRONE_11);
                me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                _stepping = false;
                sUndercityBattleMgr->End(_team, 120s);
                break;
            default:
                break;
        }
    }

private:
    void DespawnTrash()
    {
        for (ObjectGuid const& guid : _trash)
            if (Creature* trash = ObjectAccessor::GetCreature(*me, guid))
                trash->DespawnOrUnsummon();
        _trash.clear();
    }

    void SylvanasFollow()
    {
        if (Creature* sylvanas = ObjectAccessor::GetCreature(*me, _sylvanas))
        {
            sylvanas->GetMotionMaster()->Clear();
            sylvanas->SetImmuneToAll(false);
            sylvanas->SetReactState(REACT_AGGRESSIVE);
            sylvanas->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_ACTIVE);
            sylvanas->GetMotionMaster()->MoveFollow(me, 1.0f, float(M_PI) * 0.1f);
        }
    }

    void Resume()
    {
        SylvanasFollow();
        SetEscortPaused(false);
        _stepping = false;
        Next(0s);
    }

    void SummonPack(uint8 count, std::initializer_list<std::pair<uint32, uint32>> choices, float spread)
    {
        for (uint8 i = 0; i < count; ++i)
        {
            std::pair<uint32, uint32> const& choice = *(choices.begin() + urand(0, uint32(choices.size()) - 1));
            Position pos = WaveOrigin(choice.second);
            pos.m_positionX += frand(0.0f, spread);
            pos.m_positionY += frand(0.0f, spread);
            me->SummonCreature(choice.first, pos, TEMPSUMMON_DEAD_DESPAWN);
        }
    }

    void SummonSanctumWave(uint32 kind)
    {
        if (kind == 2)
        {
            Position pos = WaveOrigin(SPAWN_ORIGIN_HORDE_KHANOK_ABOVE);
            pos.m_positionX += frand(0.0f, 15.0f);
            pos.m_positionY += frand(0.0f, 15.0f);
            pos.m_positionZ += frand(0.0f, 5.0f);
            me->SummonCreature(NPC_DOOMGUARD_PILLAGER, pos, TEMPSUMMON_DEAD_DESPAWN);
            return;
        }

        uint32 const origin = kind == 0 ? SPAWN_ORIGIN_HORDE_KHANOK_LEFT : SPAWN_ORIGIN_HORDE_KHANOK_RIGHT;
        SummonPack(4, { { NPC_FELGUARD_MARAUDER_H, origin }, { NPC_PERFIDIOUS_DREADLORD_H, origin }, { NPC_TREACHEROUS_GUARDIAN_H, origin }, { NPC_BLIGHT_DOCTOR_H, origin }, { NPC_APOTHECARY_CHEMIST_H, origin }, { NPC_RAVISHING_BETRAYER_H, origin }, { NPC_PLAGUED_FELBEAST_H, origin } }, 5.0f);
    }

    void VarimathrasLeaves(float x, float y, float z, Milliseconds portalDelay)
    {
        if (Creature* varimathras = me->FindNearestCreature(NPC_VARIMATHRAS, 300.0f))
        {
            varimathras->GetMotionMaster()->MovePoint(0, x, y, z);
            varimathras->DespawnOrUnsummon(3s);
        }
        if (Creature* portal = me->FindNearestCreature(NPC_VARIMATHRAS_PORTAL, 300.0f))
            portal->DespawnOrUnsummon(portalDelay);
    }

    ObjectGuid _sylvanas;
    GuidVector _trash;
};

struct npc_eg_undercity_thrall_staging : public ScriptedAI
{
    npc_eg_undercity_thrall_staging(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        bool const running = sUndercityBattleMgr->IsRunning(TEAM_HORDE);
        if (running)
        {
            InitGossipMenuFor(player, GOSSIP_MENU_THRALL);
            player->PrepareQuestMenu(me->GetGUID());
        }
        else
            player->PrepareGossipMenu(me, GOSSIP_MENU_THRALL, true);

        SendGossipMenuFor(player, running ? NPC_TEXT_THRALL_BUSY : NPC_TEXT_THRALL_READY, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        CloseGossipMenuFor(player);
        if (menuId != GOSSIP_MENU_THRALL || gossipListId != GOSSIP_OPTION_START || !sUndercityBattleMgr->CanStart(player, TEAM_HORDE))
            return true;

        sUndercityBattleMgr->Start(TEAM_HORDE);
        sUndercityBattleMgr->SpawnGroup(TEAM_HORDE, me->GetMap(), SPAWN_GROUP_HORDE_LEADERS);
        Creature* thrall = me->FindNearestCreature(NPC_THRALL, 30.0f);
        if (!thrall)
            return true;

        sUndercityBattleMgr->SetLeader(TEAM_HORDE, thrall->GetGUID());

        me->SetVisible(false);
        sUndercityBattleMgr->AddHiddenActor(TEAM_HORDE, me->GetGUID());
        if (Creature* stagingSylvanas = me->FindNearestCreature(NPC_SYLVANAS_STAGING, 30.0f))
        {
            stagingSylvanas->SetVisible(false);
            sUndercityBattleMgr->AddHiddenActor(TEAM_HORDE, stagingSylvanas->GetGUID());
        }

        thrall->AI()->DoAction(ACTION_START_BATTLE);
        return true;
    }
};

struct npc_eg_undercity_sylvanas : public ScriptedAI
{
    npc_eg_undercity_sylvanas(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_WRYNN_BUFF, true);
        _scheduler.CancelAll().Schedule(1s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_SYLVANAS_BUFF, true);
            context.Repeat(10s);
        }).Schedule(3s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHRIEK_OF_THE_HIGHBORNE);
            context.Repeat(3s);
        }).Schedule(5s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHOT);
            context.Repeat(5s, 10s);
        }).Schedule(6s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_MULTI_SHOT);
            context.Repeat(10s, 13s);
        }).Schedule(15s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_BLACK_ARROW);
            context.Repeat(6s, 9s);
        });
    }

    bool CanAIAttack(Unit const* victim) const override
    {
        return victim->GetEntry() != NPC_BLIGHT_SLINGER;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this] { DoMeleeAttackIfReady(); });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_eg_undercity_varimathras : public ScriptedAI
{
    npc_eg_undercity_varimathras(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void Reset() override
    {
        _scheduler.CancelAll().Schedule(5s, 10s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 200.0f))
                DoCast(target, SPELL_CARRION_SWARM);
            context.Repeat(20s);
        }).Schedule(4s, 8s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_DRAIN_LIFE);
            context.Repeat(18s, 25s);
        }).Schedule(4s, 8s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SHADOW_BOLT_VOLLEY);
            context.Repeat(4s, 8s);
        });
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCastSelf(SPELL_MIGHT_OF_VARIMATHRAS, true);
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_VARIMATHRAS_DEATH);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this] { DoMeleeAttackIfReady(); });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_eg_undercity_khanok : public ScriptedAI
{
    npc_eg_undercity_khanok(Creature* creature) : ScriptedAI(creature)
    {
        _scheduler.SetValidator([this] { return !me->HasUnitState(UNIT_STATE_CASTING); });
    }

    void Reset() override
    {
        _scheduler.CancelAll().Schedule(3s, 5s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 200.0f))
                DoCast(target, SPELL_VEIL_OF_SHADOW);
            context.Repeat(15s, 18s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this] { DoMeleeAttackIfReady(); });
    }

private:
    TaskScheduler _scheduler;
};

struct npc_eg_undercity_doomguard_pillager : public ScriptedAI
{
    npc_eg_undercity_doomguard_pillager(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->GetMotionMaster()->MovePath(PATH_DOOMGUARD_PILLAGER, false);
    }

    void WaypointPathEnded(uint32 /*nodeId*/, uint32 /*pathId*/) override
    {
        me->SetHomePosition(me->GetPosition());
        me->SetReactState(REACT_AGGRESSIVE);
        if (Creature* thrall = me->FindNearestCreature(NPC_THRALL, 200.0f))
            AttackStart(thrall);
    }
};

struct npc_eg_undercity_blight_slinger : public ScriptedAI
{
    npc_eg_undercity_blight_slinger(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
    }

    void JustAppeared() override
    {
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        me->SetImmuneToNPC(true);
        _scheduler.Schedule(2s, 5s, [this](TaskContext context)
        {
            if (Creature* trigger = me->FindNearestCreature(NPC_SLINGER_TRIGGER, 100.0f))
                DoCast(trigger, SPELL_BLIGHT_BOMB);
            context.Repeat(12s, 15s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    TaskScheduler _scheduler;
};

class EG_undercity_battle : public WorldZoneScript
{
public:
    EG_undercity_battle() : WorldZoneScript("EG_undercity_battle", MAP_EASTERN_KINGDOMS, { ZONE_UNDERCITY, ZONE_TIRISFAL_GLADES }, PHASE_HORDE | PHASE_ALLIANCE) { }

    void OnPlayerEnter(Player* player, uint32 /*zoneId*/) override
    {
        sUndercityBattleMgr->AddZonePlayer(player->GetGUID());
    }

    void OnPlayerLeave(Player* player, uint32 /*zoneId*/) override
    {
        sUndercityBattleMgr->RemoveZonePlayer(player->GetGUID());
    }

    void FillInitialWorldStates(Player* player, WorldPackets::WorldState::InitWorldStates& packet) override
    {
        sUndercityBattleMgr->FillInitialWorldStates(player, packet);
    }

    void OnUnitDeath(Unit* unit) override
    {
        if (Creature* creature = unit->ToCreature())
            sUndercityBattleMgr->HandleCreatureDeath(creature);
    }

    void OnUpdate(Map* /*map*/, uint32 diff) override
    {
        sUndercityBattleMgr->Update(diff);
    }
};

void AddSC_EG_battle_for_undercity()
{
    RegisterCreatureAI(npc_eg_undercity_wrynn);
    RegisterCreatureAI(npc_eg_undercity_jaina);
    RegisterCreatureAI(npc_eg_undercity_blight_worm);
    RegisterSpellScript(spell_eg_undercity_blight_worm_ingest);
    RegisterCreatureAI(npc_eg_undercity_putress);
    RegisterCreatureAI(npc_eg_undercity_thrall);
    RegisterCreatureAI(npc_eg_undercity_thrall_staging);
    RegisterCreatureAI(npc_eg_undercity_sylvanas);
    RegisterCreatureAI(npc_eg_undercity_varimathras);
    RegisterCreatureAI(npc_eg_undercity_khanok);
    RegisterCreatureAI(npc_eg_undercity_doomguard_pillager);
    RegisterCreatureAI(npc_eg_undercity_blight_slinger);
    new EG_undercity_battle();
}
