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
#include "ObjectGuid.h"
#include "SharedDefines.h"
#include <array>
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
    BATTLEFIELD_ENTITY_TYPE_GRAVEYARD
};

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
    BattlefieldEntityInfo(BattlefieldEntityType type, uint8 id, uint32 worldState, uint32 entry) : EntityType(type), Id(id), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_NEUTRAL, { entry } }}) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint8 id, uint32 worldState, std::initializer_list<uint32> neutralEntryList) : EntityType(type), Id(id), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_NEUTRAL, { neutralEntryList } }}) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint8 id, uint32 worldState, std::initializer_list<uint32> hordeEntryList, std::initializer_list<uint32> allianceEntryList) : EntityType(type), Id(id), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_HORDE, { hordeEntryList } }, { PVP_TEAM_ALLIANCE, { allianceEntryList } }}) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint8 id, uint32 worldState, std::initializer_list<uint32> hordeEntryList, std::initializer_list<uint32> allianceEntryList, std::initializer_list<uint32> neutralEntryList) : EntityType(type), Id(id), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_HORDE, { hordeEntryList } }, { PVP_TEAM_ALLIANCE, { allianceEntryList } }, { PVP_TEAM_NEUTRAL, { neutralEntryList } }}) { }

    bool ValidateObjectEntry(uint32 entry) const;

    BattlefieldEntityType EntityType;
    uint8 Id;
    uint32 WorldState;
    std::unordered_map<PvPTeamId, std::vector<uint32 /*entry*/>> ObjectEntriesByPvPTeamId;
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

    virtual void OnObjectCreate(WorldObject* object);
    virtual void OnObjectRemove(WorldObject* object);
    virtual PvPTeamId GetPvPTeamId() const { return PVP_TEAM_NEUTRAL; }
    virtual void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet);
    virtual void InitializeState() { }
    virtual void SaveWorldState() { }

    bool ValidateObjectEntry(uint32 entry) const { return Info.ValidateObjectEntry(entry); }
    bool ValidateObjectGUID(ObjectGuid reference) const;

    Battlefield* Battle;
    BattlefieldEntityInfo Info;
    std::unordered_map<PvPTeamId, GuidUnorderedSet> ObjectGUIDsByPvPTeamId;
};

struct BattlefieldBuildingInfo
{
    BattlefieldBuildingInfo(uint8 id, uint32 worldState, uint32 entry, BattlefieldBuildingType type, uint32 hordeSpawnGroupId = 0, uint32 allianceSpawnGroupId = 0) : BattlefieldBuildingInfo(BattlefieldEntityInfo(BATTLEFIELD_ENTITY_TYPE_BUILDING, id, worldState, entry), type, hordeSpawnGroupId, allianceSpawnGroupId) { }
    BattlefieldBuildingInfo(BattlefieldEntityInfo const info, BattlefieldBuildingType type, uint32 hordeSpawnGroupId = 0, uint32 allianceSpawnGroupId = 0) : Info(info), Type(type), SpawnGroupIds{ hordeSpawnGroupId, allianceSpawnGroupId } { }

    BattlefieldEntityInfo Info;
    BattlefieldBuildingType Type;
    std::array<uint32, PVP_TEAMS_COUNT> SpawnGroupIds;
};

class BattlefieldBuilding : public BattlefieldEntity
{
public:
    explicit BattlefieldBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info);
    virtual ~BattlefieldBuilding() { }

    PvPTeamId GetPvPTeamId() const override;
    void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override;
    void SaveWorldState() override;

    virtual PvPTeamId GetFactionPvPTeamId() const { return GetPvPTeamId(); }
    virtual void Reset() { }
    virtual void ChangeTeam() { }
    virtual void Update(std::vector<Player*> const& players, Milliseconds diff);
    virtual bool Damage();
    virtual bool Destroy();
    GameObjectDestructibleState GetDestructibleState() const;

    BattlefieldBuildingType BuildingType;
    BattlefieldBuildingState State;
    std::array<uint32, PVP_TEAMS_COUNT> SpawnGroupIds;

protected:
    std::vector<GameObject*> GetGameObjects() const;
    bool SetState(PvPTeamId pvpTeamId, GameObjectDestructibleState destructibleState);
    void UpdateWorldState();
    void UpdateSpawnGroups() const;
};

