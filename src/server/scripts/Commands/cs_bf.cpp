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

#include "ScriptMgr.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Chat.h"
#include "Group.h"
#include "GroupMgr.h"
#include "RBAC.h"

using namespace Trinity::ChatCommands;

class bf_commandscript : public CommandScript
{
public:
    bf_commandscript() : CommandScript("bf_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable battlefieldRaidCommandTable =
        {
            { "release", HandleBattlefieldRaidRelease, rbac::RBAC_PERM_COMMAND_BF_RAID, Console::No },
            { "add", HandleBattlefieldRaidAdd, rbac::RBAC_PERM_COMMAND_BF_RAID, Console::No },
            { "remove", HandleBattlefieldRaidRemove, rbac::RBAC_PERM_COMMAND_BF_RAID, Console::No },
            { "list", HandleBattlefieldRaidList, rbac::RBAC_PERM_COMMAND_BF_RAID, Console::No },
        };

        static ChatCommandTable battlefieldcommandTable =
        {
            { "start",  HandleBattlefieldStart,  rbac::RBAC_PERM_COMMAND_BF_START,  Console::No },
            { "stop",   HandleBattlefieldEnd,    rbac::RBAC_PERM_COMMAND_BF_STOP,   Console::No },
            { "switch", HandleBattlefieldSwitch, rbac::RBAC_PERM_COMMAND_BF_SWITCH, Console::No },
            { "timer",  HandleBattlefieldTimer,  rbac::RBAC_PERM_COMMAND_BF_TIMER,  Console::No },
            { "enable", HandleBattlefieldEnable, rbac::RBAC_PERM_COMMAND_BF_ENABLE, Console::No },
            { "raid", battlefieldRaidCommandTable },
        };

        static ChatCommandTable commandTable =
        {
            { "bf", battlefieldcommandTable },
        };
        return commandTable;
    }

    static bool HandleBattlefieldStart(ChatHandler* /*handler*/)
    {
        return true;
    }

    static bool HandleBattlefieldEnd(ChatHandler* /*handler*/)
    {
        return true;
    }

    static bool HandleBattlefieldEnable(ChatHandler* /*handler*/)
    {
        return true;
    }

    static bool HandleBattlefieldSwitch(ChatHandler* handler, uint8 battleId, Optional<uint8> newControllingTeam)
    {
        if (battleId == 0 || battleId >= BattlefieldBattleId::BATTLEFIELD_BATTLEID_MAX)
            return false;
        Battlefield* battlefield = sBattlefieldMgr->GetBattlefield(BattlefieldBattleId(battleId));
        if (!battlefield)
            return false;

        if (newControllingTeam)
        {
            PvPTeamId teamId = PvPTeamId(*newControllingTeam);
            if (teamId != PVP_TEAM_ALLIANCE && teamId != PVP_TEAM_HORDE)
                return false;

            battlefield->ChangeTeams(teamId);
        }
        else
            battlefield->ChangeTeams(battlefield->GetControllingTeam() == PVP_TEAM_ALLIANCE ? PVP_TEAM_HORDE : PVP_TEAM_ALLIANCE);
        handler->PSendSysMessage("Battlefield: %u switched to team %u", battleId, battlefield->GetControllingTeam());
        return true;
    }

    static bool HandleBattlefieldTimer(ChatHandler* /*handler*/)
    {
        return true;
    }

    static Battlefield* GetBattlefieldById(uint8 battleId)
    {
        if (battleId == 0 || battleId >= BattlefieldBattleId::BATTLEFIELD_BATTLEID_MAX)
            return nullptr;
        return sBattlefieldMgr->GetBattlefield(BattlefieldBattleId(battleId));
    }

    static bool HandleBattlefieldRaidRelease(ChatHandler* handler, uint8 battleId)
    {
        Battlefield* battlefield = GetBattlefieldById(battleId);
        if (!battlefield)
            return false;

        battlefield->QueueEvent<BattlefieldRaidReleaseEvent>();
        handler->PSendSysMessage("Battlefield %u: battlefield raid release queued.", battleId);
        return true;
    }

    static bool HandleBattlefieldRaidAdd(ChatHandler* handler, uint8 battleId, Optional<PlayerIdentifier> player)
    {
        Battlefield* battlefield = GetBattlefieldById(battleId);
        if (!battlefield)
            return false;

        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);
        if (!player)
            return false;

        if (!player->IsConnected())
        {
            handler->PSendSysMessage("Battlefield %u: %s is not online.", battleId, player->GetName().c_str());
            handler->SetSentErrorMessage(true);
            return false;
        }

        battlefield->QueueEvent<BattlefieldRaidAddEvent>(player->GetGUID());
        handler->PSendSysMessage("Battlefield %u: %s battlefield raid add queued.", battleId, player->GetName().c_str());
        return true;
    }

    static bool HandleBattlefieldRaidRemove(ChatHandler* handler, uint8 battleId, Optional<PlayerIdentifier> player)
    {
        Battlefield* battlefield = GetBattlefieldById(battleId);
        if (!battlefield)
            return false;

        if (!player)
            player = PlayerIdentifier::FromTargetOrSelf(handler);
        if (!player)
            return false;

        battlefield->QueueEvent<BattlefieldRaidRemoveEvent>(player->GetGUID());
        handler->PSendSysMessage("Battlefield %u: %s battlefield raid remove queued.", battleId, player->GetName().c_str());
        return true;
    }

    static bool HandleBattlefieldRaidList(ChatHandler* handler, uint8 battleId)
    {
        Battlefield* battlefield = GetBattlefieldById(battleId);
        if (!battlefield)
            return false;

        for (TeamId teamId : { TEAM_ALLIANCE, TEAM_HORDE })
            for (ObjectGuid groupGuid : battlefield->GetBattlefieldRaids(teamId))
                if (Group* group = sGroupMgr->GetGroupByGUID(groupGuid))
                    handler->PSendSysMessage("%s raid %u: %u members, leader %s", teamId == TEAM_ALLIANCE ? "Alliance" : "Horde", groupGuid.GetCounter(), group->GetMembersCount(), group->GetLeaderName());
        return true;
    }
};

void AddSC_bf_commandscript()
{
    new bf_commandscript();
}
