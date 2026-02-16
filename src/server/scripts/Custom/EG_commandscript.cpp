#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SmartEnum.h"
#include "Language.h"
#include "Util.h"
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
            { "mount",  HandleAccountMount,  rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "riding", HandleAccountRiding, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
        };

        static ChatCommandTable customCharacterSettings =
        {
            { "transmogrification", transmogrificationSettings }, 
            { "aoeloot",            HandleAOELoot, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "account",            accountSettings },
            { "xpRate",             HandleXPRate, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "masquerade",         HandleRaceMasquerade, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
        };

        static ChatCommandTable commandTable =
        {
            { "settings", customCharacterSettings },
        };

        return commandTable;
    }

    static bool HandleDisableTransmogrification(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (active)
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Showing transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
        else
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Hiding transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
    }

    static bool HandleDisableLegendaryTransmogrification(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (active)
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Showing legendary transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
        else
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Hiding legendary transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
    }

    static bool HandleAOELoot(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_AOELOOT_FLAGS, CustomFlags::CUSTOM_FLAG_AOELOOT_ACTIVE);
            player->AOELootView.clear();
            player->AOELoot.clear();
            handler->SendSysMessage("AOE Loot activated.");
            return true;
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_AOELOOT_FLAGS, CustomFlags::CUSTOM_FLAG_AOELOOT_ACTIVE);
            player->AOELootView.clear();
            player->AOELoot.clear();
            handler->SendSysMessage("AOE Loot deactivated.");
            return true;
        }
    }

    static bool HandleXPRate(ChatHandler* /*handler*/, uint8 /*rate*/)
    {
        /*Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (!rate || rate > 6)
        {
            handler->SendSysMessage("Please use a rate value between 1 and 5.");
            return true;
        }

        player->SetCustomFlags(CustomFlagsIndex::CUSTOM_XPRATE_FLAGS, CustomFlags(1 << (rate - 1)));
        handler->SendSysMessage(Trinity::StringFormat("XP rate modified to {}.", rate));*/
        return true;
    }

    static bool HandleAccountMount(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

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

    static bool HandleRaceMasquerade(ChatHandler* handler, uint8 value)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (value > 11)
        {
            handler->SendSysMessage("Please use a value between 0 and 11.");
            return true;
        }

        if (value == 0)
        {
            player->SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags::CUSTOM_FLAG_RACE_MASQUERADE_HIDE);
            player->SetMasqueradeRace(RACE_NONE);
            handler->SendSysMessage("Other player's Race Masquerade options will be hidden from now on.");
            return true;
        }

        uint8 raceValue = value;
        if (raceValue == 9 || raceValue == 10)
            ++raceValue;

        uint32 playerTeam = Player::TeamForRace(player->GetRace());
        if (Player::TeamForRace(raceValue) != playerTeam)
        {
            std::string racesForTeam;
            if (playerTeam == ALLIANCE)
                racesForTeam = "1 - Human\n3 - Dwarf\n4 - Nightelf\n7 - Gnome\n10 - Dranei";
            else
                racesForTeam = "2 - Orc\n5 - Undead\n6 - Tauren\n8 - Troll\n9 - Bloodelf";
            handler->SendSysMessage("Please select a race from your current Faction.\nOptions: " + racesForTeam);
            return true;
        }

        Races masqueradeRace = Races(raceValue);
        player->SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags(1 << (raceValue)));
        player->SetMasqueradeRace(masqueradeRace);
        handler->PSendSysMessage(LANG_MASQUERADE_RACE_ENABLED, EnumUtils::ToTitle(masqueradeRace));
        return true;
    }
};

void AddSC_EG_commandscript()
{
    new EG_commandscript();
}
