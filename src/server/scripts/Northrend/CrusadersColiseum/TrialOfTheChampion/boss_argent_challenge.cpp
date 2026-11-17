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
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TaskScheduler.h"

enum ArgentChallengeMisc
{
    SPELL_VENGEANCE = 66865,
    SPELL_RADIANCE = 66935,
    SPELL_HAMMER_OF_JUSTICE = 66863,
    SPELL_HAMMER_OF_JUSTICE_STUN = 66940,
    SPELL_HAMMER_OF_THE_RIGHTEOUS_OVERRIDE = 66904,
    SPELL_HAMMER_OF_THE_RIGHTEOUS_THROW = 66905,
    SPELL_EADRIC_ACHIEVEMENT = 68197,
    SPELL_EADRIC_KILL_CREDIT = 68575,

    SPELL_HOLY_SMITE = 66536,
    SPELL_HOLY_FIRE = 66538,
    SPELL_RENEW = 66537,
    SPELL_HOLY_NOVA = 66546,
    SPELL_REFLECTIVE_SHIELD = 66515,
    SPELL_REFLECTIVE_SHIELD_DAMAGE = 33619,
    SPELL_SUMMON_MEMORY = 66545,
    SPELL_CONFESS_SELF = 66680,
    SPELL_CONFESSOR_ACHIEVEMENT = 68206,

    SPELL_MEMORY_HOGGER = 66543,
    SPELL_MEMORY_VANCLEEF = 66691,
    SPELL_MEMORY_MUTANUS = 66692,
    SPELL_MEMORY_HEROD = 66694,
    SPELL_MEMORY_LUCIFRON = 66695,
    SPELL_MEMORY_THUNDERAAN = 66696,
    SPELL_MEMORY_CHROMAGGUS = 66697,
    SPELL_MEMORY_HAKKAR = 66698,
    SPELL_MEMORY_VEKNILASH = 66699,
    SPELL_MEMORY_KALITHRESH = 66700,
    SPELL_MEMORY_MALCHEZAAR = 66701,
    SPELL_MEMORY_GRUUL = 66702,
    SPELL_MEMORY_VASHJ = 66703,
    SPELL_MEMORY_ARCHIMONDE = 66704,
    SPELL_MEMORY_ILLIDAN = 66705,
    SPELL_MEMORY_DELRISSA = 66706,
    SPELL_MEMORY_ENTROPIUS = 66707,
    SPELL_MEMORY_INGVAR = 66708,
    SPELL_MEMORY_CYANIGOSA = 66709,
    SPELL_MEMORY_ECK = 66710,
    SPELL_MEMORY_ONYXIA = 66711,
    SPELL_MEMORY_HEIGAN = 66712,
    SPELL_MEMORY_IGNIS = 66713,
    SPELL_MEMORY_VEZAX = 66714,
    SPELL_MEMORY_ALGALON = 66715,

    SPELL_SHADOWFORM = 41408,
    SPELL_MEMORY_SPAWN_EFFECT = 66675,
    SPELL_OLD_WOUNDS = 66620,
    SPELL_SHADOWS_OF_THE_PAST = 66619,
    SPELL_WAKING_NIGHTMARE = 66552,

    SPELL_BLAZING_LIGHT = 67247,
    SPELL_CLEAVE = 15284,
    SPELL_UNBALANCING_STRIKE = 67237,
    SPELL_FLURRY_OF_BLOWS = 67233,
    SPELL_PUMMEL = 67235,
    SPELL_DIVINE_SHIELD = 67251,
    SPELL_FINAL_MEDITATION = 67255,
    SPELL_HOLY_SMITE_PRIESTESS = 36176,
    SPELL_SHADOW_WORD_PAIN = 34941,
    SPELL_SHADOW_WORD_PAIN_HEROIC = 34942,
    SPELL_FOUNTAIN_OF_LIGHT = 67194,
    SPELL_MIND_CONTROL = 67229,
    SPELL_CHARM_BREAK = 65634,
    SPELL_LIGHT_RAIN = 67196,

