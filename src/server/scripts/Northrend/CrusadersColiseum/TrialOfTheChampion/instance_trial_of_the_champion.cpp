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

#include "trial_of_the_champion.h"
#include "Containers.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TaskScheduler.h"
#include "TemporarySummon.h"
#include "Vehicle.h"

enum InstanceTrialOfTheChampionMisc
{
    SPELL_HERALD_MUSIC = 64787,
    SPELL_GET_PLAYER_COUNT = 66986,
    SPELL_SUMMON_CHAMPION_RETAINER_WAVE = 67295,
    SPELL_SUMMON_BOSS_4 = 67396,
    SPELL_SPECTATOR_BUNNY_AURA = 66812,
    SPELL_SALUTE_RANDOM_PLAYER = 67551,
    SPELL_RIDE_VEHICLE_HARDCODED = 46598,
    SPELL_GRAND_CHAMPIONS_CREDIT = 68572,
    SPELL_FACE_BLACK_KNIGHT = 67482,
    SPELL_DEATHS_RESPITE_HERALD = 66798,
    SPELL_DEATHS_PUSH = 66797,
    SPELL_HAMMER_OF_THE_RIGHTEOUS_OVERRIDE = 66904,
    SPELL_HAMMER_OF_JUSTICE_STUN = 66940,
    SPELL_RADIANCE = 66862,
    SPELL_RADIANCE_HEROIC = 67681,
    SPELL_CONFESS = 66547,
    SPELL_HOLY_FIRE = 66538,
    SPELL_HOLY_FIRE_HEROIC = 67676,
    SPELL_SHADOWS_OF_THE_PAST = 66619,
    SPELL_SHADOWS_OF_THE_PAST_HEROIC = 67678,
    SPELL_OLD_WOUNDS = 66620,
    SPELL_OLD_WOUNDS_HEROIC = 67679,
    SPELL_WAKING_NIGHTMARE = 66552,
    SPELL_WAKING_NIGHTMARE_HEROIC = 67677,
    SPELL_MARKED_FOR_DEATH = 67823,
    SPELL_MARKED_FOR_DEATH_HEROIC = 67882,

    GO_FIREWORK_RED_TYPE_1 = 180703,
    GO_FIREWORK_RED_TYPE_2_BIG = 180708,
    GO_FIREWORK_BLUE_TYPE_1 = 180720,
    GO_FIREWORK_BLUE_TYPE_2_BIG = 180723,
    GO_FIREWORK_WHITE_TYPE_1 = 180728,
    GO_FIREWORK_WHITE_TYPE_2 = 180730,
    GO_FIREWORK_YELLOW_TYPE_1 = 180736,
    GO_FIREWORK_YELLOW_TYPE_2 = 180738,

    SOUND_HERALD_MUSIC = 15852,
    SOUND_CROWD_ROAR = 8574,
    SOUND_CROWD_CHEER = 15882,

    SAY_HERALD_EADRIC = 0,
    SAY_HERALD_PALETRESS,
    SAY_HERALD_PRESENT_TEAM,
    SAY_HERALD_ERESSEA_COLOSOS,
    SAY_HERALD_ZULTORE_JAELYNE,
    SAY_HERALD_MOKRA_AMBROSE,
    SAY_HERALD_VISCERI_JACOB,
    SAY_HERALD_RUNOK_LANA,
    SAY_HERALD_RAFTERS,
    SAY_HERALD_PRESENT_PLAYER_FIRST,
    SAY_HERALD_PRESENT_PLAYER_2,
    SAY_HERALD_PRESENT_PLAYER_3,
    SAY_HERALD_PRESENT_PLAYER_4,
    SAY_HERALD_PRESENT_PLAYER_LAST,

    SAY_TIRION_WELCOME = 50,
    SAY_TIRION_FIRST_CHALLENGE,
    SAY_TIRION_BEGIN,
    SAY_TIRION_ARGENT_CHALLENGE,
    SAY_TIRION_ARGENT_BEGIN,
    SAY_TIRION_BLACK_KNIGHT,
    SAY_TIRION_MEANING,
    SAY_TIRION_OUTRO_1,
    SAY_TIRION_OUTRO_2,

    SAY_VARIAN_INTRO_ALLIANCE = 50,
    SAY_VARIAN_BLACK_KNIGHT,
    SAY_VARIAN_INTRO_HORDE,
    SAY_VARIAN_OUTRO,

    SAY_GARROSH_INTRO_HORDE = 50,
    SAY_GARROSH_BLACK_KNIGHT,
    SAY_GARROSH_INTRO_ALLIANCE,

    SAY_THRALL_INTRO_HORDE = 0,
    SAY_THRALL_OUTRO,
    SAY_THRALL_INTRO_ALLIANCE,

    SAY_JAINA_INTRO_HORDE = 0,
    SAY_JAINA_INTRO_ALLIANCE,

    SAY_EADRIC_INTRO = 0,

    SAY_PALETRESS_INTRO_1 = 0,
    SAY_PALETRESS_INTRO_2,

    SAY_BLACK_KNIGHT_INTRO_1 = 0,
    SAY_BLACK_KNIGHT_INTRO_2,
    SAY_BLACK_KNIGHT_INTRO_3,

    SPAWN_ID_BUNNY_SUMMONER = 143293,
    SPAWN_ID_BUNNY_RETAINER = 143295,
    SPAWN_ID_BUNNY_CROWD = 143296,

    NPC_MOKRA_MOUNT = 35638,
    NPC_ERESSEA_MOUNT = 35635,
    NPC_RUNOK_MOUNT = 35640,
    NPC_ZULTORE_MOUNT = 35641,
    NPC_VISCERI_MOUNT = 35634,
    NPC_JACOB_MOUNT = 35637,
    NPC_AMBROSE_MOUNT = 35633,
    NPC_COLOSOS_MOUNT = 35768,
    NPC_JAELYNE_MOUNT = 34658,
    NPC_LANA_MOUNT = 35636,

    NPC_MEMORY_HOGGER = 34942,
    NPC_MEMORY_VANCLEEF = 35028,
    NPC_MEMORY_MUTANUS = 35029,
    NPC_MEMORY_HEROD = 35030,
    NPC_MEMORY_LUCIFRON = 35031,
    NPC_MEMORY_THUNDERAAN = 35032,
    NPC_MEMORY_CHROMAGGUS = 35033,
    NPC_MEMORY_HAKKAR = 35034,
    NPC_MEMORY_VEKNILASH = 35036,
    NPC_MEMORY_KALITHRESH = 35037,
    NPC_MEMORY_MALCHEZAAR = 35038,
    NPC_MEMORY_GRUUL = 35039,
    NPC_MEMORY_VASHJ = 35040,
    NPC_MEMORY_ARCHIMONDE = 35041,
    NPC_MEMORY_ILLIDAN = 35042,
    NPC_MEMORY_DELRISSA = 35043,
    NPC_MEMORY_ENTROPIUS = 35044,
    NPC_MEMORY_INGVAR = 35045,
    NPC_MEMORY_CYANIGOSA = 35046,
    NPC_MEMORY_ECK = 35047,
    NPC_MEMORY_ONYXIA = 35048,
    NPC_MEMORY_HEIGAN = 35049,
    NPC_MEMORY_IGNIS = 35050,
    NPC_MEMORY_VEZAX = 35051,
    NPC_MEMORY_ALGALON = 35052,

    CRITERIA_CONFESSOR_HOGGER = 11863,
    CRITERIA_CONFESSOR_VANCLEEF = 11904,
    CRITERIA_CONFESSOR_MUTANUS = 11905,
    CRITERIA_CONFESSOR_HEROD = 11906,
    CRITERIA_CONFESSOR_LUCIFRON = 11907,
    CRITERIA_CONFESSOR_THUNDERAAN = 11908,
    CRITERIA_CONFESSOR_CHROMAGGUS = 11909,
    CRITERIA_CONFESSOR_HAKKAR = 11910,
    CRITERIA_CONFESSOR_VEKNILASH = 11911,
    CRITERIA_CONFESSOR_KALITHRESH = 11912,
    CRITERIA_CONFESSOR_MALCHEZAAR = 11913,
    CRITERIA_CONFESSOR_GRUUL = 11914,
    CRITERIA_CONFESSOR_VASHJ = 11915,
    CRITERIA_CONFESSOR_ARCHIMONDE = 11916,
    CRITERIA_CONFESSOR_ILLIDAN = 11917,
    CRITERIA_CONFESSOR_DELRISSA = 11918,
    CRITERIA_CONFESSOR_ENTROPIUS = 11919,
    CRITERIA_CONFESSOR_INGVAR = 11920,
    CRITERIA_CONFESSOR_CYANIGOSA = 11921,
    CRITERIA_CONFESSOR_ECK = 11922,
    CRITERIA_CONFESSOR_ONYXIA = 11923,
    CRITERIA_CONFESSOR_HEIGAN = 11924,
    CRITERIA_CONFESSOR_IGNIS = 11925,
    CRITERIA_CONFESSOR_VEZAX = 11926,
    CRITERIA_CONFESSOR_ALGALON = 11927,

