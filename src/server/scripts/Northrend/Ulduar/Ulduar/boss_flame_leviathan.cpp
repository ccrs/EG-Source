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
#include "CellImpl.h"
#include "ChaseMovementGenerator.h"
#include "CombatAI.h"
#include "CommonHelpers.h"
#include "Containers.h"
#include "Creature.h"
#include "GameObjectAI.h"
#include "GridNotifiersImpl.h"
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "MovementTypedefs.h"
#include "ObjectAccessor.h"
#include "PassiveAI.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Vehicle.h"

enum FlameLeviathanSpells
{
    SPELL_PURSUED                  = 62374,
    SPELL_GATHERING_SPEED          = 62375,
    SPELL_BATTERING_RAM            = 62376,
    SPELL_FLAME_VENTS              = 62396,
    SPELL_MISSILE_BARRAGE          = 62400,
    SPELL_SYSTEMS_SHUTDOWN         = 62475,
    SPELL_OVERLOAD_CIRCUIT         = 62399,
    SPELL_START_THE_ENGINE         = 62472,
    SPELL_SEARING_FLAME            = 62402,
    SPELL_BLAZE                    = 62292,
    SPELL_TAR_PASSIVE              = 62288,
    SPELL_SMOKE_TRAIL              = 63575,
    SPELL_ELECTROSHOCK             = 62522,
    SPELL_NAPALM                   = 63666,
    SPELL_INVIS_AND_STEALTH_DETECT = 18950, // Passive
    SPELL_FLAME_VENTS_TRIGGER      = 63847,
    SPELL_HOOKSHOT                 = 62323,
    SPELL_HOOKSHOT_AURA            = 62336,
    //TOWER Additional SPELLS
    SPELL_THORIM_S_HAMMER          = 62911, // Tower of Storms
    SPELL_MIMIRON_S_INFERNO        = 62909, // Tower of Flames
    SPELL_HODIR_S_FURY             = 62533, // Tower of Frost
    SPELL_FREYA_S_WARD             = 62906, // Tower of Nature
    //TOWER ap & health spells
    SPELL_BUFF_TOWER_OF_STORMS     = 65076,
    SPELL_BUFF_TOWER_OF_FLAMES     = 65075,
    SPELL_BUFF_TOWER_OF_FR0ST      = 65077,
    SPELL_BUFF_TOWER_OF_LIFE       = 64482,
    //Additional Spells
    SPELL_LASH                     = 65062,
    SPELL_AUTO_REPAIR              = 62705,
    SPELL_LIQUID_PYRITE            = 62494,
    SPELL_LIQUID_PYRITE_RELOAD     = 62496,
    SPELL_LIQUID_PYRITE_DRIP       = 62987,
    SPELL_SPAWN_PYRITE             = 62543,
    SPELL_DUSTY_EXPLOSION          = 63360,
    SPELL_DUST_CLOUD_IMPACT        = 54740,
    AURA_STEALTH_DETECTION         = 18950,
    SPELL_RIDE_VEHICLE             = 46598,
    // Ulduar Colossus
    SPELL_GROUND_SLAM              = 62625,
    SPELL_LIGHTNING_SKYBEAM        = 62897,
    SPELL_RED_SKYBEAM              = 63772,
    SPELL_BLUE_SKYBEAM             = 63769,
    SPELL_GREEN_SKYBEAM            = 62895,
};

enum FlameLeviathanCreatures
{
    NPC_SEAT                       = 33114,
    NPC_MECHANOLIFT                = 33214,
    NPC_LIQUID_PYRITE              = 33189,
    NPC_CONTAINER                  = 33218,
    NPC_THORIM_BEACON              = 33364,
    NPC_THORIM_HAMMER              = 33365,
    NPC_MIMIRON_BEACON             = 33369,
    NPC_MIMIRON_INFERNO            = 33370,
    NPC_HODIR_BEACON               = 33108,
    NPC_HODIR_FURY                 = 33212,
    NPC_FREYA_BEACON               = 33366,
    NPC_FREYA_WARD                 = 33367,
    NPC_ULDUAR_GAUNTLET_GENERATOR  = 34159, // 33571, // Trigger tied to towers
};

enum FlameLeviathanTowers
{
    GO_TOWER_OF_STORMS    = 194377,
    GO_TOWER_OF_FLAMES    = 194371,
    GO_TOWER_OF_FROST     = 194370,
    GO_TOWER_OF_LIFE      = 194375,
};

enum FlameLeviathanEvents
{
    EVENT_PURSUE               = 1,
    EVENT_MISSILE              = 2,
    EVENT_VENT                 = 3,
    EVENT_SPEED                = 4,
    EVENT_SHUTDOWN             = 6,
    EVENT_REPAIR               = 7,
    EVENT_THORIM_S_HAMMER      = 8,    // Tower of Storms
    EVENT_MIMIRON_S_INFERNO    = 9,    // Tower of Flames
    EVENT_HODIR_S_FURY         = 10,   // Tower of Frost
    EVENT_FREYA_S_WARD         = 11,   // Tower of Nature
    EVENT_CHECK_WIPE           = 12,
    // Ulduar Colossus
    EVENT_GROUND_SLAM,
};

enum FlameLeviathanSeats
{
    SEAT_PLAYER    = 0,
    SEAT_TURRET    = 1,
    SEAT_DEVICE    = 2,
    SEAT_CANNON    = 7,
};

enum FlameLeviathanVehicles
{
    VEHICLE_SIEGE         = 33060,
    VEHICLE_CHOPPER       = 33062,
    VEHICLE_DEMOLISHER    = 33109,
};

enum FlameLeviathanMisc
{
    DATA_SHUTOUT               = 29112912, // 2911, 2912 are achievement IDs
    DATA_ORBIT_ACHIEVEMENTS    = 1,
    FREYA_SPAWNS               = 4,
    HODIR_SPAWNS               = 2,
    POINT_ENGAGE               = 1
};

enum FlameLeviathanYells
{
    SAY_AGGRO            = 0,
    SAY_SLAY             = 1,
    SAY_DEATH            = 2,
    SAY_TARGET           = 3,
    SAY_HARDMODE         = 4,
    SAY_TOWER_NONE       = 5,
    SAY_TOWER_FROST      = 6,
    SAY_TOWER_FLAME      = 7,
    SAY_TOWER_NATURE     = 8,
    SAY_TOWER_STORM      = 9,
    SAY_PLAYER_RIDING    = 10,
    SAY_OVERLOAD         = 11,
    EMOTE_PURSUE         = 12,
    EMOTE_OVERLOAD       = 13,
    EMOTE_REPAIR         = 14
};

enum FlameLeviathanMiscellanousData
{
    // Other Actions are in Ulduar.h
    // ACTION_START_HARD_MODE    = 5,
    // ACTION_SPAWN_VEHICLES     = 6,
    // Amount of seats depending on Raid mode
    TWO_SEATS                 = 2,
    FOUR_SEATS                = 4,
};

Position const FlameLeviathanCenter = { 354.8771f, -12.90240f, 409.803650f, 3.14f };
Position const FlameLeviathanInfernoStart = { 390.93f, -13.91f, 409.81f, 3.14f };

Position const FlameLeviathanFreyaBeacons[FREYA_SPAWNS] =
{
    { 377.02f, -119.10f, 409.81f, 0.0f },
    { 185.62f, -119.10f, 409.81f, 0.0f },
    { 377.02f, 54.78f, 409.81f, 0.0f },
    { 185.62f, 54.78f, 409.81f, 0.0f },
};

Position const FlameLeviathanHodirBeacons[HODIR_SPAWNS] =
{
    { 219.9013f, 7.913357f, 409.7861f, 0.0f },
    { 326.0777f, -74.99034f, 409.887f, 0.0f },
};

class boss_flame_leviathan : public CreatureScript
{
    public:
        boss_flame_leviathan() : CreatureScript("boss_flame_leviathan") { }

        struct boss_flame_leviathanAI : public BossAI
        {
            boss_flame_leviathanAI(Creature* creature) : BossAI(creature, DATA_FLAME_LEVIATHAN)
            {
                Initialize();
            }

