#include "ScriptMgr.h"
#include "Chat.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
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

        static ChatCommandTable customCharacterSettings =
        {
            { "transmogrification", transmogrificationSettings }, 
            { "aoeloot",            HandleAOELoot, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "worldChat",          HandleWorldChat, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
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

    static bool HandleWorldChat(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (active)
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_WORLDCHAT_FLAGS, CustomFlags::CUSTOM_FLAG_WORLDCHAT_ACTIVE);
            handler->SendSysMessage("LFG World Chat activated.");
            if (ChannelMgr* cMgr = ChannelMgr::forTeam(Team::ALLIANCE))
            {
                if (Channel* channel = cMgr->GetCustomChannel("world"))
                    channel->JoinChannel(player);
                else if (Channel* channel = cMgr->CreateCustomChannel("world"))
                    channel->JoinChannel(player);
            }
            return true;
        }
        else
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_WORLDCHAT_FLAGS, CustomFlags::CUSTOM_FLAG_WORLDCHAT_ACTIVE);
            handler->SendSysMessage("LFG World Chat deactivated.");
            if (ChannelMgr* cMgr = ChannelMgr::forTeam(Team::ALLIANCE))
            {
                if (Channel* channel = cMgr->GetCustomChannel("world"))
                    channel->Kick(player, player->GetName());
            }
            return true;
        }
    }
};

void AddSC_EG_commandscript()
{
    new EG_commandscript();
}