    SUMMON_GROUP_MOKRA = 1,
    SUMMON_GROUP_ERESSEA = 2,
    SUMMON_GROUP_RUNOK = 3,
    SUMMON_GROUP_ZULTORE = 4,
    SUMMON_GROUP_VISCERI = 5,
    SUMMON_GROUP_JACOB = 6,
    SUMMON_GROUP_AMBROSE = 7,
    SUMMON_GROUP_COLOSOS = 8,
    SUMMON_GROUP_JAELYNE = 9,
    SUMMON_GROUP_LANA = 10,
    SUMMON_GROUP_ARGENT_SOLDIERS = 11,
    SUMMON_GROUP_BLACK_KNIGHT = 12,
    SUMMON_GROUP_MOKRA_GATE = 13,
    SUMMON_GROUP_ERESSEA_GATE = 14,
    SUMMON_GROUP_RUNOK_GATE = 15,
    SUMMON_GROUP_ZULTORE_GATE = 16,
    SUMMON_GROUP_VISCERI_GATE = 17,
    SUMMON_GROUP_JACOB_GATE = 18,
    SUMMON_GROUP_AMBROSE_GATE = 19,
    SUMMON_GROUP_COLOSOS_GATE = 20,
    SUMMON_GROUP_JAELYNE_GATE = 21,
    SUMMON_GROUP_LANA_GATE = 22,
    SUMMON_GROUP_FOOT_SLOT_1 = 23,
    SUMMON_GROUP_EADRIC = 1,
    SUMMON_GROUP_PALETRESS = 2,

    SPAWN_GROUP_MOUNTS_ALLIANCE = 1019,
    SPAWN_GROUP_MOUNTS_HORDE = 1020,

    CRITERIA_IVE_HAD_WORSE = 11789,

    CRITERIA_VICTORIES_WARRIOR = 12538,
    CRITERIA_VICTORIES_WARRIOR_HEROIC = 12539,
    CRITERIA_VICTORIES_HUNTER = 12540,
    CRITERIA_VICTORIES_HUNTER_HEROIC = 12541,
    CRITERIA_VICTORIES_MAGE = 12542,
    CRITERIA_VICTORIES_MAGE_HEROIC = 12543,
    CRITERIA_VICTORIES_ROGUE = 12544,
    CRITERIA_VICTORIES_ROGUE_HEROIC = 12545,
    CRITERIA_VICTORIES_SHAMAN = 12546,
    CRITERIA_VICTORIES_SHAMAN_HEROIC = 12547,
    CRITERIA_VICTORIES_PALETRESS = 12548,
    CRITERIA_VICTORIES_EADRIC = 12550,
    CRITERIA_VICTORIES_BLACK_KNIGHT = 12552,

    CRITERIA_CHAMPION_JACOB = 11420,
    CRITERIA_CHAMPION_LANA = 12298,
    CRITERIA_CHAMPION_COLOSOS = 12299,
    CRITERIA_CHAMPION_AMBROSE = 12300,
    CRITERIA_CHAMPION_JAELYNE = 12301,
    CRITERIA_CHAMPION_MOKRA = 12302,
    CRITERIA_CHAMPION_VISCERI = 12303,
    CRITERIA_CHAMPION_RUNOK = 12304,
    CRITERIA_CHAMPION_ERESSEA = 12305,
    CRITERIA_CHAMPION_ZULTORE = 12306,
    CRITERIA_CHAMPION_JACOB_HEROIC = 12310,
    CRITERIA_CHAMPION_LANA_HEROIC = 12311,
    CRITERIA_CHAMPION_COLOSOS_HEROIC = 12312,
    CRITERIA_CHAMPION_AMBROSE_HEROIC = 12313,
    CRITERIA_CHAMPION_JAELYNE_HEROIC = 12314,
    CRITERIA_CHAMPION_MOKRA_HEROIC = 12318,
    CRITERIA_CHAMPION_VISCERI_HEROIC = 12319,
    CRITERIA_CHAMPION_RUNOK_HEROIC = 12320,
    CRITERIA_CHAMPION_ERESSEA_HEROIC = 12321,
    CRITERIA_CHAMPION_ZULTORE_HEROIC = 12322,

    CHAMPION_WARRIOR = 0,
    CHAMPION_MAGE,
    CHAMPION_SHAMAN,
    CHAMPION_HUNTER,
    CHAMPION_ROGUE,

    PHASE_IDLE = 0,
    PHASE_CHAMPIONS_INTRO_GATE,
    PHASE_CHAMPIONS_INTRO,
    PHASE_CHAMPIONS_MOUNTED,
    PHASE_CHAMPIONS_PAUSED,
    PHASE_CHAMPIONS_FOOT_TRANSITION,
    PHASE_CHAMPIONS_FOOT,
    PHASE_ARGENT_INTRO,
    PHASE_ARGENT,
    PHASE_BLACK_KNIGHT_INTRO,
    PHASE_BLACK_KNIGHT,

    GROUP_STAGE = 1
};

struct GrandChampionInfo
{
    uint32 Entry;
    uint32 MountEntry;
    uint8 HeraldText;
    uint8 SummonGroup;
    uint8 GateSummonGroup;
};

static constexpr std::array<std::array<GrandChampionInfo, 5>, 2> GrandChampions =
{{
    {{
        { NPC_MOKRA, NPC_MOKRA_MOUNT, SAY_HERALD_MOKRA_AMBROSE, SUMMON_GROUP_MOKRA, SUMMON_GROUP_MOKRA_GATE },
        { NPC_ERESSEA, NPC_ERESSEA_MOUNT, SAY_HERALD_ERESSEA_COLOSOS, SUMMON_GROUP_ERESSEA, SUMMON_GROUP_ERESSEA_GATE },
        { NPC_RUNOK, NPC_RUNOK_MOUNT, SAY_HERALD_RUNOK_LANA, SUMMON_GROUP_RUNOK, SUMMON_GROUP_RUNOK_GATE },
        { NPC_ZULTORE, NPC_ZULTORE_MOUNT, SAY_HERALD_ZULTORE_JAELYNE, SUMMON_GROUP_ZULTORE, SUMMON_GROUP_ZULTORE_GATE },
        { NPC_VISCERI, NPC_VISCERI_MOUNT, SAY_HERALD_VISCERI_JACOB, SUMMON_GROUP_VISCERI, SUMMON_GROUP_VISCERI_GATE }
    }},
    {{
        { NPC_JACOB, NPC_JACOB_MOUNT, SAY_HERALD_VISCERI_JACOB, SUMMON_GROUP_JACOB, SUMMON_GROUP_JACOB_GATE },
        { NPC_AMBROSE, NPC_AMBROSE_MOUNT, SAY_HERALD_MOKRA_AMBROSE, SUMMON_GROUP_AMBROSE, SUMMON_GROUP_AMBROSE_GATE },
        { NPC_COLOSOS, NPC_COLOSOS_MOUNT, SAY_HERALD_ERESSEA_COLOSOS, SUMMON_GROUP_COLOSOS, SUMMON_GROUP_COLOSOS_GATE },
        { NPC_JAELYNE, NPC_JAELYNE_MOUNT, SAY_HERALD_ZULTORE_JAELYNE, SUMMON_GROUP_JAELYNE, SUMMON_GROUP_JAELYNE_GATE },
        { NPC_LANA, NPC_LANA_MOUNT, SAY_HERALD_RUNOK_LANA, SUMMON_GROUP_LANA, SUMMON_GROUP_LANA_GATE }
    }}
}};

static constexpr std::array<uint32, 3> SummonChampionSpells =
{
    SPELL_SUMMON_CHAMPION_1,
    SPELL_SUMMON_CHAMPION_2,
    SPELL_SUMMON_CHAMPION_3
};

static constexpr std::array<uint32, 13> ArgentChallengePlayerAuras =
{
    SPELL_HAMMER_OF_THE_RIGHTEOUS_OVERRIDE,
    SPELL_HAMMER_OF_JUSTICE_STUN,
    SPELL_RADIANCE,
    SPELL_RADIANCE_HEROIC,
    SPELL_CONFESS,
    SPELL_HOLY_FIRE,
    SPELL_HOLY_FIRE_HEROIC,
    SPELL_SHADOWS_OF_THE_PAST,
    SPELL_SHADOWS_OF_THE_PAST_HEROIC,
    SPELL_OLD_WOUNDS,
    SPELL_OLD_WOUNDS_HEROIC,
    SPELL_WAKING_NIGHTMARE,
    SPELL_WAKING_NIGHTMARE_HEROIC
};

static constexpr std::array<uint32, 8> Fireworks =
{
    GO_FIREWORK_RED_TYPE_1,
    GO_FIREWORK_RED_TYPE_2_BIG,
    GO_FIREWORK_BLUE_TYPE_1,
    GO_FIREWORK_BLUE_TYPE_2_BIG,
    GO_FIREWORK_WHITE_TYPE_1,
    GO_FIREWORK_WHITE_TYPE_2,
    GO_FIREWORK_YELLOW_TYPE_1,
    GO_FIREWORK_YELLOW_TYPE_2
};

static std::array<Position, 8> const FireworkSpots =
{{
    { 794.441f, 638.306f, 425.700f },
    { 782.309f, 582.892f, 421.546f },
    { 792.309f, 598.457f, 424.636f },
    { 779.951f, 655.656f, 421.818f },
    { 714.158f, 585.533f, 425.579f },
    { 704.793f, 600.736f, 429.519f },
    { 704.302f, 636.326f, 425.136f },
    { 712.521f, 655.648f, 424.593f }
}};

static std::array<Position, 3> const ArgentSoldierPosts =
{{
    { 717.998f, 649.101f, 411.925f },
    { 746.686f, 653.094f, 411.605f },
    { 777.108f, 649.010f, 411.930f }
}};

static Position const ArenaCenter = { 746.977f, 618.793f, 411.971f, 0.0f };
static Position const HeraldArgentFightSpot = { 734.245f, 674.726f, 412.369f, 4.714f };

