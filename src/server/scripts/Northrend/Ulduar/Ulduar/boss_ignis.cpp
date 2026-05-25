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
#include "Containers.h"
#include "GameTime.h"
#include "GridNotifiers.h"
#include "InstanceScript.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum IgnisYells
{
    SAY_AGGRO       = 0,
    SAY_SUMMON      = 1,
    SAY_SLAG_POT    = 2,
    SAY_SCORCH      = 3,
    SAY_SLAY        = 4,
    SAY_BERSERK     = 5,
    SAY_DEATH       = 6,
    EMOTE_JETS      = 7
};

enum IgnisSpells
{
    SPELL_FLAME_JETS            = 62680,
    SPELL_SCORCH                = 62546,
    SPELL_SLAG_POT              = 62717,
    SPELL_SLAG_POT_DAMAGE       = 65722,
    SPELL_SLAG_IMBUED           = 62836,
    SPELL_ACTIVATE_CONSTRUCT    = 62488,
    SPELL_STRENGHT              = 64473,
    SPELL_GRAB                  = 62707,
    SPELL_BERSERK               = 47008,

    // Iron Construct
    SPELL_HEAT                  = 65667,
    SPELL_MOLTEN                = 62373,
    SPELL_BRITTLE               = 62382,
    SPELL_BRITTLE_25            = 67114,
    SPELL_SHATTER               = 62383,
    SPELL_GROUND                = 62548,
    SPELL_FREEZE_ANIM           = 16245,
};

enum IgnisEvents
{
    EVENT_JET           = 1,
    EVENT_SCORCH        = 2,
    EVENT_SLAG_POT      = 3,
    EVENT_GRAB_POT      = 4,
    EVENT_CHANGE_POT    = 5,
    EVENT_END_POT       = 6,
    EVENT_CONSTRUCT     = 7,
    EVENT_BERSERK       = 8,
};

enum IgnisActions
{
    ACTION_REMOVE_BUFF          = 20,
    ACTION_ACTIVATE_CONSTRUCT,
    ACTION_RESET_DORMANT,
};

enum IgnisCreatures
{
    NPC_GROUND_SCORCH   = 33221,
};

enum IgnisAchievementData
{
    DATA_SHATTERED                  = 29252926,
    ACHIEVEMENT_IGNIS_START_EVENT   = 20951,
};

class boss_ignis : public CreatureScript
{
    public:
        boss_ignis() : CreatureScript("boss_ignis") { }

        struct boss_ignis_AI : public BossAI
        {
            boss_ignis_AI(Creature* creature) : BossAI(creature, DATA_IGNIS)
            {
                Initialize();
            }

            void Initialize()
            {
                _slagPotGUID.Clear();
                _shattered = false;
                _firstConstructKill = 0;
            }

            void Reset() override
            {
                _Reset();
                if (Vehicle* _vehicle = me->GetVehicleKit())
                    _vehicle->RemoveAllPassengers();

                instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_IGNIS_START_EVENT);

                std::list<Creature*> constructs;
                me->GetCreatureListWithEntryInGrid(constructs, NPC_IRON_CONSTRUCT, 200.0f);
                for (Creature* construct : constructs)
                {
                    if (construct->IsAlive())
                    {
                        if (construct->GetReactState() == REACT_PASSIVE)
                            continue;
                        if (construct->IsAIEnabled())
                            construct->AI()->DoAction(ACTION_RESET_DORMANT);
                    }
                    else
                        construct->Respawn(true);
                }
            }

