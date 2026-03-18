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
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldStatePackets.h"
#include <algorithm>

Battlefield::Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId) :
    _mapId(0), _enabled(false), _active(false), _controllingTeam(PVP_TEAM_NEUTRAL), _timer(0), _battleId(battleId), _zoneId(zoneId), _resurrectionBaseTimer(30 * IN_MILLISECONDS), _resurrectionTimer(_resurrectionBaseTimer)
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
    _playerGUIDs.insert(player->GetGUID());
}

void Battlefield::HandlePlayerLeaveZone(Player* player)
{
    _playerGUIDs.erase(player->GetGUID());
}

void Battlefield::UpdateAreaDependentAuras()
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
            }
            ++itr;
        }
        else
            itr = _playerGUIDs.erase(itr);
    }
}

void Battlefield::SendAreaSpiritHealerQueryOpcode(Player* player, ObjectGuid source)
{
    WorldPackets::Battleground::AreaSpiritHealerTime areaSpiritHealerTime;
    areaSpiritHealerTime.HealerGuid = source;
    areaSpiritHealerTime.TimeLeft = _resurrectionTimer.GetTimer();
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

void Battlefield::SendInitWorldStatesTo(Player* player)
{
    WorldPackets::WorldState::InitWorldStates packet;
    packet.MapID = _mapId;
    packet.AreaID = _zoneId;
    packet.SubareaID = player->GetAreaId();
    FillInitialWorldStates(packet);

    player->SendDirectMessage(packet.Write());
}

void Battlefield::EmplaceGraveyard(uint8 id, BattlefieldGraveyardPointer&& pointer)
{
    _graveyards.emplace(id, std::move(pointer));
}

BattlefieldGraveyard* Battlefield::GetGraveyard(uint8 graveyardId)
{
    return _graveyards.contains(graveyardId) ? _graveyards.find(graveyardId)->second.get() : nullptr;
}

BattlefieldGraveyard const* Battlefield::GetGraveyard(uint8 graveyardId) const
{
    return _graveyards.contains(graveyardId) ? _graveyards.find(graveyardId)->second.get() : nullptr;
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

Creature* Battlefield::GetCreature(ObjectGuid guid)
{
    if (!_mapId || !guid)
        return nullptr;
    if (Map* battleMap = sMapMgr->FindMap(_mapId, 0))
        return battleMap->GetCreature(guid);
    return nullptr;
}

GameObject* Battlefield::GetGameObject(ObjectGuid guid)
{
    if (!_mapId || !guid)
        return nullptr;
    if (Map* battleMap = sMapMgr->FindMap(_mapId, 0))
        return battleMap->GetGameObject(guid);
    return nullptr;
}
