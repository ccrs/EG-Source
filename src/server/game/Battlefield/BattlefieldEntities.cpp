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

#include "BattlefieldEntities.h"
#include "Battlefield.h"
#include "Creature.h"
#include "Errors.h"
#include "GameObject.h"
#include "MapUtils.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldStatePackets.h"
#include <algorithm>
#include <array>

BattlefieldEntity::BattlefieldEntity(Battlefield* battlefield, BattlefieldEntityInfo const info) : Battle(battlefield), Info(info)
{
    ASSERT(battlefield, "BattlefieldEntity::BattlefieldEntity: Tried to construct an entity without a battlefield! (type: %u, worldState: %u)", info.EntityType, info.WorldState);
}

void BattlefieldEntity::OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo)
{
    ObjectGUIDs[&objectInfo].insert(object->GetGUID());
}

void BattlefieldEntity::OnObjectRemove(WorldObject* object, BattlefieldObjectInfo const& objectInfo)
{
    ObjectGUIDs[&objectInfo].erase(object->GetGUID());
}

bool BattlefieldEntity::IsObjectInfoActive(BattlefieldObjectInfo const& objectInfo) const
{
    if (objectInfo.Phase == BATTLEFIELD_OBJECT_INFO_PHASE_BATTLE && !Battle->IsBattleTime())
        return false;

    if (objectInfo.Phase == BATTLEFIELD_OBJECT_INFO_PHASE_IDLE && Battle->GetPhase() != BATTLEFIELD_PHASE_IDLE)
        return false;

    if (objectInfo.Team == PVP_TEAM_NEUTRAL)
        return true;

    PvPTeamId const displayedPvPTeamId = GetDisplayedPvPTeamId();
    if (!objectInfo.Flags.HasFlag(BATTLEFIELD_OBJECT_INFO_FLAG_OPPOSITE_TEAM))
        return objectInfo.Team == displayedPvPTeamId;

    return displayedPvPTeamId != PVP_TEAM_NEUTRAL && objectInfo.Team != displayedPvPTeamId;
}

void BattlefieldEntity::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(Info.WorldState, 0);
}

void BattlefieldEntity::UpdateObjectSpawns()
{
    std::vector<BattlefieldObjectInfo> const* objectInfos = GetObjectInfos();
    if (!objectInfos)
        return;

    for (BattlefieldObjectInfo const& objectInfo : *objectInfos)
    {
        if (!objectInfo.SpawnGroupId)
            continue;

        bool const active = IsObjectInfoActive(objectInfo);
        GuidUnorderedSet const* objectGuids = Trinity::Containers::MapGetValuePtr(ObjectGUIDs, &objectInfo);
        if (!active && objectGuids)
        {
            for (ObjectGuid objectGuid : *objectGuids)
                if (GameObject* gameObject = Battle->GetGameObject(objectGuid))
                    gameObject->SendObjectDeSpawnAnim(objectGuid);
        }

        Battle->SetSpawnGroupSpawned(objectInfo.SpawnGroupId, active);
    }
}

BattlefieldObjectInfo const* BattlefieldEntity::FindObjectInfo(WorldObject const* object) const
{
    std::vector<BattlefieldObjectInfo> const* objectInfos = GetObjectInfos();
    if (!objectInfos)
        return nullptr;

    SpawnData const* spawnData = nullptr;
    if (Creature const* creature = object->ToCreature())
        spawnData = creature->GetCreatureData();
    else if (GameObject const* gameObject = object->ToGameObject())
        spawnData = gameObject->GetGameObjectData();

    for (BattlefieldObjectInfo const& objectInfo : *objectInfos)
    {
        if (objectInfo.SpawnGroupId)
        {
            if (spawnData && spawnData->spawnGroupData->groupId == objectInfo.SpawnGroupId)
                return &objectInfo;

            continue;
        }

        if (objectInfo.ObjectType != object->GetTypeId())
            continue;

        bool const entryMatches = std::ranges::find(objectInfo.Entries, object->GetEntry()) != objectInfo.Entries.end();
        if (!objectInfo.SpawnIds.empty())
        {
            if (spawnData && std::ranges::find(objectInfo.SpawnIds, spawnData->spawnId) != objectInfo.SpawnIds.end() && (objectInfo.Entries.empty() || entryMatches))
                return &objectInfo;
        }
        else if (entryMatches)
            return &objectInfo;
    }

    return nullptr;
}

