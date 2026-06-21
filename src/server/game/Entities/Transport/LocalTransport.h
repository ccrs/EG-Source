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

        void Update(uint32 diff) override;
        void RelocateToProgress(uint32 progress);
        void UpdatePosition(float x, float y, float z, float o) override;
        void UpdatePassengerPositions();

        void AddPassenger(WorldObject* passenger) override;
        void RemovePassenger(WorldObject* passenger) override;

        uint32 GetPathProgress() const { return GetGOValue()->Transport.PathProgress; }
        void SetPathProgress(uint32 val) { m_goValue.Transport.PathProgress = val; }

        uint32 GetPauseTime() const { return GetUInt32Value(GAMEOBJECT_LEVEL); }
        void SetPauseTime(uint32 val) { SetUInt32Value(GAMEOBJECT_LEVEL, val); }
        uint32 GetPeriod() const { return m_goValue.Transport.AnimationInfo ? m_goValue.Transport.AnimationInfo->TotalTime : GetPauseTime() + 2; }

        void SetNeedDoInitialRelocation(bool val) { _needDoInitialRelocation = val; }

    private:
        bool _needDoInitialRelocation;
};

#endif
