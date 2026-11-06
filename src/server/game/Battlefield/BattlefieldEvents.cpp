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

#include "BattlefieldEvents.h"
#include "Battlefield.h"
#include "ObjectAccessor.h"

void BattlefieldEvent::Execute(Battlefield& battlefield)
{
    Handle(battlefield);
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_RAID_RELEASE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessRaidRelease(GetKey());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_RAID_ADD>::Handle(Battlefield& battlefield)
{
    if (Player* player = ObjectAccessor::FindPlayer(GetObjectGUID()))
        battlefield.ProcessRaidAdd(GetKey(), player);
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_RAID_REMOVE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessRaidRemove(GetKey(), GetObjectGUID());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_PHASE_ADVANCE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessPhaseAdvance(GetKey());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_GRAVEYARD_RESURRECT>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessGraveyardResurrect(GetKey());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_ZONE_REFRESH>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessZoneRefresh(GetKey());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_QUEUE_INVITE>::Handle(Battlefield& battlefield)
{
    if (Player* player = ObjectAccessor::FindPlayer(GetObjectGUID()))
        battlefield.ProcessQueueInvite(GetKey(), player);
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_INVITE>::Handle(Battlefield& battlefield)
{
    if (Player* player = ObjectAccessor::FindPlayer(GetObjectGUID()))
        battlefield.ProcessBattleInvite(GetKey(), player);
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_ACCEPT>::Handle(Battlefield& battlefield)
{
    if (Player* player = ObjectAccessor::FindConnectedPlayer(GetObjectGUID()))
        battlefield.ProcessBattleAccept(GetKey(), player);
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_DECLINE>::Handle(Battlefield& battlefield)
{
    if (Player* player = ObjectAccessor::FindConnectedPlayer(GetObjectGUID()))
        battlefield.ProcessBattleDecline(GetKey(), player);
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_EXPIRE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBattleExpire(GetKey(), GetObjectGUID());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_LEAVE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBattleLeave(GetKey(), GetObjectGUID());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_RAID_LEAVE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBattleLeave(GetKey(), GetObjectGUID());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_FILL>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBattleFill(GetKey());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_KICK>::Handle(Battlefield& battlefield)
{
    if (Player* player = ObjectAccessor::FindPlayer(GetObjectGUID()))
        battlefield.ProcessBattleKick(GetKey(), player);
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BATTLE_STATUS_CLEAR>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBattleStatusClear(GetKey());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BUILDING_DAMAGE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBuildingDamage(GetKey(), GetObjectGUID());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BUILDING_DESTROY>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBuildingDestroy(GetKey(), GetObjectGUID());
}

template<>
void BattlefieldTypedEvent<BATTLEFIELD_EVENT_BUILDING_UPDATE>::Handle(Battlefield& battlefield)
{
    battlefield.ProcessBuildingUpdate(GetKey());
}
