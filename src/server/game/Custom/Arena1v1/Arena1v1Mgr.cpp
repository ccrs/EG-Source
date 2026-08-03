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

#include "Arena1v1Mgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "BattlegroundPackets.h"
#include "BattlegroundQueue.h"
#include "Chat.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "DisableMgr.h"
#include "GameTime.h"
#include "Group.h"
#include "Language.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SharedDefines.h"
#include "StringFormat.h"
#include "World.h"
#include "WorldSession.h"
#include <cmath>

Arena1v1Mgr::Arena1v1Mgr() = default;
Arena1v1Mgr::~Arena1v1Mgr() = default;

Arena1v1Mgr* Arena1v1Mgr::instance()
{
    static Arena1v1Mgr inst;
    return &inst;
}

bool Arena1v1Mgr::IsEnabled() const
{
    return sWorld->getBoolConfig(CONFIG_ARENA_1V1_ENABLE);
}

void Arena1v1Mgr::LoadFromDB()
{
    _store.clear();

    uint32 oldMSTime = getMSTime();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ARENA_1V1_ALL);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 1v1 arena rows.");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();
        ObjectGuid guid = ObjectGuid::Create<HighGuid::Player>(fields[0].GetUInt32());

        Arena1v1Stats& stats = _store[guid];
        stats.Rating = fields[1].GetUInt16();
        stats.MatchMakerRating = fields[2].GetUInt16();
        stats.WeekGames = fields[3].GetUInt16();
        stats.WeekWins = fields[4].GetUInt16();
        stats.SeasonGames = fields[5].GetUInt16();
        stats.SeasonWins = fields[6].GetUInt16();
        stats.PreviousOpponent = fields[7].GetUInt32();
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded {} 1v1 arena rows in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
}

Arena1v1Stats const* Arena1v1Mgr::TryGetStats(ObjectGuid guid) const
{
    auto it = _store.find(guid);
    if (it == _store.end())
        return nullptr;
    return &it->second;
}

Arena1v1Stats& Arena1v1Mgr::GetOrCreate(ObjectGuid guid)
{
    auto it = _store.find(guid);
    if (it != _store.end())
        return it->second;

    Arena1v1Stats& stats = _store[guid];
    stats.Rating = uint16(sWorld->getIntConfig(CONFIG_ARENA_START_RATING));
    stats.MatchMakerRating = uint16(sWorld->getIntConfig(CONFIG_ARENA_START_MATCHMAKER_RATING));
    stats.WeekGames = 0;
    stats.WeekWins = 0;
    stats.SeasonGames = 0;
    stats.SeasonWins = 0;
    stats.PreviousOpponent = 0;

    SaveStats(guid, stats, /*insert*/ true);
    return stats;
}

void Arena1v1Mgr::SaveStats(ObjectGuid guid, Arena1v1Stats const& stats, bool insert) const
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(insert ? CHAR_INS_ARENA_1V1 : CHAR_UPD_ARENA_1V1);
    if (insert)
    {
        stmt->setUInt32(0, guid.GetCounter());
        stmt->setUInt16(1, stats.Rating);
        stmt->setUInt16(2, stats.MatchMakerRating);
        stmt->setUInt16(3, stats.WeekGames);
        stmt->setUInt16(4, stats.WeekWins);
        stmt->setUInt16(5, stats.SeasonGames);
        stmt->setUInt16(6, stats.SeasonWins);
        stmt->setUInt32(7, stats.PreviousOpponent);
    }
    else
    {
        stmt->setUInt16(0, stats.Rating);
        stmt->setUInt16(1, stats.MatchMakerRating);
        stmt->setUInt16(2, stats.WeekGames);
        stmt->setUInt16(3, stats.WeekWins);
        stmt->setUInt16(4, stats.SeasonGames);
        stmt->setUInt16(5, stats.SeasonWins);
        stmt->setUInt32(6, stats.PreviousOpponent);
        stmt->setUInt32(7, guid.GetCounter());
    }
    CharacterDatabase.Execute(stmt);
}

