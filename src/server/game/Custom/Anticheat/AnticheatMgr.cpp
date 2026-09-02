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

#include "AnticheatMgr.h"
#include "AccountMgr.h"
#include "Battleground.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Log.h"
#include "Map.h"
#include "MapManager.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Realm.h"
#include "SharedDefines.h"
#include "SpellAuras.h"
#include "World.h"
#include "WorldSession.h"
#include <algorithm>
#include <cmath>
#include <sstream>

constexpr uint32 LANG_ANTICHEAT_ALERT = 30087;
constexpr uint32 LANG_ANTICHEAT_TELEPORT = 30088;
constexpr uint32 LANG_ANTICHEAT_IGNORECONTROL = 30089;
constexpr uint32 LANG_ANTICHEAT_DUEL = 30090;
constexpr uint32 LANG_ANTICHEAT_BG_EXPLOIT = 30091;
constexpr uint32 LANG_ANTICHEAT_COUNTERMEASURE = 30092;

enum AnticheatSpells
{
    SHACKLES = 38505,
    LFG_SPELL_DUNGEON_DESERTER = 71041,
    BG_SPELL_DESERTER = 26013,
    SILENCED = 23207,
    RESURRECTION_SICKNESS = 15007,
    SLOWDOWN = 61458
};

namespace
{
    constexpr float MIN_MOVEMENT_DELTA_2D = 0.1f;
    constexpr float DEFAULT_TRANSPORT_MAX_DIST_2D = 70.0f;
    constexpr float DEEPRUN_TRAM_TRANSPORT_MAX_DIST_2D = 3000.0f;
    constexpr float ANTICHEAT_INVALID_HEIGHT = -100000.0f;

    bool IsUsableHeight(float z)
    {
        return std::isfinite(z) && z > ANTICHEAT_INVALID_HEIGHT;
    }

    bool HasAnyLiquidStatus(Player const* player, uint32 flags)
    {
        return (player->GetLiquidStatus() & flags) != 0;
    }

    bool HasTransportMovementExemption(Player* player, MovementInfo const& movementInfo)
    {
        GameObject* transportGobj = player->GetMap()->GetGameObject(movementInfo.transport.guid);
        if (!transportGobj)
            return false;

        float maxDist2d = player->GetMapId() == 369 ? DEEPRUN_TRAM_TRANSPORT_MAX_DIST_2D : DEFAULT_TRANSPORT_MAX_DIST_2D;
        return transportGobj->IsTransport() || transportGobj->IsWithinDist(player, maxDist2d, false);
    }
}

AnticheatMgr::AnticheatMgr()
{
    /*
    **    May you do good and not evil.
    **    May you find forgiveness for yourself and forgive others.
    **    May you share freely, never taking more than you give.
    **    CTHULHU is watching ^(;,;)^
    */

    // create a conf to establish a speed limit tolerance over server rate set speed
    // this is done so we can ignore minor violations that are not false positives such as going 1 or 2 over the speed limit
    _assignedspeeddiff = sWorld->getIntConfig(CONFIG_ANTICHEAT_SPEED_LIMIT_TOLERANCE);
    _alertFrequency = sWorld->getIntConfig(CONFIG_ANTICHEAT_ALERT_FREQUENCY);
    if (_alertFrequency < 1)
        _alertFrequency = 1;
    _ingameNotificationThreshold = sWorld->getIntConfig(CONFIG_ANTICHEAT_REPORTS_INGAME_NOTIFICATION);
}

AnticheatMgr::~AnticheatMgr()
{
}

AnticheatMgr* AnticheatMgr::instance()
{
    static AnticheatMgr instance;
    return &instance;
}

void AnticheatMgr::Initialize()
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        return;

    _LoadBlockedLuaFunctions();
}

void AnticheatMgr::HandlePlayerLogin(Player* player)
{
    time_t gameTime = GameTime::GetGameTime();
    time_t today = (gameTime / DAY) * DAY;
    auto loginStmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ANTICHEAT_REPORTS);
    loginStmt->setUInt32(0, player->GetSession()->GetAccountId());
    loginStmt->setUInt32(1, realm.Id.Realm);
    loginStmt->setUInt32(2, player->GetGUID().GetCounter());
    loginStmt->setUInt32(3, uint32(today));
    PreparedQueryResult resultDB = LoginDatabase.Query(loginStmt);

    std::unique_lock<std::shared_mutex> lock(_playersMutex);
    AnticheatData& playerAntiCheatData = _players[player->GetGUID().GetCounter()];
    playerAntiCheatData.SetPosition(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
    if (resultDB)
    {
        Field* fields = resultDB->Fetch();
        playerAntiCheatData.SetCreationTime(fields[4].GetUInt32());
        playerAntiCheatData.SetAverage(fields[5].GetFloat());
        playerAntiCheatData.SetTotalReports(fields[6].GetUInt32());
        playerAntiCheatData.SetTypeReports(SPEED_HACK_REPORT, fields[7].GetUInt32());
        playerAntiCheatData.SetTypeReports(FLY_HACK_REPORT, fields[8].GetUInt32());
        playerAntiCheatData.SetTypeReports(JUMP_HACK_REPORT, fields[9].GetUInt32());
        playerAntiCheatData.SetTypeReports(WALK_WATER_HACK_REPORT, fields[10].GetUInt32());
        playerAntiCheatData.SetTypeReports(TELEPORT_PLANE_HACK_REPORT, fields[11].GetUInt32());
        playerAntiCheatData.SetTypeReports(CLIMB_HACK_REPORT, fields[12].GetUInt32());
        playerAntiCheatData.SetTypeReports(TELEPORT_HACK_REPORT, fields[13].GetUInt32());
        playerAntiCheatData.SetTypeReports(IGNORE_CONTROL_REPORT, fields[14].GetUInt32());
        playerAntiCheatData.SetTypeReports(ZAXIS_HACK_REPORT, fields[15].GetUInt32());
        playerAntiCheatData.SetTypeReports(ANTISWIM_HACK_REPORT, fields[16].GetUInt32());
        playerAntiCheatData.SetTypeReports(GRAVITY_HACK_REPORT, fields[17].GetUInt32());
        playerAntiCheatData.SetTypeReports(ANTIKNOCK_BACK_HACK_REPORT, fields[18].GetUInt32());
        playerAntiCheatData.SetTypeReports(NO_FALL_DAMAGE_HACK_REPORT, fields[19].GetUInt32());
        playerAntiCheatData.SetTypeReports(COUNTER_MEASURES_REPORT, fields[20].GetUInt32());
        playerAntiCheatData.SetDailyReportState(true);
    }
}

void AnticheatMgr::HandlePlayerLogout(Player* player)
{
    if (!player)
        return;

    std::unique_lock<std::shared_mutex> lock(_playersMutex);
    _players.erase(player->GetGUID().GetCounter());
}

