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

#include "WorldSession.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "BattlefieldPackets.h"
#include "GameTime.h"
#include "Player.h"

void WorldSession::SendBfInvitePlayerToBattle(uint32 battleId, uint32 zoneId, uint32 acceptTime)
{
    WorldPackets::Battlefield::BFMgrEntryInvite bfMgrEntryInvite;
    bfMgrEntryInvite.BattleID = battleId;
    bfMgrEntryInvite.ZoneID = zoneId;
    bfMgrEntryInvite.ExpireTime = GameTime::GetGameTime() + acceptTime;
    SendPacket(bfMgrEntryInvite.Write());
}

void WorldSession::SendBfInvitePlayerToQueue(uint32 battleId)
{
    WorldPackets::Battlefield::BFMgrQueueInvite bfMgrQueueInvite;
    bfMgrQueueInvite.BattleID = battleId;
    bfMgrQueueInvite.BattleState = 1;
    SendPacket(bfMgrQueueInvite.Write());
}

void WorldSession::SendBfQueueInviteResponse(uint32 battleId, uint32 zoneId, bool accepted, bool loggingIn, bool warmup)
{
    WorldPackets::Battlefield::BFMgrQueueRequestResponse bfMgrQueueRequestResponse;
    bfMgrQueueRequestResponse.BattleID = battleId;
    bfMgrQueueRequestResponse.ZoneID = zoneId;
    bfMgrQueueRequestResponse.Accepted = accepted;
    bfMgrQueueRequestResponse.LoggingIn = loggingIn;
    bfMgrQueueRequestResponse.BattleState = warmup ? 1 : 3;
    SendPacket(bfMgrQueueRequestResponse.Write());
}

void WorldSession::SendBfEntered(uint32 battleId, bool relocated, bool onOffense)
{
    WorldPackets::Battlefield::BFMgrEntering bfMgrEntering;
    bfMgrEntering.BattleID = battleId;
    bfMgrEntering.Relocated = relocated;
    bfMgrEntering.OnOffense = onOffense;
    bfMgrEntering.ClearedAFK = _player->isAFK();
    SendPacket(bfMgrEntering.Write());
}

void WorldSession::SendBfLeaveMessage(uint32 battleId, BattlefieldLeaveReason reason, BattlefieldPlayerStatus status)
{
    WorldPackets::Battlefield::BFMgrEjected bfMgrEjected;
    bfMgrEjected.BattleID = battleId;
    bfMgrEjected.Reason = reason;
    bfMgrEjected.BattleState = status;
    bfMgrEjected.Relocated = false;
    SendPacket(bfMgrEjected.Write());
}

void WorldSession::SendBfStateChange(uint32 battleId, BattlefieldPlayerStatus status)
{
    WorldPackets::Battlefield::BFMgrStateChange bfMgrStateChange;
    bfMgrStateChange.BattleID = battleId;
    bfMgrStateChange.BattleState = status;
    SendPacket(bfMgrStateChange.Write());
}

void WorldSession::HandleBfQueueInviteResponse(WorldPackets::Battlefield::BFMgrQueueInviteResponse& bfMgrQueueInviteResponse)
{
    if (Battlefield* battlefield = sBattlefieldMgr->GetEnabledBattlefield(BattlefieldBattleId(bfMgrQueueInviteResponse.BattleID)))
        battlefield->HandleQueueInviteResponse(_player, bfMgrQueueInviteResponse.AcceptedInvite);
}

void WorldSession::HandleBfEntryInviteResponse(WorldPackets::Battlefield::BFMgrEntryInviteResponse& bfMgrEntryInviteResponse)
{
    if (Battlefield* battlefield = sBattlefieldMgr->GetEnabledBattlefield(BattlefieldBattleId(bfMgrEntryInviteResponse.BattleID)))
        battlefield->HandleBattleInviteResponse(_player, bfMgrEntryInviteResponse.AcceptedInvite);
}

void WorldSession::HandleBfQueueExitRequest(WorldPackets::Battlefield::BFMgrQueueExitRequest& bfMgrQueueExitRequest)
{
    if (Battlefield* battlefield = sBattlefieldMgr->GetEnabledBattlefield(BattlefieldBattleId(bfMgrQueueExitRequest.BattleID)))
        battlefield->HandleQueueExitRequest(_player);
}
