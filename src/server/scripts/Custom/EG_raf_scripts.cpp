#include "ScriptMgr.h"
#include "CharacterCache.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Log.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Realm.h"
#include "StringFormat.h"
#include "World.h"
#include <unordered_set>

enum RAFRewardStatus
{
    RAF_REWARD_PENDING = 0,
    RAF_REWARD_PROCESSING,
    RAF_REWARD_DELIVERED,
    RAF_REWARD_FAILED
};

struct RAFRewardRow
{
    uint32 Id;
    uint32 RecruiterAccountId;
    uint32 CharacterGuid;
    std::string CharacterName;
    uint32 ItemEntry;
    uint32 ItemCount;
    std::string Subject;
    std::string Body;
};

class EG_RAFRewardDispatcher : public WorldScript
{
    public:
        EG_RAFRewardDispatcher() : WorldScript("EG_RAFRewardDispatcher") { }

        void OnConfigLoad(bool /*reload*/) override
        {
            _enabled = sConfigMgr->GetBoolDefault("RAF.Reward.Enable", false);
            _pollInterval = uint32(sConfigMgr->GetIntDefault("RAF.Reward.PollInterval", 60)) * IN_MILLISECONDS;
            _batchSize = uint32(sConfigMgr->GetIntDefault("RAF.Reward.BatchSize", 25));

            if (_pollInterval < IN_MILLISECONDS)
                _pollInterval = IN_MILLISECONDS;

            if (!_batchSize)
                _batchSize = 1;
        }

        void OnStartup() override
        {
            if (!_enabled)
                return;

            // A crash can strand a row after its mail committed but before the status write, so these
            // are failed rather than requeued: a false FAILED beats mailing the reward twice
            LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_RAF_REWARD_STRANDED);
            stmt->setUInt32(0, realm.Id.Realm);
            LoginDatabase.DirectExecute(stmt);

