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

#ifndef TRINITY_BATTLEFIELD_ENTITIES_H_
#define TRINITY_BATTLEFIELD_ENTITIES_H_

#include "Common.h"
#include "Duration.h"
#include "EnumFlag.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"
#include <initializer_list>
#include <unordered_map>
#include <vector>

class Battlefield;
class GameObject;
class Player;
class WorldObject;
struct GameObjectTemplate;

enum BattlefieldEntityType : uint8
{
    BATTLEFIELD_ENTITY_TYPE_BUILDING = 0,
    BATTLEFIELD_ENTITY_TYPE_CAPTUREPOINT,
    BATTLEFIELD_ENTITY_TYPE_GRAVEYARD,
    BATTLEFIELD_ENTITY_TYPE_ZONE
};

enum BattlefieldObjectInfoPhase : uint8
{
    BATTLEFIELD_OBJECT_INFO_PHASE_ANY = 0,
    BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE,
    BATTLEFIELD_OBJECT_INFO_PHASE_IDLE
};

enum BattlefieldObjectInfoFlags : uint8
{
    BATTLEFIELD_OBJECT_INFO_FLAG_NONE = 0x00,
    BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED = 0x01,
    BATTLEFIELD_OBJECT_INFO_FLAG_OPPOSITE_TEAM = 0x02
};

DEFINE_ENUM_FLAG(BattlefieldObjectInfoFlags);

enum BattlefieldBuildingType : uint8
{
    BATTLEFIELD_BUILDING_TYPE_TOWER = 0,
    BATTLEFIELD_BUILDING_TYPE_WALL,
    BATTLEFIELD_BUILDING_TYPE_KEEP_TOWER,
    BATTLEFIELD_BUILDING_TYPE_DOOR,
    BATTLEFIELD_BUILDING_TYPE_DOOR_LAST,
    BATTLEFIELD_BUILDING_TYPE_TITANRELIC,
    BATTLEFIELD_BUILDING_TYPE_WORKSHOP
};

enum BattlefieldBuildingState : uint8
{
    BATTLEFIELD_BUILDING_STATE_NONE = 0,
    BATTLEFIELD_BUILDING_STATE_NEUTRAL_INTACT,
    BATTLEFIELD_BUILDING_STATE_NEUTRAL_DAMAGED,
    BATTLEFIELD_BUILDING_STATE_NEUTRAL_DESTROYED,
    BATTLEFIELD_BUILDING_STATE_HORDE_INTACT,
    BATTLEFIELD_BUILDING_STATE_HORDE_DAMAGED,
    BATTLEFIELD_BUILDING_STATE_HORDE_DESTROYED,
    BATTLEFIELD_BUILDING_STATE_ALLIANCE_INTACT,
    BATTLEFIELD_BUILDING_STATE_ALLIANCE_DAMAGED,
    BATTLEFIELD_BUILDING_STATE_ALLIANCE_DESTROYED
};

enum BattlefieldCapturePointArtKit : uint8
{
    BATTLEFIELD_CAPTUREPOINT_ARTKIT_HORDE = 1,
    BATTLEFIELD_CAPTUREPOINT_ARTKIT_ALLIANCE
};

enum BattlefieldCapturePointFlags : uint8
{
    BATTLEFIELD_CAPTUREPOINT_FLAG_NONE = 0x00,
    BATTLEFIELD_CAPTUREPOINT_FLAG_CAPTURABLE = 0x01,
    BATTLEFIELD_CAPTUREPOINT_FLAG_STARTS_WITH_ATTACKER = 0x02,
    BATTLEFIELD_CAPTUREPOINT_FLAG_KEEPS_OWNER = 0x04
};

DEFINE_ENUM_FLAG(BattlefieldCapturePointFlags);

enum BattlefieldGraveyardState : uint8
{
    BATTLEFIELD_GRAVEYARD_STATE_NEUTRAL = 0,
    BATTLEFIELD_GRAVEYARD_STATE_ALLIANCE,
    BATTLEFIELD_GRAVEYARD_STATE_HORDE,
};

struct BattlefieldEntityInfo
{
    BattlefieldEntityInfo(BattlefieldEntityType type, uint8 id) : EntityType(type), Id(id), WorldState(0) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint8 id, uint32 worldState) : EntityType(type), Id(id), WorldState(worldState) { }

    BattlefieldEntityType EntityType;
    uint8 Id;
    uint32 WorldState;
};