            void JustEngagedWith(Unit* who) override
            {
                BossAI::JustEngagedWith(who);
                Talk(SAY_AGGRO);
                events.ScheduleEvent(EVENT_JET, 30s);
                events.ScheduleEvent(EVENT_SCORCH, 25s);
                events.ScheduleEvent(EVENT_SLAG_POT, 35s);
                events.ScheduleEvent(EVENT_CONSTRUCT, 15s);
                events.ScheduleEvent(EVENT_BERSERK, 480s);
                Initialize();
                instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_IGNIS_START_EVENT);
            }

            void JustDied(Unit* /*killer*/) override
            {
                _JustDied();
                Talk(SAY_DEATH);
            }

            uint32 GetData(uint32 type) const override
            {
                if (type == DATA_SHATTERED)
                    return _shattered ? 1 : 0;

                return 0;
            }

            void KilledUnit(Unit* who) override
            {
                if (who->GetTypeId() == TYPEID_PLAYER)
                    Talk(SAY_SLAY);
            }

            void DoAction(int32 action) override
            {
                if (action != ACTION_REMOVE_BUFF)
                    return;

                me->RemoveAuraFromStack(SPELL_STRENGHT);
                // Shattered Achievement
                time_t secondKill = GameTime::GetGameTime();
                if ((secondKill - _firstConstructKill) < 5)
                    _shattered = true;
                _firstConstructKill = secondKill;
            }

            void UpdateAI(uint32 diff) override
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_JET:
                            Talk(EMOTE_JETS);
                            DoCast(me, SPELL_FLAME_JETS);
                            events.ScheduleEvent(EVENT_JET, 35s, 40s);
                            break;
                        case EVENT_SLAG_POT:
                            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true, false))
                            {
                                Talk(SAY_SLAG_POT);
                                _slagPotGUID = target->GetGUID();
                                DoCast(target, SPELL_GRAB);
                                events.DelayEvents(3s);
                                events.ScheduleEvent(EVENT_GRAB_POT, 500ms);
                            }
                            events.ScheduleEvent(EVENT_SLAG_POT, RAID_MODE(30s, 15s));
                            break;
                        case EVENT_GRAB_POT:
                            if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                            {
                                slagPotTarget->EnterVehicle(me, 0);
                                events.CancelEvent(EVENT_GRAB_POT);
                                events.ScheduleEvent(EVENT_CHANGE_POT, 1s);
                            }
                            break;
                        case EVENT_CHANGE_POT:
                            if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                            {
                                DoCast(slagPotTarget, SPELL_SLAG_POT, true);
                                slagPotTarget->EnterVehicle(me, 1);
                                events.CancelEvent(EVENT_CHANGE_POT);
                                events.ScheduleEvent(EVENT_END_POT, 10s);
                            }
                            break;
                        case EVENT_END_POT:
                            if (Unit* slagPotTarget = ObjectAccessor::GetUnit(*me, _slagPotGUID))
                            {
                                slagPotTarget->ExitVehicle();
                                slagPotTarget = nullptr;
                                _slagPotGUID.Clear();
                                events.CancelEvent(EVENT_END_POT);
                            }
                            break;
                        case EVENT_SCORCH:
                            Talk(SAY_SCORCH);
                            if (Unit* target = me->GetVictim())
                                me->SummonCreature(NPC_GROUND_SCORCH, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 45s);
                            DoCast(SPELL_SCORCH);
                            events.ScheduleEvent(EVENT_SCORCH, 25s);
                            break;
                        case EVENT_CONSTRUCT:
                            DoCastAOE(SPELL_ACTIVATE_CONSTRUCT);
                            events.ScheduleEvent(EVENT_CONSTRUCT, RAID_MODE(40s, 30s));
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            Talk(SAY_BERSERK);
                            break;
                    }

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;
                }

                DoMeleeAttackIfReady();
            }

        private:
            ObjectGuid _slagPotGUID;
            time_t _firstConstructKill;
            bool _shattered;

        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<boss_ignis_AI>(creature);
        }
};

class npc_iron_construct : public CreatureScript
{
    public:
        npc_iron_construct() : CreatureScript("npc_iron_construct") { }