            void Initialize()
            {
                ActiveTowersCount = 0;
                Shutdown = 0;
                ActiveTowers = false;
                Shutout = true;
                Unbroken = true;
            }

            void InitializeAI() override
            {
                if (!me->isDead())
                    Reset();

                Initialize();

                DoCast(SPELL_INVIS_AND_STEALTH_DETECT);

                me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_STUNNED);
                me->SetReactState(REACT_PASSIVE);
            }

            void JustAppeared() override
            {
                if (me->IsAlive() && instance->GetData(DATA_COLOSSUS) >= 2 && instance->GetBossState(DATA_FLAME_LEVIATHAN) != DONE)
                {
                    me->SetHomePosition(FlameLeviathanCenter);
                    me->NearTeleportTo(FlameLeviathanCenter);
                    me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_STUNNED);
                    me->SetReactState(REACT_AGGRESSIVE);
                }
            }

            void Reset() override
            {
                _Reset();
                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_STORMS);
                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FLAMES);
                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FR0ST);
                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_LIFE);

                ActiveTowersCount = 0;
                //resets shutdown counter to 0.  2 or 4 depending on raid mode
                Shutdown = 0;
                _pursueTarget.Clear();

                me->SetReactState(REACT_DEFENSIVE);
                me->SetRegenerateHealth(false);
            }

            void JustEngagedWith(Unit* who) override
            {
                BossAI::JustEngagedWith(who);
                events.ScheduleEvent(EVENT_PURSUE, 1ms);
                events.ScheduleEvent(EVENT_MISSILE, 1500ms, 4s);
                events.ScheduleEvent(EVENT_VENT, 31s);
                events.ScheduleEvent(EVENT_SHUTDOWN, 150s);
                events.ScheduleEvent(EVENT_SPEED, 10s);
                events.RescheduleEvent(EVENT_CHECK_WIPE, 5s);

                CheckTowers();
            }

            void EnterEvadeMode(EvadeReason why) override
            {
                instance->SetBossState(DATA_FLAME_LEVIATHAN, FAIL);
                BossAI::EnterEvadeMode(why);
            }

            void JustReachedHome() override
            {
                BossAI::JustReachedHome();
                me->SetFullHealth();
            }

            void CheckTowers()
            {
                ActiveTowers = instance->GetData(DATA_ACTIVE_TOWERS) != 0;
                uint32 destroyedTowers = instance->GetData(DATA_DESTROYED_TOWERS);
                uint8 towersCount = 0;
                if (ActiveTowers)
                {
                    if ((destroyedTowers & ACTION_TOWER_OF_STORM_DESTROYED) == 0)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_STORMS, me);
                        events.ScheduleEvent(EVENT_THORIM_S_HAMMER, 35s);
                        ++towersCount;
                    }
                    if ((destroyedTowers & ACTION_TOWER_OF_FLAMES_DESTROYED) == 0)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_FLAMES, me);
                        events.ScheduleEvent(EVENT_MIMIRON_S_INFERNO, 70s);
                        ++towersCount;
                    }
                    if ((destroyedTowers & ACTION_TOWER_OF_FROST_DESTROYED) == 0)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_FR0ST, me);
                        events.ScheduleEvent(EVENT_HODIR_S_FURY, 105s);
                        ++towersCount;
                    }
                    if ((destroyedTowers & ACTION_TOWER_OF_LIFE_DESTROYED) == 0)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_LIFE, me);
                        events.ScheduleEvent(EVENT_FREYA_S_WARD, 140s);
                        ++towersCount;
                    }
                    if (towersCount == 0)
                        Talk(SAY_TOWER_NONE);
                    else
                        Talk(SAY_HARDMODE);
                }
                else
                    Talk(SAY_AGGRO);

                switch (towersCount)
                {
                    case 0:
                        me->SetLootMode(LOOT_MODE_DEFAULT);
                        break;
                    case 1:
                        me->SetLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1);
                        break;
                    case 2:
                        me->SetLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2);
                        break;
                    case 3:
                        me->SetLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2 | LOOT_MODE_HARD_MODE_3);
                        break;
                    case 4:
                        me->SetLootMode(LOOT_MODE_DEFAULT | LOOT_MODE_HARD_MODE_1 | LOOT_MODE_HARD_MODE_2 | LOOT_MODE_HARD_MODE_3 | LOOT_MODE_HARD_MODE_4);
                        break;
                    default:
                        break;
                }
                ActiveTowersCount = towersCount;
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);

                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_STORMS);
                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FLAMES);
                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_FR0ST);
                me->RemoveAurasDueToSpell(SPELL_BUFF_TOWER_OF_LIFE);
            }

            void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
            {
                if (spellInfo->Id == SPELL_START_THE_ENGINE)
                    if (Vehicle* vehicleKit = me->GetVehicleKit())
                        vehicleKit->InstallAllAccessories(false);

                if (spellInfo->Id == SPELL_ELECTROSHOCK)
                    me->InterruptSpell(CURRENT_CHANNELED_SPELL);

                if (spellInfo->Id == SPELL_OVERLOAD_CIRCUIT)
                    ++Shutdown;
            }

            uint32 GetData(uint32 type) const override
            {
                switch (type)
                {
                    case DATA_SHUTOUT:
                        return Shutout ? 1 : 0;
                    case DATA_UNBROKEN:
                        return Unbroken ? 1 : 0;
                    case DATA_ORBIT_ACHIEVEMENTS:
                        if (ActiveTowers) // Only on HardMode
                            return ActiveTowersCount;
                        break;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 id, uint32 data) override
            {
                if (id == DATA_UNBROKEN)
                    Unbroken = data ? true : false;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (Shutdown >= RAID_MODE(TWO_SEATS, FOUR_SEATS))
                {
                    Shutdown = 0;
                    events.CancelEvent(EVENT_SHUTDOWN);
                    events.ScheduleEvent(EVENT_SHUTDOWN, 4s);
                    me->RemoveAurasDueToSpell(SPELL_OVERLOAD_CIRCUIT);
                    me->InterruptNonMeleeSpells(true);
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PURSUE:
                            _pursueTarget.Clear();
                            if (SpellCastResult::SPELL_CAST_OK == DoCast(SPELL_PURSUED))  // Will select target in spellscript
                                Talk(SAY_TARGET);
                            events.ScheduleEvent(EVENT_PURSUE, 31s);
                            break;
                        case EVENT_MISSILE:
                            DoCast(me, SPELL_MISSILE_BARRAGE, true);
                            events.ScheduleEvent(EVENT_MISSILE, 2s);
                            break;
                        case EVENT_VENT:
                            DoCastAOE(SPELL_FLAME_VENTS);
                            events.ScheduleEvent(EVENT_VENT, 20s);
                            break;
                        case EVENT_SPEED:
                            DoCastAOE(SPELL_GATHERING_SPEED);
                            events.ScheduleEvent(EVENT_SPEED, 10s);
                            break;
                        case EVENT_SHUTDOWN:
                            Talk(SAY_OVERLOAD);
                            Talk(EMOTE_OVERLOAD);
                            me->CastSpell(me, SPELL_SYSTEMS_SHUTDOWN, true);
                            if (Shutout)
                                Shutout = false;
                            events.ScheduleEvent(EVENT_REPAIR, 4s);
                            events.DelayEvents(20s, 0);
                            break;
                        case EVENT_REPAIR:
                            Talk(EMOTE_REPAIR);
                            events.ScheduleEvent(EVENT_SHUTDOWN, 150s);
                            events.CancelEvent(EVENT_REPAIR);
                            break;
                        case EVENT_THORIM_S_HAMMER: // Tower of Storms
                        {
                            uint8 count = 0;
                            for (auto const& summonGUID : summons)
                                if (Creature* summon = ObjectAccessor::GetCreature(*me, summonGUID))
                                    if (summon->GetEntry() == NPC_THORIM_BEACON)
                                        ++count;
                            if (count < 4)
                            {
                                Position pos = me->GetPosition();
                                me->MovePosition(pos, frand(30.f, 70.f) * (float)rand_norm(), (float)rand_norm() * static_cast<float>(2 * M_PI));
                                if (Creature* thorim = DoSummon(NPC_THORIM_BEACON, pos, 8s, TEMPSUMMON_TIMED_DESPAWN))
                                {
                                    thorim->GetMotionMaster()->MoveRandom(100.f);
                                    thorim->CastSpell(thorim, SPELL_LIGHTNING_SKYBEAM, true);
                                }
                            }
                            if (count == 0)
                                Talk(SAY_TOWER_STORM);
                            events.RescheduleEvent(EVENT_THORIM_S_HAMMER, count < 4 ? 1s : 6s);
                            break;
                        }
                        case EVENT_MIMIRON_S_INFERNO: // Tower of Flames
                            if (Creature* mimiron = DoSummon(NPC_MIMIRON_BEACON, FlameLeviathanInfernoStart, 0s))
                                mimiron->CastSpell(mimiron, SPELL_RED_SKYBEAM, true);
                            Talk(SAY_TOWER_FLAME);
                            events.CancelEvent(EVENT_MIMIRON_S_INFERNO);
                            break;
                        case EVENT_HODIR_S_FURY: // Tower of Frost
                            for (Position const& currentPosition : FlameLeviathanHodirBeacons)
                            {
                                Position pos = currentPosition;
                                me->MovePosition(pos, 50.f * (float)rand_norm(), (float)rand_norm() * static_cast<float>(2 * M_PI));
                                if (Creature* hodir = DoSummon(NPC_HODIR_BEACON, pos, 0s))
                                    hodir->CastSpell(hodir, SPELL_BLUE_SKYBEAM, true);
                            }
                            Talk(SAY_TOWER_FROST);
                            events.CancelEvent(EVENT_HODIR_S_FURY);
                            break;
                        case EVENT_FREYA_S_WARD: // Tower of Nature
                            Talk(SAY_TOWER_NATURE);
                            for (Position const& currentPosition : FlameLeviathanFreyaBeacons)
                                if (TempSummon* summon = me->SummonCreature(NPC_FREYA_BEACON, currentPosition))
                                    summon->CastSpell(summon, SPELL_GREEN_SKYBEAM, true);

                            events.CancelEvent(EVENT_FREYA_S_WARD);
                            break;
                        case EVENT_CHECK_WIPE:
                        {
                            auto combatReferences = me->GetCombatManager().GetPvECombatRefs();
                            if (combatReferences.empty())
                            {
                                EnterEvadeMode(EVADE_REASON_NO_HOSTILES);
                                break;
                            }

                            bool evade = true;
                            for (std::unordered_map<ObjectGuid, CombatReference*>::value_type current : combatReferences)
                            {
                                if (current.second->GetOther(me)->IsCharmedOwnedByPlayerOrPlayer() || current.second->GetOther(me)->IsControlledByPlayer())
                                {
                                    evade = false;
                                    break;
                                }
                            }
                            if (evade)
                                EnterEvadeMode(EVADE_REASON_NO_HOSTILES);
                            else
                                events.RescheduleEvent(EVENT_CHECK_WIPE, 5s);
                            break;
                        }
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoBatteringRamIfReady();
            }

            void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override
            {
                if (spellInfo->Id == SPELL_PURSUED)
                    if (Unit* unitTarget = target->ToUnit())
                    {
                        _pursueTarget = target->GetGUID();
                        AttackStart(unitTarget);
                        me->GetThreatManager().FixateTarget(unitTarget);

                        if (MovementGenerator const* base = me->GetMotionMaster()->GetMovementGenerator([](MovementGenerator const* movegen) -> bool
                        {
                            return movegen->GetMovementGeneratorType() == CHASE_MOTION_TYPE;
                        }))
                        {
                            ChaseMovementGenerator const* chase = static_cast<ChaseMovementGenerator const*>(base);
                            if (chase->GetTarget() != unitTarget)
                                me->GetMotionMaster()->MoveChase(unitTarget);
                        }
                        else
                            me->GetMotionMaster()->MoveChase(unitTarget);

                        if (unitTarget->GetVehicleKit())
                            for (auto itr = unitTarget->GetVehicleKit()->Seats.begin(); itr != unitTarget->GetVehicleKit()->Seats.end(); ++itr)
                            {
                                if (itr->second.Passenger.Guid.IsPlayer())
                                    if (Player* passenger = ObjectAccessor::GetPlayer(*me, itr->second.Passenger.Guid))
                                    {
                                        Talk(EMOTE_PURSUE, passenger);
                                        break;
                                    }
                            }
                    }
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_MOVE_TO_CENTER_POSITION: // Triggered by 2 Collossus near door
                        if (me->IsAlive())
                        {
                            me->SetHomePosition(FlameLeviathanCenter);
                            me->RemoveUnitFlag(UNIT_FLAG_STUNNED);
                            me->GetMotionMaster()->MoveCharge(FlameLeviathanCenter.GetPositionX(), FlameLeviathanCenter.GetPositionY(), FlameLeviathanCenter.GetPositionZ(), 42.0f, POINT_ENGAGE); // position center
                            return;
                        }
                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                switch (id)
                {
                    case POINT_ENGAGE:
                        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE);
                        me->m_Events.AddEvent(new Trinity::Helpers::Events::GenericEvent(me, [](WorldObject* obj) -> bool
                        {
                            if (Creature* c = obj->ToCreature())
                                c->SetReactState(REACT_AGGRESSIVE);
                            return true;
                        }), me->m_Events.CalculateTime(2s));
                    default:
                        break;
                }
            }

        private:
            //! Copypasta from DoSpellAttackIfReady, only difference is the target - it cannot be selected trough getVictim this way -
            //! I also removed the spellInfo check
            void DoBatteringRamIfReady()
            {
                if (me->isAttackReady())
                {
                    Unit* target = ObjectAccessor::GetUnit(*me, _pursueTarget);
                    if (!target && events.GetTimeUntilEvent(EVENT_PURSUE) > 5s)
                        events.RescheduleEvent(EVENT_PURSUE, 3s);

                    // Pursue was unable to acquire a valid target, so get the current victim as target.
                    if (!target && me->GetVictim())
                        target = me->GetVictim();

                    if (me->IsWithinCombatRange(target, 30.0f))
                    {
                        DoCast(target, SPELL_BATTERING_RAM);
                        me->resetAttackTimer();
                    }
                }
            }

            ObjectGuid _pursueTarget;
            uint8 Shutdown;
            bool ActiveTowers;
            bool Shutout;
            bool Unbroken;
            uint32 ActiveTowersCount;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<boss_flame_leviathanAI>(creature);
        }
};