struct BattlefieldObjectInfo
{
    BattlefieldObjectInfo(PvPTeamId team, TypeID objectType, std::initializer_list<uint32> entries, std::initializer_list<ObjectGuid::LowType> spawnIds = { }) : Team(team), Phase(BATTLEFIELD_OBJECT_INFO_PHASE_ANY), Flags(BATTLEFIELD_OBJECT_INFO_FLAG_NONE), SpawnGroupId(0), Entries(entries), SpawnIds(spawnIds), ObjectType(objectType) { }
    BattlefieldObjectInfo(PvPTeamId team, BattlefieldObjectInfoPhase phase, EnumFlag<BattlefieldObjectInfoFlags> flags, uint32 spawnGroupId) : Team(team), Phase(phase), Flags(flags), SpawnGroupId(spawnGroupId), ObjectType(TYPEID_OBJECT) { }

    PvPTeamId Team;
    BattlefieldObjectInfoPhase Phase;
    EnumFlag<BattlefieldObjectInfoFlags> Flags;
    uint32 SpawnGroupId;
    std::vector<uint32> Entries;
    std::vector<ObjectGuid::LowType> SpawnIds;
    TypeID ObjectType;
};

namespace WorldPackets
{
    namespace WorldState
    {
        class InitWorldStates;
    }
}

class BattlefieldEntity
{
public:
    explicit BattlefieldEntity(Battlefield* battlefield, BattlefieldEntityInfo const info);
    virtual ~BattlefieldEntity() { }

    virtual void OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo);
    virtual void OnObjectRemove(WorldObject* object, BattlefieldObjectInfo const& objectInfo);
    virtual PvPTeamId GetPvPTeamId() const { return PVP_TEAM_NEUTRAL; }
    virtual PvPTeamId GetDisplayedPvPTeamId() const { return GetPvPTeamId(); }
    virtual std::vector<BattlefieldObjectInfo> const* GetObjectInfos() const { return nullptr; }
    virtual bool IsObjectInfoActive(BattlefieldObjectInfo const& objectInfo) const;
    virtual void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet);
    virtual void InitializeState() { }
    virtual void SaveWorldState() { }

    void UpdateObjectSpawns();

    BattlefieldObjectInfo const* FindObjectInfo(WorldObject const* object) const;
    bool ValidateObjectGUID(ObjectGuid reference) const;

    Battlefield* Battle;
    BattlefieldEntityInfo Info;
    std::unordered_map<BattlefieldObjectInfo const*, GuidUnorderedSet> ObjectGUIDs;
};

class BattlefieldZone : public BattlefieldEntity
{
public:
    static constexpr BattlefieldEntityType Type = BATTLEFIELD_ENTITY_TYPE_ZONE;
    typedef BattlefieldZone StorageClass;

    explicit BattlefieldZone(Battlefield* battlefield);
    virtual ~BattlefieldZone() { }

    PvPTeamId GetDisplayedPvPTeamId() const override;
};

struct BattlefieldBuildingInfo
{
    BattlefieldBuildingInfo(uint8 id, uint32 worldState, BattlefieldBuildingType type) : BattlefieldBuildingInfo(BattlefieldEntityInfo(BATTLEFIELD_ENTITY_TYPE_BUILDING, id, worldState), type) { }
    BattlefieldBuildingInfo(BattlefieldEntityInfo const info, BattlefieldBuildingType type) : Info(info), Type(type) { }

    BattlefieldEntityInfo Info;
    BattlefieldBuildingType Type;
};

class BattlefieldBuilding : public BattlefieldEntity
{
public:
    static constexpr BattlefieldEntityType Type = BATTLEFIELD_ENTITY_TYPE_BUILDING;
    typedef BattlefieldBuilding StorageClass;

    explicit BattlefieldBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info);
    virtual ~BattlefieldBuilding() { }

    PvPTeamId GetPvPTeamId() const override;
    bool IsObjectInfoActive(BattlefieldObjectInfo const& objectInfo) const override;
    void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override;
    void SaveWorldState() override;

    virtual void Reset() { }
    virtual void ChangeTeam() { }
    virtual void Update(std::vector<Player*> const& /*players*/, Milliseconds /*diff*/) { }
    virtual bool Damage();
    virtual bool Destroy(Player* destroyer);
    GameObjectDestructibleState GetDestructibleState() const;
    std::vector<GameObject*> GetGameObjects() const;

    BattlefieldBuildingType BuildingType;
    BattlefieldBuildingState State;

