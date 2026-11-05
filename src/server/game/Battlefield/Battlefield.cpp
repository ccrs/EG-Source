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

#include "Battlefield.h"
#include "BattlefieldEntities.h"
#include "BattlegroundPackets.h"
#include "Containers.h"
#include "DBCStores.h"
#include "Errors.h"
#include "GameTime.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Log.h"
#include "MapManager.h"
#include "MapReference.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "WorldStatePackets.h"
#include <algorithm>

Battlefield::Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId) : _battleId(battleId), _zoneId(zoneId), _phase(BATTLEFIELD_PHASE_IDLE), _flags(BATTLEFIELD_FLAG_NONE)
{
    Values.Set(BATTLEFIELD_VALUE_ID_MAP, uint32(0));
    Values.Set(BATTLEFIELD_VALUE_STATE_CONTROLLING_TEAM, PVP_TEAM_NEUTRAL);
    Values.Set(BATTLEFIELD_VALUE_INTERVAL_RESURRECTION, 30s);
    Values.Set(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT, BATTLEFIELD_BATTLE_RESULT_NONE);
    Values.Set(BATTLEFIELD_VALUE_LIMIT_TEAM_PLAYERS, uint32(0));
    Values.Set(BATTLEFIELD_VALUE_LIMIT_PLAYER_LEVEL, uint32(0));
    Values.Set(BATTLEFIELD_VALUE_DURATION_BATTLE_INVITE, 20s);
    Values.Set(BATTLEFIELD_VALUE_DELAY_BATTLE_LOGOUT_LEAVE, 30s);
    Values.Set(BATTLEFIELD_VALUE_DELAY_BATTLE_FILL, 30s);
    Values.Set(BATTLEFIELD_VALUE_DELAY_BATTLE_STATUS_CLEAR, 30s);
}

Battlefield::~Battlefield()
{
}

bool Battlefield::SetupBattlefield()
{
    ScheduleNextGraveyardResurrect();
    return true;
}

void Battlefield::ChangeTeams(PvPTeamId newControllingTeam)
{
    Values.Set(BATTLEFIELD_VALUE_STATE_CONTROLLING_TEAM, newControllingTeam);

    for (BattlefieldGraveyardContainer::value_type& graveyardPair : _graveyards)
        graveyardPair.second->InitializeState();
}

bool Battlefield::SetPhase(BattlefieldPhase phase)
{
    if (phase != BATTLEFIELD_PHASE_IDLE && (!IsEnabled() || _flags.HasFlag(BATTLEFIELD_FLAG_RAID_RELEASE_PENDING)))
        return false;

    BattlefieldPhase const oldPhase = _phase;
    if (phase == oldPhase)
        return true;

    bool wasBattlefieldRaidPhase = IsBattlefieldRaidPhase();
    _phase = phase;
    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_START, GameTime::GetGameTime());
    if (wasBattlefieldRaidPhase && !IsBattlefieldRaidPhase())
    {
        _flags |= BATTLEFIELD_FLAG_RAID_RELEASE_PENDING;
        QueueEvent<BattlefieldRaidReleaseEvent>();
    }

    if (phase != BATTLEFIELD_PHASE_IDLE)
    {
        RemoveEvents(BATTLEFIELD_EVENT_BATTLE_STATUS_CLEAR, ObjectGuid::Empty);
        ClearBattleStatus();
    }

    if (phase == BATTLEFIELD_PHASE_BATTLE)
    {
        time_t const start = std::min(GameTime::GetGameTime(), Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START));
        Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END, start + Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_BATTLE).count());
        Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START, start + Values.Get<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_BATTLE_START).count());
        Values.Set(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT, BATTLEFIELD_BATTLE_RESULT_NONE);
    }
    else if (oldPhase == BATTLEFIELD_PHASE_BATTLE)
    {
        for (TeamId teamId : { TEAM_ALLIANCE, TEAM_HORDE })
        {
            for (ObjectGuid playerGuid : _battlefieldPlayers.List(teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE))
            {
                _battlefieldPlayers.Insert(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_BATTLE_ENDED);
                if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                    player->GetSession()->SendBfStateChange(GetId(), BATTLEFIELD_PLAYER_STATUS_BATTLE_ENDED);
            }
        }

        QueueEvent<BattlefieldBattleStatusClearEvent>(ObjectGuid::Empty, Values.Get<Seconds>(BATTLEFIELD_VALUE_DELAY_BATTLE_STATUS_CLEAR));
        Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END, time_t(0));

        time_t const now = GameTime::GetGameTime();
        time_t const interval = Values.Get<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_BATTLE_START).count();
        time_t const preparationStartTime = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START) - Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION).count();
        if (preparationStartTime < now)
            Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START, Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START) + ((now - preparationStartTime) / interval + 1) * interval);

        if (Values.Get<BattlefieldBattleResult>(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT) == BATTLEFIELD_BATTLE_RESULT_CAPTURED)
            ChangeTeams(GetAttackingTeam());
    }

    if (phase == BATTLEFIELD_PHASE_IDLE)
    {
        for (TeamId teamId : { TEAM_ALLIANCE, TEAM_HORDE })
        {
            for (BattlefieldPlayerState state : { BATTLEFIELD_PLAYER_STATE_QUEUED, BATTLEFIELD_PLAYER_STATE_INVITED })
            {
                for (ObjectGuid playerGuid : _battlefieldPlayers.List(teamId, state))
                    if (Player* player = ObjectAccessor::FindConnectedPlayer(playerGuid))
                        player->GetSession()->SendBfLeaveMessage(GetId(), BATTLEFIELD_LEAVE_REASON_CLOSE, GetPlayerStatus(playerGuid, teamId));

                _battlefieldPlayers.EraseAll(teamId, state);
            }
        }

        RemoveEvents([](BattlefieldEvent const& event) -> bool
        {
            return event.GetType() == BATTLEFIELD_EVENT_BATTLE_EXPIRE || event.GetType() == BATTLEFIELD_EVENT_BATTLE_LEAVE || event.GetType() == BATTLEFIELD_EVENT_BATTLE_FILL;
        });
    }

    SendPhaseTimes();

    if (phase == BATTLEFIELD_PHASE_PREPARATION)
    {
        Map* battleMap = GetBattleMap();
        for (ObjectGuid playerGuid : _playerGUIDs)
        {
            Player* player = ObjectAccessor::GetPlayer(battleMap, playerGuid);
            if (!player || !CanJoinBattle(player))
                continue;

            player->GetSession()->SendBfInvitePlayerToQueue(GetId());
            player->GetSession()->SendBfStateChange(GetId(), BATTLEFIELD_PLAYER_STATUS_QUEUED);
        }
    }
    else if (phase == BATTLEFIELD_PHASE_BATTLE)
    {
        FillBattleSlots(true);
        for (ObjectGuid playerGuid : _playerGUIDs)
            QueueEvent<BattlefieldBattleInviteEvent>(playerGuid);
    }

    QueueEvent<BattlefieldZoneRefreshEvent>();
    ScheduleNextPhaseAdvance();
    return true;
}