class boss_flame_leviathan_seat : public CreatureScript
{
    public:
        boss_flame_leviathan_seat() : CreatureScript("boss_flame_leviathan_seat") { }

        struct boss_flame_leviathan_seatAI : public ScriptedAI
        {
            boss_flame_leviathan_seatAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            void PassengerBoarded(Unit* who, int8 seatId, bool apply) override
            {
                if (!me->GetVehicle())
                    return;

                if (seatId == SEAT_PLAYER)
                {
                    if (!apply)
                        return;
                    else if (Creature* leviathan = me->GetVehicleCreatureBase())
                        leviathan->AI()->Talk(SAY_PLAYER_RIDING);

                    if (Unit* turretPassenger = me->GetVehicleKit()->GetPassenger(SEAT_TURRET))
                        if (Creature* turret = turretPassenger->ToCreature())
                        {
                            turret->SetFaction(me->GetVehicleBase()->GetFaction());
                            turret->ReplaceAllUnitFlags(UnitFlags(0)); // unselectable
                            turret->AI()->AttackStart(who);
                        }
                    if (Unit* devicePassenger = me->GetVehicleKit()->GetPassenger(SEAT_DEVICE))
                        if (Creature* device = devicePassenger->ToCreature())
                        {
                            device->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                            device->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                        }

                    me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                }
                else if (seatId == SEAT_TURRET)
                {
                    if (apply)
                        return;

                    if (Unit* device = ASSERT_NOTNULL(me->GetVehicleKit())->GetPassenger(SEAT_DEVICE))
                    {
                        device->SetNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                        device->ReplaceAllUnitFlags(UnitFlags(0)); // unselectable
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<boss_flame_leviathan_seatAI>(creature);
        }
};

class boss_flame_leviathan_defense_cannon : public CreatureScript
{
    public:
        boss_flame_leviathan_defense_cannon() : CreatureScript("boss_flame_leviathan_defense_cannon") { }

        struct boss_flame_leviathan_defense_cannonAI : public ScriptedAI
        {
            boss_flame_leviathan_defense_cannonAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void Initialize()
            {
                NapalmTimer = 5 * IN_MILLISECONDS;
            }

            uint32 NapalmTimer;

            void Reset() override
            {
                Initialize();
                DoCast(me, AURA_STEALTH_DETECTION);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (Unit* base = me->GetVehicleBase())
                    if (base->HasAura(SPELL_SYSTEMS_SHUTDOWN))
                        return;

                if (NapalmTimer <= diff)
                {
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                        if (CanAIAttack(target))
                            DoCast(target, SPELL_NAPALM, true);

                    NapalmTimer = 5000;
                }
                else
                    NapalmTimer -= diff;
            }

            bool CanAIAttack(Unit const* who) const override
            {
                if (who->GetTypeId() != TYPEID_PLAYER || !who->GetVehicle() || who->GetVehicleBase()->GetEntry() == NPC_SEAT)
                    return false;
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<boss_flame_leviathan_defense_cannonAI>(creature);
        }
};

class boss_flame_leviathan_defense_turret : public CreatureScript
{
    public:
        boss_flame_leviathan_defense_turret() : CreatureScript("boss_flame_leviathan_defense_turret") { }

        struct boss_flame_leviathan_defense_turretAI : public TurretAI
        {
            boss_flame_leviathan_defense_turretAI(Creature* creature) : TurretAI(creature) { }

            void DamageTaken(Unit* who, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
            {
                if (!CanAIAttack(who))
                    damage = 0;
            }

            bool CanAIAttack(Unit const* who) const override
            {
                if (!who || who->GetTypeId() != TYPEID_PLAYER || !who->GetVehicle() || who->GetVehicleBase()->GetEntry() != NPC_SEAT)
                    return false;
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<boss_flame_leviathan_defense_turretAI>(creature);
        }
};

class boss_flame_leviathan_overload_device : public CreatureScript
{
    public:
        boss_flame_leviathan_overload_device() : CreatureScript("boss_flame_leviathan_overload_device") { }

        struct boss_flame_leviathan_overload_deviceAI : public PassiveAI
        {
            boss_flame_leviathan_overload_deviceAI(Creature* creature) : PassiveAI(creature)
            {
            }

            void OnSpellClick(Unit* /*clicker*/, bool spellClickHandled) override
            {
                if (!spellClickHandled)
                    return;

                if (me->GetVehicle())
                {
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_SPELLCLICK);
                    me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);

                    if (Unit* player = me->GetVehicle()->GetPassenger(SEAT_PLAYER))
                    {
                        me->GetVehicleBase()->CastSpell(player, SPELL_SMOKE_TRAIL, true);
                        player->GetMotionMaster()->MoveKnockbackFrom(me->GetVehicleBase()->GetPositionX(), me->GetVehicleBase()->GetPositionY(), 30, 30);
                        player->ExitVehicle();
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<boss_flame_leviathan_overload_deviceAI>(creature);
        }
};

class npc_mechanolift : public CreatureScript
{
    public:
        npc_mechanolift() : CreatureScript("npc_mechanolift") { }

        struct npc_mechanoliftAI : public PassiveAI
        {
            npc_mechanoliftAI(Creature* creature) : PassiveAI(creature)
            {
                me->GetMotionMaster()->MoveRandom(50.f);
            }

            void JustAppeared() override
            {
                if (Vehicle* kit = me->GetVehicleKit())
                    if (Unit* container = kit->GetPassenger(1))
                        _containerGuid = container->GetGUID();
            }

            void JustDied(Unit* /*killer*/) override
            {
                DoCastAOE(SPELL_LIQUID_PYRITE_DRIP, true);

                float groundZ = me->GetFloorZ();
                float heightDiff = me->GetPositionZ() - groundZ;
                Position landing(me->GetPositionX(), me->GetPositionY(), groundZ, me->GetOrientation());
                ObjectGuid containerGuid = _containerGuid;

                if (heightDiff <= 0.5f)
                {
                    FireDrop(me, containerGuid, landing);
                    return;
                }

                Milliseconds fallTime = std::chrono::round<Milliseconds>(std::chrono::duration<float>(Movement::computeFallTime(heightDiff, false)));
                me->m_Events.AddEventAtOffset(new Trinity::Helpers::Events::GenericEvent(me, [containerGuid, landing](WorldObject* o)
                {
                    if (Unit* mecha = o->ToUnit())
                        FireDrop(mecha, containerGuid, landing);
                    return true;
                }), fallTime);
            }

        private:
            static void FireDrop(Unit* mechanolift, ObjectGuid containerGuid, Position const& landing)
            {
                Unit* caster = mechanolift;
                Creature* container = ObjectAccessor::GetCreature(*mechanolift, containerGuid);
                if (container)
                    caster = container;
                caster->CastSpell(landing, SPELL_DUSTY_EXPLOSION, true);
                caster->CastSpell(caster, SPELL_DUST_CLOUD_IMPACT, true);
                caster->CastSpell(landing, SPELL_SPAWN_PYRITE, true);

                if (container)
                    container->DespawnOrUnsummon(2s);
                if (Creature* lift = mechanolift->ToCreature())
                    lift->DespawnOrUnsummon(2s);
            }

            ObjectGuid _containerGuid;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_mechanoliftAI>(creature);
        }
};

class npc_pool_of_tar : public CreatureScript
{
    public:
        npc_pool_of_tar() : CreatureScript("npc_pool_of_tar") { }

        struct npc_pool_of_tarAI : public ScriptedAI
        {
            npc_pool_of_tarAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_PASSIVE);
                me->CastSpell(me, SPELL_TAR_PASSIVE, true);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
            {
                damage = 0;
            }

            void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
            {
                if (spellInfo->SchoolMask & SPELL_SCHOOL_MASK_FIRE && !me->HasAura(SPELL_BLAZE))
                    me->CastSpell(me, SPELL_BLAZE, true);
            }

            void UpdateAI(uint32 /*diff*/) override { }
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_pool_of_tarAI>(creature);
        }
};

class npc_colossus : public CreatureScript
{
    public:
        npc_colossus() : CreatureScript("npc_colossus") { }

        struct npc_colossusAI : public ScriptedAI
        {
            npc_colossusAI(Creature* creature) : ScriptedAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void Reset() override
            {
                _events.Reset();
                ScriptedAI::Reset();
            }

            void JustEngagedWith(Unit* /*who*/) override
            {
                _events.ScheduleEvent(EVENT_GROUND_SLAM, 1s, 5s);
            }

            void JustDied(Unit* /*killer*/) override
            {
                if (me->GetHomePosition().IsInDist(&FlameLeviathanCenter, 50.f))
                    instance->SetData(DATA_COLOSSUS, instance->GetData(DATA_COLOSSUS) + 1);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (EVENT_GROUND_SLAM == _events.ExecuteEvent())
                {
                    DoCastVictim(SPELL_GROUND_SLAM);
                    _events.ScheduleEvent(EVENT_GROUND_SLAM, 5s, 6s);
                }

                DoMeleeAttackIfReady();
            }
        private:
            EventMap _events;
            InstanceScript* instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_colossusAI>(creature);
        }
};

class npc_thorims_hammer : public CreatureScript
{
    public:
        npc_thorims_hammer() : CreatureScript("npc_thorims_hammer") { }

        struct npc_thorims_hammerAI : public ScriptedAI
        {
            npc_thorims_hammerAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                creature->SetReactState(REACT_PASSIVE);
                _cooldown.Reset(4s);
                _casted = false;
            }

            void UpdateAI(uint32 diff) override
            {
                _cooldown.Update(diff);
                if (!_casted && _cooldown.Passed())
                {
                    _casted = true;
                    DoCastAOE(SPELL_THORIM_S_HAMMER);
                }
            }

        private:
            TimeTracker _cooldown;
            bool _casted;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_thorims_hammerAI>(creature);
        }
};

static constexpr uint32 PATH_ESCORT_MIMIRONS_INFERNO = 266962;

class npc_mimirons_inferno : public CreatureScript
{
    public:
        npc_mimirons_inferno() : CreatureScript("npc_mimirons_inferno") { }

        struct npc_mimirons_infernoAI : public EscortAI
        {
            npc_mimirons_infernoAI(Creature* creature) : EscortAI(creature)
            {
                Initialize();
                creature->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE);
                creature->SetReactState(REACT_PASSIVE);
            }

            void Initialize()
            {
                infernoTimer = 15000;
            }

            void Reset() override
            {
                LoadPath(PATH_ESCORT_MIMIRONS_INFERNO);
                Start(false, ObjectGuid::Empty, nullptr, false, true);
                SetDespawnAtFar(false);
                SetDespawnAtEnd(false);
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_MIMIRON_INFERNO)
                    _spellCaster = summon->GetGUID();
            }

            void UpdateAI(uint32 diff) override
            {
                EscortAI::UpdateAI(diff);

                if (infernoTimer <= diff)
                {
                    me->PauseMovement(4000, MOTION_SLOT_DEFAULT);
                    if (Creature* caster = ObjectAccessor::GetCreature(*me, _spellCaster))
                        caster->CastSpell(nullptr, SPELL_MIMIRON_S_INFERNO);
                    infernoTimer = 24000;
                }
                else
                    infernoTimer -= diff;
            }

        private:
            uint32 infernoTimer;
            TimeTracker _pauseTimer;
            ObjectGuid _spellCaster;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_mimirons_infernoAI>(creature);
        }
};

class npc_hodirs_fury : public CreatureScript
{
    public:
        npc_hodirs_fury() : CreatureScript("npc_hodirs_fury") { }

        struct npc_hodirs_furyAI : public ScriptedAI
        {
            npc_hodirs_furyAI(Creature* creature) : ScriptedAI(creature)
            {
                creature->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                creature->SetReactState(REACT_PASSIVE);
                _cooldown.Reset(3s);
                _moving = false;
            }

            void JustSummoned(Creature* summon) override
            {
                if (summon->GetEntry() == NPC_HODIR_FURY)
                    _spellCaster = summon->GetGUID();
            }

            void MovementInform(uint32 type, uint32 id) override
            {
                if (type != POINT_MOTION_TYPE || !id)
                    return;

                if (Creature* caster = ObjectAccessor::GetCreature(*me, _spellCaster))
                    caster->CastSpell(nullptr, SPELL_HODIR_S_FURY);
                _cooldown.Reset(5s);
                _moving = false;
            }

            void UpdateAI(uint32 diff) override
            {
                _cooldown.Update(diff);
                if (!_moving && _cooldown.Passed())
                {
                    if (InstanceScript* instance = me->GetInstanceScript())
                        if (Creature* leviathan = instance->GetCreature(DATA_FLAME_LEVIATHAN))
                        {
                            if (Unit* target = leviathan->AI()->SelectTarget(SelectTargetMethod::Random, 0))
                            {
                                me->GetMotionMaster()->MovePoint(1, target->GetPositionX(), target->GetPositionY(), target->GetFloorZ());
                                _moving = true;
                            }
                        }
                }
            }

        private:
            TimeTracker _cooldown;
            ObjectGuid _spellCaster;
            bool _moving;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_hodirs_furyAI>(creature);
        }
};

class npc_freyas_ward : public CreatureScript
{
    public:
        npc_freyas_ward() : CreatureScript("npc_freyas_ward") { }

        struct npc_freyas_wardAI : public ScriptedAI
        {
            npc_freyas_wardAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
                creature->SetReactState(REACT_PASSIVE);
            }

            void Initialize()
            {
                _summonTimer.Reset(5s);
            }

            void Reset() override
            {
                Initialize();
            }

            void UpdateAI(uint32 diff) override
            {
                _summonTimer.Update(diff);
                if (_summonTimer.Passed())
                {
                    DoCastAOE(SPELL_FREYA_S_WARD);
                    _summonTimer.Reset(30s);
                }
            }

        private:
            TimeTracker _summonTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_freyas_wardAI>(creature);
        }
};

class npc_freya_ward_summon : public CreatureScript
{
    public:
        npc_freya_ward_summon() : CreatureScript("npc_freya_ward_summon") { }

