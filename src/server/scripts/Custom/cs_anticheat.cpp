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

#include "AnticheatMgr.h"
#include "AccountMgr.h"
#include "GameTime.h"
#include "CharacterCache.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Realm.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "World.h"
#include "WorldSession.h"

using namespace Trinity::ChatCommands;

enum AnticheatSpells
{
    SHACKLES                    = 38505,
    LFG_SPELL_DUNGEON_DESERTER  = 71041,
    BG_SPELL_DESERTER           = 26013,
    SILENCED                    = 23207
};

class anticheat_commandscript : public CommandScript
{
public:
    anticheat_commandscript() : CommandScript("anticheat_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable anticheatCommandTable =
        {
            { "global", HandleAntiCheatGlobalCommand, rbac::RBAC_ROLE_GAMEMASTER,    Console::Yes },
            { "player", HandleAntiCheatPlayerCommand, rbac::RBAC_ROLE_GAMEMASTER,    Console::Yes },
            { "delete", HandleAntiCheatDeleteCommand, rbac::RBAC_ROLE_ADMINISTRATOR, Console::Yes },
            { "purge",  HandleAntiCheatPurgeCommand,  rbac::RBAC_ROLE_ADMINISTRATOR, Console::Yes },
            { "handle", HandleAntiCheatHandleCommand, rbac::RBAC_ROLE_ADMINISTRATOR, Console::Yes },
            { "jail",   HandleAnticheatJailCommand,   rbac::RBAC_ROLE_GAMEMASTER,    Console::Yes },
            { "parole", HandleAnticheatParoleCommand, rbac::RBAC_ROLE_ADMINISTRATOR, Console::Yes },
            { "warn",   HandleAnticheatWarnCommand,   rbac::RBAC_ROLE_GAMEMASTER,    Console::Yes }
        };

        static ChatCommandTable commandTable =
        {
            { "anticheat", anticheatCommandTable },
        };

        return commandTable;
    }

