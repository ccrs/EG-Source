#include "Chat.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Language.h"
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
};

void AddSC_EG_commandscript()
{
    new EG_commandscript();
}