void Battlefield::SetPhaseTime(time_t phaseTime)
{
    Values.Set(IsBattleTime() ? BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END : BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START, std::max(phaseTime, GameTime::GetGameTime()));
    SendPhaseTimes();
    ScheduleNextPhaseAdvance();
}

void Battlefield::Update(uint32 /*diff*/)
{
    TimePoint const now = GameTime::Now();
    if (!_events.empty() && _events.begin()->first <= now)
    {
        ASSERT(!_flags.HasFlag(BATTLEFIELD_FLAG_EVENTS_UPDATING));
        _flags |= BATTLEFIELD_FLAG_EVENTS_UPDATING;

        BattlefieldEventContainer::iterator const last = std::prev(_events.upper_bound(now));
        for (BattlefieldEventContainer::iterator itr = _events.begin();;)
        {
            bool const isLast = itr == last;
            if (!itr->second->IsCancelled())
                itr->second->Execute(*this);
            itr = _events.erase(itr);
            if (isLast)
                break;
        }

        _flags.RemoveFlag(BATTLEFIELD_FLAG_EVENTS_UPDATING);
        if (_flags.HasFlag(BATTLEFIELD_FLAG_EVENTS_CLEANUP_PENDING))
        {
            std::erase_if(_events, [](BattlefieldEventContainer::value_type const& pair) -> bool
            {
                return pair.second->IsCancelled();
            });
            _flags.RemoveFlag(BATTLEFIELD_FLAG_EVENTS_CLEANUP_PENDING);
        }
    }
}

void Battlefield::HandlePlayerEnterZone(Player* player)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    std::vector<ObjectGuid> const& raid = _battlefieldRaids[teamId];
    Group* group = player->GetGroup();
    bool const inBattlefieldRaid = group && std::ranges::find(raid, group->GetGUID()) != raid.end();
    if (inBattlefieldRaid)
        RemoveEvents(BATTLEFIELD_EVENT_RAID_REMOVE, playerGuid);

    if (IsBattleTime() && _battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE))
    {
        RemoveEvents(BATTLEFIELD_EVENT_BATTLE_LEAVE, playerGuid);
        if (!inBattlefieldRaid)
        {
            player->GetSession()->SendBfEntered(GetId(), false, teamId == GetAttackingTeamId());
            QueueEvent<BattlefieldRaidAddEvent>(playerGuid);
        }
    }
    else if (IsInPreparation())
        QueueEvent<BattlefieldQueueInviteEvent>(playerGuid);
    else if (IsBattleTime())
        QueueEvent<BattlefieldBattleInviteEvent>(playerGuid);

    _playerGUIDs.insert(playerGuid);
}

