#include "ScriptMgr.h"
#include "Spell.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Unit.h"

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

enum DestroyDeathforgedInfernal
{
    SPELL_RANDOM_ROCKET_MISSILE = 38054,
    SPELL_FRS_QUEST_CREDIT = 38022,
    SPELL_FEL_REAVER_CONTROLLER_1 = 38002,
    SPELL_FEL_REAVER_CONTROLLER_1_1 = 38003,
    SPELL_FEL_REAVER_CONTROLLER_2 = 38120,
    SPELL_FEL_REAVER_CONTROLLER_2_1 = 38121,
    SPELL_FEL_REAVER_CONTROLLER_3 = 38122,
    SPELL_FEL_REAVER_CONTROLLER_3_1 = 38123,
    SPELL_FEL_REAVER_CONTROLLER_4 = 38125,
    SPELL_FEL_REAVER_CONTROLLER_4_1 = 38126,
    SPELL_FEL_REAVER_CONTROLLER_5 = 38127,
    SPELL_FEL_REAVER_CONTROLLER_5_1 = 38128,
    SPELL_FEL_REAVER_CONTROLLER_6 = 38129,
    SPELL_FEL_REAVER_CONTROLLER_6_1 = 38130
};

// 38055 - Destroy Deathforged Infernal
class EG_spell_destroy_deathforged_infernal : public SpellScript
{
    PrepareSpellScript(EG_spell_destroy_deathforged_infernal);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_RANDOM_ROCKET_MISSILE });
    }

    void HandleScript(SpellEffIndex effIndex)
    {
        GetHitUnit()->CastSpell(nullptr, SPELL_RANDOM_ROCKET_MISSILE, CastSpellExtraArgs(TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD | TRIGGERED_IGNORE_CAST_IN_PROGRESS)));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_destroy_deathforged_infernal::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 38020 - Fel Reaver Sentinel Tag
class EG_spell_fel_reaver_sentinel_tag : public SpellScript
{
    PrepareSpellScript(EG_spell_fel_reaver_sentinel_tag);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_FRS_QUEST_CREDIT });
    }

    void HandleSpell(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        target->CastSpell(nullptr, SPELL_FRS_QUEST_CREDIT);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_fel_reaver_sentinel_tag::HandleSpell, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 38002 - Fel Reaver Controller
// 38120 - Fel Reaver Controller
// 38122 - Fel Reaver Controller
// 38125 - Fel Reaver Controller
// 38127 - Fel Reaver Controller
// 38129 - Fel Reaver Controller
class EG_spell_fel_reaver_controller : public SpellScript
{
    PrepareSpellScript(EG_spell_fel_reaver_controller);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_FEL_REAVER_CONTROLLER_1_1,
            SPELL_FEL_REAVER_CONTROLLER_2_1,
            SPELL_FEL_REAVER_CONTROLLER_3_1,
            SPELL_FEL_REAVER_CONTROLLER_4_1,
            SPELL_FEL_REAVER_CONTROLLER_5_1,
            SPELL_FEL_REAVER_CONTROLLER_6_1
        });
    }

    void HandleSpell(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        uint32 spelld = GetSpellInfo()->Id + 1;
        target->CastSpell(nullptr, spelld);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_fel_reaver_controller::HandleSpell, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_EG_gen_spell_scripts()
{
    RegisterSpellScript(EG_spell_Cosmetic___Divine_Shield_Blue);
    RegisterSpellScript(EG_spell_destroy_deathforged_infernal);
    RegisterSpellScript(EG_spell_fel_reaver_sentinel_tag);
    RegisterSpellScript(EG_spell_fel_reaver_controller);
}
