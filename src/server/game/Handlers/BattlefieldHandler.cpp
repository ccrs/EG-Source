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
#include "GameTime.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "WorldPacket.h"

/**
 */
void WorldSession::SendBfInvitePlayerToWar(uint32/* battleId*/, uint32/* zoneId*/, uint32/* acceptTime*/)
{
}

/**
 */
void WorldSession::SendBfInvitePlayerToQueue(uint32/* battleId*/)
{
}

/**
 */
void WorldSession::SendBfQueueInviteResponse(uint32/* battleId*/, uint32/* zoneId*/, bool/* canQueue*/, bool/* full*/)
{
}

/**
 */
void WorldSession::SendBfEntered(uint32/* battleId*/)
{
}

/**
 */
void WorldSession::SendBfLeaveMessage(uint32/* battleId*/, BFLeaveReason/* reason = BF_LEAVE_REASON_EXITED*/)
{
}

/**
 */
void WorldSession::HandleBfQueueInviteResponse(WorldPacket&/* recvData*/)
{
}

/**
 */
void WorldSession::HandleBfEntryInviteResponse(WorldPacket&/* recvData*/)
{
}

/**
 */
void WorldSession::HandleBfQueueExitRequest(WorldPacket&/* recvData*/)
{
}