void Battlefield::HandlePlayerLeaveZone(Player* player)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    bool const logout = player->GetSession()->PlayerLogout();
    bool const leavingBattle = IsBattleTime() && _battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE);
    RemovePlayerFromResurrectionQueue(player);
    if (IsEnabled() && !logout && !leavingBattle)
        player->GetSession()->SendBfLeaveMessage(GetId(), BATTLEFIELD_LEAVE_REASON_ZONE_EXIT, GetPlayerStatus(playerGuid, teamId));

    if (leavingBattle)
        QueueEvent<BattlefieldBattleLeaveEvent>(playerGuid, logout ? Values.Get<Seconds>(BATTLEFIELD_VALUE_DELAY_BATTLE_LOGOUT_LEAVE) : 0s);
    else
        QueueEvent<BattlefieldRaidRemoveEvent>(playerGuid);

    _playerGUIDs.erase(playerGuid);
}

void Battlefield::SendAreaSpiritHealerQueryOpcode(Player* player, ObjectGuid source)
{
    WorldPackets::Battleground::AreaSpiritHealerTime areaSpiritHealerTime;
    areaSpiritHealerTime.HealerGuid = source;
    TimePoint const now = GameTime::Now();
    TimePoint const nextResurrectionTime = Values.Get<TimePoint>(BATTLEFIELD_VALUE_TIME_NEXT_RESURRECTION);
    areaSpiritHealerTime.TimeLeft = nextResurrectionTime > now ? uint32(std::chrono::duration_cast<Milliseconds>(nextResurrectionTime - now).count()) : 0;
    player->SendDirectMessage(areaSpiritHealerTime.Write());
}

void Battlefield::AddPlayerToResurrectionQueue(Player* player, ObjectGuid source)
{
    auto itr = std::find_if(_graveyards.begin(), _graveyards.end(), [&](BattlefieldGraveyardContainer::value_type const& pair) -> bool
    {
        return pair.second->ValidateObjectGUID(source);
    });
    if (itr != _graveyards.end())
        itr->second->AddPlayerToResurrectionQueue(player);
}

void Battlefield::RemovePlayerFromResurrectionQueue(Player* player)
{
    for (BattlefieldGraveyardContainer::value_type const& pair : _graveyards)
        pair.second->RemovePlayerFromResurrectionQueue(player);
}

void Battlefield::EmplaceGraveyard(uint8 id, BattlefieldGraveyardPointer&& pointer)
{
    _graveyards.emplace(id, std::move(pointer));
}

void Battlefield::SendInitWorldStatesTo(Player* player)
{
    WorldPackets::WorldState::InitWorldStates packet;
    packet.MapID = Values.Get<uint32>(BATTLEFIELD_VALUE_ID_MAP);
    packet.AreaID = _zoneId;
    packet.SubareaID = player->GetAreaId();
    FillInitialWorldStates(packet);

    player->SendDirectMessage(packet.Write());
}

void Battlefield::EmplaceEvent(std::unique_ptr<BattlefieldEvent>&& event)
{
    if (event->GetQueuePolicy() == BATTLEFIELD_EVENT_QUEUE_REPLACE)
        RemoveEvents(event->GetType(), event->GetObjectGUID());

    TimePoint const due = GameTime::Now() + event->GetDelay();
    _events.emplace(due, std::move(event));
}

void Battlefield::RemoveEvents(std::function<bool(BattlefieldEvent const&)> const& predicate)
{
    if (!_flags.HasFlag(BATTLEFIELD_FLAG_EVENTS_UPDATING))
    {
        std::erase_if(_events, [&predicate](BattlefieldEventContainer::value_type const& pair) -> bool
        {
            return predicate(*pair.second);
        });
        return;
    }

    for (BattlefieldEventContainer::value_type& pair : _events)
    {
        if (pair.second->IsCancelled() || !predicate(*pair.second))
            continue;

        pair.second->Cancel();
        _flags |= BATTLEFIELD_FLAG_EVENTS_CLEANUP_PENDING;
    }
}

void Battlefield::RemoveEvents(BattlefieldEventType type, ObjectGuid objectGuid)
{
    RemoveEvents([type, objectGuid](BattlefieldEvent const& event) -> bool
    {
        return event.GetType() == type && event.GetObjectGUID() == objectGuid;
    });
}

void Battlefield::ProcessRaidRelease(BattlefieldEventKey /*key*/)
{
    _flags.RemoveFlag(BATTLEFIELD_FLAG_RAID_RELEASE_PENDING);
    for (std::vector<ObjectGuid>& raid : _battlefieldRaids)
    {
        for (ObjectGuid groupGuid : raid)
        {
            Group* group = sGroupMgr->GetGroupByGUID(groupGuid);
            if (!group)
                continue;

            if (group->GetMembersCount() < 2)
                group->Disband();
            else
                group->ConvertFromBattlefieldGroup();
        }
        raid.clear();
    }
}