    static bool HandleAnticheatWarnCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ChatHandler(player->GetConnectedPlayer()->GetSession()).SendSysMessage("The anticheat system has reported several times that you may be cheating. You will be monitored to confirm if this is accurate.");
        return true;
    }

    static bool HandleAnticheatJailCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = player->GetConnectedPlayer();

        WorldLocation jailLoc(1, 16226.5f, 16403.6f, -64.5f, 3.2f);

        if (!handler->IsConsole())
            handler->GetSession()->GetPlayer()->TeleportTo(jailLoc);

        target->TeleportTo(jailLoc);
        target->SetHomebind(jailLoc, 876);
        target->CastSpell(target, SHACKLES);

        if (Aura* aura = target->AddAura(LFG_SPELL_DUNGEON_DESERTER, target))
            aura->SetDuration(-1);
        if (Aura* aura = target->AddAura(BG_SPELL_DESERTER, target))
            aura->SetDuration(-1);
        if (Aura* aura = target->AddAura(SILENCED, target))
            aura->SetDuration(-1);

        return true;
    }

    static bool HandleAnticheatParoleCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = player->GetConnectedPlayer();

        WorldLocation allianceLoc(0, -8833.37f, 628.62f, 94.00f, 1.06f);
        WorldLocation hordeLoc(1, 1569.59f, -4397.63f, 16.06f, 0.54f);

        if (target->GetTeamId() == TEAM_ALLIANCE)
        {
            target->TeleportTo(allianceLoc);
            target->SetHomebind(allianceLoc, 1519);
        }
        else
        {
            target->TeleportTo(hordeLoc);
            target->SetHomebind(hordeLoc, 1653);
        }

        target->RemoveAura(SHACKLES);
        target->RemoveAura(LFG_SPELL_DUNGEON_DESERTER);
        target->RemoveAura(BG_SPELL_DESERTER);
        target->RemoveAura(SILENCED);
        sAnticheatMgr->AnticheatDeleteCommand(target->GetGUID().GetCounter());
        return true;
    }

    static bool HandleAntiCheatDeleteCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        sAnticheatMgr->AnticheatDeleteCommand(player->GetGUID().GetCounter());
        handler->PSendSysMessage("Anticheat data deleted for player %s", player->GetName().c_str());
        return true;
    }

    static bool HandleAntiCheatPurgeCommand(ChatHandler* handler)
    {
        sAnticheatMgr->AnticheatPurgeCommand();
        handler->PSendSysMessage("The Anticheat data has been purged.");
        return true;
    }

    static bool HandleAntiCheatPlayerCommand(ChatHandler* handler, Optional<PlayerIdentifier> player)
    {
        if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
            return false;

        if (!player)
            player = PlayerIdentifier::FromTarget(handler);
        if (!player || !player->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* target = player->GetConnectedPlayer();
        uint32 guid = player->GetGUID().GetCounter();
        uint32 accountId = target->GetSession()->GetAccountId();
        uint32 latency = target->GetSession()->GetLatency();

        uint32 total_reports = sAnticheatMgr->GetTotalReports(guid);
        uint32 elapsedSecs = sAnticheatMgr->GetElapsedSeconds(guid);
        float ratePerMin = (elapsedSecs > 0) ? (float(total_reports) / float(elapsedSecs)) * 60.0f : 0.0f;

        std::string windowStr;
        if (elapsedSecs >= 3600)
            windowStr = Trinity::StringFormat("{}h {:02}m", elapsedSecs / 3600, (elapsedSecs % 3600) / 60);
        else if (elapsedSecs >= 60)
            windowStr = Trinity::StringFormat("{}m {:02}s", elapsedSecs / 60, elapsedSecs % 60);
        else if (elapsedSecs > 0)
            windowStr = Trinity::StringFormat("{}s", elapsedSecs);
        else
            windowStr = "N/A";

        uint32 speed_reports = sAnticheatMgr->GetTypeReports(guid, SPEED_HACK_REPORT);
        uint32 fly_reports = sAnticheatMgr->GetTypeReports(guid, FLY_HACK_REPORT);
        uint32 jump_reports = sAnticheatMgr->GetTypeReports(guid, JUMP_HACK_REPORT);
        uint32 waterwalk_reports = sAnticheatMgr->GetTypeReports(guid, WALK_WATER_HACK_REPORT);
        uint32 teleportplane_reports = sAnticheatMgr->GetTypeReports(guid, TELEPORT_PLANE_HACK_REPORT);
        uint32 climb_reports = sAnticheatMgr->GetTypeReports(guid, CLIMB_HACK_REPORT);
        uint32 teleport_reports = sAnticheatMgr->GetTypeReports(guid, TELEPORT_HACK_REPORT);
        uint32 ignorecontrol_reports = sAnticheatMgr->GetTypeReports(guid, IGNORE_CONTROL_REPORT);
        uint32 zaxis_reports = sAnticheatMgr->GetTypeReports(guid, ZAXIS_HACK_REPORT);
        uint32 antiswim_reports = sAnticheatMgr->GetTypeReports(guid, ANTISWIM_HACK_REPORT);
        uint32 gravity_reports = sAnticheatMgr->GetTypeReports(guid, GRAVITY_HACK_REPORT);
        uint32 antiknockback_reports = sAnticheatMgr->GetTypeReports(guid, ANTIKNOCK_BACK_HACK_REPORT);
        uint32 no_fall_damage_reports = sAnticheatMgr->GetTypeReports(guid, NO_FALL_DAMAGE_HACK_REPORT);
        uint32 counter_measures_reports = sAnticheatMgr->GetTypeReports(guid, COUNTER_MEASURES_REPORT);

        bool luaCheater = sAnticheatMgr->CheckIsLuaCheater(accountId);

        auto adbStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ANTICHEAT_PLAYER_ACCOUNT_BANS);
        adbStmt->setUInt32(0, accountId);
        PreparedQueryResult resultADB = LoginDatabase.Query(adbStmt);

        auto cdbStmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ANTICHEAT_PLAYER_CHARACTER_BANS);
        cdbStmt->setUInt32(0, guid);
        PreparedQueryResult resultCDB = CharacterDatabase.Query(cdbStmt);

        if (!handler->IsConsole())
        {
            handler->PSendSysMessage("|cFFFFA500-----------------------------------------------------------------");
            handler->PSendSysMessage("|cFF20B2AAPlayer: |cffffff00%s |cFF20B2AAAccount: |cffffff00%s", player->GetName().c_str(), target->GetSession()->GetAccountName().c_str());
            handler->PSendSysMessage("|cffff0000IP Address: |cffffff00%s |cffff0000Latency |cffffff00%u ms", target->GetSession()->GetRemoteAddress().c_str(), latency);

            if (resultADB)
            {
                do
                {
                    Field* fields = resultADB->Fetch();
                    handler->PSendSysMessage("|cffff0000Account Previously Banned: |cffffff00Yes");
                    handler->PSendSysMessage("|cffff0000Ban Ended: |cffffff00%s", TimeToTimestampStr(fields[3].GetUInt64()).c_str());
                    handler->PSendSysMessage("|cffff0000Ban by: |cffffff00%s |cffff0000Ban Reason: |cffffff00%s", fields[5].GetString().c_str(), fields[4].GetString().c_str());
                } while (resultADB->NextRow());
            }
            else
                handler->PSendSysMessage("|cffff0000Account Previously Banned: |cffffff00No");

            if (resultCDB)
            {
                do
                {
                    Field* fields = resultCDB->Fetch();
                    handler->PSendSysMessage("|cffff0000Character Previously Banned: |cffffff00Yes");
                    handler->PSendSysMessage("|cffff0000Ban Ended: |cffffff00%s", TimeToTimestampStr(fields[3].GetUInt64()).c_str());
                    handler->PSendSysMessage("|cffff0000Ban by: |cffffff00%s |cffff0000Ban Reason: |cffffff00%s", fields[5].GetString().c_str(), fields[4].GetString().c_str());
                } while (resultCDB->NextRow());
            }
            else
                handler->PSendSysMessage("|cffff0000Character Previously Banned: |cffffff00No");

            handler->PSendSysMessage("|cffff0000Lua Cheater Flagged: |cffffff00%s", luaCheater ? "Yes" : "No");
            handler->PSendSysMessage("|cffff0000Counter Measures Deployed: |cffffff00%u", counter_measures_reports);
            handler->PSendSysMessage("|cffff0000Total Reports: |cffffff00%u |cffff0000Window: |cffffff00%s |cffff0000Rate: |cffffff00%.2f/min", total_reports, windowStr.c_str(), ratePerMin);
            handler->PSendSysMessage("|cffff0000Speed Reports: |cffffff00%u |cffff0000Fly Reports: |cffffff00%u |cffff0000Jump Reports: |cffffff00%u", speed_reports, fly_reports, jump_reports);
            handler->PSendSysMessage("|cffff0000Walk On Water Reports: |cffffff00%u |cffff0000Teleport To Plane Reports: |cffffff00%u", waterwalk_reports, teleportplane_reports);
            handler->PSendSysMessage("|cffff0000Teleport Reports: |cffffff00%u |cffff0000Climb Reports: |cffffff00%u", teleport_reports, climb_reports);
            handler->PSendSysMessage("|cffff0000Ignore Control Reports: |cffffff00%u |cffff0000Ignore Z-Axis Reports: |cffffff00%u", ignorecontrol_reports, zaxis_reports);
            handler->PSendSysMessage("|cffff0000Ignore Anti-Swim Reports: |cffffff00%u |cffff0000Gravity Reports: |cffffff00%u", antiswim_reports, gravity_reports);
            handler->PSendSysMessage("|cffff0000Anti-Knock Back Reports: |cffffff00%u |cffff0000No Fall Damage Reports: |cffffff00%u", antiknockback_reports, no_fall_damage_reports);
        }
        else
        {
            handler->PSendSysMessage("-----------------------------------------------------------------");
            handler->PSendSysMessage("Player: %s || Account: %s", player->GetName().c_str(), target->GetSession()->GetAccountName().c_str());
            handler->PSendSysMessage("IP Address: %s || Latency %u ms", target->GetSession()->GetRemoteAddress().c_str(), latency);

            if (resultADB)
            {
                do
                {
                    Field* fields = resultADB->Fetch();
                    handler->PSendSysMessage("Account Previously Banned: Yes");
                    handler->PSendSysMessage("Ban Ended: %s", TimeToTimestampStr(fields[3].GetUInt64()).c_str());
                    handler->PSendSysMessage("Ban by: %s || Ban Reason: %s", fields[5].GetString().c_str(), fields[4].GetString().c_str());
                } while (resultADB->NextRow());
            }
            else
                handler->PSendSysMessage("Account Previously Banned: No");

            if (resultCDB)
            {
                do
                {
                    Field* fields = resultCDB->Fetch();
                    handler->PSendSysMessage("Character Previously Banned: Yes");
                    handler->PSendSysMessage("Ban Ended: %s", TimeToTimestampStr(fields[3].GetUInt64()).c_str());
                    handler->PSendSysMessage("Ban by: %s || Ban Reason: %s", fields[5].GetString().c_str(), fields[4].GetString().c_str());
                } while (resultCDB->NextRow());
            }
            else
                handler->PSendSysMessage("Character Previously Banned: No");

            handler->PSendSysMessage("Lua Cheater Flagged: %s", luaCheater ? "Yes" : "No");
            handler->PSendSysMessage("Counter Measures Deployed: %u", counter_measures_reports);
            handler->PSendSysMessage("Total Reports: %u || Window: %s || Rate: %.2f/min", total_reports, windowStr.c_str(), ratePerMin);
            handler->PSendSysMessage("Speed Reports: %u || Fly Reports: %u || Jump Reports: %u", speed_reports, fly_reports, jump_reports);
            handler->PSendSysMessage("Walk On Water Reports: %u || Teleport To Plane Reports: %u", waterwalk_reports, teleportplane_reports);
            handler->PSendSysMessage("Teleport Reports: %u || Climb Reports: %u", teleport_reports, climb_reports);
            handler->PSendSysMessage("Ignore Control Reports: %u || Ignore Z-Axis Reports: %u", ignorecontrol_reports, zaxis_reports);
            handler->PSendSysMessage("Ignore Anti-Swim Reports: %u || Gravity Reports: %u", antiswim_reports, gravity_reports);
            handler->PSendSysMessage("Anti-Knock Back Reports: %u || No Fall Damage Reports: %u", antiknockback_reports, no_fall_damage_reports);
        }

        return true;
    }

    static bool HandleAntiCheatHandleCommand(ChatHandler* handler, bool enable)
    {
        sWorld->setBoolConfig(CONFIG_ANTICHEAT_ENABLE, enable);
        handler->PSendSysMessage("The Anticheat System is now: %s", enable ? "Enabled!" : "Disabled!");
        return true;
    }

    static bool HandleAntiCheatGlobalCommand(ChatHandler* handler, Optional<uint32> days)
    {
        if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        {
            handler->PSendSysMessage("The Anticheat System is disabled.");
            return true;
        }

        for (auto const& [_, session] : sWorld->GetAllSessions())
            if (Player* player = session->GetPlayer())
                sAnticheatMgr->SavePlayerData(player);

        uint32 daysBack = days.value_or(30);
        uint32 cutoffTime = uint32((GameTime::GetGameTime() / DAY - time_t(daysBack)) * DAY);

        auto printRow = [handler](Field* fields)
        {
            uint32 accountId = fields[0].GetUInt32();
            uint32 lowGuid = fields[1].GetUInt32();
            float average = fields[2].GetFloat();
            uint32 totalReports = fields[3].GetUInt32();
            std::string lastSeen = TimeToTimestampStr(fields[4].GetUInt64());
            uint32 daysFlagged = fields[6].GetUInt32();

            if (Player* player = ObjectAccessor::FindPlayerByLowGUID(lowGuid))
                handler->PSendSysMessage("Account: %s | Player: %s [ONLINE] | Avg: %.2f | Total: %u | Last: %s (%u day(s) flagged)", player->GetSession()->GetAccountName().c_str(), player->GetName().c_str(), average, totalReports, lastSeen.c_str(), daysFlagged);
            else
            {
                std::string accountName, charName;
                AccountMgr::GetName(accountId, accountName);
                if (!sCharacterCache->GetCharacterNameByGuid(ObjectGuid::Create<HighGuid::Player>(lowGuid), charName))
                    charName = Trinity::StringFormat("(GUID: {})", lowGuid);
                handler->PSendSysMessage("Account: %s | Player: %s | Avg: %.2f | Total: %u | Last: %s (%u day(s) flagged)", accountName.c_str(), charName.c_str(), average, totalReports, lastSeen.c_str(), daysFlagged);
            }
        };

        auto avgStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ANTICHEAT_REPORTS_BY_AVERAGE);
        avgStmt->setUInt32(0, realm.Id.Realm);
        avgStmt->setUInt32(1, cutoffTime);
        if (PreparedQueryResult result = LoginDatabase.Query(avgStmt))
        {
            handler->PSendSysMessage("============================= (last %u days)", daysBack);
            handler->PSendSysMessage("Players with the highest report rate:");
            do
                printRow(result->Fetch());
            while (result->NextRow());
        }

        auto totStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ANTICHEAT_REPORTS_BY_TOTAL);
        totStmt->setUInt32(0, realm.Id.Realm);
        totStmt->setUInt32(1, cutoffTime);
        if (PreparedQueryResult result = LoginDatabase.Query(totStmt))
        {
            handler->PSendSysMessage("============================= (last %u days)", daysBack);
            handler->PSendSysMessage("Players with the most total reports:");
            do
                printRow(result->Fetch());
            while (result->NextRow());
        }

        return true;
    }
};

void AddSC_anticheat_commandscript()
{
    new anticheat_commandscript();
}
