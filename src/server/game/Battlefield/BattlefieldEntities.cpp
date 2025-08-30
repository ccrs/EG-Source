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
#include "Object.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include <algorithm>

bool BattlefieldEntityInfo::ValidateObjectEntry(uint32 entry) const
{
    return std::any_of(ObjectEntriesByPvPTeamId.begin(), ObjectEntriesByPvPTeamId.end(), [&](std::unordered_map<PvPTeamId, std::vector<uint32 /*entry*/>>::value_type pair) -> bool
    {
        return std::find(pair.second.begin(), pair.second.end(), entry) != pair.second.end();
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
        if (Info.ObjectEntriesByPvPTeamId.contains(currentTeam))
        {
            std::vector<uint32> factionEntries = Info.ObjectEntriesByPvPTeamId.at(currentTeam);
            if (std::find(factionEntries.begin(), factionEntries.end(), object->GetEntry()) != factionEntries.end())
                ObjectGUIDsByPvPTeamId[currentTeam].insert(object->GetGUID());
        }
    }
}

void BattlefieldEntity::OnObjectRemove(WorldObject* object)
{
    for (PvPTeamId currentTeam : { PVP_TEAM_HORDE, PVP_TEAM_ALLIANCE, PVP_TEAM_NEUTRAL })
    {
        if (Info.ObjectEntriesByPvPTeamId.contains(currentTeam))
        {
            std::vector<uint32> factionEntries = Info.ObjectEntriesByPvPTeamId.at(currentTeam);
            if (std::find(factionEntries.begin(), factionEntries.end(), object->GetEntry()) != factionEntries.end())
                ObjectGUIDsByPvPTeamId[currentTeam].erase(object->GetGUID());
        }
    }
}

bool BattlefieldEntity::ValidateObjectGUID(ObjectGuid reference) const
{
    return std::any_of(ObjectGUIDsByPvPTeamId.begin(), ObjectGUIDsByPvPTeamId.end(), [&](std::unordered_map<PvPTeamId, GuidUnorderedSet>::value_type pair) -> bool
    {
        return std::find(pair.second.begin(), pair.second.end(), reference) != pair.second.end();
    });
}

BattlefieldBuilding::BattlefieldBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info) : BattlefieldEntity(battlefield, info.Info), BuildingType(info.Type), State(BATTLEFIELD_BUILDING_STATE_NEUTRAL_INTACT)
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

BattlefieldGraveyard::BattlefieldGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info) : BattlefieldEntity(battlefield, info.Info), Id(info.Id), WorldSafeLocsEntryId(info.WorldSafeLocsEntryId), TextId(info.TextId), State(BATTLEFIELD_GRAVEYARD_STATE_NEUTRAL)
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

    for (ObjectGuid playerGuid : ResurrectionQueue)
    {
        // Get player object from his guid
        Player* player = ObjectAccessor::FindPlayer(playerGuid);
        if (!player)
            continue;

        if (Creature* spirit = Battle->GetCreature(*ObjectGUIDsByPvPTeamId[GetPvPTeamId()].begin()))
            spirit->CastSpell(spirit, SPELL_BATTLEFIELD_SPIRIT_HEAL, true);

        // Resurrect player
        player->CastSpell(player, SPELL_BATTLEFIELD_RESURRECTION_VISUAL, true);
        player->ResurrectPlayer(1.0f);
        player->CastSpell(player, 6962, true);
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
