#include "TournamentMgr.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Item.h"
#include "Log.h"
#include "Map.h"
#include "MapUtils.h"
#include "Player.h"
#include "StringFormat.h"
#include "Timer.h"
#include <algorithm>
#include <utility>

TournamentMember const* TournamentTeam::GetMember(ObjectGuid::LowType guid) const
{
    for (TournamentMember const& member : members)
        if (member.charGuid == guid)
            return &member;
    return nullptr;
}

bool TournamentTeam::HasRoleComposition() const
{
    if (members.size() != TOURNAMENT_TEAM_SIZE)
        return false;

    uint8 tanks = 0;
    uint8 healers = 0;
    uint8 dps = 0;
    for (TournamentMember const& member : members)
    {
        switch (member.role)
        {
            case TOURNAMENT_ROLE_TANK:
                ++tanks;
                break;
            case TOURNAMENT_ROLE_HEALER:
                ++healers;
                break;
            case TOURNAMENT_ROLE_DPS:
                ++dps;
                break;
        }
    }

    return tanks == 1 && healers == 1 && dps == 3;
}

TournamentDungeon const* TournamentData::GetDungeonByMap(uint16 mapId, uint8 difficulty) const
{
    for (auto const& pair : dungeons)
        if (pair.second.mapId == mapId && pair.second.difficulty == difficulty)
            return &pair.second;
    return nullptr;
}

TournamentMgr* TournamentMgr::instance()
{
    static TournamentMgr inst;
    return &inst;
}

void TournamentMgr::LoadFromDB()
{
    uint32 const oldMSTime = getMSTime();

    _tournaments.clear();
    _teams.clear();
    _memberIndex.clear();
    _runsByInstance.clear();
    _nextTournamentId = 1;
    _nextTeamId = 1;
    _nextRunId = 1;

    // tournaments
    if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_TOURNAMENT_ALL)))
    {
        do
        {
            Field* fields = result->Fetch();
            TournamentData data;
            data.id = fields[0].GetUInt32();
            data.name = fields[1].GetString();
            data.state = TournamentState(fields[2].GetUInt8());
            data.difficulty = fields[3].GetUInt8();
            data.ilvlCap = fields[4].GetUInt16();
            data.startTime = fields[5].GetUInt32();
            data.endTime = fields[6].GetUInt32();
            data.createdBy = fields[7].GetUInt32();
            _nextTournamentId = std::max(_nextTournamentId, data.id + 1);
            _tournaments[data.id] = std::move(data);
        } while (result->NextRow());
    }

    // dungeon set
    if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_TOURNAMENT_DUNGEON_ALL)))
    {
        do
        {
            Field* fields = result->Fetch();
            TournamentData* tournament = Trinity::Containers::MapGetValuePtr(_tournaments, fields[0].GetUInt32());
            if (!tournament)
                continue;

            TournamentDungeon dungeon;
            dungeon.slot = fields[1].GetUInt8();
            dungeon.mapId = fields[2].GetUInt16();
            dungeon.difficulty = fields[3].GetUInt8();
            dungeon.revealed = fields[4].GetBool();
            tournament->dungeons[dungeon.slot] = dungeon;
        } while (result->NextRow());
    }

    // teams
    if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_TOURNAMENT_TEAM_ALL)))
    {
        do
        {
            Field* fields = result->Fetch();
            TournamentTeam team;
            team.id = fields[0].GetUInt32();
            team.tournamentId = fields[1].GetUInt32();
            team.name = fields[2].GetString();
            team.status = TournamentTeamStatus(fields[3].GetUInt8());
            team.dqReason = fields[4].GetString();
            _nextTeamId = std::max(_nextTeamId, team.id + 1);
            _teams[team.id] = std::move(team);
        } while (result->NextRow());
    }

    // members
    if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_TOURNAMENT_MEMBER_ALL)))
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 teamId = fields[0].GetUInt32();
            TournamentTeam* team = Trinity::Containers::MapGetValuePtr(_teams, teamId);
            if (!team)
                continue;

            TournamentMember member;
            member.charGuid = fields[1].GetUInt32();
            member.accountId = fields[2].GetUInt32();
            member.role = TournamentRole(fields[3].GetUInt8());
            team->members.push_back(member);
            _memberIndex.emplace(member.charGuid, teamId);
        } while (result->NextRow());
    }

    // runs: not held in memory, but advance the id counter past the highest stored run
    if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_TOURNAMENT_RUN_ALL)))
    {
        do
        {
            Field* fields = result->Fetch();
            _nextRunId = std::max(_nextRunId, fields[0].GetUInt32() + 1);
        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", ">> Loaded {} tournament(s), {} team(s) in {} ms",
        uint32(_tournaments.size()), uint32(_teams.size()), GetMSTimeDiffToNow(oldMSTime));
}

