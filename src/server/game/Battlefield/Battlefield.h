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

#ifndef TRINITY_BATTLEFIELD_H_
#define TRINITY_BATTLEFIELD_H_

#include "BattlefieldEvents.h"
#include "Common.h"
#include "Duration.h"
#include "EnumFlag.h"
#include "Errors.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "SharedDefines.h"
#include "Timer.h"
#include "ZoneScript.h"
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

enum BattlefieldSpells
{
    SPELL_BATTLEFIELD_WAITING_FOR_RESURRECT = 2584,  // Waiting to Resurrect
    SPELL_BATTLEFIELD_SPIRIT_HEAL_CHANNEL   = 22011, // Spirit Heal Channel
    SPELL_BATTLEFIELD_SPIRIT_HEAL           = 22012, // Spirit Heal
    SPELL_BATTLEFIELD_RESURRECTION_VISUAL   = 24171, // Resurrection Impact Visual
    SPELL_BATTLEFIELD_SPIRIT_HEAL_MANA      = 44535, // Spirit Heal
    SPELL_BATTLEFIELD_AURA_PLAYER_INACTIVE  = 43681  // Inactive
};

enum BattlefieldPhase : uint8
{
    BATTLEFIELD_PHASE_IDLE = 0,
    BATTLEFIELD_PHASE_PREPARATION,
    BATTLEFIELD_PHASE_BATTLE
};

enum BattlefieldFlags : uint8
{
    BATTLEFIELD_FLAG_NONE = 0x00,
    BATTLEFIELD_FLAG_ENABLED = 0x01,
    BATTLEFIELD_FLAG_EVENTS_UPDATING = 0x02,
    BATTLEFIELD_FLAG_EVENTS_CLEANUP_PENDING = 0x04
};

DEFINE_ENUM_FLAG(BattlefieldFlags);

enum BattlefieldPlayerStatus : uint8
{
    BATTLEFIELD_PLAYER_STATUS_NONE = 0,
    BATTLEFIELD_PLAYER_STATUS_QUEUED,
    BATTLEFIELD_PLAYER_STATUS_INVITED,
    BATTLEFIELD_PLAYER_STATUS_BATTLE_ENDED
};

enum BattlefieldPlayerState : uint8
{
    BATTLEFIELD_PLAYER_STATE_QUEUED = 0,
    BATTLEFIELD_PLAYER_STATE_INVITED,
    BATTLEFIELD_PLAYER_STATE_IN_BATTLE,
    BATTLEFIELD_PLAYER_STATE_BATTLE_ENDED,
    BATTLEFIELD_PLAYER_STATE_MAX
};

enum BattlefieldLeaveReason : uint8
{
    BATTLEFIELD_LEAVE_REASON_CLOSE = 1,
    BATTLEFIELD_LEAVE_REASON_INVITE_EXPIRED = 2,
    BATTLEFIELD_LEAVE_REASON_INVITE_DECLINED = 5,
    BATTLEFIELD_LEAVE_REASON_EXIT_REQUEST = 7,
    BATTLEFIELD_LEAVE_REASON_ZONE_EXIT = 8
};

enum BattlefieldBattleResult : uint8
{
    BATTLEFIELD_BATTLE_RESULT_NONE = 0,
    BATTLEFIELD_BATTLE_RESULT_DEFENDED,
    BATTLEFIELD_BATTLE_RESULT_CAPTURED
};

enum BattlefieldValue : uint8
{
    BATTLEFIELD_VALUE_ID_MAP = 0,
    BATTLEFIELD_VALUE_STATE_CONTROLLING_TEAM,
    BATTLEFIELD_VALUE_STATE_BATTLE_RESULT,
    BATTLEFIELD_VALUE_LIMIT_TEAM_PLAYERS,
    BATTLEFIELD_VALUE_LIMIT_PLAYER_LEVEL,
    BATTLEFIELD_VALUE_DURATION_PHASE_BATTLE,
    BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION,
    BATTLEFIELD_VALUE_DURATION_BATTLE_INVITE,
    BATTLEFIELD_VALUE_INTERVAL_PHASE_BATTLE_START,
    BATTLEFIELD_VALUE_INTERVAL_RESURRECTION,
    BATTLEFIELD_VALUE_DELAY_PHASE_BATTLE_RESTART,
    BATTLEFIELD_VALUE_DELAY_BATTLE_LOGOUT_LEAVE,
    BATTLEFIELD_VALUE_DELAY_BATTLE_STATUS_CLEAR,
    BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START,
    BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END,
    BATTLEFIELD_VALUE_TIME_NEXT_RESURRECTION,
    BATTLEFIELD_VALUE_MAX
};