            TC_LOG_INFO("raf", "RAF reward dispatcher enabled for realm {} (interval {}s, batch {})",
                realm.Id.Realm, _pollInterval / IN_MILLISECONDS, _batchSize);
        }

        void OnUpdate(uint32 diff) override
        {
            if (!_enabled)
                return;

            _queryProcessor.ProcessReadyCallbacks();

            _timer += diff;
            if (_timer < _pollInterval)
                return;

            _timer = 0;
            Poll();
        }

    private:
        void Poll()
        {
            LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_RAF_REWARD_QUEUE);
            stmt->setUInt32(0, realm.Id.Realm);
            stmt->setUInt32(1, _batchSize);

            _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(stmt).WithPreparedCallback([this](PreparedQueryResult result)
            {
                // Null covers both an empty queue and an unreachable auth db; nothing is claimed, so nothing is lost
                if (!result)
                    return;

                do
                {
                    Field* fields = result->Fetch();

                    RAFRewardRow row;
                    row.Id = fields[0].GetUInt32();
                    row.RecruiterAccountId = fields[1].GetUInt32();
                    row.CharacterGuid = fields[2].GetUInt32();
                    row.CharacterName = fields[3].GetString();
                    row.ItemEntry = fields[4].GetUInt32();
                    row.ItemCount = fields[5].GetUInt32();
                    row.Subject = fields[6].GetString();
                    row.Body = fields[7].GetString();

                    // Still reads as PENDING while an earlier tick's callbacks are unresolved
                    if (!_inFlight.insert(row.Id).second)
                        continue;

                    Claim(row);
                }
                while (result->NextRow());
            }));
        }

        // No affected-rows count exists in this tree, so the swap is confirmed by re-reading status
        void Claim(RAFRewardRow const& row)
        {
            LoginDatabasePreparedStatement* claimStmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_RAF_REWARD_CLAIM);
            claimStmt->setUInt32(0, row.Id);
            claimStmt->setUInt32(1, realm.Id.Realm);

            _queryProcessor.AddCallback(LoginDatabase.AsyncQuery(claimStmt)
                .WithChainingPreparedCallback([row](QueryCallback& callback, PreparedQueryResult /*result*/)
            {
                // Chaining is what orders the read after the write across async worker threads
                LoginDatabasePreparedStatement* checkStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_RAF_REWARD_CLAIM_CHECK);
                checkStmt->setUInt32(0, row.Id);
                checkStmt->setUInt32(1, realm.Id.Realm);
                callback.SetNextQuery(LoginDatabase.AsyncQuery(checkStmt));
            })
                .WithPreparedCallback([this, row](PreparedQueryResult result)
            {
                if (!result)
                {
                    _inFlight.erase(row.Id);
                    return;
                }

                // Someone else took it, not an error
                if ((*result)[0].GetUInt8() != RAF_REWARD_PROCESSING)
                {
                    _inFlight.erase(row.Id);
                    return;
                }

                Deliver(row);
            }));
        }

        void Deliver(RAFRewardRow const& row)
        {
            ObjectGuid receiverGuid = ObjectGuid::Create<HighGuid::Player>(row.CharacterGuid);

            // Cache entries drop on delete and rewrite on account transfer, so both checks need no db round trip
            CharacterCacheEntry const* cacheEntry = sCharacterCache->GetCharacterCacheByGuid(receiverGuid);
            if (!cacheEntry)
            {
                Fail(row, Trinity::StringFormat("character guid {} does not exist", row.CharacterGuid));
                return;
            }

            if (cacheEntry->AccountId != row.RecruiterAccountId)
            {
                Fail(row, Trinity::StringFormat("character guid {} belongs to account {}, expected {}",
                    row.CharacterGuid, cacheEntry->AccountId, row.RecruiterAccountId));
                return;
            }

            // Mandatory, not defensive: Item::CreateItem calls ABORT() on an unknown template
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(row.ItemEntry);
            if (!proto)
            {
                Fail(row, Trinity::StringFormat("item entry {} is not a valid item template", row.ItemEntry));
                return;
            }

            if (!row.ItemCount || (proto->MaxCount > 0 && row.ItemCount > uint32(proto->MaxCount)))
            {
                Fail(row, Trinity::StringFormat("invalid item_count {} for item {}", row.ItemCount, row.ItemEntry));
                return;
            }

            // CreateItem would silently clamp a larger count, so refuse rather than under-deliver
            if (row.ItemCount > proto->GetMaxStackSize())
            {
                Fail(row, Trinity::StringFormat("item_count {} exceeds stack size {} for item {}",
                    row.ItemCount, proto->GetMaxStackSize(), row.ItemEntry));
                return;
            }

            Item* item = Item::CreateItem(row.ItemEntry, row.ItemCount, nullptr);
            if (!item)
            {
                Fail(row, Trinity::StringFormat("could not create item {}", row.ItemEntry));
                return;
            }

            // Resolved by guid, never by name: a rename would misdeliver the reward
            Player* receiver = ObjectAccessor::FindConnectedPlayer(receiverGuid);
            CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
            item->SaveToDB(trans); // Save to prevent being lost at next mail load. If send fails, the item will be deleted.

            MailDraft draft(row.Subject, row.Body);
            draft.AddItem(item);
            draft.SendMailTo(trans, MailReceiver(receiver, row.CharacterGuid), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM));
            CharacterDatabase.CommitTransaction(trans);

            LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_RAF_REWARD_DELIVERED);
            stmt->setUInt32(0, row.Id);
            stmt->setUInt32(1, realm.Id.Realm);
            LoginDatabase.Execute(stmt);

            _inFlight.erase(row.Id);

            TC_LOG_INFO("raf", "RAF reward {}: delivered item {} x{} to character {} ({}) for account {}",
                row.Id, row.ItemEntry, row.ItemCount, row.CharacterName, row.CharacterGuid, row.RecruiterAccountId);
        }

        void Fail(RAFRewardRow const& row, std::string const& error)
        {
            LoginDatabasePreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_RAF_REWARD_FAILED);
            stmt->setString(0, error.substr(0, 255));
            stmt->setUInt32(1, row.Id);
            stmt->setUInt32(2, realm.Id.Realm);
            LoginDatabase.Execute(stmt);

            _inFlight.erase(row.Id);

            TC_LOG_ERROR("raf", "RAF reward {}: failed permanently: {}", row.Id, error);
        }

        bool _enabled = false;
        uint32 _pollInterval = 60 * IN_MILLISECONDS;
        uint32 _batchSize = 25;
        uint32 _timer = 0;

        std::unordered_set<uint32> _inFlight;
        QueryCallbackProcessor _queryProcessor;
};

void AddSC_EG_raf_scripts()
{
    new EG_RAFRewardDispatcher();
}