void Battlefield::ProcessRaidAdd(BattlefieldEventKey /*key*/, Player* player)
{
    if (!IsBattlefieldRaidPhase() || !player->IsInWorld())
        return;

    TeamId teamId = player->GetTeamId();
    std::vector<ObjectGuid>& raid = _battlefieldRaids[teamId];
    for (ObjectGuid groupGuid : raid)
    {
        Group* group = sGroupMgr->GetGroupByGUID(groupGuid);
        if (!group || !group->IsMember(player->GetGUID()) || player->GetGroup() == group)
            continue;

        player->SetBattlegroundOrBattlefieldRaid(group, group->GetMemberGroup(player->GetGUID()));
        group->SendUpdate();
        return;
    }

    if (Group* group = player->GetGroup())
    {
        if (std::ranges::find(raid, group->GetGUID()) != raid.end())
            return;

        if (group->isBGGroup())
            return;

        group->RemoveMember(player->GetGUID(), GROUP_REMOVEMETHOD_LEAVE);
        if (player->GetGroup() || !player->IsInWorld())
            return;
    }

    if (Group* invite = player->GetGroupInvite())
    {
        if (invite->IsCreated())
            invite->RemoveInvite(player);
        else if (invite->GetLeaderGUID() == player->GetGUID())
        {
            invite->RemoveAllInvites();
            delete invite;
        }
        else
            player->UninviteFromGroup();
    }

    Group* currentGroup = nullptr;
    for (auto itr = raid.begin(); itr != raid.end();)
    {
        Group* group = sGroupMgr->GetGroupByGUID(*itr);
        if (!group)
        {
            itr = raid.erase(itr);
            continue;
        }

        if (!group->IsFull())
        {
            currentGroup = group;
            break;
        }

        ++itr;
    }

    if (!currentGroup || !currentGroup->AddMember(player))
    {
        currentGroup = new Group();
        currentGroup->SetBattlefieldGroup(this);
        currentGroup->Create(player);
        sGroupMgr->AddGroup(currentGroup);
        raid.push_back(currentGroup->GetGUID());
    }
}

void Battlefield::ProcessRaidRemove(BattlefieldEventKey /*key*/, ObjectGuid playerGuid)
{
    for (std::vector<ObjectGuid> const& raid : _battlefieldRaids)
    {
        for (ObjectGuid groupGuid : raid)
        {
            Group* group = sGroupMgr->GetGroupByGUID(groupGuid);
            if (!group || !group->IsMember(playerGuid))
                continue;

            group->RemoveMember(playerGuid);
            return;
        }
    }
}

void Battlefield::ProcessPhaseAdvance(BattlefieldEventKey /*key*/)
{
    time_t const now = GameTime::GetGameTime();
    time_t const nextBattleStartTime = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START);
    switch (_phase)
    {
        case BATTLEFIELD_PHASE_IDLE:
            if (now >= nextBattleStartTime - Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION).count() && SetPhase(BATTLEFIELD_PHASE_PREPARATION))
                return;
            break;
        case BATTLEFIELD_PHASE_PREPARATION:
            if (now >= nextBattleStartTime && SetPhase(BATTLEFIELD_PHASE_BATTLE))
                return;
            break;
        case BATTLEFIELD_PHASE_BATTLE:
            if (now >= Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END))
            {
                if (Values.Get<BattlefieldBattleResult>(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT) == BATTLEFIELD_BATTLE_RESULT_NONE)
                    Values.Set(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT, BATTLEFIELD_BATTLE_RESULT_DEFENDED);
                SetPhase(BATTLEFIELD_PHASE_IDLE);
                return;
            }
            break;
        default:
            break;
    }

    ScheduleNextPhaseAdvance();
}

void Battlefield::ProcessGraveyardResurrect(BattlefieldEventKey /*key*/)
{
    for (BattlefieldGraveyardContainer::value_type& pair : _graveyards)
        pair.second->ResurrectPlayers();

    ScheduleNextGraveyardResurrect();
}

void Battlefield::ProcessZoneRefresh(BattlefieldEventKey /*key*/)
{
    Map* battleMap = GetBattleMap();
    for (auto itr = _playerGUIDs.begin(); itr != _playerGUIDs.end();)
    {
        if (Player* player = ObjectAccessor::GetPlayer(battleMap, *itr))
        {
            if (player->IsInWorld())
            {
                player->UpdateAreaDependentAuras(player->GetAreaId());
                player->UpdateZoneDependentAuras(player->GetZoneId());
                SendInitWorldStatesTo(player);
            }
            ++itr;
        }
        else
            itr = _playerGUIDs.erase(itr);
    }
}

void Battlefield::ProcessQueueInvite(BattlefieldEventKey /*key*/, Player* player)
{
    if (!IsInPreparation() || !CanJoinBattle(player) || _battlefieldPlayers.FindState(player->GetGUID(), player->GetTeamId()))
        return;

    player->GetSession()->SendBfInvitePlayerToQueue(GetId());
}

void Battlefield::ProcessBattleInvite(BattlefieldEventKey /*key*/, Player* player)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    if (!IsBattleTime() || !_playerGUIDs.contains(playerGuid) || !CanJoinBattle(player))
        return;

    if (_battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_INVITED) || _battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE))
        return;

    if (HasBattleSlot(teamId))
        InviteToBattle(player, false);
    else
        _battlefieldPlayers.Insert(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_QUEUED);
}

