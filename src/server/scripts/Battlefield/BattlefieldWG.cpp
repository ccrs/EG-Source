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

#include "BattlefieldWG.h"
#include "Creature.h"
#include "GameObject.h"
#include "GameTime.h"
#include "Player.h"
#include <vector>

static std::vector<BattlefieldBuildingInfo> const wintergraspBuildingInfo =
{
    { WORLDSTATE_WINTERGRASP_FORTRESS_GATE,            GO_WINTERGRASP_FORTRESS_GATE,            BATTLEFIELD_BUILDING_TYPE_DOOR },
    { WORLDSTATE_WINTERGRASP_FORTRESS_VAULT_GATE,      GO_WINTERGRASP_FORTRESS_VAULT_GATE,      BATTLEFIELD_BUILDING_TYPE_DOOR_LAST },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_1,          GO_WINTERGRASP_FORTRESS_WALL_1,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_2,          GO_WINTERGRASP_FORTRESS_WALL_2,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_3,          GO_WINTERGRASP_FORTRESS_WALL_3,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_4,          GO_WINTERGRASP_FORTRESS_WALL_4,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_5,          GO_WINTERGRASP_FORTRESS_WALL_5,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_6,          GO_WINTERGRASP_FORTRESS_WALL_6,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_7,          GO_WINTERGRASP_FORTRESS_WALL_7,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_8,          GO_WINTERGRASP_FORTRESS_WALL_8,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_9,          GO_WINTERGRASP_FORTRESS_WALL_9,          BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_10,         GO_WINTERGRASP_FORTRESS_WALL_10,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_11,         GO_WINTERGRASP_FORTRESS_WALL_11,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_12,         GO_WINTERGRASP_FORTRESS_WALL_12,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_13,         GO_WINTERGRASP_FORTRESS_WALL_13,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_14,         GO_WINTERGRASP_FORTRESS_WALL_14,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_15,         GO_WINTERGRASP_FORTRESS_WALL_15,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_16,         GO_WINTERGRASP_FORTRESS_WALL_16,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_17,         GO_WINTERGRASP_FORTRESS_WALL_17,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_18,         GO_WINTERGRASP_FORTRESS_WALL_18,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_19,         GO_WINTERGRASP_FORTRESS_WALL_19,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_WALL_20,         GO_WINTERGRASP_FORTRESS_WALL_20,         BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_1, GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1, BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_2, GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2, BATTLEFIELD_BUILDING_TYPE_WALL },
    { WORLDSTATE_WINTERGRASP_FORTRESS_INTERIOR_WALL_3, GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3, BATTLEFIELD_BUILDING_TYPE_WALL }
};

static std::vector<BattlefieldGraveyardInfo> const wintergraspGraveyardInfo =
{
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_NE, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NE, GOSSIP_WINTERGRASP_GRAVEYARD_NE },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_NW, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_DEFENSE_NW, GOSSIP_WINTERGRASP_GRAVEYARD_NW },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_SE, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SE,         GOSSIP_WINTERGRASP_GRAVEYARD_SE },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_WORKSHOP_SW, WORLDSAFELOC_WINTERGRASP_SIEGE_FACTORY_SW,         GOSSIP_WINTERGRASP_GRAVEYARD_SW },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_KEEP,        WORLDSAFELOC_WINTERGRASP_FORTRESS_EAST,            GOSSIP_WINTERGRASP_GRAVEYARD_KEEP },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_HORDE,       WORLDSAFELOC_WINTERGRASP_HORDE_STARTING_AREA,      GOSSIP_WINTERGRASP_GRAVEYARD_HORDE },
    { 0, NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE, NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE, 0, GRAVEYARD_ALLIANCE,    WORLDSAFELOC_WINTERGRASP_ALLIANCE_STARTING_AREA,   GOSSIP_WINTERGRASP_GRAVEYARD_ALLIANCE }
};

BattlefieldWintergrasp::BattlefieldWintergrasp() : Battlefield(BATTLEFIELD_BATTLEID_WINTERGRASP, BATTLEFIELD_ZONEID_WINTERGRASP)
{
    for (auto itr = wintergraspBuildingInfo.begin(); itr != wintergraspBuildingInfo.end(); ++itr)
    {
        WintergraspBuildingPointer building = std::make_unique<WintergraspBuilding>(this, *itr);
        _buildings.emplace(itr->Info.GetNeutralObjectEntry(), std::move(building));
    }

    for (auto itr = wintergraspGraveyardInfo.begin(); itr != wintergraspGraveyardInfo.end(); ++itr)
    {
        WintergraspGraveyardPointer graveyard = std::make_unique<WintergraspGraveyard>(this, *itr);
        EmplaceGraveyard(itr->Id, std::move(graveyard));
    }
}

BattlefieldWintergrasp::~BattlefieldWintergrasp()
{
}