static constexpr std::array<std::pair<uint32, uint32>, 25> ConfessorCriteria =
{{
    { CRITERIA_CONFESSOR_HOGGER, NPC_MEMORY_HOGGER },
    { CRITERIA_CONFESSOR_VANCLEEF, NPC_MEMORY_VANCLEEF },
    { CRITERIA_CONFESSOR_MUTANUS, NPC_MEMORY_MUTANUS },
    { CRITERIA_CONFESSOR_HEROD, NPC_MEMORY_HEROD },
    { CRITERIA_CONFESSOR_LUCIFRON, NPC_MEMORY_LUCIFRON },
    { CRITERIA_CONFESSOR_THUNDERAAN, NPC_MEMORY_THUNDERAAN },
    { CRITERIA_CONFESSOR_CHROMAGGUS, NPC_MEMORY_CHROMAGGUS },
    { CRITERIA_CONFESSOR_HAKKAR, NPC_MEMORY_HAKKAR },
    { CRITERIA_CONFESSOR_VEKNILASH, NPC_MEMORY_VEKNILASH },
    { CRITERIA_CONFESSOR_KALITHRESH, NPC_MEMORY_KALITHRESH },
    { CRITERIA_CONFESSOR_MALCHEZAAR, NPC_MEMORY_MALCHEZAAR },
    { CRITERIA_CONFESSOR_GRUUL, NPC_MEMORY_GRUUL },
    { CRITERIA_CONFESSOR_VASHJ, NPC_MEMORY_VASHJ },
    { CRITERIA_CONFESSOR_ARCHIMONDE, NPC_MEMORY_ARCHIMONDE },
    { CRITERIA_CONFESSOR_ILLIDAN, NPC_MEMORY_ILLIDAN },
    { CRITERIA_CONFESSOR_DELRISSA, NPC_MEMORY_DELRISSA },
    { CRITERIA_CONFESSOR_ENTROPIUS, NPC_MEMORY_ENTROPIUS },
    { CRITERIA_CONFESSOR_INGVAR, NPC_MEMORY_INGVAR },
    { CRITERIA_CONFESSOR_CYANIGOSA, NPC_MEMORY_CYANIGOSA },
    { CRITERIA_CONFESSOR_ECK, NPC_MEMORY_ECK },
    { CRITERIA_CONFESSOR_ONYXIA, NPC_MEMORY_ONYXIA },
    { CRITERIA_CONFESSOR_HEIGAN, NPC_MEMORY_HEIGAN },
    { CRITERIA_CONFESSOR_IGNIS, NPC_MEMORY_IGNIS },
    { CRITERIA_CONFESSOR_VEZAX, NPC_MEMORY_VEZAX },
    { CRITERIA_CONFESSOR_ALGALON, NPC_MEMORY_ALGALON }
}};

struct ChampionCriteriaInfo
{
    uint32 CriteriaId;
    uint8 Champion;
    bool NormalOnly;
};

static constexpr std::array<ChampionCriteriaInfo, 30> ChampionCriteria =
{{
    { CRITERIA_VICTORIES_WARRIOR, CHAMPION_WARRIOR, true },
    { CRITERIA_VICTORIES_WARRIOR_HEROIC, CHAMPION_WARRIOR, false },
    { CRITERIA_VICTORIES_HUNTER, CHAMPION_HUNTER, true },
    { CRITERIA_VICTORIES_HUNTER_HEROIC, CHAMPION_HUNTER, false },
    { CRITERIA_VICTORIES_MAGE, CHAMPION_MAGE, true },
    { CRITERIA_VICTORIES_MAGE_HEROIC, CHAMPION_MAGE, false },
    { CRITERIA_VICTORIES_ROGUE, CHAMPION_ROGUE, true },
    { CRITERIA_VICTORIES_ROGUE_HEROIC, CHAMPION_ROGUE, false },
    { CRITERIA_VICTORIES_SHAMAN, CHAMPION_SHAMAN, true },
    { CRITERIA_VICTORIES_SHAMAN_HEROIC, CHAMPION_SHAMAN, false },
    { CRITERIA_CHAMPION_JACOB, CHAMPION_WARRIOR, false },
    { CRITERIA_CHAMPION_LANA, CHAMPION_ROGUE, false },
    { CRITERIA_CHAMPION_COLOSOS, CHAMPION_SHAMAN, false },
    { CRITERIA_CHAMPION_AMBROSE, CHAMPION_MAGE, false },
    { CRITERIA_CHAMPION_JAELYNE, CHAMPION_HUNTER, false },
    { CRITERIA_CHAMPION_MOKRA, CHAMPION_WARRIOR, false },
    { CRITERIA_CHAMPION_VISCERI, CHAMPION_ROGUE, false },
    { CRITERIA_CHAMPION_RUNOK, CHAMPION_SHAMAN, false },
    { CRITERIA_CHAMPION_ERESSEA, CHAMPION_MAGE, false },
    { CRITERIA_CHAMPION_ZULTORE, CHAMPION_HUNTER, false },
    { CRITERIA_CHAMPION_JACOB_HEROIC, CHAMPION_WARRIOR, false },
    { CRITERIA_CHAMPION_LANA_HEROIC, CHAMPION_ROGUE, false },
    { CRITERIA_CHAMPION_COLOSOS_HEROIC, CHAMPION_SHAMAN, false },
    { CRITERIA_CHAMPION_AMBROSE_HEROIC, CHAMPION_MAGE, false },
    { CRITERIA_CHAMPION_JAELYNE_HEROIC, CHAMPION_HUNTER, false },
    { CRITERIA_CHAMPION_MOKRA_HEROIC, CHAMPION_WARRIOR, false },
    { CRITERIA_CHAMPION_VISCERI_HEROIC, CHAMPION_ROGUE, false },
    { CRITERIA_CHAMPION_RUNOK_HEROIC, CHAMPION_SHAMAN, false },
    { CRITERIA_CHAMPION_ERESSEA_HEROIC, CHAMPION_MAGE, false },
    { CRITERIA_CHAMPION_ZULTORE_HEROIC, CHAMPION_HUNTER, false }
}};

static DoorData const TrialOfTheChampionDoors[] =
{
    { GO_NORTH_PORTCULLIS, DATA_GRAND_CHAMPIONS, DOOR_TYPE_ROOM },
    { GO_NORTH_PORTCULLIS, DATA_BLACK_KNIGHT, DOOR_TYPE_ROOM },
    { 0, 0, DOOR_TYPE_ROOM }
};

static ObjectData const TrialOfTheChampionCreatures[] =
{
    { NPC_JAEREN_SUNSWORN, DATA_HERALD },
    { NPC_ARELAS_BRIGHTSTAR, DATA_HERALD },
    { NPC_TIRION, DATA_TIRION },
    { NPC_VARIAN, DATA_VARIAN },
    { NPC_JAINA, DATA_JAINA },
    { NPC_THRALL, DATA_THRALL },
    { NPC_GARROSH, DATA_GARROSH },
    { NPC_EADRIC, DATA_ARGENT_CHALLENGE },
    { NPC_PALETRESS, DATA_ARGENT_CHALLENGE },
    { NPC_BLACK_KNIGHT, DATA_BLACK_KNIGHT },
    { NPC_BLACK_KNIGHT_GRYPHON, DATA_BLACK_KNIGHT_GRYPHON },
    { 0, 0 }
};

static ObjectData const TrialOfTheChampionGameObjects[] =
{
    { GO_MAIN_GATE, DATA_MAIN_GATE },
    { GO_NORTH_PORTCULLIS, DATA_NORTH_PORTCULLIS },
    { GO_CHAMPIONS_CACHE, DATA_CHAMPIONS_CACHE },
    { GO_CHAMPIONS_CACHE_H, DATA_CHAMPIONS_CACHE },
    { GO_EADRIC_CACHE, DATA_EADRIC_CACHE },
    { GO_EADRIC_CACHE_H, DATA_EADRIC_CACHE },
    { GO_CONFESSOR_CACHE, DATA_CONFESSOR_CACHE },
    { GO_CONFESSOR_CACHE_H, DATA_CONFESSOR_CACHE },
    { 0, 0 }
};

class instance_trial_of_the_champion : public InstanceMapScript
{
public:
    instance_trial_of_the_champion() : InstanceMapScript(TrialOfTheChampionScriptName, TrialOfTheChampionMapId) { }

    struct instance_trial_of_the_champion_InstanceMapScript : public InstanceScript
    {
        instance_trial_of_the_champion_InstanceMapScript(InstanceMap* map) : InstanceScript(map), _championPicks({ 0, 1, 2 }), _argentChampionEntry(NPC_EADRIC), _activeWave(0), _kneelingChampions(0), _killedMemoryEntry(0), _hadWorse(true), _phase(PHASE_IDLE), _wiped(false)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadDoorData(TrialOfTheChampionDoors);
            LoadObjectData(TrialOfTheChampionCreatures, TrialOfTheChampionGameObjects);

            _scheduler.Schedule(1s, [this](TaskContext context)
            {
                bool livingPlayers = false;
                for (MapReference const& ref : instance->GetPlayers())
                {
                    Player* player = ref.GetSource();
                    if (player && player->IsAlive() && !player->IsGameMaster())
                    {
                        livingPlayers = true;
                        break;
                    }
                }

                bool const mounted = _phase == PHASE_CHAMPIONS_MOUNTED;
                bool const wiped = !livingPlayers && (mounted || _phase == PHASE_CHAMPIONS_FOOT || _phase == PHASE_ARGENT || _phase == PHASE_BLACK_KNIGHT);
                if (wiped && !_wiped)
                {
                    GuidVector const summons = _summonGUIDs;
                    for (ObjectGuid const& guid : summons)
                    {
                        Creature* summon = instance->GetCreature(guid);
                        if (summon && summon->IsAlive() && summon->IsAIEnabled())
                            summon->AI()->DoAction(ACTION_NO_ALIVE_PLAYERS);
                    }

                    if (mounted)
                    {
                        for (ObjectGuid const& guid : _playerMountGUIDs)
                        {
                            Creature* mount = instance->GetCreature(guid);
                            if (mount && mount->IsAlive() && mount->GetVehicleKit() && !mount->GetVehicleKit()->IsVehicleInUse())
                                mount->DespawnOrUnsummon(0s, 1s);
                        }
                    }
                }

                _wiped = wiped;
                context.Repeat();
            });
        }

