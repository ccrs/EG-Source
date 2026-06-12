#include "CharacterCache.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "GameTime.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "StringFormat.h"
#include "Timer.h"
#include "TournamentMgr.h"
#include "Util.h"

using namespace Trinity::ChatCommands;

class EG_tournament_commandscript : public CommandScript
{
public:
    EG_tournament_commandscript() : CommandScript("EG_tournament_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable tournamentDungeonCommandTable =
        {
            { "set",    HandleDungeonSet,    rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "remove", HandleDungeonRemove, rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
        };

        static ChatCommandTable tournamentMemberCommandTable =
        {
            { "add",    HandleMemberAdd,    rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "remove", HandleMemberRemove, rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
        };

        static ChatCommandTable tournamentTeamCommandTable =
        {
            { "create",     HandleTeamCreate,     rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "delete",     HandleTeamDelete,     rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "info",       HandleTeamInfo,       rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "list",       HandleTeamList,       rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "disqualify", HandleTeamDisqualify, rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "requalify",  HandleTeamRequalify,  rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "member",     tournamentMemberCommandTable },
        };

        static ChatCommandTable tournamentRunCommandTable =
        {
            { "list",   HandleRunList,   rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "live",   HandleRunLive,   rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "reject", HandleRunReject, rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "void",   HandleRunVoid,   rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
        };

        static ChatCommandTable tournamentCommandTable =
        {
            { "create",    HandleCreate,    rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "delete",    HandleDelete,    rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "state",     HandleState,     rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "ilvl",      HandleIlvl,      rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "status",    HandleStatus,    rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "standings", HandleStandings, rbac::RBAC_PERM_COMMAND_TOURNAMENT, Console::Yes },
            { "dungeon",   tournamentDungeonCommandTable },
            { "team",      tournamentTeamCommandTable },
            { "run",       tournamentRunCommandTable },
        };

        static ChatCommandTable commandTable =
        {
            { "tournament", tournamentCommandTable },
        };

        return commandTable;
    }

    static char const* StateName(TournamentState state)
    {
        switch (state)
        {
            case TOURNAMENT_STATE_DRAFT: return "draft";
            case TOURNAMENT_STATE_REGISTRATION: return "registration";
            case TOURNAMENT_STATE_LOCKED: return "locked";
            case TOURNAMENT_STATE_RUNNING: return "running";
            case TOURNAMENT_STATE_ENDED: return "ended";
        }
        return "unknown";
    }

    static char const* RoleName(TournamentRole role)
    {
        switch (role)
        {
            case TOURNAMENT_ROLE_TANK: return "tank";
            case TOURNAMENT_ROLE_HEALER: return "healer";
            case TOURNAMENT_ROLE_DPS: return "dps";
        }
        return "unknown";
    }

    static char const* RunStateName(TournamentRunState state)
    {
        switch (state)
        {
            case TOURNAMENT_RUN_PENDING: return "pending";
            case TOURNAMENT_RUN_ACTIVE: return "active";
            case TOURNAMENT_RUN_COMPLETED: return "completed";
            case TOURNAMENT_RUN_VOID: return "void";
            case TOURNAMENT_RUN_REJECTED: return "rejected";
        }
        return "unknown";
    }

    static std::string MapName(uint16 mapId, ChatHandler* handler)
    {
        if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
            return mapEntry->MapName[handler->GetSessionDbcLocale()];
        return Trinity::StringFormat("<unknown map {}>", mapId);
    }

    static bool ParseDifficulty(std::string_view text, uint8& difficulty)
    {
        if (StringEqualI(text, "normal"))
        {
            difficulty = 0;
            return true;
        }
        if (StringEqualI(text, "heroic"))
        {
            difficulty = 1;
            return true;
        }
        return false;
    }

    // ----- tournament level -----

    static bool HandleCreate(ChatHandler* handler, QuotedString name, Optional<std::string_view> difficultyText)
    {
        uint8 difficulty = 1;
        if (difficultyText && !ParseDifficulty(*difficultyText, difficulty))
        {
            handler->SendSysMessage("Invalid difficulty, use: normal | heroic.");
            return false;
        }

        ObjectGuid::LowType const admin = handler->GetPlayer() ? handler->GetPlayer()->GetGUID().GetCounter() : 0;
        uint32 const id = sTournamentMgr->CreateTournament(name, difficulty, admin);
        handler->PSendSysMessage("Tournament %u '%s' created (%s), state: draft.", id, name.c_str(), difficulty ? "heroic" : "normal");
        return true;
    }

    static bool HandleDelete(ChatHandler* handler, uint32 tournamentId)
    {
        if (!sTournamentMgr->DeleteTournament(tournamentId))
        {
            handler->PSendSysMessage("Tournament %u does not exist.", tournamentId);
            return false;
        }

        handler->PSendSysMessage("Tournament %u deleted (teams and dungeon set included).", tournamentId);
        return true;
    }

    static bool HandleState(ChatHandler* handler, uint32 tournamentId, std::string_view stateText)
    {
        TournamentState state;
        if (StringEqualI(stateText, "draft"))
            state = TOURNAMENT_STATE_DRAFT;
        else if (StringEqualI(stateText, "registration"))
            state = TOURNAMENT_STATE_REGISTRATION;
        else if (StringEqualI(stateText, "locked"))
            state = TOURNAMENT_STATE_LOCKED;
        else if (StringEqualI(stateText, "running"))
            state = TOURNAMENT_STATE_RUNNING;
        else if (StringEqualI(stateText, "ended"))
            state = TOURNAMENT_STATE_ENDED;
        else
        {
            handler->SendSysMessage("Invalid state, use: draft | registration | locked | running | ended.");
            return false;
        }

        if (!sTournamentMgr->SetState(tournamentId, state))
        {
            handler->PSendSysMessage("Could not set tournament %u state (unknown tournament, or another one is already running).", tournamentId);
            return false;
        }

        handler->PSendSysMessage("Tournament %u state set to %s.", tournamentId, StateName(state));
        if (state == TOURNAMENT_STATE_RUNNING)
            handler->PSendSysMessage("Dungeon selection is now revealed.");
        return true;
    }

    static bool HandleIlvl(ChatHandler* handler, uint32 tournamentId, uint16 cap)
    {
        if (!sTournamentMgr->SetIlvlCap(tournamentId, cap))
        {
            handler->PSendSysMessage("Tournament %u does not exist.", tournamentId);
            return false;
        }

        handler->PSendSysMessage("Tournament %u equipped item level cap set to %u.", tournamentId, cap);
        return true;
    }

    static bool HandleStatus(ChatHandler* handler, Optional<uint32> tournamentIdArg)
    {
        TournamentData const* tournament = tournamentIdArg ? sTournamentMgr->GetTournament(*tournamentIdArg) : sTournamentMgr->GetActiveTournament();
        if (!tournament)
        {
            handler->SendSysMessage("No tournament found.");
            return false;
        }

        handler->PSendSysMessage("Tournament %u: '%s' [%s], difficulty: %s, ilvl cap: %u.",
            tournament->id, tournament->name.c_str(), StateName(tournament->state), tournament->difficulty ? "heroic" : "normal", tournament->ilvlCap);
        if (tournament->startTime)
            handler->PSendSysMessage("Started: %s", TimeToTimestampStr(tournament->startTime).c_str());
        if (tournament->endTime)
            handler->PSendSysMessage("Ended: %s", TimeToTimestampStr(tournament->endTime).c_str());

        if (tournament->dungeons.empty())
            handler->SendSysMessage("No dungeons selected.");
        else
            for (uint8 slot = 1; slot <= TOURNAMENT_DUNGEON_NUM; ++slot)
            {
                auto itr = tournament->dungeons.find(slot);
                if (itr == tournament->dungeons.end())
                    continue;

                TournamentDungeon const& dungeon = itr->second;
                handler->PSendSysMessage("Dungeon %u: %s (map %u, %s)%s", slot, MapName(dungeon.mapId, handler).c_str(),
                    dungeon.mapId, dungeon.difficulty ? "heroic" : "normal", dungeon.revealed ? "" : " [hidden]");
            }

        std::vector<TournamentTeam const*> teams = sTournamentMgr->GetTeams(tournament->id);
        handler->PSendSysMessage("%u team(s) registered.", uint32(teams.size()));
        return true;
    }

    static bool HandleStandings(ChatHandler* handler, Optional<uint32> tournamentIdArg)
    {
        TournamentData const* tournament = tournamentIdArg ? sTournamentMgr->GetTournament(*tournamentIdArg) : sTournamentMgr->GetActiveTournament();
        if (!tournament)
        {
            handler->SendSysMessage("No tournament found.");
            return false;
        }

        std::vector<TournamentStanding> standings;
        sTournamentMgr->BuildStandings(tournament->id, standings);
        if (standings.empty())
        {
            handler->PSendSysMessage("Tournament %u has no active teams.", tournament->id);
            return true;
        }

        handler->PSendSysMessage("Standings for tournament %u '%s':", tournament->id, tournament->name.c_str());
        uint32 rank = 0;
        for (TournamentStanding const& standing : standings)
        {
            TournamentTeam const* team = sTournamentMgr->GetTeam(standing.teamId);
            handler->PSendSysMessage("%u. %s - %u point(s), %u/%u dungeons, total %s", ++rank,
                team ? team->name.c_str() : "<deleted>", standing.points, standing.completedSlots,
                TOURNAMENT_DUNGEON_NUM, TournamentMgr::FormatDuration(uint32(standing.totalTimeMs)).c_str());
        }
        return true;
    }

    // ----- dungeon level -----

    static bool HandleDungeonSet(ChatHandler* handler, uint32 tournamentId, uint8 slot, uint16 mapId, Optional<std::string_view> difficultyText)
    {
        TournamentData const* tournament = sTournamentMgr->GetTournament(tournamentId);
        if (!tournament)
        {
            handler->PSendSysMessage("Tournament %u does not exist.", tournamentId);
            return false;
        }

        MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
        if (!mapEntry || !mapEntry->IsNonRaidDungeon())
        {
            handler->PSendSysMessage("Map %u is not a 5-player dungeon.", mapId);
            return false;
        }

        uint8 difficulty = tournament->difficulty;
        if (difficultyText && !ParseDifficulty(*difficultyText, difficulty))
        {
            handler->SendSysMessage("Invalid difficulty, use: normal | heroic.");
            return false;
        }

        if (!sTournamentMgr->SetDungeon(tournamentId, slot, mapId, difficulty))
        {
            handler->PSendSysMessage("Could not set dungeon slot %u (valid slots: 1-%u, no duplicate dungeons).", slot, TOURNAMENT_DUNGEON_NUM);
            return false;
        }

        handler->PSendSysMessage("Tournament %u dungeon %u set to %s (map %u, %s).", tournamentId, slot,
            MapName(mapId, handler).c_str(), mapId, difficulty ? "heroic" : "normal");
        return true;
    }

    static bool HandleDungeonRemove(ChatHandler* handler, uint32 tournamentId, uint8 slot)
    {
        if (!sTournamentMgr->RemoveDungeon(tournamentId, slot))
        {
            handler->PSendSysMessage("Tournament %u has no dungeon in slot %u.", tournamentId, slot);
            return false;
        }

        handler->PSendSysMessage("Tournament %u dungeon slot %u cleared.", tournamentId, slot);
        return true;
    }

    // ----- team level -----

    static bool HandleTeamCreate(ChatHandler* handler, uint32 tournamentId, QuotedString name)
    {
        uint32 const teamId = sTournamentMgr->CreateTeam(tournamentId, name);
        if (!teamId)
        {
            handler->PSendSysMessage("Tournament %u does not exist.", tournamentId);
            return false;
        }

        handler->PSendSysMessage("Team %u '%s' registered to tournament %u.", teamId, name.c_str(), tournamentId);
        return true;
    }

    static bool HandleTeamDelete(ChatHandler* handler, uint32 teamId)
    {
        if (!sTournamentMgr->DeleteTeam(teamId))
        {
            handler->PSendSysMessage("Team %u does not exist.", teamId);
            return false;
        }

        handler->PSendSysMessage("Team %u deleted.", teamId);
        return true;
    }

    static bool HandleTeamInfo(ChatHandler* handler, uint32 teamId)
    {
        TournamentTeam const* team = sTournamentMgr->GetTeam(teamId);
        if (!team)
        {
            handler->PSendSysMessage("Team %u does not exist.", teamId);
            return false;
        }

        handler->PSendSysMessage("Team %u: '%s' (tournament %u), status: %s%s%s.", team->id, team->name.c_str(), team->tournamentId,
            team->status == TOURNAMENT_TEAM_ACTIVE ? "active" : "disqualified",
            team->dqReason.empty() ? "" : ", reason: ", team->dqReason.c_str());

        if (team->members.empty())
        {
            handler->SendSysMessage("No members.");
            return true;
        }

        for (TournamentMember const& member : team->members)
        {
            std::string name;
            sCharacterCache->GetCharacterNameByGuid(ObjectGuid::Create<HighGuid::Player>(member.charGuid), name);
            handler->PSendSysMessage("- %s (guid %u, account %u): %s", name.empty() ? "<unknown>" : name.c_str(),
                member.charGuid, member.accountId, RoleName(member.role));
        }

        if (!team->HasRoleComposition())
            handler->SendSysMessage("WARNING: team does not meet the 1 tank / 1 healer / 3 dps composition yet.");
        return true;
    }

    static bool HandleTeamList(ChatHandler* handler, Optional<uint32> tournamentIdArg)
    {
        TournamentData const* tournament = tournamentIdArg ? sTournamentMgr->GetTournament(*tournamentIdArg) : sTournamentMgr->GetActiveTournament();
        if (!tournament)
        {
            handler->SendSysMessage("No tournament found.");
            return false;
        }

        std::vector<TournamentTeam const*> teams = sTournamentMgr->GetTeams(tournament->id);
        handler->PSendSysMessage("Tournament %u '%s': %u team(s).", tournament->id, tournament->name.c_str(), uint32(teams.size()));
        for (TournamentTeam const* team : teams)
            handler->PSendSysMessage("%u. '%s' - %u member(s), %s", team->id, team->name.c_str(), uint32(team->members.size()),
                team->status == TOURNAMENT_TEAM_ACTIVE ? "active" : "disqualified");
        return true;
    }

    static bool HandleTeamDisqualify(ChatHandler* handler, uint32 teamId, Tail reason)
    {
        if (!sTournamentMgr->DisqualifyTeam(teamId, reason))
        {
            handler->PSendSysMessage("Team %u does not exist.", teamId);
            return false;
        }

        handler->PSendSysMessage("Team %u disqualified: %s", teamId, std::string(reason).c_str());
        return true;
    }

    static bool HandleTeamRequalify(ChatHandler* handler, uint32 teamId)
    {
        if (!sTournamentMgr->RequalifyTeam(teamId))
        {
            handler->PSendSysMessage("Team %u does not exist.", teamId);
            return false;
        }

        handler->PSendSysMessage("Team %u requalified.", teamId);
        return true;
    }

    // ----- member level -----

    static bool HandleMemberAdd(ChatHandler* handler, uint32 teamId, PlayerIdentifier player, std::string_view roleText)
    {
        TournamentRole role;
        if (StringEqualI(roleText, "tank"))
            role = TOURNAMENT_ROLE_TANK;
        else if (StringEqualI(roleText, "healer"))
            role = TOURNAMENT_ROLE_HEALER;
        else if (StringEqualI(roleText, "dps"))
            role = TOURNAMENT_ROLE_DPS;
        else
        {
            handler->SendSysMessage("Invalid role, use: tank | healer | dps.");
            return false;
        }

        TournamentTeam const* team = sTournamentMgr->GetTeam(teamId);
        if (!team)
        {
            handler->PSendSysMessage("Team %u does not exist.", teamId);
            return false;
        }

        ObjectGuid::LowType const charGuid = player.GetGUID().GetCounter();
        if (TournamentTeam const* existing = sTournamentMgr->GetTeamByMember(charGuid, team->tournamentId))
        {
            handler->PSendSysMessage("%s already belongs to team %u '%s' of this tournament.", player.GetName().c_str(), existing->id, existing->name.c_str());
            return false;
        }

        uint32 const accountId = sCharacterCache->GetCharacterAccountIdByGuid(player.GetGUID());
        if (!sTournamentMgr->AddMember(teamId, charGuid, accountId, role))
        {
            handler->PSendSysMessage("Could not add %s to team %u (team full or character already registered).", player.GetName().c_str(), teamId);
            return false;
        }

        handler->PSendSysMessage("%s added to team %u as %s.", player.GetName().c_str(), teamId, RoleName(role));
        return true;
    }

    static bool HandleMemberRemove(ChatHandler* handler, uint32 teamId, PlayerIdentifier player)
    {
        if (!sTournamentMgr->RemoveMember(teamId, player.GetGUID().GetCounter()))
        {
            handler->PSendSysMessage("%s is not a member of team %u.", player.GetName().c_str(), teamId);
            return false;
        }

        handler->PSendSysMessage("%s removed from team %u.", player.GetName().c_str(), teamId);
        return true;
    }

    // ----- run level -----

    static bool HandleRunLive(ChatHandler* handler)
    {
        std::vector<TournamentRun> const runs = sTournamentMgr->GetLiveRuns();
        if (runs.empty())
        {
            handler->SendSysMessage("No live tournament runs.");
            return true;
        }

        for (TournamentRun const& run : runs)
        {
            TournamentTeam const* team = sTournamentMgr->GetTeam(run.teamId);
            std::string const elapsed = run.state == TOURNAMENT_RUN_ACTIVE
                ? TournamentMgr::FormatDuration(getMSTimeDiff(run.combatStartMSTime, GameTime::GetGameTimeMS()))
                : "not started";
            handler->PSendSysMessage("Run %u: team '%s', dungeon %u (map %u, instance %u), %s, elapsed: %s", run.id,
                team ? team->name.c_str() : "<deleted>", run.dungeonSlot, run.mapId, run.instanceId, RunStateName(run.state), elapsed.c_str());
        }

        handler->PSendSysMessage("%u live run(s).", uint32(runs.size()));
        return true;
    }

    static bool HandleRunList(ChatHandler* handler, uint32 teamId)
    {
        TournamentTeam const* team = sTournamentMgr->GetTeam(teamId);
        if (!team)
        {
            handler->PSendSysMessage("Team %u does not exist.", teamId);
            return false;
        }

        uint32 count = 0;
        if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_TOURNAMENT_RUN_ALL)))
        {
            do
            {
                Field* fields = result->Fetch();
                if (fields[1].GetUInt32() != teamId)
                    continue;

                ++count;
                TournamentRunState state = TournamentRunState(fields[5].GetUInt8());
                std::string line = Trinity::StringFormat("Run {}: dungeon {} (map {}), {}", fields[0].GetUInt32(),
                    fields[2].GetUInt8(), fields[3].GetUInt16(), RunStateName(state));
                if (state == TOURNAMENT_RUN_COMPLETED)
                    line += Trinity::StringFormat(", time {}", TournamentMgr::FormatDuration(fields[8].GetUInt32()));
                std::string reason = fields[9].GetString();
                if (!reason.empty())
                    line += Trinity::StringFormat(", reason: {}", reason);
                handler->SendSysMessage(line.c_str());
            } while (result->NextRow());
        }

        handler->PSendSysMessage("%u run(s) for team %u '%s'.", count, teamId, team->name.c_str());
        return true;
    }

    static bool HandleRunReject(ChatHandler* handler, uint32 runId, Tail reason)
    {
        ObjectGuid::LowType const staff = handler->GetPlayer() ? handler->GetPlayer()->GetGUID().GetCounter() : 0;
        if (!sTournamentMgr->SetRunVerdict(runId, TOURNAMENT_RUN_REJECTED, reason, staff))
        {
            handler->PSendSysMessage("Could not reject run %u.", runId);
            return false;
        }

        handler->PSendSysMessage("Run %u rejected: %s", runId, std::string(reason).c_str());
        return true;
    }

    static bool HandleRunVoid(ChatHandler* handler, uint32 runId, Tail reason)
    {
        ObjectGuid::LowType const staff = handler->GetPlayer() ? handler->GetPlayer()->GetGUID().GetCounter() : 0;
        if (!sTournamentMgr->SetRunVerdict(runId, TOURNAMENT_RUN_VOID, reason, staff))
        {
            handler->PSendSysMessage("Could not void run %u.", runId);
            return false;
        }

        handler->PSendSysMessage("Run %u voided: %s", runId, std::string(reason).c_str());
        return true;
    }
};

void AddSC_EG_tournament_commandscript()
{
    new EG_tournament_commandscript();
}
