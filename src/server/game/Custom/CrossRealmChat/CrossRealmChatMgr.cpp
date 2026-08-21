#include "CrossRealmChatMgr.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Language.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Realm.h"
#include "RealmStatusMgr.h"
#include "SharedDefines.h"
#include "StringFormat.h"
#include "Util.h"
#include "World.h"

constexpr uint32 CROSS_REALM_CHAT_SWEEP_INTERVAL = 1 * MINUTE * IN_MILLISECONDS;

CrossRealmChatMgr* CrossRealmChatMgr::instance()
{
    static CrossRealmChatMgr inst;
    return &inst;
}

void CrossRealmChatMgr::Initialize()
{
    if (!sWorld->getBoolConfig(CONFIG_CROSS_REALM_CHAT))
        return;

    if (!sWorld->getBoolConfig(CONFIG_WORLD_CHAT))
    {
        TC_LOG_ERROR("crossrealm", "CrossRealmChat.Enable requires WorldChat.Enable, cross-realm world chat stays off");
        return;
    }

    // Peers only write to realms advertising this feature
    sRealmStatusMgr->AddCapability(REALM_CAPABILITY_CROSS_REALM_CHAT);

    // Leftovers predate the restart, replaying them would dump stale chat into the channel
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHAT_INBOX);
    stmt->setUInt32(0, realm.Id.Realm);
    LoginDatabase.DirectExecute(stmt);

    _active = true;

    TC_LOG_INFO("crossrealm", "Cross-realm world chat enabled for realm {} '{}' (poll {}s)",
        realm.Id.Realm, realm.Name, sWorld->getIntConfig(CONFIG_CROSS_REALM_CHAT_POLL_INTERVAL) / IN_MILLISECONDS);
}

void CrossRealmChatMgr::Update(uint32 diff)
{
    if (!_active)
        return;

    _queryProcessor.ProcessReadyCallbacks();
    _transactionProcessor.ProcessReadyCallbacks();

    _sweepTimer += diff;
    if (_sweepTimer >= CROSS_REALM_CHAT_SWEEP_INTERVAL)
    {
        _sweepTimer = 0;
        SweepExpired();
    }

    _pollTimer += diff;
    if (_pollTimer < sWorld->getIntConfig(CONFIG_CROSS_REALM_CHAT_POLL_INTERVAL))
        return;

    _pollTimer = 0;

    AnnouncePeerChanges();

    if (_pollPending)
        return;

    Poll();
}

void CrossRealmChatMgr::Publish(std::string const& channelName, std::string const& senderName, uint8 senderClass, uint8 chatTag, std::string const& text)
{
    if (!_active || text.empty() || senderName.empty())
        return;

    if (!Channel::IsDefaultWorldChat(channelName))
        return;

    LoginDatabaseTransaction trans = LoginDatabase.BeginTransaction();
    bool queued = false;

    for (auto const& [peerRealmId, peer] : sRealmStatusMgr->GetPeers())
    {
        if (!(peer.Capabilities & REALM_CAPABILITY_CROSS_REALM_CHAT))
            continue;

        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_REALM_CHAT_QUEUE);
        stmt->setUInt32(0, peerRealmId);
        stmt->setUInt32(1, realm.Id.Realm);
        stmt->setString(2, realm.Name);
        stmt->setString(3, channelName);
        stmt->setString(4, senderName);
        stmt->setUInt8(5, senderClass);
        stmt->setUInt8(6, chatTag);
        stmt->setString(7, text);
        trans->Append(stmt);
        queued = true;
    }

    // One commit for the whole fan-out, cost does not scale with realm count
    if (queued)
        LoginDatabase.CommitTransaction(trans);
}

void CrossRealmChatMgr::Poll()
{
    _pollPending = true;

    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALM_CHAT_QUEUE);
    stmt->setUInt32(0, realm.Id.Realm);
    stmt->setUInt32(1, sWorld->getIntConfig(CONFIG_CROSS_REALM_CHAT_BATCH_SIZE));

    _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([this](PreparedQueryResult result)
    {
        // Null covers an empty inbox and an unreachable auth db alike, nothing was read
        if (!result)
        {
            _pollPending = false;
            return;
        }

        std::vector<QueuedMessage> messages;
        messages.reserve(std::size_t(result->GetRowCount()));

        do
        {
            Field* fields = result->Fetch();

            QueuedMessage message;
            message.Id = fields[0].GetUInt64();
            message.SourceRealm = fields[1].GetString();
            message.Channel = fields[2].GetString();
            message.Sender = fields[3].GetString();
            message.SenderClass = fields[4].GetUInt8();
            message.ChatTag = fields[5].GetUInt8();
            message.Text = fields[6].GetString();

            messages.push_back(std::move(message));
        }
        while (result->NextRow());

        Consume(std::move(messages));
    }));
}

