#include "ScriptMgr.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "Language.h"
#include "Mail.h"
#include "Player.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "World.h"
#include "WorldSession.h"
#include <unordered_set>


class EG_AccountSpells : public PlayerScript
{
    public:
        EG_AccountSpells() : PlayerScript("EG_AccountSpells") { }

        void OnLevelChanged(Player* player, uint8/* oldLevel*/) override
        {
            if (!player->HasCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_RIDING, CustomFlags::CUSTOM_FLAG_ACCOUNT_RIDING_ACTIVE)
                || (player->HasSpell(34091) /*Artisan Riding (Artisan)*/ && player->HasSpell(54197) /*Cold Weather Flying (Passive)*/)  // Lets try to be efficient here...
            )
                return;

            uint32 playerAccountID = player->GetSession()->GetAccountId();
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXISTING_CHARACTER_SPELLS);
            stmt->setUInt32(0, playerAccountID);
            stmt->setUInt32(1, player->GetGUID().GetCounter());

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

            bool searchForRiding = player->HasCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_RIDING, CustomFlags::CUSTOM_FLAG_ACCOUNT_RIDING_ACTIVE);
            for (uint32 spellId : spellIds)
            {
                SpellInfo const* relatedInfo = sSpellMgr->GetSpellInfo(spellId);
                if (!relatedInfo)
                    continue;
                if (searchForRiding)
                {
                    switch (relatedInfo->Id)
                    {
                        case 33388: // Apprentice Riding (Apprentice)
                        case 5784: // Felsteed (Summon)
                        case 13819: // Warhorse (Summon)
                        case 34769: // Summon Warhorse (Summon)
                            if (player->GetLevel() >= 20)
                                player->LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                            break;
                        case 33391: // Journeyman Riding (Journeyman)
                        case 23161: // Dreadsteed (Summon)
                        case 23214: // Charger (Summon)
                        case 34767: // Summon Charger (Summon)
                        case 48778: // Acherus Deathcharger (Summon)
                            if (player->GetLevel() >= 40)
                                player->LearnSpell(33391, false); // Journeyman Riding (Journeyman)
                            else if (player->GetLevel() >= 20)
                                player->LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                            break;
                        case 34090: // Expert Riding (Expert)
                        case 33943: // Flight Form (Shapeshift)
                            if (player->GetLevel() >= 60)
                                player->LearnSpell(34090, false); // Expert Riding (Expert)
                            else if (player->GetLevel() >= 40)
                                player->LearnSpell(33391, false); // Journeyman Riding (Journeyman)
                            else if (player->GetLevel() >= 20)
                                player->LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                            break;
                        case 34091: // Artisan Riding (Artisan)
                        case 40120: // Swift Flight Form (Shapeshift)
                            if (player->GetLevel() >= 70)
                                player->LearnSpell(34091, false);
                            else if (player->GetLevel() >= 60)
                                player->LearnSpell(34090, false); // Expert Riding (Expert)
                            else if (player->GetLevel() >= 40)
                                player->LearnSpell(33391, false); // Journeyman Riding (Journeyman)
                            else if (player->GetLevel() >= 20)
                                player->LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                            break;
                        case 54197: // Cold Weather Flying (Passive)
                            if (player->GetLevel() >= 68)
                                player->LearnSpell(relatedInfo->Id, false);
                            break;
                    }
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
            if (sWorld->getBoolConfig(CONFIG_WORLD_CHAT) && lang != LANG_ADDON && Channel::IsWorldChat(channel->GetName()))
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
            if (player->GetClass() == CLASS_DEATH_KNIGHT && player->GetMapId() == 609 && !player->IsGameMaster() && !player->HasSpell(50977))
                return;
            uint16 storedValue = player->GetCustomFlags(CustomFlagsIndex::CUSTOM_XPRATE_FLAGS);
            if (storedValue > CustomFlags::CUSTOM_FLAG_XPRATE_1)
            {
                uint8 index = 0;
                for (index = 0; index < 5; index++)
                    if (storedValue & (1 << index))
                        break;
                amount *= ++index;
            }
        }
};

class EG_Hardcore : public PlayerScript
{
    public:
        EG_Hardcore() : PlayerScript("EG_Hardcore") { }

        void OnLogin(Player* player, bool /*firstLogin*/) override
        {
            if (!player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE))
                return;

            ChatHandler handler(player->GetSession());
            if (player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD))
            {
                if (player->IsAlive())
                    player->setDeathState(JUST_DIED);

                handler.SendSysMessage("|cffff0000This character fell in Hardcore mode and is permanently dead.|r");
            }
            else
                handler.SendSysMessage("|cffff8000Hardcore mode is active on this character: death is permanent.|r");
        }

        void OnLevelChanged(Player* player, uint8 /*oldLevel*/) override
        {
            if (!player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE)
                || player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD))
                return;

            if (player->GetLevel() % 10 == 0)
                GrantMilestone(player, player->GetLevel());
        }

    private:
        static void GrantMilestone(Player* player, uint8 tier)
        {
            uint32 money = 0;
            uint32 itemId = 0;
            uint32 itemCount = 0;
            uint32 spellId = 0;

            switch (tier)
            {
                case 10:
                    money = 1 * GOLD;
                    break;
                case 20:
                    money = 25000;
                    break;
                case 30:
                    money = 5 * GOLD;
                    itemId = 21841; // Netherweave Bag
                    itemCount = 2;
                    break;
                case 40:
                    money = 25 * GOLD;
                    itemId = 28788; // Tabard of the Protector
                    itemCount = 1;
                    break;
                case 50:
                    money = 50 * GOLD;
                    itemId = 21841; // Netherweave Bag
                    itemCount = 2;
                    break;
                case 60:
                    money = 125 * GOLD;
                    break;
                case 70:
                    money = 250 * GOLD;
                    spellId = 63318; // Summon Murkimus the Gladiator
                    break;
                case 80:
                    money = 500 * GOLD;
                    spellId = 51412; // Big Battle Bear
                    break;
                default:
                    return;
            }

            CustomFlags rewardFlag = CustomFlags(CUSTOM_FLAG_HARDCORE_REWARD_10 << (tier / 10 - 1));
            if (player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, rewardFlag))
                return;

            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, rewardFlag);

            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            MailDraft draft(Trinity::StringFormat("Hardcore Milestone: Level {}", tier), "Your dedication to the one-life journey has been noticed. Take these supplies and press on.");
            draft.AddMoney(money);
            for (uint32 i = 0; i < itemCount; ++i)
            {
                if (Item* item = Item::CreateItem(itemId, 1, player))
                {
                    item->SaveToDB(trans);
                    draft.AddItem(item);
                }
            }
            draft.SendMailTo(trans, player, MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM));
            CharacterDatabase.CommitTransaction(trans);

            if (spellId)
                player->LearnSpell(spellId, false);

            ChatHandler handler(player->GetSession());
            if (tier == 80)
                handler.PSendSysMessage(LANG_HARDCORE_MILESTONE_FINAL, uint32(tier));
            else
                handler.PSendSysMessage(LANG_HARDCORE_MILESTONE, uint32(tier), uint32(tier) + 10);
        }
};

void AddSC_EG_player_scripts()
{
    new EG_AccountSpells();
    if (sWorld->getBoolConfig(CONFIG_WORLD_CHAT))
        new EG_WorldChat();
    new EG_XPRate();
    new EG_Hardcore();
}