TournamentData const* TournamentMgr::GetActiveTournament() const
{
    TournamentData const* fallback = nullptr;
    for (auto const& pair : _tournaments)
    {
        if (pair.second.state == TOURNAMENT_STATE_RUNNING)
            return &pair.second;
        if (!fallback || pair.second.id > fallback->id)
            fallback = &pair.second;
    }
    return fallback;
}

TournamentData const* TournamentMgr::GetTournament(uint32 id) const
{
    return Trinity::Containers::MapGetValuePtr(_tournaments, id);
}

TournamentTeam const* TournamentMgr::GetTeam(uint32 teamId) const
{
    return Trinity::Containers::MapGetValuePtr(_teams, teamId);
}

TournamentTeam const* TournamentMgr::GetTeamByMember(ObjectGuid::LowType charGuid, uint32 tournamentId) const
{
    auto range = _memberIndex.equal_range(charGuid);
    for (auto itr = range.first; itr != range.second; ++itr)
        if (TournamentTeam const* team = GetTeam(itr->second))
            if (team->tournamentId == tournamentId)
                return team;

    return nullptr;
}

std::vector<TournamentTeam const*> TournamentMgr::GetTeams(uint32 tournamentId) const
{
    std::vector<TournamentTeam const*> teams;
    for (auto const& pair : _teams)
        if (pair.second.tournamentId == tournamentId)
            teams.push_back(&pair.second);

    std::sort(teams.begin(), teams.end(), [](TournamentTeam const* a, TournamentTeam const* b) { return a->id < b->id; });
    return teams;
}

uint32 TournamentMgr::CreateTournament(std::string_view name, uint8 difficulty, ObjectGuid::LowType admin)
{
    TournamentData data;
    data.id = _nextTournamentId++;
    data.name = name;
    data.state = TOURNAMENT_STATE_DRAFT;
    data.difficulty = difficulty;
    data.ilvlCap = 213;
    data.createdBy = admin;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TOURNAMENT);
    stmt->setUInt32(0, data.id);
    stmt->setString(1, data.name);
    stmt->setUInt8(2, data.state);
    stmt->setUInt8(3, data.difficulty);
    stmt->setUInt16(4, data.ilvlCap);
    stmt->setUInt32(5, data.startTime);
    stmt->setUInt32(6, data.endTime);
    stmt->setUInt32(7, data.createdBy);
    CharacterDatabase.Execute(stmt);

    uint32 const id = data.id;
    _tournaments[id] = std::move(data);
    return id;
}

bool TournamentMgr::DeleteTournament(uint32 id)
{
    auto itr = _tournaments.find(id);
    if (itr == _tournaments.end())
        return false;

    // cascade teams (and their members) belonging to this tournament
    std::vector<uint32> teamIds;
    for (auto const& pair : _teams)
        if (pair.second.tournamentId == id)
            teamIds.push_back(pair.first);
    for (uint32 teamId : teamIds)
        DeleteTeam(teamId);

    CharacterDatabasePreparedStatement* delDungeons = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TOURNAMENT_DUNGEON_ALL);
    delDungeons->setUInt32(0, id);
    CharacterDatabase.Execute(delDungeons);

    CharacterDatabasePreparedStatement* delTournament = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TOURNAMENT);
    delTournament->setUInt32(0, id);
    CharacterDatabase.Execute(delTournament);

    _tournaments.erase(itr);
    return true;
}