bool BattlefieldEntity::ValidateObjectGUID(ObjectGuid reference) const
{
    return std::any_of(ObjectGUIDs.begin(), ObjectGUIDs.end(), [&](std::unordered_map<BattlefieldObjectInfo const*, GuidUnorderedSet>::value_type const& pair) -> bool
    {
        return pair.second.contains(reference);
    });
}

BattlefieldZone::BattlefieldZone(Battlefield* battlefield) : BattlefieldEntity(battlefield, BattlefieldEntityInfo(BATTLEFIELD_ENTITY_TYPE_ZONE, 0, 0))
{
}

PvPTeamId BattlefieldZone::GetDisplayedPvPTeamId() const
{
    return Battle->GetControllingTeam();
}

BattlefieldBuilding::BattlefieldBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info) : BattlefieldEntity(battlefield, info.Info), BuildingType(info.Type), State(BATTLEFIELD_BUILDING_STATE_NONE)
{
}

PvPTeamId BattlefieldBuilding::GetPvPTeamId() const
{
    switch (State)
    {
        case BATTLEFIELD_BUILDING_STATE_HORDE_INTACT:
        case BATTLEFIELD_BUILDING_STATE_HORDE_DAMAGED:
        case BATTLEFIELD_BUILDING_STATE_HORDE_DESTROYED:
            return PVP_TEAM_HORDE;
        case BATTLEFIELD_BUILDING_STATE_ALLIANCE_INTACT:
        case BATTLEFIELD_BUILDING_STATE_ALLIANCE_DAMAGED:
        case BATTLEFIELD_BUILDING_STATE_ALLIANCE_DESTROYED:
            return PVP_TEAM_ALLIANCE;
        default:
            return PVP_TEAM_NEUTRAL;
    }
}

bool BattlefieldBuilding::IsObjectInfoActive(BattlefieldObjectInfo const& objectInfo) const
{
    if (!BattlefieldEntity::IsObjectInfoActive(objectInfo))
        return false;

    return !objectInfo.Flags.HasFlag(BATTLEFIELD_OBJECT_INFO_FLAG_NOT_DESTROYED) || GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED;
}

void BattlefieldBuilding::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(Info.WorldState, State);
}

void BattlefieldBuilding::SaveWorldState()
{
    sWorld->setWorldState(Info.WorldState, State);
}

bool BattlefieldBuilding::Damage()
{
    if (GetDestructibleState() >= GO_DESTRUCTIBLE_DAMAGED)
        return false;

    SetState(GetPvPTeamId(), GO_DESTRUCTIBLE_DAMAGED);
    UpdateWorldState();
    return true;
}

bool BattlefieldBuilding::Destroy(Player* /*destroyer*/)
{
    if (GetDestructibleState() >= GO_DESTRUCTIBLE_DESTROYED)
        return false;

    SetState(GetPvPTeamId(), GO_DESTRUCTIBLE_DESTROYED);
    UpdateWorldState();
    UpdateObjectSpawns();
    return true;
}

GameObjectDestructibleState BattlefieldBuilding::GetDestructibleState() const
{
    if (State == BATTLEFIELD_BUILDING_STATE_NONE)
        return GO_DESTRUCTIBLE_INTACT;

    return GameObjectDestructibleState((State - BATTLEFIELD_BUILDING_STATE_NEUTRAL_INTACT) % 3);
}

