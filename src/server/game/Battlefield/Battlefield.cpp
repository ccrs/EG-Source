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
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldStatePackets.h"
#include <algorithm>

Battlefield::Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId) :
    _mapId(0), _controllingTeam(PVP_TEAM_NEUTRAL), _timer(0), _battleId(battleId), _zoneId(zoneId), _phase(BATTLEFIELD_PHASE_IDLE), _flags(BATTLEFIELD_FLAG_NONE), _resurrectionBaseTimer(30 * IN_MILLISECONDS), _resurrectionTimer(_resurrectionBaseTimer)
{
}

Battlefield::~Battlefield()
{
}

bool Battlefield::SetupBattlefield()
{
    return true;
}

void Battlefield::ChangeTeams(PvPTeamId newControllingTeam)
{
    _controllingTeam = newControllingTeam;

    for (BattlefieldGraveyardContainer::value_type& graveyardPair : _graveyards)
        graveyardPair.second->InitializeState();
}

void Battlefield::Update(uint32 diff)
{
    TimePoint const now = GameTime::Now();
    if (!_events.empty() && _events.begin()->first <= now)
    {
        ASSERT(!_flags.HasFlag(BATTLEFIELD_FLAG_UPDATE));
        _flags |= BATTLEFIELD_FLAG_UPDATE;

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

        _flags.RemoveFlag(BATTLEFIELD_FLAG_UPDATE);
        if (_flags.HasFlag(BATTLEFIELD_FLAG_EVENTS_CLEANUP))
        {
            std::erase_if(_events, [](BattlefieldEventContainer::value_type const& pair) -> bool
            {
                return pair.second->IsCancelled();
            });
            _flags.RemoveFlag(BATTLEFIELD_FLAG_EVENTS_CLEANUP);
        }
    }

    _resurrectionTimer.Update(diff);
    if (_resurrectionTimer.Passed())
    {
        for (BattlefieldGraveyardContainer::value_type& pair : _graveyards)
            pair.second->ResurrectPlayers();

        _resurrectionTimer.Reset(_resurrectionBaseTimer);
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
    areaSpiritHealerTime.TimeLeft = uint32(_resurrectionTimer.GetExpiry().count());
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
    packet.MapID = _mapId;
    packet.AreaID = _zoneId;
    packet.SubareaID = player->GetAreaId();
    FillInitialWorldStates(packet);

    player->SendDirectMessage(packet.Write());
}

void Battlefield::SendUpdateToPlayers()
{
    Map* battleMap = sMapMgr->FindMap(_mapId, 0);
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

void Battlefield::EmplaceEvent(std::unique_ptr<BattlefieldEvent>&& event)
{
    if (event->GetQueuePolicy() == BATTLEFIELD_EVENT_QUEUE_REPLACE)
        RemoveEvents(event->GetType(), event->GetObjectGUID());

    TimePoint const due = GameTime::Now() + event->GetDelay();
    _events.emplace(due, std::move(event));
}

void Battlefield::RemoveEvents(std::function<bool(BattlefieldEvent const&)> const& predicate)
{
    if (!_flags.HasFlag(BATTLEFIELD_FLAG_UPDATE))
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
        _flags |= BATTLEFIELD_FLAG_EVENTS_CLEANUP;
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
            if (Group* group = sGroupMgr->GetGroupByGUID(groupGuid))
                group->ConvertFromBattlefieldGroup();
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

void Battlefield::HandleBattlefieldRaidMemberRemoved(ObjectGuid /*playerGuid*/, RemoveMethod /*method*/)
{
}

bool Battlefield::SetPhase(BattlefieldPhase phase)
{
    if (phase != BATTLEFIELD_PHASE_IDLE && !IsEnabled())
        return false;

    bool wasBattlefieldRaidPhase = IsBattlefieldRaidPhase();
    _phase = phase;
    if (wasBattlefieldRaidPhase && !IsBattlefieldRaidPhase())
        QueueEvent<BattlefieldRaidReleaseEvent>();
    return true;
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
    if (!_mapId || !guid)
        return nullptr;
    if (Map* battleMap = sMapMgr->FindMap(_mapId, 0))
        return battleMap->GetCreature(guid);
    return nullptr;
}

GameObject* Battlefield::GetGameObject(ObjectGuid guid) const
{
    if (!_mapId || !guid)
        return nullptr;
    if (Map* battleMap = sMapMgr->FindMap(_mapId, 0))
        return battleMap->GetGameObject(guid);
    return nullptr;
}