bool TournamentMgr::SetState(uint32 id, TournamentState state)
{
    TournamentData* data = Trinity::Containers::MapGetValuePtr(_tournaments, id);
    if (!data)
        return false;

    data->state = state;
    if (state == TOURNAMENT_STATE_RUNNING && !data->startTime)
        data->startTime = uint32(GameTime::GetGameTime());
    if (state == TOURNAMENT_STATE_ENDED)
        data->endTime = uint32(GameTime::GetGameTime());

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_TOURNAMENT_STATE);
    stmt->setUInt8(0, data->state);
    stmt->setUInt32(1, data->startTime);
    stmt->setUInt32(2, data->endTime);
    stmt->setUInt32(3, id);
    CharacterDatabase.Execute(stmt);

    if (state == TOURNAMENT_STATE_RUNNING)
        RevealDungeons(id);
    else
    {
        // live runs only exist while their tournament is RUNNING; void any leftovers
        std::vector<uint32> liveInstances;
        {
            std::lock_guard<std::mutex> lock(_runsLock);
            for (auto const& pair : _runsByInstance)
                if (TournamentTeam const* team = GetTeam(pair.second.teamId))
                    if (team->tournamentId == id)
                        liveInstances.push_back(pair.first);
        }

        for (uint32 instanceId : liveInstances)
            TerminateRun(instanceId, TOURNAMENT_RUN_VOID, "tournament no longer running");
    }
    return true;
}

bool TournamentMgr::SetIlvlCap(uint32 id, uint16 cap)
{
    TournamentData* data = Trinity::Containers::MapGetValuePtr(_tournaments, id);
    if (!data)
        return false;

    data->ilvlCap = cap;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_TOURNAMENT_ILVL);
    stmt->setUInt16(0, cap);
    stmt->setUInt32(1, id);
    CharacterDatabase.Execute(stmt);
    return true;
}

bool TournamentMgr::SetDungeon(uint32 id, uint8 slot, uint16 mapId, uint8 difficulty)
{
    TournamentData* data = Trinity::Containers::MapGetValuePtr(_tournaments, id);
    if (!data || slot < 1 || slot > TOURNAMENT_DUNGEON_NUM)
        return false;

    TournamentDungeon dungeon;
    dungeon.slot = slot;
    dungeon.mapId = mapId;
    dungeon.difficulty = difficulty;
    dungeon.revealed = data->state == TOURNAMENT_STATE_RUNNING; // hidden until start; a swap mid-event is immediately playable

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_TOURNAMENT_DUNGEON);
    stmt->setUInt32(0, id);
    stmt->setUInt8(1, dungeon.slot);
    stmt->setUInt16(2, dungeon.mapId);
    stmt->setUInt8(3, dungeon.difficulty);
    stmt->setBool(4, dungeon.revealed);
    CharacterDatabase.Execute(stmt);

    data->dungeons[slot] = dungeon;
    return true;
}

bool TournamentMgr::RemoveDungeon(uint32 id, uint8 slot)
{
    TournamentData* data = Trinity::Containers::MapGetValuePtr(_tournaments, id);
    if (!data || !data->dungeons.count(slot))
        return false;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TOURNAMENT_DUNGEON);
    stmt->setUInt32(0, id);
    stmt->setUInt8(1, slot);
    CharacterDatabase.Execute(stmt);

    data->dungeons.erase(slot);
    return true;
}

void TournamentMgr::RevealDungeons(uint32 id)
{
    TournamentData* data = Trinity::Containers::MapGetValuePtr(_tournaments, id);
    if (!data)
        return;

    for (auto& pair : data->dungeons)
        pair.second.revealed = true;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_TOURNAMENT_DUNGEON_REVEAL);
    stmt->setBool(0, true);
    stmt->setUInt32(1, id);
    CharacterDatabase.Execute(stmt);
}

uint32 TournamentMgr::CreateTeam(uint32 tournamentId, std::string_view name)
{
    if (!_tournaments.count(tournamentId))
        return 0;

    TournamentTeam team;
    team.id = _nextTeamId++;
    team.tournamentId = tournamentId;
    team.name = name;
    team.status = TOURNAMENT_TEAM_ACTIVE;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TOURNAMENT_TEAM);
    stmt->setUInt32(0, team.id);
    stmt->setUInt32(1, team.tournamentId);
    stmt->setString(2, team.name);
    stmt->setUInt8(3, team.status);
    stmt->setString(4, team.dqReason);
    CharacterDatabase.Execute(stmt);

    uint32 const id = team.id;
    _teams[id] = std::move(team);
    return id;
}

void TournamentMgr::VoidLiveRunsOfTeam(uint32 teamId, std::string_view why)
{
    std::vector<uint32> liveInstances;
    {
        std::lock_guard<std::mutex> lock(_runsLock);
        for (auto const& pair : _runsByInstance)
            if (pair.second.teamId == teamId)
                liveInstances.push_back(pair.first);
    }

    for (uint32 instanceId : liveInstances)
        TerminateRun(instanceId, TOURNAMENT_RUN_VOID, why);
}