        struct npc_freya_ward_summonAI : public ScriptedAI
        {
            npc_freya_ward_summonAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
            }

            void IsSummonedBy(WorldObject* /*summoner*/) override
            {
                if (InstanceScript* instance = me->GetInstanceScript())
                    if (Creature* leviathan = instance->GetCreature(DATA_FLAME_LEVIATHAN))
                        leviathan->AI()->JustSummoned(me);
            }

            void Initialize()
            {
                lashTimer = 5000;
            }

            void Reset() override
            {
                Initialize();
                me->GetMotionMaster()->MoveRandom(100.0f);
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                if (lashTimer <= diff)
                {
                    DoCastVictim(SPELL_LASH);
                    lashTimer = 20000;
                }
                else
                    lashTimer -= diff;

                DoMeleeAttackIfReady();
            }

        private:
            uint32 lashTimer;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_freya_ward_summonAI>(creature);
        }
};

enum BrannBronzebeardGossips
{
    GOSSIP_MENU_BRANN_BRONZEBEARD   = 10355,
    GOSSIP_OPTION_BRANN_BRONZEBEARD = 0
};

class npc_brann_bronzebeard_ulduar_intro : public CreatureScript
{
    public:
        npc_brann_bronzebeard_ulduar_intro() : CreatureScript("npc_brann_bronzebeard_ulduar_intro") { }

