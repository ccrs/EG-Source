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

#ifndef LOCALTRANSPORTS_H
#define LOCALTRANSPORTS_H

#include "Transport.h"

class TC_GAME_API LocalTransport : public Transport
{
    public:
        LocalTransport();

        // Spawns the transport from its `gameobject` row and adds it to the map.
        static GameObject* CreateLocalTransportFromDB(ObjectGuid::LowType spawnId, Map* map);

        // Advances the animation according to the GoState and repositions the transport and its passengers.
        void Update(uint32 diff) override;
        void DelayedUpdate(uint32 /*diff*/) override { }
        // Places the transport, and every passenger, at the world position and facing for the given progress.
        void RelocateToProgress(uint32 progress);
        void UpdatePosition(float x, float y, float z, float o) override;
        // Moves every boarded passenger to its stored transport-relative offset.
        void UpdatePassengerPositions();

        // Boards a passenger at its current world position, recording where it sits on the transport.
        void BoardScriptedPassenger(WorldObject* passenger);

        // Current position along the animation, in milliseconds (0 .. period).
        uint32 GetPathProgress() const { return GetGOValue()->Transport.PathProgress; }
        void SetPathProgress(uint32 val) { m_goValue.Transport.PathProgress = val; }

        // Animation offset (ms) where GO_STATE_ACTIVE parks the transport - 0 means no stop (continuous loop).
        uint32 GetPauseTime() const { return GetUInt32Value(GAMEOBJECT_LEVEL); }
        // Full length of the animation in milliseconds.
        uint32 GetPeriod() const { return m_goValue.Transport.AnimationInfo ? m_goValue.Transport.AnimationInfo->TotalTime : GetPauseTime() + 2; }

        // Length the path-progress fraction is taken over: the full animation length, overriding the base.
        uint32 GetTransportPeriod() const override;
        uint32 GetTransportPathTimer() const override;

        bool IsTransportStopped() const override { return _stopped; }

        void SetNeedDoInitialRelocation(bool val) { _needDoInitialRelocation = val; }

    private:
        // Pending one-time placement at the starting phase, done on the first Update.
        bool _needDoInitialRelocation;
        // Whether the transport is currently parked at an endpoint (no movement this tick).
        bool _stopped = false;
        // Re-broadcast interval the animation phase is re-published.
        uint32 _phaseBroadcastTimer = 0;
};

#endif
