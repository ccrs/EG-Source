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

#ifndef EG_ARENA_1V1_MGR_H
#define EG_ARENA_1V1_MGR_H

#include "Common.h"
#include "ObjectGuid.h"
#include <map>
#include <unordered_map>

class Creature;
class Player;


struct Arena1v1Stats
{
    uint16 Rating;
    uint16 MatchMakerRating;
    uint16 WeekGames;
    uint16 WeekWins;
    uint16 SeasonGames;
    uint16 SeasonWins;
    uint32 PreviousOpponent; // low-guid
};

struct Arena1v1MatchOutcome
{
    int32 WinnerRatingDelta;
    int32 LoserRatingDelta;
    uint16 WinnerMmrSnapshot;
    uint16 LoserMmrSnapshot;
};

class TC_GAME_API Arena1v1Mgr
{
    Arena1v1Mgr();
    ~Arena1v1Mgr();

public:
    static Arena1v1Mgr* instance();

    void LoadFromDB();

    bool IsEnabled() const;

    Arena1v1Stats const* TryGetStats(ObjectGuid guid) const;
    Arena1v1Stats& GetOrCreate(ObjectGuid guid);

    bool HandleJoinQueue(Player* player, Creature* npc);
    bool HandleLeaveQueue(Player* player);
    void WhisperStats(Player* player, Creature* npc);

    // Called from Group hook when the player joins any group while queued
    void CancelQueueIfPresent(ObjectGuid guid);

    // Called from Arena::EndBattleground when GetArenaType() == ARENA_TYPE_1V1
    Arena1v1MatchOutcome ApplyMatchResult(ObjectGuid winnerGuid, ObjectGuid loserGuid, uint32 winnerMmrSeen, uint32 loserMmrSeen);

    // Called when a player forfeits a rated match in progress (RemovePlayerAtLeave)
    // Pass ObjectGuid::Empty for opponentGuid to void the win credit (e.g., both players are leaving simultaneously)
    // Returns the rating outcome so the Battleground's _arenaTeamScores can be populated for the post-match score packet
    Arena1v1MatchOutcome ApplyForfeit(ObjectGuid leaverGuid, ObjectGuid opponentGuid, uint32 opponentMmrSeen);

    // Called from Arena::EndBattleground for a 1v1 draw (45-min timeout)
    void ApplyDraw(ObjectGuid playerA, ObjectGuid playerB);

    // Folded into ArenaTeamMgr::DistributeArenaPoints. Adds 1v1 points to the shared per-player point map so they compete via max() with bracketed points
    void AccumulateWeeklyPoints(std::map<ObjectGuid, uint32>& playerPoints);

    // Resets weekly counters; called after AccumulateWeeklyPoints from the weekly distribution flow
    void FinishWeek();

    void DeleteCachedStats(ObjectGuid guid);

private:
    static float GetChanceAgainst(uint32 ownRating, uint32 opponentRating);
    static int32 GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won);
    static int32 GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won);

    static void ApplyClampedDelta(uint16& field, int32 delta);

    uint32 ComputeWeeklyPoints(uint16 personalRating) const;

    void SaveStats(ObjectGuid guid, Arena1v1Stats const& stats, bool insert) const;

    typedef std::unordered_map<ObjectGuid, Arena1v1Stats> StatsStore;
    StatsStore _store;
};

#define sArena1v1Mgr Arena1v1Mgr::instance()

#endif // EG_ARENA_1V1_MGR_H
