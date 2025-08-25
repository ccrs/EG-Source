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

#include "BattlefieldMgr.h"
#include "Battlefield.h"
#include "Log.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "World.h"
#include "ZoneScript.h"
#include <algorithm>

BattlefieldMgr::BattlefieldMgr() : _updateTimer(0)
{
}

BattlefieldMgr::~BattlefieldMgr()
{
}

BattlefieldMgr* BattlefieldMgr::instance()
{
    static BattlefieldMgr instance;
    return &instance;
}

void BattlefieldMgr::Initialize()
{
    uint32 oldMSTime = getMSTime();
    uint32 count = 0;
    if (QueryResult result = WorldDatabase.Query("SELECT TypeId, ScriptName FROM battlefield_template"))
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 typeId = fields[0].GetUInt8();
            if (typeId >= BATTLEFIELD_BATTLEID_MAX)
            {
                TC_LOG_ERROR("sql.sql", "BattlefieldMgr::InitBattlefield: Invalid TypeId value {} in battlefield_template, skipped.", typeId);
                continue;
            }

            uint32 scriptId = sObjectMgr->GetScriptId(fields[1].GetString());
            Battlefield* scriptPointer = sScriptMgr->CreateBattlefield(scriptId);
            if (!scriptPointer)
                continue;

            std::unique_ptr<Battlefield> newBattlefield = std::unique_ptr<Battlefield>(scriptPointer);
            if (!newBattlefield->SetupBattlefield())
            {
                TC_LOG_INFO("server.loading", ">> Setting up battlefield with TypeId {} failed.", typeId);
            }
            else
            {
                _battlefieldContainer.emplace(newBattlefield->GetZoneId(), std::move(newBattlefield));
                TC_LOG_INFO("server.loading", ">> Setting up battlefield with TypeId {} succeeded.", typeId);
            }

            ++count;
        } while (result->NextRow());
    }

    TC_LOG_INFO("server.loading", ">> Loaded {} battlefields in {} ms", count, GetMSTimeDiffToNow(oldMSTime));
}

void BattlefieldMgr::Update(uint32 diff)
{
    _updateTimer += diff;
    if (_updateTimer <= BATTLEFIELD_OBJECTIVE_UPDATE_INTERVAL)
        return;

    for (BattlefieldContainer::value_type const& value : _battlefieldContainer)
    {
        if (value.second->IsEnabled())
            value.second->Update(_updateTimer);
    }
    _updateTimer = 0;
}

void BattlefieldMgr::ForEach(std::function<void(Battlefield*)> const& action)
{
    for (BattlefieldContainer::value_type const& value : _battlefieldContainer)
        action(value.second.get());
}

void BattlefieldMgr::HandlePlayerEnterZone(Player* player, uint32 zoneId)
{
    auto itr = _battlefieldContainer.find(zoneId);
    if (itr == _battlefieldContainer.end())
        return;

    itr->second->HandlePlayerEnterZone(player);
}

void BattlefieldMgr::HandlePlayerLeaveZone(Player* player, uint32 zoneId)
{
    auto itr = _battlefieldContainer.find(zoneId);
    if (itr == _battlefieldContainer.end())
        return;

    itr->second->HandlePlayerLeaveZone(player);
}

Battlefield* BattlefieldMgr::GetBattlefield(uint32 zoneId) const
{
    auto itr = _battlefieldContainer.find(zoneId);
    if (itr == _battlefieldContainer.end())
        return nullptr;

    return itr->second.get();
}

Battlefield* BattlefieldMgr::GetBattlefield(BattlefieldBattleId battleId) const
{
    auto itr = std::find_if(_battlefieldContainer.begin(), _battlefieldContainer.end(), [battleId](BattlefieldContainer::value_type const& a) -> bool
    {
        return a.second->GetId() == battleId;
    });
    return itr != _battlefieldContainer.end() ? itr->second.get() : nullptr;
}

ZoneScript* BattlefieldMgr::GetZoneScript(uint32 zoneId) const
{
    return GetBattlefield(zoneId);
}

ZoneScript* BattlefieldMgr::GetZoneScript(BattlefieldBattleId battleId) const
{
    return GetBattlefield(battleId);
}

Battlefield* BattlefieldMgr::GetEnabledBattlefield(uint32 zoneId) const
{
    auto itr = _battlefieldContainer.find(zoneId);
    if (itr == _battlefieldContainer.end())
        return nullptr;

    if (itr->second->IsEnabled())
        return itr->second.get();

    return nullptr;
}

Battlefield* BattlefieldMgr::GetEnabledBattlefield(BattlefieldBattleId battleId) const
{
    auto itr = std::find_if(_battlefieldContainer.begin(), _battlefieldContainer.end(), [battleId](BattlefieldContainer::value_type const& a) -> bool
    {
        return a.second->GetId() == battleId && a.second->IsEnabled();
    });
    return itr != _battlefieldContainer.end() ? itr->second.get() : nullptr;
}