    NPC_FOUNTAIN_OF_LIGHT = 35311,

    SAY_EADRIC_AGGRO = 1,
    SAY_EADRIC_RADIANCE,
    SAY_EADRIC_HAMMER_EMOTE,
    SAY_EADRIC_HAMMER,
    SAY_EADRIC_KILL_PLAYER,
    SAY_EADRIC_DEFEATED,

    SAY_PALETRESS_AGGRO = 2,
    SAY_PALETRESS_MEMORY_SUMMON,
    SAY_PALETRESS_MEMORY_DIED,
    SAY_PALETRESS_KILL_PLAYER,
    SAY_PALETRESS_DEFEATED,

    SAY_MEMORY_WAKING_NIGHTMARE = 0,

    POINT_INTRO,
    POINT_ENGAGE,
    POINT_DEFEAT,
    POINT_EXIT
};

static Position const ArgentChampionIntroSpot = { 746.777f, 661.653f, 411.699f, 4.712f };
static Position const ArgentChampionEngageSpot = { 746.520f, 637.491f, 411.563f, 4.712f };

static constexpr std::array<uint32, 25> MemorySummonSpells =
{
    SPELL_MEMORY_HOGGER,
    SPELL_MEMORY_VANCLEEF,
    SPELL_MEMORY_MUTANUS,
    SPELL_MEMORY_HEROD,
    SPELL_MEMORY_LUCIFRON,
    SPELL_MEMORY_THUNDERAAN,
    SPELL_MEMORY_CHROMAGGUS,
    SPELL_MEMORY_HAKKAR,
    SPELL_MEMORY_VEKNILASH,
    SPELL_MEMORY_KALITHRESH,
    SPELL_MEMORY_MALCHEZAAR,
    SPELL_MEMORY_GRUUL,
    SPELL_MEMORY_VASHJ,
    SPELL_MEMORY_ARCHIMONDE,
    SPELL_MEMORY_ILLIDAN,
    SPELL_MEMORY_DELRISSA,
    SPELL_MEMORY_ENTROPIUS,
    SPELL_MEMORY_INGVAR,
    SPELL_MEMORY_CYANIGOSA,
    SPELL_MEMORY_ECK,
    SPELL_MEMORY_ONYXIA,
    SPELL_MEMORY_HEIGAN,
    SPELL_MEMORY_IGNIS,
    SPELL_MEMORY_VEZAX,
    SPELL_MEMORY_ALGALON
};