namespace
{
    uint32 EncodeAteamId(ObjectGuid guid)
    {
        return 0x80000000u | guid.GetCounter();
    }

    uint32 EncodeAteamId(uint32 guidLow)
    {
        return guidLow ? (0x80000000u | guidLow) : 0u;
    }

    BattlegroundQueueTypeId Build1v1QueueTypeId(uint8 bracketId)
    {
        return BattlegroundMgr::BGQueueTypeId(BATTLEGROUND_AA, bracketId, ARENA_TYPE_1V1);
    }
}

bool Arena1v1Mgr::HandleJoinQueue(Player* player, Creature* npc)
{
    if (!player || !npc)
        return false;

    WorldSession* session = player->GetSession();

    if (!IsEnabled())
    {
        ChatHandler(session).PSendSysMessage(LANG_ARENA_DISABLED);
        return false;
    }

    if (player->InBattleground())
    {
        npc->Whisper("You are already in a battleground or arena.", LANG_UNIVERSAL, player);
        return false;
    }

    // EG - Hardcore
    if (player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD))
    {
        npc->Whisper("The dead may not enter the arena.", LANG_UNIVERSAL, player);
        return false;
    }

    if (player->isUsingLfg())
    {
        WorldPackets::Battleground::BattlefieldStatusFailed status;
        BattlegroundMgr::BuildBattlegroundStatusFailed(&status, ERR_LFG_CANT_USE_BATTLEGROUND);
        session->SendPacket(status.Write());
        return false;
    }

    if (player->GetGroup() && player->GetGroup()->GetMembersCount() > 1)
    {
        npc->Whisper("You must leave your party before queueing for 1v1 arena.", LANG_UNIVERSAL, player);
        return false;
    }

    if (player->HasAura(9454))
    {
        npc->Whisper("You cannot queue while frozen.", LANG_UNIVERSAL, player);
        return false;
    }

    if (player->GetLevel() < sWorld->getIntConfig(CONFIG_ARENA_1V1_MIN_LEVEL))
    {
        npc->Whisper(Trinity::StringFormat("You must be at least level {} to queue for 1v1 arena.",
            sWorld->getIntConfig(CONFIG_ARENA_1V1_MIN_LEVEL)).c_str(), LANG_UNIVERSAL, player);
        return false;
    }

    Battleground* bgTpl = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    if (!bgTpl)
        return false;

    if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_AA, nullptr))
    {
        ChatHandler(session).PSendSysMessage(LANG_ARENA_DISABLED);
        return false;
    }

    if (!player->CanJoinToBattleground(bgTpl))
    {
        WorldPackets::Battleground::BattlefieldStatusFailed status;
        BattlegroundMgr::BuildBattlegroundStatusFailed(&status, ERR_BATTLEGROUND_JOIN_FAILED);
        session->SendPacket(status.Write());
        return false;
    }

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bgTpl->GetMapId(), player->GetLevel());
    if (!bracketEntry)
        return false;

    BattlegroundQueueTypeId qid = Build1v1QueueTypeId(bracketEntry->GetBracketId());

    if (player->GetBattlegroundQueueIndex(qid) < PLAYER_MAX_BATTLEGROUND_QUEUES)
    {
        npc->Whisper("You are already in the 1v1 queue.", LANG_UNIVERSAL, player);
        return false;
    }
    if (!player->HasFreeBattlegroundQueueId())
    {
        npc->Whisper("You are already in too many battleground queues.", LANG_UNIVERSAL, player);
        return false;
    }

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(qid);

    // EG - anti-exploit
    if (sWorld->getBoolConfig(CONFIG_ARENA_1V1_SAME_IP_CHECK))
    {
        std::string const& joinerIp = session->GetRemoteAddress();
        if (!joinerIp.empty())
        {
            for (auto const& [otherGuid, pinfo] : bgQueue.m_QueuedPlayers)
            {
                if (otherGuid == player->GetGUID())
                    continue;
                Player* other = ObjectAccessor::FindConnectedPlayer(otherGuid);
                if (!other)
                    continue;
                if (other->GetSession()->GetRemoteAddress() == joinerIp)
                {
                    npc->Whisper("Another character from your IP is already queued for 1v1 arena.", LANG_UNIVERSAL, player);
                    return false;
                }
            }
        }
    }

    Arena1v1Stats& stats = GetOrCreate(player->GetGUID());
    uint32 arenaRating = stats.Rating ? stats.Rating : 1;
    uint32 mmr = stats.MatchMakerRating;
    uint32 ateamId = EncodeAteamId(player->GetGUID());
    uint32 previousOpponents = EncodeAteamId(stats.PreviousOpponent);

    bgTpl->SetRated(true);

    GroupQueueInfo* ginfo = bgQueue.AddGroup(player, nullptr, bracketEntry, /*isRated*/ true, false,
                                             arenaRating, mmr, ateamId, previousOpponents);
    uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo);
    uint32 queueSlot = player->AddBattlegroundQueueId(qid);

    WorldPackets::Battleground::BattlefieldStatusQueued status;
    BattlegroundMgr::BuildBattlegroundStatusQueued(&status, bgTpl, queueSlot, ginfo->JoinTime, qid, avgTime);
    session->SendPacket(status.Write());

    sBattlegroundMgr->ScheduleQueueUpdate(mmr, qid);

    TC_LOG_DEBUG("bg.battleground", "Arena1v1: player {} ({}) joined 1v1 queue, bracket {}, rating {}, mmr {}",
        player->GetName(), player->GetGUID().ToString(), uint32(bracketEntry->GetBracketId()), arenaRating, mmr);
    return true;
}