        struct npc_brann_bronzebeard_ulduar_introAI : public ScriptedAI
        {
            npc_brann_bronzebeard_ulduar_introAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = creature->GetInstanceScript();
            }

            bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
            {
                if (menuId == GOSSIP_MENU_BRANN_BRONZEBEARD && gossipListId == GOSSIP_OPTION_BRANN_BRONZEBEARD)
                {
                    me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    player->PlayerTalkClass->SendCloseGossip();
                    _instance->SetGuidData(DATA_FL_INTRO_PLAYER, player->GetGUID());
                    _instance->SetData(DATA_FL_INTRO_START, 1);
                }
                return false;
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_brann_bronzebeard_ulduar_introAI>(creature);
        }
};

enum LoreKeeperGossips
{
    GOSSIP_MENU_LORE_KEEPER_CONFIRM   = 10477,
    GOSSIP_OPTION_LORE_KEEPER_CONFIRM = 0
};

class npc_lorekeeper : public CreatureScript
{
    public:
        npc_lorekeeper() : CreatureScript("npc_lorekeeper") { }

        struct npc_lorekeeperAI : public ScriptedAI
        {
            npc_lorekeeperAI(Creature* creature) : ScriptedAI(creature)
            {
                _instance = creature->GetInstanceScript();
            }

            bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
            {
                if (menuId == GOSSIP_MENU_LORE_KEEPER_CONFIRM && gossipListId == GOSSIP_OPTION_LORE_KEEPER_CONFIRM)
                {
                    if (_instance->GetData(DATA_ACTIVE_TOWERS) != 0)
                    {
                        player->PlayerTalkClass->SendCloseGossip();
                        return false;
                    }
                    player->PlayerTalkClass->SendCloseGossip();
                    _instance->instance->LoadGrid(FlameLeviathanCenter.GetPositionX(), FlameLeviathanCenter.GetPositionY());
                    _instance->SetData(DATA_ACTIVE_TOWERS, 1);
                    if (Creature* brann = _instance->GetCreature(DATA_BRANN_BRONZEBEARD_INTRO))
                        brann->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    Talk(SAY_LORE_KEEPER_CONFIRMED);
                    _instance->SetGuidData(DATA_FL_HARDMODE_PLAYER, player->GetGUID());
                    _instance->SetData(DATA_FL_HARDMODE_CONFIRMED, 1);
                }
                return false;
            }