void AnticheatMgr::SavePlayerData(Player* player)
{
    if (!player || !player->GetSession())
        return;

    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        return;

    uint32 lowGUID = player->GetGUID().GetCounter();
    std::shared_lock<std::shared_mutex> lock(_playersMutex);
    auto itr = _players.find(lowGUID);
    if (itr == _players.end())
        return;

    AnticheatData const& playerData = itr->second;
    if (!playerData.GetDailyReportState())
        return;

    time_t gameTime = GameTime::GetGameTime();
    time_t today = (gameTime / DAY) * DAY;
    auto saveStmt = LoginDatabase.GetPreparedStatement(LOGIN_REP_ANTICHEAT_REPORTS);
    saveStmt->setUInt32(0,  player->GetSession()->GetAccountId());
    saveStmt->setUInt32(1,  realm.Id.Realm);
    saveStmt->setUInt32(2,  lowGUID);
    saveStmt->setUInt32(3,  uint32(today));
    saveStmt->setUInt32(4,  playerData.GetCreationTime());
    saveStmt->setFloat (5,  playerData.GetAverage());
    saveStmt->setUInt32(6,  playerData.GetTotalReports());
    saveStmt->setUInt32(7,  playerData.GetTypeReports(SPEED_HACK_REPORT));
    saveStmt->setUInt32(8,  playerData.GetTypeReports(FLY_HACK_REPORT));
    saveStmt->setUInt32(9,  playerData.GetTypeReports(JUMP_HACK_REPORT));
    saveStmt->setUInt32(10, playerData.GetTypeReports(WALK_WATER_HACK_REPORT));
    saveStmt->setUInt32(11, playerData.GetTypeReports(TELEPORT_PLANE_HACK_REPORT));
    saveStmt->setUInt32(12, playerData.GetTypeReports(CLIMB_HACK_REPORT));
    saveStmt->setUInt32(13, playerData.GetTypeReports(TELEPORT_HACK_REPORT));
    saveStmt->setUInt32(14, playerData.GetTypeReports(IGNORE_CONTROL_REPORT));
    saveStmt->setUInt32(15, playerData.GetTypeReports(ZAXIS_HACK_REPORT));
    saveStmt->setUInt32(16, playerData.GetTypeReports(ANTISWIM_HACK_REPORT));
    saveStmt->setUInt32(17, playerData.GetTypeReports(GRAVITY_HACK_REPORT));
    saveStmt->setUInt32(18, playerData.GetTypeReports(ANTIKNOCK_BACK_HACK_REPORT));
    saveStmt->setUInt32(19, playerData.GetTypeReports(NO_FALL_DAMAGE_HACK_REPORT));
    saveStmt->setUInt32(20, playerData.GetTypeReports(COUNTER_MEASURES_REPORT));
    LoginDatabase.Execute(saveStmt);
}

void AnticheatMgr::OnPlayerMove(Player* player, MovementInfo const& movementInfo, uint32 opcode, uint32 rawMovementFlags)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        return;

    uint32 key = player->GetGUID().GetCounter();

    std::shared_lock<std::shared_mutex> lock(_playersMutex);
    auto itr = _players.find(key);
    if (itr == _players.end())
        return;

    AnticheatData& data = itr->second;
    if (!data.IsDirty())
        if (!AccountMgr::IsAdminAccount(player->GetSession()->GetSecurity()) || sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE_ON_GM))
            _StartHackDetection(player, movementInfo, opcode, rawMovementFlags, data);

    data.SetLastMovementInfo(movementInfo);
    data.SetLastOpcode(opcode);
    data.SetLastRawMovementFlags(rawMovementFlags);
}

uint32 AnticheatMgr::GetTotalReports(uint32 lowGUID) const
{
    std::shared_lock<std::shared_mutex> lock(_playersMutex);
    auto itr = _players.find(lowGUID);
    return itr != _players.end() ? itr->second.GetTotalReports() : 0;
}

float AnticheatMgr::GetAverage(uint32 lowGUID) const
{
    std::shared_lock<std::shared_mutex> lock(_playersMutex);
    auto itr = _players.find(lowGUID);
    return itr != _players.end() ? itr->second.GetAverage() : 0;
}

uint32 AnticheatMgr::GetTypeReports(uint32 lowGUID, uint8 type) const
{
    std::shared_lock<std::shared_mutex> lock(_playersMutex);
    auto itr = _players.find(lowGUID);
    return itr != _players.end() ? itr->second.GetTypeReports(type) : 0;
}

uint32 AnticheatMgr::GetElapsedSeconds(uint32 lowGUID) const
{
    std::shared_lock<std::shared_mutex> lock(_playersMutex);
    auto itr = _players.find(lowGUID);
    if (itr == _players.end())
        return 0;
    uint32 creationTime = itr->second.GetCreationTime();
    if (!creationTime)
        return 0;
    return getMSTimeDiff(creationTime, getMSTime()) / IN_MILLISECONDS;
}

// .anticheat delete gm cmd
void AnticheatMgr::AnticheatDeleteCommand(uint32 guid)
{
    if (!guid)
        return;

    {
        std::unique_lock<std::shared_mutex> lock(_playersMutex);
        auto itr = _players.find(guid);
        if (itr != _players.end())
            itr->second.ResetReports();
    }
    auto delStmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_ANTICHEAT_REPORTS);
    delStmt->setUInt32(0, realm.Id.Realm);
    delStmt->setUInt32(1, guid);
    LoginDatabase.Execute(delStmt);
}

void AnticheatMgr::AnticheatPurgeCommand()
{
    {
        std::unique_lock<std::shared_mutex> lock(_playersMutex);
        for (AnticheatPlayersDataMap::value_type& pair : _players)
            pair.second.ResetReports();
    }
    LoginDatabase.Execute("TRUNCATE TABLE account_anticheat_reports;");
}

void AnticheatMgr::ResetDailyReportStates()
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        return;

    std::unique_lock<std::shared_mutex> lock(_playersMutex);
    for (AnticheatPlayersDataMap::value_type& pair : _players)
        pair.second.ResetReports();
}

bool AnticheatMgr::CheckIsLuaCheater(uint32 accountId)
{
    auto pstmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ANTICHEAT_LUA_CHEATERS);
    pstmt->setUInt32(0, accountId);
    auto result = LoginDatabase.Query(pstmt);
    if (result)
        return true;

    return false;
}

bool AnticheatMgr::CheckBlockedLuaFunctions(AccountData const* accountData, Player* player)
{
    if (!accountData || !player || !player->GetSession())
        return false;

    for (auto const& kv : _luaBlockedFunctions)
    {
        if (!kv.second)
            continue;

        for (uint8 i = 0; i < NUM_ACCOUNT_DATA_TYPES; ++i)
        {
            std::string const& currentData = accountData[i].Data;
            std::size_t pos = currentData.find(kv.first);
            if (pos == std::string::npos)
                continue;

            static constexpr std::size_t defaultLength = 200;
            std::size_t minPos = pos < 50 ? 0 : pos - 50;
            std::size_t length = std::min(defaultLength, currentData.length() - minPos);
            std::string macro = currentData.substr(minPos, length);

            TC_LOG_INFO("anticheat", "ANTICHEAT COUNTER MEASURE::Player {} has inaccessible LUA MACRO, placing on watch list", player->GetName());
            _SaveLuaCheater(player->GetSession()->GetAccountId(), realm.Id.Realm, player->GetGUID().GetCounter(), macro);
            return true;
        }
    }

    return false;
}

