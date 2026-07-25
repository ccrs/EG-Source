#include "ScriptMgr.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Unit.h"

// 58686 - Glyph of the Ghoul
class EG_spell_dk_glyph_of_the_ghoul : public AuraScript
{
    PrepareAuraScript(EG_spell_dk_glyph_of_the_ghoul);

    void HandleChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Guardian* pet = GetTarget()->GetGuardianPet())
            if (pet->IsPetGhoul())
                pet->UpdateAllStats();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_spell_dk_glyph_of_the_ghoul::HandleChange, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(EG_spell_dk_glyph_of_the_ghoul::HandleChange, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_EG_dk_spell_scripts()
{
    RegisterSpellScript(EG_spell_dk_glyph_of_the_ghoul);
}