void Battlefield::ProcessBattleAccept(BattlefieldEventKey key, Player* player)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    if (!IsBattleTime() || player->InArena() || !_battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_INVITED))
        return;

    if (!player->IsInWorld())
    {
        QueueEvent<BattlefieldBattleAcceptEvent>(playerGuid, 1s);
        return;
    }

    RemoveEvents(BATTLEFIELD_EVENT_BATTLE_EXPIRE, playerGuid);
    _battlefieldPlayers.Insert(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE);
    bool const relocated = !_playerGUIDs.contains(playerGuid);
    player->GetSession()->SendBfEntered(GetId(), relocated, teamId == GetAttackingTeamId());
    if (player->InBattleground())
        player->LeaveBattleground(false, true);

    RemoveEvents(BATTLEFIELD_EVENT_RAID_REMOVE, playerGuid);
    ProcessRaidAdd(key, player);
    if (!relocated)
        return;

    if (!player->IsAlive())
    {
        player->ResurrectPlayer(1.0f);
        player->SpawnCorpseBones();
    }

    player->FinishTaxiFlight();
    if (!TeleportPlayerToBattle(player))
        ProcessBattleLeave(key, playerGuid);
}

void Battlefield::ProcessBattleDecline(BattlefieldEventKey /*key*/, Player* player)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    if (!_battlefieldPlayers.Erase(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_INVITED))
        return;

    RemoveEvents(BATTLEFIELD_EVENT_BATTLE_EXPIRE, playerGuid);
    player->GetSession()->SendBfLeaveMessage(GetId(), BATTLEFIELD_LEAVE_REASON_INVITE_DECLINED, BATTLEFIELD_PLAYER_STATUS_INVITED);
    FillBattleSlots(false);
    _battlefieldPlayers.Insert(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_QUEUED);
    QueueEvent<BattlefieldBattleFillEvent>(ObjectGuid::Empty, Values.Get<Seconds>(BATTLEFIELD_VALUE_DELAY_BATTLE_FILL));
}

void Battlefield::ProcessBattleExpire(BattlefieldEventKey /*key*/, ObjectGuid playerGuid)
{
    Player* player = ObjectAccessor::FindConnectedPlayer(playerGuid);
    for (TeamId teamId : { TEAM_ALLIANCE, TEAM_HORDE })
    {
        if (!_battlefieldPlayers.Erase(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_INVITED))
            continue;

        if (player)
            player->GetSession()->SendBfLeaveMessage(GetId(), BATTLEFIELD_LEAVE_REASON_INVITE_EXPIRED, BATTLEFIELD_PLAYER_STATUS_INVITED);

        FillBattleSlots(false);
        if (player)
        {
            _battlefieldPlayers.Insert(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_QUEUED);
            QueueEvent<BattlefieldBattleFillEvent>(ObjectGuid::Empty, Values.Get<Seconds>(BATTLEFIELD_VALUE_DELAY_BATTLE_FILL));
        }

        return;
    }
}

void Battlefield::ProcessBattleLeave(BattlefieldEventKey key, ObjectGuid playerGuid)
{
    if (!IsBattleTime())
        return;

    for (TeamId teamId : { TEAM_ALLIANCE, TEAM_HORDE })
    {
        if (!_battlefieldPlayers.Erase(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE))
            continue;

        if (Player* player = ObjectAccessor::FindConnectedPlayer(playerGuid))
            player->GetSession()->SendBfLeaveMessage(GetId(), BATTLEFIELD_LEAVE_REASON_ZONE_EXIT, BATTLEFIELD_PLAYER_STATUS_NONE);

        ProcessRaidRemove(key, playerGuid);
        FillBattleSlots(false);
        return;
    }
}

void Battlefield::ProcessBattleFill(BattlefieldEventKey /*key*/)
{
    if (IsBattleTime())
        FillBattleSlots(false);
}

void Battlefield::ProcessBattleStatusClear(BattlefieldEventKey /*key*/)
{
    ClearBattleStatus();
}

void Battlefield::HandleBattlefieldRaidMemberRemoved(ObjectGuid /*playerGuid*/, RemoveMethod /*method*/)
{
}

void Battlefield::HandlePlayerLogout(Player* player)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    if (IsBattleTime() && _battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE))
    {
        QueueEvent<BattlefieldBattleLeaveEvent>(playerGuid, Values.Get<Seconds>(BATTLEFIELD_VALUE_DELAY_BATTLE_LOGOUT_LEAVE));
        return;
    }

    if (Group* group = player->GetGroup())
        if (group->GetBattlefield() == this)
            QueueEvent<BattlefieldRaidRemoveEvent>(playerGuid);

    RemoveFromQueue(playerGuid, teamId);
}

void Battlefield::HandlePlayerAddToMap(Player* player)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    if (IsBattleTime() && _battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE) && !_playerGUIDs.contains(playerGuid))
    {
        QueueEvent<BattlefieldBattleLeaveEvent>(playerGuid);
        return;
    }

    if (!_battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_QUEUED))
        return;

    player->GetSession()->SendBfQueueInviteResponse(GetId(), GetZoneId(), true, true, IsInPreparation());
    if (IsBattleTime())
        FillBattleSlots(false);
}