std::vector<GameObject*> BattlefieldBuilding::GetGameObjects() const
{
    std::vector<GameObject*> gameObjects;
    for (std::unordered_map<BattlefieldObjectInfo const*, GuidUnorderedSet>::value_type const& pair : ObjectGUIDs)
    {
        if (pair.first->SpawnGroupId)
            continue;

        for (ObjectGuid objectGuid : pair.second)
            if (GameObject* gameObject = Battle->GetGameObject(objectGuid))
                gameObjects.push_back(gameObject);
    }

    return gameObjects;
}

bool BattlefieldBuilding::SetState(PvPTeamId pvpTeamId, GameObjectDestructibleState destructibleState)
{
    BattlefieldBuildingState intactState = BATTLEFIELD_BUILDING_STATE_NEUTRAL_INTACT;
    switch (pvpTeamId)
    {
        case PVP_TEAM_HORDE:
            intactState = BATTLEFIELD_BUILDING_STATE_HORDE_INTACT;
            break;
        case PVP_TEAM_ALLIANCE:
            intactState = BATTLEFIELD_BUILDING_STATE_ALLIANCE_INTACT;
            break;
        default:
            break;
    }

    BattlefieldBuildingState const previousState = State;
    State = BattlefieldBuildingState(intactState + uint8(destructibleState));
    return State != previousState;
}

void BattlefieldBuilding::UpdateWorldState()
{
    SaveWorldState();
    Battle->SendWorldStateToMap(Info.WorldState, State);
}

BattlefieldDestructibleBuilding::BattlefieldDestructibleBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info) : BattlefieldBuilding(battlefield, info)
{
}

void BattlefieldDestructibleBuilding::OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo)
{
    BattlefieldBuilding::OnObjectCreate(object, objectInfo);

    GameObject* gameObject = object->ToGameObject();
    if (!gameObject || objectInfo.SpawnGroupId)
        return;

    if (uint32 const faction = GetFaction())
        gameObject->SetFaction(faction);

    if (GetDestructibleState() != GO_DESTRUCTIBLE_INTACT)
        gameObject->SetDestructibleState(GetDestructibleState(), nullptr, true);
}

PvPTeamId BattlefieldDestructibleBuilding::GetDisplayedPvPTeamId() const
{
    return BuildingType == BATTLEFIELD_BUILDING_TYPE_TOWER ? Battle->GetAttackingTeam() : Battle->GetControllingTeam();
}

void BattlefieldDestructibleBuilding::InitializeState()
{
    uint64 const savedState = sWorld->getWorldState(Info.WorldState);
    State = savedState <= BATTLEFIELD_BUILDING_STATE_ALLIANCE_DESTROYED ? BattlefieldBuildingState(savedState) : BATTLEFIELD_BUILDING_STATE_NONE;
    SetState(GetOwnerPvPTeamId(), GetDestructibleState());
    SaveWorldState();
}

void BattlefieldDestructibleBuilding::Reset()
{
    BattlefieldBuilding::Reset();

    if (SetState(GetOwnerPvPTeamId(), GO_DESTRUCTIBLE_INTACT))
        UpdateWorldState();

    uint32 const faction = GetFaction();
    for (GameObject* gameObject : GetGameObjects())
    {
        if (faction)
            gameObject->SetFaction(faction);

        gameObject->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING, nullptr, true);
    }
}

void BattlefieldDestructibleBuilding::ChangeTeam()
{
    BattlefieldBuilding::ChangeTeam();

    if (SetState(GetOwnerPvPTeamId(), GetDestructibleState()))
        UpdateWorldState();

    if (uint32 const faction = GetFaction())
        for (GameObject* gameObject : GetGameObjects())
            gameObject->SetFaction(faction);
}

PvPTeamId BattlefieldDestructibleBuilding::GetOwnerPvPTeamId() const
{
    if (BuildingType != BATTLEFIELD_BUILDING_TYPE_TOWER)
        return Battle->GetControllingTeam();

    return Battle->IsBattleTime() || !Battle->IsEnabled() ? Battle->GetAttackingTeam() : PVP_TEAM_NEUTRAL;
}

