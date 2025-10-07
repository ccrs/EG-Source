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
#include "Chat.h"
#include "Common.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "WorldSession.h"
#include <unordered_map>

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
    OP_ACK_HACK_REPORT = 13,
    COUNTER_MEASURES_REPORT = 14

   // MAX_REPORT_TYPES
};

// GUIDLow is the key.
typedef std::map<uint32, AnticheatData> AnticheatPlayersDataMap;

class TC_GAME_API ServerOrderData
{
    public:
        ServerOrderData(uint32 serv, uint32 resp) : ServerOpcode1(serv), ServerOpcode2(0), ClientResp(resp), LastSent(0), LastRcvd(0), Counter(0) {}
        ServerOrderData(uint32 serv1, uint32 serv2, uint32 resp) : ServerOpcode1(serv1), ServerOpcode2(serv2), ClientResp(resp), LastSent(0), LastRcvd(0), Counter(0) {}

        uint32 ServerOpcode1;
        uint32 ServerOpcode2;
        uint32 ClientResp;

        uint32 LastSent;
        uint32 LastRcvd;
        int32 Counter;
};

class TC_GAME_API AnticheatMgr
{
    AnticheatMgr();
    ~AnticheatMgr();

    public:
        static AnticheatMgr* instance()
        {
           static AnticheatMgr* instance = new AnticheatMgr();
           return instance;
        }
        void Initialize();
        void SetAllowedMovement(Player* player, bool);
        void StartHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode);
        void SavePlayerData(Player* player);
        void SavePlayerDataDaily(Player* player);
        void OnPlayerMove(Player* player, MovementInfo const& movementInfo, uint32 opcode);

        void HandlePlayerLogin(Player* player);
        void HandlePlayerLogout(Player* player);
        void AckUpdate(Player* player, uint32 diff);
        void DoActions(Player* player);

        // orders
        void OrderSent(WorldPacket const* data);
        void CheckForOrderAck(uint32 opcode);
        std::vector<ServerOrderData> _opackorders; // Packets sent by server, triggering *_ACK from client

        uint32 GetTotalReports(uint32 lowGUID);
        float GetAverage(uint32 lowGUID);
        uint32 GetTypeReports(uint32 lowGUID, uint8 type);

        void AnticheatGlobalCommand(ChatHandler* handler);
        void AnticheatDeleteCommand(uint32 guid);
        void AnticheatPurgeCommand(ChatHandler* handler);
        void ResetDailyReportStates();
        void SaveLuaCheater(uint32 guid, uint32 accountId, std::string macro);
        bool CheckIsLuaCheater(uint32 accountId);
        bool CheckBlockedLuaFunctions(AccountData accountData[NUM_ACCOUNT_DATA_TYPES], Player* player = nullptr);

    private:
        void _LoadBlockedLuaFunctions();

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

        void _BGreport(Player* player);
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
                case OP_ACK_HACK_REPORT:
                    return false;
                default:
                    return true;
            }
        }

        uint32 _counter = 0;
        uint32 _alertFrequency = 0;
        uint32 _assignedspeeddiff = 0;
        uint32 _updateCheckTimer = 4000;
        std::unordered_map<std::string, bool> _luaBlockedFunctions;
        AnticheatPlayersDataMap _players;
};

#define sAnticheatMgr AnticheatMgr::instance()

#endif
