#include "AccountMgr.h"
#include "Chat.h"
#include "CustomFunctions.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SmartEnum.h"
#include "Language.h"
#include "StringConvert.h"
#include "Util.h"
#include "World.h"
#include "WorldSession.h"

using namespace Trinity::ChatCommands;

class EG_commandscript : public CommandScript
{
public:
    EG_commandscript() : CommandScript("EG_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable transmogrificationSettings =
        {
            { "legendary", HandleDisableLegendaryTransmogrification, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "",          HandleDisableTransmogrification,          rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
        };

        static ChatCommandTable accountSettings =
        {
            { "mount",      HandleAccountMount,      rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "riding",     HandleAccountRiding,     rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "pet",        HandleAccountPet,        rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "flightPath", HandleAccountFlightPath, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
        };

        static ChatCommandTable hardcoreCommands =
        {
            { "list", HandleHardcoreList, rbac::RBAC_ROLE_MODERATOR, Console::Yes },
        };

        static ChatCommandTable customCharacterSettings =
        {
            { "transmogrification", transmogrificationSettings },
            { "aoeloot",            HandleAOELoot, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "account",            accountSettings },
            { "xpRate",             HandleXPRate, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "masquerade",         HandleRaceMasquerade, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "weaponSkill",        HandleWeaponSkill, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "visuals",            HandleVisuals, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "hardcore",           HandleHardcore, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "resetflags",         HandleResetCustomFlags, rbac::RBAC_ROLE_MODERATOR, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "settings",    customCharacterSettings },
            { "hardcore",    hardcoreCommands },
            { "massreward",  HandleMassRewardCommand, rbac::RBAC_ROLE_ADMINISTRATOR, Console::Yes },
        };

        return commandTable;
    }

    // EG - Hardcore
    static bool CheckHardcore(ChatHandler* handler, Player* player)
    {
        if (player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE))
        {
            handler->SendSysMessage("Hardcore characters cannot modify character settings.");
            return true;
        }
        return false;
    }

    static bool HandleHardcore(ChatHandler* handler, Optional<EXACT_SEQUENCE("confirm")> confirmed)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD))
        {
            handler->SendSysMessage("This character fell in Hardcore mode and is permanently dead.");
            return true;
        }

        if (player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE))
        {
            uint32 maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
            if (!sWorld->getIntConfig(CONFIG_HARDCORE_GRACE_PERIOD))
            {
                handler->SendSysMessage("This character is in Hardcore mode and it can never be disabled.");
                return true;
            }

            if (player->GetLevel() < maxLevel)
            {
                handler->PSendSysMessage("This character is in Hardcore mode. Survive to level %u and you will get a one-time window to retire it.", maxLevel);
                return true;
            }

            uint32 secondsLeft = player->GetHardcoreGraceSecondsLeft();
            if (!secondsLeft)
            {
                handler->SendSysMessage("This character is in Hardcore mode and the window to retire it has passed. It can no longer be disabled.");
                return true;
            }

            if (!confirmed)
            {
                handler->SendSysMessage("|cffff0000=== RETIRE HARDCORE MODE ===|r");
                handler->PSendSysMessage("You have %s left to make this choice, and it can only be made once.", secsToTimeString(secondsLeft, TimeFormat::ShortText).c_str());
                handler->SendSysMessage("- Hardcore mode is switched off and death is no longer permanent.");
                handler->SendSysMessage("- A permanent record that you levelled this character in Hardcore is kept. Nothing else from Hardcore is retained.");
                handler->SendSysMessage("- Every other character setting becomes available again.");
                handler->SendSysMessage("- Hardcore mode can never be switched back on.");
                handler->SendSysMessage("If you are certain, type: .settings hardcore confirm");
                return true;
            }

            player->DisableHardcore();
            handler->SendSysMessage("|cff00ff00Hardcore mode retired.|r Your Hardcore levelling record is kept, all character settings are available again and death is no longer permanent.");
            return true;
        }

        if (player->GetLevel() != 1)
        {
            handler->SendSysMessage("Hardcore mode can only be activated at level 1.");
            return true;
        }

        if (!player->IsAlive())
        {
            handler->SendSysMessage("You must be alive to activate Hardcore mode.");
            return true;
        }

        if (!confirmed)
        {
            handler->SendSysMessage("|cffff0000=== HARDCORE MODE WARNING ===|r");
            handler->SendSysMessage("Hardcore mode cannot be switched off once it is on, with a single exception:");
            handler->SendSysMessage("- If this character dies for ANY reason, the death is PERMANENT. You will remain a ghost forever and can never be resurrected by any means.");
            handler->SendSysMessage("- All other character settings are wiped NOW and stay disabled while Hardcore is on.");
            if (uint32 graceHours = sWorld->getIntConfig(CONFIG_HARDCORE_GRACE_PERIOD))
                handler->PSendSysMessage("- The only way out is to survive to level %u. You then get %s of played time to retire Hardcore and return to regular play.",
                    sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL), secsToTimeString(graceHours * HOUR, TimeFormat::ShortText).c_str());
            else
                handler->SendSysMessage("- There is no way out. Hardcore mode can never be removed from this character.");
            handler->SendSysMessage("If you are absolutely certain, type: .settings hardcore confirm");
            return true;
        }

        player->ActivateHardcore();
        handler->SendSysMessage("|cffff0000Hardcore mode activated.|r All other settings have been wiped and permanently disabled. Death is now permanent. Good luck.");
        return true;
    }

    static bool HandleHardcoreList(ChatHandler* handler)
    {
        QueryResult result = CharacterDatabase.PQuery(
            "SELECT c.name, c.level, c.race, c.class, c.account, c.online "
            "FROM characters c "
            "INNER JOIN character_extended ce ON ce.guid = c.guid "
            "WHERE (CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(ce.customFlags, ' ', {}), ' ', -1) AS UNSIGNED) & {}) = {} "
            "ORDER BY c.level DESC, c.name ASC",
            uint32(CustomFlagsIndex::CUSTOM_HARDCORE) + 1,
            uint32(CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE | CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD), uint32(CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE));

        if (!result)
        {
            handler->SendSysMessage("No alive hardcore characters found.");
            return true;
        }

        handler->SendSysMessage("Alive hardcore characters:");
        uint32 count = 0;
        do
        {
            Field* fields = result->Fetch();
            std::string name = fields[0].GetString();
            uint8 level = fields[1].GetUInt8();
            uint8 race = fields[2].GetUInt8();
            uint8 characterClass = fields[3].GetUInt8();
            uint32 accountId = fields[4].GetUInt32();
            bool online = fields[5].GetBool();

            std::string accountName = "<unknown>";
            AccountMgr::GetName(accountId, accountName);

            handler->PSendSysMessage("| - %s (level %u) %s %s, account: %s%s", name.c_str(), uint32(level),
                EnumUtils::ToTitle(Races(race)), EnumUtils::ToTitle(Classes(characterClass)), accountName.c_str(), online ? " [online]" : "");
            ++count;
        }
        while (result->NextRow());

        handler->PSendSysMessage("%u alive hardcore character(s) found.", count);
        return true;
    }

    static bool HandleDisableTransmogrification(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Showing transmoged items, disconnect and reconnect to see this setting applied.");
            return true;
        }
        else
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Hiding transmoged items, disconnect and reconnect to see this setting applied.");
            return true;
        }
    }

    static bool HandleDisableLegendaryTransmogrification(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Showing legendary transmoged items, disconnect and reconnect to see this setting applied.");
            return true;
        }
        else
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Hiding legendary transmoged items, disconnect and reconnect to see this setting applied.");
            return true;
        }
    }

    static bool HandleAOELoot(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_AOELOOT_FLAGS, CustomFlags::CUSTOM_FLAG_AOELOOT_ACTIVE);
            player->StoredLootView.clear();
            player->StoredLoot.clear();
            handler->SendSysMessage("AOE Loot activated.");
            return true;
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_AOELOOT_FLAGS, CustomFlags::CUSTOM_FLAG_AOELOOT_ACTIVE);
            player->StoredLootView.clear();
            player->StoredLoot.clear();
            handler->SendSysMessage("AOE Loot deactivated.");
            return true;
        }
    }

    static bool HandleXPRate(ChatHandler* handler, uint8 rate)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (!rate || rate > 5)
        {
            handler->SendSysMessage("Please use a rate value between 1 and 5.");
            return true;
        }

        player->SetCustomFlags(CustomFlagsIndex::CUSTOM_XPRATE_FLAGS, CustomFlags(1 << (rate - 1)));
        handler->SendSysMessage(Trinity::StringFormat("XP rate modified to {}.", rate));
        return true;
    }

    static bool HandleAccountMount(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_MOUNT, CustomFlags::CUSTOM_FLAG_ACCOUNT_MOUNT_ACTIVE);
            handler->SendSysMessage("Account mounts transfering activated, mounts will be transfered on next character login.");
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_MOUNT, CustomFlags::CUSTOM_FLAG_ACCOUNT_MOUNT_ACTIVE);
            handler->SendSysMessage("Account mounts transfering deactivated.");
        }
        return true;
    }

    static bool HandleAccountRiding(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_RIDING, CustomFlags::CUSTOM_FLAG_ACCOUNT_RIDING_ACTIVE);
            handler->SendSysMessage("Account riding training transfering activated, training level will be transfered on next character login.");
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_RIDING, CustomFlags::CUSTOM_FLAG_ACCOUNT_RIDING_ACTIVE);
            handler->SendSysMessage("Account riding training transfering deactivated.");
        }
        return true;
    }

    static bool HandleAccountPet(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_PET, CustomFlags::CUSTOM_FLAG_ACCOUNT_PET_ACTIVE);
            handler->SendSysMessage("Account pet companions transfering activated, they will be transfered on next character login.");
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_PET, CustomFlags::CUSTOM_FLAG_ACCOUNT_PET_ACTIVE);
            handler->SendSysMessage("Account pet companions transfering deactivated.");
        }
        return true;
    }

    static bool HandleRaceMasquerade(ChatHandler* handler, uint8 value)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (value > 11)
        {
            handler->SendSysMessage("Please use a value between 0 and 11.\nDisconnect and reconnect to see this setting applied.");
            return true;
        }

        if (value == 0)
        {
            player->SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags::CUSTOM_FLAG_RACE_MASQUERADE_HIDE);
            handler->SendSysMessage("Other players Race Masquerade options will be hidden from now on.\nDisconnect and reconnect to see this setting applied.");
            return true;
        }

        if (value == 11)
        {
            player->SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags::CUSTOM_FLAG_NONE);
            handler->SendSysMessage("Displaying your character's original race visual.");
            return true;
        }

        uint8 raceValue = value;
        if (raceValue == 9 || raceValue == 10)
            ++raceValue;

        Races masqueradeRace = Races(raceValue);
        player->SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags(1 << (value)));
        handler->PSendSysMessage(LANG_MASQUERADE_RACE_ENABLED, EnumUtils::ToTitle(masqueradeRace));
        return true;
    }

    static bool HandleWeaponSkill(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_WEAPON_SKILL, CustomFlags::CUSTOM_FLAG_WEAPON_SKILL_ACTIVE);
            player->UpdateWeaponsSkillsToMaxSkillsForLevel();
            handler->SendSysMessage("Weapon Skill setting activated, related skills will remain always maxed.");
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_WEAPON_SKILL, CustomFlags::CUSTOM_FLAG_WEAPON_SKILL_ACTIVE);
            handler->SendSysMessage("Weapon Skill setting deactivated, related skills may need leveling from now on.");
        }
        return true;
    }

    static bool HandleAccountFlightPath(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_TAXI, CustomFlags::CUSTOM_FLAG_ACCOUNT_TAXI_ACTIVE);
            handler->SendSysMessage("Account flight path (same faction) transfering activated, it will be transfered on next character login.");
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_TAXI, CustomFlags::CUSTOM_FLAG_ACCOUNT_TAXI_ACTIVE);
            handler->SendSysMessage("Account flight path transfering deactivated.");
        }
        return true;
    }

    static bool HandleVisuals(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        // EG - Hardcore
        if (CheckHardcore(handler, player))
            return true;

        CustomFlags flag = CustomFlags::CUSTOM_FLAG_NONE;
        switch (player->GetClass())
        {
            case CLASS_DRUID:
                flag = CustomFlags::CUSTOM_FLAG_VISUALS_DRUID_ACTIVE;
                break;
            case CLASS_DEATH_KNIGHT:
                flag = CustomFlags::CUSTOM_FLAG_VISUALS_DEATH_KNIGHT_ACTIVE;
                break;
            case CLASS_SHAMAN:
                flag = CustomFlags::CUSTOM_FLAG_VISUALS_SHAMAN_ACTIVE;
                break;
            case CLASS_WARLOCK:
                flag = CustomFlags::CUSTOM_FLAG_VISUALS_WARLOCK_ACTIVE;
                break;
            case CLASS_MAGE:
                flag = CustomFlags::CUSTOM_FLAG_VISUALS_MAGE_ACTIVE;
                break;
            case CLASS_PALADIN:
                flag = CustomFlags::CUSTOM_FLAG_VISUALS_PALADIN_ACTIVE;
                break;
            case CLASS_HUNTER:
                flag = CustomFlags::CUSTOM_FLAG_VISUALS_HUNTER_ACTIVE;
                break;
            default:
                break;
        }

        if (flag != CustomFlags::CUSTOM_FLAG_NONE)
        {
            if (active)
            {
                player->AddCustomFlag(CustomFlagsIndex::CUSTOM_VISUALS, flag);
                handler->SendSysMessage("Alternative visuals activated.");
            }
            else
            {
                player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_VISUALS, flag);
                if (Pet* pet = player->GetPet())
                    pet->Remove(PET_SAVE_NOT_IN_SLOT, true);
                handler->SendSysMessage("Alternative visuals deactivated.");
            }
        }
        return true;
    }

    static bool HandleResetCustomFlags(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!target)
            target = PlayerIdentifier::FromTarget(handler);
        if (!target)
        {
            handler->SendSysMessage("Please select a player or provide a player name.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // EG - Hardcore
        uint16 preservedHardcore = 0;
        if (Player* player = target->GetConnectedPlayer())
        {
            preservedHardcore = player->GetCustomFlags(CustomFlagsIndex::CUSTOM_HARDCORE);
            for (uint16 i = 0; i < static_cast<uint16>(CustomFlagsIndex::CUSTOM_FLAGS_MAX); ++i)
                if (i != CustomFlagsIndex::CUSTOM_HARDCORE)
                    player->SetCustomFlags(CustomFlagsIndex(i), CustomFlags::CUSTOM_FLAG_NONE);

            if (preservedHardcore & CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE)
            {
                player->SetCustomFlags(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags(CUSTOM_FLAG_TRANSMOG_HIDE | CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY));
                player->SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags::CUSTOM_FLAG_RACE_MASQUERADE_HIDE);
            }
        }
        else
        {
            CharacterDatabasePreparedStatement* selStmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CUSTOM_SETTINGS);
            selStmt->setUInt32(0, target->GetGUID().GetCounter());
            if (PreparedQueryResult result = CharacterDatabase.Query(selStmt))
            {
                std::vector<std::string_view> tokens = Trinity::Tokenize(result->Fetch()[0].GetStringView(), ' ', false);
                if (tokens.size() > CustomFlagsIndex::CUSTOM_HARDCORE)
                    if (Optional<uint16> value = Trinity::StringTo<uint16>(tokens[CustomFlagsIndex::CUSTOM_HARDCORE]))
                        preservedHardcore = *value;
            }
        }

        bool preservedHardcoreActive = (preservedHardcore & CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE) != 0;
        std::ostringstream data;
        for (uint16 i = 0; i < static_cast<uint16>(CustomFlagsIndex::CUSTOM_FLAGS_MAX); ++i)
        {
            if (i == CustomFlagsIndex::CUSTOM_HARDCORE)
                data << preservedHardcore << ' ';
            else if (preservedHardcoreActive && i == CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS)
                data << uint16(CUSTOM_FLAG_TRANSMOG_HIDE | CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY) << ' ';
            else if (preservedHardcoreActive && i == CustomFlagsIndex::CUSTOM_RACE_MASQUERADE)
                data << uint16(CUSTOM_FLAG_RACE_MASQUERADE_HIDE) << ' ';
            else
                data << 0 << ' ';
        }

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CUSTOM_SETTINGS);
        stmt->setUInt32(0, target->GetGUID().GetCounter());
        stmt->setString(1, data.str());
        CharacterDatabase.Execute(stmt);

        if (preservedHardcore)
            handler->PSendSysMessage("Custom flags for player '%s' have been reset to defaults (hardcore state preserved).", target->GetName().c_str());
        else
            handler->PSendSysMessage("Custom flags for player '%s' have been reset to defaults.", target->GetName().c_str());
        return true;
    }

    static bool HandleMassRewardCommand(ChatHandler* handler, uint32 itemEntry, uint32 sinceTimestamp, Optional<uint32> count)
    {
        ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemEntry);
        if (!proto)
        {
            handler->PSendSysMessage("Unknown item entry %u.", itemEntry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 itemCount = count.value_or(1);
        if (itemCount < 1 || (proto->MaxCount > 0 && itemCount > uint32(proto->MaxCount)))
        {
            handler->PSendSysMessage("Invalid count %u for item %u.", itemCount, itemEntry);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // One character per account: highest level, lowest guid on tie.
        // Account eligibility: any character on the account logged out after sinceTimestamp, or is currently online.
        QueryResult result = CharacterDatabase.PQuery(
            "SELECT c.guid, c.name "
            "FROM characters c "
            "WHERE c.account IN (SELECT DISTINCT account FROM characters WHERE logout_time >= {} OR online = 1) "
            "AND c.guid = (SELECT c2.guid FROM characters c2 WHERE c2.account = c.account ORDER BY c2.level DESC, c2.guid ASC LIMIT 1)",
            sinceTimestamp);

        if (!result)
        {
            handler->SendSysMessage("No eligible characters found for the given timestamp.");
            return true;
        }

        MailSender sender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);
        uint32 sent = 0;
        uint32 failed = 0;

        do
        {
            Field* fields = result->Fetch();
            ObjectGuid::LowType charGuid = fields[0].GetUInt32();
            std::string charName = fields[1].GetString();

            Item* item = Item::CreateItem(itemEntry, itemCount, handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr);
            if (!item)
            {
                ++failed;
                continue;
            }

            Player* receiver = ObjectAccessor::FindPlayerByName(charName);
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            item->SaveToDB(trans);                              // Save to prevent being lost at next mail load. If send fails, the item will be deleted.

            MailDraft draft("Server Reward", "Please accept this item as a reward for your dedication and continuous support.");
            draft.AddItem(item);
            draft.SendMailTo(trans, MailReceiver(receiver, charGuid), sender);
            CharacterDatabase.CommitTransaction(trans);
            ++sent;

        }
        while (result->NextRow());

        handler->PSendSysMessage("Mass reward complete: item %u (x%u) sent to %u character(s).", itemEntry, itemCount, sent);
        if (failed > 0)
            handler->PSendSysMessage("Warning: item creation failed for %u character(s).", failed);
        return true;
    }
};

void AddSC_EG_commandscript()
{
    new EG_commandscript();
}
