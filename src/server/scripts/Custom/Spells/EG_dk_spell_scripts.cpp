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