struct ArgentChampionAI : public ScriptedAI
{
    ArgentChampionAI(Creature* creature) : ScriptedAI(creature), Instance(creature->GetInstanceScript()), Defeated(false)
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
        DoCastSelf(SPELL_FORCE_RESPONSE_CHEER_ALLIANCE, true);
        DoCastSelf(SPELL_FORCE_RESPONSE_CHEER_HORDE, true);
        Scheduler.Schedule(6s, [this](TaskContext /*context*/)
        {
            me->SetWalk(true);
            me->GetMotionMaster()->MovePoint(POINT_INTRO, ArgentChampionIntroSpot, true, ArgentChampionIntroSpot.GetOrientation());
        });
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ENGAGE:
            {
                Scheduler.CancelAll();
                me->SetFaction(FACTION_MONSTER_2);
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(POINT_ENGAGE, ArgentChampionEngageSpot);

                std::list<Creature*> fountains;
                me->GetCreatureListWithEntryInGrid(fountains, NPC_FOUNTAIN_OF_LIGHT, 200.0f);
                for (Creature* fountain : fountains)
                {
                    fountain->DespawnOrUnsummon();
                }
                break;
            }
            case ACTION_NO_ALIVE_PLAYERS:
                if (!Defeated && me->IsEngaged())
                    EnterEvadeMode(EVADE_REASON_OTHER);
                break;
            default:
                break;
        }
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (pointId)
        {
            case POINT_ENGAGE:
                DoZoneInCombat();
                break;
            case POINT_DEFEAT:
                Scheduler.Schedule(2s, [this](TaskContext context)
                {
                    DoCastSelf(SPELL_FULL_HEAL, true);
                    me->SetFaction(FACTION_FRIENDLY);

                    context.Schedule(6s, [this](TaskContext /*context*/)
                    {
                        me->SetWalk(true);
                        me->GetMotionMaster()->MovePoint(POINT_EXIT, Position(746.815f, 662.321f, 411.711f));
                    });
                });
                break;
            case POINT_EXIT:
                me->DespawnOrUnsummon();
                break;
            default:
                break;
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Instance->SetBossState(DATA_ARGENT_CHALLENGE, IN_PROGRESS);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* spellInfo) override
    {
        if (Defeated)
        {
            damage = 0;
            return;
        }

        if (damage < me->GetHealth())
            return;

        damage = 0;
        me->SetHealth(1);
        Defeated = true;
        Scheduler.CancelAll();
        me->InterruptNonMeleeSpells(true);
        me->RemoveAllAuras();
        OnDefeated(spellInfo);
        me->AttackStop();
        me->SetIsCombatDisallowed(true);
        me->CombatStop(true);
        me->SetReactState(REACT_PASSIVE);
        me->SetImmuneToPC(true);
        me->GetMotionMaster()->Clear();
        me->SetWalk(false);
        me->GetMotionMaster()->MovePoint(POINT_DEFEAT, Position(746.660f, 627.444f, 411.172f), true, 4.712f);
        Instance->SetData(DATA_ARGENT_CHAMPION_DEFEATED, 0);
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (Defeated)
            return;

        Scheduler.CancelAll();
        me->SetHomePosition(ArgentChampionEngageSpot);
        ScriptedAI::EnterEvadeMode(why);

        if (Instance->GetBossState(DATA_ARGENT_CHALLENGE) == IN_PROGRESS)
            Instance->SetBossState(DATA_ARGENT_CHALLENGE, FAIL);
    }

    void UpdateAI(uint32 diff) override
    {
        Scheduler.Update(diff);

        if (Defeated || !UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

protected:
    virtual void OnDefeated(SpellInfo const* killingSpell) = 0;

    InstanceScript* Instance;
    TaskScheduler Scheduler;
    bool Defeated;
};

struct boss_eadric : public ArgentChampionAI
{
    boss_eadric(Creature* creature) : ArgentChampionAI(creature) { }

    void JustEngagedWith(Unit* who) override
    {
        ArgentChampionAI::JustEngagedWith(who);
        Talk(SAY_EADRIC_AGGRO);
        DoCastSelf(SPELL_VENGEANCE, true);

        Scheduler.Schedule(11s, 18s, [this](TaskContext radiance)
        {
            if (DoCastSelf(SPELL_RADIANCE) != SPELL_CAST_OK)
            {
                radiance.Repeat(1s);
                return;
            }

            Talk(SAY_EADRIC_RADIANCE);
            radiance.Repeat(19s, 28s);
        })
        .Schedule(20s, 31s, [this](TaskContext hammer)
        {
            Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 55.0f, true);
            if (!target || DoCast(target, SPELL_HAMMER_OF_JUSTICE) != SPELL_CAST_OK)
            {
                hammer.Repeat(1s);
                return;
            }

            Talk(SAY_EADRIC_HAMMER_EMOTE, target);
            Talk(SAY_EADRIC_HAMMER);
            hammer.Repeat(30s, 38s);
        });
    }

    void OnDefeated(SpellInfo const* killingSpell) override
    {
        Talk(SAY_EADRIC_DEFEATED);
        if (killingSpell && killingSpell->Id == SPELL_HAMMER_OF_THE_RIGHTEOUS_THROW && IsHeroic())
            DoCastSelf(SPELL_EADRIC_ACHIEVEMENT, true);
        DoCastSelf(SPELL_EADRIC_KILL_CREDIT, true);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_EADRIC_KILL_PLAYER);
    }
};