        private:
            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_lorekeeperAI>(creature);
        }
};

struct EG_npc_salvaged_demolisher_mechanic_seat : public VehicleAI
{
    EG_npc_salvaged_demolisher_mechanic_seat(Creature* creature) : VehicleAI(creature) { }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (apply)
            if (who->GetEntry() == NPC_LIQUID_PYRITE)
            {
                me->CastSpell(me, SPELL_LIQUID_PYRITE_RELOAD, true);
                who->ToCreature()->DespawnOrUnsummon(1s);
            }
    }
};

struct EG_npc_flame_leviathan_outro_flying_machine : public PassiveAI
{
    EG_npc_flame_leviathan_outro_flying_machine(Creature* creature) : PassiveAI(creature) { }

    void MovementInform(uint32 type, uint32 id) override
    {
        if ((type == EFFECT_MOTION_TYPE || type == POINT_MOTION_TYPE) && id == POINT_FL_OUTRO_FLYING_MACHINE_LAND)
            if (InstanceScript* instance = me->GetInstanceScript())
                instance->SetData(DATA_FL_OUTRO_FLYING_MACHINE_LANDED, 0);
    }
};

class go_ulduar_tower : public GameObjectScript
{
    public:
        go_ulduar_tower() : GameObjectScript("go_ulduar_tower") { }

        struct go_ulduar_towerAI : public GameObjectAI
        {
            go_ulduar_towerAI(GameObject* go) : GameObjectAI(go), instance(go->GetInstanceScript()) { }

            InstanceScript* instance;

            void Destroyed(WorldObject* /*attacker*/, uint32 /*eventId*/) override
            {
                switch (me->GetEntry())
                {
                    case GO_TOWER_OF_STORMS:
                        instance->ProcessEvent(me, EVENT_TOWER_OF_STORM_DESTROYED);
                        break;
                    case GO_TOWER_OF_FLAMES:
                        instance->ProcessEvent(me, EVENT_TOWER_OF_FLAMES_DESTROYED);
                        break;
                    case GO_TOWER_OF_FROST:
                        instance->ProcessEvent(me, EVENT_TOWER_OF_FROST_DESTROYED);
                        break;
                    case GO_TOWER_OF_LIFE:
                        instance->ProcessEvent(me, EVENT_TOWER_OF_LIFE_DESTROYED);
                        break;
                }

                if (Creature* trigger = me->FindNearestCreature(NPC_ULDUAR_GAUNTLET_GENERATOR, 15.0f, true))
                    trigger->DisappearAndDie();
            }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return GetUlduarAI<go_ulduar_towerAI>(go);
        }
};

class achievement_three_car_garage_demolisher : public AchievementCriteriaScript
{
    public:
        achievement_three_car_garage_demolisher() : AchievementCriteriaScript("achievement_three_car_garage_demolisher") { }

        bool OnCheck(Player* source, Unit* /*target*/) override
        {
            if (Creature* vehicle = source->GetVehicleCreatureBase())
            {
                if (vehicle->GetEntry() == VEHICLE_DEMOLISHER)
                    return true;
            }

            return false;
        }
};

class achievement_three_car_garage_chopper : public AchievementCriteriaScript
{
    public:
        achievement_three_car_garage_chopper() : AchievementCriteriaScript("achievement_three_car_garage_chopper") { }

        bool OnCheck(Player* source, Unit* /*target*/) override
        {
            if (Creature* vehicle = source->GetVehicleCreatureBase())
            {
                if (vehicle->GetEntry() == VEHICLE_CHOPPER)
                    return true;
            }

            return false;
        }
};

class achievement_three_car_garage_siege : public AchievementCriteriaScript
{
    public:
        achievement_three_car_garage_siege() : AchievementCriteriaScript("achievement_three_car_garage_siege") { }

        bool OnCheck(Player* source, Unit* /*target*/) override
        {
            if (Creature* vehicle = source->GetVehicleCreatureBase())
            {
                if (vehicle->GetEntry() == VEHICLE_SIEGE)
                    return true;
            }

            return false;
        }
};

class achievement_shutout : public AchievementCriteriaScript
{
    public:
        achievement_shutout() : AchievementCriteriaScript("achievement_shutout") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (target)
                if (Creature* leviathan = target->ToCreature())
                    if (leviathan->AI()->GetData(DATA_SHUTOUT))
                        return true;

            return false;
        }
};

class achievement_unbroken : public AchievementCriteriaScript
{
    public:
        achievement_unbroken() : AchievementCriteriaScript("achievement_unbroken") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (target)
                if (InstanceScript* instance = target->GetInstanceScript())
                    return instance->GetData(DATA_UNBROKEN) != 0;

            return false;
        }
};

class achievement_orbital_bombardment : public AchievementCriteriaScript
{
    public:
        achievement_orbital_bombardment() : AchievementCriteriaScript("achievement_orbital_bombardment") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) >= 1)
                    return true;

            return false;
        }
};

class achievement_orbital_devastation : public AchievementCriteriaScript
{
    public:
        achievement_orbital_devastation() : AchievementCriteriaScript("achievement_orbital_devastation") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) >= 2)
                    return true;

            return false;
        }
};

class achievement_nuked_from_orbit : public AchievementCriteriaScript
{
    public:
        achievement_nuked_from_orbit() : AchievementCriteriaScript("achievement_nuked_from_orbit") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) >= 3)
                    return true;

            return false;
        }
};

class achievement_orbit_uary : public AchievementCriteriaScript
{
    public:
        achievement_orbit_uary() : AchievementCriteriaScript("achievement_orbit_uary") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) == 4)
                    return true;

            return false;
        }
};

