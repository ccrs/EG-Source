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

#ifndef TRINITY_BATTLEFIELD_EVENTS_H_
#define TRINITY_BATTLEFIELD_EVENTS_H_

#include "Define.h"
#include "Duration.h"
#include "ObjectGuid.h"
#include "Timer.h"

class Battlefield;

enum BattlefieldEventType : uint8
{
    BATTLEFIELD_EVENT_RAID_RELEASE = 0,
    BATTLEFIELD_EVENT_RAID_ADD,
    BATTLEFIELD_EVENT_RAID_REMOVE
};

class BattlefieldEventKey
{
    friend class BattlefieldEvent;

    BattlefieldEventKey() = default;
};

class TC_GAME_API BattlefieldEvent
{
public:
    explicit BattlefieldEvent(BattlefieldEventType type, Milliseconds delay = 0ms, ObjectGuid objectGuid = ObjectGuid::Empty) : _type(type), _objectGUID(objectGuid), _delay(delay) { }
    virtual ~BattlefieldEvent() = default;

    bool Execute(Battlefield& battlefield, uint32 diff);

    BattlefieldEventType GetType() const { return _type; }
    ObjectGuid GetObjectGUID() const { return _objectGUID; }

protected:
    static BattlefieldEventKey GetKey() { return {}; }
    virtual bool Handle(Battlefield& battlefield) = 0;

private:
    BattlefieldEventType _type;
    ObjectGuid _objectGUID;
    TimeTracker _delay;
};

template<BattlefieldEventType Type>
class BattlefieldTypedEvent final : public BattlefieldEvent
{
public:
    explicit BattlefieldTypedEvent(ObjectGuid objectGuid = ObjectGuid::Empty, Milliseconds delay = 0ms) : BattlefieldEvent(Type, delay, objectGuid) { }

protected:
    bool Handle(Battlefield& battlefield) override;
};

#define BATTLEFIELD_EVENT(Name, Type) \
    template<> TC_GAME_API bool BattlefieldTypedEvent<Type>::Handle(Battlefield& battlefield); \
    using Battlefield##Name##Event = BattlefieldTypedEvent<Type>

BATTLEFIELD_EVENT(RaidRelease, BATTLEFIELD_EVENT_RAID_RELEASE);
BATTLEFIELD_EVENT(RaidAdd, BATTLEFIELD_EVENT_RAID_ADD);
BATTLEFIELD_EVENT(RaidRemove, BATTLEFIELD_EVENT_RAID_REMOVE);

#endif
