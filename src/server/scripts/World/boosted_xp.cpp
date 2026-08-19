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

#include "CustomFunctions.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Log.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Util.h"
#include "World.h"
#include "WorldSession.h"

namespace
{
    bool ComputeBoostedDayActive()
    {
        uint32 const dayMask = sWorld->getIntConfig(CONFIG_XP_BOOST_DAYMASK);
        if (!dayMask)
            return false;

        tm localTm = TimeBreakdown(GameTime::GetGameTime());
        return (dayMask & (1 << localTm.tm_wday)) != 0;
    }

    void ClearManualPvPFlags()
    {
        uint32 const flags = PLAYER_FLAGS_CONTESTED_PVP | PLAYER_FLAGS_IN_PVP | PLAYER_FLAGS_PVP_TIMER;

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ALL_CHARACTERS_REM_PLAYER_FLAGS);
        stmt->setUInt32(0, flags);
        stmt->setUInt32(1, flags);
        CharacterDatabase.Execute(stmt);

        for (std::pair<uint32 const, WorldSession*> const& session : sWorld->GetAllSessions())
        {
            if (!session.second)
                continue;

            Player* player = session.second->GetPlayer();
            if (!player)
                continue;

            player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP | PLAYER_FLAGS_PVP_TIMER);
            player->ResetContestedPvP();
        }
    }

    void ApplyBoostedDayPvPState(bool active)
    {
        if (!active)
            ClearManualPvPFlags();

        for (std::pair<uint32 const, WorldSession*> const& session : sWorld->GetAllSessions())
        {
            if (!session.second)
                continue;

            Player* player = session.second->GetPlayer();
            if (!player || !player->IsInWorld())
                continue;

            player->RefreshForcedPvPState(!active);
        }
    }
}

class xp_boost_WorldScript : public WorldScript
{
public:
    xp_boost_WorldScript() : WorldScript("xp_boost_WorldScript"), _nextCheck(0) { }

    void OnConfigLoad(bool reload) override
    {
        if (reload)
        {
            CheckTransition();
            return;
        }

        _nextCheck = GetLocalHourTimestamp(GameTime::GetGameTime(), 0, true);

        if (!sWorld->getIntConfig(CONFIG_XP_BOOST_DAYMASK))
            return;

        bool const active = ComputeBoostedDayActive();
        EG::SetBoostedDay(active);
        TC_LOG_INFO("misc", "Boosted day is {}.", active ? "active" : "inactive");

        if (!active)
            ClearManualPvPFlags();
    }

    void OnUpdate(uint32 /*diff*/) override
    {
        if (GameTime::GetGameTime() >= _nextCheck)
            CheckTransition();
    }

private:
    void CheckTransition()
    {
        _nextCheck = GetLocalHourTimestamp(GameTime::GetGameTime(), 0, true);

        bool const active = ComputeBoostedDayActive();
        if (active == EG::IsBoostedDay())
            return;

        EG::SetBoostedDay(active);
        TC_LOG_INFO("misc", "Boosted day {}, refreshing forced PvP state for all characters.", active ? "started" : "ended");

        ApplyBoostedDayPvPState(active);
    }

    time_t _nextCheck;
};

class xp_boost_PlayerScript : public PlayerScript
{
public:
    xp_boost_PlayerScript() : PlayerScript("xp_boost_PlayerScript") { }

    void OnGiveXP(Player* /*player*/, uint32& amount, Unit* /*unit*/) override
    {
        if (EG::IsBoostedDay())
            amount *= sWorld->getRate(RATE_XP_BOOST);
    }

    void OnReputationChange(Player* /*player*/, uint32 /*factionId*/, int32& standing, bool incremental) override
    {
        if (incremental && EG::IsBoostedDay())
            standing = int32(float(standing) * 1.25f);
    }
};

void AddSC_xp_boost()
{
    new xp_boost_WorldScript();
    new xp_boost_PlayerScript();
}
