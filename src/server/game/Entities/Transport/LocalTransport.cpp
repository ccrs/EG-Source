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

#include "LocalTransport.h"
#include "Creature.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "Vehicle.h"
#include <G3D/Quat.h>
#include <G3D/Vector3.h>

LocalTransport::LocalTransport() : Transport(), _needDoInitialRelocation(false)
{
}

void LocalTransport::Update(uint32 diff)
{
    // Skip Transport::Update (type-15 keyframe path); type-11 drives its own animation below.
    GameObject::Update(diff);

    if (!IsInWorld())
        return;

    if (!m_goValue.Transport.AnimationInfo)
        return;

    if (_needDoInitialRelocation)
    {
        _needDoInitialRelocation = false;
        RelocateToProgress(GetPathProgress());
    }

    if (GetPauseTime())
    {
        if (GetGoState() == GO_STATE_READY)
        {
            if (GetPathProgress() == 0) // waiting at its destination for state change, do nothing
                return;

            if (GetPathProgress() < GetPauseTime()) // GOState changed before previous state was reached, move to new destination immediately
                SetPathProgress(0);
            else if (GetPathProgress() + diff < GetPeriod())
                SetPathProgress(GetPathProgress() + diff);
            else
                SetPathProgress(0);
        }
        else
        {
            if (GetPathProgress() == GetPauseTime()) // waiting at its destination for state change, do nothing
                return;

            if (GetPathProgress() > GetPauseTime()) // GOState changed before previous state was reached, move to new destination immediately
                SetPathProgress(GetPauseTime());
            else if (GetPathProgress() + diff < GetPauseTime())
                SetPathProgress(GetPathProgress() + diff);
            else
                SetPathProgress(GetPauseTime());
        }
    }
    else
    {
        SetPathProgress(GetPathProgress() + diff);
        if (GetPathProgress() >= GetPeriod())
            SetPathProgress(GetPathProgress() % GetPeriod());
    }

    RelocateToProgress(GetPathProgress());
}

void LocalTransport::RelocateToProgress(uint32 progress)
{
    if (!m_goValue.Transport.AnimationInfo || !m_goValue.Transport.AnimationInfo->TotalTime)
        return;

    uint32 timer = progress % m_goValue.Transport.AnimationInfo->TotalTime;

    TransportAnimationEntry const* node = m_goValue.Transport.AnimationInfo->GetAnimNode(timer);
    if (!node)
        return;

    if (m_goValue.Transport.CurrentSeg == node->TimeIndex)
        return;
    m_goValue.Transport.CurrentSeg = node->TimeIndex;

    G3D::Quat rotation;
    if (TransportRotationEntry const* rot = m_goValue.Transport.AnimationInfo->GetAnimRotation(timer))
        rotation = G3D::Quat(rot->X, rot->Y, rot->Z, rot->W);

    G3D::Vector3 pos = rotation.toRotationMatrix()
                     * G3D::Matrix3::fromEulerAnglesZYX(GetStationaryO(), 0.0f, 0.0f)
                     * G3D::Vector3(node->Pos.X, node->Pos.Y, node->Pos.Z);

    pos += G3D::Vector3(GetStationaryX(), GetStationaryY(), GetStationaryZ());

    // check if position is valid
    if (!Trinity::IsValidMapCoord(pos.x, pos.y, pos.z))
        return;

    UpdatePosition(pos.x, pos.y, pos.z, GetStationaryO());
}

void LocalTransport::UpdatePosition(float x, float y, float z, float o)
{
    if (!GetMap()->IsGridLoaded(x, y))
        GetMap()->LoadGrid(x, y);

    Relocate(x, y, z, o);
    UpdateModelPosition();

    UpdatePassengerPositions();
}

void LocalTransport::UpdatePassengerPositions()
{
    for (PassengerSet::iterator itr = _passengers.begin(); itr != _passengers.end(); ++itr)
    {
        WorldObject* passenger = *itr;
        // transport teleported but passenger not yet (can happen for players)
        if (passenger->GetMap() != GetMap())
            continue;

        // if passenger is on vehicle we have to assume the vehicle is also on transport
        // and its the vehicle that will be updating its passengers
        if (Unit* unit = passenger->ToUnit())
            if (unit->GetVehicle())
                continue;

        // Do not use Unit::UpdatePosition here, we don't want to remove auras
        // as if regular movement occurred
        float x, y, z, o;
        passenger->m_movementInfo.transport.pos.GetPosition(x, y, z, o);
        CalculatePassengerPosition(x, y, z, &o);

        // check if position is valid
        if (!Trinity::IsValidMapCoord(x, y, z))
            continue;

        switch (passenger->GetTypeId())
        {
            case TYPEID_UNIT:
            {
                Creature* creature = passenger->ToCreature();
                GetMap()->CreatureRelocation(creature, x, y, z, o, false);
                creature->GetTransportHomePosition(x, y, z, o);
                CalculatePassengerPosition(x, y, z, &o);
                creature->SetHomePosition(x, y, z, o);
                break;
            }
            case TYPEID_PLAYER:
                // relocate only passengers in world and skip any player that might be still logging in/teleporting
                if (passenger->IsInWorld() && !passenger->ToPlayer()->IsBeingTeleported())
                {
                    GetMap()->PlayerRelocation(passenger->ToPlayer(), x, y, z, o);
                    passenger->ToPlayer()->SetFallInformation(0, passenger->GetPositionZ());
                }
                break;
            case TYPEID_GAMEOBJECT:
                GetMap()->GameObjectRelocation(passenger->ToGameObject(), x, y, z, o, false);
                passenger->ToGameObject()->RelocateStationaryPosition(x, y, z, o);
                break;
            case TYPEID_DYNAMICOBJECT:
                GetMap()->DynamicObjectRelocation(passenger->ToDynObject(), x, y, z, o);
                break;
            default:
                break;
        }

        if (Unit* unit = passenger->ToUnit())
            if (Vehicle* vehicle = unit->GetVehicleKit())
                vehicle->RelocatePassengers();
    }
}

void LocalTransport::AddPassenger(WorldObject* passenger)
{
    if (!IsInWorld())
        return;

    if (_passengers.insert(passenger).second)
    {
        float x, y, z, o;
        passenger->GetPosition(x, y, z, o);
        CalculatePassengerOffset(x, y, z, &o);

        passenger->SetTransport(this);
        passenger->m_movementInfo.AddMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
        passenger->m_movementInfo.transport.guid = GetGUID();
        passenger->m_movementInfo.transport.pos.Relocate(x, y, z, o);

        // keep the home anchored to the moving transport so passenger evade/leashing tracks it
        if (Creature* creature = passenger->ToCreature())
            creature->SetTransportHomePosition(creature->m_movementInfo.transport.pos);

        TC_LOG_DEBUG("entities.transport", "Object {} boarded local transport {}.", passenger->GetName(), GetName());
    }
}

void LocalTransport::RemovePassenger(WorldObject* passenger)
{
    if (_passengers.erase(passenger))
    {
        passenger->SetTransport(nullptr);
        passenger->m_movementInfo.RemoveMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
        passenger->m_movementInfo.transport.Reset();

        if (Player* plr = passenger->ToPlayer())
            plr->SetFallInformation(0, plr->GetPositionZ());

        TC_LOG_DEBUG("entities.transport", "Object {} removed from local transport {}.", passenger->GetName(), GetName());
    }
}
