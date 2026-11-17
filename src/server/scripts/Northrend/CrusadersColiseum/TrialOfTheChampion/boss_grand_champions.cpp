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
#include "CombatAI.h"
#include "Containers.h"
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TaskScheduler.h"
#include "Vehicle.h"

enum GrandChampionsMisc
{
    SPELL_DEFEND_PERIODIC = 64101,
    SPELL_DEFEND = 62719,
    SPELL_DEFEND_PLAYER = 62552,
    SPELL_LESSER_DEFEND = 64100,
    SPELL_SHIELD_BREAKER = 68504,
    SPELL_CHARGE = 68301,
    SPELL_CHARGE_VEHICLE = 68307,
    SPELL_CHARGE_DAMAGE = 68321,
    SPELL_LESSER_CHARGE = 63010,
    SPELL_FEAR_SELF = 68326,
    SPELL_UNHORSED = 65918,
    SPELL_TRAMPLED = 67867,
    SPELL_RIDE_ARGENT_MOUNT = 69692,
    SPELL_MOUNT_FULL_HEAL = 69693,
    SPELL_KNEEL = 68442,
    SPELL_PENNANT_SELECTOR = 67347,
    SPELL_SPECTATOR_FX_CHANT = 66677,

    SPELL_PENNANT_ORGRIMMAR = 63433,
    SPELL_PENNANT_SILVERMOON = 63403,
    SPELL_PENNANT_THUNDER_BLUFF = 63436,
    SPELL_PENNANT_SENJIN = 63399,
    SPELL_PENNANT_UNDERCITY = 63430,
    SPELL_PENNANT_STORMWIND = 62594,
    SPELL_PENNANT_GNOMEREGAN = 63396,
    SPELL_PENNANT_EXODAR = 63423,
    SPELL_PENNANT_DARNASSUS = 63406,
    SPELL_PENNANT_IRONFORGE = 63427,

    SPELL_MORTAL_STRIKE = 67542,
    SPELL_BLADESTORM = 67541,
    SPELL_ROLLING_THROW = 67546,
    SPELL_ROLLING_THROW_GRAB = 67730,
    SPELL_INTERCEPT = 67540,

    SPELL_FIREBALL = 66042,
    SPELL_POLYMORPH = 66043,
    SPELL_BLAST_WAVE = 66044,
    SPELL_HASTE = 66045,

    SPELL_CHAIN_LIGHTNING = 67529,
    SPELL_HEALING_WAVE = 67528,
    SPELL_EARTH_SHIELD = 67530,
    SPELL_HEX_OF_MENDING = 67534,
    SPELL_HEX_OF_MENDING_HEAL = 67535,

    SPELL_SHOOT = 65868,
    SPELL_MULTI_SHOT = 66081,
    SPELL_LIGHTNING_ARROWS = 66083,
    SPELL_LIGHTNING_ARROWS_PROC = 66085,
    SPELL_DISENGAGE = 68340,
    SPELL_DISENGAGE_JUMP = 68339,

    SPELL_DEADLY_POISON = 67711,
    SPELL_EVISCERATE = 67709,
    SPELL_FAN_OF_KNIVES = 67706,
    SPELL_POISON_BOTTLE = 67701,

    ITEM_CHAMPION_LANCE = 46106,

    SAY_TRAMPLED = 0,

    POINT_REMOUNT,
    POINT_LEAVE,

    STATE_MOUNTED = 0,
    STATE_UNHORSED,
    STATE_LEAVING,
    STATE_ON_FOOT,
    STATE_DEFEATED
};

struct GrandChampionAI : public ScriptedAI
{
    GrandChampionAI(Creature* creature) : ScriptedAI(creature), Instance(creature->GetInstanceScript()), _state(STATE_MOUNTED), _active(false), _paused(false)
    {
        Scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();

        DoCastSelf(SPELL_PENNANT_SELECTOR, true);

        if (Instance->GetData(DATA_CHAMPION_FOOT_PHASE))
        {
            _state = STATE_ON_FOOT;
            me->SetFaction(FACTION_MONSTER_2);
            me->SetReactState(REACT_AGGRESSIVE);
            me->SetImmuneToPC(false);
            return;
        }

        _slotHome = me->GetHomePosition();
        me->SetReactState(REACT_PASSIVE);
        me->SetImmuneToPC(true);
        me->SetVirtualItem(0, ITEM_CHAMPION_LANCE);
        SetCombatMovement(false);
        DoCastSelf(SPELL_DEFEND_PERIODIC, true);
        DoCastSelf(SPELL_SPECTATOR_FX_CHANT, true);
    }

