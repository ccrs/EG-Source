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
#include "ObjectGuid.h"
#include "SharedDefines.h"
#include <initializer_list>
#include <unordered_map>
#include <vector>

class Battlefield;
class Player;
class WorldObject;

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
    BATTLEFIELD_BUILDING_TYPE_TITANRELIC
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

enum BattlefieldCapturePointState : uint8
{
    BATTLEFIELD_CAPTUREPOINT_STATE_NEUTRAL = 0,
    BATTLEFIELD_CAPTUREPOINT_STATE_ALLIANCE,
    BATTLEFIELD_CAPTUREPOINT_STATE_HORDE,
    BATTLEFIELD_CAPTUREPOINT_STATE_NEUTRAL_ALLIANCE_CHALLENGE,
    BATTLEFIELD_CAPTUREPOINT_STATE_NEUTRAL_HORDE_CHALLENGE,
    BATTLEFIELD_CAPTUREPOINT_STATE_ALLIANCE_HORDE_CHALLENGE,
    BATTLEFIELD_CAPTUREPOINT_STATE_HORDE_ALLIANCE_CHALLENGE
};

enum BattlefieldGraveyardState : uint8
{
    BATTLEFIELD_GRAVEYARD_STATE_NEUTRAL = 0,
    BATTLEFIELD_GRAVEYARD_STATE_ALLIANCE,
    BATTLEFIELD_GRAVEYARD_STATE_HORDE,
};

struct BattlefieldEntityInfo
{
    BattlefieldEntityInfo(BattlefieldEntityType type) : EntityType(type), WorldState(0) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint32 worldState) : EntityType(type), WorldState(worldState) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint32 worldState, uint32 entry) : EntityType(type), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_NEUTRAL, { entry } }}) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint32 worldState, std::initializer_list<uint32> neutralEntryList) : EntityType(type), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_NEUTRAL, { neutralEntryList } }}) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint32 worldState, std::initializer_list<uint32> hordeEntryList, std::initializer_list<uint32> allianceEntryList) : EntityType(type), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_HORDE, { hordeEntryList } }, { PVP_TEAM_ALLIANCE, { allianceEntryList } }}) { }
    BattlefieldEntityInfo(BattlefieldEntityType type, uint32 worldState, std::initializer_list<uint32> hordeEntryList, std::initializer_list<uint32> allianceEntryList, std::initializer_list<uint32> neutralEntryList) : EntityType(type), WorldState(worldState), ObjectEntriesByPvPTeamId({{ PVP_TEAM_HORDE, { hordeEntryList } }, { PVP_TEAM_ALLIANCE, { allianceEntryList } }, { PVP_TEAM_NEUTRAL, { neutralEntryList } }}) { }

    uint32 GetNeutralObjectEntry() const { return ObjectEntriesByPvPTeamId.contains(PVP_TEAM_NEUTRAL) ? ObjectEntriesByPvPTeamId.at(PVP_TEAM_NEUTRAL)[0] : 0; }
    bool ValidateObjectEntry(uint32 entry) const;

    BattlefieldEntityType EntityType;
    uint32 WorldState;
    std::unordered_map<PvPTeamId, std::vector<uint32 /*entry*/>> ObjectEntriesByPvPTeamId;
};

class BattlefieldEntity
{
public:
    explicit BattlefieldEntity(Battlefield* battlefield, BattlefieldEntityInfo const info);
    virtual ~BattlefieldEntity() { }

    virtual void OnObjectCreate(WorldObject* object);
    virtual void OnObjectRemove(WorldObject* object);
    virtual void Update(uint32 /*diff*/) { }
    virtual PvPTeamId GetPvPTeamId() const { return PVP_TEAM_NEUTRAL; }

    bool ValidateObjectEntry(uint32 entry) const { return Info.ValidateObjectEntry(entry); }
    bool ValidateObjectGUID(ObjectGuid reference) const;