struct boss_paletress : public ArgentChampionAI
{
    boss_paletress(Creature* creature) : ArgentChampionAI(creature), _memoryPhase(false), _memoryPending(false) { }

    void Reset() override
    {
        _memoryPhase = false;
        _memoryPending = false;
        if (Creature* memory = ObjectAccessor::GetCreature(*me, _memoryGUID))
            memory->DespawnOrUnsummon();
    }

    void DoAction(int32 action) override
    {
        if (action != ACTION_MEMORY_DIED)
        {
            ArgentChampionAI::DoAction(action);
            return;
        }

        Talk(SAY_PALETRESS_MEMORY_DIED);
        me->RemoveAurasDueToSpell(SPELL_REFLECTIVE_SHIELD);
    }

    void JustEngagedWith(Unit* who) override
    {
        ArgentChampionAI::JustEngagedWith(who);
        Talk(SAY_PALETRESS_AGGRO);

        Scheduler.Schedule(1s, [this](TaskContext holySmite)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f))
                DoCast(target, SPELL_HOLY_SMITE);
            holySmite.Repeat(1s, 3s);
        })
        .Schedule(16s, 25s, [this](TaskContext holyFire)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true))
                DoCast(target, SPELL_HOLY_FIRE);
            holyFire.Repeat(19s, 26s);
        })
        .Schedule(19s, 31s, [this](TaskContext renew)
        {
            if (Unit* target = me->DoFindLowestHPFriendlyInRange(50.0f))
                DoCast(target, SPELL_RENEW);
            renew.Repeat(19s, 24s);
        });
    }

    void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType damageType, SpellInfo const* spellInfo) override
    {
        if (_memoryPending)
        {
            if (damage >= me->GetHealth())
                damage = me->GetHealth() - 1;
            return;
        }

        if (_memoryPhase || Defeated || !me->HealthBelowPctDamaged(25, damage))
        {
            ArgentChampionAI::DamageTaken(attacker, damage, damageType, spellInfo);
            return;
        }

        if (damage >= me->GetHealth())
            damage = me->GetHealth() - 1;

        _memoryPhase = true;
        _memoryPending = true;
        me->InterruptNonMeleeSpells(true);
        DoCastSelf(SPELL_HOLY_NOVA, true);
        Scheduler.Schedule(1s, [this](TaskContext /*context*/)
        {
            _memoryPending = false;
            Talk(SAY_PALETRESS_MEMORY_SUMMON);
            DoCastSelf(SPELL_SUMMON_MEMORY, true);
            DoCastSelf(SPELL_REFLECTIVE_SHIELD, true);
            DoCastSelf(SPELL_CONFESS_SELF, true);
        });
    }

    void JustSummoned(Creature* summon) override
    {
        _memoryGUID = summon->GetGUID();
    }

    void OnDefeated(SpellInfo const* /*killingSpell*/) override
    {
        if (Creature* memory = ObjectAccessor::GetCreature(*me, _memoryGUID))
            memory->DespawnOrUnsummon();

        Talk(SAY_PALETRESS_DEFEATED);
        DoCastSelf(SPELL_CONFESSOR_ACHIEVEMENT, true);
        DoCastSelf(SPELL_ARGENT_CHAMPION_CREDIT, true);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_PALETRESS_KILL_PLAYER);
    }

private:
    ObjectGuid _memoryGUID;
    bool _memoryPhase;
    bool _memoryPending;
};