bool TournamentMgr::DeleteTeam(uint32 teamId)
{
    auto itr = _teams.find(teamId);
    if (itr == _teams.end())
        return false;

    VoidLiveRunsOfTeam(teamId, "team deleted");

    for (TournamentMember const& member : itr->second.members)
        Trinity::Containers::MultimapErasePair(_memberIndex, member.charGuid, teamId);

    CharacterDatabasePreparedStatement* delMembers = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TOURNAMENT_MEMBER_BY_TEAM);
    delMembers->setUInt32(0, teamId);
    CharacterDatabase.Execute(delMembers);

    CharacterDatabasePreparedStatement* delTeam = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TOURNAMENT_TEAM);
    delTeam->setUInt32(0, teamId);
    CharacterDatabase.Execute(delTeam);

    _teams.erase(itr);
    return true;
}

bool TournamentMgr::AddMember(uint32 teamId, ObjectGuid::LowType charGuid, uint32 accountId, TournamentRole role)
{
    TournamentTeam* team = Trinity::Containers::MapGetValuePtr(_teams, teamId);
    if (!team)
        return false;

    // a character may belong to only one team per tournament
    if (GetTeamByMember(charGuid, team->tournamentId))
        return false;

    if (team->members.size() >= TOURNAMENT_TEAM_SIZE)
        return false;

    TournamentMember member;
    member.charGuid = charGuid;
    member.accountId = accountId;
    member.role = role;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TOURNAMENT_MEMBER);
    stmt->setUInt32(0, teamId);
    stmt->setUInt32(1, charGuid);
    stmt->setUInt32(2, accountId);
    stmt->setUInt8(3, role);
    CharacterDatabase.Execute(stmt);

    team->members.push_back(member);
    _memberIndex.emplace(charGuid, teamId);
    return true;
}

bool TournamentMgr::RemoveMember(uint32 teamId, ObjectGuid::LowType charGuid)
{
    TournamentTeam* team = Trinity::Containers::MapGetValuePtr(_teams, teamId);
    if (!team)
        return false;

    std::vector<TournamentMember>& members = team->members;
    auto memberItr = std::find_if(members.begin(), members.end(),
        [charGuid](TournamentMember const& m) { return m.charGuid == charGuid; });
    if (memberItr == members.end())
        return false;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TOURNAMENT_MEMBER);
    stmt->setUInt32(0, teamId);
    stmt->setUInt32(1, charGuid);
    CharacterDatabase.Execute(stmt);

    members.erase(memberItr);
    Trinity::Containers::MultimapErasePair(_memberIndex, charGuid, teamId);
    return true;
}

bool TournamentMgr::DisqualifyTeam(uint32 teamId, std::string_view reason)
{
    TournamentTeam* team = Trinity::Containers::MapGetValuePtr(_teams, teamId);
    if (!team)
        return false;

    VoidLiveRunsOfTeam(teamId, "team disqualified");

    team->status = TOURNAMENT_TEAM_DISQUALIFIED;
    team->dqReason = reason;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_TOURNAMENT_TEAM_STATUS);
    stmt->setUInt8(0, team->status);
    stmt->setString(1, team->dqReason);
    stmt->setUInt32(2, teamId);
    CharacterDatabase.Execute(stmt);
    return true;
}

bool TournamentMgr::RequalifyTeam(uint32 teamId)
{
    TournamentTeam* team = Trinity::Containers::MapGetValuePtr(_teams, teamId);
    if (!team)
        return false;

    team->status = TOURNAMENT_TEAM_ACTIVE;
    team->dqReason.clear();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_TOURNAMENT_TEAM_STATUS);
    stmt->setUInt8(0, team->status);
    stmt->setString(1, team->dqReason);
    stmt->setUInt32(2, teamId);
    CharacterDatabase.Execute(stmt);
    return true;
}

TournamentTeam const* TournamentMgr::MatchTeam(std::vector<ObjectGuid::LowType> const& memberGuids) const
{
    if (memberGuids.size() != TOURNAMENT_TEAM_SIZE)
        return nullptr;

    // resolve candidate teams via the first member, then require an exact set match
    auto range = _memberIndex.equal_range(memberGuids.front());
    for (auto idxItr = range.first; idxItr != range.second; ++idxItr)
        if (TournamentTeam const* team = MatchTeamCandidate(GetTeam(idxItr->second), memberGuids))
            return team;

    return nullptr;
}