        struct npc_iron_constructAI : public ScriptedAI
        {
            npc_iron_constructAI(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

            void JustAppeared() override
            {
                EnterDormantState();
            }

            void DoAction(int32 action) override
            {
                switch (action)
                {
                    case ACTION_ACTIVATE_CONSTRUCT:
                        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                        me->SetImmuneToPC(false);
                        me->SetControlled(false, UNIT_STATE_ROOT);
                        me->SetControlled(false, UNIT_STATE_STUNNED);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                        if (Creature* ignis = _instance->GetCreature(DATA_IGNIS))
                            if (Unit* victim = ignis->GetVictim())
                                AttackStart(victim);
                        DoZoneInCombat();
                        break;
                    case ACTION_RESET_DORMANT:
                        me->DespawnOrUnsummon(0s, 1s);
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
            {
                if (me->HasAura(RAID_MODE(SPELL_BRITTLE, SPELL_BRITTLE_25)) && damage >= 5000)
                {
                    me->SetReactState(REACT_PASSIVE);
                    DoCast(SPELL_SHATTER);
                    if (Creature* ignis = _instance->GetCreature(DATA_IGNIS))
                        if (ignis->AI())
                            ignis->AI()->DoAction(ACTION_REMOVE_BUFF);

                    me->DespawnOrUnsummon(1s);
                }
            }

            void UpdateAI(uint32 /*uiDiff*/) override
            {
                if (!UpdateVictim())
                    return;

                if (Aura* aur = me->GetAura(SPELL_HEAT))
                {
                    if (aur->GetStackAmount() >= 10)
                    {
                        me->RemoveAura(SPELL_HEAT);
                        DoCast(SPELL_MOLTEN);
                    }
                }

                if (me->IsInWater() && me->HasAura(SPELL_MOLTEN))
                {
                    DoCast(SPELL_BRITTLE);
                    me->RemoveAura(SPELL_MOLTEN);
                }

                DoMeleeAttackIfReady();
            }

        private:
            void EnterDormantState()
            {
                me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                me->SetImmuneToPC(true);
                me->SetControlled(true, UNIT_STATE_ROOT);
                me->SetControlled(true, UNIT_STATE_STUNNED);
                me->SetReactState(REACT_PASSIVE);
                DoCastSelf(SPELL_FREEZE_ANIM, true);
            }

            InstanceScript* _instance;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_iron_constructAI>(creature);
        }
};

class npc_scorch_ground : public CreatureScript
{
    public:
        npc_scorch_ground() : CreatureScript("npc_scorch_ground") { }

        struct npc_scorch_groundAI : public ScriptedAI
        {
            npc_scorch_groundAI(Creature* creature) : ScriptedAI(creature)
            {
                Initialize();
                me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_PACIFIED);
                me->SetControlled(true, UNIT_STATE_ROOT);
                creature->SetDisplayId(16925); //model 2 in db cannot overwrite wdb fields
            }

            void Initialize()
            {
                _heat = false;
                _constructGUID.Clear();
                _heatTimer = 0;
            }

            void MoveInLineOfSight(Unit* who) override
            {
                if (!_heat)
                {
                    if (who->GetEntry() == NPC_IRON_CONSTRUCT)
                    {
                        if (!who->HasAura(SPELL_HEAT) && !who->HasAura(SPELL_MOLTEN))
                        {
                            _constructGUID = who->GetGUID();
                            _heat = true;
                        }
                    }
                }
            }

            void Reset() override
            {
                Initialize();
                DoCast(me, SPELL_GROUND);
            }

            void UpdateAI(uint32 uiDiff) override
            {
                if (_heat)
                {
                    if (_heatTimer <= uiDiff)
                    {
                        Creature* construct = ObjectAccessor::GetCreature(*me, _constructGUID);
                        if (construct && !construct->HasAura(SPELL_MOLTEN))
                        {
                            me->AddAura(SPELL_HEAT, construct);
                            _heatTimer = 1000;
                        }
                    }
                    else
                        _heatTimer -= uiDiff;
                }
            }

        private:
            ObjectGuid _constructGUID;
            uint32 _heatTimer;
            bool _heat;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return GetUlduarAI<npc_scorch_groundAI>(creature);
        }
};

// 62717, 63477 - Slag Pot
class spell_ignis_slag_pot : public SpellScriptLoader
{
    public:
        spell_ignis_slag_pot() : SpellScriptLoader("spell_ignis_slag_pot") { }

        class spell_ignis_slag_pot_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ignis_slag_pot_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                return ValidateSpellInfo({ SPELL_SLAG_POT_DAMAGE, SPELL_SLAG_IMBUED });
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                {
                    Unit* target = GetTarget();
                    caster->CastSpell(target, SPELL_SLAG_POT_DAMAGE, true);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->IsAlive())
                    GetTarget()->CastSpell(GetTarget(), SPELL_SLAG_IMBUED, true);
            }

            void Register() override
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ignis_slag_pot_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ignis_slag_pot_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_ignis_slag_pot_AuraScript();
        }
};

// 62488 - Activate Construct
class EG_spell_ignis_activate_construct : public SpellScript
{
    PrepareSpellScript(EG_spell_ignis_activate_construct);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_STRENGHT });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* obj)
        {
            Creature const* c = obj->ToCreature();
            return !c || !c->IsAlive() || c->GetReactState() != REACT_PASSIVE;
        });
        if (targets.empty())
            return;
        WorldObject* chosen = Trinity::Containers::SelectRandomContainerElement(targets);
        targets.clear();
        targets.push_back(chosen);
    }

    void HandleScriptEffect(SpellEffIndex /*effIndex*/)
    {
        Creature* construct = GetHitCreature();
        Unit* caster = GetCaster();
        if (!construct || !caster)
            return;

        if (construct->IsAIEnabled())
            construct->AI()->DoAction(ACTION_ACTIVATE_CONSTRUCT);

        if (Creature* ignis = caster->ToCreature())
            if (ignis->IsAIEnabled())
                ignis->AI()->Talk(SAY_SUMMON);

        caster->CastSpell(caster, SPELL_STRENGHT, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_ignis_activate_construct::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(EG_spell_ignis_activate_construct::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class achievement_ignis_shattered : public AchievementCriteriaScript
{
    public:
        achievement_ignis_shattered() : AchievementCriteriaScript("achievement_ignis_shattered") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            if (UnitAI* ai = target ? target->GetAI() : nullptr)
                return ai->GetData(DATA_SHATTERED) != 0;

            return false;
        }
};

void AddSC_boss_ignis()
{
    new boss_ignis();
    new npc_iron_construct();
    new npc_scorch_ground();
    RegisterSpellScript(EG_spell_ignis_activate_construct);
    new spell_ignis_slag_pot();
    new achievement_ignis_shattered();
}
