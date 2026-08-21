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

#ifndef EG_CROSS_REALM_CHAT_MGR_H
#define EG_CROSS_REALM_CHAT_MGR_H

#include "AsyncCallbackProcessor.h"
#include "Common.h"
#include "DatabaseEnvFwd.h"
#include <string>
#include <unordered_map>
#include <vector>


class TC_GAME_API CrossRealmChatMgr
{
public:
    static CrossRealmChatMgr* instance();

    void Initialize();
    void Update(uint32 diff);
    void Publish(std::string const& channelName, std::string const& senderName, uint8 senderClass, uint8 chatTag, std::string const& text);

private:
    CrossRealmChatMgr() = default;
    ~CrossRealmChatMgr() = default;
    CrossRealmChatMgr(CrossRealmChatMgr const&) = delete;
    CrossRealmChatMgr& operator=(CrossRealmChatMgr const&) = delete;

    struct QueuedMessage
    {
        uint64 Id;
        std::string SourceRealm;
        std::string Channel;
        std::string Sender;
        uint8 SenderClass;
        uint8 ChatTag;
        std::string Text;
    };

    void Poll();
    void Consume(std::vector<QueuedMessage> messages);
    void SweepExpired() const;
    void AnnouncePeerChanges();
    void AnnouncePeer(std::string const& realmName, bool connected) const;

    static void Broadcast(std::string const& channelName, std::string const& senderName, std::string const& text, uint8 chatTag);

    bool _active = false;
    uint32 _pollTimer = 0;
    uint32 _sweepTimer = 0;
    bool _pollPending = false;
    std::unordered_map<uint32, std::string> _announcedPeers;
    QueryCallbackProcessor _queryProcessor;
    AsyncCallbackProcessor<TransactionCallback> _transactionProcessor;
};

#define sCrossRealmChatMgr CrossRealmChatMgr::instance()

#endif
