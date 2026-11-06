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
#include "Player.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldStatePackets.h"
#include <algorithm>

bool BattlefieldEntityInfo::ValidateObjectEntry(uint32 entry) const
{
    return std::any_of(ObjectEntriesByPvPTeamId.begin(), ObjectEntriesByPvPTeamId.end(), [&](std::unordered_map<PvPTeamId, std::vector<uint32 /*entry*/>>::value_type const& pair) -> bool
    {
        return std::ranges::find(pair.second, entry) != pair.second.end();
    });
}

BattlefieldEntity::BattlefieldEntity(Battlefield* battlefield, BattlefieldEntityInfo const info) : Battle(battlefield), Info(info)
{
    ASSERT(battlefield, "BattlefieldBuilding::BattlefieldEntity: Tried to construct an entity without a battlefield! (type: %u, worldState: %u)", info.EntityType, info.WorldState);
}

void BattlefieldEntity::OnObjectCreate(WorldObject* object)
{
    for (PvPTeamId currentTeam : { PVP_TEAM_HORDE, PVP_TEAM_ALLIANCE, PVP_TEAM_NEUTRAL })
    {
        if (std::vector<uint32> const* factionEntries = Trinity::Containers::MapGetValuePtr(Info.ObjectEntriesByPvPTeamId, currentTeam))
            if (std::ranges::find(*factionEntries, object->GetEntry()) != factionEntries->end())
                ObjectGUIDsByPvPTeamId[currentTeam].insert(object->GetGUID());
    }
}

void BattlefieldEntity::OnObjectRemove(WorldObject* object)
{
    for (PvPTeamId currentTeam : { PVP_TEAM_HORDE, PVP_TEAM_ALLIANCE, PVP_TEAM_NEUTRAL })
    {
        if (std::vector<uint32> const* factionEntries = Trinity::Containers::MapGetValuePtr(Info.ObjectEntriesByPvPTeamId, currentTeam))
            if (std::ranges::find(*factionEntries, object->GetEntry()) != factionEntries->end())
                ObjectGUIDsByPvPTeamId[currentTeam].erase(object->GetGUID());
    }
}

void BattlefieldEntity::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(Info.WorldState, 0);
}

bool BattlefieldEntity::ValidateObjectGUID(ObjectGuid reference) const
{
    return std::any_of(ObjectGUIDsByPvPTeamId.begin(), ObjectGUIDsByPvPTeamId.end(), [&](std::unordered_map<PvPTeamId, GuidUnorderedSet>::value_type const& pair) -> bool
    {
        return pair.second.contains(reference);
    });
}

BattlefieldBuilding::BattlefieldBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info) : BattlefieldEntity(battlefield, info.Info), BuildingType(info.Type), State(BATTLEFIELD_BUILDING_STATE_NEUTRAL_INTACT)
{
}

void BattlefieldBuilding::OnObjectCreate(WorldObject* object)
{
    BattlefieldEntity::OnObjectCreate(object);

    GameObject* gameObject = object->ToGameObject();
    if (!gameObject)
        return;

    if (uint32 const faction = GetFaction())
        gameObject->SetFaction(faction);

    if (GetDestructibleState() != GO_DESTRUCTIBLE_INTACT)
        gameObject->SetDestructibleState(GetDestructibleState(), nullptr, true);
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

void BattlefieldBuilding::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(Info.WorldState, State);
}

void BattlefieldBuilding::InitializeState()
{
    uint64 const savedState = sWorld->getWorldState(Info.WorldState);
    State = savedState <= BATTLEFIELD_BUILDING_STATE_ALLIANCE_DESTROYED ? BattlefieldBuildingState(savedState) : BATTLEFIELD_BUILDING_STATE_NONE;
    SetState(GetOwnerPvPTeamId(), GetDestructibleState());
    SaveWorldState();
}

void BattlefieldBuilding::SaveWorldState()
{
    sWorld->setWorldState(Info.WorldState, State);
}

GameObjectDestructibleState BattlefieldBuilding::GetDestructibleState() const
{
    if (State == BATTLEFIELD_BUILDING_STATE_NONE)
        return GO_DESTRUCTIBLE_INTACT;

    return GameObjectDestructibleState((State - BATTLEFIELD_BUILDING_STATE_NEUTRAL_INTACT) % 3);
}

bool BattlefieldBuilding::Damage()
{
    if (GetDestructibleState() >= GO_DESTRUCTIBLE_DAMAGED)
        return false;

    SetState(GetPvPTeamId(), GO_DESTRUCTIBLE_DAMAGED);
    UpdateWorldState();
    return true;
}

