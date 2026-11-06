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

#include "ulduar.h"
#include "AreaBoundary.h"
#include "CreatureAI.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Item.h"
#include "LocalTransport.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"
#include "Vehicle.h"
#include "WorldStatePackets.h"

static BossBoundaryData const boundaries =
{
    { DATA_FLAME_LEVIATHAN, new RectangleBoundary(148.0f, 401.3f, -155.0f, 90.0f) },
    { DATA_IGNIS, new RectangleBoundary(495.0f, 680.0f, 90.0f, 400.0f) },
    { DATA_RAZORSCALE, new RectangleBoundary(370.0f, 810.0f, -542.0f, -55.0f) },
    { DATA_XT002, new RectangleBoundary(755.0f, 940.0f, -125.0f, 95.0f) },
    { DATA_ASSEMBLY_OF_IRON, new CircleBoundary(Position(1587.2f, 121.0f), 90.0) },
    { DATA_ALGALON, new CircleBoundary(Position(1632.668f, -307.7656f), 45.0) },
    { DATA_ALGALON, new ZRangeBoundary(410.0f, 470.0f) },
    { DATA_HODIR, new EllipseBoundary(Position(2001.5f, -240.0f), 50.0, 75.0) },
    // Thorim sets boundaries dynamically
    { DATA_FREYA, new RectangleBoundary(2094.6f, 2520.0f, -250.0f, 200.0f) },
    { DATA_BRIGHTLEAF, new RectangleBoundary(2094.6f, 2520.0f, -250.0f, 200.0f) },
    { DATA_IRONBRANCH, new RectangleBoundary(2094.6f, 2520.0f, -250.0f, 200.0f) },
    { DATA_STONEBARK, new RectangleBoundary(2094.6f, 2520.0f, -250.0f, 200.0f) },
    { DATA_MIMIRON, new CircleBoundary(Position(2744.0f, 2569.0f), 70.0) },
    { DATA_VEZAX, new RectangleBoundary(1740.0f, 1930.0f, 31.0f, 228.0f) },
    { DATA_YOGG_SARON, new CircleBoundary(Position(1980.42f, -27.68f), 105.0) }
};

static DoorData const doorData[] =
{
    { GO_LEVIATHAN_DOOR,                DATA_FLAME_LEVIATHAN,   DOOR_TYPE_ROOM },
    { GO_XT_002_DOOR,                   DATA_XT002,             DOOR_TYPE_ROOM },
    { GO_IRON_COUNCIL_DOOR,             DATA_ASSEMBLY_OF_IRON,  DOOR_TYPE_ROOM },
    { GO_ARCHIVUM_DOOR,                 DATA_ASSEMBLY_OF_IRON,  DOOR_TYPE_PASSAGE },
    { GO_KOLOGARN_DOOR,                 DATA_KOLOGARN,          DOOR_TYPE_ROOM },
    { GO_HODIR_ENTRANCE,                DATA_HODIR,             DOOR_TYPE_ROOM },
    { GO_HODIR_DOOR,                    DATA_HODIR,             DOOR_TYPE_PASSAGE },
    { GO_HODIR_ICE_DOOR,                DATA_HODIR,             DOOR_TYPE_PASSAGE },
    { GO_MIMIRON_DOOR_1,                DATA_MIMIRON,           DOOR_TYPE_ROOM },
    { GO_MIMIRON_DOOR_2,                DATA_MIMIRON,           DOOR_TYPE_ROOM },
    { GO_MIMIRON_DOOR_3,                DATA_MIMIRON,           DOOR_TYPE_ROOM },
    { GO_THORIM_ENCOUNTER_DOOR,         DATA_THORIM,            DOOR_TYPE_ROOM },
    { GO_ANCIENT_GATE_OF_THE_KEEPERS,   DATA_HODIR,             DOOR_TYPE_PASSAGE },
    { GO_ANCIENT_GATE_OF_THE_KEEPERS,   DATA_MIMIRON,           DOOR_TYPE_PASSAGE },
    { GO_ANCIENT_GATE_OF_THE_KEEPERS,   DATA_THORIM,            DOOR_TYPE_PASSAGE },
    { GO_ANCIENT_GATE_OF_THE_KEEPERS,   DATA_FREYA,             DOOR_TYPE_PASSAGE },
    { GO_VEZAX_DOOR,                    DATA_VEZAX,             DOOR_TYPE_PASSAGE },
    { GO_YOGG_SARON_DOOR,               DATA_YOGG_SARON,        DOOR_TYPE_ROOM },
    { GO_DOODAD_UL_SIGILDOOR_03,        DATA_ALGALON,           DOOR_TYPE_ROOM },
    { GO_DOODAD_UL_UNIVERSEFLOOR_01,    DATA_ALGALON,           DOOR_TYPE_ROOM },
    { GO_DOODAD_UL_UNIVERSEFLOOR_02,    DATA_ALGALON,           DOOR_TYPE_SPAWN_HOLE },
    { GO_DOODAD_UL_UNIVERSEGLOBE01,     DATA_ALGALON,           DOOR_TYPE_SPAWN_HOLE },
    { GO_DOODAD_UL_ULDUAR_TRAPDOOR_03,  DATA_ALGALON,           DOOR_TYPE_SPAWN_HOLE },
    { 0,                                0,                      DOOR_TYPE_ROOM },
};

MinionData const minionData[] =
{
    { NPC_STEELBREAKER,   DATA_ASSEMBLY_OF_IRON },
    { NPC_MOLGEIM,        DATA_ASSEMBLY_OF_IRON },
    { NPC_BRUNDIR,        DATA_ASSEMBLY_OF_IRON },
    { 0,                  0,                    }
};

ObjectData const creatureData[] =
{
    { NPC_FLAME_LEVIATHAN,          DATA_FLAME_LEVIATHAN          },
    { NPC_IGNIS,                    DATA_IGNIS                    },
    { NPC_RAZORSCALE,               DATA_RAZORSCALE               },
    { NPC_XT002,                    DATA_XT002                    },
    { NPC_KOLOGARN,                 DATA_KOLOGARN                 },
    { NPC_AURIAYA,                  DATA_AURIAYA                  },
    { NPC_HODIR,                    DATA_HODIR                    },
    { NPC_THORIM,                   DATA_THORIM                   },
    { NPC_FREYA,                    DATA_FREYA                    },
    { NPC_MIMIRON,                  DATA_MIMIRON                  },
    { NPC_VEZAX,                    DATA_VEZAX                    },
    { NPC_YOGG_SARON,               DATA_YOGG_SARON               },
    { NPC_ALGALON,                  DATA_ALGALON                  },

    { NPC_EXPEDITION_COMMANDER,     DATA_EXPEDITION_COMMANDER     },
    { NPC_RAZORSCALE_CONTROLLER,    DATA_RAZORSCALE_CONTROL       },
    { NPC_SIF,                      DATA_SIF                      },
    { NPC_RUNIC_COLOSSUS,           DATA_RUNIC_COLOSSUS           },
    { NPC_RUNE_GIANT,               DATA_RUNE_GIANT               },
    { NPC_THORIM_CONTROLLER,        DATA_THORIM_CONTROLLER        },
    { NPC_COMPUTER,                 DATA_COMPUTER                 },
    { NPC_WORLD_TRIGGER_MIMIRON,    DATA_MIMIRON_WORLD_TRIGGER    },
    { NPC_LEVIATHAN_MKII,           DATA_LEVIATHAN_MK_II          },
    { NPC_VX_001,                   DATA_VX_001                   },
    { NPC_AERIAL_COMMAND_UNIT,      DATA_AERIAL_COMMAND_UNIT      },
    { NPC_VOICE_OF_YOGG_SARON,      DATA_VOICE_OF_YOGG_SARON      },
    { NPC_SARA,                     DATA_SARA                     },
    { NPC_BRAIN_OF_YOGG_SARON,      DATA_BRAIN_OF_YOGG_SARON      },
    { NPC_BRANN_BRONZBEARD_ALG,     DATA_BRANN_BRONZEBEARD_ALG    },
    { NPC_BRANN_BRONZEBEARD_INTRO,  DATA_BRANN_BRONZEBEARD_INTRO  },
    { NPC_ARCHMAGE_PENTARUS,        DATA_ARCHMAGE_PENTARUS        },
    { NPC_ARCHMAGE_RHYDIAN,         DATA_ARCHMAGE_RHYDIAN         },
    { NPC_LORE_KEEPER_OF_NORGANNON, DATA_LORE_KEEPER_OF_NORGANNON },
    { NPC_HIGH_EXPLORER_DELLORAH,   DATA_DELLORAH                 },
    { NPC_BRONZEBEARD_RADIO,        DATA_BRONZEBEARD_RADIO        },
    { NPC_HEART_OF_DECONSTRUCTOR,   DATA_XT002_HEART              },
    { NPC_AZEROTH,                  DATA_AZEROTH                  },
    { 0,                            0,                            }
};

ObjectData const objectData[] =
{
    { GO_MIMIRON_ELEVATOR,             DATA_MIMIRON_ELEVATOR     },
    { GO_MIMIRON_BUTTON,               DATA_MIMIRON_BUTTON       },
    { GO_MIMIRON_TRAM,                 DATA_MIMIRON_TRAM                 },
    { GO_MIMIRON_ACTIVATE_TRAM,        DATA_MIMIRON_ACTIVATE_TRAM        },
    { GO_MIMIRON_TRAM_ROCKET_BOOSTER,  DATA_MIMIRON_TRAM_ROCKET_BOOSTER  },
    { GO_MIMIRON_CALL_TRAM_CENTER,     DATA_MIMIRON_CALL_TRAM_CENTER     },
    { GO_MIMIRON_CALL_TRAM_MIMIRON,    DATA_MIMIRON_CALL_TRAM_MIMIRON    },
    { GO_DOODAD_UL_TRAIN_TURNAROUND01, DATA_MIMIRON_TRAM_TURNAROUND_1    },
    { GO_DOODAD_UL_TRAIN_TURNAROUND02, DATA_MIMIRON_TRAM_TURNAROUND_2    },
    { GO_DOODAD_UL_UNIVERSEGLOBE01,    DATA_UNIVERSE_GLOBE       },
    { GO_DOODAD_UL_ULDUAR_TRAPDOOR_03, DATA_ALGALON_TRAPDOOR     },
    { GO_RAZOR_HARPOON_1,              GO_RAZOR_HARPOON_1        },
    { GO_RAZOR_HARPOON_2,              GO_RAZOR_HARPOON_2        },
    { GO_RAZOR_HARPOON_3,              GO_RAZOR_HARPOON_3        },
    { GO_RAZOR_HARPOON_4,              GO_RAZOR_HARPOON_4        },
    { GO_THORIM_LEVER,                 DATA_THORIM_LEVER         },
    { GO_THORIM_STONE_DOOR,            DATA_STONE_DOOR           },
    { GO_THORIM_RUNIC_DOOR,            DATA_RUNIC_DOOR           },
    { GO_DOODAD_UL_SIGILDOOR_01,       DATA_SIGILDOOR_01         },
    { GO_DOODAD_UL_SIGILDOOR_02,       DATA_SIGILDOOR_02         },
    { GO_DOODAD_UL_SIGILDOOR_03,       DATA_SIGILDOOR_03         },
    { GO_DOODAD_UL_UNIVERSEFLOOR_01,   DATA_UNIVERSE_FLOOR_01    },
    { GO_DOODAD_UL_UNIVERSEFLOOR_02,   DATA_UNIVERSE_FLOOR_02    },
    { GO_GIFT_OF_THE_OBSERVER_10,      DATA_GIFT_OF_THE_OBSERVER },
    { GO_GIFT_OF_THE_OBSERVER_25,      DATA_GIFT_OF_THE_OBSERVER },
    { 0,                               0                         }
};

