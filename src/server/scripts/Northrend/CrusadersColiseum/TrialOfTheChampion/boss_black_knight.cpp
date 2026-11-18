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
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TaskScheduler.h"

enum BlackKnightMisc
{
    SPELL_RAISE_DEAD_ARELAS = 67705,
    SPELL_RAISE_DEAD_JAEREN = 67715,
    SPELL_KILL_CREDIT = 68663,
    SPELL_FEIGN_DEATH = 67691,
    SPELL_CLEAR_ALL_DEBUFFS = 34098,
    SPELL_BLACK_KNIGHT_FULL_HEAL = 17683,

    SPELL_ICY_TOUCH = 67718,
    SPELL_PLAGUE_STRIKE = 67724,
    SPELL_OBLITERATE = 67725,
    SPELL_DEATHS_RESPITE = 67745,
    SPELL_ARMY_OF_THE_DEAD = 67761,
    SPELL_DESECRATION = 67778,
    SPELL_GHOUL_EXPLODE = 67751,
    SPELL_DEATHS_BITE = 67808,
    SPELL_MARKED_FOR_DEATH = 67823,

    SPELL_CLAW = 67774,
    SPELL_RANDOM_AGGRO_PERIODIC = 61910,
    SPELL_LEAP = 67749,
    SPELL_GHOUL_SELF_EXPLODE = 67754,
    SPELL_EXPLODE = 67729,

    SPELL_DESECRATION_PERIODIC = 67782,
    SPELL_DESECRATION_ARM = 67803,
    SPELL_DESECRATION_VISUAL = 67804,

    NPC_RISEN_JAEREN = 35545,
    NPC_RISEN_ARELAS = 35564,
    NPC_RISEN_CHAMPION = 35590,

    MODEL_SKELETON = 29846,
    MODEL_GHOST = 21300,

    SAY_BLACK_KNIGHT_AGGRO = 3,
    SAY_BLACK_KNIGHT_SKELETON,
    SAY_BLACK_KNIGHT_GHOST,
    SAY_BLACK_KNIGHT_KILL_PLAYER,
    SAY_BLACK_KNIGHT_DEATH,

    SAY_RISEN_HERALD_FIRST = 0,
    SAY_RISEN_HERALD_LAST = 2,

    ACTION_RESURRECTED = 100,

    PHASE_DEATH_KNIGHT = 1,
    PHASE_SKELETON,
    PHASE_GHOST,

    POINT_LANDING = 1,
    POINT_LEAVE,

    SPLINE_CHAIN_GRYPHON_LANDING = 1,
    SPLINE_CHAIN_GRYPHON_LEAVE = 2
};

struct boss_black_knight : public ScriptedAI
{
    boss_black_knight(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()), _summons(creature), _phase(PHASE_DEATH_KNIGHT), _transition(false)
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        me->SetReactState(REACT_PASSIVE);
        me->SetImmuneToPC(true);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ENGAGE:
                me->SetHomePosition(me->GetPosition());
                me->SetImmuneToPC(false);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat();
                break;
            case ACTION_NO_ALIVE_PLAYERS:
                if (me->IsEngaged())
                    EnterEvadeMode(EVADE_REASON_OTHER);
                break;
            case ACTION_RESURRECTED:
                if (!_transition)
                    break;

                _transition = false;
                _scheduler.CancelAll();
                me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                me->RemoveUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
                me->RemoveDynamicFlag(UNIT_DYNFLAG_DEAD);
                me->SetReactState(REACT_AGGRESSIVE);
                ++_phase;