TournamentTeam const* TournamentMgr::MatchTeamCandidate(TournamentTeam const* team, std::vector<ObjectGuid::LowType> const& memberGuids) const
{
    if (!team || team->status != TOURNAMENT_TEAM_ACTIVE)
        return nullptr;

    if (team->members.size() != memberGuids.size() || !team->HasRoleComposition())
        return nullptr;

    for (ObjectGuid::LowType guid : memberGuids)
        if (!team->GetMember(guid))
            return nullptr;

    return team;
}

Item const* TournamentMgr::GetEquippedViolation(Player const* player, uint16 ilvlCap)
{
    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        if (Item const* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
            if (item->GetItemLevel() > ilvlCap)
                return item;

    return nullptr;
}

Item const* TournamentMgr::GetContestantEntryViolation(Player const* player, uint16 mapId, uint8 difficulty) const
{
    TournamentData const* tournament = GetActiveTournament();
    if (!tournament || tournament->state != TOURNAMENT_STATE_RUNNING)
        return nullptr;

    if (!tournament->GetDungeonByMap(mapId, difficulty))
        return nullptr;

    TournamentTeam const* team = GetTeamByMember(player->GetGUID().GetCounter(), tournament->id);
    if (!team || team->status != TOURNAMENT_TEAM_ACTIVE)
        return nullptr;

    return GetEquippedViolation(player, tournament->ilvlCap);
}

TournamentData const* TournamentMgr::GetRunningTournamentForTeam(uint32 teamId) const
{
    TournamentTeam const* team = GetTeam(teamId);
    if (!team)
        return nullptr;

    TournamentData const* tournament = GetTournament(team->tournamentId);
    if (!tournament || tournament->state != TOURNAMENT_STATE_RUNNING)
        return nullptr;

    return tournament;
}

uint32 TournamentMgr::GetEquipViolationRunId(Player const* player, Item const* item) const
{
    Map const* map = player->GetMap();
    if (!map || !map->IsDungeon())
        return 0;

    std::lock_guard<std::mutex> lock(_runsLock);

    TournamentRun const* run = Trinity::Containers::MapGetValuePtr(_runsByInstance, map->GetInstanceId());
    if (!run)
        return 0;

    if (run->state != TOURNAMENT_RUN_PENDING && run->state != TOURNAMENT_RUN_ACTIVE)
        return 0;

    TournamentData const* tournament = GetRunningTournamentForTeam(run->teamId);
    if (!tournament)
        return 0;

    TournamentTeam const* team = GetTeam(run->teamId);
    if (!team->GetMember(player->GetGUID().GetCounter()))
        return 0;

    if (item->GetItemLevel() <= tournament->ilvlCap)
        return 0;

    return run->id;
}

bool TournamentMgr::IsContestantEquipViolation(Player const* player, Item const* item) const
{
    return GetEquipViolationRunId(player, item) != 0;
}

void TournamentMgr::LogEquipViolation(Player const* player, Item const* item)
{
    if (uint32 const runId = GetEquipViolationRunId(player, item))
        LogEvent(runId, TOURNAMENT_EVENT_GEAR_VIOLATION, Trinity::StringFormat("{} attempted to equip item {} (ilvl {}), denied",
            player->GetName(), item->GetEntry(), item->GetItemLevel()));
}

void TournamentMgr::OnPlayerCombatStart(Player const* player)
{
    Map const* map = player->GetMap();
    if (!map || !map->IsDungeon())
        return;

    std::lock_guard<std::mutex> lock(_runsLock);

    TournamentRun* run = Trinity::Containers::MapGetValuePtr(_runsByInstance, map->GetInstanceId());
    if (!run)
        return;

    if (run->state != TOURNAMENT_RUN_PENDING)
        return;

    if (!GetRunningTournamentForTeam(run->teamId))
        return;

    TournamentTeam const* team = GetTeam(run->teamId);
    if (!team->GetMember(player->GetGUID().GetCounter()))
        return;

    StampCombatStart(*run);
}

uint32 TournamentMgr::CreateRun(uint32 teamId, uint8 dungeonSlot, uint16 mapId, uint32 instanceId)
{
    if (!GetRunningTournamentForTeam(teamId))
        return 0;

    std::lock_guard<std::mutex> lock(_runsLock);

    TournamentRun run;
    run.id = _nextRunId++;
    run.teamId = teamId;
    run.dungeonSlot = dungeonSlot;
    run.mapId = mapId;
    run.instanceId = instanceId;
    run.state = TOURNAMENT_RUN_PENDING;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TOURNAMENT_RUN);
    stmt->setUInt32(0, run.id);
    stmt->setUInt32(1, run.teamId);
    stmt->setUInt8(2, run.dungeonSlot);
    stmt->setUInt16(3, run.mapId);
    stmt->setUInt32(4, run.instanceId);
    stmt->setUInt8(5, run.state);
    stmt->setUInt32(6, uint32(GameTime::GetGameTime()));
    CharacterDatabase.Execute(stmt);

    uint32 const id = run.id;
    _runsByInstance[instanceId] = std::move(run);
    LogEvent(id, TOURNAMENT_EVENT_ENTER, "");
    return id;
}