static Position const BrannRadioSummonPos = { -312.553f, 294.34140f, 525.1342f, 5.408990f };
static Position const FlameLeviathanOutroFlyingMachineSpawn = { 166.760f, -273.302f, 499.799f, 1.43817f };
static Position const FlameLeviathanOutroFlyingMachineLand = { 246.4216f, -80.0379f, 416.2025f, 3.14159f };
static Position const FlameLeviathanOutroFlyingMachinePath[] =
{
    { 162.31393f, -298.57043f, 499.29520f, 0.0f },
    { 163.25363f, -298.22842f, 499.29520f, 0.0f },
    { 187.40060f, -142.13303f, 499.75803f, 0.0f },
    { 216.52350f, -102.91764f, 475.61917f, 0.0f },
    { 207.26460f,   -0.70204f, 460.25810f, 0.0f },
    { 201.75299f,   29.98020f, 465.31372f, 0.0f },
    { 231.98503f,   47.57292f, 459.28592f, 0.0f },
    { 247.23238f,   44.02615f, 459.17484f, 0.0f },
    { 253.05849f,   22.74127f, 446.11926f, 0.0f },
    { 255.55441f,  -23.08404f, 431.00824f, 0.0f },
    { 260.49127f,  -54.52697f, 421.70270f, 0.0f },
    { 246.42160f,  -80.03793f, 416.20250f, 0.0f }
};
static Position const FlameLeviathanOutroRhydianSpawn = { 235.560f, -136.188f, 409.651f, 1.78024f };
static Position const FlameLeviathanOutroRhydianWalkTo = { 244.500f,  -94.000f, 409.819f, 1.57080f };
static Position const FlameLeviathanOutroBrannWalkTo = { 244.500f,  -91.000f, 409.819f, 4.71239f };
static Position const FlameLeviathanHardmodeDellorahNearBrann = { -716.500f, -58.000f, 430.000f, 1.59000f };

static constexpr uint32 FlameLeviathanOutroSummonDespawnMs = 10 * MINUTE * IN_MILLISECONDS;

static constexpr uint32 MimironTramArrivalMimiron = 33200;
static constexpr uint32 MimironTramArrivalCenter = 101867;

enum UlduarBrannIntroTexts
{
    SAY_BRANN_INTRO_PENTARUS_YOU_HEARD = 0,
    SAY_BRANN_INTRO_LETS_MOVE_OUT,
    SAY_BRANN_INTRO_BRING_DOWN_SHIELD
};

enum UlduarPentarusIntroTexts
{
    SAY_PENTARUS_INTRO_OF_COURSE_BRANN = 0,
    SAY_PENTARUS_INTRO_MAGES_OF_KIRIN_TOR
};

static constexpr ObjectGuid::LowType UlduarShieldChannelMageSpawnIdEast = 136525;
static constexpr ObjectGuid::LowType UlduarShieldChannelMageSpawnIdNorthEast = 136528;

enum UlduarDellorahHardmodeTexts
{
    SAY_DELLORAH_HARDMODE_REACT = 7,  // "What... What did you just do, $n?! Brann! Braaaaannn!"
    SAY_DELLORAH_HARDMODE_WARN        // "Brann! $n just activated the orbital defense system!..."
};

struct FlameLeviathanGauntletProximityPoint
{
    Position pos;
    float radius;
    uint32 brannGroup;   // creature_text GroupID on Bronzebeard Radio (34054)
    uint32 firedMask;    // bit in _leviathanGauntletRadioFiredMask once played
    bool hardmodeOnly;   // tower warnings only fire under hardmode
};

enum FlameLeviathanGauntletRadioBits : uint32
{
    GAUNTLET_RADIO_IRON_DWARVES = 0x01,
    GAUNTLET_RADIO_GENERATORS   = 0x02,
    GAUNTLET_RADIO_TOWER_FROST  = 0x04,
    GAUNTLET_RADIO_TOWER_LIFE   = 0x08,
    GAUNTLET_RADIO_TOWER_FLAMES = 0x10,
    GAUNTLET_RADIO_TOWER_STORMS = 0x20,
    GAUNTLET_RADIO_REPAIR_PAD   = 0x40
};

static FlameLeviathanGauntletProximityPoint const FlGauntletProximityPoints[] =
{
    // Tower of Frost (GO 194370) - Hammer of Hodir
    { {  82.86f, -394.29f, 406.85f, 0.f }, 60.0f, SAY_BRANN_RADIO_GAUNTLET_TOWER_FROST,  GAUNTLET_RADIO_TOWER_FROST,  true  },
    // Tower of Life (GO 194375) - Freya
    { {-223.56f, -298.56f, 365.34f, 0.f }, 60.0f, SAY_BRANN_RADIO_GAUNTLET_TOWER_LIFE,   GAUNTLET_RADIO_TOWER_LIFE,   true  },
    // Tower of Flames (GO 194371) - Mimiron's Gaze
    { { -79.40f,   89.11f, 430.44f, 0.f }, 60.0f, SAY_BRANN_RADIO_GAUNTLET_TOWER_FLAMES, GAUNTLET_RADIO_TOWER_FLAMES, true  },
    // Tower of Storms (GO 194377) - Krolmir / Thorim
    { { 339.89f,  318.53f, 405.78f, 0.f }, 60.0f, SAY_BRANN_RADIO_GAUNTLET_TOWER_STORMS, GAUNTLET_RADIO_TOWER_STORMS, true  },
    // Repair pad south (GO 194261)
    { { 155.55f, -128.06f, 409.80f, 0.f }, 30.0f, SAY_BRANN_RADIO_GAUNTLET_REPAIR_PAD,   GAUNTLET_RADIO_REPAIR_PAD,   false },
    // Repair pad north (GO 194261)
    { { 163.52f,   56.11f, 409.80f, 0.f }, 30.0f, SAY_BRANN_RADIO_GAUNTLET_REPAIR_PAD,   GAUNTLET_RADIO_REPAIR_PAD,   false }
};

struct UlduarKeeperData
{
    uint32 BossId;
    uint32 ObservationRingEntry;
    uint32 YoggSaronEntry;
};

UlduarKeeperData const KeeperData[MAX_ULDUAR_KEEPERS] =
{
    { DATA_FREYA,   NPC_FREYA_OBSERVATION_RING,   NPC_FREYA_YS   },
    { DATA_HODIR,   NPC_HODIR_OBSERVATION_RING,   NPC_HODIR_YS   },
    { DATA_THORIM,  NPC_THORIM_OBSERVATION_RING,  NPC_THORIM_YS  },
    { DATA_MIMIRON, NPC_MIMIRON_OBSERVATION_RING, NPC_MIMIRON_YS }
};

UlduarKeeperDespawnEvent::UlduarKeeperDespawnEvent(Creature* owner, Milliseconds despawnTimerOffset) : _owner(owner), _despawnTimer(despawnTimerOffset)
{
}

bool UlduarKeeperDespawnEvent::Execute(uint64 /*eventTime*/, uint32 /*updateTime*/)
{
    _owner->CastSpell(_owner, SPELL_TELEPORT_KEEPER_VISUAL);
    _owner->DespawnOrUnsummon(1s + _despawnTimer);
    return true;
}

class instance_ulduar : public InstanceMapScript
{
    public:
        instance_ulduar() : InstanceMapScript(UlduarScriptName, 603) { }

        struct instance_ulduar_InstanceMapScript : public InstanceScript
        {
            instance_ulduar_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
            {
                SetHeaders(DataHeader);
                SetBossNumber(MAX_ENCOUNTER);
                LoadBossBoundaries(boundaries);
                LoadDoorData(doorData);
                LoadMinionData(minionData);
                LoadObjectData(creatureData, objectData);

                _algalonTimer = 61;
                _maxArmorItemLevel = 0;
                _maxWeaponItemLevel = 0;
                _teamInInstance = 0;
                _colossusData = 0;
                _elderCount = 0;
                _illusion = 0;
                _activatedKeeperMask = 0;
                _conSpeedAtory = false;
                _lumberjacked = false;
                _unbroken = true;
                _isDriveMeCrazyEligible = true;
                _algalonSummoned = false;
                _summonAlgalon = false;
                _couAchivePlayerDeathMask = 0;

                _activeTowers = false;
                _destroyedTowers = 0;
                _stunned = 1;
                _leviathanIntroCompleted = false;
                _leviathanIntroStarted = false;
                _leviathanGauntletRadioFiredMask = 0;
                _mimironTramUsed = false;
                _tramProgress = 0;
                _mimironEngaged = false;
            }

            void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override
            {
                packet.Worldstates.emplace_back(WORLD_STATE_ALGALON_TIMER_ENABLED, (_algalonTimer && _algalonTimer <= 60) ? 1 : 0);
                packet.Worldstates.emplace_back(WORLD_STATE_ALGALON_DESPAWN_TIMER, std::min<int32>(_algalonTimer, 60));
            }

            void OnPlayerEnter(Player* player) override
            {
                if (!_teamInInstance)
                    _teamInInstance = player->GetTeam();

                if (_summonAlgalon)
                {
                    _summonAlgalon = false;
                    TempSummon* algalon = instance->SummonCreature(NPC_ALGALON, AlgalonLandPos);
                    if (_algalonTimer && _algalonTimer <= 60)
                        algalon->AI()->DoAction(ACTION_INIT_ALGALON);
                    else
                        algalon->SetImmuneToPC(false);
                }

                VerifyKeeperState();
            }

