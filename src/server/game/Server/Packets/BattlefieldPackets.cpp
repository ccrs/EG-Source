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

#include "BattlefieldPackets.h"

WorldPacket const* WorldPackets::Battlefield::BFMgrEntryInvite::Write()
{
    _worldPacket << uint32(BattleID);
    _worldPacket << uint32(ZoneID);
    _worldPacket << uint32(ExpireTime);
    return &_worldPacket;
}

void WorldPackets::Battlefield::BFMgrEntryInviteResponse::Read()
{
    _worldPacket >> BattleID;
    AcceptedInvite = _worldPacket.read<uint8>() != 0;
}

WorldPacket const* WorldPackets::Battlefield::BFMgrQueueInvite::Write()
{
    _worldPacket << uint32(BattleID);
    _worldPacket << uint8(BattleState);
    return &_worldPacket;
}

void WorldPackets::Battlefield::BFMgrQueueInviteResponse::Read()
{
    _worldPacket >> BattleID;
    AcceptedInvite = _worldPacket.read<uint8>() != 0;
}

WorldPacket const* WorldPackets::Battlefield::BFMgrQueueRequestResponse::Write()
{
    _worldPacket << uint32(BattleID);
    _worldPacket << uint32(ZoneID);
    _worldPacket << uint8(Accepted);
    _worldPacket << uint8(LoggingIn);
    _worldPacket << uint8(BattleState);
    return &_worldPacket;
}

void WorldPackets::Battlefield::BFMgrQueueExitRequest::Read()
{
    _worldPacket >> BattleID;
}

WorldPacket const* WorldPackets::Battlefield::BFMgrEntering::Write()
{
    _worldPacket << uint32(BattleID);
    _worldPacket << uint8(Relocated);
    _worldPacket << uint8(OnOffense);
    _worldPacket << uint8(ClearedAFK);
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlefield::BFMgrEjected::Write()
{
    _worldPacket << uint32(BattleID);
    _worldPacket << uint8(Reason);
    _worldPacket << uint8(BattleState);
    _worldPacket << uint8(Relocated);
    return &_worldPacket;
}

WorldPacket const* WorldPackets::Battlefield::BFMgrStateChange::Write()
{
    _worldPacket << uint32(BattleID);
    _worldPacket << uint32(BattleState);
    return &_worldPacket;
}
