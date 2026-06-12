#ifndef EG_TOURNAMENT_MGR_H
#define EG_TOURNAMENT_MGR_H

#include "Define.h"
#include "ObjectGuid.h"
#include <shared_mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class Item;
class Player;

enum TournamentState : uint8
{
    TOURNAMENT_STATE_DRAFT = 0,
    TOURNAMENT_STATE_REGISTRATION,
    TOURNAMENT_STATE_LOCKED,
    TOURNAMENT_STATE_RUNNING,
    TOURNAMENT_STATE_ENDED
};

enum TournamentRole : uint8
{
    TOURNAMENT_ROLE_TANK = 0,
    TOURNAMENT_ROLE_HEALER,
    TOURNAMENT_ROLE_DPS
};

enum TournamentTeamStatus : uint8
{
    TOURNAMENT_TEAM_ACTIVE = 0,
    TOURNAMENT_TEAM_DISQUALIFIED
};

enum TournamentRunState : uint8
{
    TOURNAMENT_RUN_PENDING = 0,
    TOURNAMENT_RUN_ACTIVE,
    TOURNAMENT_RUN_COMPLETED,
    TOURNAMENT_RUN_VOID,
    TOURNAMENT_RUN_REJECTED
};

enum TournamentRunEventType : uint8
{
    TOURNAMENT_EVENT_ENTER = 0,
    TOURNAMENT_EVENT_COMBAT_START,
    TOURNAMENT_EVENT_BOSS_KILL,
    TOURNAMENT_EVENT_WIPE,
    TOURNAMENT_EVENT_GEAR_VIOLATION,
    TOURNAMENT_EVENT_FINISH,
    TOURNAMENT_EVENT_VOID,
    TOURNAMENT_EVENT_REJECTED
};

uint8 constexpr TOURNAMENT_TEAM_SIZE = 5;
uint8 constexpr TOURNAMENT_DUNGEON_NUM = 5;

struct TournamentMember
{
    ObjectGuid::LowType charGuid = 0;
    uint32 accountId = 0;
    TournamentRole role = TOURNAMENT_ROLE_DPS;
};

struct TournamentTeam
{
    uint32 id = 0;
    uint32 tournamentId = 0;
    std::string name;
    TournamentTeamStatus status = TOURNAMENT_TEAM_ACTIVE;
    std::string dqReason;
    std::vector<TournamentMember> members;

    TournamentMember const* GetMember(ObjectGuid::LowType guid) const;
    bool HasRoleComposition() const; // exactly 1 tank, 1 healer, 3 dps
};

struct TournamentDungeon
{
    uint8 slot = 0;
    uint16 mapId = 0;
    uint8 difficulty = 0;
    bool revealed = false;
};

struct TournamentData
{
    uint32 id = 0;
    std::string name;
    TournamentState state = TOURNAMENT_STATE_DRAFT;
    uint8 difficulty = 0;
    uint16 ilvlCap = 213;
    uint32 startTime = 0;
    uint32 endTime = 0;
    ObjectGuid::LowType createdBy = 0;
    std::unordered_map<uint8, TournamentDungeon> dungeons; // slot -> dungeon

    TournamentDungeon const* GetDungeonByMap(uint16 mapId, uint8 difficulty) const;
};

struct TournamentRun
{
    uint32 id = 0;
    uint32 teamId = 0;
    uint8 dungeonSlot = 0;
    uint16 mapId = 0;
    uint32 instanceId = 0;
    TournamentRunState state = TOURNAMENT_RUN_PENDING;
    uint32 combatStart = 0; // unix time, first player in combat
    uint32 bossFinish = 0; // unix time, final boss dead with all encounters done
    uint32 durationMs = 0; // millisecond-precise run time, measured on server uptime
    std::string rejectReason;
    uint32 combatStartMSTime = 0; // transient: GetGameTimeMS() stamp backing durationMs
    bool finalizing = false; // transient: final boss credited, completion pending its DONE state
};

struct TournamentStanding
{
    uint32 teamId = 0;
    uint32 points = 0;
    uint64 totalTimeMs = 0;
    uint8 completedSlots = 0;
    std::unordered_map<uint8, uint32> bestSlotTimeMs; // slot -> best completed duration
};

class TC_GAME_API TournamentMgr
{
public:
    static TournamentMgr* instance();

    void LoadFromDB();

    // ----- read access -----
    TournamentData const* GetActiveTournament() const; // the single RUNNING/most-recent tournament
    TournamentData const* GetTournament(uint32 id) const;
    TournamentTeam const* GetTeam(uint32 teamId) const;
    TournamentTeam const* GetTeamByMember(ObjectGuid::LowType charGuid, uint32 tournamentId) const; // membership is scoped per tournament
    std::vector<TournamentTeam const*> GetTeams(uint32 tournamentId) const;

    // ----- tournament lifecycle (admin) -----
    uint32 CreateTournament(std::string_view name, uint8 difficulty, ObjectGuid::LowType admin);
    bool DeleteTournament(uint32 id);
    bool SetState(uint32 id, TournamentState state);
    bool SetIlvlCap(uint32 id, uint16 cap);
    bool SetDungeon(uint32 id, uint8 slot, uint16 mapId, uint8 difficulty);
    bool RemoveDungeon(uint32 id, uint8 slot);
    void RevealDungeons(uint32 id); // flips revealed on RUNNING transition

