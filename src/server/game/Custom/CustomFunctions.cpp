#include "Player.h"
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


void Player::_SaveCustomSettings()
{
    std::ostringstream data;
    for (uint16 value : _customFlags)
        data << value << ' ';

    if (PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CUSTOM_SETTINGS))
    {
        stmt->setUInt32(0, GetGUID().GetCounter());
        stmt->setString(1, data.str());
        CharacterDatabase.Execute(stmt);
    }
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