struct npc_memory : public ScriptedAI
{
    npc_memory(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript())
    {
        _scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        DoCastSelf(SPELL_SHADOWFORM, true);
        DoCastSelf(SPELL_MEMORY_SPAWN_EFFECT, true);
        me->SetControlled(true, UNIT_STATE_STUNNED);
        DoZoneInCombat();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(7s, [this](TaskContext /*context*/)
        {
            me->SetControlled(false, UNIT_STATE_STUNNED);
        })
        .Schedule(7s, 9s, [this](TaskContext shadowsOfThePast)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 40.0f, true))
                DoCast(target, SPELL_SHADOWS_OF_THE_PAST);
            shadowsOfThePast.Repeat(10s, 18s);
        })
        .Schedule(10s, 12s, [this](TaskContext oldWounds)
        {
            DoCastVictim(SPELL_OLD_WOUNDS);
            oldWounds.Repeat(11s, 18s);
        })
        .Schedule(15s, 18s, [this](TaskContext wakingNightmare)
        {
            Talk(SAY_MEMORY_WAKING_NIGHTMARE);
            DoCastSelf(SPELL_WAKING_NIGHTMARE);
            wakingNightmare.Repeat(27s, 33s);
        });
    }

    void Reset() override
    {
        _scheduler.CancelAll();
        me->SetControlled(false, UNIT_STATE_STUNNED);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _instance->SetData(DATA_MEMORY_DIED, me->GetEntry());
        if (Creature* paletress = _instance->GetCreature(DATA_ARGENT_CHALLENGE))
            paletress->AI()->DoAction(ACTION_MEMORY_DIED);
        me->DespawnOrUnsummon();
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
};

struct ArgentSoldierAI : public ScriptedAI
{
    ArgentSoldierAI(Creature* creature) : ScriptedAI(creature), _post(creature->GetHomePosition())
    {
        Scheduler.SetValidator([this]
        {
            return !me->HasUnitState(UNIT_STATE_CASTING);
        });
    }

    void MovementInform(uint32 type, uint32 pointId) override
    {
        if (type != POINT_MOTION_TYPE || pointId != POINT_ARGENT_POST)
            return;

        me->SetEmoteState(EMOTE_STATE_READY1H);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_POST_ASSIGNED:
                _post = me->GetHomePosition();
                break;
            case ACTION_NO_ALIVE_PLAYERS:
                if (me->IsEngaged())
                    EnterEvadeMode(EVADE_REASON_OTHER);
                break;
            default:
                break;
        }
    }

    void Reset() override
    {
        Scheduler.CancelAll();
    }

    void JustReachedHome() override
    {
        me->SetEmoteState(EMOTE_STATE_READY1H);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        me->SetHomePosition(_post);
        me->SetEmoteState(EMOTE_ONESHOT_NONE);
        me->CallForHelp(10.0f);
        ScheduleAbilities();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        Scheduler.Update(diff);
        DoMeleeAttackIfReady();
    }

protected:
    virtual void ScheduleAbilities() = 0;

    TaskScheduler Scheduler;

private:
    Position _post;
};

struct npc_argent_lightwielder : public ArgentSoldierAI
{
    npc_argent_lightwielder(Creature* creature) : ArgentSoldierAI(creature) { }

    void ScheduleAbilities() override
    {
        Scheduler.Schedule(7s, 12s, [this](TaskContext blazingLight)
        {
            if (Unit* target = me->DoFindLowestHPFriendlyInRange(40.0f))
                DoCast(target, SPELL_BLAZING_LIGHT);
            blazingLight.Repeat(14s, 18s);
        })
        .Schedule(7s, 12s, [this](TaskContext cleave)
        {
            DoCastVictim(SPELL_CLEAVE);
            cleave.Repeat(8s, 13s);
        });

        if (IsHeroic())
        {
            Scheduler.Schedule(9s, 15s, [this](TaskContext unbalancingStrike)
            {
                DoCastVictim(SPELL_UNBALANCING_STRIKE);
                unbalancingStrike.Repeat(12s, 16s);
            });
        }
    }
};

struct npc_argent_monk : public ArgentSoldierAI
{
    npc_argent_monk(Creature* creature) : ArgentSoldierAI(creature), _divineShieldUsed(false) { }

    void Reset() override
    {
        ArgentSoldierAI::Reset();
        _divineShieldUsed = false;
    }