void CrossRealmChatMgr::Consume(std::vector<QueuedMessage> messages)
{
    LoginDatabaseTransaction trans = LoginDatabase.BeginTransaction();

    for (QueuedMessage const& message : messages)
    {
        // By id, never by range: ids are assigned at insert but only visible at commit
        LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHAT_MESSAGE);
        stmt->setUInt64(0, message.Id);
        trans->Append(stmt);
    }

    // Broadcast only once the rows are gone, a failed commit must not show the batch twice
    _transactionProcessor.AddCallback(LoginDatabase.AsyncCommitTransaction(trans)).AfterComplete([this, messages = std::move(messages)](bool success)
    {
        _pollPending = false;

        // Rows survived, the next poll delivers them and nothing was shown yet
        if (!success)
        {
            TC_LOG_ERROR("crossrealm", "Could not clear {} consumed message(s), delivery deferred to the next poll", messages.size());
            return;
        }

        for (QueuedMessage const& message : messages)
        {
            // The sending realm validated the links, CheckAllLinks rejects the faction colour tag
            if (message.Sender.find('|') != std::string::npos)
            {
                TC_LOG_ERROR("crossrealm", "Dropped message {} from realm '{}': sender contains markup", message.Id, message.SourceRealm);
                continue;
            }

            // Realm tag ahead of the text, which already carries its own realm's faction tag
            Broadcast(message.Channel, message.Sender, Trinity::StringFormat("|cff00FF96[{}]|r {}", message.SourceRealm, message.Text), message.ChatTag);
        }
    });
}

void CrossRealmChatMgr::SweepExpired() const
{
    // Every inbox, not just ours: rows for a down realm have no other reader
    LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHAT_EXPIRED);
    stmt->setUInt32(0, sWorld->getIntConfig(CONFIG_CROSS_REALM_CHAT_MESSAGE_TTL));
    LoginDatabase.Execute(stmt);
}

void CrossRealmChatMgr::AnnouncePeerChanges()
{
    std::unordered_map<uint32, std::string> current;

    for (auto const& [peerRealmId, peer] : sRealmStatusMgr->GetPeers())
        if (peer.Capabilities & REALM_CAPABILITY_CROSS_REALM_CHAT)
            current.emplace(peerRealmId, peer.Name);

    for (auto const& [peerRealmId, peerRealmName] : current)
        if (!_announcedPeers.contains(peerRealmId))
            AnnouncePeer(peerRealmName, true);

    for (auto const& [peerRealmId, peerRealmName] : _announcedPeers)
        if (!current.contains(peerRealmId))
            AnnouncePeer(peerRealmName, false);

    _announcedPeers = std::move(current);
}

void CrossRealmChatMgr::AnnouncePeer(std::string const& realmName, bool connected) const
{
    if (!sWorld->getBoolConfig(CONFIG_CROSS_REALM_CHAT_ANNOUNCE_PEERS))
        return;

    // trinity_string is printf-style, so not Trinity::StringFormat
    char const* format = sObjectMgr->GetTrinityStringForDBCLocale(connected ? LANG_CROSS_REALM_CHAT_PEER_UP : LANG_CROSS_REALM_CHAT_PEER_DOWN);
    if (!format)
        return;

    std::string text;
    try
    {
        text = ChatHandler::PGetParseString(format, realmName.c_str());
    }
    catch (std::exception const& error)
    {
        TC_LOG_ERROR("crossrealm", "trinity_string {} is malformed: {}", uint32(connected ? LANG_CROSS_REALM_CHAT_PEER_UP : LANG_CROSS_REALM_CHAT_PEER_DOWN), error.what());
        return;
    }

    std::string const sender(CROSS_REALM_CHAT_SENDER_NAME);

    Broadcast(std::string(WORLD_CHAT), sender, text, CHAT_TAG_NONE);
}

/*static*/ void CrossRealmChatMgr::Broadcast(std::string const& channelName, std::string const& senderName, std::string const& text, uint8 chatTag)
{
    if (!Channel::IsDefaultWorldChat(channelName))
        return;

    ChannelMgr* channelMgr = ChannelMgr::ForTeam(Team::ALLIANCE);
    if (!channelMgr)
        return;

    // Absent while nobody here has joined it
    if (Channel* channel = channelMgr->GetCustomChannel(channelName))
        channel->SayRemote(senderName, text, chatTag);
}