uint32 BattlefieldDestructibleBuilding::GetFaction() const
{
    if (!Battle->IsEnabled() || !Battle->IsBattleTime())
        return FACTION_FRIENDLY;

    return Battle->GetFaction(GetDisplayedPvPTeamId());
}

BattlefieldCapturePoint::BattlefieldCapturePoint(Battlefield* battlefield, BattlefieldCapturePointInfo const info) : BattlefieldBuilding(battlefield, info.Info), GraveyardId(info.GraveyardId), Capturable(info.Capturable), StartsWithAttacker(info.StartsWithAttacker), KeepsOwner(info.KeepsOwner), _slider(0.0f), _contested(false)
{
}

void BattlefieldCapturePoint::OnObjectCreate(WorldObject* object, BattlefieldObjectInfo const& objectInfo)
{
    BattlefieldBuilding::OnObjectCreate(object, objectInfo);

    GameObject* banner = object->ToGameObject();
    if (!banner || objectInfo.SpawnGroupId)
        return;

    banner->SetGoArtKit(objectInfo.Team == PVP_TEAM_HORDE ? BATTLEFIELD_CAPTUREPOINT_ARTKIT_HORDE : BATTLEFIELD_CAPTUREPOINT_ARTKIT_ALLIANCE);
}

void BattlefieldCapturePoint::InitializeState()
{
    PvPTeamId owner = Battle->GetControllingTeam();
    if (Capturable && KeepsOwner && Battle->IsEnabled())
    {
        uint64 const savedState = sWorld->getWorldState(Info.WorldState);
        if (savedState == BATTLEFIELD_BUILDING_STATE_HORDE_INTACT)
            owner = PVP_TEAM_HORDE;
        else if (savedState == BATTLEFIELD_BUILDING_STATE_ALLIANCE_INTACT)
            owner = PVP_TEAM_ALLIANCE;
    }

    SetOwner(owner);
}

void BattlefieldCapturePoint::Reset()
{
    BattlefieldBuilding::Reset();

    SetOwner(Capturable && StartsWithAttacker ? Battle->GetAttackingTeam() : Battle->GetControllingTeam());
}

void BattlefieldCapturePoint::ChangeTeam()
{
    BattlefieldBuilding::ChangeTeam();

    if (!Capturable || !KeepsOwner || !Battle->IsEnabled())
        SetOwner(Battle->GetControllingTeam());
}

void BattlefieldCapturePoint::Update(std::vector<Player*> const& players, Milliseconds diff)
{
    BattlefieldBuilding::Update(players, diff);

    GameObjectTemplate const* bannerTemplate = GetBannerTemplate();
    if (!Capturable || !bannerTemplate)
        return;

    std::vector<GameObject*> const banners = GetGameObjects();
    GameObject* banner = banners.empty() ? nullptr : banners.front();

    std::array<uint32, PVP_TEAMS_COUNT> counts = { };
    GuidUnorderedSet playersInRange;
    for (Player* player : players)
    {
        if (!banner || !player->IsOutdoorPvPActive() || !banner->IsInMap(player) || banner->GetExactDist(player) > float(bannerTemplate->capturePoint.radius))
            continue;

        ++counts[player->GetTeamId()];
        playersInRange.insert(player->GetGUID());
        if (_capturePlayers.insert(player->GetGUID()).second)
            SendCaptureBar(player, true);
    }

    for (GuidUnorderedSet::iterator itr = _capturePlayers.begin(); itr != _capturePlayers.end();)
    {
        if (playersInRange.contains(*itr))
        {
            ++itr;
            continue;
        }

        if (Player* player = ObjectAccessor::FindPlayer(*itr))
            SendCaptureBar(player, false);

        itr = _capturePlayers.erase(itr);
    }

    int32 const superiority = int32(counts[TEAM_ALLIANCE]) - int32(counts[TEAM_HORDE]);
    uint32 const minSuperiority = bannerTemplate->capturePoint.minSuperiority;
    uint32 const maxSuperiority = bannerTemplate->capturePoint.maxSuperiority;
    if (!superiority || uint32(std::abs(superiority)) < minSuperiority)
        return;

    float const minTime = float(bannerTemplate->capturePoint.minTime);
    float const maxTime = float(bannerTemplate->capturePoint.maxTime);
    float fullTime = minTime;
    if (maxSuperiority > minSuperiority)
        fullTime = maxTime - (maxTime - minTime) * float(std::clamp<uint32>(std::abs(superiority), minSuperiority, maxSuperiority) - minSuperiority) / float(maxSuperiority - minSuperiority);

    float const previousSlider = _slider;
    _slider = std::clamp(_slider + (superiority > 0 ? 100.0f : -100.0f) * std::chrono::duration<float>(diff).count() / fullTime, 0.0f, 100.0f);
    if (uint32(_slider) != uint32(previousSlider))
        SendCaptureBarValue();

    PvPTeamId const challenger = superiority > 0 ? PVP_TEAM_ALLIANCE : PVP_TEAM_HORDE;
    if (challenger == GetPvPTeamId())
    {
        if (_slider == (challenger == PVP_TEAM_ALLIANCE ? 100.0f : 0.0f))
            _contested = false;

        return;
    }

    if (!_contested)
    {
        _contested = true;
        Attack(challenger);
    }

    float const threshold = (100.0f - float(bannerTemplate->capturePoint.neutralPercent)) / 2.0f;
    if (challenger == PVP_TEAM_ALLIANCE ? _slider >= 100.0f - threshold : _slider <= threshold)
        Capture(challenger);
}