    void ScheduleAbilities() override
    {
        Scheduler.Schedule(6s, 14s, [this](TaskContext flurryOfBlows)
        {
            DoCastSelf(SPELL_FLURRY_OF_BLOWS);
            flurryOfBlows.Repeat(23s, 28s);
        })
        .Schedule(6s, 12s, [this](TaskContext pummel)
        {
            DoCastVictim(SPELL_PUMMEL);
            pummel.Repeat(10s, 14s);
        });
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        if (_divineShieldUsed || !IsHeroic() || damage < me->GetHealth())
            return;

        _divineShieldUsed = true;
        damage = me->GetHealth() - 1;
        DoCastSelf(SPELL_DIVINE_SHIELD, true);
        DoCastSelf(SPELL_FINAL_MEDITATION, true);
    }

private:
    bool _divineShieldUsed;
};

struct npc_argent_priestess : public ArgentSoldierAI
{
    npc_argent_priestess(Creature* creature) : ArgentSoldierAI(creature) { }

    void Reset() override
    {
        ArgentSoldierAI::Reset();
        if (Creature* fountain = ObjectAccessor::GetCreature(*me, _fountainGUID))
            fountain->DespawnOrUnsummon();
        _fountainGUID.Clear();
    }

    void JustSummoned(Creature* summon) override
    {
        if (summon->GetEntry() == NPC_FOUNTAIN_OF_LIGHT)
            _fountainGUID = summon->GetGUID();
    }

    void ScheduleAbilities() override
    {
        Scheduler.Schedule(2s, 6s, [this](TaskContext shadowWordPain)
        {
            uint32 const spellId = IsHeroic() ? SPELL_SHADOW_WORD_PAIN_HEROIC : SPELL_SHADOW_WORD_PAIN;
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true, true, -int32(spellId)))
                DoCast(target, spellId);
            shadowWordPain.Repeat(15s, 21s);
        })
        .Schedule(3s, 9s, [this](TaskContext holySmite)
        {
            DoCastVictim(SPELL_HOLY_SMITE_PRIESTESS);
            holySmite.Repeat(4s, 8s);
        })
        .Schedule(9s, 15s, [this](TaskContext /*fountainOfLight*/)
        {
            DoCastSelf(SPELL_FOUNTAIN_OF_LIGHT);
        });

        if (IsHeroic())
        {
            Scheduler.Schedule(14s, 17s, [this](TaskContext /*mindControl*/)
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 1, 30.0f, true))
                    DoCast(target, SPELL_MIND_CONTROL);
            });
        }
    }

private:
    ObjectGuid _fountainGUID;
};

struct npc_toc5_fountain_of_light : public ScriptedAI
{
    npc_toc5_fountain_of_light(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();
        me->SetReactState(REACT_PASSIVE);
        DoCastSelf(SPELL_LIGHT_RAIN, true);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void UpdateAI(uint32 /*diff*/) override { }
};

// 66545 - Summon Memory
class spell_paletress_summon_memory : public SpellScript
{
    PrepareSpellScript(spell_paletress_summon_memory);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(MemorySummonSpells);
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
        targets.clear();
        targets.push_back(target);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), Trinity::Containers::SelectRandomContainerElement(MemorySummonSpells), GetCaster()->GetGUID());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_paletress_summon_memory::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_paletress_summon_memory::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66862, 67681 - Radiance