    uint32 GetData(uint32 type) const override
    {
        return type == DATA_CHAMPION_MOUNTED && _state == STATE_MOUNTED ? 1 : 0;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ENGAGE:
                _active = true;
                _paused = false;
                me->SetFaction(FACTION_MONSTER_2);
                me->SetImmuneToPC(false);
                me->SetReactState(REACT_AGGRESSIVE);
                if (_state == STATE_UNHORSED)
                    break;

                if (Creature* mount = GetMount())
                {
                    if (mount->GetEntry() != NPC_ARGENT_BATTLEWORG_ALLIANCE && mount->GetEntry() != NPC_ARGENT_WARHORSE_HORDE)
                        mount->SetFaction(FACTION_MONSTER_2);
                    mount->SetImmuneToPC(false);
                }
                DoZoneInCombat();
                break;
            case ACTION_NO_ALIVE_PLAYERS:
            {
                if (_state != STATE_MOUNTED && _state != STATE_UNHORSED)
                {
                    if (me->IsEngaged())
                        EnterEvadeMode(EVADE_REASON_OTHER);
                    break;
                }

                if (!_active || _paused)
                    break;

                _paused = true;
                Instance->SetData(DATA_MOUNTED_WIPE, 0);
                me->SetReactState(REACT_PASSIVE);
                me->SetImmuneToPC(true);
                if (_state == STATE_UNHORSED || !me->GetVehicle())
                {
                    if (me->IsEngaged())
                    {
                        me->CombatStop(true);
                        EngagementOver();
                    }
                    me->SetFullHealth();
                }
                else if (me->IsEngaged())
                    ResetMounted();

                if (_state != STATE_MOUNTED)
                    break;

                Scheduler.CancelAll();
                if (Creature* mount = GetMount())
                {
                    ReturnMountHome(mount);
                    mount->SetImmuneToPC(true);
                }
                break;
            }
            case ACTION_RESET:
                if (_state == STATE_DEFEATED)
                {
                    _state = STATE_ON_FOOT;
                    me->RemoveAurasDueToSpell(SPELL_KNEEL);
                    me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                    me->SetIsCombatDisallowed(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetFullHealth();
                }

                if (_state == STATE_ON_FOOT && me->IsEngaged())
                    EnterEvadeMode(EVADE_REASON_OTHER);
                break;
            case ACTION_LEAVE:
                _state = STATE_LEAVING;
                Scheduler.CancelAll();
                me->InterruptNonMeleeSpells(true);
                me->RemoveAurasDueToSpell(SPELL_TRAMPLED);
                me->RemoveAurasDueToSpell(SPELL_UNHORSED);
                me->SetReactState(REACT_PASSIVE);
                me->SetImmuneToPC(true);
                me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                me->CombatStop(true);
                me->GetMotionMaster()->Clear();
                me->DespawnOrUnsummon(17s);
                Scheduler.Schedule(3s, [this](TaskContext context)
                {
                    me->CombatStop(true);
                    DoCastSelf(SPELL_FULL_HEAL, true);
                    context.Schedule(3s, [this](TaskContext /*context*/)
                    {
                        me->SetWalk(false);
                        me->GetMotionMaster()->MovePoint(POINT_LEAVE, { .Destination = Position(746.672f, 666.688f, 411.758f), .Velocity = 10.0f });
                    });
                });
                break;
            default:
                break;
        }
    }