/*
 * Private class members
 */

void AnticheatMgr::_LoadBlockedLuaFunctions()
{
    if (!sWorld->getBoolConfig(CONFIG_LUABLOCKER_ENABLE))
    {
        TC_LOG_INFO("server.loading", ">> Anticheat.LUAblocker conf is set to 0");
        return;
    }
    uint32 oldmsTime = getMSTime();
    auto pstmt = WorldDatabase.GetPreparedStatement(WORLD_SEL_ANTICHEAT_FUNCTIONS);
    auto result = WorldDatabase.Query(pstmt);
    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Anticheat loaded 0 LUA blocked private functions");
        return;
    }
    uint32 count = 0;
    do
    {
        auto fields = result->Fetch();
        _luaBlockedFunctions[fields[0].GetString()] = fields[1].GetBool();
        ++count;
    } while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Anticheat loaded {} LUA blocked private functions in {} ms", count, GetMSTimeDiffToNow(oldmsTime));
}

void AnticheatMgr::_SaveLuaCheater(uint32 accountId, uint32 realmId, uint32 guid, std::string macro)
{
    auto pstmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ANTICHEAT_LUA_CHEATERS);
    pstmt->setUInt32(0, accountId);
    pstmt->setUInt32(1, realmId);
    pstmt->setUInt32(2, guid);
    pstmt->setString(3, macro);
    LoginDatabase.Execute(pstmt);
}

void AnticheatMgr::_StartHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, uint32 rawMovementFlags, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
        return;

    // GMs are the enforcer of the server, they should be exempt.
    if (player->IsGameMaster())
        return;

    if (player->IsInFlight() || player->GetTransport() || player->GetVehicle())
        return;

    // Dear future me. Please forgive me.
    // I can't even begin to express how sorry I am for this order
    // If you bought this you have been scammed.
    // Visit TC: https://discord.com/invite/HPP3wNh for help on the Open Source Anticheat
    // The project compromised of various developers of the open source scene and we hang out there.
    // We would never charge for modules or "lessons"
    _SpeedHackDetection(player, movementInfo, data);
    _FlyHackDetection(player, movementInfo, rawMovementFlags, data);
    _TeleportHackDetection(player, movementInfo, data);
    _JumpHackDetection(player, movementInfo, opcode, data);
    _TeleportPlaneHackDetection(player, movementInfo, opcode, data);
    _ClimbHackDetection(player, movementInfo, opcode, data);
    _IgnoreControlHackDetection(player, movementInfo, opcode, data);
    _GravityHackDetection(player, movementInfo, data);
    if (HasAnyLiquidStatus(player, LIQUID_MAP_WATER_WALK))
        _WalkOnWaterHackDetection(player, movementInfo, rawMovementFlags, data);
    else
        _ZAxisHackDetection(player, movementInfo, data);

    if (HasAnyLiquidStatus(player, LIQUID_MAP_UNDER_WATER))
        _AntiSwimHackDetection(player, movementInfo, opcode, data);
    _AntiKnockBackHackDetection(player, movementInfo, data);
    _NoFallDamageDetection(player, movementInfo, data);
    if (Battleground* bg = player->GetBattleground())
    {
        if (bg->GetStatus() == STATUS_WAIT_JOIN)
        {
            _BGStartExploitDetection(player, movementInfo, data);
        }
    }
}

void AnticheatMgr::_SpeedHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_SPEEDHACK_ENABLE))
        return;

    if (HasTransportMovementExemption(player, movementInfo))
        return;

    // sometimes I believe the compiler ignores all my comments
    float distance2D = movementInfo.pos.GetExactDist2d(&data.GetLastMovementInfo().pos);

    // We don't need to check for a speedhack if the player hasn't moved
    // This is necessary since MovementHandler fires if you rotate the camera in place
    if (distance2D < 0.1f)
        return;

    uint8 moveType = 0;

    // we need to know HOW is the player moving
    // TO-DO: Should we check the incoming movement flags?
    if (player->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
        moveType = MOVE_SWIM;
    else if (player->IsFlying())
        moveType = MOVE_FLIGHT;
    else if (player->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
        moveType = MOVE_WALK;
    else
        moveType = MOVE_RUN;

    // how many yards the player can do in one sec.
    // We remove the added speed for jumping because otherwise permanently jumping doubles your allowed speed
    float speedRate = player->GetSpeed(UnitMoveType(moveType));

    // how long the player took to move to here.
    uint32 timeDiff = getMSTimeDiff(data.GetLastMovementInfo().time, movementInfo.time);

    // Ah ah ah! You'll never understand why this one works. Or will you?
    // This covers packet manipulation
    if (data.GetLastMovementInfo().time > movementInfo.time || !timeDiff)
    {
        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_TIMEMANIPULATION))
        {
            if (Aura* slowcheater = player->AddAura(SLOWDOWN, player)) // SLOWDOWN
                slowcheater->SetDuration(10000);

            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
                _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] TIME MANIPULATION COUNTER MEASURE ALERT");

            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
            {
                std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
                TC_LOG_INFO("anticheat", "ANTICHEAT COUNTER MEASURE:: {} Time Diff Corrected(Map: {}) (possible Zero Time Manipulation) - Flagged at: {}", player->GetName(), player->GetMapId(), goXYZ);
            }
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
            {
                std::string str = "|cFFFFFC00 TIME MANIPULATION COUNTER MEASURE ALERT";
                sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
            }
            _BuildReport(player, COUNTER_MEASURES_REPORT, data);
        }
        _LogInfo(player, "Time Manipulation - Hack detected");
        _BuildReport(player, SPEED_HACK_REPORT, data);
        return;
    }

    // this is the distance doable by the player in 1 sec, using the time done to move to this point.
    float clientSpeedRate = distance2D * 1000.0f / float(timeDiff);

    // we create a diff speed in uint32 for further precision checking to avoid legit fall and slide

    // We did the (uint32) cast to accept a margin of tolerance for seasonal spells and buffs such as sugar rush
    // We check the last MovementInfo for the falling flag since falling down a hill and sliding a bit triggered a false positive
    if ((clientSpeedRate >= speedRate + float(_assignedspeeddiff)) && !data.GetLastMovementInfo().HasMovementFlag(MOVEMENTFLAG_FALLING))
    {
        if (!player->CanTeleport())
        {
            _LogInfo(player, "Speed-Hack (Speed Movement at " + std::to_string(clientSpeedRate) + " above allowed Server Set rate " + std::to_string(speedRate) + ".) detected");
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_SPEEDHACK))
            {
                if (Aura* slowcheater = player->AddAura(SLOWDOWN, player)) // SLOWDOWN
                    slowcheater->SetDuration(10000);

                if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
                {
                    std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
                    TC_LOG_INFO("anticheat.module", "ANTICHEAT COUNTER MEASURE:: {} Speed Hack Countered and has been set to Server Rate - Flagged at: {}", player->GetName(), goXYZ);
                }
                if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
                    _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] SPEED HACK COUNTER MEASURE ALERT");

                if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
                {
                    std::string str = "|cFFFFFC00 SPEED HACK COUNTER MEASURE ALERT";
                    sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
                }
                _BuildReport(player, COUNTER_MEASURES_REPORT, data);
            }
            _BuildReport(player, SPEED_HACK_REPORT, data);
        }
        return;
    }
}

