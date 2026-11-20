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
#include "IteratorPair.h"
#include "MapUtils.h"
#include "ObjectGuid.h"
#include "Optional.h"
#include "SharedDefines.h"
#include "Timer.h"
#include "ZoneScript.h"
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

enum BattlefieldSpells
{
    SPELL_BATTLEFIELD_WAITING_FOR_RESURRECT = 2584, // Waiting to Resurrect
    SPELL_BATTLEFIELD_SPIRIT_HEAL_CHANNEL = 22011, // Spirit Heal Channel
    SPELL_BATTLEFIELD_SPIRIT_HEAL = 22012, // Spirit Heal
    SPELL_BATTLEFIELD_RESURRECTION_VISUAL = 24171, // Resurrection Impact Visual
    SPELL_BATTLEFIELD_SPIRIT_HEAL_MANA = 44535, // Spirit Heal
    SPELL_BATTLEFIELD_AURA_PLAYER_INACTIVE = 43681 // Inactive
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
    BATTLEFIELD_FLAG_EVENTS_CLEANUP_PENDING = 0x04,
    BATTLEFIELD_FLAG_RAID_RELEASE_PENDING = 0x08,
    BATTLEFIELD_FLAG_BATTLE_STATUS_CLEAR_PENDING = 0x10
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
    BATTLEFIELD_PLAYER_STATE_INVITE_PENDING = 0,
    BATTLEFIELD_PLAYER_STATE_QUEUED,
    BATTLEFIELD_PLAYER_STATE_INVITED,
    BATTLEFIELD_PLAYER_STATE_IN_BATTLE,
    BATTLEFIELD_PLAYER_STATE_BATTLE_RESULT_PENDING,
    BATTLEFIELD_PLAYER_STATE_BATTLE_RESULT_DONE,
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
    BATTLEFIELD_VALUE_STATE_BATTLE_DEFENDER,
    BATTLEFIELD_VALUE_LIMIT_TEAM_PLAYERS,
    BATTLEFIELD_VALUE_LIMIT_PLAYER_LEVEL,
    BATTLEFIELD_VALUE_DURATION_PHASE_BATTLE,
    BATTLEFIELD_VALUE_DURATION_PHASE_PREPARATION,
    BATTLEFIELD_VALUE_DURATION_BATTLE_INVITE,
    BATTLEFIELD_VALUE_DURATION_BATTLE_REWARD_TOLERANCE,
    BATTLEFIELD_VALUE_INTERVAL_PHASE_BATTLE_START,
    BATTLEFIELD_VALUE_INTERVAL_RESURRECTION,
    BATTLEFIELD_VALUE_INTERVAL_PERIODIC_UPDATE,
    BATTLEFIELD_VALUE_DELAY_PHASE_BATTLE_RESTART,
    BATTLEFIELD_VALUE_DELAY_BATTLE_LOGOUT_LEAVE,
    BATTLEFIELD_VALUE_DELAY_BATTLE_FILL,
    BATTLEFIELD_VALUE_DELAY_BATTLE_KICK,
    BATTLEFIELD_VALUE_DELAY_BATTLE_STATUS_CLEAR,
    BATTLEFIELD_VALUE_TIME_PHASE_START,
    BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_START,
    BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_NEXT_START,
    BATTLEFIELD_VALUE_TIME_PHASE_BATTLE_END,
    BATTLEFIELD_VALUE_TIME_NEXT_RESURRECTION,
    BATTLEFIELD_VALUE_TIME_LAST_PERIODIC_UPDATE,
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

class BattlefieldBuilding;
class BattlefieldEntity;
class BattlefieldGraveyard;
class Creature;
class GameObject;
class Map;
class ObjectGuid;
class Player;
class Unit;
class WorldObject;
struct BattlefieldObjectInfo;
struct WorldSafeLocsEntry;

enum BattlefieldEntityType : uint8;

template<class T>
using BattlefieldEntityContainer = std::unordered_map<uint8 /*id*/, std::unique_ptr<T>>;

class BattlefieldEntityGroup
{
public:
    virtual ~BattlefieldEntityGroup() { }

    virtual BattlefieldEntity* FindEntity(std::function<bool(BattlefieldEntity*)> const& predicate) const = 0;
    virtual void ForEachEntity(std::function<void(BattlefieldEntity*)> const& function) const = 0;
};

template<class T>
class BattlefieldEntityGroupOf : public BattlefieldEntityGroup
{
public:
    BattlefieldEntity* FindEntity(std::function<bool(BattlefieldEntity*)> const& predicate) const override
    {
        for (auto const& [_, entity] : Entities)
            if (predicate(entity.get()))
                return entity.get();

        return nullptr;
    }

    void ForEachEntity(std::function<void(BattlefieldEntity*)> const& function) const override
    {
        for (auto const& [_, entity] : Entities)
            function(entity.get());
    }

