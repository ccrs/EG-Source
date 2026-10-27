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

#include "BattlefieldEvents.h"
#include "Battlefield.h"
#include "ObjectAccessor.h"

bool BattlefieldEvent::Execute(Battlefield& battlefield, uint32 diff)
{
    _delay.Update(diff);
    if (!_delay.Passed())
        return false;

    return Handle(battlefield);
}

bool BattlefieldRaidReleaseEvent::Handle(Battlefield& battlefield)
{
    battlefield.ProcessRaidRelease(GetKey());
    return true;
}

bool BattlefieldRaidAddEvent::Handle(Battlefield& battlefield)
{
    if (Player* player = ObjectAccessor::FindPlayer(GetObjectGUID()))
        battlefield.ProcessRaidAdd(GetKey(), player);
    return true;
}

bool BattlefieldRaidRemoveEvent::Handle(Battlefield& battlefield)
{
    battlefield.ProcessRaidRemove(GetKey(), GetObjectGUID());
    return true;
}