class BattlefieldDestructibleBuilding : public BattlefieldBuilding
{
public:
    explicit BattlefieldDestructibleBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info);
    virtual ~BattlefieldDestructibleBuilding() { }

    void OnObjectCreate(WorldObject* object) override;
    void InitializeState() override;
    PvPTeamId GetFactionPvPTeamId() const override;
    void Reset() override;
    void ChangeTeam() override;

private:
    PvPTeamId GetOwnerPvPTeamId() const;
    uint32 GetFaction() const;
};

struct BattlefieldCapturePointInfo
{
    BattlefieldCapturePointInfo(uint8 id, uint32 worldState, std::initializer_list<uint32> hordeEntryList, std::initializer_list<uint32> allianceEntryList, uint8 graveyardId, bool capturable, bool startsWithAttacker, bool keepsOwner, uint32 hordeSpawnGroupId = 0, uint32 allianceSpawnGroupId = 0) : Info(BattlefieldEntityInfo(BATTLEFIELD_ENTITY_TYPE_CAPTUREPOINT, id, worldState, hordeEntryList, allianceEntryList), BATTLEFIELD_BUILDING_TYPE_WORKSHOP, hordeSpawnGroupId, allianceSpawnGroupId), GraveyardId(graveyardId), Capturable(capturable), StartsWithAttacker(startsWithAttacker), KeepsOwner(keepsOwner) { }

    BattlefieldBuildingInfo Info;
    uint8 GraveyardId;
    bool Capturable;
    bool StartsWithAttacker;
    bool KeepsOwner;
};

class BattlefieldCapturePoint : public BattlefieldBuilding
{
public:
    explicit BattlefieldCapturePoint(Battlefield* battlefield, BattlefieldCapturePointInfo const info);
    virtual ~BattlefieldCapturePoint() { }

    void OnObjectCreate(WorldObject* object) override;
    void InitializeState() override;
    void Reset() override;
    void ChangeTeam() override;
    void Update(std::vector<Player*> const& players, Milliseconds diff) override;

    virtual void SetOwner(PvPTeamId pvpTeamId);
    virtual void Attack(PvPTeamId /*pvpTeamId*/) { }
    virtual void Capture(PvPTeamId pvpTeamId);

    uint8 GraveyardId;
    bool Capturable;
    bool StartsWithAttacker;
    bool KeepsOwner;

private:
    GameObjectTemplate const* GetBannerTemplate() const;
    void SendCaptureBar(Player* player, bool show) const;
    void SendCaptureBarValue() const;

    float _slider;
    bool _contested;
    GuidUnorderedSet _capturePlayers;
};

struct BattlefieldGraveyardInfo
{
    BattlefieldGraveyardInfo(uint8 id, uint32 worldState, uint32 hordeEntry, uint32 allianceEntry, uint32 neutralEntry, uint16 worldSafeLocsEntryId, uint32 textId, uint32 teleportSpellId) : Info(BATTLEFIELD_ENTITY_TYPE_GRAVEYARD, id, worldState, { hordeEntry }, { allianceEntry }, { neutralEntry }), WorldSafeLocsEntryId(worldSafeLocsEntryId), TextId(textId), TeleportSpellId(teleportSpellId) { }

    BattlefieldEntityInfo Info;
    uint16 WorldSafeLocsEntryId;
    uint32 TextId;
    uint32 TeleportSpellId;
};

class BattlefieldGraveyard : public BattlefieldEntity
{
public:
    explicit BattlefieldGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info);
    virtual ~BattlefieldGraveyard() { }

    void AddPlayerToResurrectionQueue(Player* player);
    void RemovePlayerFromResurrectionQueue(Player* player);
    void ResurrectPlayers();
    void SetOwner(PvPTeamId pvpTeamId);

    PvPTeamId GetPvPTeamId() const override;
    void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet) override;
    void SaveWorldState() override;
    uint16 GetWorldSafeLocsEntryId() const { return WorldSafeLocsEntryId; }
    uint32 GetTeleportSpellId() const { return TeleportSpellId; }

    uint16 WorldSafeLocsEntryId;
    uint32 TextId;
    BattlefieldGraveyardState State;
    GuidUnorderedSet ResurrectionQueue;
    uint32 TeleportSpellId;
};

#endif