bool Arena1v1Mgr::HandleLeaveQueue(Player* player)
{
    if (!player)
        return false;

    Battleground* bgTpl = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    if (!bgTpl)
        return false;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bgTpl->GetMapId(), player->GetLevel());
    if (!bracketEntry)
        return false;

    BattlegroundQueueTypeId qid = Build1v1QueueTypeId(bracketEntry->GetBracketId());
    uint32 queueSlot = player->GetBattlegroundQueueIndex(qid);
    if (queueSlot >= PLAYER_MAX_BATTLEGROUND_QUEUES)
        return false;

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(qid);

    GroupQueueInfo ginfo;
    if (!bgQueue.GetPlayerGroupInfoData(player->GetGUID(), &ginfo))
        return false;

    WorldPackets::Battleground::BattlefieldStatusNone none;
    BattlegroundMgr::BuildBattlegroundStatusNone(&none, queueSlot);
    player->GetSession()->SendPacket(none.Write());

    player->RemoveBattlegroundQueueId(qid);
    bgQueue.RemovePlayer(player->GetGUID(), true);
    return true;
}

void Arena1v1Mgr::WhisperStats(Player* player, Creature* npc)
{
    if (!player || !npc)
        return;

    Arena1v1Stats const* stats = TryGetStats(player->GetGUID());
    if (!stats)
    {
        npc->Whisper("You have no 1v1 arena history yet. Queue once to be ranked.", LANG_UNIVERSAL, player);
        return;
    }

    std::string line;
    line = Trinity::StringFormat("1v1 rating: {} (MMR {})", stats->Rating, stats->MatchMakerRating);
    npc->Whisper(line.c_str(), LANG_UNIVERSAL, player);

    line = Trinity::StringFormat("This week: {} games, {} wins.", stats->WeekGames, stats->WeekWins);
    npc->Whisper(line.c_str(), LANG_UNIVERSAL, player);

    line = Trinity::StringFormat("This season: {} games, {} wins.", stats->SeasonGames, stats->SeasonWins);
    npc->Whisper(line.c_str(), LANG_UNIVERSAL, player);
}