            void OnCreatureCreate(Creature* creature) override
            {
                InstanceScript::OnCreatureCreate(creature);

                switch (creature->GetEntry())
                {
                    case NPC_WORLD_TRIGGER_MIMIRON:
                    case NPC_WORLD_TRIGGER:
                        creature->SetIsCombatDisallowed(true);
                        break;
                    case NPC_SALVAGED_DEMOLISHER:
                    case NPC_SALVAGED_SIEGE_ENGINE:
                    case NPC_SALVAGED_CHOPPER:
                        if (GetBossState(DATA_FLAME_LEVIATHAN) == DONE)
                            DespawnLeviatanVehicle(creature);
                        else
                            _leviathanVehicleGUIDs.push_back(creature->GetGUID());
                        break;
                    case NPC_BRANN_BRONZEBEARD_INTRO:
                        if (_leviathanIntroCompleted || _activeTowers || GetBossState(DATA_FLAME_LEVIATHAN) == DONE)
                            creature->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        break;
                    case NPC_LORE_KEEPER_OF_NORGANNON:
                        if (_activeTowers || GetBossState(DATA_FLAME_LEVIATHAN) == DONE)
                            creature->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        break;
                    case NPC_KIRIN_TOR_BATTLE_MAGE:
                        if (creature->GetSpawnId() == UlduarShieldChannelMageSpawnIdEast || creature->GetSpawnId() == UlduarShieldChannelMageSpawnIdNorthEast)
                            if (!_leviathanIntroCompleted)
                                _kirinTorMageGUIDs.push_back(creature->GetGUID());
                        break;

                    // XT-002 Deconstructor
                    case NPC_XT_TOY_PILE:
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            if (!_xtToyPileGUIDs[i])
                            {
                                _xtToyPileGUIDs[i] = creature->GetGUID();
                                break;
                            }
                        }
                        break;

                    // Assembly of Iron
                    case NPC_STEELBREAKER:
                        _assemblyGUIDs[0] = creature->GetGUID();
                        AddMinion(creature, true);
                        break;
                    case NPC_MOLGEIM:
                        _assemblyGUIDs[1] = creature->GetGUID();
                        AddMinion(creature, true);
                        break;
                    case NPC_BRUNDIR:
                        _assemblyGUIDs[2] = creature->GetGUID();
                        AddMinion(creature, true);
                        break;

                    // Freya
                    case NPC_BRIGHTLEAF:
                        _elderGUIDs[0] = creature->GetGUID();
                        if (GetBossState(DATA_FREYA) == DONE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_IRONBRANCH:
                        _elderGUIDs[1] = creature->GetGUID();
                        if (GetBossState(DATA_FREYA) == DONE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_STONEBARK:
                        _elderGUIDs[2] = creature->GetGUID();
                        if (GetBossState(DATA_FREYA) == DONE)
                            creature->DespawnOrUnsummon();
                        break;
                    case NPC_FREYA_ACHIEVE_TRIGGER:
                        _freyaAchieveTriggerGUID = creature->GetGUID();
                        break;

                    // Thorim
                    case NPC_THUNDER_ORB:
                        creature->SetCanFly(true);
                        creature->SetDisableGravity(true);
                        break;

                    // Yogg-Saron
                    case NPC_VOICE_OF_YOGG_SARON:
                        VerifyKeeperState();
                        break;
                    case NPC_FREYA_YS:
                    case NPC_HODIR_YS:
                    case NPC_THORIM_YS:
                    case NPC_MIMIRON_YS:
                    {
                        uint8 index = GetKeeperIndex(creature->GetEntry(), false);
                        if (index == MAX_ULDUAR_KEEPERS)
                            break;

                        _keeperGUIDs[index] = creature->GetGUID();
                        if (!(_activatedKeeperMask & (1 << index)))
                        {
                            _activatedKeeperMask |= 1 << index;
                            SaveToDB();
                        }
                        break;
                    }
                    case NPC_FREYA_OBSERVATION_RING:
                    case NPC_HODIR_OBSERVATION_RING:
                    case NPC_THORIM_OBSERVATION_RING:
                    case NPC_MIMIRON_OBSERVATION_RING:
                    {
                        uint8 index = GetKeeperIndex(creature->GetEntry(), true);
                        if (index == MAX_ULDUAR_KEEPERS)
                            break;

                        _observationRingKeeperGUIDs[index] = creature->GetGUID();
                        break;
                    }
                    case NPC_SANITY_WELL:
                        creature->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_GARONA:
                    case NPC_KING_LLANE:
                    case NPC_THE_LICH_KING:
                    case NPC_IMMOLATED_CHAMPION:
                    case NPC_YSERA:
                    case NPC_NELTHARION:
                    case NPC_MALYGOS:
                    case NPC_ALEXSTRASZA:
                    case NPC_TURNED_CHAMPION:
                        creature->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
                        break;

                    // Algalon
                    //! These creatures are summoned by something else than Algalon
                    //! but need to be controlled/despawned by him - so they need to be
                    //! registered in his summon list
                    case NPC_ALGALON_VOID_ZONE_VISUAL_STALKER:
                    case NPC_ALGALON_STALKER_ASTEROID_TARGET_01:
                    case NPC_ALGALON_STALKER_ASTEROID_TARGET_02:
                    case NPC_UNLEASHED_DARK_MATTER:
                        if (Creature* algalon = GetCreature(DATA_ALGALON))
                            algalon->AI()->JustSummoned(creature);
                        break;
                }
            }

            uint32 GetCreatureEntry(ObjectGuid::LowType /*guidLow*/, CreatureData const* data) override
            {
                if (!_teamInInstance)
                {
                    Map::PlayerList const& Players = instance->GetPlayers();
                    if (!Players.isEmpty())
                        if (Player* player = Players.begin()->GetSource())
                            _teamInInstance = player->GetTeam();
                }

                uint32 entry = data->id;
                switch (entry)
                {
                    case NPC_MERCENARY_CAPTAIN_H:
                        return _teamInInstance == HORDE ? NPC_MERCENARY_CAPTAIN_A : NPC_MERCENARY_CAPTAIN_H;
                    case NPC_MERCENARY_SOLDIER_H:
                        return _teamInInstance == HORDE ? NPC_MERCENARY_SOLDIER_A : NPC_MERCENARY_SOLDIER_H;
                    default:
                        return entry;
                }
            }

            void OnCreatureRemove(Creature* creature) override
            {
                InstanceScript::OnCreatureRemove(creature);

                switch (creature->GetEntry())
                {
                    case NPC_XT_TOY_PILE:
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            if (_xtToyPileGUIDs[i] == creature->GetGUID())
                            {
                                _xtToyPileGUIDs[i].Clear();
                                break;
                            }
                        }
                        break;
                    case NPC_STEELBREAKER:
                    case NPC_MOLGEIM:
                    case NPC_BRUNDIR:
                        AddMinion(creature, false);
                        break;
                    case NPC_FREYA_YS:
                    case NPC_HODIR_YS:
                    case NPC_THORIM_YS:
                    case NPC_MIMIRON_YS:
                        if (uint8 index = GetKeeperIndex(creature->GetEntry(), false); index != MAX_ULDUAR_KEEPERS)
                            _keeperGUIDs[index].Clear();
                        break;
                    case NPC_FREYA_OBSERVATION_RING:
                    case NPC_HODIR_OBSERVATION_RING:
                    case NPC_THORIM_OBSERVATION_RING:
                    case NPC_MIMIRON_OBSERVATION_RING:
                        if (uint8 index = GetKeeperIndex(creature->GetEntry(), true); index != MAX_ULDUAR_KEEPERS)
                            _observationRingKeeperGUIDs[index].Clear();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* gameObject) override
            {
                InstanceScript::OnGameObjectCreate(gameObject);

                switch (gameObject->GetEntry())
                {
                    case GO_ANCIENT_GATE_OF_THE_KEEPERS:
                        gameObject->SetFlag(GO_FLAG_NOT_SELECTABLE);
                        break;
                    case GO_KOLOGARN_CHEST_HERO:
                    case GO_KOLOGARN_CHEST:
                        _kologarnChestGUID = gameObject->GetGUID();
                        break;
                    case GO_KOLOGARN_BRIDGE:
                        _kologarnBridgeGUID = gameObject->GetGUID();
                        if (GetBossState(DATA_KOLOGARN) == DONE)
                            HandleGameObject(ObjectGuid::Empty, false, gameObject);
                        break;
                    case GO_THORIM_DARK_IRON_PORTCULLIS:
                        _thorimDarkIronPortcullisGUID = gameObject->GetGUID();
                        break;
                    case GO_THORIM_LEVER:
                        if (GetBossState(DATA_THORIM) != IN_PROGRESS)
                            gameObject->SetFlag(GO_FLAG_NOT_SELECTABLE);
                        break;
                    case GO_CACHE_OF_STORMS_10:
                    case GO_CACHE_OF_STORMS_25:
                        _cacheOfStormsGUID = gameObject->GetGUID();
                        break;
                    case GO_CACHE_OF_STORMS_HARDMODE_10:
                    case GO_CACHE_OF_STORMS_HARDMODE_25:
                        _cacheOfStormsHardmodeGUID = gameObject->GetGUID();
                        break;
                    case GO_HODIR_RARE_CACHE_OF_WINTER_HERO:
                    case GO_HODIR_RARE_CACHE_OF_WINTER:
                        _hodirRareCacheGUID = gameObject->GetGUID();
                        break;
                    case GO_HODIR_CHEST_HERO:
                    case GO_HODIR_CHEST:
                        _hodirChestGUID = gameObject->GetGUID();
                        break;
                    case GO_MIMIRON_TRAM:
                        if (GetBossState(DATA_MIMIRON) == DONE)
                            _mimironTramUsed = true;
                        break;
                    case GO_MIMIRON_TRAM_ROCKET_BOOSTER:
                    case GO_MIMIRON_ACTIVATE_TRAM:
                        if (LocalTransport* tram = GetMimironTram())
                            if (!gameObject->GetTransport())
                                tram->BoardScriptedPassenger(gameObject);
                        break;
                    case GO_LEVIATHAN_GATE:
                        _leviathanGateGUID = gameObject->GetGUID();
                        if (GetBossState(DATA_FLAME_LEVIATHAN) == DONE || _colossusData >= 2)
                            gameObject->SetGoState(GO_STATE_DESTROYED);
                        break;
                    case GO_LEVIATHAN_PROTECTIVE_BUBBLE:
                        _leviathanProtectiveBubbleGUID = gameObject->GetGUID();
                        if (_leviathanIntroCompleted || GetBossState(DATA_FLAME_LEVIATHAN) == DONE)
                            gameObject->DespawnOrUnsummon();
                        break;
                    case GO_LEVIATHAN_INTRO_COLLISION:
                        _leviathanIntroCollisionGUIDs.push_back(gameObject->GetGUID());
                        if (_leviathanIntroCompleted || GetBossState(DATA_FLAME_LEVIATHAN) == DONE)
                            gameObject->DespawnOrUnsummon();
                        break;
                    case GO_BRAIN_ROOM_DOOR_1:
                        _brainRoomDoorGUIDs[0] = gameObject->GetGUID();
                        break;
                    case GO_BRAIN_ROOM_DOOR_2:
                        _brainRoomDoorGUIDs[1] = gameObject->GetGUID();
                        break;
                    case GO_BRAIN_ROOM_DOOR_3:
                        _brainRoomDoorGUIDs[2] = gameObject->GetGUID();
                        break;
                    case GO_CELESTIAL_PLANETARIUM_ACCESS_10:
                    case GO_CELESTIAL_PLANETARIUM_ACCESS_25:
                        if (_algalonSummoned)
                            gameObject->SetFlag(GO_FLAG_IN_USE);
                        break;
                    case GO_DOODAD_UL_SIGILDOOR_01:
                    case GO_DOODAD_UL_SIGILDOOR_02:
                        if (_algalonSummoned)
                            gameObject->SetGoState(GO_STATE_ACTIVE);
                        break;
                    default:
                        break;
                }
            }

            void OnUnitDeath(Unit* unit) override
            {
                // Champion/Conqueror of Ulduar
                if (unit->GetTypeId() == TYPEID_PLAYER)
                {
                    for (uint8 i = 0; i < DATA_ALGALON; ++i)
                    {
                        if (GetBossState(i) == IN_PROGRESS)
                        {
                            _couAchivePlayerDeathMask |= (1 << i);
                            SaveToDB();
                        }
                    }
                }

                Creature* creature = unit->ToCreature();
                if (!creature)
                    return;

                switch (creature->GetEntry())
                {
                    case NPC_STEELFORGED_DEFFENDER:
                    case NPC_STEELFORGED_DEFENDER_GENERATOR_SUMMON:
                        if (_leviathanIntroCompleted && GetData(DATA_ACTIVE_TOWERS) != 0)
                            FireGauntletRadioWarning(SAY_BRANN_RADIO_GAUNTLET_GENERATORS, GAUNTLET_RADIO_GENERATORS);
                        break;
                    case NPC_CORRUPTED_SERVITOR:
                    case NPC_MISGUIDED_NYMPH:
                    case NPC_GUARDIAN_LASHER:
                    case NPC_FOREST_SWARMER:
                    case NPC_MANGROVE_ENT:
                    case NPC_IRONROOT_LASHER:
                    case NPC_NATURES_BLADE:
                    case NPC_GUARDIAN_OF_LIFE:
                        if (!_conSpeedAtory)
                        {
                            DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, CRITERIA_CON_SPEED_ATORY);
                            _conSpeedAtory = true;
                        }
                        break;
                    case NPC_IRONBRANCH:
                    case NPC_STONEBARK:
                    case NPC_BRIGHTLEAF:
                        if (!_lumberjacked)
                        {
                            DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, CRITERIA_LUMBERJACKED);
                            _lumberjacked = true;
                        }
                        break;
                    default:
                        break;
                }
            }