                if (_phase == PHASE_SKELETON)
                {
                    Talk(SAY_BLACK_KNIGHT_SKELETON);
                    me->SetDisplayId(MODEL_SKELETON);
                    me->SetMaxHealth(me->GetMaxHealth() * (IsHeroic() ? 20 : 15) / (IsHeroic() ? 22 : 16));
                    me->SetFullHealth();
                    DoCastSelf(SPELL_ARMY_OF_THE_DEAD);
                    ScheduleDeathKnightAbilities();
                    _scheduler.Schedule(7s, 10s, [this](TaskContext desecration)
                    {
                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f))
                            DoCast(target, SPELL_DESECRATION);
                        desecration.Repeat(16s);
                    })
                    .Schedule(11s, 12s, [this](TaskContext ghoulExplode)
                    {
                        if (std::any_of(_summons.begin(), _summons.end(), [this](ObjectGuid const& guid)
                        {
                            Creature* summon = ObjectAccessor::GetCreature(*me, guid);
                            return summon && summon->IsAlive() && summon->GetEntry() == NPC_RISEN_CHAMPION;
                        }))
                            DoCastSelf(SPELL_GHOUL_EXPLODE);
                        ghoulExplode.Repeat(12s, 15s);
                    });
                }
                else
                {
                    Talk(SAY_BLACK_KNIGHT_GHOST);
                    me->SetDisplayId(MODEL_GHOST);
                    SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                    _scheduler.Schedule(6s, 8s, [this](TaskContext deathsBite)
                    {
                        DoCastSelf(SPELL_DEATHS_BITE);
                        deathsBite.Repeat(2s, 4s);
                    })
                    .Schedule(1s, [this](TaskContext markedForDeath)
                    {
                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f))
                            DoCast(target, SPELL_MARKED_FOR_DEATH);
                        markedForDeath.Repeat(10s, 12s);
                    });
                }
                break;
            default:
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _instance->SetBossState(DATA_BLACK_KNIGHT, IN_PROGRESS);
        Talk(SAY_BLACK_KNIGHT_AGGRO);

        if (Creature* herald = _instance->GetCreature(DATA_HERALD))
            DoCast(herald, me->GetMap()->ToInstanceMap()->GetTeamIdInInstance() == TEAM_ALLIANCE ? SPELL_RAISE_DEAD_ARELAS : SPELL_RAISE_DEAD_JAEREN, true);

        ScheduleDeathKnightAbilities();
        _scheduler.Schedule(6s, 11s, [this](TaskContext /*deathsRespite*/)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                DoCast(target, SPELL_DEATHS_RESPITE);
        });
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        if (_transition)
        {
            damage = 0;
            return;
        }

        if (_phase == PHASE_GHOST || damage < me->GetHealth())
            return;

        damage = 0;
        _transition = true;
        _scheduler.CancelAll();
        me->InterruptNonMeleeSpells(true);
        me->AttackStop();
        me->SetReactState(REACT_PASSIVE);
        me->GetMotionMaster()->Clear();
        me->GetMotionMaster()->MoveIdle();
        me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->SetUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
        me->SetDynamicFlag(UNIT_DYNFLAG_DEAD);

        for (ObjectGuid const& guid : _summons)
        {
            Creature* summon = ObjectAccessor::GetCreature(*me, guid);
            if (!summon || !summon->IsAlive() || summon->FindCurrentSpellBySpellId(sSpellMgr->GetSpellIdForDifficulty(SPELL_EXPLODE, summon)))
                continue;

            switch (summon->GetEntry())
            {
                case NPC_RISEN_JAEREN:
                case NPC_RISEN_ARELAS:
                case NPC_RISEN_CHAMPION:
                    summon->CastSpell(summon, SPELL_GHOUL_SELF_EXPLODE, true);
                    break;
                default:
                    break;
            }
        }

        DoCastSelf(SPELL_FEIGN_DEATH, true);
        DoCastSelf(SPELL_CLEAR_ALL_DEBUFFS, true);
        DoCastSelf(SPELL_BLACK_KNIGHT_FULL_HEAL, true);
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        _summons.Despawn(summon);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_BLACK_KNIGHT_KILL_PLAYER);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _scheduler.CancelAll();
        _summons.DespawnAll();
        Talk(SAY_BLACK_KNIGHT_DEATH);
        DoCastSelf(SPELL_KILL_CREDIT, true);
        _instance->SetBossState(DATA_BLACK_KNIGHT, DONE);
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        _scheduler.CancelAll();
        _summons.DespawnAll();
        _phase = PHASE_DEATH_KNIGHT;
        _transition = false;
        me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveUnitFlag2(UNIT_FLAG2_FEIGN_DEATH);
        me->RemoveDynamicFlag(UNIT_DYNFLAG_DEAD);
        me->SetDisplayId(me->GetNativeDisplayId());
        me->LoadEquipment(1, true);
        me->SetReactState(REACT_AGGRESSIVE);
        ScriptedAI::EnterEvadeMode(why);
        me->UpdateMaxHealth();

        if (_instance->GetBossState(DATA_BLACK_KNIGHT) == IN_PROGRESS)
            _instance->SetBossState(DATA_BLACK_KNIGHT, FAIL);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);

        if (!_transition)
            DoMeleeAttackIfReady();
    }