void Battlefield::HandleQueueRequest(Player* player)
{
    if (!IsBattlefieldRaidPhase() || !CanJoinBattle(player))
        return;

    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    if (_battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_INVITED) || _battlefieldPlayers.Contains(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE))
        return;

    if (IsBattleTime() && HasBattleSlot(teamId))
    {
        InviteToBattle(player, false);
        return;
    }

    _battlefieldPlayers.Insert(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_QUEUED);
    player->GetSession()->SendBfQueueInviteResponse(GetId(), GetZoneId(), true, false, IsInPreparation());
}

void Battlefield::HandleQueueInviteResponse(Player* player, bool accepted)
{
    if (accepted)
        HandleQueueRequest(player);
}

void Battlefield::HandleQueueExitRequest(Player* player)
{
    BattlefieldPlayerStatus const status = RemoveFromQueue(player->GetGUID(), player->GetTeamId());
    if (status == BATTLEFIELD_PLAYER_STATUS_QUEUED || status == BATTLEFIELD_PLAYER_STATUS_INVITED)
        player->GetSession()->SendBfLeaveMessage(GetId(), BATTLEFIELD_LEAVE_REASON_EXIT_REQUEST, status);
}

void Battlefield::HandleBattleInviteResponse(Player* player, bool accepted)
{
    if (accepted)
        QueueEvent<BattlefieldBattleAcceptEvent>(player->GetGUID());
    else
        QueueEvent<BattlefieldBattleDeclineEvent>(player->GetGUID());
}

void Battlefield::SetEnabled(bool enabled)
{
    if (enabled)
        _flags |= BATTLEFIELD_FLAG_ENABLED;
    else
    {
        SetPhase(BATTLEFIELD_PHASE_IDLE);
        _flags.RemoveFlag(BATTLEFIELD_FLAG_ENABLED);
    }
}

bool Battlefield::SetBattleResult(BattlefieldBattleResult result)
{
    if (!IsBattleTime())
        return false;

    Values.Set(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT, result);
    return true;
}

bool Battlefield::CanJoinBattle(Player const* player) const
{
    return !player->IsGameMaster() && player->GetLevel() >= Values.Get<uint32>(BATTLEFIELD_VALUE_LIMIT_PLAYER_LEVEL);
}

PvPTeamId Battlefield::GetAttackingTeam() const
{
    switch (GetControllingTeam())
    {
        case PVP_TEAM_HORDE:
            return PVP_TEAM_ALLIANCE;
        case PVP_TEAM_ALLIANCE:
            return PVP_TEAM_HORDE;
        case PVP_TEAM_NEUTRAL:
            return PVP_TEAM_NEUTRAL;
        default:
            break;
    }
    return PVP_TEAM_NEUTRAL;
}

TeamId Battlefield::GetControllingTeamId() const
{
    return TeamIdByPvPTeamId(GetControllingTeam());
}

TeamId Battlefield::GetAttackingTeamId() const
{
    return TeamIdByPvPTeamId(GetAttackingTeam());
}

BattlefieldGraveyard const* Battlefield::GetGraveyard(uint8 graveyardId) const
{
    BattlefieldGraveyardPointer const* graveyard = Trinity::Containers::MapGetValuePtr(_graveyards, graveyardId);
    return graveyard ? graveyard->get() : nullptr;
}

BattlefieldGraveyard* Battlefield::GetGraveyard(uint8 graveyardId)
{
    BattlefieldGraveyardPointer* graveyard = Trinity::Containers::MapGetValuePtr(_graveyards, graveyardId);
    return graveyard ? graveyard->get() : nullptr;
}

WorldSafeLocsEntry const* Battlefield::GetClosestGraveyardLocation(Player* player) const
{
    std::pair<float, WorldSafeLocsEntry const*> selection{ -1, nullptr };
    for (BattlefieldGraveyardContainer::value_type const& pair : _graveyards)
    {
        if (TeamIdByPvPTeamId(pair.second->GetPvPTeamId()) != player->GetTeamId())
            continue;
        WorldSafeLocsEntry const* safeLoc = sWorldSafeLocsStore.LookupEntry(pair.second->GetWorldSafeLocsEntryId());
        if (safeLoc == nullptr)
            continue;

        float dist = player->GetDistance2d(safeLoc->Loc.X, safeLoc->Loc.Y);
        if (dist < selection.first || selection.first < 0)
            selection = { dist, safeLoc };
    }

    return selection.second;
}

Creature* Battlefield::GetCreature(ObjectGuid guid) const
{
    if (!guid)
        return nullptr;
    if (Map* battleMap = GetBattleMap())
        return battleMap->GetCreature(guid);
    return nullptr;
}

GameObject* Battlefield::GetGameObject(ObjectGuid guid) const
{
    if (!guid)
        return nullptr;
    if (Map* battleMap = GetBattleMap())
        return battleMap->GetGameObject(guid);
    return nullptr;
}

