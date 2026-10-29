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

#ifndef BattlefieldPackets_h__
#define BattlefieldPackets_h__

#include "Packet.h"
#include <ctime>

namespace WorldPackets
{
    namespace Battlefield
    {
        class BFMgrEntryInvite final : public ServerPacket
        {
        public:
            BFMgrEntryInvite() : ServerPacket(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, 4 + 4 + 4) { }

            WorldPacket const* Write() override;

            uint32 BattleID = 0;
            uint32 ZoneID = 0;
            time_t ExpireTime = time_t(0);
        };

        class BFMgrEntryInviteResponse final : public ClientPacket
        {
        public:
            explicit BFMgrEntryInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint32 BattleID = 0;
            bool AcceptedInvite = false;
        };

        class BFMgrQueueInvite final : public ServerPacket
        {
        public:
            BFMgrQueueInvite() : ServerPacket(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, 4 + 1) { }

            WorldPacket const* Write() override;

            uint32 BattleID = 0;
            uint8 BattleState = 0;
        };

        class BFMgrQueueInviteResponse final : public ClientPacket
        {
        public:
            explicit BFMgrQueueInviteResponse(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_MGR_QUEUE_INVITE_RESPONSE, std::move(packet)) { }

            void Read() override;

            uint32 BattleID = 0;
            bool AcceptedInvite = false;
        };

        class BFMgrQueueRequestResponse final : public ServerPacket
        {
        public:
            BFMgrQueueRequestResponse() : ServerPacket(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, 4 + 4 + 1 + 1 + 1) { }

            WorldPacket const* Write() override;

            uint32 BattleID = 0;
            uint32 ZoneID = 0;
            bool Accepted = false;
            bool LoggingIn = false;
            uint8 BattleState = 0;
        };

        class BFMgrQueueExitRequest final : public ClientPacket
        {
        public:
            explicit BFMgrQueueExitRequest(WorldPacket&& packet) : ClientPacket(CMSG_BATTLEFIELD_MGR_EXIT_REQUEST, std::move(packet)) { }

            void Read() override;

            uint32 BattleID = 0;
        };

        class BFMgrEntering final : public ServerPacket
        {
        public:
            BFMgrEntering() : ServerPacket(SMSG_BATTLEFIELD_MGR_ENTERED, 4 + 1 + 1 + 1) { }

            WorldPacket const* Write() override;

            uint32 BattleID = 0;
            bool Relocated = false;
            bool OnOffense = false;
            bool ClearedAFK = false;
        };

        class BFMgrEjected final : public ServerPacket
        {
        public:
            BFMgrEjected() : ServerPacket(SMSG_BATTLEFIELD_MGR_EJECTED, 4 + 1 + 1 + 1) { }

            WorldPacket const* Write() override;

            uint32 BattleID = 0;
            uint8 Reason = 0;
            uint8 BattleState = 0;
            bool Relocated = false;
        };

        class BFMgrStateChange final : public ServerPacket
        {
        public:
            BFMgrStateChange() : ServerPacket(SMSG_BATTLEFIELD_MGR_STATE_CHANGE, 4 + 4) { }

            WorldPacket const* Write() override;

            uint32 BattleID = 0;
            uint32 BattleState = 0;
        };
    }
}

#endif // BattlefieldPackets_h__