private:
    void ScheduleDeathKnightAbilities()
    {
        _scheduler.Schedule(5s, 9s, [this](TaskContext icyTouch)
        {
            DoCastVictim(SPELL_ICY_TOUCH);
            icyTouch.Repeat(10s, 12s);
        })
        .Schedule(6s, 10s, [this](TaskContext plagueStrike)
        {
            if (DoCastVictim(SPELL_PLAGUE_STRIKE) != SPELL_CAST_OK)
            {
                plagueStrike.Repeat(1s);
                return;
            }

            plagueStrike.Repeat(10s, 14s);
        })
        .Schedule(10s, 14s, [this](TaskContext obliterate)
        {
            if (DoCastVictim(SPELL_OBLITERATE) != SPELL_CAST_OK)
            {
                obliterate.Repeat(1s);
                return;
            }

            obliterate.Repeat(22s, 35s);
        });
    }

    InstanceScript* _instance;
    TaskScheduler _scheduler;
    SummonList _summons;
    uint8 _phase;
    bool _transition;
};

struct npc_risen_ghoul : public ScriptedAI
{
    npc_risen_ghoul(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
    {
        Scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        me->SetReactState(REACT_PASSIVE);
        Scheduler.Schedule(4s, [this](TaskContext /*context*/)
        {
            me->SetReactState(REACT_AGGRESSIVE);
            DoZoneInCombat();
        });
    }

    void Reset() override
    {
        Scheduler.CancelAll();
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCastSelf(SPELL_RANDOM_AGGRO_PERIODIC, true);
        Scheduler.Schedule(2s, 5s, [this](TaskContext claw)
        {
            DoCastVictim(SPELL_CLAW);
            claw.Repeat(6s, 10s);
        });
    }

    void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override
    {
        if (target->GetTypeId() == TYPEID_PLAYER && spellInfo->Id == sSpellMgr->GetSpellIdForDifficulty(SPELL_EXPLODE, me))
            _instance->SetData(DATA_HAD_WORSE_FAILED, 0);
    }

    void UpdateAI(uint32 diff) override
    {
        Scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

protected:
    TaskScheduler Scheduler;

private:
    InstanceScript* _instance;
};

struct npc_risen_herald : public npc_risen_ghoul
{
    npc_risen_herald(Creature* creature) : npc_risen_ghoul(creature), _exploding(false) { }

    void Reset() override
    {
        npc_risen_ghoul::Reset();
        _exploding = false;
    }

    void JustEngagedWith(Unit* who) override
    {
        npc_risen_ghoul::JustEngagedWith(who);
        Scheduler.Schedule(1s, [this](TaskContext leap)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, [this](Unit* candidate)
            {
                return candidate->GetTypeId() == TYPEID_PLAYER && me->IsInRange(candidate, 5.0f, 30.0f);
            }))
            {
                DoCast(target, SPELL_LEAP);
                leap.Repeat(10s, 12s);
            }
            else
                leap.Repeat(1s);
        });
    }

    void OnSpellCast(SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == sSpellMgr->GetSpellIdForDifficulty(SPELL_CLAW, me) && urand(0, 2) == 0)
            Talk(urand(SAY_RISEN_HERALD_FIRST, SAY_RISEN_HERALD_LAST));
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        if (_exploding || !me->HealthBelowPctDamaged(30, damage))
            return;

        _exploding = true;
        if (me->FindCurrentSpellBySpellId(sSpellMgr->GetSpellIdForDifficulty(SPELL_EXPLODE, me)))
            return;

        if (damage >= me->GetHealth())
            damage = me->GetHealth() - 1;
        DoCastSelf(SPELL_GHOUL_SELF_EXPLODE, true);
    }

private:
    bool _exploding;
};