class TC_GAME_API BattlefieldValues
{
public:
    template<class T>
    T Get(BattlefieldValue value) const
    {
        T const* data = std::get_if<T>(&_values[value]);
        ASSERT(data, "BattlefieldValues::Get: value %u stores type index %u.", uint32(value), uint32(_values[value].index()));
        return *data;
    }

    template<class T>
    void Set(BattlefieldValue value, T data)
    {
        _values[value].emplace<T>(data);
    }

private:
    std::array<std::variant<bool, uint32, time_t, Seconds, TimePoint, PvPTeamId, BattlefieldBattleResult>, BATTLEFIELD_VALUE_MAX> _values;
};

namespace WorldPackets
{
    namespace WorldState
    {
        class InitWorldStates;
    }
}

class BattlefieldGraveyard;
class GameObject;
class ObjectGuid;
class Player;
class Unit;
struct WorldSafeLocsEntry;

class TC_GAME_API Battlefield : public ZoneScript
{
public:
    typedef std::unique_ptr<BattlefieldGraveyard> BattlefieldGraveyardPointer;
    typedef std::unordered_map<uint8 /*id*/, BattlefieldGraveyardPointer> BattlefieldGraveyardContainer;
    typedef std::multimap<TimePoint /*due*/, std::unique_ptr<BattlefieldEvent>> BattlefieldEventContainer;

    explicit Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId);
    Battlefield(Battlefield const&) = delete;
    Battlefield(Battlefield&&) = delete;

    Battlefield& operator=(Battlefield const&) = delete;
    Battlefield& operator=(Battlefield&&) = delete;

    virtual ~Battlefield();

    // Called on battlefield creation
    virtual bool SetupBattlefield();
    virtual void ChangeTeams(PvPTeamId newControllingTeam);
    virtual bool SetPhase(BattlefieldPhase phase);
    virtual void SetPhaseTime(time_t phaseTime);
    virtual void SendPhaseTimes() { }
    virtual bool TeleportPlayerToBattle(Player* /*player*/) { return false; }
    virtual void Update(uint32 diff);
    // Called when a player enters the battlefield zone
    virtual void HandlePlayerEnterZone(Player* player);
    // Called when a player leaves the battlefield zone
    virtual void HandlePlayerLeaveZone(Player* player);
    // Called when a player inside the battlefield zone kills a unit
    virtual void HandleKill(Player* /*killer*/, Unit* /*victim*/) { }
    // Called when a player queries a gossip from a spirit healer
    virtual void SendAreaSpiritHealerQueryOpcode(Player* player, ObjectGuid source);
    // Called when a player moves into a resurrection queue
    virtual void AddPlayerToResurrectionQueue(Player* player, ObjectGuid source);
    // Called when a player moves out of a resurrection queue
    virtual void RemovePlayerFromResurrectionQueue(Player* player);
    virtual void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& /*packet*/) { }
    virtual void SendGlobalWorldStates(Player const* /*player*/) const { }
    // Can players inside the battlefield zone use ground mounts?
    virtual bool IsMountAllowed() const { return true; }
    // Can players inside the battlefield zone use flying mounts?
    virtual bool IsFlyingMountAllowed() const { return true; }
    // Is the referenced SpellArea spellId allowed for the referenced player and newArea?
    virtual bool IsSpellAreaAllowed(uint32 /*spellId*/, Player const* /*player*/, uint32 /*newArea*/) const { return false; }

    void EmplaceGraveyard(uint8 id, BattlefieldGraveyardPointer&& pointer);
    void SendInitWorldStatesTo(Player* player);
    template<class Event, class... Args>
    void QueueEvent(Args&&... args)
    {
        EmplaceEvent(std::make_unique<Event>(std::forward<Args>(args)...));
    }
    void EmplaceEvent(std::unique_ptr<BattlefieldEvent>&& event);
    void RemoveEvents(std::function<bool(BattlefieldEvent const&)> const& predicate);
    void RemoveEvents(BattlefieldEventType type, ObjectGuid objectGuid);
    void ProcessRaidRelease(BattlefieldEventKey key);
    void ProcessRaidAdd(BattlefieldEventKey key, Player* player);
    void ProcessRaidRemove(BattlefieldEventKey key, ObjectGuid playerGuid);
    void ProcessPhaseAdvance(BattlefieldEventKey key);
    void ProcessGraveyardResurrect(BattlefieldEventKey key);
    void ProcessZoneRefresh(BattlefieldEventKey key);
    void ProcessQueueInvite(BattlefieldEventKey key, Player* player);
    void ProcessBattleInvite(BattlefieldEventKey key, Player* player);
    void ProcessBattleAccept(BattlefieldEventKey key, Player* player);
    void ProcessBattleDecline(BattlefieldEventKey key, Player* player);
    void ProcessBattleExpire(BattlefieldEventKey key, ObjectGuid playerGuid);
    void ProcessBattleLeave(BattlefieldEventKey key, ObjectGuid playerGuid);
    void ProcessBattleStatusClear(BattlefieldEventKey key);
    void HandleBattlefieldRaidMemberRemoved(ObjectGuid playerGuid, RemoveMethod method);
    void HandlePlayerLogout(Player* player);
    void HandlePlayerAddToMap(Player* player);
    void HandleQueueRequest(Player* player);
    void HandleQueueInviteResponse(Player* player, bool accepted);
    void HandleQueueExitRequest(Player* player);
    void HandleBattleInviteResponse(Player* player, bool accepted);

    void SetMapId(uint32 mapId) { Values.Set(BATTLEFIELD_VALUE_ID_MAP, mapId); }
    void SetEnabled(bool enabled);
    bool SetBattleResult(BattlefieldBattleResult result);

    BattlefieldBattleId GetId() const { return _battleId; }
    uint32 GetZoneId() const { return _zoneId; }
    BattlefieldPhase GetPhase() const { return _phase; }
    bool IsEnabled() const { return _flags.HasFlag(BATTLEFIELD_FLAG_ENABLED); }
    bool IsInPreparation() const { return _phase == BATTLEFIELD_PHASE_PREPARATION; }
    bool IsBattleTime() const { return _phase == BATTLEFIELD_PHASE_BATTLE; }
    bool CanJoinBattle(Player const* player) const;
    // enum PvPTeamId
    PvPTeamId GetControllingTeam() const { return Values.Get<PvPTeamId>(BATTLEFIELD_VALUE_STATE_CONTROLLING_TEAM); }
    // enum PvPTeamId
    PvPTeamId GetAttackingTeam() const;
    // enum TeamId
    TeamId GetControllingTeamId() const;
    // enum TeamId
    TeamId GetAttackingTeamId() const;
    BattlefieldValues const& GetValues() const { return Values; }
    std::vector<ObjectGuid> const& GetBattlefieldRaids(TeamId teamId) const { return _battlefieldRaids[teamId]; }
    BattlefieldGraveyard const* GetGraveyard(uint8 graveyardId) const;
    BattlefieldGraveyard* GetGraveyard(uint8 graveyardId);
    // Closest available graveyard WorldSafeLocsEntry for Player's TeamId
    WorldSafeLocsEntry const* GetClosestGraveyardLocation(Player* player) const;
    Creature* GetCreature(ObjectGuid guid) const;
    GameObject* GetGameObject(ObjectGuid guid) const;