bool BattlefieldWintergrasp::SetupBattlefield()
{
    SetMapId(MAPID_WINTERGRASP);
    _enabled = /*sWorld->getBoolConfig(CONFIG_WINTERGRASP_ENABLE)*/false;
    return true;
}

void BattlefieldWintergrasp::OnCreatureCreate(Creature* object)
{
    switch (object->GetEntry())
    {
        case NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE:
        case NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE:
            if (uint8 graveyardId = GetWintergraspGraveyardId(object))
                if (BattlefieldGraveyardPointer& graveyard = GetGraveyard(graveyardId))
                    graveyard->OnObjectCreate(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnCreatureRemove(Creature* object)
{
    switch (object->GetEntry())
    {
        case NPC_WINTERGRASP_TAUNKA_SPIRIT_GUIDE:
        case NPC_WINTERGRASP_DWARVEN_SPIRIT_GUIDE:
            if (uint8 graveyardId = GetWintergraspGraveyardId(object))
                if (BattlefieldGraveyardPointer& graveyard = GetGraveyard(graveyardId))
                    graveyard->OnObjectRemove(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnGameObjectCreate(GameObject* object)
{
    switch (object->GetEntry())
    {
        case GO_WINTERGRASP_FORTRESS_GATE:
        case GO_WINTERGRASP_FORTRESS_VAULT_GATE:
        case GO_WINTERGRASP_FORTRESS_WALL_1:
        case GO_WINTERGRASP_FORTRESS_WALL_2:
        case GO_WINTERGRASP_FORTRESS_WALL_3:
        case GO_WINTERGRASP_FORTRESS_WALL_4:
        case GO_WINTERGRASP_FORTRESS_WALL_5:
        case GO_WINTERGRASP_FORTRESS_WALL_6:
        case GO_WINTERGRASP_FORTRESS_WALL_7:
        case GO_WINTERGRASP_FORTRESS_WALL_8:
        case GO_WINTERGRASP_FORTRESS_WALL_9:
        case GO_WINTERGRASP_FORTRESS_WALL_10:
        case GO_WINTERGRASP_FORTRESS_WALL_11:
        case GO_WINTERGRASP_FORTRESS_WALL_12:
        case GO_WINTERGRASP_FORTRESS_WALL_13:
        case GO_WINTERGRASP_FORTRESS_WALL_14:
        case GO_WINTERGRASP_FORTRESS_WALL_15:
        case GO_WINTERGRASP_FORTRESS_WALL_16:
        case GO_WINTERGRASP_FORTRESS_WALL_17:
        case GO_WINTERGRASP_FORTRESS_WALL_18:
        case GO_WINTERGRASP_FORTRESS_WALL_19:
        case GO_WINTERGRASP_FORTRESS_WALL_20:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3:
            if (WintergraspBuildingPointer& building = _buildings[object->GetEntry()])
                building->OnObjectCreate(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::OnGameObjectRemove(GameObject* object)
{
    switch (object->GetEntry())
    {
        case GO_WINTERGRASP_FORTRESS_GATE:
        case GO_WINTERGRASP_FORTRESS_VAULT_GATE:
        case GO_WINTERGRASP_FORTRESS_WALL_1:
        case GO_WINTERGRASP_FORTRESS_WALL_2:
        case GO_WINTERGRASP_FORTRESS_WALL_3:
        case GO_WINTERGRASP_FORTRESS_WALL_4:
        case GO_WINTERGRASP_FORTRESS_WALL_5:
        case GO_WINTERGRASP_FORTRESS_WALL_6:
        case GO_WINTERGRASP_FORTRESS_WALL_7:
        case GO_WINTERGRASP_FORTRESS_WALL_8:
        case GO_WINTERGRASP_FORTRESS_WALL_9:
        case GO_WINTERGRASP_FORTRESS_WALL_10:
        case GO_WINTERGRASP_FORTRESS_WALL_11:
        case GO_WINTERGRASP_FORTRESS_WALL_12:
        case GO_WINTERGRASP_FORTRESS_WALL_13:
        case GO_WINTERGRASP_FORTRESS_WALL_14:
        case GO_WINTERGRASP_FORTRESS_WALL_15:
        case GO_WINTERGRASP_FORTRESS_WALL_16:
        case GO_WINTERGRASP_FORTRESS_WALL_17:
        case GO_WINTERGRASP_FORTRESS_WALL_18:
        case GO_WINTERGRASP_FORTRESS_WALL_19:
        case GO_WINTERGRASP_FORTRESS_WALL_20:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_1:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_2:
        case GO_WINTERGRASP_FORTRESS_INTERIOR_WALL_3:
            if (WintergraspBuildingPointer& building = _buildings[object->GetEntry()])
                building->OnObjectRemove(object);
            break;
        default:
            break;
    }
}

void BattlefieldWintergrasp::SendGlobalWorldStates(Player const* player) const
{
    if (!IsEnabled())
        return;

    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_SHOW_NOWAR_TIMER, IsWarTime() ? 0 : 1);

    uint32 timer = 0;
    if (IsWarTime())
        timer = GetTimer() / 1000;

    player->SendUpdateWorldState(WORLDSTATE_WINTERGRASP_TIME_TO_NEXT_BATTLE, GameTime::GetGameTime() + timer);
}

bool BattlefieldWintergrasp::IsSpellAreaAllowed(uint32 spellId, Player const* player, uint32 /*newArea*/) const
{
    if (!player)
        return false;

    switch (spellId)
    {
        case SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA:
            if (IsFlyingMountAllowed() || (!player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) && !player->HasAuraType(SPELL_AURA_FLY)))
                return false;
            break;
        case SPELL_WINTERGRASP_ESSENCE_OF_WINTERGRASP:
            return IsEnabled() && (player->GetTeamId() == GetControllingTeamId()) && !IsWarTime();
        case SPELL_WINTERGRASP_ESSENCE_OF_WINTERGRASP_NORTHREND:
            return false;
        case SPELL_WINTERGRASP_BATTLEGROUND_DAMPENING:
            return IsEnabled() && IsWarTime();
        case SPELL_WINTERGRASP_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT:
        case SPELL_WINTERGRASP_HORDE_CONTROLS_FACTORY_PHASE_SHIFT:
            return false;
        case SPELL_WINTERGRASP_ALLIANCE_CONTROL_PHASE_SHIFT:
        case SPELL_WINTERGRASP_HORDE_CONTROL_PHASE_SHIFT:
            return player->GetTeamId() == GetControllingTeamId();
        default:
            break;
    }

    return true;
}

uint8 BattlefieldWintergrasp::GetWintergraspGraveyardId(Creature* creature) const
{
    switch (creature->GetSpawnId())
    {
        case 88315: // Horde base spirit
            return GRAVEYARD_HORDE;
        case 88321: // Alliance base spirit
            return GRAVEYARD_ALLIANCE;
        case 88319: // Broken Temple graveyard areaId != Broken Temple workshop areaId
        case 88313:
            return GRAVEYARD_WORKSHOP_NW;
        default:
            switch (creature->GetAreaId())
            {
                case AREA_WINTERGRASP_FORTRESS:
                    return GRAVEYARD_KEEP;
                case AREA_THE_SUNKEN_RING:
                    return GRAVEYARD_WORKSHOP_NE;
                case AREA_THE_BROKEN_TEMPLE:
                    return GRAVEYARD_WORKSHOP_NW;
                case AREA_WESTPARK_WORKSHOP:
                    return GRAVEYARD_WORKSHOP_SW;
                case AREA_EASTPARK_WORKSHOP:
                    return GRAVEYARD_WORKSHOP_SE;
                default:
                    break;
            }
            break;
    }

    return 0;
}

WintergraspBuilding::WintergraspBuilding(Battlefield* battlefield, BattlefieldBuildingInfo const info) : BattlefieldBuilding(battlefield, info)
{
}

void WintergraspBuilding::OnObjectCreate(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldBuilding::OnObjectCreate(object);
}

void WintergraspBuilding::OnObjectRemove(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldBuilding::OnObjectRemove(object);
}

WintergraspCapturePoint::WintergraspCapturePoint(Battlefield* battlefield, BattlefieldEntityInfo const info) : BattlefieldCapturePoint(battlefield, info)
{
}

void WintergraspCapturePoint::OnObjectCreate(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldCapturePoint::OnObjectCreate(object);
}

void WintergraspCapturePoint::OnObjectRemove(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_GAMEOBJECT)
        return;

    BattlefieldCapturePoint::OnObjectRemove(object);
}

WintergraspGraveyard::WintergraspGraveyard(Battlefield* battlefield, BattlefieldGraveyardInfo const info) : BattlefieldGraveyard(battlefield, info)
{
}

void WintergraspGraveyard::OnObjectCreate(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_UNIT)
        return;

    BattlefieldGraveyard::OnObjectCreate(object);
}

void WintergraspGraveyard::OnObjectRemove(WorldObject* object)
{
    if (object->GetTypeId() != TYPEID_UNIT)
        return;

    BattlefieldGraveyard::OnObjectRemove(object);
}

class BattlefieldWintergraspScript : public BattlefieldScript
{
public:
    BattlefieldWintergraspScript() : BattlefieldScript("battlefield_wg") {}

    Battlefield* GetBattlefield() const override
    {
        return new BattlefieldWintergrasp();
    }
};

void AddSCBattlefieldWintergraspScript() {
    new BattlefieldWintergraspScript();
}