void AnticheatMgr::_FlyHackDetection(Player* player, MovementInfo const& /*movementInfo*/, uint32 rawMovementFlags, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_FLYHACK_ENABLE))
        return;

    if (!(data.GetLastRawMovementFlags() & MOVEMENTFLAG_FLYING))
        return;

    if (player->HasUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING))
        return;

    //we check to see if they have legal flight auras
    if (player->HasAuraType(SPELL_AURA_FLY) || player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || player->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED))
        return;

    /*Thanks to @LilleCarl for info to check extra flag*/
    bool stricterChecks = true;
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_FLYHACKSTRICT_ENABLE))
    {
        // super strict way to check, you can only ascend\descend in water and air, we check u are ascending\descending and not in water.
        // we are not checking for legal flight here because those checks were dont earlier.
        stricterChecks = !((rawMovementFlags & (MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) && !player->IsInWater());
    }

    // if you are not flying and not ascending then we do a return, you are then not guilty.
    if (!(rawMovementFlags & (MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING)) && stricterChecks)
        return;

    _LogInfo(player, "Fly-Hack detected");
    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_FLYHACK))
    {
        // Drop them with a op code set if they use a exploit or hack app
        WorldPacket cheater(12);
        cheater.SetOpcode(SMSG_MOVE_UNSET_CAN_FLY);
        cheater << player->GetPackGUID();
        cheater << uint32(0);
        player->SendMessageToSet(&cheater, true);

        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
            _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] FLY HACK COUNTER MEASURE ALERT");

        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
        {
            std::string str = "|cFFFFFC00 FLY HACK COUNTER MEASURE ALERT";
            sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
        }
        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
        {
            std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
            TC_LOG_INFO("anticheat", "ANTICHEAT COUNTER MEASURE:: Fly Hack detected player {} ({}) - SMSG_MOVE_UNSET_CAN_FLY Set - Flagged at: {}", player->GetName(), player->GetGUID().ToString(), goXYZ);
        }
        _BuildReport(player, COUNTER_MEASURES_REPORT, data);
    }

    _BuildReport(player, FLY_HACK_REPORT, data);
}

void AnticheatMgr::_TeleportHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_TELEPORTHACK_ENABLE))
        return;

    float lastX = data.GetLastMovementInfo().pos.GetPositionX();
    float lastY = data.GetLastMovementInfo().pos.GetPositionY();
    float lastZ = data.GetLastMovementInfo().pos.GetPositionZ();
    if (lastX == 0.0f && lastY == 0.0f && lastZ == 0.0f)
        return;

    float newX = movementInfo.pos.GetPositionX();
    float newY = movementInfo.pos.GetPositionY();
    float xDiff = fabs(lastX - newX);
    float yDiff = fabs(lastY - newY);

    if (player->IsFalling() || (player->IsFalling() && player->IsMounted()))
        return;

    if (HasTransportMovementExemption(player, movementInfo))
        return;

    /* Please work */
    if ((xDiff >= 50.0f || yDiff >= 50.0f) && !player->CanTeleport() && !player->IsBeingTeleported())// teleport helpers in play
    {
        if (player->duel)
        {
            Player* opponent = player->duel->Opponent;
            if (data.GetTotalReports() > _ingameNotificationThreshold)
                _NotifyGameMasters(player, "Possible Teleport Hack Detected! While Dueling [|cFF60FF00" + std::string(opponent->GetName()) + "|cFF00FFFF]", LANG_ANTICHEAT_DUEL, data);

            _LogInfo(player, "DUEL ALERT Teleport-Hack detected");
            _LogInfo(opponent, "DUEL ALERT Teleport-Hack detected");
        }
        else
        {
            if (data.GetTotalReports() > _ingameNotificationThreshold)
                _NotifyGameMasters(player, "Possible Teleport Hack Detected!", LANG_ANTICHEAT_TELEPORT, data);

            _LogInfo(player, "Teleport-Hack detected");
        }

        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_TELEPORT))
        {
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
            {
                std::string LastgoXYZ = ".go xyz " + std::to_string(lastX) + " " + std::to_string(lastY) + " " + std::to_string(lastZ + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
                TC_LOG_INFO("anticheat", "ANTICHEAT COUNTER MEASURE:: {} TELEPORT HACK REVERTED PLAYER BACK TO {}", player->GetName(), LastgoXYZ);
            }
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
                _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] TELEPORT COUNTER MEASURE ALERT");

            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
            {
                std::string str = "|cFFFFFC00 TELEPORT COUNTER MEASURE ALERT";
                sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
            }
            player->TeleportTo(player->GetMapId(), lastX, lastY, lastZ, player->GetOrientation());
            _BuildReport(player, COUNTER_MEASURES_REPORT, data);
        }
        if (player->duel)
        {
            // Keep the duel opponent only as context. Do not assign a cheat report to the opponent.
        }
        _BuildReport(player, TELEPORT_HACK_REPORT, data);
    }
    else if (player->CanTeleport())// if we hit the teleport helpers in the source then we return it to false
        player->SetCanTeleport(false);
}