protected:
    bool SetState(PvPTeamId pvpTeamId, GameObjectDestructibleState destructibleState);
    void UpdateWorldState();
};

class BattlefieldDestructibleBuilding : public BattlefieldBuilding
{
public:
    explicit BattlefieldDestructibleBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info);
    virtual ~BattlefieldDestructibleBuilding() { }

    void OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo) override;
    PvPTeamId GetDisplayedPvPTeamId() const override;
    void InitializeState() override;
    void Reset() override;
    void ChangeTeam() override;

private:
    PvPTeamId GetOwnerPvPTeamId() const;
    uint32 GetFaction() const;
};

struct BattlefieldCapturePointInfo
{
    BattlefieldCapturePointInfo(uint8 id, uint32 worldState, uint8 graveyardId, EnumFlag<BattlefieldCapturePointFlags> flags) : Info(BattlefieldEntityInfo(BATTLEFIELD_ENTITY_TYPE_CAPTUREPOINT, id, worldState), BATTLEFIELD_BUILDING_TYPE_WORKSHOP), GraveyardId(graveyardId), Flags(flags) { }

    BattlefieldBuildingInfo Info;
    uint8 GraveyardId;
    EnumFlag<BattlefieldCapturePointFlags> Flags;
};

class BattlefieldCapturePoint : public BattlefieldBuilding
{
public:
    explicit BattlefieldCapturePoint(Battlefield* battlefield, BattlefieldCapturePointInfo const info);
    virtual ~BattlefieldCapturePoint() { }

    void OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo) override;
    void InitializeState() override;
    void SaveWorldState() override;
    void Reset() override;
    void ChangeTeam() override;
    void Update(std::vector<Player*> const& players, Milliseconds diff) override;

    virtual void SetOwner(PvPTeamId pvpTeamId);
    virtual void Attack(PvPTeamId /*pvpTeamId*/) { }
    virtual void Capture(PvPTeamId pvpTeamId);

    uint8 GraveyardId;
    EnumFlag<BattlefieldCapturePointFlags> Flags;

private:
    GameObjectTemplate const* GetReferenceTemplate() const;
    void SendCaptureBar(Player* player, bool show) const;
    void SendCaptureBarValue() const;

    float _slider;
    bool _contested;
    GuidUnorderedSet _capturePlayers;
};

struct BattlefieldGraveyardInfo
{
    BattlefieldGraveyardInfo(uint8 id, uint32 worldState, uint16 worldSafeLocsEntryId, uint32 textId, uint32 teleportSpellId) : Info(BATTLEFIELD_ENTITY_TYPE_GRAVEYARD, id, worldState), WorldSafeLocsEntryId(worldSafeLocsEntryId), TextId(textId), TeleportSpellId(teleportSpellId) { }

    BattlefieldEntityInfo Info;
    uint16 WorldSafeLocsEntryId;
    uint32 TextId;
    uint32 TeleportSpellId;
};

class BattlefieldGraveyard : public BattlefieldEntity
{
public:
    static constexpr BattlefieldEntityType Type = BATTLEFIELD_ENTITY_TYPE_GRAVEYARD;
    typedef BattlefieldGraveyard StorageClass;

    explicit BattlefieldGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info);
    virtual ~BattlefieldGraveyard() { }

    PvPTeamId GetPvPTeamId() const override;
    void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override;
    void SaveWorldState() override;

    void AddPlayerToResurrectionQueue(Player* player);
    void RemovePlayerFromResurrectionQueue(Player* player);
    void ResurrectPlayers();
    void SetOwner(PvPTeamId pvpTeamId);

    uint16 GetWorldSafeLocsEntryId() const { return WorldSafeLocsEntryId; }
    uint32 GetTeleportSpellId() const { return TeleportSpellId; }

    uint16 WorldSafeLocsEntryId;
    uint32 TextId;
    BattlefieldGraveyardState State;
    GuidUnorderedSet ResurrectionQueue;
    uint32 TeleportSpellId;
};

#endif