// 62399 - Overload Circuit
class spell_overload_circuit : public AuraScript
{
    PrepareAuraScript(spell_overload_circuit);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SYSTEMS_SHUTDOWN });
    }

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        if (!GetTarget()->GetMap()->IsDungeon() || int32(GetTarget()->GetAppliedAuras().count(GetId())) < (GetTarget()->GetMap()->Is25ManRaid() ? 4 : 2))
            return;

        GetTarget()->CastSpell(nullptr, SPELL_SYSTEMS_SHUTDOWN, true);
        if (Unit* veh = GetTarget()->GetVehicleBase())
            veh->CastSpell(nullptr, SPELL_SYSTEMS_SHUTDOWN, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_overload_circuit::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 62292 - Blaze
class spell_tar_blaze : public AuraScript
{
    PrepareAuraScript(spell_tar_blaze);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        GetTarget()->CastSpell(nullptr, aurEff->GetSpellEffectInfo().TriggerSpell, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_tar_blaze::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 62290 - SCHOOL_DAMAGE periodic blast from a burning Pool of Tar (triggered by 62292 every 1s)
//   DBC effect 0:
//     TargetA = 22 (TARGET_SRC_CASTER, sets source position)
//     TargetB = 30 (TARGET_UNIT_SRC_AREA_ALLY, units within 15y, ALLY check vs caster)
class EG_spell_pool_of_tar_blaze_damage : public SpellScript
{
    PrepareSpellScript(EG_spell_pool_of_tar_blaze_damage);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        SpellInfo const* spellInfo = GetSpellInfo();
        float radius = spellInfo->GetEffect(EFFECT_0).CalcRadius(caster);
        targets.clear();

        std::list<Unit*> units;
        Trinity::AnyUnitInObjectRangeCheck check(caster, radius);
        Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(caster, units, check);
        Cell::VisitAllObjects(caster, searcher, radius);

        for (Unit* u : units)
        {
            if (u == caster || !u->IsAlive())
                continue;
            if (spellInfo->CheckTarget(caster, u, true) != SPELL_CAST_OK)
                continue;
            if (u->GetVehicle())
                continue;
            if (u->HasUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE))
                continue;
            if (u->IsImmunedToSpell(spellInfo, caster))
                continue;
            targets.push_back(u);
        }
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_pool_of_tar_blaze_damage::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// 64414 - Load into Catapult
class spell_load_into_catapult : public SpellScriptLoader
{
    enum Spells
    {
        SPELL_PASSENGER_LOADED = 62340,
    };

    public:
        spell_load_into_catapult() : SpellScriptLoader("spell_load_into_catapult") { }

        class spell_load_into_catapult_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_load_into_catapult_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* owner = GetOwner()->ToUnit();
                if (!owner)
                    return;

                owner->CastSpell(owner, SPELL_PASSENGER_LOADED, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* owner = GetOwner()->ToUnit();
                if (!owner)
                    return;

                owner->RemoveAurasDueToSpell(SPELL_PASSENGER_LOADED);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_load_into_catapult_AuraScript::OnApply, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_load_into_catapult_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_load_into_catapult_AuraScript();
        }
};

// 62705 - Auto-repair
class spell_auto_repair : public SpellScriptLoader
{
    enum Misc
    {
        SPELL_AUTO_REPAIR = 62705,
        NPC_EARTHEN_STONESHAPER = 33620,
        BROADCAST_TEXT_AUTO_REPAIR = 33538, // "Automatic repair sequence initiated."
    };

    public:
        spell_auto_repair() : SpellScriptLoader("spell_auto_repair") { }

        class spell_auto_repair_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_auto_repair_SpellScript);

            void CheckCooldownForTarget(SpellMissInfo missInfo)
            {
                if (missInfo != SPELL_MISS_NONE)
                    return;

                if (GetHitUnit()->HasAuraEffect(SPELL_AUTO_REPAIR, EFFECT_2))   // Check presence of dummy aura indicating cooldown
                {
                    PreventHitEffect(EFFECT_0);
                    PreventHitDefaultEffect(EFFECT_1);
                    PreventHitDefaultEffect(EFFECT_2);
                    //! Currently this doesn't work: if we call PreventHitAura(), the existing aura will be removed
                    //! because of recent aura refreshing changes. Since removing the existing aura negates the idea
                    //! of a cooldown marker, we just let the dummy aura refresh itself without executing the other spelleffects.
                    //! The spelleffects can be executed by letting the dummy aura expire naturally.
                    //! This is a temporary solution only.
                    //PreventHitAura();
                }
            }

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Vehicle* vehicle = GetHitUnit()->GetVehicleKit();
                if (!vehicle)
                    return;

                Unit* driver = vehicle->GetPassenger(0);
                if (!driver)
                    return;

                for (auto const& seatPair : vehicle->Seats)
                    if (Unit* passenger = ObjectAccessor::GetUnit(*GetHitUnit(), seatPair.second.Passenger.Guid))
                        if (passenger->GetEntry() == NPC_EARTHEN_STONESHAPER)
                        {
                            passenger->TextEmote(BROADCAST_TEXT_AUTO_REPAIR, driver, true);
                            break;
                        }

                InstanceScript* instance = driver->GetInstanceScript();
                if (!instance)
                    return;

                // Actually should/could use basepoints (100) for this spell effect as percentage of health, but oh well.
                vehicle->GetBase()->SetFullHealth();

                // For achievement
                instance->SetData(DATA_UNBROKEN, 0);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_auto_repair_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                BeforeHit += BeforeSpellHitFn(spell_auto_repair_SpellScript::CheckCooldownForTarget);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_auto_repair_SpellScript();
        }
};

// 62475 - Systems Shutdown
class spell_systems_shutdown : public SpellScriptLoader
{
    public:
        spell_systems_shutdown() : SpellScriptLoader("spell_systems_shutdown") { }

        class spell_systems_shutdown_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_systems_shutdown_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                //! This could probably in the SPELL_EFFECT_SEND_EVENT handler too:
                owner->StopMoving();
                owner->GetMotionMaster()->Clear(MOTION_PRIORITY_NORMAL);
                owner->AddUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                owner->SetUnitFlag(UNIT_FLAG_STUNNED);
                owner->RemoveAurasDueToSpell(SPELL_GATHERING_SPEED);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                owner->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                owner->RemoveUnitFlag(UNIT_FLAG_STUNNED);
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_systems_shutdown_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_systems_shutdown_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_systems_shutdown_AuraScript();
        }
};

class FlameLeviathanPursuedTargetSelector
{
    enum Area
    {
        AREA_FORMATION_GROUNDS = 4652,
    };

    public:
        explicit FlameLeviathanPursuedTargetSelector() { };

        bool operator()(WorldObject* target) const
        {
            //! No players, only vehicles. Pursue is never cast on players.
            Creature* creatureTarget = target->ToCreature();
            if (!creatureTarget)
                return true;

            //! NPC entries must match
            if (creatureTarget->GetEntry() != NPC_SALVAGED_DEMOLISHER && creatureTarget->GetEntry() != NPC_SALVAGED_SIEGE_ENGINE && creatureTarget->GetEntry() != NPC_SALVAGED_CHOPPER)
                return true;

            //! NPC must be a valid vehicle installation
            Vehicle* vehicle = creatureTarget->GetVehicleKit();
            if (!vehicle)
                return true;

            //! Entity needs to be in appropriate area
            if (target->GetAreaId() != AREA_FORMATION_GROUNDS)
                return true;

            //! Vehicle must be in use by player
            bool playerFound = false;
            for (auto itr = vehicle->Seats.begin(); itr != vehicle->Seats.end() && !playerFound; ++itr)
                if (itr->second.Passenger.Guid.IsPlayer())
                    playerFound = true;

            return !playerFound;
        }
};

// 62374 - Pursued
class spell_pursue : public SpellScriptLoader
{
    public:
        spell_pursue() : SpellScriptLoader("spell_pursue") { }

        class spell_pursue_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pursue_SpellScript);

        public:
            spell_pursue_SpellScript() { }

        private:
            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(FlameLeviathanPursuedTargetSelector());
                if (!targets.empty())
                {
                    //! In the end, only one target should be selected
                    _target = Trinity::Containers::SelectRandomContainerElement(targets);
                    FilterTargetsSubsequently(targets);
                }
            }

            void FilterTargetsSubsequently(std::list<WorldObject*>& targets)
            {
                targets.clear();
                if (_target)
                    targets.push_back(_target);
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pursue_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pursue_SpellScript::FilterTargetsSubsequently, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            WorldObject* _target = nullptr;
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pursue_SpellScript();
        }
};

// 62324 - Throw Passenger
class spell_vehicle_throw_passenger : public SpellScriptLoader
{
    public:
        spell_vehicle_throw_passenger() : SpellScriptLoader("spell_vehicle_throw_passenger") { }