    Battlefield* Battle;
    BattlefieldEntityInfo Info;
    std::unordered_map<PvPTeamId, GuidUnorderedSet> ObjectGUIDsByPvPTeamId;
};

struct BattlefieldBuildingInfo
{
    BattlefieldBuildingInfo(uint32 worldState, uint32 entry, BattlefieldBuildingType type) : BattlefieldBuildingInfo(BattlefieldEntityInfo(BATTLEFIELD_ENTITY_TYPE_BUILDING, worldState, entry), type) { }
    BattlefieldBuildingInfo(BattlefieldEntityInfo const info, BattlefieldBuildingType type) : Info(info), Type(type) { }

    BattlefieldEntityInfo Info;
    BattlefieldBuildingType Type;
};

class BattlefieldBuilding : public BattlefieldEntity
{
public:
    explicit BattlefieldBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info);
    virtual ~BattlefieldBuilding() { }

    PvPTeamId GetPvPTeamId() const override;

    bool IsIntact() const { return State == BATTLEFIELD_BUILDING_STATE_NEUTRAL_INTACT || State == BATTLEFIELD_BUILDING_STATE_HORDE_INTACT || State == BATTLEFIELD_BUILDING_STATE_ALLIANCE_INTACT; }
    bool IsDestroyed() const { return State == BATTLEFIELD_BUILDING_STATE_NEUTRAL_DESTROYED || State == BATTLEFIELD_BUILDING_STATE_HORDE_DESTROYED || State == BATTLEFIELD_BUILDING_STATE_ALLIANCE_DESTROYED; }
    bool IsDamaged() const { return State == BATTLEFIELD_BUILDING_STATE_NEUTRAL_DAMAGED || State == BATTLEFIELD_BUILDING_STATE_HORDE_DAMAGED || State == BATTLEFIELD_BUILDING_STATE_ALLIANCE_DAMAGED; }

    BattlefieldBuildingType BuildingType;
    BattlefieldBuildingState State;
};

class BattlefieldCapturePoint : public BattlefieldEntity
{
public:
    explicit BattlefieldCapturePoint(Battlefield* battlefield, uint32 worldState, uint32 entry) : BattlefieldCapturePoint(battlefield, BattlefieldEntityInfo(BATTLEFIELD_ENTITY_TYPE_CAPTUREPOINT, worldState, entry)) { }
    explicit BattlefieldCapturePoint(Battlefield* battlefield, BattlefieldEntityInfo const info);
    virtual ~BattlefieldCapturePoint() { }

    PvPTeamId GetPvPTeamId() const override;

    BattlefieldCapturePointState State;
};

struct BattlefieldGraveyardInfo
{
    BattlefieldGraveyardInfo(uint32 worldState, uint32 hordeEntry, uint32 allianceEntry, uint32 neutralEntry, uint8 id, uint16 worldSafeLocsEntryId, uint32 textId) : Info(BATTLEFIELD_ENTITY_TYPE_GRAVEYARD, worldState, { hordeEntry }, { allianceEntry }, { neutralEntry }), Id(id), WorldSafeLocsEntryId(worldSafeLocsEntryId), TextId(textId) { }

    BattlefieldEntityInfo Info;
    uint8 Id;
    uint16 WorldSafeLocsEntryId;
    uint32 TextId;
};

class BattlefieldGraveyard : public BattlefieldEntity
{
public:
    explicit BattlefieldGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info);
    virtual ~BattlefieldGraveyard() { }

    void AddPlayerToResurrectionQueue(Player* player);
    void RemovePlayerFromResurrectionQueue(Player* player);
    void ResurrectPlayers();

    PvPTeamId GetPvPTeamId() const override;
    uint16 GetWorldSafeLocsEntryId() const { return WorldSafeLocsEntryId; }

    uint8 Id;
    uint16 WorldSafeLocsEntryId;
    uint32 TextId;
    BattlefieldGraveyardState State;
    GuidUnorderedSet ResurrectionQueue;
};

#endif