bool BattlefieldBuilding::Destroy()
{
    if (GetDestructibleState() >= GO_DESTRUCTIBLE_DESTROYED)
        return false;

    SetState(GetPvPTeamId(), GO_DESTRUCTIBLE_DESTROYED);
    UpdateWorldState();
    return true;
}

void BattlefieldBuilding::Rebuild()
{
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

void BattlefieldBuilding::ChangeTeam()
{
    if (SetState(GetOwnerPvPTeamId(), GetDestructibleState()))
        UpdateWorldState();

    if (uint32 const faction = GetFaction())
        for (GameObject* gameObject : GetGameObjects())
            gameObject->SetFaction(faction);
}

PvPTeamId BattlefieldBuilding::GetOwnerPvPTeamId() const
{
    if (BuildingType != BATTLEFIELD_BUILDING_TYPE_TOWER)
        return Battle->GetControllingTeam();

    return Battle->IsBattleTime() ? Battle->GetAttackingTeam() : PVP_TEAM_NEUTRAL;
}

uint32 BattlefieldBuilding::GetFaction() const
{
    if (!Battle->IsEnabled())
        return FACTION_FRIENDLY;

    return Battle->GetFaction(BuildingType == BATTLEFIELD_BUILDING_TYPE_TOWER ? Battle->GetAttackingTeam() : Battle->GetControllingTeam());
}

std::vector<GameObject*> BattlefieldBuilding::GetGameObjects() const
{
    std::vector<GameObject*> gameObjects;
    for (std::unordered_map<PvPTeamId, GuidUnorderedSet>::value_type const& pair : ObjectGUIDsByPvPTeamId)
        for (ObjectGuid objectGuid : pair.second)
            if (GameObject* gameObject = Battle->GetGameObject(objectGuid))
                gameObjects.push_back(gameObject);

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

BattlefieldCapturePoint::BattlefieldCapturePoint(Battlefield* battlefield, BattlefieldEntityInfo const info) : BattlefieldEntity(battlefield, info), State(BATTLEFIELD_CAPTUREPOINT_STATE_NEUTRAL)
{
}

PvPTeamId BattlefieldCapturePoint::GetPvPTeamId() const
{
    switch (State)
    {
        case BATTLEFIELD_CAPTUREPOINT_STATE_HORDE:
        case BATTLEFIELD_CAPTUREPOINT_STATE_HORDE_ALLIANCE_CHALLENGE:
            return PVP_TEAM_HORDE;
        case BATTLEFIELD_CAPTUREPOINT_STATE_ALLIANCE:
        case BATTLEFIELD_CAPTUREPOINT_STATE_ALLIANCE_HORDE_CHALLENGE:
            return PVP_TEAM_ALLIANCE;
        default:
            return PVP_TEAM_NEUTRAL;
    }
}

void BattlefieldCapturePoint::FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& packet)
{
    packet.Worldstates.emplace_back(Info.WorldState, State);
}

void BattlefieldCapturePoint::SaveWorldState()
{
    sWorld->setWorldState(Info.WorldState, State);
}

BattlefieldGraveyard::BattlefieldGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info) : BattlefieldEntity(battlefield, info.Info), Id(info.Id), WorldSafeLocsEntryId(info.WorldSafeLocsEntryId), TextId(info.TextId), State(BATTLEFIELD_GRAVEYARD_STATE_NEUTRAL), TeleportSpellId(info.TeleportSpellId), SpellAreaForced(false)
{
}

void BattlefieldGraveyard::AddPlayerToResurrectionQueue(Player* player)
{
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

    auto spiritIt = ObjectGUIDsByPvPTeamId.find(GetPvPTeamId());
    if (spiritIt != ObjectGUIDsByPvPTeamId.end() && !spiritIt->second.empty())
        if (Creature* spirit = Battle->GetCreature(*spiritIt->second.begin()))
            spirit->CastSpell(spirit, SPELL_BATTLEFIELD_SPIRIT_HEAL, true);

    for (ObjectGuid playerGuid : ResurrectionQueue)
    {
        Player* player = ObjectAccessor::FindPlayer(playerGuid);
        if (!player || player->IsAlive() || player->GetZoneId() != Battle->GetZoneId())
            continue;

        player->CastSpell(player, SPELL_BATTLEFIELD_RESURRECTION_VISUAL, true);
        player->ResurrectPlayer(1.0f);
        player->CastSpell(player, SPELL_BATTLEFIELD_SPIRIT_HEAL_MANA, true);
        player->SpawnCorpseBones(false);
    }

    ResurrectionQueue.clear();
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
