#include "ScriptMgr.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Unit.h"

// 3045 - Rapid Fire
class EG_hunter_rapid_fire : public AuraScript
{
    PrepareAuraScript(EG_hunter_rapid_fire);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        if (Player* owner = target->ToPlayer())
            if (owner->HasCustomFlag(CustomFlagsIndex::CUSTOM_VISUALS, CustomFlags::CUSTOM_FLAG_VISUALS_HUNTER_ACTIVE))
            {
                if (owner->GetTeamId() == TeamId::TEAM_HORDE)
                {
                    owner->SetDisplayId(28213);
                    owner->Yell("For the Forsaken!", Language::LANG_UNIVERSAL);
                }
                else
                {
                    owner->SetDisplayId(7274);
                    owner->Yell("For Elune!", Language::LANG_UNIVERSAL);
                }
            }
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTarget()->GetDisplayId() == 28213 || GetTarget()->GetDisplayId() == 7274)
            GetTarget()->RestoreDisplayId();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_hunter_rapid_fire::HandleApply, EFFECT_0, SPELL_AURA_MOD_RANGED_HASTE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        AfterEffectRemove += AuraEffectRemoveFn(EG_hunter_rapid_fire::HandleRemove, EFFECT_0, SPELL_AURA_MOD_RANGED_HASTE, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_EG_hunter_spell_scripts()
{
    RegisterSpellScript(EG_hunter_rapid_fire);
}
