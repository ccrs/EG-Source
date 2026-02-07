#include "ScriptMgr.h"
#include "SpellMgr.h"
#include "SpellScript.h"

// 45775 - Cosmetic - Divine Shield Blue
class EG_spell_Cosmetic___Divine_Shield_Blue : public AuraScript
{
    PrepareAuraScript(EG_spell_Cosmetic___Divine_Shield_Blue);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(EG_spell_Cosmetic___Divine_Shield_Blue::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(EG_spell_Cosmetic___Divine_Shield_Blue::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

void AddSC_EG_gen_spell_scripts()
{
    RegisterSpellScript(EG_spell_Cosmetic___Divine_Shield_Blue);
}