        void Create() override
        {
            std::array<uint8, 5> indexes = { 0, 1, 2, 3, 4 };
            Trinity::Containers::RandomShuffle(indexes);
            _championPicks = { indexes[0], indexes[1], indexes[2] };
            _argentChampionEntry = RAND(NPC_EADRIC, NPC_PALETRESS);

            InstanceScript::Create();
            RestoreMounts();
        }

        void Load(char const* data) override
        {
            InstanceScript::Load(data);

            if (GetBossState(DATA_GRAND_CHAMPIONS) == TO_BE_DECIDED)
                Create();
            else if (GetBossState(DATA_GRAND_CHAMPIONS) != DONE)
                RestoreMounts();
        }

        void WriteSaveDataMore(std::ostringstream& data) override
        {
            data << uint32(_championPicks[0]) << ' ' << uint32(_championPicks[1]) << ' ' << uint32(_championPicks[2]) << ' ' << _argentChampionEntry;
        }

        void ReadSaveDataMore(std::istringstream& data) override
        {
            std::array<uint32, 3> picks = { };
            uint32 argentChampionEntry = 0;
            data >> picks[0] >> picks[1] >> picks[2] >> argentChampionEntry;

            if (picks[0] < 5 && picks[1] < 5 && picks[2] < 5 && picks[0] != picks[1] && picks[0] != picks[2] && picks[1] != picks[2])
                _championPicks = { uint8(picks[0]), uint8(picks[1]), uint8(picks[2]) };

            if (argentChampionEntry == NPC_EADRIC || argentChampionEntry == NPC_PALETRESS)
                _argentChampionEntry = argentChampionEntry;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            InstanceScript::OnCreatureCreate(creature);

            if (creature->IsSummon() && !creature->IsControlledByPlayer() && !creature->ToTempSummon()->GetSummonerGUID().IsPlayer())
                _summonGUIDs.push_back(creature->GetGUID());

            switch (creature->GetEntry())
            {
                case NPC_ARGENT_WARHORSE_ALLIANCE:
                case NPC_ARGENT_BATTLEWORG_HORDE:
                    _playerMountGUIDs.push_back(creature->GetGUID());
                    break;
                case NPC_ARGENT_WARHORSE_HORDE:
                case NPC_ARGENT_BATTLEWORG_ALLIANCE:
                    _hostileMountGUIDs.push_back(creature->GetGUID());
                    break;
                default:
                    break;
            }

            switch (creature->GetSpawnId())
            {
                case SPAWN_ID_BUNNY_SUMMONER:
                    _summonerBunnyGUID = creature->GetGUID();
                    for (uint32 type = DATA_GRAND_CHAMPIONS; type < EncounterCount; ++type)
                    {
                        EncounterState const state = GetBossState(type);
                        if (state == SPECIAL || state == IN_PROGRESS || state == FAIL)
                        {
                            _scheduler.Schedule(1s, [this, type](TaskContext /*context*/)
                            {
                                _scheduler.CancelGroup(GROUP_STAGE);
                                for (ObjectGuid const& guid : _summonGUIDs)
                                {
                                    if (Creature* summon = instance->GetCreature(guid))
                                        summon->DespawnOrUnsummon();
                                }
                                _summonGUIDs.clear();

                                _phase = PHASE_IDLE;
                                _championGUIDs = { };
                                _waveGUIDs = { };
                                _argentSoldierGUIDs.clear();
                                _activeWave = 0;
                                _kneelingChampions = 0;
                                _killedMemoryEntry = 0;
                                _hadWorse = true;
                                HandleGameObject(ObjectGuid::Empty, false, GetGameObject(DATA_MAIN_GATE));
                                HandleGameObject(ObjectGuid::Empty, true, GetGameObject(DATA_NORTH_PORTCULLIS));

                                if (Creature* herald = GetCreature(DATA_HERALD))
                                {
                                    std::array<Position, EncounterCount> const spots = { herald->GetHomePosition(), HeraldCenterSpot, HeraldArgentSpot };
                                    herald->RemoveAurasDueToSpell(SPELL_HERALD_FEIGN_DEATH);
                                    herald->RemoveAurasDueToSpell(SPELL_DEATHS_RESPITE_HERALD);
                                    herald->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                                    herald->GetMotionMaster()->Clear();
                                    herald->NearTeleportTo(spots[type]);
                                    herald->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                                }

                                SetBossState(type, NOT_STARTED);
                                if (GetBossState(DATA_GRAND_CHAMPIONS) != DONE)
                                    RestoreMounts();
                            });
                        }
                    }
                    break;
                case SPAWN_ID_BUNNY_RETAINER:
                    _retainerBunnyGUID = creature->GetGUID();
                    break;
                case SPAWN_ID_BUNNY_CROWD:
                    _crowdBunnyGUID = creature->GetGUID();
                    break;
                default:
                    break;
            }

            for (uint8 slot = 0; slot < 3; ++slot)
            {
                if (GetChampionInfo(slot).Entry == creature->GetEntry())
                    _championGUIDs[slot] = creature->GetGUID();
            }
        }

        void OnUnitDeath(Unit* unit) override
        {
            switch (unit->GetEntry())
            {
                case NPC_ARGENT_WARHORSE_ALLIANCE:
                case NPC_ARGENT_BATTLEWORG_HORDE:
                    if (Creature* mount = unit->ToCreature())
                    {
                        ObjectGuid::LowType const spawnId = mount->GetSpawnId();
                        mount->DespawnOrUnsummon();
                        _scheduler.Schedule(5s, 10s, GROUP_STAGE, [this, spawnId](TaskContext /*context*/)
                        {
                            if (IsFootPhase() || GetBossState(DATA_GRAND_CHAMPIONS) == DONE)
                                return;

                            instance->RemoveRespawnTime(SPAWN_TYPE_CREATURE, spawnId);
                            instance->SpawnGroupSpawn(instance->GetTeamIdInInstance() == TEAM_ALLIANCE ? SPAWN_GROUP_MOUNTS_ALLIANCE : SPAWN_GROUP_MOUNTS_HORDE);
                        });
                    }
                    break;
                case NPC_ORGRIMMAR_CHAMPION:
                case NPC_SILVERMOON_CHAMPION:
                case NPC_THUNDER_BLUFF_CHAMPION:
                case NPC_SENJIN_CHAMPION:
                case NPC_UNDERCITY_CHAMPION:
                case NPC_STORMWIND_CHAMPION:
                case NPC_GNOMEREGAN_CHAMPION:
                case NPC_EXODAR_CHAMPION:
                case NPC_DARNASSUS_CHAMPION:
                case NPC_IRONFORGE_CHAMPION:
                    if (_activeWave < 3 && AllDead(_waveGUIDs[_activeWave]))
                    {
                        ++_activeWave;
                        if (Creature* bunny = instance->GetCreature(_retainerBunnyGUID))
                            bunny->CastSpell(bunny, SPELL_SUMMON_CHAMPION_RETAINER_WAVE, true);
                    }
                    break;
                case NPC_ARGENT_LIGHTWIELDER:
                case NPC_ARGENT_MONK:
                case NPC_ARGENT_PRIESTESS:
                    if (AllDead(_argentSoldierGUIDs))
                    {
                        if (Creature* champion = GetCreature(DATA_ARGENT_CHALLENGE))
                            champion->AI()->DoAction(ACTION_ENGAGE);
                        CrowdRoar();
                    }
                    break;
                case NPC_BLACK_KNIGHT:
                    _phase = PHASE_IDLE;
                    CrowdRoar();
                    _scheduler.Schedule(6s, GROUP_STAGE, [this](TaskContext /*context*/)
                    {
                        StartSpectatorReactions();
                    })
                    .Schedule(7s, [this](TaskContext context)
                    {
                        if (Creature* tirion = GetCreature(DATA_TIRION))
                            tirion->AI()->Talk(SAY_TIRION_OUTRO_1);

                        context.Schedule(6s, [this](TaskContext /*context*/)
                        {
                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                tirion->AI()->Talk(SAY_TIRION_OUTRO_2);
                        })
                        .Schedule(12s, [this](TaskContext /*context*/)
                        {
                            if (instance->GetTeamIdInInstance() == TEAM_ALLIANCE)
                            {
                                if (Creature* varian = GetCreature(DATA_VARIAN))
                                    varian->AI()->Talk(SAY_VARIAN_OUTRO);
                            }
                            else if (Creature* thrall = GetCreature(DATA_THRALL))
                                thrall->AI()->Talk(SAY_THRALL_OUTRO);
                        })
                        .Schedule(13s, [this](TaskContext /*context*/)
                        {
                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                tirion->CastSpell(tirion, SPELL_SALUTE_RANDOM_PLAYER);
                        });
                    });
                    break;
                default:
                    break;
            }
        }