void AnticheatMgr::_JumpHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_JUMPHACK_ENABLE))
        return;

    float ground_Z = movementInfo.pos.GetPositionZ() - player->GetMapHeight(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY(), movementInfo.pos.GetPositionZ());

    bool no_fly_auras = !(player->HasAuraType(SPELL_AURA_FLY) || player->HasAuraType(SPELL_AURA_MOD_INCREASE_VEHICLE_FLIGHT_SPEED) || player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) || player->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED) || player->HasAuraType(SPELL_AURA_MOD_MOUNTED_FLIGHT_SPEED_ALWAYS));
    bool no_fly_flags = ((movementInfo.flags & (MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING)) == 0);
    bool no_swim_in_water = !player->IsInWater();
    bool no_swim_above_water = movementInfo.pos.GetPositionZ() - 7.0f >= player->GetMap()->GetWaterLevel(movementInfo.pos.GetPositionX(), movementInfo.pos.GetPositionY());
    bool no_swim_water = no_swim_in_water && no_swim_above_water;

    // Chain or double multi jumping is not a thing in 335
    if (data.GetLastOpcode() == MSG_MOVE_JUMP && opcode == MSG_MOVE_JUMP)
    {
        _LogInfo(player, "Jump-Hack detected");
        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_JUMPHACK))
        {
            player->GetMotionMaster()->MoveFall();

            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
            {
                std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
                TC_LOG_INFO("anticheat.module", "ANTICHEAT COUNTER MEASURE:: {} JUMP Hack Countered and has been set to fall - Flagged at: {}", player->GetName(), goXYZ);
            }
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
                _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] JUMP HACK COUNTER MEASURE ALERT");

            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
            {
                std::string str = "|cFFFFFC00 JUMP HACK COUNTER MEASURE ALERT";
                sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
            }
            _BuildReport(player, COUNTER_MEASURES_REPORT, data);
        }
        _BuildReport(player, JUMP_HACK_REPORT, data);
    }
    else if (no_fly_auras && no_fly_flags && no_swim_water)
    {
        if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ADV_JUMPHACK_ENABLE))
            return;

        if (data.GetLastOpcode() == MSG_MOVE_JUMP && !player->IsFalling())
            return;

        float distance2D = movementInfo.pos.GetExactDist2d(&data.GetLastMovementInfo().pos);

        // This is necessary since MovementHandler fires if you rotate the camera in place
        if (distance2D < 0.1f)
            return;

        // The anticheat is disabled on transports, so we need to be sure that the player is indeed on a transport.
        GameObject* transportGobj = player->GetMap()->GetGameObject(movementInfo.transport.guid);
        if (transportGobj && transportGobj->IsTransport())
            return;

        // The anticheat check is disabled on Elevators, so we need to be sure that the player is indeed on a transport.
        if (player->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
            return;

        if (!player->HasUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY) && movementInfo.jump.zspeed < -10.0f)
            return;

        if (player->HasAuraType(SPELL_AURA_WATER_WALK) || player->HasAuraType(SPELL_AURA_FEATHER_FALL) || player->HasAuraType(SPELL_AURA_SAFE_FALL))
            return;

        // We exempt select areas found in 335 to prevent false hack hits
        if (player->GetAreaId())
        {
            switch (player->GetAreaId())
            {
                case 4273: //Celestial Planetarium Observer Battle has a narrow path that false flags
                    return;
            }
        }

        if (ground_Z > 5.0f && movementInfo.pos.GetPositionZ() >= player->GetPositionZ())
        {
            _LogInfo(player, "Stricter Jump-Hack detected");
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ADVJUMPHACK))
            {
                player->GetMotionMaster()->MoveFall();

                if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
                {
                    std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
                    TC_LOG_INFO("anticheat.module", "ANTICHEAT COUNTER MEASURE:: {} ADVANCE JUMP Hack Countered and has been set to fall - Flagged at: {}", player->GetName(), goXYZ);
                }
                if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
                    _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] ADVANCE JUMP HACK COUNTER MEASURE ALERT");

                if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
                {
                    std::string str = "|cFFFFFC00 ADVANCE JUMP HACK COUNTER MEASURE ALERT";
                    sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
                }
                _BuildReport(player, COUNTER_MEASURES_REPORT, data);
            }
            _BuildReport(player, JUMP_HACK_REPORT, data);
        }
    }
}

void AnticheatMgr::_TeleportPlaneHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_TELEPANEHACK_ENABLE))
        return;

    if (player->HasAuraType(SPELL_AURA_WATER_WALK) || player->HasAuraType(SPELL_AURA_WATER_BREATHING) || player->HasAuraType(SPELL_AURA_GHOST))
        return;

    float distance2D = movementInfo.pos.GetExactDist2d(&data.GetLastMovementInfo().pos);

    // We don't need to check for a water walking hack if the player hasn't moved
    // This is necessary since MovementHandler fires if you rotate the camera in place
    if (distance2D < 0.1f)
        return;

    if (data.GetLastOpcode() == MSG_MOVE_JUMP)
        return;

    if (opcode == (MSG_MOVE_FALL_LAND))
        return;

    if (player->GetLiquidStatus() == LIQUID_MAP_ABOVE_WATER)
        return;

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_SWIMMING))
        return;

    // If he is flying we dont need to check
    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING))
        return;

    // We exempt select areas found in 335 to prevent false hack hits
    if (player->GetAreaId())
    {
        switch (player->GetAreaId())
        {
            case 4273: //Celestial Planetarium Observer Battle has a narrow path that false flags
                return;
        }
    }

    float pos_z = player->GetPositionZ();
    float ground_Z = player->GetFloorZ();
    float groundZ = player->GetMapHeight(player->GetPositionX(), player->GetPositionY(), MAX_HEIGHT);
    float floorZ = player->GetMapHeight(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ());

    // we are not really walking there
    float zDiff = pos_z - ground_Z;
    if (IsUsableHeight(ground_Z) && IsUsableHeight(groundZ) && IsUsableHeight(floorZ) && std::fabs(groundZ - floorZ) < 0.01f && (zDiff > 2.0f || zDiff < -1.0f))
    {
        _LogInfo(player, "Teleport To Plane - Hack detected");
        _BuildReport(player, TELEPORT_PLANE_HACK_REPORT, data);
    }
}

// basic detection
void AnticheatMgr::_ClimbHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_CLIMBHACK_ENABLE))
        return;

    // in this case we don't care if they are "legal" flags, they are handled in another parts of the Anticheat Manager.
    if (player->IsInWater() || player->IsFlying() || player->IsFalling())
        return;

    // If the player jumped, we dont want to check for climb hack
    // This can lead to false positives for climbing game objects legit
    if (opcode == MSG_MOVE_JUMP)
        return;

    if (player->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
        return;

    // Ships and zeppelins are handled by GetTransport() guard in _StartHackDetection.
    // Elevators (GAMEOBJECT_TYPE_TRANSPORT) set ONTRANSPORT but are not Transport* objects,
    // so _StartHackDetection does not catch them — guard here explicitly.
    GameObject* transportGobj = player->GetMap()->GetGameObject(movementInfo.transport.guid);
    if (transportGobj && transportGobj->IsTransport())
        return;

    if (player->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
        return;

    Position const& lastPos = data.GetLastMovementInfo().pos;
    float diffz = movementInfo.pos.GetPositionZ() - lastPos.GetPositionZ();
    if (diffz <= 1.87f)
        return;

    float dist2D = movementInfo.pos.GetExactDist2d(&lastPos);
    float tanangle = dist2D / diffz;
    if (dist2D < MIN_MOVEMENT_DELTA_2D)
        return;

    if (!movementInfo.HasMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FALLING) && tanangle < 0.57735026919f)
    {
        _LogInfo(player, "Climb-Hack detected");
        _BuildReport(player, CLIMB_HACK_REPORT, data);
    }
}

void AnticheatMgr::_IgnoreControlHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_IGNORECONTROLHACK_ENABLE))
        return;

    if (data.GetLastOpcode() == MSG_MOVE_JUMP)
        return;

    if (opcode == MSG_MOVE_FALL_LAND)
        return;

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_SPLINE_ENABLED))
        return;

    // High latency causes root desync: the movement packet was in-flight before the root landed server-side
    if (player->GetSession()->GetLatency() >= 200)
        return;

    if (!player->HasAuraType(SPELL_AURA_MOD_ROOT) || player->GetVehicle())
        return;

    float deltaXY = movementInfo.pos.GetExactDist2d(&data.GetLastMovementInfo().pos);
    if (deltaXY < 0.5f)
        return;

    if (data.GetTotalReports() > _ingameNotificationThreshold)
        _NotifyGameMasters(player, "Possible Ignore Control Hack Detected!", LANG_ANTICHEAT_IGNORECONTROL, data);

    _LogInfo(player, Trinity::StringFormat("Ignore Control Hack detected (DeltaXY: {:.2f})", deltaXY));
    _BuildReport(player, IGNORE_CONTROL_REPORT, data);
}

