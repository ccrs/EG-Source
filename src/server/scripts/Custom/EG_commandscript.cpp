#include "Chat.h"
#include "CustomFunctions.h"
#include "ObjectMgr.h"
#include "Pet.h"
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
            { "mount",      HandleAccountMount,      rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "riding",     HandleAccountRiding,     rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "pet",        HandleAccountPet,        rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
            { "flightPath", HandleAccountFlightPath, rbac::RBAC_PERM_COMMAND_CUSTOM_CHARACTER_SETTINGS, Console::No },
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

    static bool HandleAccountPet(ChatHandler* handler, bool active)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

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

        if (value > 12)
        {
            handler->SendSysMessage("Please use a value between 0 and 12.\nDisconnect and reconnect to see this setting applied.");
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

        uint32 playerTeam = Player::TeamForRace(player->GetRace());
        if (Player::TeamForRace(raceValue) != playerTeam)
        {
            std::string racesForTeam;
            if (playerTeam == ALLIANCE)
                racesForTeam = "0 - HIDE all\n1 - Human\n3 - Dwarf\n4 - Nightelf\n7 - Gnome\n10 - Dranei\n11 - Reset";
            else
                racesForTeam = "0 - HIDE all\n2 - Orc\n5 - Undead\n6 - Tauren\n8 - Troll\n9 - Bloodelf\n11 - Reset";
            handler->SendSysMessage("Please select a race from your current Faction.\nOptions:\n" + racesForTeam);
            return true;
        }

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
            default:
                break;
        }

        if (flag != CustomFlags::CUSTOM_FLAG_NONE)
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
        return true;
    }
};

void AddSC_EG_commandscript()
{
    new EG_commandscript();
}