    void AttackStart(Unit* who) override
    {
        if (_state != STATE_MOUNTED && _state != STATE_ON_FOOT)
            return;

        ScriptedAI::AttackStart(who);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (_state != STATE_MOUNTED && _state != STATE_UNHORSED && _state != STATE_ON_FOOT)
            return;

        if (_state != STATE_ON_FOOT)
        {
            if (_state == STATE_MOUNTED)
            {
                if (Creature* mount = GetMount())
                    mount->SetImmuneToPC(false);
                ScheduleMountedAbilities();
            }
            return;
        }

        Instance->SetBossState(DATA_GRAND_CHAMPIONS, IN_PROGRESS);
        me->CallForHelp(50.0f);
        DoZoneInCombat();
        ScheduleFootAbilities();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        if (_state == STATE_LEAVING || _state == STATE_DEFEATED)
        {
            damage = 0;
            return;
        }

        if (damage < me->GetHealth())
            return;

        damage = me->GetHealth() - 1;

        if (Unit* mount = me->GetVehicleBase())
        {
            mount->CastSpell(me, SPELL_UNHORSED, true);
            me->ExitVehicle();
            if (Creature* mountCreature = mount->ToCreature())
                mountCreature->DespawnOrUnsummon(2s);

            _state = STATE_UNHORSED;
            _mountGUID.Clear();
            Scheduler.CancelAll();
            me->AttackStop();
            me->RemoveAurasDueToSpell(SPELL_DEFEND_PERIODIC);
            me->RemoveAurasDueToSpell(SPELL_DEFEND);
            me->LoadEquipment(1, true);

            Scheduler.Schedule(10s, [this](TaskContext seek)
            {
                Creature* mount = ObjectAccessor::GetCreature(*me, _mountGUID);
                if (!mount || !IsFreeMount(mount))
                {
                    mount = nullptr;
                    std::vector<Creature*> mounts;
                    me->GetCreatureListWithEntryInGrid(mounts, me->GetMap()->ToInstanceMap()->GetTeamIdInInstance() == TEAM_ALLIANCE ? NPC_ARGENT_BATTLEWORG_ALLIANCE : NPC_ARGENT_WARHORSE_HORDE, 100.0f);
                    for (Creature* candidate : mounts)
                    {
                        if (!IsFreeMount(candidate))
                            continue;

                        if (!mount || me->GetDistance(candidate) < me->GetDistance(mount))
                            mount = candidate;
                    }

                    _mountGUID = mount ? mount->GetGUID() : ObjectGuid::Empty;
                    if (mount)
                        me->GetMotionMaster()->Clear();
                }

                if (mount && !me->HasUnitState(UNIT_STATE_STUNNED) && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                {
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(POINT_REMOUNT, mount->GetNearPosition(3.0f, mount->GetRelativeAngle(me)));
                }

                seek.Repeat(1s);
            });

            Instance->SetData(DATA_CHAMPION_UNHORSED, 0);
            return;
        }

        if (_state != STATE_ON_FOOT)
            return;

        damage = 0;
        me->SetHealth(1);
        _state = STATE_DEFEATED;
        Scheduler.CancelAll();
        me->InterruptNonMeleeSpells(true);
        me->RemoveAurasDueToSpell(SPELL_BLADESTORM);
        me->RemoveAurasDueToSpell(SPELL_LIGHTNING_ARROWS);
        if (Vehicle* vehicle = me->GetVehicleKit())
            vehicle->RemoveAllPassengers();
        me->AttackStop();
        me->SetReactState(REACT_PASSIVE);
        me->SetIsCombatDisallowed(true);
        me->CombatStop(true);
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        DoCastSelf(SPELL_KNEEL, true);
        me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        Instance->SetData(DATA_CHAMPION_DEFEATED, 0);
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case POINT_REMOUNT:
                if (Creature* mount = ObjectAccessor::GetCreature(*me, _mountGUID))
                {
                    if (_state != STATE_UNHORSED || !IsFreeMount(mount))
                        break;

                    _state = STATE_MOUNTED;
                    Scheduler.CancelAll();
                    me->SetVirtualItem(0, ITEM_CHAMPION_LANCE);
                    DoCast(mount, SPELL_RIDE_ARGENT_MOUNT, true);
                    DoCastSelf(SPELL_DEFEND_PERIODIC, true);
                    if (_paused)
                    {
                        ReturnMountHome(mount);
                        break;
                    }

                    if (me->IsEngaged() && !me->IsInCombat())
                    {
                        me->CombatStop(true);
                        EngagementOver();
                    }

                    mount->SetImmuneToPC(false);
                    bool const engaged = me->IsEngaged();
                    if (engaged)
                        ScheduleMountedAbilities();

                    Scheduler.Schedule(1s, [this, engaged](TaskContext /*context*/)
                    {
                        DoCastSelf(SPELL_FEAR_SELF);
                        if (!engaged)
                            DoZoneInCombat();
                    });
                }
                break;
            case POINT_LEAVE:
                me->DespawnOrUnsummon();
                break;
            default:
                break;
        }
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        switch (_state)
        {
            case STATE_MOUNTED:
                if (!_paused)
                    ResetMounted();
                break;
            case STATE_ON_FOOT:
                Scheduler.CancelAll();
                ScriptedAI::EnterEvadeMode(why);
                if (Instance->GetBossState(DATA_GRAND_CHAMPIONS) == IN_PROGRESS)
                    Instance->SetBossState(DATA_GRAND_CHAMPIONS, FAIL);
                break;
            default:
                break;
        }
    }

    void JustReachedHome() override
    {
        if (_state == STATE_ON_FOOT)
            DoCastSelf(SPELL_PENNANT_SELECTOR, true);
    }