void Arena1v1Mgr::CancelQueueIfPresent(ObjectGuid guid)
{
    Player* player = ObjectAccessor::FindConnectedPlayer(guid);
    if (!player)
        return;

    if (player->InBattleground())
        return;

    Battleground* bgTpl = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
    if (!bgTpl)
        return;

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bgTpl->GetMapId(), player->GetLevel());
    if (!bracketEntry)
        return;

    BattlegroundQueueTypeId qid = Build1v1QueueTypeId(bracketEntry->GetBracketId());
    uint32 queueSlot = player->GetBattlegroundQueueIndex(qid);
    if (queueSlot >= PLAYER_MAX_BATTLEGROUND_QUEUES)
        return;

    BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(qid);

    WorldPackets::Battleground::BattlefieldStatusNone none;
    BattlegroundMgr::BuildBattlegroundStatusNone(&none, queueSlot);
    player->GetSession()->SendPacket(none.Write());

    player->RemoveBattlegroundQueueId(qid);
    bgQueue.RemovePlayer(guid, true);
}

float Arena1v1Mgr::GetChanceAgainst(uint32 ownRating, uint32 opponentRating)
{
    return 1.0f / (1.0f + std::exp(std::log(10.0f) * (float(opponentRating) - float(ownRating)) / 650.0f));
}

int32 Arena1v1Mgr::GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won)
{
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float wonMod = won ? 1.0f : 0.0f;
    float mod = (wonMod - chance) * sWorld->getFloatConfig(CONFIG_ARENA_MATCHMAKER_RATING_MODIFIER);
    return int32(std::ceil(mod));
}

int32 Arena1v1Mgr::GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won)
{
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float mod;

    if (won)
    {
        if (ownRating < 1300)
        {
            float win1 = sWorld->getFloatConfig(CONFIG_ARENA_WIN_RATING_MODIFIER_1);
            if (ownRating < 1000)
                mod = win1 * (1.0f - chance);
            else
                mod = ((win1 / 2.0f) + ((win1 / 2.0f) * (1300.0f - float(ownRating)) / 300.0f)) * (1.0f - chance);
        }
        else
            mod = sWorld->getFloatConfig(CONFIG_ARENA_WIN_RATING_MODIFIER_2) * (1.0f - chance);
    }
    else
        mod = sWorld->getFloatConfig(CONFIG_ARENA_LOSE_RATING_MODIFIER) * (-chance);

    return int32(std::ceil(mod));
}

void Arena1v1Mgr::ApplyClampedDelta(uint16& field, int32 delta)
{
    int32 v = int32(field) + delta;
    if (v < 0)
        v = 0;
    if (v > 0xFFFF)
        v = 0xFFFF;
    field = uint16(v);
}

Arena1v1MatchOutcome Arena1v1Mgr::ApplyMatchResult(ObjectGuid winnerGuid, ObjectGuid loserGuid,
                                                  uint32 winnerMmrSeen, uint32 loserMmrSeen)
{
    Arena1v1Stats& winner = GetOrCreate(winnerGuid);
    Arena1v1Stats& loser = GetOrCreate(loserGuid);

    Arena1v1MatchOutcome outcome;
    outcome.WinnerRatingDelta = GetRatingMod(winner.Rating, loserMmrSeen, true);
    outcome.LoserRatingDelta = GetRatingMod(loser.Rating, winnerMmrSeen, false);
    outcome.WinnerMmrSnapshot = uint16(winnerMmrSeen);
    outcome.LoserMmrSnapshot = uint16(loserMmrSeen);

    int32 winnerMmrDelta = GetMatchmakerRatingMod(winnerMmrSeen, loserMmrSeen, true);
    int32 loserMmrDelta = GetMatchmakerRatingMod(loserMmrSeen, winnerMmrSeen, false);

    ApplyClampedDelta(winner.Rating, outcome.WinnerRatingDelta);
    ApplyClampedDelta(winner.MatchMakerRating, winnerMmrDelta);
    winner.WeekGames += 1;
    winner.WeekWins += 1;
    winner.SeasonGames += 1;
    winner.SeasonWins += 1;
    winner.PreviousOpponent = loserGuid.GetCounter();

    ApplyClampedDelta(loser.Rating, outcome.LoserRatingDelta);
    ApplyClampedDelta(loser.MatchMakerRating, loserMmrDelta);
    loser.WeekGames += 1;
    loser.SeasonGames += 1;
    loser.PreviousOpponent = winnerGuid.GetCounter();

    SaveStats(winnerGuid, winner, /*insert*/ false);
    SaveStats(loserGuid, loser, /*insert*/ false);

    return outcome;
}

