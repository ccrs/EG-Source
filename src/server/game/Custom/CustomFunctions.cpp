#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "Log.h"
#include "Player.h"
#include "Transmogrification.h"
#include "CustomFunctions.h"

void Player::_LoadCustomSettings(PreparedQueryResult result)
{
    if (!result)
        return;

    Field* fields = result->Fetch();
    std::string value = fields[0].GetString();
    if (!value.empty())
    {
        std::istringstream data(value);
        for (uint16 itr = 0; itr < CUSTOM_FLAGS_MAX; itr++)
        {
            uint16 temp = 0;
            data >> temp;
            _customFlags[itr] = temp;
        }
    }
}

void Player::_LoadTransmogrifications(PreparedQueryResult result)
{
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 lowGUID = fields[0].GetUInt32();
            uint32 fakeEntry = fields[1].GetUInt32();

            ObjectGuid itemGUID(HighGuid::Item, 0, lowGUID);

            // Only load items that are in inventory / bank / etc
            if (sObjectMgr->GetItemTemplate(fakeEntry) && GetItemByGuid(itemGUID))
            {
                InsertTransmogrificationEntry(itemGUID, fakeEntry);
            }
            else
            {
                // Ignore, will be erased on next save.
                // Additionally this can happen if an item was deleted from DB but still exists for the player
                TC_LOG_DEBUG("transmogrification", "Item entry (Entry: %u, itemGUID: %u, playerGUID: %u) does not exist, ignoring.", fakeEntry, itemGUID.GetCounter(), GetGUID().GetCounter());
            }
        } while (result->NextRow());
    }
}

void Player::_SaveCustomSettings()
{
    std::ostringstream data;
    for (uint16 value : _customFlags)
        data << value << ' ';

    if (CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CUSTOM_SETTINGS))
    {
        stmt->setUInt32(0, GetGUID().GetCounter());
        stmt->setString(1, data.str());
        CharacterDatabase.Execute(stmt);
    }
}

void Player::_SaveTransmogrifications()
{
    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    uint32 lowguid = GetGUID().GetCounter();
    
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TRANSMOGRIFICATIONS);
    stmt->setUInt32(0, lowguid);
    trans->Append(stmt);

    if (HasTransmogrifications())
    {
        std::unordered_map<ObjectGuid, uint32> _map = GetTransmogrificationContainer();

        // Only save items that are in inventory / bank / etc
        std::vector<ObjectGuid> items = sTransmogrification->GetItemList(this);
        for (ObjectGuid itemGUID : items)
        {
            auto itr = _map.find(itemGUID);
            if (itr == _map.end())
                continue;

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TRANSMOGRIFICATION);
            stmt->setUInt32(0, itr->first.GetCounter());
            stmt->setUInt32(1, itr->second);
            stmt->setUInt32(2, lowguid);
            trans->Append(stmt);
        }
    }

    CharacterDatabase.CommitTransaction(trans);
}

bool Player::HasCustomFlag(CustomFlagsIndex const i, CustomFlags const f) const
{
    uint16 index = i;
    uint16 value = f;

    if ((_customFlags[index] & value) != 0)
        return true;

    return false;
}

void Player::SetCustomFlags(CustomFlagsIndex const i, CustomFlags const f)
{
    uint16 index = i;
    uint16 value = f;
    _customFlags[index] = value;
}

void Player::AddCustomFlag(CustomFlagsIndex const i, CustomFlags const f)
{
    uint16 index = i;
    uint16 value = f;
    _customFlags[index] |= value;
}

void Player::RemoveCustomFlag(CustomFlagsIndex const i, CustomFlags const f)
{
    uint16 index = i;
    uint16 value = f;
    _customFlags[index] &= ~value;
}

uint16 Player::GetCustomFlags(CustomFlagsIndex const i) const
{
    uint16 index = i;
    return _customFlags[index];
}

uint32 Player::GetTransmogrificationEntry(ObjectGuid itemGUID) const
{
    auto it = _transmogrificationMap.find(itemGUID);
    if (it == _transmogrificationMap.end())
        return 0;

    return it->second;
}

bool Player::EraseTransmogrificationEntry(ObjectGuid itemGUID)
{
    return _transmogrificationMap.erase(itemGUID) != 0;
}

void Player::InsertTransmogrificationEntry(ObjectGuid itemGUID, uint32 entry)
{
    _transmogrificationMap[itemGUID] = entry;
}

uint32 Player::GetHiddenTransmogrificationEntry(uint8 itemIndex) const
{
    auto it = _transmogrificationHideMap.find(itemIndex);
    if (it == _transmogrificationHideMap.end())
        return 0;

    return it->second;
}
