#include "ScriptMgr.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "World.h"
#include "WorldSession.h"
#include <unordered_set>


class EG_AccountMounts : public PlayerScript
{
    public:
        EG_AccountMounts() : PlayerScript("EG_AccountMounts") { }
        
        void OnPlayerLogin(Player* player)
        {
            if (sWorld->getBoolConfig(CONFIG_ACCOUNT_MOUNTS))
            {
                uint32 playerAccountID = player->GetSession()->GetAccountId();
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXISTING_CHARACTER_SPELLS);
                stmt->setUInt32(0, playerAccountID);
                stmt->setUInt32(1, Player::TeamForRace(GetPlayer()->GetRace()) == ALLIANCE ? RACEMASK_ALLIANCE : RACEMASK_HORDE);
                stmt->setUInt32(2, player->GetGUID().GetCounter());

                std::unordered_set<uint32> spellIds;
                if (PreparedQueryResult resultCharacterSpells = CharacterDatabase.Query(stmt))
                {
                    do
                    {
                        Field* fields = resultCharacterSpells->Fetch();
                        uint32 spellId = fields[0].GetUInt32();
                        spellIds.insert(spellId);
                    }
                    while (resultCharacterSpells->NextRow());
                }

                for (uint32 spellId : spellIds)
                {
                    SpellInfo const* relatedInfo = sSpellMgr->GetSpellInfo(spellId);
                    if (relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).Effect == SPELL_EFFECT_APPLY_AURA && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).ApplyAuraName == SPELL_AURA_MOUNTED)
                        player->learnSpell(sSpell->Id);
                }
            }
        }
};

void AddSC_EG_player_scripts()
{
    new EG_AccountMounts();
}
