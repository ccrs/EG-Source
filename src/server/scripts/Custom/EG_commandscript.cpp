#include "Chat.h"
#include "ObjectMgr.h"
#include "ItemTemplate.h"
#include "Item.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Language.h"
#include "WorldSession.h"

class EG_commandscript : public CommandScript
{
public:
    EG_commandscript() : CommandScript("EG_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> customCharacterSettings =
        {
            { "transmogrification", rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, false, &HandleDisableTransmogrification,          "" },
            { "legendary",          rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, false, &HandleDisableLegendaryTransmogrification, "" },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "settings",           rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, false, nullptr,                                   "", customCharacterSettings },
        };

        return commandTable;
    }

    static bool HandleDisableTransmogrification(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        if (!handler->GetSession())
            return false;

        char* param1 = strtok(const_cast<char*>(args), " ");
        if (!param1)
        {
            handler->SendSysMessage("Wrong command parameter, use on/off.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        std::string command = param1;
        if (command == "on")
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Showing transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
        else if (command == "off")
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Hiding transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
        else
        {
            handler->SendSysMessage("Wrong command parameter, use on/off.");
            handler->SetSentErrorMessage(true);
            return false;
        }
    }

    static bool HandleDisableLegendaryTransmogrification(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        if (!handler->GetSession())
            return false;

        char* param1 = strtok(const_cast<char*>(args), " ");
        if (!param1)
        {
            handler->SendSysMessage("Wrong command parameter, use on/off.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        std::string command = param1;
        if (command == "on")
        {
            player->RemoveCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Showing legendary transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
        else if (command == "off")
        {
            player->AddCustomFlag(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY);
            player->UpdateObjectVisibility();
            handler->SendSysMessage("Hiding legendary transmoged items, disconnect and reconnect to see the applied changes.");
            return true;
        }
        else
        {
            handler->SendSysMessage("Wrong command parameter, use on/off.");
            handler->SetSentErrorMessage(true);
            return false;
        }
    }
};

void AddSC_EG_commandscript()
{
    new EG_commandscript();
}