    void UpdateAI(uint32 diff) override
    {
        Scheduler.Update(diff);

        if ((_state != STATE_MOUNTED && _state != STATE_ON_FOOT) || !UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

protected:
    virtual void ScheduleFootAbilities() = 0;

    InstanceScript* Instance;
    TaskScheduler Scheduler;

private:
    Creature* GetMount() const
    {
        if (Creature* mount = me->GetVehicleCreatureBase())
            return mount;

        return ObjectAccessor::GetCreature(*me, _mountGUID);
    }

    bool IsFreeMount(Creature const* mount) const
    {
        return mount->IsAlive() && mount->GetVehicleKit() && !mount->HasAuraType(SPELL_AURA_CONTROL_VEHICLE) && !mount->HasUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
    }

    void ResetMounted()
    {
        Scheduler.CancelAll();
        ScriptedAI::EnterEvadeMode(EVADE_REASON_OTHER);
        me->SetFullHealth();
        DoCastSelf(SPELL_PENNANT_SELECTOR, true);
        DoCastSelf(SPELL_DEFEND_PERIODIC, true);
        if (Creature* mount = me->GetVehicleCreatureBase())
            ReturnMountHome(mount);
    }

    void ReturnMountHome(Creature* mount)
    {
        me->SetHomePosition(_slotHome);
        mount->CombatStop(true);
        mount->SetHomePosition(_slotHome);
        mount->GetMotionMaster()->MoveTargetedHome();
    }

    void ScheduleMountedAbilities()
    {
        Scheduler.Schedule(1s, 7s, [this](TaskContext charge)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, [this](Unit* candidate)
            {
                return me->IsInRange(candidate, 8.0f, 25.0f);
            }))
                DoCast(target, SPELL_CHARGE);
            charge.Repeat(7s, 14s);
        })
        .Schedule(1s, 9s, [this](TaskContext shieldBreaker)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, [this](Unit* candidate)
            {
                return me->IsInRange(candidate, 10.0f, 30.0f);
            }))
                DoCast(target, SPELL_SHIELD_BREAKER);
            shieldBreaker.Repeat(3s, 14s);
        })
        .Schedule(6s, 12s, [this](TaskContext fearSelf)
        {
            DoCastSelf(SPELL_FEAR_SELF);
            fearSelf.Repeat(9s, 13s);
        });
    }

    ObjectGuid _mountGUID;
    Position _slotHome;
    uint8 _state;
    bool _active;
    bool _paused;
};

struct boss_warrior_toc5 : public GrandChampionAI
{
    boss_warrior_toc5(Creature* creature) : GrandChampionAI(creature) { }

    void OnSpellFailed(SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id != SPELL_ROLLING_THROW)
            return;

        if (Vehicle* vehicle = me->GetVehicleKit())
            vehicle->RemoveAllPassengers();
    }

    void ScheduleFootAbilities() override
    {
        Scheduler.Schedule(14s, 16s, [this](TaskContext mortalStrike)
        {
            if (DoCastVictim(SPELL_MORTAL_STRIKE) != SPELL_CAST_OK)
            {
                mortalStrike.Repeat(1s);
                return;
            }

            mortalStrike.Repeat(15s, 25s);
        })
        .Schedule(19s, 27s, [this](TaskContext bladestorm)
        {
            DoCastSelf(SPELL_BLADESTORM);
            bladestorm.Repeat(45s, 57s);
        })
        .Schedule(20s, 29s, [this](TaskContext rollingThrow)
        {
            if (me->GetVehicleKit())
            {
                DoCastVictim(SPELL_ROLLING_THROW_GRAB, true);
                DoCastSelf(SPELL_ROLLING_THROW);
                rollingThrow.Schedule(2s, [this](TaskContext /*context*/)
                {
                    if (Vehicle* vehicle = me->GetVehicleKit())
                        vehicle->RemoveAllPassengers();
                });
            }
            rollingThrow.Repeat(30s, 35s);
        })
        .Schedule(1s, [this](TaskContext /*intercept*/)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, [this](Unit* candidate)
            {
                return candidate->GetTypeId() == TYPEID_PLAYER && me->IsInRange(candidate, 8.0f, 25.0f);
            }))
                DoCast(target, SPELL_INTERCEPT);
        });
    }
};

struct boss_mage_toc5 : public GrandChampionAI
{
    boss_mage_toc5(Creature* creature) : GrandChampionAI(creature) { }

    void AttackStart(Unit* who) override
    {
        if (!who || me->GetVehicleBase() || me->HasReactState(REACT_PASSIVE))
        {
            GrandChampionAI::AttackStart(who);
            return;
        }

        AttackStartCaster(who, 20.0f);
    }

