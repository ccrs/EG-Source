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
#include <BattlegroundPackets.h>

Battlefield::Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId) : _enabled(false), _resurrectionBaseTimer(30 * IN_MILLISECONDS), _battleId(battleId), _zoneId(zoneId), _active(false), _controllingTeam(PVP_TEAM_NEUTRAL), _timer(0), _resurrectionTimer(_resurrectionBaseTimer)
{
}

Battlefield::~Battlefield()
{
}

bool Battlefield::SetupBattlefield()
{
    return true;
}

void Battlefield::Update(uint32 diff)
{
    _resurrectionTimer.Update(diff);
    if (_resurrectionTimer.Passed())
    {
        _resurrectionTimer.Reset(_resurrectionBaseTimer);
    }
}

void Battlefield::HandlePlayerEnterZone(Player* /*player*/)
{
}

void Battlefield::HandlePlayerLeaveZone(Player* /*player*/)
{
}

void Battlefield::HandleAreaSpiritHealerQueryOpcode(Player* player, ObjectGuid source)
{
    WorldPackets::Battleground::AreaSpiritHealerTime areaSpiritHealerTime;
    areaSpiritHealerTime.HealerGuid = source;
    areaSpiritHealerTime.TimeLeft = _resurrectionTimer.GetTimer();
    player->SendDirectMessage(areaSpiritHealerTime.Write());
}

void Battlefield::HandleAddPlayerToResurrectionQueue(Player* /*player*/, ObjectGuid /*source*/)
{
}

void Battlefield::HandleRemovePlayerFromResurrectionQueue(Player* /*player*/)
{
}

void Battlefield::EmplaceGraveyard(uint8 id, BattlefieldGraveyardPointer&& pointer)
{
    _graveyards.emplace(id, std::move(pointer));
}

Battlefield::BattlefieldGraveyardPointer& Battlefield::GetGraveyard(uint8 graveyardId)
{
    return _graveyards[graveyardId];
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

WorldSafeLocsEntry const* Battlefield::GetClosestGraveyardLocation(Player* /*who*/) const
{
    return nullptr;
}