Arena1v1MatchOutcome Arena1v1Mgr::ApplyForfeit(ObjectGuid leaverGuid, ObjectGuid opponentGuid, uint32 opponentMmrSeen)
{
    if (!opponentGuid.IsEmpty())
    {
        Arena1v1Stats const& leaver = GetOrCreate(leaverGuid);
        uint32 leaverMmrSnapshot = leaver.MatchMakerRating;
        return ApplyMatchResult(opponentGuid, leaverGuid, opponentMmrSeen, leaverMmrSnapshot);
    }

    Arena1v1Stats& leaver = GetOrCreate(leaverGuid);
    int32 mod = GetRatingMod(leaver.Rating, opponentMmrSeen, false);
    ApplyClampedDelta(leaver.Rating, mod);
    leaver.WeekGames += 1;
    leaver.SeasonGames += 1;

    SaveStats(leaverGuid, leaver, /*insert*/ false);

    Arena1v1MatchOutcome voided{};
    voided.LoserRatingDelta = mod;
    voided.LoserMmrSnapshot = leaver.MatchMakerRating;
    return voided;
}

void Arena1v1Mgr::ApplyDraw(ObjectGuid playerA, ObjectGuid playerB)
{
    constexpr int32 DRAW_RATING_LOSS = -16;

    Arena1v1Stats& a = GetOrCreate(playerA);
    Arena1v1Stats& b = GetOrCreate(playerB);

    ApplyClampedDelta(a.Rating, DRAW_RATING_LOSS);
    a.WeekGames += 1;
    a.SeasonGames += 1;
    a.PreviousOpponent = playerB.GetCounter();

    ApplyClampedDelta(b.Rating, DRAW_RATING_LOSS);
    b.WeekGames += 1;
    b.SeasonGames += 1;
    b.PreviousOpponent = playerA.GetCounter();

    SaveStats(playerA, a, /*insert*/ false);
    SaveStats(playerB, b, /*insert*/ false);
}

uint32 Arena1v1Mgr::ComputeWeeklyPoints(uint16 personalRating) const
{
    // Mirrors ArenaTeam::GetPoints rating curve, with the 1v1-specific multiplier
    float points;
    uint32 rating = personalRating;

    if (rating <= 1500)
    {
        if (sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID) < 6)
            points = float(rating) * 0.22f + 14.0f;
        else
            points = 344.0f;
    }
    else
        points = 1511.26f / (1.0f + 1639.28f * std::exp(-0.00412f * float(rating)));

    points *= sWorld->getFloatConfig(CONFIG_ARENA_1V1_POINTS_MULTI);
    points *= sWorld->getRate(RATE_ARENA_POINTS);

    return uint32(points);
}

void Arena1v1Mgr::AccumulateWeeklyPoints(std::map<ObjectGuid, uint32>& playerPoints)
{
    if (!IsEnabled())
        return;

    for (auto const& [guid, stats] : _store)
    {
        if (stats.WeekGames < 10)
            continue;

        uint32 pointsToAdd = ComputeWeeklyPoints(stats.Rating);

        auto it = playerPoints.find(guid);
        if (it != playerPoints.end())
        {
            if (it->second < pointsToAdd)
                playerPoints[guid] = pointsToAdd;
        }
        else
            playerPoints[guid] = pointsToAdd;
    }
}

void Arena1v1Mgr::DeleteCachedStats(ObjectGuid guid)
{
    _store.erase(guid);
}

void Arena1v1Mgr::FinishWeek()
{
    for (auto& [guid, stats] : _store)
    {
        if (stats.WeekGames == 0 && stats.WeekWins == 0)
            continue;
        stats.WeekGames = 0;
        stats.WeekWins = 0;
        SaveStats(guid, stats, /*insert*/ false);
    }
}