    void ScheduleFootAbilities() override
    {
        Scheduler.Schedule(1s, [this](TaskContext fireball)
        {
            DoCastVictim(SPELL_FIREBALL);
            fireball.Repeat(3s, 5s);
        })
        .Schedule(10s, 14s, [this](TaskContext polymorph)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f))
                DoCast(target, SPELL_POLYMORPH);
            polymorph.Repeat(20s, 28s);
        })
        .Schedule(7s, 19s, [this](TaskContext blastWave)
        {
            if (SelectTarget(SelectTargetMethod::Random, 0, 10.0f, true))
            {
                DoCastSelf(SPELL_BLAST_WAVE);
                blastWave.Repeat(15s, 20s);
            }
            else
                blastWave.Repeat(2s);
        })
        .Schedule(10s, 18s, [this](TaskContext haste)
        {
            DoCastSelf(SPELL_HASTE);
            haste.Repeat(22s, 28s);
        });
    }
};

struct boss_shaman_toc5 : public GrandChampionAI
{
    boss_shaman_toc5(Creature* creature) : GrandChampionAI(creature) { }

    void ScheduleFootAbilities() override
    {
        Scheduler.Schedule(7s, 10s, [this](TaskContext chainLightning)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f))
                DoCast(target, SPELL_CHAIN_LIGHTNING);
            chainLightning.Repeat(8s, 12s);
        })
        .Schedule(9s, 12s, [this](TaskContext healingWave)
        {
            Unit* target = me->DoFindLowestHPFriendlyInRange(40.0f);
            if (target && target->GetHealthPct() <= 70.0f)
            {
                DoCast(target, SPELL_HEALING_WAVE);
                healingWave.Repeat(6s, 10s);
            }
            else
                healingWave.Repeat(2s);
        })
        .Schedule(13s, 20s, [this](TaskContext earthShield)
        {
            std::list<Creature*> allies = DoFindFriendlyMissingBuff(40.0f, sSpellMgr->GetSpellIdForDifficulty(SPELL_EARTH_SHIELD, me));
            if (!allies.empty())
                DoCast(Trinity::Containers::SelectRandomContainerElement(allies), SPELL_EARTH_SHIELD);
            earthShield.Repeat(25s, 30s);
        })
        .Schedule(20s, 25s, [this](TaskContext hexOfMending)
        {
            DoCastVictim(SPELL_HEX_OF_MENDING);
            hexOfMending.Repeat(20s, 25s);
        });
    }
};

struct boss_hunter_toc5 : public GrandChampionAI
{
    boss_hunter_toc5(Creature* creature) : GrandChampionAI(creature) { }

    void AttackStart(Unit* who) override
    {
        if (!who || me->GetVehicleBase() || me->HasReactState(REACT_PASSIVE))
        {
            GrandChampionAI::AttackStart(who);
            return;
        }

        AttackStartCaster(who, 20.0f);
    }

    void ScheduleFootAbilities() override
    {
        Scheduler.Schedule(1s, [this](TaskContext shoot)
        {
            if (me->GetVictim() && !me->IsWithinMeleeRange(me->GetVictim()))
                DoCastVictim(SPELL_SHOOT);
            shoot.Repeat(2s, 3s);
        })
        .Schedule(7s, 12s, [this](TaskContext multiShot)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, [this](Unit* candidate)
            {
                return me->IsInRange(candidate, 5.0f, 30.0f);
            }))
                DoCast(target, SPELL_MULTI_SHOT);
            multiShot.Repeat(10s, 16s);
        })
        .Schedule(18s, 23s, [this](TaskContext lightningArrows)
        {
            DoCastSelf(SPELL_LIGHTNING_ARROWS);
            lightningArrows.Schedule(3s, [this](TaskContext /*context*/)
            {
                DoCastSelf(SPELL_LIGHTNING_ARROWS_PROC, true);
            });
            lightningArrows.Repeat(30s, 34s);
        })
        .Schedule(24s, [this](TaskContext disengage)
        {
            if (me->GetVictim() && me->IsWithinMeleeRange(me->GetVictim()))
            {
                DoCastVictim(SPELL_DISENGAGE);
                disengage.Repeat(22s, 30s);
            }
            else
                disengage.Repeat(1s);
        });
    }
};

struct boss_rogue_toc5 : public GrandChampionAI
{
    boss_rogue_toc5(Creature* creature) : GrandChampionAI(creature) { }

    void JustAppeared() override
    {
        GrandChampionAI::JustAppeared();

        if (Instance->GetData(DATA_CHAMPION_FOOT_PHASE))
            DoCastSelf(SPELL_DEADLY_POISON, true);
    }