        uint32 GetData(uint32 type) const override
        {
            switch (type)
            {
                case DATA_CHAMPION_FOOT_PHASE:
                    return IsFootPhase() ? 1 : 0;
                case DATA_STAGE_PAUSED:
                    return _phase == PHASE_CHAMPIONS_PAUSED ? 1 : 0;
                default:
                    return 0;
            }
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_HERALD_GOSSIP:
                    if (data == GOSSIP_OPTION_START && _phase == PHASE_CHAMPIONS_PAUSED)
                    {
                        _phase = PHASE_CHAMPIONS_MOUNTED;
                        if (Creature* herald = GetCreature(DATA_HERALD))
                            herald->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);

                        if (Creature* bunny = instance->GetCreature(_retainerBunnyGUID))
                            bunny->CastSpell(bunny, SPELL_SUMMON_CHAMPION_RETAINER_WAVE, true);
                    }
                    else if ((data == GOSSIP_OPTION_START || data == GOSSIP_OPTION_START_SKIP) && GetBossState(DATA_GRAND_CHAMPIONS) == NOT_STARTED)
                    {
                        SetBossState(DATA_GRAND_CHAMPIONS, SPECIAL);
                        _waveGUIDs = { };
                        _championGUIDs = { };
                        _activeWave = 0;
                        _phase = data == GOSSIP_OPTION_START ? PHASE_CHAMPIONS_INTRO_GATE : PHASE_CHAMPIONS_INTRO;

                        if (Creature* herald = GetCreature(DATA_HERALD))
                        {
                            herald->CastSpell(herald, SPELL_GET_PLAYER_COUNT);
                            herald->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                            herald->SetWalk(true);
                            herald->GetMotionMaster()->MovePoint(0, { .Destination = Position(732.524f, 663.007f, 412.393f), .GeneratePath = true, .FinalOrientation = 4.714f, .Velocity = HeraldVelocity });
                            if (_phase == PHASE_CHAMPIONS_INTRO_GATE)
                                herald->AI()->Talk(SAY_HERALD_PRESENT_TEAM);
                        }

                        if (_phase != PHASE_CHAMPIONS_INTRO_GATE)
                        {
                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                tirion->AI()->Talk(SAY_TIRION_WELCOME);

                            _scheduler.Schedule(6s, GROUP_STAGE, [this](TaskContext /*context*/)
                            {
                                if (Creature* tirion = GetCreature(DATA_TIRION))
                                    tirion->AI()->Talk(SAY_TIRION_FIRST_CHALLENGE);
                            })
                            .Schedule(7s, GROUP_STAGE, [this](TaskContext /*context*/)
                            {
                                if (Creature* bunny = instance->GetCreature(_summonerBunnyGUID))
                                    bunny->CastSpell(bunny, SummonChampionSpells[0], true);
                            })
                            .Schedule(10s, GROUP_STAGE, [this](TaskContext /*context*/)
                            {
                                if (Creature* bunny = instance->GetCreature(_summonerBunnyGUID))
                                    bunny->CastSpell(bunny, SummonChampionSpells[1], true);
                            })
                            .Schedule(12s, GROUP_STAGE, [this](TaskContext /*context*/)
                            {
                                if (Creature* bunny = instance->GetCreature(_summonerBunnyGUID))
                                    bunny->CastSpell(bunny, SummonChampionSpells[2], true);
                            })
                            .Schedule(18s, GROUP_STAGE, [this](TaskContext context)
                            {
                                if (Creature* tirion = GetCreature(DATA_TIRION))
                                    tirion->AI()->Talk(SAY_TIRION_BEGIN);

                                _phase = PHASE_CHAMPIONS_MOUNTED;
                                if (Creature* bunny = instance->GetCreature(_retainerBunnyGUID))
                                    bunny->CastSpell(bunny, SPELL_SUMMON_CHAMPION_RETAINER_WAVE, true);

                                CrowdRoar();
                                context.Schedule(6s, GROUP_STAGE, [this](TaskContext /*context*/)
                                {
                                    StartSpectatorReactions();
                                });
                            });
                            break;
                        }

                        DoCastSpellOnPlayers(SPELL_HERALD_MUSIC);
                        bool const alliance = instance->GetTeamIdInInstance() == TEAM_ALLIANCE;

                        _scheduler.Schedule(5s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                tirion->AI()->Talk(SAY_TIRION_WELCOME);
                        })
                        .Schedule(14s, GROUP_STAGE, [this, alliance](TaskContext /*context*/)
                        {
                            if (Creature* speaker = GetCreature(alliance ? DATA_VARIAN : DATA_THRALL))
                                speaker->AI()->Talk(alliance ? SAY_VARIAN_INTRO_ALLIANCE : SAY_THRALL_INTRO_HORDE);
                        })
                        .Schedule(alliance ? 22s : 19s, GROUP_STAGE, [this, alliance](TaskContext /*context*/)
                        {
                            if (Creature* speaker = GetCreature(alliance ? DATA_JAINA : DATA_GARROSH))
                                speaker->AI()->Talk(alliance ? SAY_JAINA_INTRO_ALLIANCE : SAY_GARROSH_INTRO_HORDE);
                        })
                        .Schedule(27s, GROUP_STAGE, [this, alliance](TaskContext /*context*/)
                        {
                            if (Creature* speaker = GetCreature(alliance ? DATA_GARROSH : DATA_VARIAN))
                                speaker->AI()->Talk(alliance ? SAY_GARROSH_INTRO_ALLIANCE : SAY_VARIAN_INTRO_HORDE);
                        })
                        .Schedule(35s, GROUP_STAGE, [this, alliance](TaskContext /*context*/)
                        {
                            if (Creature* speaker = GetCreature(alliance ? DATA_THRALL : DATA_JAINA))
                                speaker->AI()->Talk(alliance ? SAY_THRALL_INTRO_ALLIANCE : SAY_JAINA_INTRO_HORDE);
                        })
                        .Schedule(40s, GROUP_STAGE, [this](TaskContext context)
                        {
                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                tirion->AI()->Talk(SAY_TIRION_FIRST_CHALLENGE);

                            std::vector<ObjectGuid> players;
                            for (MapReference const& ref : instance->GetPlayers())
                            {
                                if (Player* player = ref.GetSource())
                                    if (!player->IsGameMaster())
                                        players.push_back(player->GetGUID());
                            }

                            std::array<uint8, 5> presentations = { SAY_HERALD_PRESENT_PLAYER_FIRST, SAY_HERALD_PRESENT_PLAYER_2, SAY_HERALD_PRESENT_PLAYER_3, SAY_HERALD_PRESENT_PLAYER_4, SAY_HERALD_PRESENT_PLAYER_LAST };
                            Trinity::Containers::RandomShuffle(presentations);

                            Milliseconds delay = 1s;
                            for (std::size_t i = 0; i < players.size() && i < presentations.size(); ++i)
                            {
                                context.Schedule(delay, GROUP_STAGE, [this, guid = players[i], text = presentations[i]](TaskContext /*context*/)
                                {
                                    Player* player = ObjectAccessor::GetPlayer(instance, guid);
                                    if (!player)
                                        return;

                                    if (Creature* herald = GetCreature(DATA_HERALD))
                                        herald->AI()->Talk(text, player);

                                    if (Creature* bunny = instance->GetCreature(_crowdBunnyGUID))
                                        bunny->CastSpell(player, SPELL_MASTER_FX_CHANT, true);
                                });
                                delay += 7s;
                            }

                            for (uint8 slot = 0; slot < 3; ++slot)
                            {
                                context.Schedule(delay, GROUP_STAGE, [this, slot](TaskContext /*context*/)
                                {
                                    HandleGameObject(ObjectGuid::Empty, true, GetGameObject(DATA_MAIN_GATE));
                                    if (Creature* bunny = instance->GetCreature(_summonerBunnyGUID))
                                        bunny->CastSpell(bunny, SummonChampionSpells[slot], true);
                                })
                                .Schedule(delay + 10s, GROUP_STAGE, [this](TaskContext /*context*/)
                                {
                                    HandleGameObject(ObjectGuid::Empty, false, GetGameObject(DATA_MAIN_GATE));
                                });
                                delay += 12s;
                            }

                            context.Schedule(delay + 3s, GROUP_STAGE, [this](TaskContext context)
                            {
                                if (Creature* tirion = GetCreature(DATA_TIRION))
                                    tirion->AI()->Talk(SAY_TIRION_BEGIN);

                                _phase = PHASE_CHAMPIONS_MOUNTED;
                                if (Creature* bunny = instance->GetCreature(_retainerBunnyGUID))
                                    bunny->CastSpell(bunny, SPELL_SUMMON_CHAMPION_RETAINER_WAVE, true);

                                CrowdRoar();
                                context.Schedule(6s, GROUP_STAGE, [this](TaskContext /*context*/)
                                {
                                    StartSpectatorReactions();
                                });
                            });
                        });
                    }
                    else if (data == GOSSIP_OPTION_NEXT_CHALLENGE && GetBossState(DATA_GRAND_CHAMPIONS) == DONE && GetBossState(DATA_ARGENT_CHALLENGE) == NOT_STARTED)
                    {
                        SetBossState(DATA_ARGENT_CHALLENGE, SPECIAL);
                        _phase = PHASE_ARGENT_INTRO;
                        _argentSoldierGUIDs.clear();

                        if (Creature* herald = GetCreature(DATA_HERALD))
                        {
                            herald->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                            herald->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                            herald->SetWalk(true);
                            herald->GetMotionMaster()->MovePoint(0, { .Destination = Position(746.356f, 620.694f, 411.989f), .Velocity = HeraldVelocity });
                        }

                        _scheduler.Schedule(3s, GROUP_STAGE, [this](TaskContext context)
                        {
                            if (Creature* herald = GetCreature(DATA_HERALD))
                                herald->PlayDistanceSound(SOUND_HERALD_MUSIC);

                            context.Schedule(1s, GROUP_STAGE, [this](TaskContext context)
                            {
                                HandleGameObject(ObjectGuid::Empty, true, GetGameObject(DATA_MAIN_GATE));

                                std::list<TempSummon*> soldiers;
                                if (Creature* bunny = instance->GetCreature(_summonerBunnyGUID))
                                    bunny->SummonCreatureGroup(SUMMON_GROUP_ARGENT_SOLDIERS, &soldiers);
                                _phase = PHASE_ARGENT;

                                std::vector<TempSummon*> lightwielders;
                                for (TempSummon* soldier : soldiers)
                                {
                                    _argentSoldierGUIDs.push_back(soldier->GetGUID());
                                    if (soldier->GetEntry() == NPC_ARGENT_LIGHTWIELDER)
                                        lightwielders.push_back(soldier);
                                }

                                std::sort(lightwielders.begin(), lightwielders.end(), [](TempSummon const* left, TempSummon const* right)
                                {
                                    return left->GetPositionX() < right->GetPositionX();
                                });

                                for (std::size_t i = 0; i < lightwielders.size() && i < std::size(ArgentSoldierPosts); ++i)
                                {
                                    Position const post(ArgentSoldierPosts[i].GetPositionX(), ArgentSoldierPosts[i].GetPositionY(), ArgentSoldierPosts[i].GetPositionZ(), ArgentSoldierPosts[i].GetAbsoluteAngle(ArenaCenter));
                                    lightwielders[i]->SetHomePosition(post);
                                    lightwielders[i]->AI()->DoAction(ACTION_POST_ASSIGNED);
                                    lightwielders[i]->SetWalk(true);
                                    lightwielders[i]->GetMotionMaster()->MovePoint(POINT_ARGENT_POST, { .Destination = post, .GeneratePath = true, .FinalOrientation = post.GetOrientation(), .Velocity = 3.2f });
                                }

                                for (TempSummon* soldier : soldiers)
                                {
                                    if (soldier->GetEntry() == NPC_ARGENT_LIGHTWIELDER || lightwielders.empty())
                                        continue;

                                    std::size_t const leader = std::distance(lightwielders.begin(), std::min_element(lightwielders.begin(), lightwielders.end(), [soldier](TempSummon const* left, TempSummon const* right)
                                    {
                                        return soldier->GetDistance(left) < soldier->GetDistance(right);
                                    }));

                                    if (leader >= std::size(ArgentSoldierPosts))
                                        continue;

                                    Position const& post = ArgentSoldierPosts[leader];
                                    bool const monk = soldier->GetEntry() == NPC_ARGENT_MONK;
                                    float const facing = post.GetAbsoluteAngle(ArenaCenter);
                                    float const angle = facing + (monk ? 1.30f : -1.24f);
                                    float const distance = monk ? 5.42f : 5.05f;
                                    Position const slot(post.GetPositionX() + distance * std::cos(angle), post.GetPositionY() + distance * std::sin(angle), post.GetPositionZ(), facing);
                                    soldier->SetHomePosition(slot);
                                    soldier->AI()->DoAction(ACTION_POST_ASSIGNED);
                                    soldier->SetWalk(true);
                                    soldier->GetMotionMaster()->MovePoint(POINT_ARGENT_POST, { .Destination = slot, .GeneratePath = true, .FinalOrientation = slot.GetOrientation(), .Velocity = 3.2f });
                                }

                                context.Schedule(3s, GROUP_STAGE, [this](TaskContext context)
                                {
                                    if (Creature* herald = GetCreature(DATA_HERALD))
                                    {
                                        herald->CastSpell(herald, SPELL_SUMMON_BOSS_4);
                                        herald->AI()->Talk(_argentChampionEntry == NPC_EADRIC ? SAY_HERALD_EADRIC : SAY_HERALD_PALETRESS);
                                    }

                                    context.Schedule(10s, GROUP_STAGE, [this](TaskContext /*context*/)
                                    {
                                        HandleGameObject(ObjectGuid::Empty, false, GetGameObject(DATA_MAIN_GATE));
                                        if (Creature* herald = GetCreature(DATA_HERALD))
                                            herald->GetMotionMaster()->MovePoint(0, { .Destination = HeraldArgentFightSpot, .GeneratePath = true, .FinalOrientation = HeraldArgentFightSpot.GetOrientation(), .Velocity = HeraldVelocity });

                                        if (GetBossState(DATA_ARGENT_CHALLENGE) == SPECIAL && AllDead(_argentSoldierGUIDs))
                                            if (Creature* champion = GetCreature(DATA_ARGENT_CHALLENGE))
                                                champion->AI()->DoAction(ACTION_ENGAGE);
                                    })
                                    .Schedule(12s, GROUP_STAGE, [this](TaskContext /*context*/)
                                    {
                                        if (GetBossState(DATA_ARGENT_CHALLENGE) != SPECIAL)
                                            return;

                                        if (Creature* champion = GetCreature(DATA_ARGENT_CHALLENGE))
                                            champion->AI()->Talk(_argentChampionEntry == NPC_EADRIC ? SAY_EADRIC_INTRO : SAY_PALETRESS_INTRO_1);
                                    })
                                    .Schedule(18s, GROUP_STAGE, [this](TaskContext /*context*/)
                                    {
                                        if (_argentChampionEntry != NPC_PALETRESS || GetBossState(DATA_ARGENT_CHALLENGE) != SPECIAL)
                                            return;

                                        if (Creature* champion = GetCreature(DATA_ARGENT_CHALLENGE))
                                            champion->AI()->Talk(SAY_PALETRESS_INTRO_2);
                                    })
                                    .Schedule(32s, GROUP_STAGE, [this](TaskContext context)
                                    {
                                        if (GetBossState(DATA_ARGENT_CHALLENGE) != SPECIAL)
                                            return;

                                        if (Creature* tirion = GetCreature(DATA_TIRION))
                                            tirion->AI()->Talk(SAY_TIRION_ARGENT_BEGIN);
                                        CrowdRoar();
                                        context.Schedule(6s, GROUP_STAGE, [this](TaskContext /*context*/)
                                        {
                                            if (GetBossState(DATA_ARGENT_CHALLENGE) == SPECIAL)
                                                StartSpectatorReactions();
                                        });
                                    });
                                });
                            });
                        });
                    }
                    else if (data == GOSSIP_OPTION_FINAL_CHALLENGE && GetBossState(DATA_ARGENT_CHALLENGE) == DONE && GetBossState(DATA_BLACK_KNIGHT) == NOT_STARTED)
                    {
                        SetBossState(DATA_BLACK_KNIGHT, SPECIAL);
                        _phase = PHASE_BLACK_KNIGHT_INTRO;

                        if (Creature* herald = GetCreature(DATA_HERALD))
                        {
                            herald->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                            herald->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                            herald->SetWalk(true);
                            herald->GetMotionMaster()->MovePoint(0, { .Destination = Position(744.765f, 628.513f, 411.172f), .GeneratePath = true, .FinalOrientation = 1.0647f, .Velocity = HeraldVelocity });
                        }

                        if (Creature* bunny = instance->GetCreature(_summonerBunnyGUID))
                            bunny->SummonCreatureGroup(SUMMON_GROUP_BLACK_KNIGHT);

                        _scheduler.Schedule(2s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                tirion->AI()->Talk(SAY_TIRION_BLACK_KNIGHT);

                            Creature* knight = GetCreature(DATA_BLACK_KNIGHT);
                            Creature* gryphon = GetCreature(DATA_BLACK_KNIGHT_GRYPHON);
                            if (knight && gryphon)
                                knight->CastSpell(gryphon, SPELL_RIDE_VEHICLE_HARDCODED, true);
                        })
                        .Schedule(9s, GROUP_STAGE, [this](TaskContext context)
                        {
                            if (Creature* herald = GetCreature(DATA_HERALD))
                                herald->AI()->Talk(SAY_HERALD_RAFTERS);

                            context.Schedule(3s, GROUP_STAGE, [this](TaskContext /*context*/)
                            {
                                Creature* herald = GetCreature(DATA_HERALD);
                                Creature* knight = GetCreature(DATA_BLACK_KNIGHT);
                                if (herald && knight)
                                    herald->CastSpell(knight, SPELL_FACE_BLACK_KNIGHT, false);

                                if (Creature* gryphon = GetCreature(DATA_BLACK_KNIGHT_GRYPHON))
                                    gryphon->AI()->DoAction(ACTION_START_FLIGHT);
                            });
                        });
                    }
                    break;
                case DATA_SUMMON_CHAMPION:
                {
                    Creature* bunny = instance->GetCreature(_summonerBunnyGUID);
                    GrandChampionInfo const& info = GetChampionInfo(data);
                    std::vector<TempSummonData> const* homeGroup = sObjectMgr->GetSummonGroup(NPC_SPECTATOR_BUNNY, SUMMONER_TYPE_CREATURE, info.SummonGroup);
                    if (!bunny || !homeGroup)
                        break;

                    bool const throughGate = _phase == PHASE_CHAMPIONS_INTRO_GATE;
                    std::list<TempSummon*> summons;
                    bunny->SummonCreatureGroup(throughGate ? info.GateSummonGroup : info.SummonGroup, &summons);

                    for (TempSummon* summon : summons)
                    {
                        auto home = std::find_if(homeGroup->begin(), homeGroup->end(), [summon](TempSummonData const& summonData)
                        {
                            return summonData.entry == summon->GetEntry();
                        });

                        if (home == homeGroup->end())
                            continue;

                        if (summon->GetEntry() == info.MountEntry)
                        {
                            Position const mountHome(home->pos.GetPositionX(), home->pos.GetPositionY(), home->pos.GetPositionZ(), home->pos.GetAbsoluteAngle(ArenaCenter));
                            summon->SetHomePosition(mountHome);
                            if (Creature* champion = instance->GetCreature(_championGUIDs[data]))
                                champion->SetHomePosition(mountHome);

                            if (throughGate)
                                summon->GetMotionMaster()->MovePoint(0, mountHome, true, mountHome.GetOrientation());
                            else
                                summon->SetFacingTo(mountHome.GetOrientation());
                            continue;
                        }

                        _waveGUIDs[data].push_back(summon->GetGUID());

                        float const centreAngle = home->pos.GetAbsoluteAngle(ArenaCenter);
                        float side = 0.0f;
                        if (summon->GetOrientation() > 4.73f)
                            side = -float(M_PI) / 2.0f;
                        else if (summon->GetOrientation() < 4.69f)
                            side = float(M_PI) / 2.0f;

                        Position const post(home->pos.GetPositionX() + (side != 0.0f ? 5.0f * std::cos(centreAngle + side) : 0.0f), home->pos.GetPositionY() + (side != 0.0f ? 5.0f * std::sin(centreAngle + side) : 0.0f), home->pos.GetPositionZ(), centreAngle);
                        summon->SetFacingTo(centreAngle);
                        summon->SetHomePosition(post);
                        if (throughGate)
                            summon->GetMotionMaster()->MovePoint(0, post, true, centreAngle);
                        else if (side != 0.0f)
                        {
                            summon->SetWalk(true);
                            summon->GetMotionMaster()->MovePoint(0, { .Destination = post, .GeneratePath = true, .FinalOrientation = centreAngle, .Velocity = 3.1f });
                        }
                    }

                    if (throughGate)
                        if (Creature* herald = GetCreature(DATA_HERALD))
                            herald->AI()->Talk(info.HeraldText);
                    break;
                }
                case DATA_RETAINER_WAVE:
                    if (_phase == PHASE_CHAMPIONS_PAUSED)
                        break;

                    if (_activeWave < 3)
                        EngageGroup(_waveGUIDs[_activeWave]);
                    else
                    {
                        EngageGroup({ _championGUIDs.begin(), _championGUIDs.end() });
                        CrowdRoar();
                    }
                    break;
                case DATA_MOUNTED_WIPE:
                    if (_phase != PHASE_CHAMPIONS_MOUNTED)
                        break;

                    _phase = PHASE_CHAMPIONS_PAUSED;
                    RestoreMounts();
                    if (Creature* herald = GetCreature(DATA_HERALD))
                        herald->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    break;
                case DATA_CHAMPION_UNHORSED:
                    if (_phase != PHASE_CHAMPIONS_MOUNTED || std::any_of(_championGUIDs.begin(), _championGUIDs.end(), [this](ObjectGuid const& guid)
                    {
                        Creature* champion = instance->GetCreature(guid);
                        return champion && (champion->GetVehicleBase() || champion->AI()->GetData(DATA_CHAMPION_MOUNTED));
                    }))
                        break;

                    _phase = PHASE_CHAMPIONS_FOOT_TRANSITION;

                    for (ObjectGuid const& guid : _playerMountGUIDs)
                    {
                        if (Creature* mount = instance->GetCreature(guid))
                            mount->DespawnOrUnsummon();
                    }
                    _playerMountGUIDs.clear();

                    for (ObjectGuid const& guid : _championGUIDs)
                    {
                        if (Creature* champion = instance->GetCreature(guid))
                            champion->AI()->DoAction(ACTION_LEAVE);
                    }

                    _scheduler.Schedule(10s, GROUP_STAGE, [this](TaskContext context)
                    {
                        for (ObjectGuid const& guid : _hostileMountGUIDs)
                        {
                            if (Creature* mount = instance->GetCreature(guid))
                                mount->DespawnOrUnsummon();
                        }
                        _hostileMountGUIDs.clear();
                        HandleGameObject(ObjectGuid::Empty, false, GetGameObject(DATA_NORTH_PORTCULLIS));

                        context.Schedule(12s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            Creature* bunny = instance->GetCreature(_summonerBunnyGUID);
                            if (!bunny)
                                return;

                            _phase = PHASE_CHAMPIONS_FOOT;
                            for (uint8 slot = 0; slot < 3; ++slot)
                            {
                                std::vector<TempSummonData> const* spots = sObjectMgr->GetSummonGroup(NPC_SPECTATOR_BUNNY, SUMMONER_TYPE_CREATURE, SUMMON_GROUP_FOOT_SLOT_1 + slot);
                                if (!spots)
                                    continue;

                                uint32 const entry = GetChampionInfo(slot).Entry;
                                auto spot = std::find_if(spots->begin(), spots->end(), [entry](TempSummonData const& summonData)
                                {
                                    return summonData.entry == entry;
                                });

                                if (spot != spots->end())
                                    bunny->SummonCreature(entry, spot->pos, spot->type, Milliseconds(spot->time));
                            }
                        });
                    });
                    break;
                case DATA_CHAMPION_DEFEATED:
                    if (++_kneelingChampions != 3 || GetBossState(DATA_GRAND_CHAMPIONS) == DONE)
                        break;

                    if (Creature* bunny = instance->GetCreature(_summonerBunnyGUID))
                        bunny->CastSpell(bunny, SPELL_GRAND_CHAMPIONS_CREDIT, true);
                    SetBossState(DATA_GRAND_CHAMPIONS, DONE);
                    _phase = PHASE_IDLE;

                    for (ObjectGuid const& guid : _championGUIDs)
                    {
                        if (Creature* champion = instance->GetCreature(guid))
                            champion->DespawnOrUnsummon(randtime(2s, 7s));
                    }

                    if (instance->IsHeroic())
                        instance->PermBindAllPlayers();

                    CrowdRoar();
                    RevealCache(DATA_CHAMPIONS_CACHE);
                    OfferNextChallenge(HeraldCenterSpot, 10s);

                    _scheduler.Schedule(7s, [this](TaskContext /*context*/)
                    {
                        StopSpectatorReactions();
                        if (Creature* tirion = GetCreature(DATA_TIRION))
                            tirion->AI()->Talk(SAY_TIRION_ARGENT_CHALLENGE);
                    });
                    break;
                case DATA_SUMMON_ARGENT_CHAMPION:
                    if (Creature* herald = GetCreature(DATA_HERALD))
                        herald->SummonCreatureGroup(_argentChampionEntry == NPC_EADRIC ? SUMMON_GROUP_EADRIC : SUMMON_GROUP_PALETRESS);
                    break;
                case DATA_ARGENT_CHAMPION_DEFEATED:
                    if (GetBossState(DATA_ARGENT_CHALLENGE) == DONE)
                        break;

                    if (_argentChampionEntry == NPC_EADRIC)
                        UpdateEncounterStateForSpellCast(SPELL_ARGENT_CHAMPION_CREDIT, nullptr);
                    SetBossState(DATA_ARGENT_CHALLENGE, DONE);
                    _phase = PHASE_IDLE;

                    if (instance->IsHeroic())
                        instance->PermBindAllPlayers();

                    StopSpectatorReactions();
                    _scheduler.Schedule(3s, [this](TaskContext /*context*/)
                    {
                        CrowdRoar();
                        RevealCache(_argentChampionEntry == NPC_EADRIC ? DATA_EADRIC_CACHE : DATA_CONFESSOR_CACHE);
                        OfferNextChallenge(HeraldArgentSpot, 14s);
                    });
                    break;
                case DATA_MEMORY_DIED:
                    _killedMemoryEntry = data;
                    break;
                case DATA_HAD_WORSE_FAILED:
                    _hadWorse = false;
                    break;
                case DATA_GRYPHON_LANDED:
                    _scheduler.Schedule(2500ms, GROUP_STAGE, [this](TaskContext context)
                    {
                        Creature* knight = GetCreature(DATA_BLACK_KNIGHT);
                        Creature* herald = GetCreature(DATA_HERALD);
                        if (!knight || !herald)
                            return;

                        knight->ExitVehicle();
                        knight->AI()->Talk(SAY_BLACK_KNIGHT_INTRO_1);

                        context.Schedule(1s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            Creature* knight = GetCreature(DATA_BLACK_KNIGHT);
                            Creature* herald = GetCreature(DATA_HERALD);
                            if (knight && herald)
                                knight->GetMotionMaster()->MovePoint(0, Position(747.785f, 632.490f, 411.414f), true, knight->GetAbsoluteAngle(herald));
                        })
                        .Schedule(3s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            Creature* knight = GetCreature(DATA_BLACK_KNIGHT);
                            Creature* herald = GetCreature(DATA_HERALD);
                            if (knight && herald)
                                knight->CastSpell(herald, SPELL_DEATHS_RESPITE_HERALD, false);
                        })
                        .Schedule(6s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                tirion->AI()->Talk(SAY_TIRION_MEANING);
                        })
                        .Schedule(7s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            Creature* knight = GetCreature(DATA_BLACK_KNIGHT);
                            Creature* herald = GetCreature(DATA_HERALD);
                            if (knight && herald)
                                knight->CastSpell(herald, SPELL_DEATHS_PUSH, true);

                            if (Creature* gryphon = GetCreature(DATA_BLACK_KNIGHT_GRYPHON))
                                gryphon->AI()->DoAction(ACTION_LEAVE);
                        })
                        .Schedule(9s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            if (Creature* herald = GetCreature(DATA_HERALD))
                            {
                                herald->RemoveAurasDueToSpell(SPELL_DEATHS_RESPITE_HERALD);
                                herald->CastSpell(herald, SPELL_HERALD_FEIGN_DEATH, true);
                            }
                        })
                        .Schedule(9s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            Creature* knight = GetCreature(DATA_BLACK_KNIGHT);
                            if (!knight)
                                return;

                            if (Creature* tirion = GetCreature(DATA_TIRION))
                                knight->SetFacingToObject(tirion);
                            knight->AI()->Talk(SAY_BLACK_KNIGHT_INTRO_2);
                        })
                        .Schedule(10s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            if (Creature* knight = GetCreature(DATA_BLACK_KNIGHT))
                            {
                                knight->SetWalk(true);
                                knight->GetMotionMaster()->MovePoint(0, Position(747.211f, 622.748f, 411.417f));
                            }
                        })
                        .Schedule(25s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            if (Creature* knight = GetCreature(DATA_BLACK_KNIGHT))
                                knight->AI()->Talk(SAY_BLACK_KNIGHT_INTRO_3);
                        })
                        .Schedule(28s, GROUP_STAGE, [this](TaskContext /*context*/)
                        {
                            if (instance->GetTeamIdInInstance() == TEAM_ALLIANCE)
                            {
                                if (Creature* varian = GetCreature(DATA_VARIAN))
                                    varian->AI()->Talk(SAY_VARIAN_BLACK_KNIGHT);
                            }
                            else if (Creature* garrosh = GetCreature(DATA_GARROSH))
                                garrosh->AI()->Talk(SAY_GARROSH_BLACK_KNIGHT);

                            _phase = PHASE_BLACK_KNIGHT;
                            if (Creature* knight = GetCreature(DATA_BLACK_KNIGHT))
                                knight->AI()->DoAction(ACTION_ENGAGE);
                        });
                    });
                    break;
                default:
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (state)
            {
                case IN_PROGRESS:
                    if (type != DATA_ARGENT_CHALLENGE)
                        break;

                    StopSpectatorReactions();
                    if (_argentChampionEntry == NPC_PALETRESS)
                        HandleGameObject(ObjectGuid::Empty, false, GetGameObject(DATA_NORTH_PORTCULLIS));
                    break;
                case FAIL:
                    _scheduler.CancelGroup(GROUP_STAGE);
                    HandleGameObject(ObjectGuid::Empty, false, GetGameObject(DATA_MAIN_GATE));

                    switch (type)
                    {
                        case DATA_GRAND_CHAMPIONS:
                            _kneelingChampions = 0;
                            for (ObjectGuid const& guid : _championGUIDs)
                            {
                                Creature* champion = instance->GetCreature(guid);
                                if (champion && champion->IsAlive())
                                    champion->AI()->DoAction(ACTION_RESET);
                            }
                            break;
                        case DATA_ARGENT_CHALLENGE:
                            _killedMemoryEntry = 0;
                            HandleGameObject(ObjectGuid::Empty, true, GetGameObject(DATA_NORTH_PORTCULLIS));
                            for (uint32 spellId : ArgentChallengePlayerAuras)
                            {
                                DoRemoveAurasDueToSpellOnPlayers(spellId);
                            }
                            break;
                        case DATA_BLACK_KNIGHT:
                            _hadWorse = true;
                            DoRemoveAurasDueToSpellOnPlayers(SPELL_MARKED_FOR_DEATH);
                            DoRemoveAurasDueToSpellOnPlayers(SPELL_MARKED_FOR_DEATH_HEROIC);
                            break;
                        default:
                            break;
                    }
                    break;
                case DONE:
                    if (type == DATA_ARGENT_CHALLENGE)
                        HandleGameObject(ObjectGuid::Empty, true, GetGameObject(DATA_NORTH_PORTCULLIS));
                    break;
                default:
                    break;
            }

            return true;
        }

        bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscValue1) override
        {
            if (criteriaId == CRITERIA_IVE_HAD_WORSE)
                return _hadWorse;

            for (std::pair<uint32, uint32> const& criteria : ConfessorCriteria)
            {
                if (criteria.first == criteriaId)
                    return criteria.second == _killedMemoryEntry;
            }

            for (ChampionCriteriaInfo const& criteria : ChampionCriteria)
            {
                if (criteria.CriteriaId == criteriaId)
                    return (!criteria.NormalOnly || !instance->IsHeroic()) && std::find(_championPicks.begin(), _championPicks.end(), criteria.Champion) != _championPicks.end();
            }

            switch (criteriaId)
            {
                case CRITERIA_VICTORIES_PALETRESS:
                case CRITERIA_VICTORIES_EADRIC:
                case CRITERIA_VICTORIES_BLACK_KNIGHT:
                    return !instance->IsHeroic();
                default:
                    break;
            }

            return InstanceScript::CheckAchievementCriteriaMeet(criteriaId, source, target, miscValue1);
        }

        void Update(uint32 diff) override
        {
            _scheduler.Update(diff);
        }

    private:
        GrandChampionInfo const& GetChampionInfo(uint8 slot) const
        {
            return GrandChampions[instance->GetTeamIdInInstance() == TEAM_ALLIANCE ? 0 : 1][_championPicks[slot]];
        }

        bool IsFootPhase() const
        {
            return _phase == PHASE_CHAMPIONS_FOOT_TRANSITION || _phase == PHASE_CHAMPIONS_FOOT;
        }

        bool AllDead(GuidVector const& guids) const
        {
            return std::none_of(guids.begin(), guids.end(), [this](ObjectGuid const& guid)
            {
                Creature* creature = instance->GetCreature(guid);
                return creature && creature->IsAlive();
            });
        }

        void EngageGroup(GuidVector const& guids)
        {
            for (ObjectGuid const& guid : guids)
            {
                if (Creature* creature = instance->GetCreature(guid))
                    creature->AI()->DoAction(ACTION_ENGAGE);
            }
        }

        void CrowdRoar()
        {
            if (Creature* bunny = instance->GetCreature(_crowdBunnyGUID))
                bunny->PlayDistanceSound(SOUND_CROWD_ROAR);

            LaunchFireworks();
            _scheduler.Schedule(5s, GROUP_STAGE, [this](TaskContext /*context*/)
            {
                LaunchFireworks();
            })
            .Schedule(6s, GROUP_STAGE, [this](TaskContext /*context*/)
            {
                if (Creature* bunny = instance->GetCreature(_crowdBunnyGUID))
                    bunny->PlayDistanceSound(SOUND_CROWD_CHEER);
            });
        }

        void StartSpectatorReactions()
        {
            if (Creature* bunny = instance->GetCreature(_crowdBunnyGUID))
                bunny->CastSpell(bunny, SPELL_SPECTATOR_BUNNY_AURA, true);
        }

        void StopSpectatorReactions()
        {
            if (Creature* bunny = instance->GetCreature(_crowdBunnyGUID))
                bunny->RemoveAurasDueToSpell(SPELL_SPECTATOR_BUNNY_AURA);
        }

        void LaunchFireworks()
        {
            Creature* bunny = instance->GetCreature(_crowdBunnyGUID);
            if (!bunny)
                return;

            for (Position const& spot : FireworkSpots)
            {
                bunny->SummonGameObject(Trinity::Containers::SelectRandomContainerElement(Fireworks), spot, QuaternionData::fromEulerAnglesZYX(0.0f, 0.0f, 0.0f), 3s);
            }
        }

        void RevealCache(uint32 cacheData)
        {
            if (GameObject* cache = GetGameObject(cacheData))
            {
                DoRespawnGameObject(cache->GetGUID(), 24h);
                _scheduler.Schedule(2s, [this, cacheData](TaskContext /*context*/)
                {
                    if (GameObject* cache = GetGameObject(cacheData))
                        cache->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                });
            }
        }

        void RestoreMounts()
        {
            instance->SpawnGroupSpawn(instance->GetTeamIdInInstance() == TEAM_ALLIANCE ? SPAWN_GROUP_MOUNTS_ALLIANCE : SPAWN_GROUP_MOUNTS_HORDE, true);
        }

        void OfferNextChallenge(Position const& spot, Milliseconds gossipDelay)
        {
            if (Creature* herald = GetCreature(DATA_HERALD))
            {
                herald->SetWalk(true);
                herald->GetMotionMaster()->MovePoint(0, { .Destination = spot, .GeneratePath = true, .FinalOrientation = spot.GetOrientation(), .Velocity = HeraldVelocity });
            }

            _scheduler.Schedule(gossipDelay, [this](TaskContext /*context*/)
            {
                if (Creature* herald = GetCreature(DATA_HERALD))
                {
                    herald->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                    herald->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                }
            });
        }

        TaskScheduler _scheduler;
        std::array<uint8, 3> _championPicks;
        uint32 _argentChampionEntry;
        std::array<ObjectGuid, 3> _championGUIDs;
        std::array<GuidVector, 3> _waveGUIDs;
        GuidVector _argentSoldierGUIDs;
        GuidVector _playerMountGUIDs;
        GuidVector _hostileMountGUIDs;
        GuidVector _summonGUIDs;
        ObjectGuid _summonerBunnyGUID;
        ObjectGuid _retainerBunnyGUID;
        ObjectGuid _crowdBunnyGUID;
        uint8 _activeWave;
        uint8 _kneelingChampions;
        uint32 _killedMemoryEntry;
        bool _hadWorse;
        uint8 _phase;
        bool _wiped;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_trial_of_the_champion_InstanceMapScript(map);
    }
};

void AddSC_instance_trial_of_the_champion()
{
    new instance_trial_of_the_champion();
}