    BattlefieldEntityContainer<T> Entities;
};

class TC_GAME_API Battlefield : public ZoneScript
{
public:
    typedef std::map<BattlefieldEntityType, std::unique_ptr<BattlefieldEntityGroup>> BattlefieldEntityGroupContainer;
    typedef std::multimap<TimePoint /*due*/, std::unique_ptr<BattlefieldEvent>> BattlefieldEventContainer;

    explicit Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId);
    Battlefield(Battlefield const&) = delete;
    Battlefield(Battlefield&&) = delete;

    Battlefield& operator=(Battlefield const&) = delete;
    Battlefield& operator=(Battlefield&&) = delete;

    virtual ~Battlefield();

    // Called on battlefield creation
    virtual bool SetupBattlefield();
    virtual void ChangeTeam(PvPTeamId newControllingTeam);
    virtual bool SetPhase(BattlefieldPhase phase);
    virtual void SetPhaseTime(time_t phaseTime);
    virtual void SendPhaseTimes() { }
    virtual bool TeleportPlayerToBattle(Player* /*player*/) { return false; }
    virtual void TeleportPlayerOutOfBattle(Player* /*player*/) { }
    virtual uint32 GetFaction(PvPTeamId /*pvpTeamId*/) const { return 0; }
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
    virtual void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet);
    virtual void SendGlobalWorldStates(Player const* /*player*/) { }
    // Can players inside the battlefield zone use ground mounts?
    virtual bool IsMountAllowed() const { return true; }
    // Can players inside the battlefield zone use flying mounts?
    virtual bool IsFlyingMountAllowed() const { return true; }
    // Is the referenced SpellArea spellId allowed for the referenced player and newArea?
    virtual bool IsSpellAreaAllowed(uint32 /*spellId*/, Player const* /*player*/, uint32 /*newArea*/) const { return false; }

    void OnCreatureCreate(Creature* object) override;
    void OnCreatureRemove(Creature* object) override;
    void OnGameObjectCreate(GameObject* object) override;
    void OnGameObjectRemove(GameObject* object) override;
    void ProcessEvent(WorldObject* object, uint32 eventId) override;

    template<class T>
    void EmplaceEntity(std::type_identity_t<std::unique_ptr<T>>&& pointer)
    {
        static_assert(std::is_same_v<T, typename T::StorageClass>);

        std::unique_ptr<BattlefieldEntityGroup>& group = _entities[T::Type];
        if (!group)
            group = std::make_unique<BattlefieldEntityGroupOf<T>>();

        uint8 const id = pointer->Info.Id;
        static_cast<BattlefieldEntityGroupOf<T>*>(group.get())->Entities.emplace(id, std::move(pointer));
    }
    void SendInitWorldStatesTo(Player* player);
    void SendWorldStateToMap(uint32 variable, uint32 value);
    void SetSpawnGroupSpawned(uint32 spawnGroupId, bool spawned);
    void HandleBattlefieldRaidMemberRemoved(ObjectGuid playerGuid, RemoveMethod method);
    void HandlePlayerLogout(Player* player);
    void HandlePlayerAddToMap(Player* player);
    void HandleQueueRequest(Player* player);
    void HandleQueueInviteResponse(Player* player, bool accepted);
    void HandleQueueExitRequest(Player* player);
    void HandleBattleInviteResponse(Player* player, bool accepted);

    template<class Event, class... Args>
    bool QueueEvent(Args&&... args)
    {
        return EmplaceEvent(std::make_unique<Event>(std::forward<Args>(args)...));
    }
    bool EmplaceEvent(std::unique_ptr<BattlefieldEvent>&& event);
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
    void ProcessBattleFill(BattlefieldEventKey key);
    void ProcessBattleKick(BattlefieldEventKey key, Player* player);
    void ProcessBuildingDamage(BattlefieldEventKey key, ObjectGuid objectGuid);
    void ProcessBuildingDestroy(BattlefieldEventKey key, ObjectGuid objectGuid);
    void ProcessPeriodicUpdate(BattlefieldEventKey key);
    void ProcessBattleStatusClear(BattlefieldEventKey key);
    void ProcessBattleCapture(BattlefieldEventKey key);
    void ProcessBattleResult(BattlefieldEventKey key);

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
    bool IsPlayerInBattle(Player const* player) const;
    // enum PvPTeamId
    PvPTeamId GetControllingTeam() const { return Values.Get<PvPTeamId>(BATTLEFIELD_VALUE_STATE_CONTROLLING_TEAM); }
    // enum PvPTeamId
    PvPTeamId GetAttackingTeam() const;
    // enum TeamId
    TeamId GetControllingTeamId() const;
    // enum TeamId
    TeamId GetAttackingTeamId() const;
    BattlefieldValues const& GetValues() const { return Values; }
    int32 GetBattleHonorGainPct(Player const* player) const;
    std::vector<ObjectGuid> const& GetBattlefieldRaids(TeamId teamId) const { return _battlefieldRaids[teamId]; }
    template<class T>
    T* GetEntity(uint8 id)
    {
        std::unique_ptr<T> const* entity = Trinity::Containers::MapGetValuePtr(GetEntities<T>(), id);
        return entity ? entity->get() : nullptr;
    }
    template<class T>
    T const* GetEntity(uint8 id) const
    {
        std::unique_ptr<T> const* entity = Trinity::Containers::MapGetValuePtr(GetEntities<T>(), id);
        return entity ? entity->get() : nullptr;
    }
    BattlefieldBuilding* GetBuildingByEntry(uint32 entry);
    std::pair<BattlefieldEntity*, BattlefieldObjectInfo const*> FindObjectOwner(WorldObject const* object) const;
    BattlefieldGraveyard const* GetClosestGraveyard(Player* player) const;
    // Closest available graveyard WorldSafeLocsEntry for Player's TeamId
    WorldSafeLocsEntry const* GetClosestGraveyardLocation(Player* player) const;
    Creature* GetCreature(ObjectGuid guid) const;
    GameObject* GetGameObject(ObjectGuid guid) const;