            void ProcessEvent(WorldObject* /*gameObject*/, uint32 eventId) override
            {
                switch (eventId)
                {
                    // Flame Leviathan's Tower Event triggers
                    case EVENT_TOWER_OF_STORM_DESTROYED:
                        _destroyedTowers |= ACTION_TOWER_OF_STORM_DESTROYED;
                        break;
                    case EVENT_TOWER_OF_FROST_DESTROYED:
                        _destroyedTowers |= ACTION_TOWER_OF_FROST_DESTROYED;
                        break;
                    case EVENT_TOWER_OF_FLAMES_DESTROYED:
                        _destroyedTowers |= ACTION_TOWER_OF_FLAMES_DESTROYED;
                        break;
                    case EVENT_TOWER_OF_LIFE_DESTROYED:
                        _destroyedTowers |= ACTION_TOWER_OF_LIFE_DESTROYED;
                        break;

                    // Hodir Event triggers
                    case EVENT_INITIAL_AGGRO_HODIR:
                        if (Creature* hodir = GetCreature(DATA_HODIR))
                            hodir->AI()->DoAction(ACTION_INITIAL_AGGRO_HODIR);
                        break;
                    case EVENT_CACHE_SHATTERED:
                        if (GetBossState(DATA_HODIR) != IN_PROGRESS)
                            break;
                        if (Creature* hodir = GetCreature(DATA_HODIR))
                            hodir->AI()->DoAction(ACTION_CACHE_SHATTERED);
                        if (GameObject* hodirRareCache = instance->GetGameObject(_hodirRareCacheGUID))
                            hodirRareCache->ActivateObject(GameObjectActions::Despawn);
                        break;
                    case EVENT_FLASH_FREEZE_FINISHED:
                        if (Creature* hodir = GetCreature(DATA_HODIR))
                            hodir->AI()->DoAction(ACTION_FLASH_FREEZE_FINISHED);
                        break;

                    // Yogg-Saron Event triggers
                    case EVENT_ACTIVATE_SANITY_WELL:
                        if (Creature* freya = instance->GetCreature(_keeperGUIDs[0]))
                            freya->AI()->DoAction(4/*ACTION_SANITY_WELLS*/);
                        break;
                    case EVENT_HODIRS_PROTECTIVE_GAZE_PROC:
                        if (Creature* hodir = instance->GetCreature(_keeperGUIDs[1]))
                            hodir->AI()->DoAction(5/*ACTION_FLASH_FREEZE*/);
                        break;

                    case EVENT_TRAM_PARKED_MIMIRON:
                        if (GameObject* activateTramButton = GetGameObject(DATA_MIMIRON_ACTIVATE_TRAM))
                            activateTramButton->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                        if (GameObject* callTramCenterButton = GetGameObject(DATA_MIMIRON_CALL_TRAM_CENTER))
                            callTramCenterButton->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                        break;
                    case EVENT_TRAM_PARKED_CENTER:
                        if (GameObject* activateTramButton = GetGameObject(DATA_MIMIRON_ACTIVATE_TRAM))
                            activateTramButton->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                        if (GameObject* callTramMimironButton = GetGameObject(DATA_MIMIRON_CALL_TRAM_MIMIRON))
                            callTramMimironButton->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_FLAME_LEVIATHAN:
                        if (state == DONE)
                        {
                            _events.ScheduleEvent(EVENT_DESPAWN_LEVIATHAN_VEHICLES, 5s);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_SPAWN, 10s);
                            _events.CancelEvent(EVENT_FL_GAUNTLET_RADIO_POLL);
                        }
                        else if (state == NOT_STARTED)
                        {
                            for (ObjectGuid const& vehicleGuid : _leviathanVehicleGUIDs)
                            {
                                if (Creature* vehicleCreature = instance->GetCreature(vehicleGuid))
                                    if (vehicleCreature->IsAlive())
                                    {
                                        vehicleCreature->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                                        vehicleCreature->DespawnOrUnsummon(0ms, 30s);
                                    }
                            }
                            ForceRespawnQueuedCreaturesByEntry({ NPC_SALVAGED_DEMOLISHER, NPC_SALVAGED_SIEGE_ENGINE, NPC_SALVAGED_CHOPPER });

                            _events.CancelEvent(EVENT_FL_GAUNTLET_RADIO_POLL);
                            if (_leviathanIntroCompleted)
                                _events.ScheduleEvent(EVENT_FL_GAUNTLET_RADIO_POLL, 3s);
                        }
                        else if (state == IN_PROGRESS)
                            _events.CancelEvent(EVENT_FL_GAUNTLET_RADIO_POLL);
                        break;
                    case DATA_IGNIS:
                        if (state == NOT_STARTED)
                            ForceRespawnQueuedCreaturesByEntry({ NPC_IRON_CONSTRUCT });
                        break;
                    case DATA_RAZORSCALE:
                    case DATA_XT002:
                        break;
                    case DATA_ASSEMBLY_OF_IRON:
                        if (state == NOT_STARTED)
                            ForceRespawnQueuedCreaturesByEntry({ NPC_STEELBREAKER, NPC_MOLGEIM, NPC_BRUNDIR });
                        break;
                    case DATA_AURIAYA:
                    case DATA_VEZAX:
                    case DATA_YOGG_SARON:
                        break;
                    case DATA_MIMIRON:
                        if (state == DONE)
                            VerifyKeeperState();
                        else if (state == IN_PROGRESS)
                            _mimironEngaged = true;
                        break;
                    case DATA_FREYA:
                        if (state == DONE)
                            VerifyKeeperState();
                        break;
                    case DATA_IRONBRANCH:
                    case DATA_STONEBARK:
                    case DATA_BRIGHTLEAF:
                        if (GetBossState(DATA_BRIGHTLEAF) == DONE && GetBossState(DATA_IRONBRANCH) == DONE && GetBossState(DATA_STONEBARK) == DONE && GetBossState(DATA_FREYA) != DONE)
                            if (Creature* trigger = instance->GetCreature(_freyaAchieveTriggerGUID))
                                trigger->CastSpell(trigger, SPELL_LUMBERJACKED_CREDIT, true);
                        break;
                    case DATA_KOLOGARN:
                        if (state == DONE)
                        {
                            if (GameObject* gameObject = instance->GetGameObject(_kologarnChestGUID))
                            {
                                gameObject->SetRespawnTime(gameObject->GetRespawnDelay());
                                gameObject->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
                            }
                            HandleGameObject(_kologarnBridgeGUID, false);
                        }
                        break;
                    case DATA_HODIR:
                        if (state == DONE)
                        {
                            if (GameObject* hodirRareCache = instance->GetGameObject(_hodirRareCacheGUID))
                                hodirRareCache->ActivateObject(GameObjectActions::MakeActive);
                            if (GameObject* hodirChest = instance->GetGameObject(_hodirChestGUID))
                                hodirChest->ActivateObject(GameObjectActions::MakeActive);

                            VerifyKeeperState();
                        }
                        else if (state == FAIL)
                            RestoreHodirRareCache();
                        break;
                    case DATA_THORIM:
                        if (state == DONE)
                        {
                            if (Creature* thorim = GetCreature(DATA_THORIM))
                            {
                                if (GameObject* cache = instance->GetGameObject(thorim->AI()->GetData(DATA_THORIM_HARDMODE) ? _cacheOfStormsHardmodeGUID : _cacheOfStormsGUID))
                                {
                                    cache->SetLootRecipient(thorim->GetLootRecipient());
                                    cache->SetRespawnTime(cache->GetRespawnDelay());
                                    cache->RemoveFlag(GO_FLAG_LOCKED | GO_FLAG_NOT_SELECTABLE | GO_FLAG_NODESPAWN);
                                }
                            }

                            VerifyKeeperState();
                        }
                        else
                        {
                            DoCloseDoorOrButton(GetGuidData(DATA_THORIM_LEVER));
                            DoCloseDoorOrButton(_thorimDarkIronPortcullisGUID);
                        }
                        break;
                    case DATA_ALGALON:
                        if (state == DONE)
                        {
                            _events.CancelEvent(EVENT_UPDATE_ALGALON_TIMER);
                            _events.CancelEvent(EVENT_DESPAWN_ALGALON);
                            DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 0);
                            _algalonTimer = 61;
                            if (GameObject* gift = GetGameObject(DATA_GIFT_OF_THE_OBSERVER))
                                gift->SetRespawnTime(gift->GetRespawnDelay());
                            // get item level (recheck weapons)
                            Map::PlayerList const& players = instance->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                                if (Player* player = itr->GetSource())
                                    for (uint8 slot = EQUIPMENT_SLOT_MAINHAND; slot <= EQUIPMENT_SLOT_RANGED; ++slot)
                                        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                                            if (item->GetItemLevel() > _maxWeaponItemLevel)
                                                _maxWeaponItemLevel = item->GetItemLevel();
                        }
                        else if (state == IN_PROGRESS)
                        {
                            // get item level (armor cannot be swapped in combat)
                            Map::PlayerList const& players = instance->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                            {
                                if (Player* player = itr->GetSource())
                                {
                                    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                                    {
                                        if (slot == EQUIPMENT_SLOT_TABARD || slot == EQUIPMENT_SLOT_BODY)
                                            continue;

                                        if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                                        {
                                            if (slot >= EQUIPMENT_SLOT_MAINHAND && slot <= EQUIPMENT_SLOT_RANGED)
                                            {
                                                if (item->GetItemLevel() > _maxWeaponItemLevel)
                                                    _maxWeaponItemLevel = item->GetItemLevel();
                                            }
                                            else if (item->GetItemLevel() > _maxArmorItemLevel)
                                                _maxArmorItemLevel = item->GetItemLevel();
                                        }
                                    }
                                }
                            }
                        }
                        break;
                }

                return true;
            }

            void SetData(uint32 type, uint32 data) override
            {
                switch (type)
                {
                    case DATA_COLOSSUS:
                        _colossusData = data;
                        if (data >= 2 && GetBossState(DATA_FLAME_LEVIATHAN) == NOT_STARTED)
                        {
                            Creature* radio = GetCreature(DATA_BRONZEBEARD_RADIO);
                            if (!radio)
                                radio = instance->SummonCreature(NPC_BRONZEBEARD_RADIO, BrannRadioSummonPos);
                            if (radio)
                            {
                                radio->AI()->Talk(SAY_BRANN_RADIO_LEVIATHAN);
                                if (GetBossState(DATA_FLAME_LEVIATHAN) != DONE)
                                    _events.ScheduleEvent(EVENT_BRANN_RADIO, 8s);
                            }
                        }
                        break;
                    case DATA_UNBROKEN:
                        _unbroken = data != 0;
                        break;
                    case DATA_THORIM_PREADD_SUMMONS:
                        DespawnThorimSummons(_thorimPreAddGUIDs);
                        break;
                    case DATA_ILLUSION:
                        _illusion = data;
                        break;
                    case DATA_DRIVE_ME_CRAZY:
                        _isDriveMeCrazyEligible = data ? true : false;
                        break;
                    case DATA_VERIFY_YS_KEEPERS:
                        VerifyKeeperState();
                        break;
                    case EVENT_DESPAWN_ALGALON:
                        DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 1);
                        DoUpdateWorldState(WORLD_STATE_ALGALON_DESPAWN_TIMER, 60);
                        _algalonTimer = 60;
                        _events.ScheduleEvent(EVENT_DESPAWN_ALGALON, 1h);
                        _events.ScheduleEvent(EVENT_UPDATE_ALGALON_TIMER, 1min);
                        break;
                    case DATA_ALGALON_SUMMON_STATE:
                        _algalonSummoned = true;
                        break;
                    case DATA_ACTIVE_TOWERS:
                        _activeTowers = data ? true : false;
                        break;
                    case DATA_STUNNED:
                        _stunned = data;
                        break;
                    case DATA_FL_OUTRO_FLYING_MACHINE_LANDED:
                        if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                        {
                            brann->ExitVehicle();
                            brann->NearTeleportTo(FlameLeviathanOutroFlyingMachineLand.GetPositionX(), FlameLeviathanOutroFlyingMachineLand.GetPositionY(), 409.747f, FlameLeviathanOutroFlyingMachineLand.GetOrientation());
                            brann->SetWalk(true);
                            brann->GetMotionMaster()->MovePoint(POINT_FL_OUTRO_BRANN_WALK_TO_RHYDIAN, FlameLeviathanOutroBrannWalkTo);
                        }
                        _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_1, 6500ms);
                        break;
                    case DATA_FL_INTRO_START:
                        if (_leviathanIntroStarted || _leviathanIntroCompleted)
                            break;
                        _leviathanIntroStarted = true;
                        _events.ScheduleEvent(EVENT_FL_INTRO_LINE_1, 1s);
                        break;
                    case DATA_FL_HARDMODE_CONFIRMED:
                        _events.ScheduleEvent(EVENT_FL_HARDMODE_LINE_DEACTIVATING, 14s);
                        _events.ScheduleEvent(EVENT_FL_HARDMODE_NORGANNON_DESPAWN, 18s);
                        _events.ScheduleEvent(EVENT_FL_HARDMODE_BRANN_RADIO_WARN_1, 4s);
                        _events.ScheduleEvent(EVENT_FL_HARDMODE_DELLORAH_YELL_1, 20s);
                        _events.ScheduleEvent(EVENT_FL_HARDMODE_DELLORAH_RUN, 21s);
                        _events.ScheduleEvent(EVENT_FL_HARDMODE_DELLORAH_YELL_2, 37s);
                        _events.ScheduleEvent(EVENT_FL_HARDMODE_BRANN_RESPOND, 40s);
                        _events.ScheduleEvent(EVENT_FL_INTRO_SHIELD_DOWN, 44s);
                        break;
                    case DATA_CALL_TRAM:
                        if (LocalTransport* tram = GetMimironTram())
                        {
                            // data 0 = call to center (depart Mimiron): only valid parked at Mimiron (ACTIVE, pauseAtTime)
                            if (data == 0 && tram->GetGoState() == GO_STATE_ACTIVE && tram->GetPathProgress() == tram->GetPauseTime())
                            {
                                tram->SetGoState(GO_STATE_READY);
                                if (GameObject* rocketBooster = GetGameObject(DATA_MIMIRON_TRAM_ROCKET_BOOSTER))
                                    rocketBooster->SetGoState(GO_STATE_ACTIVE);
                                if (GameObject* activateTramButton = GetGameObject(DATA_MIMIRON_ACTIVATE_TRAM))
                                    activateTramButton->SetFlag(GO_FLAG_NOT_SELECTABLE);
                                if (GameObject* callTramCenterButton = GetGameObject(DATA_MIMIRON_CALL_TRAM_CENTER))
                                    callTramCenterButton->SetFlag(GO_FLAG_NOT_SELECTABLE);
                            }
                            // data 1 = call to Mimiron (depart center): only valid parked at center (READY, progress 0)
                            if (data == 1 && tram->GetGoState() == GO_STATE_READY && tram->GetPathProgress() == 0)
                            {
                                tram->SetGoState(GO_STATE_ACTIVE);
                                if (GameObject* rocketBooster = GetGameObject(DATA_MIMIRON_TRAM_ROCKET_BOOSTER))
                                    rocketBooster->SetGoState(GO_STATE_ACTIVE);
                                if (GameObject* activateTramButton = GetGameObject(DATA_MIMIRON_ACTIVATE_TRAM))
                                    activateTramButton->SetFlag(GO_FLAG_NOT_SELECTABLE);
                                if (GameObject* callTramMimironButton = GetGameObject(DATA_MIMIRON_CALL_TRAM_MIMIRON))
                                    callTramMimironButton->SetFlag(GO_FLAG_NOT_SELECTABLE);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }

            void SetGuidData(uint32 type, ObjectGuid data) override
            {
                switch (type)
                {
                    case DATA_FL_INTRO_PLAYER:
                        _leviathanIntroPlayerGUID = data;
                        break;
                    case DATA_THORIM_PREADD_SUMMONS:
                        _thorimPreAddGUIDs.push_back(data);
                        break;
                    default:
                        break;
                }
            }

            ObjectGuid GetGuidData(uint32 data) const override
            {
                switch (data)
                {
                    // XT-002 Deconstructor
                    case DATA_TOY_PILE_0:
                    case DATA_TOY_PILE_1:
                    case DATA_TOY_PILE_2:
                    case DATA_TOY_PILE_3:
                        return _xtToyPileGUIDs[data - DATA_TOY_PILE_0];

                    // Assembly of Iron
                    case DATA_STEELBREAKER:
                        return _assemblyGUIDs[0];
                    case DATA_MOLGEIM:
                        return _assemblyGUIDs[1];
                    case DATA_BRUNDIR:
                        return _assemblyGUIDs[2];

                    // Freya
                    case DATA_BRIGHTLEAF:
                        return _elderGUIDs[0];
                    case DATA_IRONBRANCH:
                        return _elderGUIDs[1];
                    case DATA_STONEBARK:
                        return _elderGUIDs[2];

                    // Yogg-Saron
                    case GO_BRAIN_ROOM_DOOR_1:
                        return _brainRoomDoorGUIDs[0];
                    case GO_BRAIN_ROOM_DOOR_2:
                        return _brainRoomDoorGUIDs[1];
                    case GO_BRAIN_ROOM_DOOR_3:
                        return _brainRoomDoorGUIDs[2];
                    case DATA_FREYA_YS:
                        return _keeperGUIDs[KEEPER_INDEX_FREYA];
                    case DATA_HODIR_YS:
                        return _keeperGUIDs[KEEPER_INDEX_HODIR];
                    case DATA_THORIM_YS:
                        return _keeperGUIDs[KEEPER_INDEX_THORIM];
                    case DATA_MIMIRON_YS:
                        return _keeperGUIDs[KEEPER_INDEX_MIMIRON];
                }

                return InstanceScript::GetGuidData(data);
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_MIMIRON:
                        return _mimironEngaged ? 1 : 0;
                    case DATA_COLOSSUS:
                        return _colossusData;
                    case DATA_UNBROKEN:
                        return uint32(_unbroken);
                    case DATA_ILLUSION:
                        return _illusion;
                    case DATA_KEEPERS_COUNT:
                        return GetActivatedKeeperCount();
                    case WORLD_STATE_ULDUAR_TEAM_IN_INSTANCE:
                        switch (_teamInInstance)
                        {
                            default: return 0;
                            case ALLIANCE: return 1;
                            case HORDE: return 2;
                        }
                        break;
                    case DATA_ACTIVE_TOWERS:
                        return _activeTowers ? 1 : 0;
                    case DATA_DESTROYED_TOWERS:
                        return _destroyedTowers;
                    case DATA_FL_INTRO_START:
                        return (_leviathanIntroStarted || _leviathanIntroCompleted) ? 1 : 0;
                    case DATA_FL_INTRO_COMPLETED:
                        return _leviathanIntroCompleted ? 1 : 0;
                    case DATA_CALL_TRAM:
                        return _mimironTramUsed ? 1 : 0;
                    default:
                        break;
                }

                return 0;
            }

            bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const*, Unit const* /* = nullptr */, uint32 /* = 0 */) override
            {
                switch (criteriaId)
                {
                    case CRITERIA_HERALD_OF_TITANS:
                        return _maxArmorItemLevel <= MAX_HERALD_ARMOR_ITEMLEVEL && _maxWeaponItemLevel <= MAX_HERALD_WEAPON_ITEMLEVEL;
                    case CRITERIA_WAITS_DREAMING_STORMWIND_25:
                    case CRITERIA_WAITS_DREAMING_STORMWIND_10:
                        return _illusion == STORMWIND_ILLUSION;
                    case CRITERIA_WAITS_DREAMING_CHAMBER_25:
                    case CRITERIA_WAITS_DREAMING_CHAMBER_10:
                        return _illusion == CHAMBER_ILLUSION;
                    case CRITERIA_WAITS_DREAMING_ICECROWN_25:
                    case CRITERIA_WAITS_DREAMING_ICECROWN_10:
                        return _illusion == ICECROWN_ILLUSION;
                    case CRITERIA_DRIVE_ME_CRAZY_10:
                    case CRITERIA_DRIVE_ME_CRAZY_25:
                        return _isDriveMeCrazyEligible;
                    case CRITERIA_THREE_LIGHTS_IN_THE_DARKNESS_10:
                    case CRITERIA_THREE_LIGHTS_IN_THE_DARKNESS_25:
                        return GetActivatedKeeperCount() <= 3;
                    case CRITERIA_TWO_LIGHTS_IN_THE_DARKNESS_10:
                    case CRITERIA_TWO_LIGHTS_IN_THE_DARKNESS_25:
                        return GetActivatedKeeperCount() <= 2;
                    case CRITERIA_ONE_LIGHT_IN_THE_DARKNESS_10:
                    case CRITERIA_ONE_LIGHT_IN_THE_DARKNESS_25:
                        return GetActivatedKeeperCount() <= 1;
                    case CRITERIA_ALONE_IN_THE_DARKNESS_10:
                    case CRITERIA_ALONE_IN_THE_DARKNESS_25:
                    case REALM_FIRST_DEATHS_DEMISE:
                        return GetActivatedKeeperCount() == 0;
                    case CRITERIA_C_O_U_LEVIATHAN_10:
                    case CRITERIA_C_O_U_LEVIATHAN_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_FLAME_LEVIATHAN)) == 0;
                    case CRITERIA_C_O_U_IGNIS_10:
                    case CRITERIA_C_O_U_IGNIS_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_IGNIS)) == 0;
                    case CRITERIA_C_O_U_RAZORSCALE_10:
                    case CRITERIA_C_O_U_RAZORSCALE_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_RAZORSCALE)) == 0;
                    case CRITERIA_C_O_U_XT002_10:
                    case CRITERIA_C_O_U_XT002_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_XT002)) == 0;
                    case CRITERIA_C_O_U_IRON_COUNCIL_10:
                    case CRITERIA_C_O_U_IRON_COUNCIL_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_ASSEMBLY_OF_IRON)) == 0;
                    case CRITERIA_C_O_U_KOLOGARN_10:
                    case CRITERIA_C_O_U_KOLOGARN_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_KOLOGARN)) == 0;
                    case CRITERIA_C_O_U_AURIAYA_10:
                    case CRITERIA_C_O_U_AURIAYA_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_AURIAYA)) == 0;
                    case CRITERIA_C_O_U_HODIR_10:
                    case CRITERIA_C_O_U_HODIR_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_HODIR)) == 0;
                    case CRITERIA_C_O_U_THORIM_10:
                    case CRITERIA_C_O_U_THORIM_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_THORIM)) == 0;
                    case CRITERIA_C_O_U_FREYA_10:
                    case CRITERIA_C_O_U_FREYA_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_FREYA)) == 0;
                    case CRITERIA_C_O_U_MIMIRON_10:
                    case CRITERIA_C_O_U_MIMIRON_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_MIMIRON)) == 0;
                    case CRITERIA_C_O_U_VEZAX_10:
                    case CRITERIA_C_O_U_VEZAX_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_VEZAX)) == 0;
                    case CRITERIA_C_O_U_YOGG_SARON_10:
                    case CRITERIA_C_O_U_YOGG_SARON_25:
                        return (_couAchivePlayerDeathMask & (1 << DATA_YOGG_SARON)) == 0;
                    case CRITERIA_CANT_DO_THAT_WHILE_STUNNED_10_SB:
                    case CRITERIA_CANT_DO_THAT_WHILE_STUNNED_25_SB:
                        if (Creature* stormcaller = instance->GetCreature(_assemblyGUIDs[2]))
                            return _stunned == 1 && stormcaller->AI()->GetData(1 /*DATA_PHASE_3*/) == 1;
                        break;
                    case CRITERIA_CANT_DO_THAT_WHILE_STUNNED_10_S:
                    case CRITERIA_CANT_DO_THAT_WHILE_STUNNED_25_S:
                        if (Creature* steelbreaker = instance->GetCreature(_assemblyGUIDs[0]))
                            return _stunned == 1 && steelbreaker->AI()->GetData(1 /*DATA_PHASE_3*/) == 1;
                        break;
                    case CRITERIA_CANT_DO_THAT_WHILE_STUNNED_10_RM:
                    case CRITERIA_CANT_DO_THAT_WHILE_STUNNED_25_RM:
                        if (Creature* runeMaster = instance->GetCreature(_assemblyGUIDs[1]))
                            return _stunned == 1 && runeMaster->AI()->GetData(1 /*DATA_PHASE_3*/) == 1;
                        break;
                }

                return false;
            }

            void WriteSaveDataMore(std::ostringstream& data) override
            {
                data << _colossusData << ' ' << _algalonTimer << ' ' << uint32(_algalonSummoned ? 1 : 0);

                for (uint8 i = 0; i < MAX_ULDUAR_KEEPERS; ++i)
                    data << ' ' << uint32((_activatedKeeperMask & (1 << i)) ? 1 : 0);

                data << ' ' << _couAchivePlayerDeathMask;
                data << ' ' << uint32(_activeTowers ? 1 : 0);
                data << ' ' << _destroyedTowers;
                data << ' ' << uint32(_leviathanIntroCompleted ? 1 : 0);
                data << ' ' << uint32(_mimironEngaged ? 1 : 0);
            }

            void ReadSaveDataMore(std::istringstream& data) override
            {
                uint32 tempState;
                data >> tempState;
                SetData(DATA_COLOSSUS, tempState);

                data >> _algalonTimer;
                data >> tempState;
                _algalonSummoned = tempState != 0;
                if (_algalonSummoned && GetBossState(DATA_ALGALON) != DONE)
                {
                    _summonAlgalon = true;
                    if (_algalonTimer && _algalonTimer <= 60)
                    {
                        _events.ScheduleEvent(EVENT_UPDATE_ALGALON_TIMER, 1min);
                        DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 1);
                        DoUpdateWorldState(WORLD_STATE_ALGALON_DESPAWN_TIMER, _algalonTimer);
                    }
                }

                for (uint8 i = 0; i < MAX_ULDUAR_KEEPERS; ++i)
                {
                    data >> tempState;
                    if (tempState != 0)
                        _activatedKeeperMask |= 1 << i;
                }

                data >> _couAchivePlayerDeathMask;

                data >> tempState;
                _activeTowers = tempState != 0;

                data >> _destroyedTowers;

                uint32 introCompletedTemp = 0;
                data >> introCompletedTemp;
                _leviathanIntroCompleted = introCompletedTemp != 0;

                uint32 mimironEngagedTemp = 0;
                data >> mimironEngagedTemp;
                _mimironEngaged = mimironEngagedTemp != 0;

                if (GetBossState(DATA_FLAME_LEVIATHAN) == NOT_STARTED)
                    ForceRespawnQueuedCreaturesByEntry({ NPC_SALVAGED_DEMOLISHER, NPC_SALVAGED_SIEGE_ENGINE, NPC_SALVAGED_CHOPPER });
                if (GetBossState(DATA_ASSEMBLY_OF_IRON) == NOT_STARTED)
                    ForceRespawnQueuedCreaturesByEntry({ NPC_STEELBREAKER, NPC_MOLGEIM, NPC_BRUNDIR });
                if (GetBossState(DATA_HODIR) != DONE)
                    RestoreHodirRareCache();
            }

            void Update(uint32 diff) override
            {
                UpdateMimironTramArrival();

                if (_events.Empty())
                    return;

                _events.Update(diff);

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UPDATE_ALGALON_TIMER:
                            SaveToDB();
                            DoUpdateWorldState(WORLD_STATE_ALGALON_DESPAWN_TIMER, --_algalonTimer);
                            if (_algalonTimer)
                                _events.ScheduleEvent(EVENT_UPDATE_ALGALON_TIMER, 1min);
                            else
                            {
                                DoUpdateWorldState(WORLD_STATE_ALGALON_TIMER_ENABLED, 0);
                                _events.CancelEvent(EVENT_UPDATE_ALGALON_TIMER);
                                if (Creature* algalon = GetCreature(DATA_ALGALON))
                                    algalon->AI()->DoAction(EVENT_DESPAWN_ALGALON);
                            }
                            break;
                        case EVENT_DESPAWN_LEVIATHAN_VEHICLES:
                            // Eject all players from vehicles and make them untargetable.
                            // They will be despawned after a while
                            for (auto const& vehicleGuid : _leviathanVehicleGUIDs)
                                if (Creature* vehicleCreature = instance->GetCreature(vehicleGuid))
                                    DespawnLeviatanVehicle(vehicleCreature);
                            break;
                        case EVENT_BRANN_RADIO:
                            if (Creature* radio = GetCreature(DATA_BRONZEBEARD_RADIO))
                                radio->AI()->Talk(SAY_BRANN_RADIO_LEVIATHAN2);
                            _events.ScheduleEvent(EVENT_BRANN_RADIO2, 5s);
                            break;
                        case EVENT_BRANN_RADIO2:
                            if (Creature* radio = GetCreature(DATA_BRONZEBEARD_RADIO))
                                radio->AI()->Talk(SAY_BRANN_RADIO_LEVIATHAN3);
                            _events.ScheduleEvent(EVENT_LEVIATHAN_BREAK_DOOR, 5s);
                            break;
                        case EVENT_LEVIATHAN_BREAK_DOOR:
                            if (Creature* leviathan = GetCreature(DATA_FLAME_LEVIATHAN))
                                leviathan->AI()->DoAction(ACTION_MOVE_TO_CENTER_POSITION);
                            if (GameObject* gameObject = instance->GetGameObject(_leviathanGateGUID))
                                gameObject->SetGoState(GO_STATE_DESTROYED);
                            break;
                        case EVENT_FL_INTRO_LINE_1:
                            if (Creature* brann = GetCreature(DATA_BRANN_BRONZEBEARD_INTRO))
                                brann->AI()->Talk(SAY_BRANN_INTRO_PENTARUS_YOU_HEARD, ObjectAccessor::GetPlayer(*brann, _leviathanIntroPlayerGUID));
                            _events.ScheduleEvent(EVENT_FL_INTRO_LINE_2, 9s);
                            break;
                        case EVENT_FL_INTRO_LINE_2:
                            if (Creature* pentarus = GetCreature(DATA_ARCHMAGE_PENTARUS))
                                pentarus->AI()->Talk(SAY_PENTARUS_INTRO_OF_COURSE_BRANN);
                            _events.ScheduleEvent(EVENT_FL_INTRO_LINE_3, 9s);
                            break;
                        case EVENT_FL_INTRO_LINE_3:
                            if (Creature* brann = GetCreature(DATA_BRANN_BRONZEBEARD_INTRO))
                                brann->AI()->Talk(SAY_BRANN_INTRO_LETS_MOVE_OUT);
                            _events.ScheduleEvent(EVENT_FL_INTRO_LINE_4, 23s);
                            break;
                        case EVENT_FL_INTRO_LINE_4:
                            if (Creature* pentarus = GetCreature(DATA_ARCHMAGE_PENTARUS))
                                pentarus->AI()->Talk(SAY_PENTARUS_INTRO_MAGES_OF_KIRIN_TOR);
                            _events.ScheduleEvent(EVENT_FL_INTRO_LINE_5, 10s);
                            break;
                        case EVENT_FL_INTRO_LINE_5:
                            if (Creature* brann = GetCreature(DATA_BRANN_BRONZEBEARD_INTRO))
                                brann->AI()->Talk(SAY_BRANN_INTRO_BRING_DOWN_SHIELD);
                            _events.ScheduleEvent(EVENT_FL_INTRO_SHIELD_DOWN, 13s);
                            break;
                        case EVENT_FL_INTRO_SHIELD_DOWN:
                            _leviathanIntroCompleted = true;
                            SaveToDB();
                            if (GameObject* shield = instance->GetGameObject(_leviathanProtectiveBubbleGUID))
                                shield->DespawnOrUnsummon();
                            for (ObjectGuid const& guid : _leviathanIntroCollisionGUIDs)
                                if (GameObject* collision = instance->GetGameObject(guid))
                                    collision->DespawnOrUnsummon();
                            _leviathanIntroCollisionGUIDs.clear();
                            for (ObjectGuid const& mageGuid : _kirinTorMageGUIDs)
                            {
                                if (Creature* mage = instance->GetCreature(mageGuid))
                                {
                                    mage->InterruptNonMeleeSpells(true);
                                    mage->SetEmoteState(EMOTE_STATE_NONE);
                                }
                            }
                            _leviathanIntroPlayerGUID.Clear();
                            _events.ScheduleEvent(EVENT_FL_GAUNTLET_RADIO_POLL, 3s);
                            break;
                        case EVENT_FL_HARDMODE_LINE_DEACTIVATING:
                            if (Creature* norgannon = GetCreature(DATA_LORE_KEEPER_OF_NORGANNON))
                                norgannon->AI()->Talk(SAY_LORE_KEEPER_DEACTIVATING);
                            break;
                        case EVENT_FL_HARDMODE_NORGANNON_DESPAWN:
                            if (Creature* norgannon = GetCreature(DATA_LORE_KEEPER_OF_NORGANNON))
                                norgannon->DespawnOrUnsummon();
                            break;
                        case EVENT_FL_HARDMODE_BRANN_RADIO_WARN_1:
                        {
                            Creature* radio = GetCreature(DATA_BRONZEBEARD_RADIO);
                            if (!radio)
                                radio = instance->SummonCreature(NPC_BRONZEBEARD_RADIO, BrannRadioSummonPos);
                            if (radio)
                                radio->AI()->Talk(SAY_BRANN_RADIO_HARDMODE_WARN_1);
                            break;
                        }
                        case EVENT_FL_HARDMODE_DELLORAH_YELL_1:
                            if (Creature* dellorah = GetCreature(DATA_DELLORAH))
                            {
                                dellorah->AI()->SetData(99, 99);
                                dellorah->AI()->Talk(SAY_DELLORAH_HARDMODE_REACT, ObjectAccessor::GetPlayer(*dellorah, _leviathanIntroPlayerGUID));
                            }
                            break;
                        case EVENT_FL_HARDMODE_DELLORAH_RUN:
                            if (Creature* dellorah = GetCreature(DATA_DELLORAH))
                            {
                                dellorah->SetWalk(false);
                                dellorah->GetMotionMaster()->MovePoint(POINT_FL_HARDMODE_DELLORAH_TO_BRANN, FlameLeviathanHardmodeDellorahNearBrann);
                            }
                            break;
                        case EVENT_FL_HARDMODE_DELLORAH_YELL_2:
                            if (Creature* dellorah = GetCreature(DATA_DELLORAH))
                                dellorah->AI()->Talk(SAY_DELLORAH_HARDMODE_WARN, ObjectAccessor::GetPlayer(*dellorah, _leviathanIntroPlayerGUID));
                            _leviathanIntroPlayerGUID.Clear();
                            break;
                        case EVENT_FL_HARDMODE_BRANN_RESPOND:
                            if (Creature* brann = GetCreature(DATA_BRANN_BRONZEBEARD_INTRO))
                                brann->AI()->Talk(SAY_BRANN_INTRO_BRING_DOWN_SHIELD);
                            break;
                        case EVENT_FL_GAUNTLET_RADIO_POLL:
                        {
                            bool const hardmode = GetData(DATA_ACTIVE_TOWERS) != 0;

                            // proximity warnings
                            Map::PlayerList const& players = instance->GetPlayers();
                            for (FlameLeviathanGauntletProximityPoint const& point : FlGauntletProximityPoints)
                            {
                                if (_leviathanGauntletRadioFiredMask & point.firedMask)
                                    continue;
                                if (point.hardmodeOnly && !hardmode)
                                    continue;
                                for (auto i = players.begin(); i != players.end(); ++i)
                                {
                                    Player* player = i->GetSource();
                                    if (!player || !player->GetVehicleCreatureBase())
                                        continue;
                                    if (player->GetExactDist2d(point.pos.GetPositionX(), point.pos.GetPositionY()) <= point.radius)
                                    {
                                        FireGauntletRadioWarning(point.brannGroup, point.firedMask);
                                        break;
                                    }
                                }
                            }

                            if (!(_leviathanGauntletRadioFiredMask & GAUNTLET_RADIO_IRON_DWARVES))
                            {
                                for (auto i = players.begin(); i != players.end(); ++i)
                                {
                                    Player* player = i->GetSource();
                                    if (!player)
                                        continue;
                                    if (Creature* vehicle = player->GetVehicleCreatureBase())
                                    {
                                        if (vehicle->IsInCombat())
                                        {
                                            FireGauntletRadioWarning(SAY_BRANN_RADIO_GAUNTLET_IRON_DWARVES, GAUNTLET_RADIO_IRON_DWARVES);
                                            break;
                                        }
                                    }
                                }
                            }
                            _events.ScheduleEvent(EVENT_FL_GAUNTLET_RADIO_POLL, 3s);
                            break;
                        }
                        case EVENT_FL_OUTRO_SPAWN:
                        {
                            Creature* flyingMachine = instance->SummonCreature(NPC_BRANN_S_FLYING_MACHINE, FlameLeviathanOutroFlyingMachineSpawn, nullptr, FlameLeviathanOutroSummonDespawnMs);
                            Creature* brann = instance->SummonCreature(NPC_BRANN_BRONZEBEARD_FLYING_MACHINE, FlameLeviathanOutroFlyingMachineSpawn, nullptr, FlameLeviathanOutroSummonDespawnMs);
                            Creature* rhydian = instance->SummonCreature(NPC_ARCHMAGE_RHYDIAN, FlameLeviathanOutroRhydianSpawn, nullptr, FlameLeviathanOutroSummonDespawnMs);
                            if (flyingMachine && brann)
                            {
                                brann->EnterVehicle(flyingMachine);
                                flyingMachine->SetCanFly(true);
                                flyingMachine->SetDisableGravity(true);
                                flyingMachine->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                                flyingMachine->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                                flyingMachine->GetMotionMaster()->MoveSmoothPath(POINT_FL_OUTRO_FLYING_MACHINE_LAND, FlameLeviathanOutroFlyingMachinePath, std::size(FlameLeviathanOutroFlyingMachinePath), false);
                                _outroFlameLeviathanFlyingMachineGUID = flyingMachine->GetGUID();
                                _outroFlameLeviathanBrannGUID = brann->GetGUID();
                            }
                            if (rhydian)
                            {
                                _outroFlameLeviathanRhydianGUID = rhydian->GetGUID();
                                rhydian->SetWalk(true);
                                rhydian->GetMotionMaster()->MovePoint(POINT_FL_OUTRO_RHYDIAN_WALK_TO_LANDING, FlameLeviathanOutroRhydianWalkTo);
                            }
                            break;
                        }
                        case EVENT_FL_OUTRO_LINE_1:
                            if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                                brann->AI()->Talk(0);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_2, 9s);
                            break;
                        case EVENT_FL_OUTRO_LINE_2:
                            if (Creature* rhydian = instance->GetCreature(_outroFlameLeviathanRhydianGUID))
                                rhydian->AI()->Talk(2);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_3, 9s);
                            break;
                        case EVENT_FL_OUTRO_LINE_3:
                            if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                                brann->AI()->Talk(1);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_4, 8s);
                            break;
                        case EVENT_FL_OUTRO_LINE_4:
                            if (Creature* rhydian = instance->GetCreature(_outroFlameLeviathanRhydianGUID))
                                rhydian->AI()->Talk(3);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_5, 8s);
                            break;
                        case EVENT_FL_OUTRO_LINE_5:
                            if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                                brann->AI()->Talk(2);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_6, 9s);
                            break;
                        case EVENT_FL_OUTRO_LINE_6:
                            if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                                brann->AI()->Talk(3);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_7, 8s);
                            break;
                        case EVENT_FL_OUTRO_LINE_7:
                            if (Creature* rhydian = instance->GetCreature(_outroFlameLeviathanRhydianGUID))
                                rhydian->AI()->Talk(4);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_8, 9s);
                            break;
                        case EVENT_FL_OUTRO_LINE_8:
                            if (Creature* rhydian = instance->GetCreature(_outroFlameLeviathanRhydianGUID))
                                rhydian->AI()->Talk(5);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_9, 8s);
                            break;
                        case EVENT_FL_OUTRO_LINE_9:
                            if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                                brann->AI()->Talk(4);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_10, 9s);
                            break;
                        case EVENT_FL_OUTRO_LINE_10:
                            if (Creature* rhydian = instance->GetCreature(_outroFlameLeviathanRhydianGUID))
                                rhydian->AI()->Talk(6);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_LINE_11, 8s);
                            break;
                        case EVENT_FL_OUTRO_LINE_11:
                            if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                                brann->AI()->Talk(5);
                            _events.ScheduleEvent(EVENT_FL_OUTRO_DESPAWN, 30s);
                            break;
                        case EVENT_FL_OUTRO_DESPAWN:
                            if (Creature* brann = instance->GetCreature(_outroFlameLeviathanBrannGUID))
                                brann->DespawnOrUnsummon();
                            if (Creature* flyingMachine = instance->GetCreature(_outroFlameLeviathanFlyingMachineGUID))
                                flyingMachine->DespawnOrUnsummon();
                            if (Creature* rhydian = instance->GetCreature(_outroFlameLeviathanRhydianGUID))
                                rhydian->DespawnOrUnsummon();
                            _outroFlameLeviathanFlyingMachineGUID.Clear();
                            _outroFlameLeviathanBrannGUID.Clear();
                            _outroFlameLeviathanRhydianGUID.Clear();
                            break;
                    }
                }
            }

            void RestoreHodirRareCache()
            {
                if (GetBossState(DATA_HODIR) == DONE)
                    return;

                std::vector<RespawnInfo const*> respawnData;
                instance->GetRespawnInfo(respawnData, SPAWN_TYPEMASK_GAMEOBJECT);
                for (RespawnInfo const* info : respawnData)
                    if (info->entry == GO_HODIR_RARE_CACHE_OF_WINTER || info->entry == GO_HODIR_RARE_CACHE_OF_WINTER_HERO)
                        instance->Respawn(info->type, info->spawnId);
            }

            void DespawnLeviatanVehicle(Creature* vehicleCreature)
            {
                vehicleCreature->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                vehicleCreature->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                if (Vehicle* vehicle = vehicleCreature->GetVehicleKit())
                    vehicle->RemoveAllPassengers();
                vehicleCreature->DespawnOrUnsummon(5min);
            }

            LocalTransport* GetMimironTram()
            {
                GameObject* mimironTram = GetGameObject(DATA_MIMIRON_TRAM);
                if (!mimironTram || !mimironTram->ToTransport())
                    return nullptr;
                return static_cast<LocalTransport*>(mimironTram);
            }

            void UpdateMimironTramArrival()
            {
                LocalTransport* tram = GetMimironTram();
                if (!tram)
                    return;

                uint32 const progress = tram->GetPathProgress();
                if (_tramProgress < MimironTramArrivalMimiron && progress >= MimironTramArrivalMimiron)
                {
                    if (GameObject* turnaround = GetGameObject(DATA_MIMIRON_TRAM_TURNAROUND_2))
                        turnaround->UseDoorOrButton();
                    if (GameObject* rocketBooster = GetGameObject(DATA_MIMIRON_TRAM_ROCKET_BOOSTER))
                        rocketBooster->SetGoState(GO_STATE_READY);
                }
                else if (_tramProgress < MimironTramArrivalCenter && progress >= MimironTramArrivalCenter)
                {
                    if (GameObject* turnaround = GetGameObject(DATA_MIMIRON_TRAM_TURNAROUND_1))
                        turnaround->UseDoorOrButton();
                    if (GameObject* rocketBooster = GetGameObject(DATA_MIMIRON_TRAM_ROCKET_BOOSTER))
                        rocketBooster->SetGoState(GO_STATE_READY);
                }
                _tramProgress = progress;
            }

            void FireGauntletRadioWarning(uint32 brannGroup, uint32 firedBit)
            {
                if (_leviathanGauntletRadioFiredMask & firedBit)
                    return;
                _leviathanGauntletRadioFiredMask |= firedBit;
                Creature* radio = GetCreature(DATA_BRONZEBEARD_RADIO);
                if (!radio)
                    radio = instance->SummonCreature(NPC_BRONZEBEARD_RADIO, BrannRadioSummonPos);
                if (radio)
                    radio->AI()->Talk(brannGroup);
            }

            void UpdateDoorState(GameObject* door) override
            {
                // Leviathan doors are set to DOOR_TYPE_ROOM except the one it uses to enter the room
                // which has to be set to DOOR_TYPE_PASSAGE
                if (door->GetEntry() == GO_LEVIATHAN_DOOR && door->GetPositionX() > 400.f)
                    door->SetGoState(GetBossState(DATA_FLAME_LEVIATHAN) == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                else
                    InstanceScript::UpdateDoorState(door);
            }

            void AddDoor(GameObject* door, bool add) override
            {
                // Leviathan doors are South except the one it uses to enter the room
                // which is North and should not be used for boundary checks in BossAI::IsInBoundary()
                if (door->GetEntry() == GO_LEVIATHAN_DOOR && door->GetPositionX() > 400.f)
                {
                    if (add)
                        GetBossInfo(DATA_FLAME_LEVIATHAN)->door[DOOR_TYPE_PASSAGE].insert(door->GetGUID());
                    else
                        GetBossInfo(DATA_FLAME_LEVIATHAN)->door[DOOR_TYPE_PASSAGE].erase(door->GetGUID());

                    if (add)
                        UpdateDoorState(door);
                }
                else
                    InstanceScript::AddDoor(door, add);
            }

            void DespawnThorimSummons(GuidVector& summons)
            {
                for (ObjectGuid const& guid : summons)
                    if (Creature* summon = instance->GetCreature(guid))
                        summon->DespawnOrUnsummon();
                summons.clear();
            }

            static uint8 GetKeeperIndex(uint32 entry, bool observationRing)
            {
                for (uint8 i = 0; i < MAX_ULDUAR_KEEPERS; ++i)
                    if (entry == (observationRing ? KeeperData[i].ObservationRingEntry : KeeperData[i].YoggSaronEntry))
                        return i;

                return MAX_ULDUAR_KEEPERS;
            }

            uint32 GetActivatedKeeperCount() const
            {
                uint32 count = 0;
                for (uint8 i = 0; i < MAX_ULDUAR_KEEPERS; ++i)
                    if (_activatedKeeperMask & (1 << i))
                        ++count;

                return count;
            }

            void VerifyKeeperState()
            {
                for (uint8 i = 0; i < MAX_ULDUAR_KEEPERS; ++i)
                {
                    if (_activatedKeeperMask & (1 << i))
                    {
                        if (GetBossState(DATA_YOGG_SARON) != DONE && !instance->GetCreature(_keeperGUIDs[i]))
                            instance->SummonCreature(KeeperData[i].YoggSaronEntry, YSKeepersPos[i]);
                    }
                    else if (GetBossState(KeeperData[i].BossId) == DONE && !instance->GetCreature(_observationRingKeeperGUIDs[i]))
                        instance->SummonCreature(KeeperData[i].ObservationRingEntry, ObservationRingKeepersPos[i]);
                }
            }

        private:
            // Creatures
            GuidVector _leviathanVehicleGUIDs;
            GuidVector _kirinTorMageGUIDs;
            GuidVector _thorimPreAddGUIDs;
            ObjectGuid _xtToyPileGUIDs[4];
            ObjectGuid _assemblyGUIDs[3];
            ObjectGuid _elderGUIDs[3];
            ObjectGuid _freyaAchieveTriggerGUID;
            ObjectGuid _keeperGUIDs[MAX_ULDUAR_KEEPERS];
            ObjectGuid _observationRingKeeperGUIDs[MAX_ULDUAR_KEEPERS];
            ObjectGuid _outroFlameLeviathanFlyingMachineGUID;
            ObjectGuid _outroFlameLeviathanBrannGUID;
            ObjectGuid _outroFlameLeviathanRhydianGUID;
            // GameObjects
            ObjectGuid _leviathanGateGUID;
            ObjectGuid _leviathanProtectiveBubbleGUID;
            GuidVector _leviathanIntroCollisionGUIDs;
            ObjectGuid _kologarnChestGUID;
            ObjectGuid _kologarnBridgeGUID;
            ObjectGuid _thorimDarkIronPortcullisGUID;
            ObjectGuid _cacheOfStormsGUID;
            ObjectGuid _cacheOfStormsHardmodeGUID;
            ObjectGuid _hodirRareCacheGUID;
            ObjectGuid _hodirChestGUID;
            ObjectGuid _brainRoomDoorGUIDs[3];
            // Miscellaneous
            uint32 _teamInInstance;
            uint32 _colossusData;
            uint8 _elderCount;
            uint8 _illusion;
            bool _conSpeedAtory;
            bool _lumberjacked;
            bool _unbroken;
            bool _isDriveMeCrazyEligible;

            EventMap _events;
            uint32 _algalonTimer;
            bool _summonAlgalon;
            bool _algalonSummoned;
            uint8 _activatedKeeperMask;
            uint32 _maxArmorItemLevel;
            uint32 _maxWeaponItemLevel;
            uint32 _couAchivePlayerDeathMask;
            bool _activeTowers;
            uint32 _destroyedTowers;
            uint8 _stunned;

            bool _leviathanIntroCompleted;
            bool _leviathanIntroStarted;
            ObjectGuid _leviathanIntroPlayerGUID;
            uint32 _leviathanGauntletRadioFiredMask;
            bool _mimironTramUsed;
            uint32 _tramProgress;
            bool _mimironEngaged;
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_ulduar_InstanceMapScript(map);
        }
};

void AddSC_instance_ulduar()
{
    new instance_ulduar();
}
