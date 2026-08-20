#include "RealmStatusMgr.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Realm.h"
#include "World.h"

RealmStatusMgr* RealmStatusMgr::instance()
{
    static RealmStatusMgr inst;
    return &inst;
}

void RealmStatusMgr::AddCapability(RealmCapability capability)
{
    _capabilities |= capability;
}

bool RealmStatusMgr::HasCapability(uint32 realmId, RealmCapability capability) const
{
    auto itr = _peers.find(realmId);
    return itr != _peers.end() && (itr->second.Capabilities & capability) != 0;
}

void RealmStatusMgr::Update(uint32 diff)
{
    if (!_capabilities)
        return;

    _queryProcessor.ProcessReadyCallbacks();

    _timer += diff;

    // The first tick beats at once, every OnStartup has run by then so the mask is complete
    if (_started && _timer < sWorld->getIntConfig(CONFIG_REALM_STATUS_INTERVAL))
        return;

    if (!_started)
    {
        _started = true;
        TC_LOG_INFO("realmstatus", "Realm {} '{}' advertising cross-realm capabilities {:#x} every {}s",
            realm.Id.Realm, realm.Name, _capabilities, sWorld->getIntConfig(CONFIG_REALM_STATUS_INTERVAL) / IN_MILLISECONDS);
    }

    _timer = 0;

    SendHeartbeat();
    RefreshPeers();
}

void RealmStatusMgr::SendHeartbeat() const
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_REP_REALM_STATUS);
    stmt->setUInt32(0, realm.Id.Realm);
    stmt->setString(1, realm.Name);
    stmt->setUInt32(2, _capabilities);
    LoginDatabase.Execute(stmt);
}

void RealmStatusMgr::RefreshPeers()
{
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALM_STATUS_PEERS);
    stmt->setUInt32(0, realm.Id.Realm);
    stmt->setUInt32(1, sWorld->getIntConfig(CONFIG_REALM_STATUS_TIMEOUT));

    _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([this](PreparedQueryResult result)
    {
        std::unordered_map<uint32, RealmStatusEntry> peers;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();

                RealmStatusEntry entry;
                entry.RealmId = fields[0].GetUInt32();
                entry.Name = fields[1].GetString();
                entry.Capabilities = fields[2].GetUInt32();

                peers.emplace(entry.RealmId, std::move(entry));
            }
            while (result->NextRow());
        }

        for (auto const& [peerRealmId, peer] : peers)
            if (!_peers.contains(peerRealmId))
                TC_LOG_INFO("realmstatus", "Realm {} '{}' is up with capabilities {:#x}", peerRealmId, peer.Name, peer.Capabilities);

        for (auto const& [peerRealmId, peer] : _peers)
            if (!peers.contains(peerRealmId))
                TC_LOG_INFO("realmstatus", "Realm {} '{}' is down", peerRealmId, peer.Name);

        _peers = std::move(peers);
    }));
}