void Battlefield::InitializePhaseTimes()
{
    Seconds const cycleDuration = Values.Get<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_BATTLE_START);
    Seconds const minimumCycleDuration = Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_BATTLE) + Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION);
    if (cycleDuration < minimumCycleDuration)
    {
        TC_LOG_ERROR("bg.battlefield", "Battlefield::InitializePhaseTimes: battlefield {} cycle of {}s is shorter than battle plus preparation, using {}s.", uint32(_battleId), cycleDuration.count(), minimumCycleDuration.count());
        Values.Set(BATTLEFIELD_VALUE_INTERVAL_PHASE_BATTLE_START, minimumCycleDuration);
    }

    time_t const now = GameTime::GetGameTime();
    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_START, now);
    if (Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END) > now || Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START) <= now)
        Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START, now + Values.Get<Seconds>(BATTLEFIELD_VALUE_DELAY_PHASE_BATTLE_RESTART).count());

    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END, time_t(0));
    SendPhaseTimes();
    ScheduleNextPhaseAdvance();
}

void Battlefield::SendWorldStateToAll(uint32 variable, uint32 value) const
{
    WorldPackets::WorldState::UpdateWorldState worldState;
    worldState.VariableID = variable;
    worldState.Value = value;
    sWorld->SendGlobalMessage(worldState.Write());
}

void Battlefield::SendWorldStateToMap(uint32 variable, uint32 value) const
{
    Map* battleMap = GetBattleMap();
    if (!battleMap)
        return;

    WorldPackets::WorldState::UpdateWorldState worldState;
    worldState.VariableID = variable;
    worldState.Value = value;
    WorldPacket const* packet = worldState.Write();
    for (MapReference const& reference : battleMap->GetPlayers())
        if (Player* player = reference.GetSource())
            player->SendDirectMessage(packet);
}

Map* Battlefield::GetBattleMap() const
{
    uint32 const mapId = Values.Get<uint32>(BATTLEFIELD_VALUE_ID_MAP);
    return mapId ? sMapMgr->FindMap(mapId, 0) : nullptr;
}

uint32 Battlefield::GetFreeBattleSlots(TeamId teamId) const
{
    uint32 const usedSlots = _battlefieldPlayers.Count(teamId, BATTLEFIELD_PLAYER_STATE_INVITED) + _battlefieldPlayers.Count(teamId, BATTLEFIELD_PLAYER_STATE_IN_BATTLE);
    uint32 const maxSlots = Values.Get<uint32>(BATTLEFIELD_VALUE_LIMIT_TEAM_PLAYERS);
    return usedSlots < maxSlots ? maxSlots - usedSlots : 0;
}

BattlefieldPlayerStatus Battlefield::GetPlayerStatus(ObjectGuid playerGuid, TeamId teamId) const
{
    Optional<BattlefieldPlayerState> const state = _battlefieldPlayers.FindState(playerGuid, teamId);
    if (!state)
        return BATTLEFIELD_PLAYER_STATUS_NONE;

    switch (*state)
    {
        case BATTLEFIELD_PLAYER_STATE_QUEUED:
            return BATTLEFIELD_PLAYER_STATUS_QUEUED;
        case BATTLEFIELD_PLAYER_STATE_INVITED:
            return BATTLEFIELD_PLAYER_STATUS_INVITED;
        case BATTLEFIELD_PLAYER_STATE_BATTLE_ENDED:
            return BATTLEFIELD_PLAYER_STATUS_BATTLE_ENDED;
        default:
            return BATTLEFIELD_PLAYER_STATUS_NONE;
    }
}

BattlefieldPlayerStatus Battlefield::RemoveFromQueue(ObjectGuid playerGuid, TeamId teamId)
{
    BattlefieldPlayerStatus const status = GetPlayerStatus(playerGuid, teamId);
    _battlefieldPlayers.Erase(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_QUEUED);
    if (_battlefieldPlayers.Erase(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_INVITED))
    {
        RemoveEvents(BATTLEFIELD_EVENT_BATTLE_EXPIRE, playerGuid);
        FillBattleSlots(false);
    }

    return status;
}

void Battlefield::InviteToBattle(Player* player, bool battleStart)
{
    ObjectGuid const playerGuid = player->GetGUID();
    TeamId const teamId = player->GetTeamId();
    Seconds const duration = Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_BATTLE_INVITE);
    _battlefieldPlayers.Insert(playerGuid, teamId, BATTLEFIELD_PLAYER_STATE_INVITED);
    player->GetSession()->SendBfInvitePlayerToBattle(GetId(), GetZoneId(), uint32(duration.count()));
    if (battleStart)
        player->GetSession()->SendBfStateChange(GetId(), BATTLEFIELD_PLAYER_STATUS_INVITED);

    QueueEvent<BattlefieldBattleExpireEvent>(playerGuid, duration);
}

