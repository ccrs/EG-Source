#include "ScriptMgr.h"
#include "Channel.h"
#include "ChannelMgr.h"
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
        
        void OnLogin(Player* player, bool /*firstLogin*/) override
        {
            if (sWorld->getBoolConfig(CONFIG_ACCOUNT_MOUNTS))
            {
                uint32 playerAccountID = player->GetSession()->GetAccountId();
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXISTING_CHARACTER_SPELLS);
                stmt->setUInt32(0, playerAccountID);
                stmt->setUInt32(1, Player::TeamForRace(player->GetRace()) == ALLIANCE ? RACEMASK_ALLIANCE : RACEMASK_HORDE);
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
                    if (relatedInfo && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).Effect == SPELL_EFFECT_APPLY_AURA && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).ApplyAuraName == SPELL_AURA_MOUNTED)
                        player->LearnSpell(relatedInfo->Id, false);
                }
            }
        }
};

class EG_WorldChat : public PlayerScript
{
    public:

        EG_WorldChat() : PlayerScript("EG_WorldChat") { }

        using PlayerScript::OnChat;

        void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel) override
        {
            if (sWorld->getBoolConfig(CONFIG_WORLD_CHAT) && lang != LANG_ADDON && channel->GetName() == WORLD_CHAT)
            {
                if (!player->isGMChat())
                    msg = Trinity::StringFormat("[{}] {}", player->GetTeamId() == TeamId::TEAM_ALLIANCE ? "|cff3399FFA|r" : "|cffCC0000H|r", msg);
            }
        }
};

class EG_XPRate : public PlayerScript
{
    public:
        EG_XPRate() : PlayerScript("EG_XPRate") { }

        void OnGiveXP(Player* player, uint32& amount, Unit* /*unit*/) override
        {
            uint16 storedValue = player->GetCustomFlags(CustomFlagsIndex::CUSTOM_XPRATE_FLAGS);
            if (storedValue > CustomFlags::CUSTOM_FLAG_XPRATE_1)
            {
                uint8 index = 0;
                for (index = 0; index < 7; index++)
                    if (storedValue & (1 << index))
                        break;
                amount *= ++index;
            }
        }
};

void AddSC_EG_player_scripts()
{
    if (sWorld->getBoolConfig(CONFIG_ACCOUNT_MOUNTS))
        new EG_AccountMounts();
    if (sWorld->getBoolConfig(CONFIG_WORLD_CHAT))
        new EG_WorldChat();
    new EG_XPRate();
}