void AnticheatMgr::_GravityHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_GRAVITY_ENABLE))
        return;

    if (player->HasAuraType(SPELL_AURA_FEATHER_FALL))
        return;

    if (data.GetLastOpcode() == MSG_MOVE_JUMP)
    {
        if (!player->HasUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY) && movementInfo.jump.zspeed < -10.0f)
        {
            _LogInfo(player, "Gravity-Hack detected");
            _BuildReport(player, GRAVITY_HACK_REPORT, data);
        }
    }
}

void AnticheatMgr::_WalkOnWaterHackDetection(Player* player, MovementInfo const& movementInfo, uint32 rawMovementFlags, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_WATERWALKHACK_ENABLE))
        return;

    float distance2D = movementInfo.pos.GetExactDist2d(&data.GetLastMovementInfo().pos);

    // We don't need to check for a waterwalk hack if the player hasn't moved
    // This is necessary since MovementHandler fires if you rotate the camera in place
    if (distance2D < 0.1f)
        return;

    // if we are a ghost we can walk on water
    if (!player->IsAlive())
        return;

    // Prevents the False Positive for water walking when you ressurrect.
    if (data.GetLastOpcode() == MSG_DELAY_GHOST_TELEPORT)
        return;

    if ((data.GetLastRawMovementFlags() & MOVEMENTFLAG_WATERWALKING) && (rawMovementFlags & MOVEMENTFLAG_WATERWALKING))
    {
        // if player has the following auras then we return
        if (player->HasAuraType(SPELL_AURA_WATER_WALK) || player->HasAuraType(SPELL_AURA_FEATHER_FALL) || player->HasAuraType(SPELL_AURA_SAFE_FALL))
            return;
    }
    else if (player->GetLiquidStatus() != LIQUID_MAP_WATER_WALK)
        return;

    _LogInfo(player, "Walk on Water - Hack detected");
    _BuildReport(player, WALK_WATER_HACK_REPORT, data);
}

void AnticheatMgr::_ZAxisHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ZAXISHACK_ENABLE))
        return;

   float distance2D = movementInfo.pos.GetExactDist2d(&data.GetLastMovementInfo().pos);

   // We don't need to check for a waterwalk hack if the player hasn't moved
   // This is necessary since MovementHandler fires if you rotate the camera in place
   if (distance2D < 0.1f)
       return;

   // If he is flying we dont need to check
   if (movementInfo.HasMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING))
       return;

   // If the player is allowed to waterwalk (or he is dead because he automatically waterwalks then) we dont need to check any further
   // We also stop if the player is in water, because otherwise you get a false positive for swimming
   if (movementInfo.HasMovementFlag(MOVEMENTFLAG_WATERWALKING) || player->IsInWater() || !player->IsAlive())
       return;

   // We want to exclude this LiquidStatus from detection because it leads to false positives on boats, docks etc.
   // Basically everytime you stand on a game object in water
   if (player->GetLiquidStatus() == LIQUID_MAP_ABOVE_WATER)
       return;

   // The anticheat is disabled on transports, so we need to be sure that the player is indeed on a transport.
   GameObject* transportGobj = player->GetMap()->GetGameObject(movementInfo.transport.guid);
   if (transportGobj && transportGobj->IsTransport())
       return;

   // The anticheat check is disabled on Elevators, so we need to be sure that the player is indeed on a transport.
   if (player->HasUnitMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
       return;

   // We exempt select areas found in 335 to prevent false hack hits
   if (player->GetAreaId())
   {
       switch (player->GetAreaId())
       {
            case 10: // Duskwood bridge
            case 40: // Westfall bridge
            case 321: // Hammerfall wooden balcony
            case 369: // Thunder Ridge being on the over hang cliff
            case 495:  //Ring of Judgement just being in the area false flags
            case 4273: //Celestial Planetarium Observer Battle has a narrow path that false flags
            case 4161: //Wymrest Temple just being in the area false flags
                return;
       }
   }

   // This is Black Magic. Check only for x and y difference but no z difference that is greater then or equal to z +2.5 of the ground
   if (data.GetLastMovementInfo().pos.GetPositionZ() == movementInfo.pos.GetPositionZ() && player->GetPositionZ() >= player->GetFloorZ() + 7.0f)
   {
       if (data.GetTotalReports() > _ingameNotificationThreshold)
           _NotifyGameMasters(player, "Possible Ignore Zaxis Hack Detected!", LANG_ANTICHEAT_ALERT, data);

       float posZ = player->GetPositionZ();
       float floorZ = player->GetFloorZ();
       _LogInfo(player, Trinity::StringFormat("Ignore Zaxis Hack detected (Z: {:.2f}, FloorZ: {:.2f}, Delta: {:.2f})", posZ, floorZ, posZ - floorZ));
       if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_IGNOREZ))
       {
            player->GetMotionMaster()->MoveFall();

            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
            {
                std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
                TC_LOG_INFO("anticheat.module", "ANTICHEAT COUNTER MEASURE:: {} IGNORE-Z Hack Countered and has been set to fall - Flagged at: {}", player->GetName(), goXYZ);
            }
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
                _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] IGNORE-Z HACK COUNTER MEASURE ALERT");
        
            if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
            {
                std::string str = "|cFFFFFC00 IGNORE-Z HACK COUNTER MEASURE ALERT";
                sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
            }
           _BuildReport(player, COUNTER_MEASURES_REPORT, data);
       }
       _BuildReport(player, ZAXIS_HACK_REPORT, data);
   }
}

// basic detection
void AnticheatMgr::_AntiSwimHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ANTISWIM_ENABLE))
        return;

    if (player->GetAreaId())
    {
        switch (player->GetAreaId())
        {
            case 2100: //Maraudon https://github.com/TrinityCore/TrinityCore/issues/27946
                return;
        }
    }

    if (HasAnyLiquidStatus(player, LIQUID_MAP_ABOVE_WATER | LIQUID_MAP_WATER_WALK))
        return;

    if (opcode == MSG_MOVE_JUMP)
        return;

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_SWIMMING))
        return;

    if (HasAnyLiquidStatus(player, LIQUID_MAP_UNDER_WATER) && !movementInfo.HasMovementFlag(MOVEMENTFLAG_SWIMMING))
    {
        _LogInfo(player, "Anti-Swim-Hack detected");
        _BuildReport(player, ANTISWIM_HACK_REPORT, data);
    }
}

// basic detection
void AnticheatMgr::_AntiKnockBackHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_ANTIKNOCKBACK_ENABLE))
        return;

    //if a knockback helper is not passed then we ignore
    //if player has root state we ignore, knock back does not break root
    if (!player->CanKnockback() || player->HasUnitState(UNIT_STATE_ROOT))
        return;

    if (movementInfo.pos == data.GetLastMovementInfo().pos)
    {
        _LogInfo(player, "Anti-Knock Back - Hack detected");
        _BuildReport(player, ANTIKNOCK_BACK_HACK_REPORT, data);
    }
    else
        player->SetCanKnockback(false);
}