void Battlefield::FillBattleSlots(bool battleStart)
{
    uint32 const maxLevel = sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL);
    for (TeamId teamId : { TEAM_ALLIANCE, TEAM_HORDE })
    {
        uint32 const freeSlots = GetFreeBattleSlots(teamId);
        if (!freeSlots)
            continue;

        std::vector<Player*> priorityPlayers;
        std::vector<Player*> otherPlayers;
        for (ObjectGuid playerGuid : _battlefieldPlayers.List(teamId, BATTLEFIELD_PLAYER_STATE_QUEUED))
        {
            Player* player = ObjectAccessor::FindPlayer(playerGuid);
            if (!player || player->InArena() || !CanJoinBattle(player))
                continue;

            if (player->GetLevel() >= maxLevel)
                priorityPlayers.push_back(player);
            else
                otherPlayers.push_back(player);
        }

        Trinity::Containers::RandomShuffle(priorityPlayers);
        Trinity::Containers::RandomShuffle(otherPlayers);
        priorityPlayers.insert(priorityPlayers.end(), otherPlayers.begin(), otherPlayers.end());
        if (priorityPlayers.size() > freeSlots)
            priorityPlayers.resize(freeSlots);

        for (Player* player : priorityPlayers)
            InviteToBattle(player, battleStart);
    }
}

void Battlefield::ClearBattleStatus()
{
    for (TeamId teamId : { TEAM_ALLIANCE, TEAM_HORDE })
    {
        for (ObjectGuid playerGuid : _battlefieldPlayers.List(teamId, BATTLEFIELD_PLAYER_STATE_BATTLE_ENDED))
            if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
                player->GetSession()->SendBfStateChange(GetId(), BATTLEFIELD_PLAYER_STATUS_NONE);

        _battlefieldPlayers.EraseAll(teamId, BATTLEFIELD_PLAYER_STATE_BATTLE_ENDED);
    }
}

void Battlefield::ScheduleNextPhaseAdvance()
{
    if (!IsEnabled())
        return;

    time_t target = 0;
    switch (_phase)
    {
        case BATTLEFIELD_PHASE_IDLE:
            target = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START) - Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION).count();
            break;
        case BATTLEFIELD_PHASE_PREPARATION:
            target = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START);
            break;
        case BATTLEFIELD_PHASE_BATTLE:
            target = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END);
            break;
        default:
            break;
    }

    time_t const now = GameTime::GetGameTime();
    QueueEvent<BattlefieldPhaseAdvanceEvent>(ObjectGuid::Empty, Seconds(target > now ? target - now : 0));
}

void Battlefield::ScheduleNextGraveyardResurrect()
{
    Seconds const interval = Values.Get<Seconds>(BATTLEFIELD_VALUE_INTERVAL_RESURRECTION);
    Values.Set(BATTLEFIELD_VALUE_TIME_NEXT_RESURRECTION, GameTime::Now() + interval);
    QueueEvent<BattlefieldGraveyardResurrectEvent>(ObjectGuid::Empty, interval);
}

/*
 * PlayerContainer
 */

bool Battlefield::PlayerContainer::Contains(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state) const
{
    return _players.contains(Key(teamId, state, playerGuid));
}

Optional<BattlefieldPlayerState> Battlefield::PlayerContainer::FindState(ObjectGuid playerGuid, TeamId teamId) const
{
    for (uint8 state = BATTLEFIELD_PLAYER_STATE_QUEUED; state < BATTLEFIELD_PLAYER_STATE_MAX; ++state)
        if (Contains(playerGuid, teamId, BattlefieldPlayerState(state)))
            return BattlefieldPlayerState(state);

    return {};
}

uint32 Battlefield::PlayerContainer::Count(TeamId teamId, BattlefieldPlayerState state) const
{
    auto const range = GetRange(teamId, state);
    return uint32(std::distance(range.begin(), range.end()));
}

std::vector<ObjectGuid> Battlefield::PlayerContainer::List(TeamId teamId, BattlefieldPlayerState state) const
{
    std::vector<ObjectGuid> playerGuids;
    for (Key const& key : GetRange(teamId, state))
        playerGuids.push_back(std::get<ObjectGuid>(key));

    return playerGuids;
}

void Battlefield::PlayerContainer::Insert(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state)
{
    if (Optional<BattlefieldPlayerState> const previousState = FindState(playerGuid, teamId))
        Erase(playerGuid, teamId, *previousState);

    _players.emplace(teamId, state, playerGuid);
}

bool Battlefield::PlayerContainer::Erase(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state)
{
    return _players.erase(Key(teamId, state, playerGuid)) != 0;
}

void Battlefield::PlayerContainer::EraseAll(TeamId teamId, BattlefieldPlayerState state)
{
    auto const range = GetRange(teamId, state);
    _players.erase(range.begin(), range.end());
}

Trinity::IteratorPair<Battlefield::PlayerContainer::Container::const_iterator> Battlefield::PlayerContainer::GetRange(TeamId teamId, BattlefieldPlayerState state) const
{
    return { _players.lower_bound(Key(teamId, state, ObjectGuid::Empty)), _players.lower_bound(Key(teamId, BattlefieldPlayerState(state + 1), ObjectGuid::Empty)) };
}
