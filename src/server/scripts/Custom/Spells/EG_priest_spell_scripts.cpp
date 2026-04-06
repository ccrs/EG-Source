#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Unit.h"


class EG_spell_priest_mind_control : public SpellScript
{
    PrepareSpellScript(EG_spell_priest_mind_control);

    SpellCastResult CheckCast()
    {
        if (GetExplTargetUnit()->GetVehicle())
            return SPELL_FAILED_BAD_TARGETS;

        return SPELL_CAST_OK;
    }

    void HandleAfterHit()
    {
        Unit* target = GetExplTargetUnit();
        Unit* caster = GetCaster();
        if (!caster || !target)
            return;

        Aura* auraCaster = caster->GetAura(GetSpellInfo()->Id, caster->GetGUID());
        if (!auraCaster)
            return;

        Aura* auraTarget = target->GetAura(GetSpellInfo()->Id, caster->GetGUID());
        if (!auraTarget)
            return;

        int32 duration = auraTarget->GetDuration();
        auraCaster->SetDuration(duration);
        auraCaster->SetMaxDuration(auraTarget->GetMaxDuration());

        if (Spell* spell = GetSpell())
            spell->SendChannelStart(duration);
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(EG_spell_priest_mind_control::CheckCast);
        AfterHit += SpellHitFn(EG_spell_priest_mind_control::HandleAfterHit);
    }
};

class EG_spell_priest_mind_control_aura : public AuraScript
{
    PrepareAuraScript(EG_spell_priest_mind_control_aura);

    void HandleRemoveAuraPossess(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            caster->RemoveAurasDueToSpell(GetSpellInfo()->Id);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(EG_spell_priest_mind_control_aura::HandleRemoveAuraPossess, EFFECT_0, SPELL_AURA_MOD_POSSESS, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_EG_priest_spell_scripts()
{
    RegisterSpellAndAuraScriptPair(EG_spell_priest_mind_control, EG_spell_priest_mind_control_aura);
}