// basic detection
void AnticheatMgr::_NoFallDamageDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_NO_FALL_DAMAGE_ENABLE))
        return;

    // ghost can not get damaged
    if (player->HasAuraType(SPELL_AURA_GHOST))
        return;

    // absorb shields can reduce fall damage to zero legitimately; don't flag those players
    if (player->HasAuraType(SPELL_AURA_SCHOOL_ABSORB))
        return;

    // players with water walk aura jumping on to the water from ledge would not get damage and neither will safe fall and feather fall
    if (((player->HasAuraType(SPELL_AURA_WATER_WALK) && player->GetLiquidStatus() == LIQUID_MAP_WATER_WALK && !player->IsFlying())) || player->HasAuraType(SPELL_AURA_FEATHER_FALL) || player->HasAuraType(SPELL_AURA_SAFE_FALL))
        return;

    float lastZ = data.GetLastMovementInfo().pos.GetPositionZ();
    float newZ = movementInfo.pos.GetPositionZ();
    float zDiff = fabs(lastZ - newZ);
    int32 safe_fall = player->GetTotalAuraModifier(SPELL_AURA_SAFE_FALL);
    float damageperc = 0.018f * (zDiff - safe_fall) - 0.2426f;
    uint32 damage = (uint32)(damageperc * player->GetMaxHealth() * sWorld->getRate(RATE_DAMAGE_FALL));

    // in the Player::Handlefall 14.57f is used to calculated the damageperc formula below to 0 for fall damamge

    if (movementInfo.pos.GetPositionZ() < data.GetLastMovementInfo().pos.GetPositionZ() && zDiff > 14.57f)
    {
        if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING) || data.GetLastMovementInfo().HasMovementFlag(MOVEMENTFLAG_FALLING))
        {
            if (damage == 0 && !player->IsImmunedToDamage(SPELL_SCHOOL_MASK_NORMAL))
            {
                _LogInfo(player, "No Fall Damage - Hack detected");
                _BuildReport(player, NO_FALL_DAMAGE_HACK_REPORT, data);
            }
        }
    }
}

void AnticheatMgr::_BGStartExploitDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_BG_START_HACK_ENABLE))
        return;

    bool report = false;
    switch (player->GetMapId())
    {
        case 30: // Alterac Valley
            if (Battleground* bg = player->GetBattleground())
            {
                if (bg->GetStatus() == STATUS_WAIT_JOIN)
                {
                    // Outside of starting area before BG has started.
                    if ((player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionX() < 770.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionX() > 940.31f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() < -525.0f))
                        report = true;
                    else if ((player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() > -535.0f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionX() > -1283.33f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() < -716.0f))
                        report = true;
                }
            }
            break;
        case 489: // Warsong Gulch
            // Only way to get this high is with engineering items malfunction.
            if (!(movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING_FAR) || data.GetLastOpcode() == MSG_MOVE_JUMP) && movementInfo.pos.GetPositionZ() > 380.0f)
                report = true;
            else if (Battleground* bg = player->GetBattleground())
            {
                if (bg->GetStatus() == STATUS_WAIT_JOIN)
                {
                    // Outside of starting area before BG has started.
                    if ((player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionX() < 1490.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() > 1500.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() < 1450.0f))
                        report = true;
                    else if ((player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionX() > 957.0f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() < 1416.0f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() > 1466.0f))
                        report = true;
                }
            }
            break;
        case 529: // Arathi Basin
            if (Battleground* bg = player->GetBattleground())
            {
                if (bg->GetStatus() == STATUS_WAIT_JOIN)
                {
                    // Outside of starting area before BG has started.
                    if ((player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionX() < 1270.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() < 1258.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() > 1361.0f))
                        report = true;
                    else if ((player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionX() > 730.0f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() > 724.8f))
                        report = true;
                }
            }
            break;
        case 566: // Eye of the Storm
            if (Battleground* bg = player->GetBattleground())
            {
                if (bg->GetStatus() == STATUS_WAIT_JOIN)
                {
                    // Outside of starting area before BG has started.
                    if ((player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionX() < 2512.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() > 1610.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() < 1584.0f))
                        report = true;
                    else if ((player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionX() > 1816.0f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() > 1554.0f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() < 1526.0f))
                        report = true;
                }
            }
            break;
        case 628: // Island Of Conquest
        {
            if (Battleground* bg = player->GetBattleground())
            {
                if (bg->GetStatus() == STATUS_WAIT_JOIN)
                {
                    // Outside of starting area before BG has started.
                    if ((player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionX() > 412.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() < -911.0f) || (player->GetTeamId() == TEAM_ALLIANCE && movementInfo.pos.GetPositionY() > -760.0f))
                        report = true;
                    else if ((player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionX() < 1147.8f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() < -855.0f) || (player->GetTeamId() == TEAM_HORDE && movementInfo.pos.GetPositionY() > -676.0f))
                        report = true;
                }
            }
            break;
        }
        default:
            return;
    }
    if (report)
    {
        _NotifyGameMasters(player, "Player Outside of Starting SPOT before BG has started!", LANG_ANTICHEAT_BG_EXPLOIT, data);
        _LogInfo(player, "BG Start Spot Exploit-Hack detected");
        _BuildReport(player, TELEPORT_HACK_REPORT, data);
        _CheckBGOriginPositions(player);
    }
}

void AnticheatMgr::_CheckBGOriginPositions(Player* player)
{
    Battleground* bg = player->GetBattleground();
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_BG_START_COUNTERHACK_ENABLE) || !bg)
        return;

    uint32 mapid = bg->GetMapId();
    uint32 team = player->GetBGTeam();
    if (team == 0)
        team = player->GetTeam();

    Position const* startPos = bg->GetTeamStartPosition(Battleground::GetTeamIndexByTeamId(team));

    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_WRITELOG))
    {
        std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
        TC_LOG_INFO("anticheat", "ANTICHEAT COUNTER MEASURE:: Sending {} back to start location (BG Map: {}) (possible exploit) - Flagged at: {}", player->GetName(), player->GetMapId(), goXYZ);
    }
    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTSCREEN))
        _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] BG START SPOT COUNTER MEASURE ALERT");

    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_CM_ALERTCHAT))
    {
        std::string str = "|cFFFFFC00 BG START SPOT COUNTER MEASURE ALERT";
        sWorld->SendGMText(LANG_ANTICHEAT_COUNTERMEASURE, str.c_str(), player->GetName().c_str(), player->GetName().c_str());
    }
    player->TeleportTo(mapid, startPos->GetPositionX(), startPos->GetPositionY(), startPos->GetPositionZ(), startPos->GetOrientation());
}

