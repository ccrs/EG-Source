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

class Battlefield;

enum BattlefieldEventType : uint8
{
    BATTLEFIELD_EVENT_RAID_RELEASE = 0,
    BATTLEFIELD_EVENT_RAID_ADD,
    BATTLEFIELD_EVENT_RAID_REMOVE,
    BATTLEFIELD_EVENT_PHASE_ADVANCE,
    BATTLEFIELD_EVENT_GRAVEYARD_RESURRECT,
    BATTLEFIELD_EVENT_ZONE_REFRESH
};

enum BattlefieldEventQueuePolicy : uint8
{
    BATTLEFIELD_EVENT_QUEUE_MULTIPLE = 0,
    BATTLEFIELD_EVENT_QUEUE_REPLACE
};

class BattlefieldEventKey
{
    friend class BattlefieldEvent;

    BattlefieldEventKey() = default;
};

class TC_GAME_API BattlefieldEvent
{
public:
    explicit BattlefieldEvent(BattlefieldEventType type, BattlefieldEventQueuePolicy queuePolicy, Milliseconds delay = 0ms, ObjectGuid objectGuid = ObjectGuid::Empty) : _type(type), _queuePolicy(queuePolicy), _objectGUID(objectGuid), _delay(delay), _cancelled(false) { }
    virtual ~BattlefieldEvent() = default;

    void Execute(Battlefield& battlefield);
    void Cancel() { _cancelled = true; }

    BattlefieldEventType GetType() const { return _type; }
    BattlefieldEventQueuePolicy GetQueuePolicy() const { return _queuePolicy; }
    ObjectGuid GetObjectGUID() const { return _objectGUID; }
    Milliseconds GetDelay() const { return _delay; }
    bool IsCancelled() const { return _cancelled; }

protected:
    static BattlefieldEventKey GetKey() { return {}; }
    virtual void Handle(Battlefield& battlefield) = 0;

private:
    BattlefieldEventType _type;
    BattlefieldEventQueuePolicy _queuePolicy;
    ObjectGuid _objectGUID;
    Milliseconds _delay;
    bool _cancelled;
};

template<BattlefieldEventType Type>
struct BattlefieldEventTraits;

template<BattlefieldEventType Type>
class BattlefieldTypedEvent final : public BattlefieldEvent
{
public:
    explicit BattlefieldTypedEvent(ObjectGuid objectGuid = ObjectGuid::Empty, Milliseconds delay = 0ms) : BattlefieldEvent(Type, BattlefieldEventTraits<Type>::QueuePolicy, delay, objectGuid) { }

protected:
    void Handle(Battlefield& battlefield) override;
};

#define BATTLEFIELD_EVENT(Name, Type, Policy) \
    template<> struct BattlefieldEventTraits<Type> { static constexpr BattlefieldEventQueuePolicy QueuePolicy = Policy; }; \
    template<> TC_GAME_API void BattlefieldTypedEvent<Type>::Handle(Battlefield& battlefield); \
    using Battlefield##Name##Event = BattlefieldTypedEvent<Type>

BATTLEFIELD_EVENT(RaidRelease, BATTLEFIELD_EVENT_RAID_RELEASE, BATTLEFIELD_EVENT_QUEUE_MULTIPLE);
BATTLEFIELD_EVENT(RaidAdd, BATTLEFIELD_EVENT_RAID_ADD, BATTLEFIELD_EVENT_QUEUE_MULTIPLE);
BATTLEFIELD_EVENT(RaidRemove, BATTLEFIELD_EVENT_RAID_REMOVE, BATTLEFIELD_EVENT_QUEUE_MULTIPLE);
BATTLEFIELD_EVENT(PhaseAdvance, BATTLEFIELD_EVENT_PHASE_ADVANCE, BATTLEFIELD_EVENT_QUEUE_REPLACE);
BATTLEFIELD_EVENT(GraveyardResurrect, BATTLEFIELD_EVENT_GRAVEYARD_RESURRECT, BATTLEFIELD_EVENT_QUEUE_REPLACE);
BATTLEFIELD_EVENT(ZoneRefresh, BATTLEFIELD_EVENT_ZONE_REFRESH, BATTLEFIELD_EVENT_QUEUE_REPLACE);

#endif
