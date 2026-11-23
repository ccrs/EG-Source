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

#include "vault_of_archavon.h"
#include "InstanceScript.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellScript.h"

enum EmalonTexts
{
    EMOTE_OVERCHARGE            = 0,
    EMOTE_MINION_RESPAWN        = 1,
    EMOTE_BERSERK               = 2
};

enum EmalonSpells
{
    SPELL_OVERCHARGE            = 64218,
    SPELL_BERSERK               = 26662,
    SPELL_SUMMON_TEMPEST_MINION = 64232,

    SPELL_SHOCK                 = 64363,
    SPELL_OVERCHARGED           = 64217,
    SPELL_OVERCHARGED_BLAST     = 64219,
    SPELL_INSTAKILL_SELF        = 29878,
    SPELL_CHAIN_LIGHTNING       = 64213,
    SPELL_LIGHTNING_NOVA        = 64216
};

enum EmalonEvents
{
    EVENT_CHAIN_LIGHTNING = 1,
    EVENT_LIGHTNING_NOVA,
    EVENT_OVERCHARGE,
    EVENT_BERSERK,
    EVENT_SUMMON_TEMPEST_MINION,
    EVENT_SHOCK
};

enum EmalonMisc
{
    SUMMON_GROUP_TEMPEST_MINIONS = 0
};

struct boss_emalon : public BossAI
{
    boss_emalon(Creature* creature) : BossAI(creature, DATA_EMALON) { }

    void Reset() override
    {
        _Reset();
        me->SummonCreatureGroup(SUMMON_GROUP_TEMPEST_MINIONS);
    }

    void JustEngagedWith(Unit* who) override
    {
        for (ObjectGuid const& summonGuid : summons)
        {
            Creature* minion = ObjectAccessor::GetCreature(*me, summonGuid);
            if (minion && minion->IsAlive() && !minion->GetVictim() && minion->AI())
                minion->AI()->AttackStart(who);
        }

        events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 15s, 17s);
        events.ScheduleEvent(EVENT_LIGHTNING_NOVA, 17s);
        events.ScheduleEvent(EVENT_BERSERK, 6min);
        events.ScheduleEvent(EVENT_OVERCHARGE, 45s);

        BossAI::JustEngagedWith(who);
    }

    void JustSummoned(Creature* summoned) override
    {
        BossAI::JustSummoned(summoned);

        // AttackStart has nullptr-check for victim
        if (summoned->AI())
            summoned->AI()->AttackStart(me->GetVictim());
    }

    void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/) override
    {
        if (IsEngaged())
            events.ScheduleEvent(EVENT_SUMMON_TEMPEST_MINION, 2s);
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
                case EVENT_CHAIN_LIGHTNING:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 0.0f, true))
                        DoCast(target, SPELL_CHAIN_LIGHTNING);
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 25s);
                    break;
                case EVENT_LIGHTNING_NOVA:
                    DoCastAOE(SPELL_LIGHTNING_NOVA);
                    events.ScheduleEvent(EVENT_LIGHTNING_NOVA, 47s);
                    break;
                case EVENT_OVERCHARGE:
                    if (DoCastSelf(SPELL_OVERCHARGE, CastSpellExtraArgs().AddSpellMod(SPELLVALUE_MAX_TARGETS, 1)) == SPELL_CAST_OK)
                        Talk(EMOTE_OVERCHARGE);
                    events.ScheduleEvent(EVENT_OVERCHARGE, 45s);
                    break;
                case EVENT_BERSERK:
                    DoCast(me, SPELL_BERSERK);
                    Talk(EMOTE_BERSERK);
                    break;
                case EVENT_SUMMON_TEMPEST_MINION:
                    if (DoCastSelf(SPELL_SUMMON_TEMPEST_MINION) == SPELL_CAST_OK)
                        Talk(EMOTE_MINION_RESPAWN);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }
};

struct EG_npc_tempest_minion : public ScriptedAI
{
    EG_npc_tempest_minion(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

    void Reset() override
    {
        _events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        DoZoneInCombat();
        _events.ScheduleEvent(EVENT_SHOCK, 4s, 11s);

        if (Creature* emalon = ObjectAccessor::GetCreature(*me, _instance->GetGuidData(DATA_EMALON)))
        {
            if (!emalon->GetVictim() && emalon->AI())
                emalon->AI()->AttackStart(who);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (_events.ExecuteEvent() == EVENT_SHOCK)
        {
            DoCastVictim(SPELL_SHOCK);
            _events.ScheduleEvent(EVENT_SHOCK, 8s, 15s);
        }

        DoMeleeAttackIfReady();
    }

private:
    InstanceScript* _instance;
    EventMap _events;
};

// 64218 - Overcharge
class EG_spell_emalon_overcharge : public AuraScript
{
    PrepareAuraScript(EG_spell_emalon_overcharge);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_OVERCHARGED, SPELL_OVERCHARGED_BLAST, SPELL_INSTAKILL_SELF });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_OVERCHARGED, aurEff);

        Aura const* overcharged = target->GetAura(SPELL_OVERCHARGED);
        if (!overcharged || overcharged->GetStackAmount() < 10)
            return;

        target->CastSpell(target, SPELL_OVERCHARGED_BLAST, true);
        target->CastSpell(target, SPELL_INSTAKILL_SELF, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_emalon_overcharge::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

void AddSC_boss_emalon()
{
    RegisterVaultOfArchavonCreatureAI(boss_emalon);
    RegisterVaultOfArchavonCreatureAI(EG_npc_tempest_minion);
    RegisterSpellScript(EG_spell_emalon_overcharge);
}