TournamentRun const* TournamentMgr::GetRunByInstance(uint32 instanceId) const
{
    std::lock_guard<std::mutex> lock(_runsLock);
    return Trinity::Containers::MapGetValuePtr(_runsByInstance, instanceId);
}

void TournamentMgr::FlagRunFinalizing(uint32 instanceId)
{
    std::lock_guard<std::mutex> lock(_runsLock);
    if (TournamentRun* run = Trinity::Containers::MapGetValuePtr(_runsByInstance, instanceId))
        run->finalizing = true;
}

bool TournamentMgr::IsRunFinalizing(uint32 instanceId) const
{
    std::lock_guard<std::mutex> lock(_runsLock);
    TournamentRun const* run = Trinity::Containers::MapGetValuePtr(_runsByInstance, instanceId);
    return run && run->finalizing;
}

void TournamentMgr::StampCombatStart(TournamentRun& run)
{
    if (run.combatStart)
        return;

    run.combatStart = uint32(GameTime::GetGameTime());
    run.combatStartMSTime = GameTime::GetGameTimeMS();
    run.state = TOURNAMENT_RUN_ACTIVE;
    SaveRun(run);
    LogEvent(run.id, TOURNAMENT_EVENT_COMBAT_START, "");
}

bool TournamentMgr::TerminateRun(uint32 instanceId, TournamentRunState state, std::string_view why)
{
    std::lock_guard<std::mutex> lock(_runsLock);

    auto itr = _runsByInstance.find(instanceId);
    if (itr == _runsByInstance.end())
        return false;

    TournamentRun& run = itr->second;
    run.state = state;
    TournamentRunEventType eventType;
    switch (state)
    {
        case TOURNAMENT_RUN_COMPLETED:
            run.bossFinish = uint32(GameTime::GetGameTime());
            run.durationMs = run.combatStartMSTime ? getMSTimeDiff(run.combatStartMSTime, GameTime::GetGameTimeMS()) : 0;
            eventType = TOURNAMENT_EVENT_FINISH;
            break;
        case TOURNAMENT_RUN_REJECTED:
            run.rejectReason = why;
            eventType = TOURNAMENT_EVENT_REJECTED;
            break;
        default:
            run.rejectReason = why;
            eventType = TOURNAMENT_EVENT_VOID;
            break;
    }

    SaveRun(run);
    LogEvent(run.id, eventType, why);
    _runsByInstance.erase(itr);
    return true;
}

void TournamentMgr::CompleteRun(uint32 instanceId)
{
    TerminateRun(instanceId, TOURNAMENT_RUN_COMPLETED, "");
}

void TournamentMgr::RejectRun(uint32 instanceId, std::string_view why)
{
    TerminateRun(instanceId, TOURNAMENT_RUN_REJECTED, why);
}

void TournamentMgr::VoidRun(uint32 instanceId, std::string_view why)
{
    TerminateRun(instanceId, TOURNAMENT_RUN_VOID, why);
}

void TournamentMgr::SaveRun(TournamentRun const& run)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_TOURNAMENT_RUN);
    stmt->setUInt8(0, run.state);
    stmt->setUInt32(1, run.combatStart);
    stmt->setUInt32(2, run.bossFinish);
    stmt->setUInt32(3, run.durationMs);
    stmt->setString(4, run.rejectReason);
    stmt->setUInt32(5, 0);
    stmt->setUInt32(6, run.id);
    CharacterDatabase.Execute(stmt);
}

