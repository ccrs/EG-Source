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
#include "WorldStatePackets.h"
#include <algorithm>

Battlefield::Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId) : _battleId(battleId), _zoneId(zoneId), _phase(BATTLEFIELD_PHASE_IDLE), _flags(BATTLEFIELD_FLAG_NONE)
{
    Values.Set(BATTLEFIELD_VALUE_ID_MAP, uint32(0));
    Values.Set(BATTLEFIELD_VALUE_STATE_CONTROLLING_TEAM, PVP_TEAM_NEUTRAL);
    Values.Set(BATTLEFIELD_VALUE_INTERVAL_RESURRECTION, 30s);
    Values.Set(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT, BATTLEFIELD_BATTLE_RESULT_NONE);
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
    if (phase != BATTLEFIELD_PHASE_IDLE && !IsEnabled())
        return false;

    BattlefieldPhase const oldPhase = _phase;
    if (phase == oldPhase)
        return true;

    bool wasBattlefieldRaidPhase = IsBattlefieldRaidPhase();
    _phase = phase;
    if (wasBattlefieldRaidPhase && !IsBattlefieldRaidPhase())
        QueueEvent<BattlefieldRaidReleaseEvent>();

    if (phase == BATTLEFIELD_PHASE_WAR)
    {
        time_t const start = std::min(GameTime::GetGameTime(), Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START));
        Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END, start + Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_WAR).count());
        Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START, start + Values.Get<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_WAR_START).count());
        Values.Set(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT, BATTLEFIELD_BATTLE_RESULT_NONE);
    }
    else if (oldPhase == BATTLEFIELD_PHASE_WAR)
    {
        Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END, time_t(0));

        time_t const now = GameTime::GetGameTime();
        time_t const interval = Values.Get<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_WAR_START).count();
        time_t const preparationStartTime = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START) - Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION).count();
        if (preparationStartTime < now)
            Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START, Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START) + ((now - preparationStartTime) / interval + 1) * interval);

        if (Values.Get<BattlefieldBattleResult>(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT) == BATTLEFIELD_BATTLE_RESULT_CAPTURED)
            ChangeTeams(GetAttackingTeam());
    }

    SendPhaseTimes();
    QueueEvent<BattlefieldZoneRefreshEvent>();
    ScheduleNextPhaseAdvance();
    return true;
}

void Battlefield::SetPhaseTime(time_t phaseTime)
{
    Values.Set(IsWarTime() ? BATTLEFIELD_VALUE_TIME_PHASE_WAR_END : BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START, std::max(phaseTime, GameTime::GetGameTime()));
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
    if (Group* group = player->GetGroup())
    {
        std::vector<ObjectGuid> const& raid = _battlefieldRaids[player->GetTeamId()];
        if (std::find(raid.begin(), raid.end(), group->GetGUID()) != raid.end())
            RemoveEvents(BATTLEFIELD_EVENT_RAID_REMOVE, player->GetGUID());
    }
    _playerGUIDs.insert(player->GetGUID());
}

void Battlefield::HandlePlayerLeaveZone(Player* player)
{
    RemovePlayerFromResurrectionQueue(player);
    QueueEvent<BattlefieldRaidRemoveEvent>(player->GetGUID());
    _playerGUIDs.erase(player->GetGUID());
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
    if (Group* group = player->GetGroup())
    {
        if (std::find(raid.begin(), raid.end(), group->GetGUID()) != raid.end())
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
    time_t const nextWarStartTime = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START);
    switch (_phase)
    {
        case BATTLEFIELD_PHASE_IDLE:
            if (now >= nextWarStartTime - Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION).count() && SetPhase(BATTLEFIELD_PHASE_PREPARATION))
                return;
            break;
        case BATTLEFIELD_PHASE_PREPARATION:
            if (now >= nextWarStartTime && SetPhase(BATTLEFIELD_PHASE_WAR))
                return;
            break;
        case BATTLEFIELD_PHASE_WAR:
            if (now >= Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END))
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
    Map* battleMap = sMapMgr->FindMap(Values.Get<uint32>(BATTLEFIELD_VALUE_ID_MAP), 0);
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

void Battlefield::HandleBattlefieldRaidMemberRemoved(ObjectGuid /*playerGuid*/, RemoveMethod /*method*/)
{
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
    if (!IsWarTime())
        return false;

    Values.Set(BATTLEFIELD_VALUE_STATE_BATTLE_RESULT, result);
    return true;
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
    return _graveyards.contains(graveyardId) ? _graveyards.find(graveyardId)->second.get() : nullptr;
}

BattlefieldGraveyard* Battlefield::GetGraveyard(uint8 graveyardId)
{
    return _graveyards.contains(graveyardId) ? _graveyards.find(graveyardId)->second.get() : nullptr;
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
    uint32 const mapId = Values.Get<uint32>(BATTLEFIELD_VALUE_ID_MAP);
    if (!mapId || !guid)
        return nullptr;
    if (Map* battleMap = sMapMgr->FindMap(mapId, 0))
        return battleMap->GetCreature(guid);
    return nullptr;
}

GameObject* Battlefield::GetGameObject(ObjectGuid guid) const
{
    uint32 const mapId = Values.Get<uint32>(BATTLEFIELD_VALUE_ID_MAP);
    if (!mapId || !guid)
        return nullptr;
    if (Map* battleMap = sMapMgr->FindMap(mapId, 0))
        return battleMap->GetGameObject(guid);
    return nullptr;
}

void Battlefield::InitializePhaseTimes()
{
    Seconds const cycleDuration = Values.Get<Seconds>(BATTLEFIELD_VALUE_INTERVAL_PHASE_WAR_START);
    Seconds const minimumCycleDuration = Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_WAR) + Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION);
    if (cycleDuration < minimumCycleDuration)
    {
        TC_LOG_ERROR("bg.battlefield", "Battlefield::InitializePhaseTimes: battlefield {} cycle of {}s is shorter than war plus preparation, using {}s.", uint32(_battleId), cycleDuration.count(), minimumCycleDuration.count());
        Values.Set(BATTLEFIELD_VALUE_INTERVAL_PHASE_WAR_START, minimumCycleDuration);
    }

    time_t const now = GameTime::GetGameTime();
    if (Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END) > now || Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START) <= now)
        Values.Set<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START, now + Values.Get<Seconds>(BATTLEFIELD_VALUE_DELAY_PHASE_WAR_RESTART).count());

    Values.Set(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END, time_t(0));
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
    Map* battleMap = sMapMgr->FindMap(Values.Get<uint32>(BATTLEFIELD_VALUE_ID_MAP), 0);
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

void Battlefield::ScheduleNextPhaseAdvance()
{
    if (!IsEnabled())
        return;

    time_t target = 0;
    switch (_phase)
    {
        case BATTLEFIELD_PHASE_IDLE:
            target = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START) - Values.Get<Seconds>(BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION).count();
            break;
        case BATTLEFIELD_PHASE_PREPARATION:
            target = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_NEXT_START);
            break;
        case BATTLEFIELD_PHASE_WAR:
            target = Values.Get<time_t>(BATTLEFIELD_VALUE_TIME_PHASE_WAR_END);
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