struct npc_black_knight_desecration_stalker : public ScriptedAI
{
    npc_black_knight_desecration_stalker(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        me->SetReactState(REACT_PASSIVE);
        me->SetControlled(true, UNIT_STATE_ROOT);
        DoCastSelf(SPELL_DESECRATION_PERIODIC, true);
        DoCastSelf(SPELL_DESECRATION_ARM, true);
        DoCastSelf(SPELL_DESECRATION_VISUAL, true);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void UpdateAI(uint32 /*diff*/) override { }
};

struct npc_black_knight_skeletal_gryphon : public ScriptedAI
{
    npc_black_knight_skeletal_gryphon(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        me->SetReactState(REACT_PASSIVE);
        me->SetImmuneToPC(true);
        me->SetDisableGravity(true);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_START_FLIGHT:
                me->GetMotionMaster()->MoveAlongSplineChain(POINT_LANDING, SPLINE_CHAIN_GRYPHON_LANDING, false);
                break;
            case ACTION_LEAVE:
                me->SetDisableGravity(true);
                me->GetMotionMaster()->MoveAlongSplineChain(POINT_LEAVE, SPLINE_CHAIN_GRYPHON_LEAVE, false);
                break;
            default:
                break;
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != SPLINE_CHAIN_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case POINT_LANDING:
                if (InstanceScript* instance = me->GetInstanceScript())
                    instance->SetData(DATA_GRYPHON_LANDED, 0);
                break;
            case POINT_LEAVE:
                me->DespawnOrUnsummon();
                break;
            default:
                break;
        }
    }

    void GetUnitVehicleExitParameters(UnitVehicleExitParameters& parameters, Unit* passenger) override
    {
        Position dismount(749.279f, 634.457f, 411.533f, passenger->GetOrientation());
        if (InstanceScript* instance = me->GetInstanceScript())
            if (Creature* herald = instance->GetCreature(DATA_HERALD))
                dismount.SetOrientation(dismount.GetAbsoluteAngle(herald));

        parameters.ExitPosition = dismount;
    }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void UpdateAI(uint32 /*diff*/) override { }
};

// 67751 - Ghoul Explode
class spell_black_knight_ghoul_explode : public SpellScript
{
    PrepareSpellScript(spell_black_knight_ghoul_explode);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ uint32(spellInfo->GetEffect(EFFECT_0).CalcValue()) });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), uint32(GetEffectInfo(EFFECT_0).CalcValue()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_black_knight_ghoul_explode::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 67754, 67889 - Ghoul Explode
class spell_black_knight_ghoul_explode_risen_ghoul : public SpellScript
{
    PrepareSpellScript(spell_black_knight_ghoul_explode_risen_ghoul);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ uint32(spellInfo->GetEffect(EFFECT_1).CalcValue()) });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), uint32(GetEffectValue()));
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_black_knight_ghoul_explode_risen_ghoul::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 67802 - Desecration Arm
class spell_black_knight_desecration_arm : public SpellScript
{
    PrepareSpellScript(spell_black_knight_desecration_arm);

    void RandomizeDest(SpellDestination& dest)
    {
        dest.Relocate(GetCaster()->GetRandomPoint(dest._position, 7.0f));
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(spell_black_knight_desecration_arm::RandomizeDest, EFFECT_0, TARGET_DEST_DEST_RANDOM);
    }
};

// 67693 - Black Knight Res
class spell_black_knight_res : public SpellScript
{
    PrepareSpellScript(spell_black_knight_res);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* knight = GetHitCreature())
            knight->AI()->DoAction(ACTION_RESURRECTED);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_black_knight_res::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 61910 - Random Aggro Periodic (10 sec)
class spell_toc5_random_aggro_periodic : public AuraScript
{
    PrepareAuraScript(spell_toc5_random_aggro_periodic);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Creature* owner = GetTarget()->ToCreature();
        if (!owner || !owner->IsAIEnabled())
            return;

        if (Unit* target = owner->AI()->SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
        {
            owner->GetThreatManager().ResetAllThreat();
            owner->GetThreatManager().AddThreat(target, 1000.0f, nullptr, true, true);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_toc5_random_aggro_periodic::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

void AddSC_boss_black_knight()
{
    RegisterTrialOfTheChampionCreatureAI(boss_black_knight);
    RegisterTrialOfTheChampionCreatureAI(npc_risen_ghoul);
    RegisterTrialOfTheChampionCreatureAI(npc_risen_herald);
    RegisterTrialOfTheChampionCreatureAI(npc_black_knight_desecration_stalker);
    RegisterTrialOfTheChampionCreatureAI(npc_black_knight_skeletal_gryphon);
    RegisterSpellScript(spell_black_knight_ghoul_explode);
    RegisterSpellScript(spell_black_knight_ghoul_explode_risen_ghoul);
    RegisterSpellScript(spell_black_knight_desecration_arm);
    RegisterSpellScript(spell_black_knight_res);
    RegisterSpellScript(spell_toc5_random_aggro_periodic);
}