class spell_eadric_radiance : public SpellScript
{
    PrepareSpellScript(spell_eadric_radiance);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([caster = GetCaster()](WorldObject* target)
        {
            return !target->isInFront(caster, 2.5f) || !target->IsWithinDist(caster, 40.0f);
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_eadric_radiance::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_eadric_radiance::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 66941 - Hammer of Justice
class spell_eadric_hammer_of_justice : public SpellScript
{
    PrepareSpellScript(spell_eadric_hammer_of_justice);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ uint32(spellInfo->GetEffect(EFFECT_0).CalcValue()) });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        ObjectGuid const targetGUID = GetHitUnit()->GetGUID();
        uint32 const spellId = uint32(GetEffectValue());
        caster->m_Events.AddEventAtOffset([caster, targetGUID, spellId]
        {
            if (!caster->IsInCombat())
                return;

            if (Unit* target = ObjectAccessor::GetUnit(*caster, targetGUID))
                caster->CastSpell(target, spellId, false);
        }, 400ms);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_eadric_hammer_of_justice::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66867 - Hammer of the Righteous
class spell_eadric_hammer_of_the_righteous : public SpellScript
{
    PrepareSpellScript(spell_eadric_hammer_of_the_righteous);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_HAMMER_OF_JUSTICE_STUN, SPELL_HAMMER_OF_THE_RIGHTEOUS_OVERRIDE });
    }

    void HandleMissile(SpellEffIndex effIndex)
    {
        Unit* target = GetHitUnit();
        if (target->GetTypeId() != TYPEID_PLAYER || target->HasAura(SPELL_HAMMER_OF_JUSTICE_STUN))
            return;

        PreventHitDefaultEffect(effIndex);
        GetCaster()->CastSpell(target, SPELL_HAMMER_OF_THE_RIGHTEOUS_OVERRIDE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_eadric_hammer_of_the_righteous::HandleMissile, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

// 67229 - Mind Control
class spell_toc5_argent_priestess_mind_control : public AuraScript
{
    PrepareAuraScript(spell_toc5_argent_priestess_mind_control);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_CHARM_BREAK });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
            if (caster->IsAlive())
                caster->CastSpell(GetTarget(), SPELL_CHARM_BREAK, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_toc5_argent_priestess_mind_control::AfterRemove, EFFECT_0, SPELL_AURA_MOD_CHARM, AURA_EFFECT_HANDLE_REAL);
    }
};

// 67197, 67292 - Light Rain
class spell_toc5_light_rain : public SpellScript
{
    PrepareSpellScript(spell_toc5_light_rain);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* target)
        {
            switch (target->GetEntry())
            {
                case NPC_ARGENT_LIGHTWIELDER:
                case NPC_ARGENT_MONK:
                case NPC_ARGENT_PRIESTESS:
                    return false;
                default:
                    return true;
            }
        });
        Trinity::Containers::RandomResize(targets, 1);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_toc5_light_rain::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
    }
};

// 66515 - Reflective Shield
class spell_paletress_reflective_shield : public AuraScript
{
    PrepareAuraScript(spell_paletress_reflective_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_REFLECTIVE_SHIELD_DAMAGE });
    }

    bool Load() override
    {
        return GetUnitOwner()->GetMapId() == TrialOfTheChampionMapId;
    }

    void HandleAfterAbsorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* attacker = dmgInfo.GetAttacker();
        if (!attacker || attacker == GetTarget())
            return;

        CastSpellExtraArgs args(aurEff);
        args.AddSpellBP0(CalculatePct(absorbAmount, 25));
        GetTarget()->CastSpell(attacker, SPELL_REFLECTIVE_SHIELD_DAMAGE, args);
    }

    void Register() override
    {
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_paletress_reflective_shield::HandleAfterAbsorb, EFFECT_0);
    }
};

void AddSC_boss_argent_challenge()
{
    RegisterTrialOfTheChampionCreatureAI(boss_eadric);
    RegisterTrialOfTheChampionCreatureAI(boss_paletress);
    RegisterTrialOfTheChampionCreatureAI(npc_memory);
    RegisterTrialOfTheChampionCreatureAI(npc_argent_lightwielder);
    RegisterTrialOfTheChampionCreatureAI(npc_argent_monk);
    RegisterTrialOfTheChampionCreatureAI(npc_argent_priestess);
    RegisterTrialOfTheChampionCreatureAI(npc_toc5_fountain_of_light);
    RegisterSpellScript(spell_paletress_summon_memory);
    RegisterSpellScript(spell_eadric_radiance);
    RegisterSpellScript(spell_eadric_hammer_of_justice);
    RegisterSpellScript(spell_eadric_hammer_of_the_righteous);
    RegisterSpellScript(spell_toc5_light_rain);
    RegisterSpellScript(spell_toc5_argent_priestess_mind_control);
    RegisterSpellScript(spell_paletress_reflective_shield);
}