//
// Dear maintainer:
//
// Once you are done trying to 'optimize' this script,
// and have identify potentionally if there was a terrible
// mistake that was here or not, please increment the
// following counter as a warning to the next guy:
//
// total_hours_wasted_here = 46
//
void AnticheatMgr::_BuildReport(Player* player, AnticheatReportTypes reportType, AnticheatData& data)
{
    if (!player || !player->GetSession())
        return;

    if (uint32(reportType) >= MAX_REPORT_TYPES)
        return;

    if (_MustCheckTempReports(reportType))
    {
        uint32 actualTime = getMSTime();

        if (!data.GetTempReportsTimer(reportType))
            data.SetTempReportsTimer(actualTime, reportType);

        if (getMSTimeDiff(data.GetTempReportsTimer(reportType), actualTime) < 3000)
        {
            data.SetTempReports(data.GetTempReports(reportType) + 1, reportType);

            if (data.GetTempReports(reportType) < 3)
                return;

            // One committed report per 3-confirmation window. Without this reset, every packet after
            // the third packet inside the same 3 second window becomes a committed report.
            data.ResetTempReports(reportType);
        }
        else
        {
            data.SetTempReportsTimer(actualTime, reportType);
            data.SetTempReports(1, reportType);
            return;
        }
    }

    if (!data.GetTotalReports())
        data.SetCreationTime(getMSTime());

    data.SetTotalReports(data.GetTotalReports() + 1);
    data.SetTypeReports(reportType, data.GetTypeReports(reportType) + 1);

    uint32 diffTime = getMSTimeDiff(data.GetCreationTime(), getMSTime()) / IN_MILLISECONDS;
    if (diffTime > 0)
        data.SetAverage(float(data.GetTotalReports()) / float(diffTime));

    if (sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_FOR_DAILY_REPORT) < data.GetTotalReports())
        data.SetDailyReportState(true);

    if (data.GetTotalReports() > _ingameNotificationThreshold)
        _NotifyGameMasters(player, "Possible cheater!", LANG_ANTICHEAT_ALERT, data);

    // Apply one automatic moderation action per report. Ban has highest severity, then kick, then jail.
    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_AUTOBAN_ENABLE) && data.GetTotalReports() > sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_FOR_BANS))
    {
        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_WRITELOG_ENABLE))
            TC_LOG_INFO("anticheat", "AnticheatMgr:: Reports reached assigned threshold and counteracted by banning player {} ({})", player->GetName(), player->GetGUID().ToString());

        _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] Auto Banned Account for Reaching Cheat Threshold!");

        std::string accountName;
        AccountMgr::GetName(player->GetSession()->GetAccountId(), accountName);
        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_ANNOUNCEBAN_ENABLE))
        {
            std::ostringstream stream;
            stream << "|CFFff0000[AntiCheat]|r|CFF7bbef7 Player |r|cffff0000" << player->GetName() << "|r|cff7bbef7 has been Banned by the Anticheat Module.|r";
            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str());
        }
        sWorld->BanAccount(BAN_ACCOUNT, accountName, "0s", "Anticheat module Auto Banned Account for Reaching Cheat Threshold", "Server");
        return;
    }

    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_AUTOKICK_ENABLE) && data.GetTotalReports() > sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_FOR_KICKS))
    {
        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_WRITELOG_ENABLE))
            TC_LOG_INFO("anticheat", "AnticheatMgr:: Reports reached assigned threshold and counteracted by kicking player {} ({})", player->GetName(), player->GetGUID().ToString());

        _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] Auto Kicked for Reaching Cheat Threshold!");

        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_ANNOUNCEKICK_ENABLE))
        {
            std::ostringstream stream;
            stream << "|CFFff0000[AntiCheat]|r|CFF7bbef7 Player |r|cffff0000" << player->GetName() << "|r|cff7bbef7 has been kicked by the Anticheat Module.|r";
            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str());
        }

        player->GetSession()->KickPlayer("Anticheat Module");
        return;
    }

    if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_AUTOJAIL_ENABLE) && data.GetTotalReports() > sWorld->getIntConfig(CONFIG_ANTICHEAT_MAX_REPORTS_FOR_JAILS))
    {
        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_WRITELOG_ENABLE))
            TC_LOG_INFO("anticheat", "AnticheatMgr:: Reports reached assigned threshold and counteracted by jailing player {} ({})", player->GetName(), player->GetGUID().ToString());

        _NotifyGameMasters("|cFFFFFC00[Playername:|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] Auto Jailed Account for Reaching Cheat Threshold!");

        WorldLocation loc = WorldLocation(1, 16226.5f, 16403.6f, -64.5f, 3.2f);
        player->TeleportTo(loc);
        player->SetHomebind(loc, 876);
        player->CastSpell(player, SHACKLES);

        if (Aura* dungdesert = player->AddAura(LFG_SPELL_DUNGEON_DESERTER, player))
            dungdesert->SetDuration(-1);

        if (Aura* bgdesert = player->AddAura(BG_SPELL_DESERTER, player))
            bgdesert->SetDuration(-1);

        if (Aura* silent = player->AddAura(SILENCED, player))
            silent->SetDuration(-1);

        if (sWorld->getBoolConfig(CONFIG_ANTICHEAT_ANNOUNCEJAIL_ENABLE))
        {
            std::ostringstream stream;
            stream << "|CFFff0000[AntiCheat]|r|CFF7bbef7 Player |r|cffff0000" << player->GetName() << "|r|cff7bbef7 has been Jailed by the Anticheat Module.|r";
            sWorld->SendServerMessage(SERVER_MSG_STRING, stream.str());
        }
    }
}

void AnticheatMgr::_NotifyGameMasters(Player* player, std::string text, uint32 trinityString, AnticheatData& data)
{
    if (++data.AlertCounter % _alertFrequency == 0)
    {
        _NotifyGameMasters("|cFFFFFC00[Playername:]|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] " + text);
        data.AlertCounter = 0;
    }
    // need better way to limit chat spam
    if (data.GetTotalReports() >= sWorld->getIntConfig(CONFIG_ANTICHEAT_REPORT_IN_CHAT_MIN) && (data.GetTotalReports() <= sWorld->getIntConfig(CONFIG_ANTICHEAT_REPORT_IN_CHAT_MAX)))
    {
        uint32 latency = 0;
        latency = player->GetSession()->GetLatency();
        switch (trinityString)
        {
            case LANG_ANTICHEAT_IGNORECONTROL:
            case LANG_ANTICHEAT_TELEPORT:
            case LANG_ANTICHEAT_DUEL:
                sWorld->SendGMText(trinityString, player->GetName().c_str(), latency);
                break;
            default:
                sWorld->SendGMText(trinityString, player->GetName().c_str(), player->GetName().c_str(), latency);
                break;
        }
    }
}

void AnticheatMgr::_NotifyGameMasters(std::string text)
{
    WorldPacket data(SMSG_NOTIFICATION, (text.size() + 1));
    data << text;
    sWorld->SendGlobalGMMessage(&data);
}

void AnticheatMgr::_LogInfo(Player* player, std::string text)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_WRITELOG_ENABLE))
        return;

    uint32 latency = player->GetSession()->GetLatency();
    std::string goXYZ = ".go xyz " + std::to_string(player->GetPositionX()) + " " + std::to_string(player->GetPositionY()) + " " + std::to_string(player->GetPositionZ() + 1.0f) + " " + std::to_string(player->GetMap()->GetId()) + " " + std::to_string(player->GetOrientation());
    TC_LOG_INFO("anticheat", "AnticheatMgr:: {} detected player {} ({}) - Latency: {} ms - IP: {} - Cheat Flagged at: {}", text, player->GetName(), player->GetGUID().ToString(), latency, player->GetSession()->GetRemoteAddress(), goXYZ);
}