void TournamentMgr::LogEvent(uint32 runId, TournamentRunEventType type, std::string_view detail)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TOURNAMENT_RUN_EVENT);
    stmt->setUInt32(0, runId);
    stmt->setUInt32(1, uint32(GameTime::GetGameTime()));
    stmt->setUInt8(2, type);
    stmt->setStringView(3, detail);
    CharacterDatabase.Execute(stmt);
}

bool TournamentMgr::SetRunVerdict(uint32 runId, TournamentRunState state, std::string_view reason, ObjectGuid::LowType staff)
{
    // completion is engine-driven only: a forced COMPLETED verdict would carry no timings (0 ms would win the slot)
    if (state != TOURNAMENT_RUN_VOID && state != TOURNAMENT_RUN_REJECTED)
        return false;

    // if the run is still live, route through the normal lifecycle so the instance entry is released
    uint32 liveInstanceId = 0;
    {
        std::lock_guard<std::mutex> lock(_runsLock);
        for (auto const& pair : _runsByInstance)
        {
            if (pair.second.id != runId)
                continue;

            liveInstanceId = pair.second.instanceId;
            break;
        }
    }

    // if the run finished in between, fall through to the stored-row verdict
    if (liveInstanceId && TerminateRun(liveInstanceId, state, reason))
        return true;

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_TOURNAMENT_RUN_VERDICT);
    stmt->setUInt8(0, state);
    stmt->setStringView(1, reason);
    stmt->setUInt32(2, staff);
    stmt->setUInt32(3, runId);
    CharacterDatabase.Execute(stmt);

    LogEvent(runId, state == TOURNAMENT_RUN_VOID ? TOURNAMENT_EVENT_VOID : TOURNAMENT_EVENT_REJECTED, reason);
    return true;
}

void TournamentMgr::BuildStandings(uint32 tournamentId, std::vector<TournamentStanding>& standings) const
{
    standings.clear();

    std::unordered_map<uint32, TournamentStanding> byTeam;
    for (auto const& pair : _teams)
        if (pair.second.tournamentId == tournamentId && pair.second.status == TOURNAMENT_TEAM_ACTIVE)
            byTeam[pair.first].teamId = pair.first;

    if (byTeam.empty())
        return;

    // best completed duration per team per dungeon slot
    if (PreparedQueryResult result = CharacterDatabase.Query(CharacterDatabase.GetPreparedStatement(CHAR_SEL_TOURNAMENT_RUN_ALL)))
    {
        do
        {
            Field* fields = result->Fetch();
            TournamentStanding* standing = Trinity::Containers::MapGetValuePtr(byTeam, fields[1].GetUInt32());
            if (!standing)
                continue;

            if (TournamentRunState(fields[5].GetUInt8()) != TOURNAMENT_RUN_COMPLETED)
                continue;

            uint8 slot = fields[2].GetUInt8();
            uint32 durationMs = fields[8].GetUInt32();
            auto slotItr = standing->bestSlotTimeMs.find(slot);
            if (slotItr == standing->bestSlotTimeMs.end() || durationMs < slotItr->second)
                standing->bestSlotTimeMs[slot] = durationMs;
        } while (result->NextRow());
    }

    // only the fastest team of each dungeon slot receives the point
    for (uint8 slot = 1; slot <= TOURNAMENT_DUNGEON_NUM; ++slot)
    {
        TournamentStanding* fastest = nullptr;
        for (auto& pair : byTeam)
        {
            auto slotItr = pair.second.bestSlotTimeMs.find(slot);
            if (slotItr == pair.second.bestSlotTimeMs.end())
                continue;

            if (!fastest || slotItr->second < fastest->bestSlotTimeMs[slot])
                fastest = &pair.second;
        }

        if (fastest)
            ++fastest->points;
    }

    for (auto& pair : byTeam)
    {
        TournamentStanding& standing = pair.second;
        standing.completedSlots = uint8(standing.bestSlotTimeMs.size());
        for (auto const& slotPair : standing.bestSlotTimeMs)
            standing.totalTimeMs += slotPair.second;
        standings.push_back(std::move(standing));
    }

    // rank: points desc, then combined time asc (tiebreaker), then completed count desc
    std::sort(standings.begin(), standings.end(), [](TournamentStanding const& a, TournamentStanding const& b)
    {
        if (a.points != b.points)
            return a.points > b.points;
        if (a.completedSlots != b.completedSlots)
            return a.completedSlots > b.completedSlots;
        return a.totalTimeMs < b.totalTimeMs;
    });
}