void BattlefieldCapturePoint::SetOwner(PvPTeamId pvpTeamId)
{
    _slider = pvpTeamId == PVP_TEAM_ALLIANCE ? 100.0f : 0.0f;
    _contested = false;
    SendCaptureBarValue();

    if (!SetState(pvpTeamId, GO_DESTRUCTIBLE_INTACT))
        return;

    UpdateWorldState();
    UpdateObjectSpawns();
    if (BattlefieldGraveyard* graveyard = Battle->GetEntity<BattlefieldGraveyard>(GraveyardId))
        graveyard->SetOwner(pvpTeamId);

    Battle->QueueEvent<BattlefieldZoneRefreshEvent>();
}

void BattlefieldCapturePoint::Capture(PvPTeamId pvpTeamId)
{
    SetOwner(pvpTeamId);
}

GameObjectTemplate const* BattlefieldCapturePoint::GetBannerTemplate() const
{
    if (std::vector<BattlefieldObjectInfo> const* objectInfos = GetObjectInfos())
        for (BattlefieldObjectInfo const& objectInfo : *objectInfos)
            if (!objectInfo.Entries.empty())
                return sObjectMgr->GetGameObjectTemplate(objectInfo.Entries.front());

    return nullptr;
}

void BattlefieldCapturePoint::SendCaptureBar(Player* player, bool show) const
{
    GameObjectTemplate const* bannerTemplate = GetBannerTemplate();
    if (!bannerTemplate)
        return;

    player->SendUpdateWorldState(bannerTemplate->capturePoint.worldState1, show ? 1 : 0);
    if (!show)
        return;

    player->SendUpdateWorldState(bannerTemplate->capturePoint.worldstate2, uint32(_slider));
    player->SendUpdateWorldState(bannerTemplate->capturePoint.worldstate3, bannerTemplate->capturePoint.neutralPercent);
}

void BattlefieldCapturePoint::SendCaptureBarValue() const
{
    GameObjectTemplate const* bannerTemplate = GetBannerTemplate();
    if (!bannerTemplate)
        return;

    for (ObjectGuid playerGuid : _capturePlayers)
        if (Player* player = ObjectAccessor::FindPlayer(playerGuid))
            player->SendUpdateWorldState(bannerTemplate->capturePoint.worldstate2, uint32(_slider));
}

BattlefieldGraveyard::BattlefieldGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info) : BattlefieldEntity(battlefield, info.Info), WorldSafeLocsEntryId(info.WorldSafeLocsEntryId), TextId(info.TextId), State(BATTLEFIELD_GRAVEYARD_STATE_NEUTRAL), TeleportSpellId(info.TeleportSpellId)
{
}