protected:
    void InitializePhaseTimes();
    void SendWorldStateToAll(uint32 variable, uint32 value) const;
    void SendWorldStateToMap(uint32 variable, uint32 value) const;

    BattlefieldValues Values;

private:
    class PlayerContainer
    {
    public:
        bool Contains(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state) const;
        Optional<BattlefieldPlayerState> FindState(ObjectGuid playerGuid, TeamId teamId) const;
        uint32 Count(TeamId teamId, BattlefieldPlayerState state) const;
        std::vector<ObjectGuid> List(TeamId teamId, BattlefieldPlayerState state) const;
        void Insert(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state);
        bool Erase(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state);
        void EraseAll(TeamId teamId, BattlefieldPlayerState state);

    private:
        typedef std::tuple<TeamId, BattlefieldPlayerState, ObjectGuid> Key;
        typedef std::set<Key> Container;

        std::pair<Container::const_iterator, Container::const_iterator> GetRange(TeamId teamId, BattlefieldPlayerState state) const;

        Container _players;
    };

    bool IsBattlefieldRaidPhase() const { return IsInPreparation() || IsBattleTime(); }
    uint32 GetFreeBattleSlots(TeamId teamId) const;
    bool HasBattleSlot(TeamId teamId) const { return GetFreeBattleSlots(teamId) > 0; }
    BattlefieldPlayerStatus GetPlayerStatus(ObjectGuid playerGuid, TeamId teamId) const;
    BattlefieldPlayerStatus RemoveFromQueue(ObjectGuid playerGuid, TeamId teamId);
    void InviteToBattle(Player* player, bool battleStart);
    void FillBattleSlots(bool battleStart);
    void ClearBattleStatus();
    void ScheduleNextPhaseAdvance();
    void ScheduleNextGraveyardResurrect();

    // constant information
    BattlefieldBattleId _battleId;
    uint32 _zoneId;

    BattlefieldPhase _phase;
    EnumFlag<BattlefieldFlags> _flags;
    GuidUnorderedSet _playerGUIDs;
    BattlefieldGraveyardContainer _graveyards;
    std::array<std::vector<ObjectGuid>, PVP_TEAMS_COUNT> _battlefieldRaids;
    PlayerContainer _battlefieldPlayers;
    BattlefieldEventContainer _events;
};

#endif
