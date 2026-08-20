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

#ifndef EG_REALM_STATUS_MGR_H
#define EG_REALM_STATUS_MGR_H

#include "AsyncCallbackProcessor.h"
#include "Common.h"
#include "DatabaseEnvFwd.h"
#include <string>
#include <unordered_map>


enum RealmCapability : uint32
{
    REALM_CAPABILITY_NONE = 0x00,

    REALM_CAPABILITY_CROSS_REALM_CHAT = 0x01
};

struct RealmStatusEntry
{
    uint32 RealmId;
    std::string Name;
    uint32 Capabilities;
};

class TC_GAME_API RealmStatusMgr
{
public:
    static RealmStatusMgr* instance();

    void Update(uint32 diff);
    void AddCapability(RealmCapability capability);

    bool IsOnline(uint32 realmId) const { return _peers.contains(realmId); }
    bool HasCapability(uint32 realmId, RealmCapability capability) const;
    std::unordered_map<uint32, RealmStatusEntry> const& GetPeers() const { return _peers; }

private:
    RealmStatusMgr() = default;
    ~RealmStatusMgr() = default;
    RealmStatusMgr(RealmStatusMgr const&) = delete;
    RealmStatusMgr& operator=(RealmStatusMgr const&) = delete;

    void SendHeartbeat() const;
    void RefreshPeers();

    uint32 _capabilities = REALM_CAPABILITY_NONE;
    uint32 _timer = 0;
    bool _started = false;
    std::unordered_map<uint32, RealmStatusEntry> _peers;
    QueryCallbackProcessor _queryProcessor;
};

#define sRealmStatusMgr RealmStatusMgr::instance()

#endif