        class spell_vehicle_throw_passenger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_vehicle_throw_passenger_SpellScript);

            void RestoreDest(SpellDestination& dest)
            {
                if (WorldLocation const* aimed = GetExplTargetDest())
                    dest.Relocate(*aimed);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Spell* baseSpell = GetSpell();
                SpellCastTargets targets = baseSpell->m_targets;
                int32 damage = GetEffectValue();
                if (targets.HasTraj())
                    if (Vehicle* vehicle = GetCaster()->GetVehicleKit())
                        if (Unit* passenger = vehicle->GetPassenger(damage - 1))
                        {
                            // use 99 because it is 3d search
                            std::list<WorldObject*> targetList;
                            Trinity::WorldObjectSpellAreaTargetCheck check(99, GetExplTargetDest(), GetCaster(), GetCaster(), GetSpellInfo(), TARGET_CHECK_DEFAULT, nullptr);
                            Trinity::WorldObjectListSearcher<Trinity::WorldObjectSpellAreaTargetCheck> searcher(GetCaster(), targetList, check);
                            Cell::VisitAllObjects(GetCaster(), searcher, 99.0f);
                            float minDist = 99 * 99;
                            Unit* target = nullptr;
                            for (std::list<WorldObject*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                            {
                                if (Unit* unit = (*itr)->ToUnit())
                                    if (unit->GetEntry() == NPC_SEAT)
                                        if (Vehicle* seat = unit->GetVehicleKit())
                                            if (!seat->GetPassenger(0))
                                                if (Unit* device = seat->GetPassenger(2))
                                                    if (!device->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                                                    {
                                                        float dist = unit->GetExactDistSq(targets.GetDstPos());
                                                        if (dist < minDist)
                                                        {
                                                            minDist = dist;
                                                            target = unit;
                                                        }
                                                    }
                            }
                            if (target && target->IsWithinDist2d(targets.GetDstPos(), GetEffectInfo().CalcRadius() * 2)) // now we use *2 because the location of the seat is not correct
                                passenger->EnterVehicle(target, 0);
                            else
                            {
                                passenger->ExitVehicle();
                                passenger->GetMotionMaster()->MoveJump(*targets.GetDstPos(), targets.GetSpeedXY(), targets.GetSpeedZ());
                            }
                        }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_vehicle_throw_passenger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_vehicle_throw_passenger_SpellScript::RestoreDest, EFFECT_0, TARGET_DEST_TRAJ);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_vehicle_throw_passenger_SpellScript();
        }
};

// 62910 - Mimiron's Inferno
class EG_spell_flame_leviathan_mimirons_inferno : public SpellScript
{
    PrepareSpellScript(EG_spell_flame_leviathan_mimirons_inferno);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* obj) -> bool
        {
            // remove GMs
            if (Player* player = obj->ToPlayer())
                return player->IsGameMaster();

            return !obj->IsUnit() || (!obj->GetCharmerOrOwnerPlayerOrPlayerItself() && !obj->ToUnit()->GetControllingPlayer());
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_flame_leviathan_mimirons_inferno::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_flame_leviathan_mimirons_inferno::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

// 62479 Grab Crate - Main (Salvaged Demolisher mechanic seat)
// -- Effect 0 - SPELL_EFFECT_DUMMY - 62480
// -- Effect 1 - SPELL_EFFECT_SCRIPT_EFFECT - 62480
// -- Effect 2 - 62482
// 62480 Grenades
// 62482 Grab Crate - Triggered
// 67372 Grab Pyrite (Salvaged Chopper)
// -- Effect 2 - 67387
// 67387 Grab Crate
// -- Effect 0 - SPELL_EFFECT_SCRIPT_EFFECT - 63827
// 63827 Ride Vehicle (rear seat)
class EG_spell_flame_leviathan_grab_crate_triggered : public SpellScript
{
    PrepareSpellScript(EG_spell_flame_leviathan_grab_crate_triggered);

    void HandleDummy(SpellEffIndex effIndex)
    {
        PreventHitEffect(effIndex);
        Unit* caster = GetCaster();
        if (!caster)
            return;

        if (Vehicle* vehicle = caster->GetVehicle())
        {
            // Demolisher: the mechanic seat pod grabs the canister for its parent vehicle
            GetHitUnit()->CastSpell(vehicle->GetBase(), GetSpellInfo()->GetEffect(effIndex).CalcValue(), true);
        }
        else if (Vehicle* vehicleKit = caster->GetVehicleKit())
        {
            // Chopper: the canister is loaded into the rear seat and can be ferried around
            if (Unit* passenger = vehicleKit->GetPassenger(1))
            {
                // Never eject a player riding the rear seat to make room
                if (passenger->GetTypeId() == TYPEID_PLAYER)
                    return;
                passenger->ExitVehicle();
            }
            GetHitUnit()->CastSpell(caster, GetSpellInfo()->GetEffect(effIndex).CalcValue(), true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_flame_leviathan_grab_crate_triggered::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum SalvagedVehicleSeating
{
    MAP_ULDUAR = 603,
    NPC_SALVAGED_SIEGE_TURRET = 33067,
    NPC_SALVAGED_DEMOLISHER_MECHANIC = 33167,
    SEAT_DRIVER = 0,
    SEAT_DEMOLISHER_MECHANIC_ON_BODY = 1,
    SEAT_SIEGE_TURRET_ON_BODY = 7
};

class EG_spell_salvaged_vehicle_ride_gate : public SpellScript
{
    PrepareSpellScript(EG_spell_salvaged_vehicle_ride_gate);

    bool Load() override
    {
        Unit* caster = GetCaster();
        return caster && caster->GetMapId() == MAP_ULDUAR;
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();
        Unit* target = GetExplTargetUnit();
        if (!caster || !target)
            return SPELL_FAILED_BAD_TARGETS;

        Vehicle* vehicle = target->GetVehicleKit();
        if (!vehicle)
            return SPELL_FAILED_BAD_TARGETS;

        if (caster->GetVehicleBase() == target)
            return SPELL_FAILED_DONT_REPORT;

        if (!vehicle->GetPassenger(SEAT_DRIVER))
            return SPELL_CAST_OK;

        uint8 accessorySeat = 0;
        uint32 accessoryEntry = 0;
        switch (target->GetEntry())
        {
            case VEHICLE_SIEGE:
                accessorySeat = SEAT_SIEGE_TURRET_ON_BODY;
                accessoryEntry = NPC_SALVAGED_SIEGE_TURRET;
                break;
            case VEHICLE_DEMOLISHER:
                accessorySeat = SEAT_DEMOLISHER_MECHANIC_ON_BODY;
                accessoryEntry = NPC_SALVAGED_DEMOLISHER_MECHANIC;
                break;
            default:
                return SPELL_CAST_OK;
        }

        if (Unit* accessoryUnit = vehicle->GetPassenger(accessorySeat))
            if (Creature* accessory = accessoryUnit->ToCreature())
                if (accessory->GetEntry() == accessoryEntry)
                    if (Vehicle* accessoryKit = accessory->GetVehicleKit())
                        if (!accessoryKit->GetPassenger(SEAT_DRIVER))
                            accessory->HandleSpellClick(caster, SEAT_DRIVER);

        return SPELL_FAILED_DONT_REPORT;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(EG_spell_salvaged_vehicle_ride_gate::CheckCast);
    }
};

void AddSC_boss_flame_leviathan()
{
    new boss_flame_leviathan();
    new boss_flame_leviathan_seat();
    new boss_flame_leviathan_defense_turret();
    new boss_flame_leviathan_defense_cannon();
    new boss_flame_leviathan_overload_device();
    new npc_mechanolift();
    new npc_pool_of_tar();
    new npc_colossus();
    new npc_thorims_hammer();
    new npc_mimirons_inferno();
    new npc_hodirs_fury();
    new npc_freyas_ward();
    new npc_freya_ward_summon();
    new npc_brann_bronzebeard_ulduar_intro();
    new npc_lorekeeper();
    RegisterUlduarCreatureAI(EG_npc_salvaged_demolisher_mechanic_seat);
    RegisterUlduarCreatureAI(EG_npc_flame_leviathan_outro_flying_machine);
    new go_ulduar_tower();

    new achievement_three_car_garage_demolisher();
    new achievement_three_car_garage_chopper();
    new achievement_three_car_garage_siege();
    new achievement_shutout();
    new achievement_unbroken();
    new achievement_orbital_bombardment();
    new achievement_orbital_devastation();
    new achievement_nuked_from_orbit();
    new achievement_orbit_uary();

    RegisterSpellScript(spell_overload_circuit);
    RegisterSpellScript(spell_tar_blaze);
    RegisterSpellScript(EG_spell_pool_of_tar_blaze_damage);
    new spell_load_into_catapult();
    new spell_auto_repair();
    new spell_systems_shutdown();
    new spell_pursue();
    new spell_vehicle_throw_passenger();
    RegisterSpellScript(EG_spell_flame_leviathan_mimirons_inferno);
    RegisterSpellScript(EG_spell_flame_leviathan_grab_crate_triggered);
    RegisterSpellScript(EG_spell_salvaged_vehicle_ride_gate);
}