    void JustReachedHome() override
    {
        GrandChampionAI::JustReachedHome();

        if (Instance->GetData(DATA_CHAMPION_FOOT_PHASE))
            DoCastSelf(SPELL_DEADLY_POISON, true);
    }

    void ScheduleFootAbilities() override
    {
        Scheduler.Schedule(4s, 11s, [this](TaskContext fanOfKnives)
        {
            DoCastSelf(SPELL_FAN_OF_KNIVES);
            fanOfKnives.Repeat(10s, 14s);
        })
        .Schedule(12s, 15s, [this](TaskContext poisonBottle)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true))
                DoCast(target, SPELL_POISON_BOTTLE);
            poisonBottle.Repeat(16s, 18s);
        })
        .Schedule(10s, 17s, [this](TaskContext eviscerate)
        {
            DoCastVictim(SPELL_EVISCERATE);
            eviscerate.Repeat(30s, 34s);
        });
    }
};

struct npc_toc5_lesser_champion : public ScriptedAI
{
    npc_toc5_lesser_champion(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()), _active(false) { }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();

        me->SetReactState(REACT_PASSIVE);
        me->SetImmuneToAll(true);
        DoCastSelf(SPELL_LESSER_DEFEND, true);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ENGAGE:
                _active = true;
                me->SetFaction(FACTION_MONSTER_2);
                me->SetImmuneToAll(false);
                me->SetReactState(REACT_AGGRESSIVE);
                if (me->IsInEvadeMode())
                {
                    me->GetMotionMaster()->Clear();
                    me->ClearUnitState(UNIT_STATE_EVADE);
                    me->SetFullHealth();
                }
                DoZoneInCombat();
                break;
            case ACTION_NO_ALIVE_PLAYERS:
                if (!_active)
                    break;

                _instance->SetData(DATA_MOUNTED_WIPE, 0);
                me->SetReactState(REACT_PASSIVE);
                me->SetImmuneToAll(true);
                if (me->IsEngaged())
                    EnterEvadeMode(EVADE_REASON_OTHER);
                break;
            default:
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(1s, 14s, [this](TaskContext charge)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, [this](Unit* candidate)
            {
                return me->IsInRange(candidate, 8.0f, 25.0f);
            }))
                DoCast(target, SPELL_LESSER_CHARGE);
            else
                charge.Repeat(1s);
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        me->Dismount();
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelAll();
        ScriptedAI::EnterEvadeMode(why);
        DoCastSelf(SPELL_LESSER_DEFEND, true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* _instance;
    TaskScheduler _scheduler;
    bool _active;
};

struct npc_toc5_champion_mount : public ScriptedAI
{
    npc_toc5_champion_mount(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void UpdateAI(uint32 /*diff*/) override
    {
        Unit* rider = me->GetVehicleKit() ? me->GetVehicleKit()->GetPassenger(0) : nullptr;
        Unit* target = rider ? rider->GetVictim() : nullptr;
        if (!target)
        {
            if (!_chaseGUID.IsEmpty())
            {
                _chaseGUID.Clear();
                me->GetMotionMaster()->Remove(CHASE_MOTION_TYPE);
            }
            return;
        }

        if (target->GetGUID() == _chaseGUID && me->GetMotionMaster()->HasMovementGenerator([](MovementGenerator const* movement)
        {
            return movement->GetMovementGeneratorType() == CHASE_MOTION_TYPE;
        }))
            return;

        _chaseGUID = target->GetGUID();
        me->GetMotionMaster()->MoveChase(target, Optional<ChaseRange>(), Optional<ChaseAngle>(), false);
    }

private:
    ObjectGuid _chaseGUID;
};

struct npc_toc5_hostile_argent_mount : public npc_toc5_champion_mount
{
    npc_toc5_hostile_argent_mount(Creature* creature) : npc_toc5_champion_mount(creature) { }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        if (!apply || passenger->GetTypeId() != TYPEID_UNIT)
            return;

        me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        DoCast(passenger, SPELL_MOUNT_FULL_HEAL);
    }
};

struct npc_toc5_argent_mount : public VehicleAI
{
    npc_toc5_argent_mount(Creature* creature) : VehicleAI(creature) { }

    void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply) override
    {
        if (apply || passenger->GetTypeId() != TYPEID_PLAYER)
            return;

        passenger->RemoveAurasDueToSpell(SPELL_DEFEND_PLAYER);
        me->RemoveAurasDueToSpell(SPELL_DEFEND_PLAYER);
    }
};

