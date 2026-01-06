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

#ifndef SC_ACMGR_H
#define SC_ACMGR_H

#include "AnticheatData.h"
#include "Common.h"
#include "SharedDefines.h"
#include <unordered_map>

struct AccountData;
class ChatHandler;
class Player;

enum AnticheatReportTypes : uint8
{
    SPEED_HACK_REPORT = 0,
    FLY_HACK_REPORT = 1,
    WALK_WATER_HACK_REPORT = 2,
    JUMP_HACK_REPORT = 3,
    TELEPORT_PLANE_HACK_REPORT = 4,
    CLIMB_HACK_REPORT = 5,
    TELEPORT_HACK_REPORT = 6,
    IGNORE_CONTROL_REPORT = 7,
    ZAXIS_HACK_REPORT = 8,
    ANTISWIM_HACK_REPORT = 9,
    GRAVITY_HACK_REPORT = 10,
    ANTIKNOCK_BACK_HACK_REPORT = 11,
    NO_FALL_DAMAGE_HACK_REPORT = 12,
    COUNTER_MEASURES_REPORT = 13

   // MAX_REPORT_TYPES
};

// GUIDLow is the key.
typedef std::unordered_map<uint32, AnticheatData> AnticheatPlayersDataMap;

class TC_GAME_API AnticheatMgr
{
    AnticheatMgr();
    ~AnticheatMgr();

    public:
        static AnticheatMgr* instance();
        void Initialize();
        void SavePlayerData(Player* player);
        void OnPlayerMove(Player* player, MovementInfo const& movementInfo, uint32 opcode);

        void HandlePlayerLogin(Player* player);
        void HandlePlayerLogout(Player* player);

        uint32 GetTotalReports(uint32 lowGUID) const;
        float GetAverage(uint32 lowGUID) const;
        uint32 GetTypeReports(uint32 lowGUID, uint8 type) const;

        void AnticheatGlobalCommand(ChatHandler* handler);
        void AnticheatDeleteCommand(uint32 guid);
        void AnticheatPurgeCommand(ChatHandler* handler);
        void ResetDailyReportStates();

        bool CheckIsLuaCheater(uint32 accountId);
        bool CheckBlockedLuaFunctions(AccountData const* accountData, Player* player = nullptr);

    private:
        void _LoadBlockedLuaFunctions();
        void _SaveLuaCheater(uint32 accountId, uint32 realmId, uint32 guid, std::string macro);

        void _StartHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode);
        void _SpeedHackDetection(Player* player, MovementInfo const& movementInfo);
        void _FlyHackDetection(Player* player, MovementInfo const& movementInfo);
        void _TeleportHackDetection(Player* player, MovementInfo const& movementInfo);
        void _JumpHackDetection(Player* player, MovementInfo const& movementInfo,uint32 opcode);
        void _TeleportPlaneHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode);
        void _ClimbHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode);
        void _IgnoreControlHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode);
        void _GravityHackDetection(Player* player, MovementInfo const& movementInfo);
        void _WalkOnWaterHackDetection(Player* player, MovementInfo const& movementInfo);
        void _ZAxisHackDetection(Player* player, MovementInfo const& movementInfo);
        void _AntiSwimHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode);
        void _AntiKnockBackHackDetection(Player* player, MovementInfo const& movementInfo);
        void _NoFallDamageDetection(Player* player, MovementInfo const& movementInfo);
        void _BGStartExploitDetection(Player* player, MovementInfo const& movementInfo);

        void _CheckBGOriginPositions(Player* player);

        void _BuildReport(Player* player, AnticheatReportTypes reportType);
        bool _MustCheckTempReports(AnticheatReportTypes type) const
        {
            switch (type)
            {
                case JUMP_HACK_REPORT:
                case TELEPORT_HACK_REPORT:
                case IGNORE_CONTROL_REPORT:
                case GRAVITY_HACK_REPORT:
                case ANTIKNOCK_BACK_HACK_REPORT:
                case NO_FALL_DAMAGE_HACK_REPORT:
                    return false;
                default:
                    return true;
            }
        }
        void _NotifyGameMasters(Player* player, std::string text, uint32 trinityString);
        void _NotifyGameMasters(std::string text);
        void _LogInfo(Player* player, std::string text);

        uint32 _alertFrequency = 0;
        uint32 _assignedspeeddiff = 0;
        uint32 _ingameNotificationThreshold = 0;
        std::unordered_map<std::string, bool> _luaBlockedFunctions;
        AnticheatPlayersDataMap _players;
};

#define sAnticheatMgr AnticheatMgr::instance()

#endif