    // ----- team / contestant (admin) -----
    uint32 CreateTeam(uint32 tournamentId, std::string_view name);
    bool DeleteTeam(uint32 teamId);
    bool AddMember(uint32 teamId, ObjectGuid::LowType charGuid, uint32 accountId, TournamentRole role);
    bool RemoveMember(uint32 teamId, ObjectGuid::LowType charGuid);
    bool DisqualifyTeam(uint32 teamId, std::string_view reason);
    bool RequalifyTeam(uint32 teamId);

    // ----- contestant signalling (LFG) -----
    // the eligible team (active, 1/1/3) whose member set exactly matches the given guids, or nullptr
    TournamentTeam const* MatchTeam(std::vector<ObjectGuid::LowType> const& memberGuids) const;

    // ----- gear enforcement -----
    // first equipped item above the cap, or nullptr
    static Item const* GetEquippedViolation(Player const* player, uint16 ilvlCap);
    // offending equipped item gating the LFG teleport of an active contestant into a tournament dungeon
    Item const* GetContestantEntryViolation(Player const* player, uint16 mapId, uint8 difficulty) const;
    // true when the player would equip an item above the cap inside his team's live run, gates Player::CanEquipItem
    bool IsContestantEquipViolation(Player const* player, Item const* item) const;
    void LogEquipViolation(Player const* player, Item const* item);

    // ----- run tracking hooks -----
    // the run timer starts when the first contestant enters combat
    void OnPlayerCombatStart(Player const* player);

    // ----- run lifecycle (run tracker) -----
    // terminal transitions are keyed by instance id, no-op without a live run and invalidate fetched run pointers
    uint32 CreateRun(uint32 teamId, uint8 dungeonSlot, uint16 mapId, uint32 instanceId);
    TournamentRun const* GetRunByInstance(uint32 instanceId) const;
    std::vector<TournamentRun> GetLiveRuns() const; // snapshot for staff overview
    void CompleteRun(uint32 instanceId); // all boss states DONE at end-boss
    void RejectRun(uint32 instanceId, std::string_view why); // gear/skip/swap violation
    void VoidRun(uint32 instanceId, std::string_view why); // no-fault termination
    void LogEvent(uint32 runId, TournamentRunEventType type, std::string_view detail);

    // two-stage completion: the encounter credit fires before the final boss state is DONE, so it flags and SetBossState evaluates
    void FlagRunFinalizing(uint32 instanceId);
    bool IsRunFinalizing(uint32 instanceId) const;

    // staff verdict on a stored run (rejected/void only; completion is engine-driven)
    bool SetRunVerdict(uint32 runId, TournamentRunState state, std::string_view reason, ObjectGuid::LowType staff);
    bool AcceptRun(uint32 runId, ObjectGuid::LowType staff);

    // ----- scoring -----
    // per dungeon slot the fastest completed run wins 1 point, ties broken by summed duration
    void BuildStandings(uint32 tournamentId, std::vector<TournamentStanding>& standings) const;

    static std::string FormatDuration(uint32 durationMs); // m:ss.mmm

private:
    TournamentMgr() = default;
    ~TournamentMgr() = default;
    TournamentMgr(TournamentMgr const&) = delete;
    TournamentMgr& operator=(TournamentMgr const&) = delete;

    // private helpers are lock-free, public entry points hold _lock
    void SaveRun(TournamentRun const& run);
    void StampCombatStart(TournamentRun& run);
    TournamentData const* FindActiveTournament() const;
    TournamentTeam const* FindTeamByMember(ObjectGuid::LowType charGuid, uint32 tournamentId) const;
    uint32 GetEquipViolationRunId(Player const* player, Item const* item) const; // 0 when no live-run cap violation
    TournamentData const* GetRunningTournamentForTeam(uint32 teamId) const;
    TournamentTeam const* MatchTeamCandidate(TournamentTeam const* team, std::vector<ObjectGuid::LowType> const& memberGuids) const;
    void RevealDungeonsOfTournament(TournamentData& data);
    void VoidLiveRunsOfTeam(uint32 teamId, std::string_view why);
    void VoidLiveRunsOfSlot(uint32 tournamentId, uint8 slot, std::string_view why);
    void AnnounceToTeam(uint32 teamId, std::string_view message);
    void EraseTeam(uint32 teamId);
    bool TerminateRun(uint32 instanceId, TournamentRunState state, std::string_view why); // true if a live run was terminated

    std::unordered_map<uint32, TournamentData> _tournaments; // id -> tournament
    std::unordered_map<uint32, TournamentTeam> _teams; // teamId -> team
    std::unordered_multimap<ObjectGuid::LowType, uint32> _memberIndex; // charGuid -> teamIds (unique per tournament, not globally)
    std::unordered_map<uint32, TournamentRun> _runsByInstance; // instanceId -> active run
    mutable std::shared_mutex _lock; // guards all manager state, hooks run on map-updater threads while admin commands run on the world thread

    uint32 _nextTournamentId = 1;
    uint32 _nextTeamId = 1;
    uint32 _nextRunId = 1;
};

#define sTournamentMgr TournamentMgr::instance()

#endif // EG_TOURNAMENT_MGR_H
