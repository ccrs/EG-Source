#include "Group.h"
#include "Item.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "StringFormat.h"
#include "TournamentMgr.h"

class EG_tournament_player_scripts : public PlayerScript
{
public:
    EG_tournament_player_scripts() : PlayerScript("EG_tournament_player_scripts") { }

    void OnMapChanged(Player* player) override
    {
        Map const* map = player->GetMap();
        if (!map || !map->IsDungeon())
            return;

        TournamentData const* tournament = sTournamentMgr->GetActiveTournament();
        if (!tournament || tournament->state != TOURNAMENT_STATE_RUNNING)
            return;

        TournamentDungeon const* dungeon = tournament->GetDungeonByMap(uint16(map->GetId()), uint8(map->GetDifficulty()));
        if (!dungeon || !dungeon->revealed)
            return;

        // an already signaled run: enforce roster integrity and the gear backstop
        if (TournamentRun const* run = sTournamentMgr->GetRunByInstance(map->GetInstanceId()))
        {
            if (player->IsGameMaster())
                return;

            TournamentTeam const* team = sTournamentMgr->GetTeam(run->teamId);
            if (team && !team->GetMember(player->GetGUID().GetCounter()))
            {
                sTournamentMgr->RejectRun(map->GetInstanceId(), Trinity::StringFormat("non-registered character {} entered the instance", player->GetName()));
                return;
            }

            EnforceGearOnEntry(*run, player, tournament->ilvlCap);
            return;
        }

        Group* group = player->GetGroup();
        if (!group || !group->isLFGGroup() || group->GetMembersCount() != TOURNAMENT_TEAM_SIZE)
            return;

        std::vector<ObjectGuid::LowType> memberGuids;
        memberGuids.reserve(TOURNAMENT_TEAM_SIZE);
        for (Group::MemberSlot const& slot : group->GetMemberSlots())
            memberGuids.push_back(slot.guid.GetCounter());

        TournamentTeam const* team = sTournamentMgr->MatchTeam(memberGuids);
        if (!team || team->tournamentId != tournament->id)
            return;

        uint32 const runId = sTournamentMgr->CreateRun(team->id, dungeon->slot, uint16(map->GetId()), map->GetInstanceId());
        if (!runId)
            return;

        if (TournamentRun const* run = sTournamentMgr->GetRunByInstance(map->GetInstanceId()))
            EnforceGearOnEntry(*run, player, tournament->ilvlCap);

        TC_LOG_INFO("tournament", "Team {} '{}' signaled as contestant (run {}, map {}, instance {})",
            team->id, team->name, runId, map->GetId(), map->GetInstanceId());
    }

private:
    static void EnforceGearOnEntry(TournamentRun const& run, Player* player, uint16 ilvlCap)
    {
        Item const* violation = TournamentMgr::GetEquippedViolation(player, ilvlCap);
        if (!violation)
            return;

        std::string const detail = Trinity::StringFormat("{} entered with item {} (ilvl {})",
            player->GetName(), violation->GetEntry(), violation->GetTemplate()->GetBaseItemLevel());
        sTournamentMgr->LogEvent(run.id, TOURNAMENT_EVENT_GEAR_VIOLATION, detail);
        sTournamentMgr->RejectRun(run.instanceId, detail);
    }
};

void AddSC_EG_tournament_scripts()
{
    new EG_tournament_player_scripts();
}