// 68301 - Charge
class spell_toc5_champion_charge : public SpellScript
{
    PrepareSpellScript(spell_toc5_champion_charge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_CHARGE_VEHICLE });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* mount = GetCaster()->GetVehicleBase())
            mount->CastSpell(GetHitUnit(), SPELL_CHARGE_VEHICLE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_champion_charge::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 68307 - Charge
class spell_toc5_champion_charge_vehicle : public SpellScript
{
    PrepareSpellScript(spell_toc5_champion_charge_vehicle);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_CHARGE_DAMAGE });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Vehicle* vehicle = GetCaster()->GetVehicleKit())
            if (Unit* rider = vehicle->GetPassenger(0))
                rider->CastSpell(GetHitUnit(), SPELL_CHARGE_DAMAGE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_champion_charge_vehicle::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 67866 - Trample
class spell_toc5_trample : public SpellScript
{
    PrepareSpellScript(spell_toc5_trample);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_TRAMPLED });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Creature* target = GetHitCreature();
        if (!target || target->GetVehicleBase() || target->HasUnitState(UNIT_STATE_STUNNED) || target->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (target->GetEntry())
        {
            case NPC_MOKRA:
            case NPC_ERESSEA:
            case NPC_RUNOK:
            case NPC_ZULTORE:
            case NPC_VISCERI:
            case NPC_JACOB:
            case NPC_AMBROSE:
            case NPC_COLOSOS:
            case NPC_JAELYNE:
            case NPC_LANA:
                if (InstanceScript* instance = target->GetInstanceScript())
                    if (instance->GetBossState(DATA_GRAND_CHAMPIONS) != SPECIAL)
                        break;
                target->CastSpell(target, SPELL_TRAMPLED, false);
                target->AI()->Talk(SAY_TRAMPLED, target);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_trample::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 64101 - Defend
class spell_toc5_champion_defend : public AuraScript
{
    PrepareAuraScript(spell_toc5_champion_defend);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DEFEND });
    }

    bool Load() override
    {
        return GetUnitOwner()->GetMapId() == TrialOfTheChampionMapId;
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_DEFEND, true);
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_DEFEND, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_toc5_champion_defend::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_toc5_champion_defend::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 67534 - Hex of Mending
class spell_toc5_hex_of_mending : public AuraScript
{
    PrepareAuraScript(spell_toc5_hex_of_mending);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HEX_OF_MENDING_HEAL });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        HealInfo* healInfo = eventInfo.GetHealInfo();
        if (!healInfo || !healInfo->GetHeal())
            return;

        CastSpellExtraArgs args(aurEff);
        args.OriginalCaster = GetCasterGUID();
        args.AddSpellBP0(healInfo->GetHeal());
        GetTarget()->CastSpell(GetTarget(), SPELL_HEX_OF_MENDING_HEAL, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_toc5_hex_of_mending::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 68340 - Disengage
class spell_toc5_disengage : public SpellScript
{
    PrepareSpellScript(spell_toc5_disengage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_DISENGAGE_JUMP });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_DISENGAGE_JUMP, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_disengage::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 67730 - Rolling Throw
class spell_toc5_rolling_throw_grab : public SpellScript
{
    PrepareSpellScript(spell_toc5_rolling_throw_grab);

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
        OnEffectHitTarget += SpellEffectFn(spell_toc5_rolling_throw_grab::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 67546 - Rolling Throw
class spell_toc5_rolling_throw : public SpellScript
{
    PrepareSpellScript(spell_toc5_rolling_throw);

    void HandleAfterCast()
    {
        if (Vehicle* vehicle = GetCaster()->GetVehicleKit())
            vehicle->RemoveAllPassengers();
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_toc5_rolling_throw::HandleAfterCast);
    }
};

static constexpr std::array<std::pair<uint32, uint32>, 10> ChampionPennants =
{{
    { NPC_MOKRA, SPELL_PENNANT_ORGRIMMAR },
    { NPC_ERESSEA, SPELL_PENNANT_SILVERMOON },
    { NPC_RUNOK, SPELL_PENNANT_THUNDER_BLUFF },
    { NPC_ZULTORE, SPELL_PENNANT_SENJIN },
    { NPC_VISCERI, SPELL_PENNANT_UNDERCITY },
    { NPC_JACOB, SPELL_PENNANT_STORMWIND },
    { NPC_AMBROSE, SPELL_PENNANT_GNOMEREGAN },
    { NPC_COLOSOS, SPELL_PENNANT_EXODAR },
    { NPC_JAELYNE, SPELL_PENNANT_DARNASSUS },
    { NPC_LANA, SPELL_PENNANT_IRONFORGE }
}};

static constexpr std::array<std::pair<uint32, uint32>, 10> ChampionForceChants =
{{
    { NPC_MOKRA, SPELL_FORCE_CHANT_ORC },
    { NPC_ERESSEA, SPELL_FORCE_CHANT_BLOOD_ELF },
    { NPC_RUNOK, SPELL_FORCE_CHANT_TAUREN },
    { NPC_ZULTORE, SPELL_FORCE_CHANT_TROLL },
    { NPC_VISCERI, SPELL_FORCE_CHANT_UNDEAD },
    { NPC_JACOB, SPELL_FORCE_CHANT_HUMAN },
    { NPC_AMBROSE, SPELL_FORCE_CHANT_GNOME },
    { NPC_COLOSOS, SPELL_FORCE_CHANT_DRAENEI },
    { NPC_JAELYNE, SPELL_FORCE_CHANT_NIGHT_ELF },
    { NPC_LANA, SPELL_FORCE_CHANT_DWARF }
}};

// 67347 - Argent Dungeon - Champion - NPC Pennant Selector
class spell_toc5_champion_pennant_selector : public SpellScript
{
    PrepareSpellScript(spell_toc5_champion_pennant_selector);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return std::all_of(ChampionPennants.begin(), ChampionPennants.end(), [](std::pair<uint32, uint32> const& pennant)
        {
            return ValidateSpellInfo({ pennant.second });
        });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        auto pennant = std::find_if(ChampionPennants.begin(), ChampionPennants.end(), [caster](std::pair<uint32, uint32> const& candidate)
        {
            return candidate.first == caster->GetEntry();
        });

        if (pennant != ChampionPennants.end())
            caster->CastSpell(caster, pennant->second, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_champion_pennant_selector::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66677 - Argent Raid - Spectator - Master - FX - Chant
class spell_toc5_champion_spectator_chant : public SpellScript
{
    PrepareSpellScript(spell_toc5_champion_spectator_chant);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return std::all_of(ChampionForceChants.begin(), ChampionForceChants.end(), [](std::pair<uint32, uint32> const& chant)
        {
            return ValidateSpellInfo({ chant.second });
        });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        auto chant = std::find_if(ChampionForceChants.begin(), ChampionForceChants.end(), [caster](std::pair<uint32, uint32> const& candidate)
        {
            return candidate.first == caster->GetEntry();
        });

        if (chant == ChampionForceChants.end())
            return;

        caster->m_Events.AddEventAtOffset([caster, spellId = chant->second]
        {
            caster->CastSpell(caster, spellId, true);
        }, 400ms);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_champion_spectator_chant::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 69693 - Full Heal
class spell_toc5_argent_mount_full_heal : public SpellScript
{
    PrepareSpellScript(spell_toc5_argent_mount_full_heal);

    void HandleHeal(SpellEffIndex /*effIndex*/)
    {
        SetEffectValue(0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_argent_mount_full_heal::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL_MAX_HEALTH);
    }
};

void AddSC_boss_grand_champions()
{
    RegisterTrialOfTheChampionCreatureAI(boss_warrior_toc5);
    RegisterTrialOfTheChampionCreatureAI(boss_mage_toc5);
    RegisterTrialOfTheChampionCreatureAI(boss_shaman_toc5);
    RegisterTrialOfTheChampionCreatureAI(boss_hunter_toc5);
    RegisterTrialOfTheChampionCreatureAI(boss_rogue_toc5);
    RegisterTrialOfTheChampionCreatureAI(npc_toc5_lesser_champion);
    RegisterTrialOfTheChampionCreatureAI(npc_toc5_champion_mount);
    RegisterTrialOfTheChampionCreatureAI(npc_toc5_hostile_argent_mount);
    RegisterTrialOfTheChampionCreatureAI(npc_toc5_argent_mount);
    RegisterSpellScript(spell_toc5_champion_charge);
    RegisterSpellScript(spell_toc5_champion_charge_vehicle);
    RegisterSpellScript(spell_toc5_trample);
    RegisterSpellScript(spell_toc5_champion_defend);
    RegisterSpellScript(spell_toc5_hex_of_mending);
    RegisterSpellScript(spell_toc5_disengage);
    RegisterSpellScript(spell_toc5_rolling_throw_grab);
    RegisterSpellScript(spell_toc5_rolling_throw);
    RegisterSpellScript(spell_toc5_champion_pennant_selector);
    RegisterSpellScript(spell_toc5_champion_spectator_chant);
    RegisterSpellScript(spell_toc5_argent_mount_full_heal);
}