PvPTeamId BattlefieldGraveyard::GetPvPTeamId() const
{
    switch (State)
    {
        case BATTLEFIELD_GRAVEYARD_STATE_HORDE:
            return PVP_TEAM_HORDE;
        case BATTLEFIELD_GRAVEYARD_STATE_ALLIANCE:
            return PVP_TEAM_ALLIANCE;
        default:
            return PVP_TEAM_NEUTRAL;
    }
}

void BattlefieldGraveyard::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(Info.WorldState, State);
}

void BattlefieldGraveyard::SaveWorldState()
{
    sWorld->setWorldState(Info.WorldState, State);
}

void BattlefieldGraveyard::AddPlayerToResurrectionQueue(Player* player)
{
    if (player->IsAlive())
        return;

    if (ResurrectionQueue.insert(player->GetGUID()).second)
        player->CastSpell(player, SPELL_BATTLEFIELD_WAITING_FOR_RESURRECT, true);
}

void BattlefieldGraveyard::RemovePlayerFromResurrectionQueue(Player* player)
{
    auto itr = ResurrectionQueue.find(player->GetGUID());
    if (itr != ResurrectionQueue.end())
    {
        ResurrectionQueue.erase(player->GetGUID());
        player->RemoveAurasDueToSpell(SPELL_BATTLEFIELD_WAITING_FOR_RESURRECT);
    }
}

void BattlefieldGraveyard::ResurrectPlayers()
{
    if (ResurrectionQueue.empty())
        return;

    for (std::unordered_map<BattlefieldObjectInfo const*, GuidUnorderedSet>::value_type const& pair : ObjectGUIDs)
    {
        if (pair.first->Team != GetPvPTeamId() || pair.second.empty())
            continue;

        if (Creature* spirit = Battle->GetCreature(*pair.second.begin()))
            spirit->CastSpell(spirit, SPELL_BATTLEFIELD_SPIRIT_HEAL, true);

        break;
    }

    GuidUnorderedSet const resurrectionQueue = std::move(ResurrectionQueue);
    ResurrectionQueue.clear();
    for (ObjectGuid playerGuid : resurrectionQueue)
    {
        Player* player = ObjectAccessor::FindPlayer(playerGuid);
        if (!player)
            continue;

        player->RemoveAurasDueToSpell(SPELL_BATTLEFIELD_WAITING_FOR_RESURRECT);
        if (player->IsAlive() || player->GetZoneId() != Battle->GetZoneId())
            continue;

        player->CastSpell(player, SPELL_BATTLEFIELD_RESURRECTION_VISUAL, true);
        player->ResurrectPlayer(1.0f);
        player->CastSpell(player, SPELL_BATTLEFIELD_SPIRIT_HEAL_MANA, true);
        player->SpawnCorpseBones(false);
    }
}

void BattlefieldGraveyard::SetOwner(PvPTeamId pvpTeamId)
{
    switch (pvpTeamId)
    {
        case PVP_TEAM_HORDE:
            State = BATTLEFIELD_GRAVEYARD_STATE_HORDE;
            break;
        case PVP_TEAM_ALLIANCE:
            State = BATTLEFIELD_GRAVEYARD_STATE_ALLIANCE;
            break;
        default:
            State = BATTLEFIELD_GRAVEYARD_STATE_NEUTRAL;
            break;
    }

    UpdateObjectSpawns();

    GuidUnorderedSet const resurrectionQueue = ResurrectionQueue;
    for (ObjectGuid playerGuid : resurrectionQueue)
    {
        Player* player = ObjectAccessor::FindPlayer(playerGuid);
        if (!player || player->IsAlive() || player->GetTeamId() == TeamIdByPvPTeamId(pvpTeamId))
            continue;

        if (BattlefieldGraveyard const* graveyard = Battle->GetClosestGraveyard(player))
            player->CastSpell(player, graveyard->GetTeleportSpellId(), true);
    }
}