protected:
    virtual void ClearBattleStatus();
    virtual void OnPlayerJoinBattle(Player* /*player*/) { }
    virtual void OnPlayerLeaveBattle(Player* /*player*/) { }
    virtual void OnPlayerBattleResult(Player* /*player*/) { }
    virtual void OnPeriodicUpdate(std::vector<Player*> const& /*players*/, Milliseconds /*diff*/) { }

    void InitializePhaseTimes();

    Map* GetBattleMap() const;
    void SendWorldStateToAll(uint32 variable, uint32 value) const;
    uint32 GetBattlePlayerCount(TeamId teamId) const;
    Optional<time_t> GetBattleJoinTime(Player const* player) const;
    bool HasPendingBattleResult(Player const* player) const;
    template<class T>
    BattlefieldEntityContainer<T> const& GetEntities() const
    {
        static_assert(std::is_same_v<T, typename T::StorageClass>);

        static BattlefieldEntityContainer<T> const empty{};
        std::unique_ptr<BattlefieldEntityGroup> const* group = Trinity::Containers::MapGetValuePtr(_entities, T::Type);
        return group ? static_cast<BattlefieldEntityGroupOf<T> const*>(group->get())->Entities : empty;
    }

    GuidUnorderedSet const& GetZonePlayers() const { return _playerGUIDs; }

    BattlefieldValues Values;

private:
    class PlayerContainer
    {
    public:
        struct BattleInfo
        {
            time_t JoinTime = 0;
            int32 HonorGainPct = 0;
        };

        void Insert(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state);
        bool Erase(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state);
        void EraseAll(TeamId teamId, BattlefieldPlayerState state);
        void SetHonorGainPct(ObjectGuid playerGuid, TeamId teamId, int32 honorGainPct);

        bool Contains(ObjectGuid playerGuid, TeamId teamId, BattlefieldPlayerState state) const;
        Optional<BattlefieldPlayerState> FindState(ObjectGuid playerGuid, TeamId teamId) const;
        BattleInfo const* FindBattleInfo(ObjectGuid playerGuid, TeamId teamId) const;
        uint32 Count(TeamId teamId, BattlefieldPlayerState state) const;
        std::vector<ObjectGuid> List(TeamId teamId, BattlefieldPlayerState state) const;

    private:
        typedef std::tuple<TeamId, BattlefieldPlayerState, ObjectGuid> Key;
        typedef std::map<Key, BattleInfo> Container;

        Trinity::IteratorPair<Container::const_iterator> GetRange(TeamId teamId, BattlefieldPlayerState state) const;

        Container _players;
    };

    BattlefieldPlayerStatus RemoveFromQueue(ObjectGuid playerGuid, TeamId teamId);
    void InviteToBattle(Player* player, bool battleStart);
    void FillBattleSlots(bool battleStart);
    void ResolveBattleResult(Player* player);
    void ScheduleNextPhaseAdvance();
    void ScheduleNextGraveyardResurrect();
    void UpdateObjectSpawns();

    bool IsBattlefieldRaidPhase() const { return IsInPreparation() || IsBattleTime(); }
    bool IsEventScopeActive(BattlefieldEventScope scope) const { return (scope & (1 << _phase)) != 0; }
    uint32 GetFreeBattleSlots(TeamId teamId) const;
    bool HasBattleSlot(TeamId teamId) const { return GetFreeBattleSlots(teamId) > 0; }
    BattlefieldPlayerStatus GetPlayerStatus(ObjectGuid playerGuid, TeamId teamId) const;

    // constant information
    BattlefieldBattleId _battleId;
    uint32 _zoneId;

    BattlefieldPhase _phase;
    EnumFlag<BattlefieldFlags> _flags;
    GuidUnorderedSet _playerGUIDs;
    BattlefieldEntityGroupContainer _entities;
    std::array<std::vector<ObjectGuid>, PVP_TEAMS_COUNT> _battlefieldRaids;
    PlayerContainer _battlefieldPlayers;
    BattlefieldEventContainer _events;
};

#endif
