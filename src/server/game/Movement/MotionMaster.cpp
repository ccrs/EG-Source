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

#include "MotionMaster.h"
#include "AbstractFollower.h"
#include "Creature.h"
#include "CreatureAISelector.h"
#include "Containers.h"
#include "DBCStores.h"
#include "Errors.h"
#include "G3DPosition.hpp"
#include "Log.h"
#include "Map.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "PathGenerator.h"
#include "Player.h"
#include "ScriptSystem.h"
#include "Unit.h"
#include "WaypointDefines.h"
#include <algorithm>
#include <iterator>

#include "ChaseMovementGenerator.h"
#include "ConfusedMovementGenerator.h"
#include "FleeingMovementGenerator.h"
#include "FlightPathMovementGenerator.h"
#include "FollowMovementGenerator.h"
#include "FormationMovementGenerator.h"
#include "GenericMovementGenerator.h"
#include "HomeMovementGenerator.h"
#include "IdleMovementGenerator.h"
#include "PointMovementGenerator.h"
#include "RandomMovementGenerator.h"
#include "SplineChainMovementGenerator.h"
#include "WaypointMovementGenerator.h"

inline MovementGenerator* GetIdleMovementGenerator()
{
    return sMovementGeneratorRegistry->GetRegistryItem(IDLE_MOTION_TYPE)->Create();
}

inline bool IsStatic(MovementGenerator* movement)
{
    return (movement == GetIdleMovementGenerator());
}

inline void MovementGeneratorPointerDeleter(MovementGenerator* a)
{
    if (a != nullptr && !IsStatic(a))
        delete a;
}

void MovementGeneratorDeleter::operator()(MovementGenerator* a)
{
    MovementGeneratorPointerDeleter(a);
}

MovementGeneratorInformation::MovementGeneratorInformation(uint8 priority, uint8 mode, MovementGeneratorType type, ObjectGuid targetGUID, std::string const& targetName) : Priority(priority), Mode(mode), Type(type), TargetGUID(targetGUID), TargetName(targetName) { }

MotionMaster::MotionMaster(Unit* unit) : _owner(unit), _defaultGenerator(nullptr), _flags(MOTIONMASTER_FLAG_INITIALIZATION_PENDING) { }

MotionMaster::~MotionMaster()
{
    _delayedActions.clear();

    for (MotionMasterContainer::value_type& value : _generators)
    {
        MotionMasterContainerList& movements = value.second;
        for (MovementGenerator* movement : movements)
            MovementGeneratorPointerDeleter(movement);
    }
}

void MotionMaster::Initialize()
{
    if (HasFlag(MOTIONMASTER_FLAG_INITIALIZATION_PENDING))
        return;

    if (HasFlag(MOTIONMASTER_FLAG_UPDATE))
    {
        DelayedActionDefine action = [this]()
        {
            Initialize();
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_INITIALIZE);
        return;
    }

    _DirectInitialize();
}

void MotionMaster::InitializeDefault()
{
    Add(FactorySelector::SelectMovementGenerator(_owner), MOTION_SLOT_DEFAULT);
}

void MotionMaster::AddToWorld()
{
    if (!HasFlag(MOTIONMASTER_FLAG_INITIALIZATION_PENDING))
        return;

    AddFlag(MOTIONMASTER_FLAG_INITIALIZING);
    RemoveFlag(MOTIONMASTER_FLAG_INITIALIZATION_PENDING);

    _DirectInitialize();
    _ResolveDelayedActions();

    RemoveFlag(MOTIONMASTER_FLAG_INITIALIZING);
}

bool MotionMaster::Empty() const
{
    return !_defaultGenerator && _generators.empty();
}

uint32 MotionMaster::Size() const
{
    return (_defaultGenerator ? 1 : 0) + uint32(_generators.size());
}

std::vector<MovementGeneratorInformation> MotionMaster::GetMovementGeneratorsInformation() const
{
    std::vector<MovementGeneratorInformation> list;

    if (_defaultGenerator)
        list.emplace_back(_defaultGenerator->Priority, _defaultGenerator->Mode, _defaultGenerator->GetMovementGeneratorType(), ObjectGuid::Empty, std::string());

    for (MotionMasterContainer::value_type const& value : _generators)
    {
        MotionMasterContainerList const& movements = value.second;
        for (MovementGenerator const* movement : movements)
        {
            MovementGeneratorType const type = movement->GetMovementGeneratorType();
            switch (type)
            {
                case CHASE_MOTION_TYPE:
                case FOLLOW_MOTION_TYPE:
                    if (AbstractFollower const* followInformation = dynamic_cast<AbstractFollower const*>(movement))
                    {
                        if (Unit* target = followInformation->GetTarget())
                            list.emplace_back(movement->Priority, movement->Mode, type, target->GetGUID(), target->GetName());
                        else
                            list.emplace_back(movement->Priority, movement->Mode, type, ObjectGuid::Empty, std::string());
                    }
                    else
                        list.emplace_back(movement->Priority, movement->Mode, type, ObjectGuid::Empty, std::string());
                    break;
                default:
                    list.emplace_back(movement->Priority, movement->Mode, type, ObjectGuid::Empty, std::string());
                    break;
            }
        }
    }

    return list;
}

MovementSlot MotionMaster::GetCurrentSlot() const
{
    if (!_generators.empty())
        return MOTION_SLOT_ACTIVE;

    if (_defaultGenerator)
        return MOTION_SLOT_DEFAULT;

    return MAX_MOTION_SLOT;
}

MovementGenerator* MotionMaster::GetCurrentMovementGenerator() const
{
    if (!_generators.empty())
        return _generators.begin()->second.front();

    if (_defaultGenerator)
        return _defaultGenerator.get();

    return nullptr;
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType() const
{
    if (Empty())
        return MAX_MOTION_TYPE;

    MovementGenerator const* movement = GetCurrentMovementGenerator();
    if (!movement)
        return MAX_MOTION_TYPE;

    return movement->GetMovementGeneratorType();
}

MovementGeneratorPriority MotionMaster::GetCurrentMovementGeneratorPriority() const
{
    if (Empty())
        return MOTION_PRIORITY_NONE;

    MovementGenerator const* movement = GetCurrentMovementGenerator();
    if (!movement)
        return MOTION_PRIORITY_NONE;

    return MovementGeneratorPriority(movement->Priority);
}

MovementGeneratorType MotionMaster::GetCurrentMovementGeneratorType(MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return MAX_MOTION_TYPE;

    if (slot == MOTION_SLOT_ACTIVE && !_generators.empty())
        return _generators.begin()->second.front()->GetMovementGeneratorType();

    if (slot == MOTION_SLOT_DEFAULT && _defaultGenerator)
        return _defaultGenerator->GetMovementGeneratorType();

    return MAX_MOTION_TYPE;
}

MovementGenerator* MotionMaster::GetCurrentMovementGenerator(MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return nullptr;

    if (slot == MOTION_SLOT_ACTIVE && !_generators.empty())
        return _generators.begin()->second.front();

    if (slot == MOTION_SLOT_DEFAULT && _defaultGenerator)
        return _defaultGenerator.get();

    return nullptr;
}

MovementGenerator* MotionMaster::GetMovementGenerator(std::function<bool(MovementGenerator const*)> const& filter, MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return nullptr;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator && filter(_defaultGenerator.get()))
                return _defaultGenerator.get();
            break;
        case MOTION_SLOT_ACTIVE:
            if (!_generators.empty())
            {
                for (MotionMasterContainer::value_type const& value : _generators)
                {
                    MotionMasterContainerList const& movements = value.second;
                    auto listItr = std::find_if(movements.begin(), movements.end(), std::ref(filter));
                    if (listItr != movements.end())
                        return *listItr;
                }
            }
            break;
        default:
            break;
    }

    return nullptr;
}

bool MotionMaster::HasMovementGenerator(std::function<bool(MovementGenerator const*)> const& filter, MovementSlot slot) const
{
    if (Empty() || IsInvalidMovementSlot(slot))
        return false;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            return _defaultGenerator && filter(_defaultGenerator.get());
            break;
        case MOTION_SLOT_ACTIVE:
            if (!_generators.empty())
            {
                for (MotionMasterContainer::value_type const& value : _generators)
                {
                    MotionMasterContainerList const& movements = value.second;
                    auto listItr = std::find_if(movements.begin(), movements.end(), std::ref(filter));
                    if (listItr != movements.end())
                        return true;
                }
            }
            break;
        default:
            break;
    }

    return false;
}

void MotionMaster::Update(uint32 diff)
{
    if (!_owner)
        return;

    if (HasFlag(MOTIONMASTER_FLAG_INITIALIZATION_PENDING | MOTIONMASTER_FLAG_INITIALIZING))
        return;

    ASSERT(!Empty(), "MotionMaster:Update: update called without Initializing! (%s)", _owner->GetGUID().ToString().c_str());

    AddFlag(MOTIONMASTER_FLAG_UPDATE);

    InitializationState initializationState = InitializationState::AlreadyInitialized;

    MovementGenerator* top = GetCurrentMovementGenerator();
    if (HasFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING) && IsStatic(top))
    {
        RemoveFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING);
        if (!HasFlag(MOTIONMASTER_FLAG_STATIC_PREVENT_INITIALIZATION))
            initializationState = top->Initialize(_owner) ? InitializationState::Success : InitializationState::No;
        else
            RemoveFlag(MOTIONMASTER_FLAG_STATIC_PREVENT_INITIALIZATION);
    }
    if (top->HasFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING))
        initializationState = top->Initialize(_owner) ? InitializationState::Success : InitializationState::No;
    if (top->HasFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED))
        initializationState = top->Reset(_owner) ? InitializationState::Success : InitializationState::No;

    ASSERT(!top->HasFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING | MOVEMENTGENERATOR_FLAG_DEACTIVATED), "MotionMaster:Update: update called on an uninitialized top! (%s) (type: %u, flags: %u)", _owner->GetGUID().ToString().c_str(), top->GetMovementGeneratorType(), top->Flags);

    bool popMovement = false;
    uint32 updateDiff = initializationState == InitializationState::AlreadyInitialized ? diff : 0;
    if (initializationState == InitializationState::No || initializationState == InitializationState::AlreadyInitialized)
        popMovement = !top->Update(_owner, updateDiff);

    if (popMovement)
    {
        ASSERT(top == GetCurrentMovementGenerator(), "MotionMaster::Update: top was modified while updating! (%s)", _owner->GetGUID().ToString().c_str());

        // Since all the actions that modify any slot are delayed, this movement is guaranteed to be top
        _Pop(true, true); // Natural, and only, call to MovementInform
    }

    RemoveFlag(MOTIONMASTER_FLAG_UPDATE);

    _ResolveDelayedActions();
}

void MotionMaster::Add(MovementGenerator* movement, MovementSlot slot/* = MOTION_SLOT_ACTIVE*/)
{
    if (!movement)
        return;

    if (IsInvalidMovementSlot(slot))
    {
        delete movement;
        return;
    }

    if (movement->HasFlag(MOVEMENTGENERATOR_FLAG_IMMEDIATE) && movement->HasFlag(MOVEMENTGENERATOR_FLAG_INITIALIZATION_PENDING))
    {
        bool wouldBecomeTop = _generators.lower_bound({ movement->Priority, movement->Mode }) == _generators.begin();
        if (!wouldBecomeTop || !movement->Initialize(_owner))
        {
            delete movement;
            return;
        }
    }

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, movement, slot]()
        {
            Add(movement, slot);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_ADD);
    }
    else
        _DirectAdd(movement, slot);
}

void MotionMaster::Remove(MovementGenerator* movement, MovementSlot slot/* = MOTION_SLOT_ACTIVE*/)
{
    if (!movement || IsInvalidMovementSlot(slot))
        return;

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, movement, slot]()
        {
            Remove(movement, slot);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_REMOVE);
        return;
    }

    if (Empty())
        return;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator && _defaultGenerator.get() == movement)
                _DirectClearDefault();
            break;
        case MOTION_SLOT_ACTIVE:
            if (!_generators.empty())
            {
                auto itr = _generators.find({ movement->Priority, movement->Mode });
                if (itr != _generators.end())
                {
                    MotionMasterContainerList& movements = itr->second;
                    auto listItr = std::find(movements.begin(), movements.end(), movement);
                    if (listItr != movements.end())
                        _Remove(itr, listItr, GetCurrentMovementGenerator() == *listItr, false);
                }
            }
            break;
        default:
            break;
    }
}

void MotionMaster::Remove(MovementGeneratorType type, MovementSlot slot/* = MOTION_SLOT_ACTIVE*/, MovementGeneratorPriority priority/* = MOTION_PRIORITY_NORMAL*/, MovementGeneratorMode mode/* = MOTION_MODE_DEFAULT*/)
{
    if (IsInvalidMovementGeneratorType(type) || IsInvalidMovementSlot(slot))
        return;

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, type, slot, priority, mode]()
        {
            Remove(type, slot, priority, mode);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_REMOVE_TYPE);
        return;
    }

    if (Empty())
        return;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator && _defaultGenerator->GetMovementGeneratorType() == type)
                _DirectClearDefault();
            break;
        case MOTION_SLOT_ACTIVE:
            if (!_generators.empty())
            {
                auto itr = _generators.find({ priority, mode });
                if (itr != _generators.end())
                {
                    MotionMasterContainerList& movements = itr->second;
                    auto listItr = std::find_if(movements.begin(), movements.end(), [type](MotionMasterContainerList::value_type const a) -> bool
                    {
                        return a->GetMovementGeneratorType() == type;
                    });

                    if (listItr != movements.end())
                        _Remove(itr, listItr, GetCurrentMovementGenerator() == *listItr, false);
                }
            }
            break;
        default:
            break;
    }
}

void MotionMaster::Clear()
{
    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this]()
        {
            Clear();
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_CLEAR);
        return;
    }

    if (!Empty())
        _DirectClear();
}

void MotionMaster::Clear(MovementSlot slot)
{
    if (IsInvalidMovementSlot(slot))
        return;

    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, slot]()
        {
            Clear(slot);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_CLEAR_SLOT);
        return;
    }

    if (Empty())
        return;

    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            _DirectClearDefault();
            break;
        case MOTION_SLOT_ACTIVE:
            _DirectClear();
            break;
        default:
            break;
    }
}

void MotionMaster::Clear(MovementGeneratorMode mode)
{
    if (Empty())
        return;

    std::function<bool(MovementGenerator const*)> criteria = [mode](MovementGenerator const* a) -> bool
    {
        return a->Mode == mode;
    };
    Clear(criteria);
}

void MotionMaster::Clear(MovementGeneratorPriority priority)
{
    if (Empty())
        return;

    std::function<bool(MovementGenerator const*)> criteria = [priority](MovementGenerator const* a) -> bool
    {
        return a->Priority == priority;
    };
    Clear(criteria);
}

void MotionMaster::Clear(std::function<bool(MovementGenerator const*)> const& filter)
{
    if (HasFlag(MOTIONMASTER_FLAG_DELAYED))
    {
        DelayedActionDefine action = [this, filter]()
        {
            Clear(filter);
        };
        _delayedActions.emplace_back(std::move(action), MOTIONMASTER_DELAYED_CLEAR_FILTER);
        return;
    }

    if (Empty())
        return;

    _DirectClear(filter);
}

void MotionMaster::PropagateSpeedChange()
{
    if (Empty())
        return;

    MovementGenerator* movement = GetCurrentMovementGenerator();
    if (!movement)
        return;

    movement->UnitSpeedChanged();
}

bool MotionMaster::GetDestination(float &x, float &y, float &z)
{
    if (_owner->movespline->Finalized())
        return false;

    G3D::Vector3 const& dest = _owner->movespline->FinalDestination();
    x = dest.x;
    y = dest.y;
    z = dest.z;
    return true;
}

bool MotionMaster::StopOnDeath()
{
    std::function<bool(MovementGenerator const*)> criteria = [](MovementGenerator const* a) -> bool
    {
        return a->HasFlag(MOVEMENTGENERATOR_FLAG_PERSIST_ON_DEATH);
    };
    bool returnValue = !HasMovementGenerator(criteria);
    if (_owner->IsInWorld())
    {
        // Only clear MotionMaster for entities that exists in world
        // Avoids crashes in the following conditions :
        //  * Using 'call pet' on dead pets
        //  * Using 'call stabled pet'
        //  * Logging in with dead pets
        criteria = [](MovementGenerator const* a) -> bool
        {
            return !a->HasFlag(MOVEMENTGENERATOR_FLAG_PERSIST_ON_DEATH);
        };
        Clear(criteria);
        MoveIdle();
    }

    return returnValue;
}

void MotionMaster::InterruptOnTeleport()
{
    if (MovementGenerator* top = GetCurrentMovementGenerator())
        if (!top->HasFlag(MOVEMENTGENERATOR_FLAG_DEACTIVATED | MOVEMENTGENERATOR_FLAG_FINALIZED))
            top->Deactivate(_owner); // only deactivate top, don't remove it. non-resumable generators will clean up themselves on next update
}

void MotionMaster::MoveIdle()
{
    Add(GetIdleMovementGenerator(), MOTION_SLOT_DEFAULT);
}

void MotionMaster::MoveTargetedHome()
{
    Creature* owner = _owner->ToCreature();
    if (!owner)
    {
        TC_LOG_ERROR("movement.motionmaster", "MotionMaster::MoveTargetedHome: '{}', attempted to move towards target home.", _owner->GetGUID().ToString());
        return;
    }

    Clear();

    Unit* target = owner->GetCharmerOrOwner();
    if (!target)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveTargetedHome: '{}', targeted home.", _owner->GetGUID().ToString());
        Add(new HomeMovementGenerator<Creature>());
    }
    else
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveTargetedHome: '{}', starts following '{}'", _owner->GetGUID().ToString(), target->GetGUID().ToString());
        Add(new FollowMovementGenerator(target, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE));
    }
}

void MotionMaster::MoveRandom(float wanderDistance)
{
    if (_owner->GetTypeId() == TYPEID_UNIT)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveRandom: '{}', started random movement (spawnDist: {})", _owner->GetGUID().ToString(), wanderDistance);
        Add(new RandomMovementGenerator<Creature>(wanderDistance), MOTION_SLOT_DEFAULT);
    }
}

void MotionMaster::MoveFollow(Unit* target, float dist, ChaseAngle angle, MovementSlot slot/* = MOTION_SLOT_ACTIVE*/, Optional<bool> run/* = {}*/)
{
    // Ignore movement request if target not exist
    if (!target || target == _owner)
        return;

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveFollow: '{}', starts following '{}'", _owner->GetGUID().ToString(), target->GetGUID().ToString());
    Add(new FollowMovementGenerator(target, dist, angle, run), slot);
}

void MotionMaster::MoveChase(Unit* target, Optional<ChaseRange> dist, Optional<ChaseAngle> angle, bool checkLostTarget/* = true*/)
{
    // Ignore movement request if target not exist
    if (!target || target == _owner)
        return;

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveChase: '{}', starts chasing '{}'", _owner->GetGUID().ToString(), target->GetGUID().ToString());
    ChaseMovementGenerator* newMovement = new ChaseMovementGenerator(target, dist, angle);
    if (!checkLostTarget)
        newMovement->CheckLostTarget = false;
    Add(newMovement);
}

void MotionMaster::MoveConfused()
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveConfused: '{}', started confused movement.", _owner->GetGUID().ToString());
        Add(new ConfusedMovementGenerator<Player>());
    }
    else
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveConfused: '{}', started confused movement.", _owner->GetGUID().ToString());
        Add(new ConfusedMovementGenerator<Creature>());
    }
}

void MotionMaster::MoveFleeing(Unit* enemy, uint32 time)
{
    if (!enemy)
        return;

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveFleeing: '{}', flees from '{}' (time: {})", _owner->GetGUID().ToString(), enemy->GetGUID().ToString(), time);
    if (_owner->GetTypeId() == TYPEID_UNIT)
    {
        if (time)
            Add(new TimedFleeingMovementGenerator(enemy->GetGUID(), time));
        else
            Add(new FleeingMovementGenerator<Creature>(enemy->GetGUID()));
    }
    else
        Add(new FleeingMovementGenerator<Player>(enemy->GetGUID()));
}

void MotionMaster::MovePoint(uint32 id, Position const& pos, bool generatePath/* = true*/, Optional<float> finalOrient/* = {}*/)
{
    MovePoint(id, pos.m_positionX, pos.m_positionY, pos.m_positionZ, generatePath, finalOrient);
}

void MotionMaster::MovePoint(uint32 id, float x, float y, float z, bool generatePath, Optional<float> finalOrient)
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MovePoint: '{}', targeted point Id: {} (X: {}, Y: {}, Z: {})", _owner->GetGUID().ToString(), id, x, y, z);
        Add(new PointMovementGenerator<Player>(id, x, y, z, generatePath, 0.0f, finalOrient));
    }
    else
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MovePoint: '{}', targeted point Id: {} (X: {}, Y: {}, Z: {})", _owner->GetGUID().ToString(), id, x, y, z);
        Add(new PointMovementGenerator<Creature>(id, x, y, z, generatePath, 0.0f, finalOrient));
    }
}

void MotionMaster::MoveCloserAndStop(uint32 id, Unit* target, float distance)
{
    float distanceToTravel = _owner->GetExactDist2d(target) - distance;
    if (distanceToTravel > 0.0f)
    {
        float angle = _owner->GetAbsoluteAngle(target);
        float destx = _owner->GetPositionX() + distanceToTravel * std::cos(angle);
        float desty = _owner->GetPositionY() + distanceToTravel * std::sin(angle);
        MovePoint(id, destx, desty, target->GetPositionZ());
    }
    else
    {
        // We are already close enough. We just need to turn toward the target without changing position.
        std::function<void(Movement::MoveSplineInit&)> initializer = [=, this, target = target->GetGUID()](Movement::MoveSplineInit& init)
        {
            init.MoveTo(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ());
            init.SetFacing(target);
        };
        Add(new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, id));
    }
}

void MotionMaster::MoveLand(uint32 id, Position const& pos, Optional<float> velocity /*= {}*/)
{
    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveLand: '{}', landing point Id: {} (X: {}, Y: {}, Z: {})", _owner->GetGUID().ToString(), id, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());

    std::function<void(Movement::MoveSplineInit&)> initializer = [=](Movement::MoveSplineInit& init)
    {
        init.MoveTo(PositionToVector3(pos), false);
        init.SetAnimation(AnimTier::Ground);
        if (velocity)
            init.SetVelocity(*velocity);
    };
    Add(new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, id));
}

void MotionMaster::MoveTakeoff(uint32 id, Position const& pos, Optional<float> velocity /*= {}*/)
{
    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveTakeoff: '{}', landing point Id: {} (X: {}, Y: {}, Z: {})", _owner->GetGUID().ToString(), id, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());

    std::function<void(Movement::MoveSplineInit&)> initializer = [=](Movement::MoveSplineInit& init)
    {
        init.MoveTo(PositionToVector3(pos), false);
        init.SetAnimation(AnimTier::Hover);
        if (velocity)
            init.SetVelocity(*velocity);
    };
    Add(new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, id));
}

void MotionMaster::MoveCharge(float x, float y, float z, float speed /*= SPEED_CHARGE*/, uint32 id /*= EVENT_CHARGE*/, bool generatePath /*= false*/)
{
/*
    if (_slot[MOTION_SLOT_CONTROLLED] && _slot[MOTION_SLOT_CONTROLLED]->GetMovementGeneratorType() != DISTRACT_MOTION_TYPE)
        return;
*/
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveCharge: '{}', charging point Id: {} (X: {}, Y: {}, Z: {})", _owner->GetGUID().ToString(), id, x, y, z);
        PointMovementGenerator<Player>* movement = new PointMovementGenerator<Player>(id, x, y, z, generatePath, speed);
        movement->Priority = MOTION_PRIORITY_HIGHEST;
        movement->Mode = MOTION_MODE_OVERRIDE;
        movement->BaseUnitState = UNIT_STATE_CHARGING;
        Add(movement);
    }
    else
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveCharge: '{}', charging point Id: {} (X: {}, Y: {}, Z: {})", _owner->GetGUID().ToString(), id, x, y, z);
        PointMovementGenerator<Creature>* movement = new PointMovementGenerator<Creature>(id, x, y, z, generatePath, speed);
        movement->Priority = MOTION_PRIORITY_HIGHEST;
        movement->Mode = MOTION_MODE_OVERRIDE;
        movement->BaseUnitState = UNIT_STATE_CHARGING;
        Add(movement);
    }
}

void MotionMaster::MoveCharge(PathGenerator const& path, float speed /*= SPEED_CHARGE*/)
{
    G3D::Vector3 dest = path.GetActualEndPosition();

    MoveCharge(dest.x, dest.y, dest.z, speed, EVENT_CHARGE_PREPATH);

    // Charge movement is not started when using EVENT_CHARGE_PREPATH
    Movement::MoveSplineInit init(_owner);
    init.MovebyPath(path.GetPath());
    init.SetVelocity(speed);
    init.Launch();
}

void MotionMaster::MoveKnockbackFrom(float srcX, float srcY, float speedXY, float speedZ)
{
    // This function may make players fall below map
    if (_owner->GetTypeId() == TYPEID_PLAYER)
        return;

    if (speedXY < 0.01f)
        return;

    Position dest = _owner->GetPosition();
    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    // Use a mmap raycast to get a valid destination.
    _owner->MovePositionToFirstCollision(dest, dist, _owner->GetRelativeAngle(srcX, srcY) + float(M_PI));

    std::function<void(Movement::MoveSplineInit&)> initializer = [=](Movement::MoveSplineInit& init)
    {
        init.MoveTo(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), false);
        init.SetParabolic(max_height, 0);
        init.SetOrientationFixed(true);
        init.SetVelocity(speedXY);
    };

    GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, 0);
    movement->Priority = MOTION_PRIORITY_HIGHEST;
    movement->Mode = MOTION_MODE_OVERRIDE;
    movement->AddFlag(MOVEMENTGENERATOR_FLAG_PERSIST_ON_DEATH);
    Add(movement);
}

void MotionMaster::MoveJumpTo(float angle, float speedXY, float speedZ)
{
    // This function may make players fall below map
    if (_owner->GetTypeId() == TYPEID_PLAYER)
        return;

    float x, y, z = _owner->GetPositionZ();

    float moveTimeHalf = speedZ / Movement::gravity;
    float dist = 2 * moveTimeHalf * speedXY;

    _owner->GetNearPoint2D(nullptr, x, y, dist, _owner->GetOrientation() + angle);
    _owner->UpdateAllowedPositionZ(x, y, z);

    MoveJump(x, y, z, 0.0f, speedXY, speedZ);
}

void MotionMaster::MoveJump(Position const& pos, float speedXY, float speedZ, uint32 id/* = EVENT_JUMP*/, bool hasOrientation/* = false*/)
{
    MoveJump(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), speedXY, speedZ, id, hasOrientation);
}

void MotionMaster::MoveJump(float x, float y, float z, float o, float speedXY, float speedZ, uint32 id, bool hasOrientation /* = false*/)
{
    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveJump: '{}', jumps to point Id: {} (X: {}, Y: {}, Z: {})", _owner->GetGUID().ToString(), id, x, y, z);
    if (speedXY < 0.01f)
        return;

    float moveTimeHalf = speedZ / Movement::gravity;
    float max_height = -Movement::computeFallElevation(moveTimeHalf, false, -speedZ);

    std::function<void(Movement::MoveSplineInit&)> initializer = [=](Movement::MoveSplineInit& init)
    {
        init.MoveTo(x, y, z, false);
        init.SetParabolic(max_height, 0);
        init.SetVelocity(speedXY);
        if (hasOrientation)
            init.SetFacing(o);
    };

    GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, id);
    movement->Priority = MOTION_PRIORITY_HIGHEST;
    movement->Mode = MOTION_MODE_OVERRIDE;
    movement->BaseUnitState = UNIT_STATE_JUMPING;
    movement->AddFlag(MOVEMENTGENERATOR_FLAG_PERSIST_ON_DEATH);
    Add(movement);
}

void MotionMaster::MoveCirclePath(float x, float y, float z, float radius, bool clockwise, uint8 stepCount)
{
    std::function<void(Movement::MoveSplineInit&)> initializer = [=, this](Movement::MoveSplineInit& init)
    {
        float step = 2 * float(M_PI) / stepCount * (clockwise ? -1.0f : 1.0f);
        Position const& pos = { x, y, z, 0.0f };
        float angle = pos.GetAbsoluteAngle(_owner->GetPositionX(), _owner->GetPositionY());

        // add the owner's current position as starting point as it gets removed after entering the cycle
        init.Path().push_back(G3D::Vector3(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ()));

        for (uint8 i = 0; i < stepCount; angle += step, ++i)
        {
            G3D::Vector3 point;
            point.x = x + radius * cosf(angle);
            point.y = y + radius * sinf(angle);

            if (_owner->IsFlying())
                point.z = z;
            else
                point.z = _owner->GetFloorZ() + _owner->GetHoverOffset();

            init.Path().push_back(point);
        }

        if (_owner->IsFlying())
        {
            init.SetFly();
            init.SetCyclic();
            init.SetAnimation(AnimTier::Hover);
        }
        else
        {
            init.SetWalk(true);
            init.SetCyclic();
        }
    };

    Add(new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, 0));
}

void MotionMaster::MoveSmoothPath(uint32 pointId, Position const* pathPoints, size_t pathSize, bool walk)
{
    Movement::PointsArray path;
    path.reserve(pathSize);
    std::transform(pathPoints, pathPoints + pathSize, std::back_inserter(path), [](Position const& point)
    {
        return G3D::Vector3(point.GetPositionX(), point.GetPositionY(), point.GetPositionZ());
    });

    std::function<void(Movement::MoveSplineInit&)> initializer = [=](Movement::MoveSplineInit& init)
    {
        init.MovebyPath(path);
        init.SetSmooth();
        init.SetWalk(walk);
    };

    // This code is not correct
    // GenericMovementGenerator does not affect UNIT_STATE_ROAMING_MOVE
    // need to call PointMovementGenerator with various pointIds
    Add(new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, pointId));
}

void MotionMaster::MoveAlongSplineChain(uint32 pointId, uint16 dbChainId, bool walk)
{
    Creature* owner = _owner->ToCreature();
    if (!owner)
    {
        TC_LOG_ERROR("movement.motionmaster", "MotionMaster::MoveAlongSplineChain: '{}', tried to walk along DB spline chain. Ignoring.", _owner->GetGUID().ToString());
        return;
    }
    std::vector<SplineChainLink> const* chain = sScriptSystemMgr->GetSplineChain(owner, dbChainId);
    if (!chain)
    {
        TC_LOG_ERROR("movement.motionmaster", "MotionMaster::MoveAlongSplineChain: '{}', tried to walk along non-existing spline chain with DB Id: {}.", _owner->GetGUID().ToString(), dbChainId);
        return;
    }
    MoveAlongSplineChain(pointId, *chain, walk);
}

void MotionMaster::MoveAlongSplineChain(uint32 pointId, std::vector<SplineChainLink> const& chain, bool walk)
{
    Add(new SplineChainMovementGenerator(pointId, chain, walk));
}

void MotionMaster::ResumeSplineChain(SplineChainResumeInfo const& info)
{
    if (info.Empty())
    {
        TC_LOG_ERROR("movement.motionmaster", "MotionMaster::ResumeSplineChain: '{}', tried to resume a spline chain from empty info.", _owner->GetGUID().ToString());
        return;
    }
    Add(new SplineChainMovementGenerator(info));
}

void MotionMaster::MoveFall(uint32 id/* = 0*/)
{
    // Use larger distance for vmap height search than in most other cases
    float tz = _owner->GetMapHeight(_owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ(), true, MAX_FALL_DISTANCE);
    if (tz <= INVALID_HEIGHT)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveFall: '{}', unable to retrieve a proper height at map Id: {} (X: {}, Y: {}, Z: {})",
            _owner->GetGUID().ToString(), _owner->GetMap()->GetId(), _owner->GetPositionX(), _owner->GetPositionY(), _owner->GetPositionZ());
        return;
    }

    // Abort too if the ground is very near
    if (std::fabs(_owner->GetPositionZ() - tz) < 0.1f)
        return;

    // rooted units don't move (also setting falling+root flag causes client freezes)
    if (_owner->HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED))
        return;

    _owner->SetFall(true);

    // Don't run spline movement for players
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        _owner->ToPlayer()->SetFallInformation(0, _owner->GetPositionZ());
        return;
    }

    std::function<void(Movement::MoveSplineInit&)> initializer = [=, this](Movement::MoveSplineInit& init)
    {
        init.MoveTo(_owner->GetPositionX(), _owner->GetPositionY(), tz + _owner->GetHoverOffset(), false);
        init.SetFall();
    };

    GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, id);
    movement->Priority = MOTION_PRIORITY_HIGHEST;
    movement->Mode = MOTION_MODE_OVERRIDE;
    Add(movement);
}

void MotionMaster::MoveSeekAssistance(float x, float y, float z)
{
    if (Creature* creature = _owner->ToCreature())
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveSeekAssistance: '{}', seeks assistance (X: {}, Y: {}, Z: {})", creature->GetGUID().ToString(), x, y, z);
        creature->AttackStop();
        creature->CastStop();
        creature->DoNotReacquireSpellFocusTarget();
        creature->SetReactState(REACT_PASSIVE);
        Add(new AssistanceMovementGenerator(EVENT_ASSIST_MOVE, x, y, z));
    }
    else
        TC_LOG_ERROR("movement.motionmaster", "MotionMaster::MoveSeekAssistance: '{}', attempted to seek assistance.", _owner->GetGUID().ToString());
}

void MotionMaster::MoveSeekAssistanceDistract(uint32 time)
{
    if (_owner->GetTypeId() == TYPEID_UNIT)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveSeekAssistanceDistract: '{}', is distracted after assistance call (Time: {})", _owner->GetGUID().ToString(), time);
        Add(new AssistanceDistractMovementGenerator(time, _owner->GetOrientation()));
    }
    else
        TC_LOG_ERROR("movement.motionmaster", "MotionMaster::MoveSeekAssistanceDistract: '{}', attempted to call distract assistance.", _owner->GetGUID().ToString());
}

void MotionMaster::MoveTaxiFlight(uint32 path, uint32 pathnode)
{
    if (_owner->GetTypeId() == TYPEID_PLAYER)
    {
        if (path < sTaxiPathNodesByPath.size())
        {
            TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveTaxiFlight: '{}', taxi to path Id: {} (node {})", _owner->GetGUID().ToString(), path, pathnode);

            // Only one FLIGHT_MOTION_TYPE is allowed
            bool hasExisting = HasMovementGenerator([](MovementGenerator const* gen) { return gen->GetMovementGeneratorType() == FLIGHT_MOTION_TYPE; });
            ASSERT(!hasExisting, "Duplicate flight path movement generator");

            FlightPathMovementGenerator* movement = new FlightPathMovementGenerator(pathnode);
            movement->LoadPath(_owner->ToPlayer());
            Add(movement);
        }
        else
            TC_LOG_ERROR("movement.motionmaster", "MotionMaster::MoveTaxiFlight: '{}', attempted taxi to non-existing path Id: {} (node: {})", _owner->GetGUID().ToString(), path, pathnode);
    }
    else
        TC_LOG_ERROR("movement.motionmaster", "MotionMaster::MoveTaxiFlight: '{}', attempted taxi to path Id: {} (node: {})", _owner->GetGUID().ToString(), path, pathnode);
}

void MotionMaster::MoveDistract(uint32 timer, float orientation)
{
/*
    if (_slot[MOTION_SLOT_CONTROLLED])
        return;
*/
    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveDistract: '{}', distracted (timer: {}, orientation: {})", _owner->GetGUID().ToString(), timer, orientation);
    Add(new DistractMovementGenerator(timer, orientation));
}

void MotionMaster::MovePath(uint32 pathId, bool repeatable)
{
    if (!pathId)
        return;

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MovePath: '{}', starts moving over path Id: {} (repeatable: {})", _owner->GetGUID().ToString(), pathId, repeatable ? "YES" : "NO");
    Add(new WaypointMovementGenerator<Creature>(pathId, repeatable), MOTION_SLOT_DEFAULT);
}

void MotionMaster::MovePath(WaypointPath& path, bool repeatable)
{
    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MovePath: '{}', starts moving over path Id: {} (repeatable: {})", _owner->GetGUID().ToString(), path.id, repeatable ? "YES" : "NO");
    Add(new WaypointMovementGenerator<Creature>(path, repeatable), MOTION_SLOT_DEFAULT);
}

void MotionMaster::MoveRotate(uint32 id, uint32 time, RotateDirection direction)
{
    if (!time)
        return;

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveRotate: '{}', starts rotate (time: {}, direction: {})", _owner->GetGUID().ToString(), time, direction);
    Add(new RotateMovementGenerator(id, time, direction));
}

void MotionMaster::MoveFormation(Unit* leader, float range, float angle, uint32 point1, uint32 point2)
{
    if (_owner->GetTypeId() == TYPEID_UNIT && leader)
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveFormation: '{}', started to move in a formation with leader {}", _owner->GetGUID().ToString(), leader->GetGUID().ToString());
        Add(new FormationMovementGenerator(leader, range, angle, point1, point2), MOTION_SLOT_DEFAULT);
    }
}

void MotionMaster::MoveFace(WorldObject const* object, uint32 id/* = EVENT_FACE*/, Milliseconds duration/* = 0ms*/)
{
    if (!object || GetCurrentMovementGeneratorPriority() == MOTION_PRIORITY_HIGHEST)
        return;

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveFace: '{}', faces '{}'", _owner->GetGUID().ToString(), object->GetGUID().ToString());

    std::function<void(Movement::MoveSplineInit&)> initializer = [owner = _owner, object](Movement::MoveSplineInit& init)
    {
        init.MoveTo(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), false);
        if (object)
            init.SetFacing(owner->GetAbsoluteAngle(object));   // when on transport, GetAbsoluteAngle will still return global coordinates (and angle) that needs transforming
    };

    GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), FACE_MOTION_TYPE, id);
    movement->Priority = MOTION_PRIORITY_NORMAL;
    movement->Mode = MOTION_MODE_OVERRIDE;
    movement->BaseUnitState = UNIT_STATE_FACING;
    if (duration != 0ms)
    {
        movement->AddFlag(MOVEMENTGENERATOR_FLAG_FIXED_DURATION);
        movement->SetDuration(duration);
    }
    Add(movement);
}

void MotionMaster::MoveFace(float orientation, uint32 id/* = EVENT_FACE*/, Milliseconds duration/* = 0ms*/)
{
    if (GetCurrentMovementGeneratorPriority() == MOTION_PRIORITY_HIGHEST)
        return;

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::MoveFace: '{}', faces '{}'", _owner->GetGUID().ToString(), orientation);

    std::function<void(Movement::MoveSplineInit&)> initializer = [owner = _owner, orientation](Movement::MoveSplineInit& init)
    {
        init.MoveTo(owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ(), false);
        if (owner->GetTransport())
            init.DisableTransportPathTransformations(); // It makes no sense to target global orientation
        init.SetFacing(orientation);
    };

    GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), FACE_MOTION_TYPE, id);
    movement->Priority = MOTION_PRIORITY_NORMAL;
    movement->Mode = MOTION_MODE_OVERRIDE;
    movement->BaseUnitState = UNIT_STATE_FACING;
    if (duration != 0ms)
    {
        movement->AddFlag(MOVEMENTGENERATOR_FLAG_FIXED_DURATION);
        movement->SetDuration(duration);
    }
    Add(movement);
}

void MotionMaster::LaunchMoveSpline(std::function<void(Movement::MoveSplineInit& init)>&& initializer, uint32 id/*= 0*/, MovementGeneratorPriority priority/* = MOTION_PRIORITY_NORMAL*/, MovementGeneratorType type/*= EFFECT_MOTION_TYPE*/, MovementGeneratorMode mode/* = MOTION_MODE_DEFAULT*/)
{
    if (IsInvalidMovementGeneratorType(type))
    {
        TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::LaunchMoveSpline: '{}', tried to launch a spline with an invalid MovementGeneratorType: {} (Id: {}, Priority: {})", _owner->GetGUID().ToString(), type, id, priority);
        return;
    }

    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::LaunchMoveSpline: '{}', initiates spline Id: {} (Type: {}, Priority: {})", _owner->GetGUID().ToString(), id, type, priority);

    GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), type, id);
    movement->Mode = mode;
    movement->Priority = priority;
    Add(movement);
}

/******************** Private members ********************/

bool MotionMaster::MovementGeneratorComparator::operator()(MotionMasterContainerKey const& a, MotionMasterContainerKey const& b) const
{
    if (a.first != b.first)
        return a.first > b.first;

    return a.second > b.second;
}

void MotionMaster::_ResolveDelayedActions()
{
    while (!_delayedActions.empty())
    {
        _delayedActions.front().Resolve();
        _delayedActions.pop_front();
    }
}

void MotionMaster::_Remove(MotionMasterContainer::iterator& iterator, MotionMasterContainerList::iterator& listIterator, bool active, bool movementInform)
{
    MovementGenerator* pointer = *listIterator;
    listIterator = iterator->second.erase(listIterator);
    if (iterator->second.empty())
        iterator = _generators.erase(iterator);
    _Delete(pointer, active, movementInform);
}

void MotionMaster::_Pop(bool active, bool movementInform)
{
    auto itr = _generators.begin();
    if (itr != _generators.end())
    {
        auto listItr = itr->second.begin();
        _Remove(itr, listItr, active, movementInform);
    }
}

void MotionMaster::_DirectInitialize()
{
    // Clear ALL movement generators (including default)
    _DirectClearDefault();
    _DirectClear();

    InitializeDefault();
}

void MotionMaster::_DirectClear()
{
    // First delete Top
    if (!_generators.empty())
        _Pop(true, false);

    // Then the rest
    while (!_generators.empty())
        _Pop(false, false);

    // Make sure the storage is empty
    _ClearBaseUnitStates();
}

void MotionMaster::_DirectClearDefault()
{
    if (_defaultGenerator)
        _DeleteDefault(_generators.empty(), false);
}

void MotionMaster::_DirectClear(std::function<bool(MovementGenerator const*)> const& filter)
{
    if (_generators.empty())
        return;

    MovementGenerator const* top = GetCurrentMovementGenerator();
    for (auto itr = _generators.begin(); itr != _generators.end();)
    {
        for (auto itrList = itr->second.begin(); itrList != itr->second.end();)
        {
            if (filter(*itrList))
            {
                MovementGenerator* pointer = *itrList;
                itrList = itr->second.erase(itrList);
                _Delete(pointer, pointer == top, false);
            }
            else
                ++itrList;
        }
        if (itr->second.empty())
            itr = _generators.erase(itr);
        else
            ++itr;
    }
}

void MotionMaster::_DirectAdd(MovementGenerator* movement, MovementSlot slot/* = MOTION_SLOT_ACTIVE*/)
{
    switch (slot)
    {
        case MOTION_SLOT_DEFAULT:
            if (_defaultGenerator)
                _defaultGenerator->Finalize(_owner, _generators.empty(), false);

            _defaultGenerator = MovementGeneratorPointer(movement);
            if (IsStatic(movement))
                AddFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING);
            break;
        case MOTION_SLOT_ACTIVE:
        {
            MotionMasterContainerKey newKey = { movement->Priority, movement->Mode };
            auto itr = _generators.lower_bound(newKey);
            bool replacesExisting = !movement->HasFlag(MOVEMENTGENERATOR_FLAG_IMMEDIATE)
                && itr != _generators.end()
                && !_generators.key_comp()(newKey, itr->first);
            if (!_generators.empty())
            {
                MovementGenerator* currentTopMovement = _generators.begin()->second.front();
                if (replacesExisting)
                {
                    MovementGenerator* existingMovement = itr->second.front();
                    MotionMasterContainerKey currentTopKey = _generators.begin()->first;
                    itr->second.clear();
                    itr->second.push_front(movement);
                    _Delete(existingMovement, newKey == currentTopKey, false);
                }
                else
                {
                    if (itr == _generators.begin())
                        currentTopMovement->Deactivate(_owner);
                }
            }
            else
                _defaultGenerator->Deactivate(_owner);

            if (!movement->HasFlag(MOVEMENTGENERATOR_FLAG_IMMEDIATE))
            {
                if (_generators.empty() || !replacesExisting)
                    _generators.emplace_hint(itr, newKey, std::list{ movement });
                _AddBaseUnitState(movement);
            }
            else
            {
                movement->Finalize(_owner, true, true);
                delete movement;
            }
            break;
        }
        default:
            break;
    }
}

void MotionMaster::_Delete(MovementGenerator* movement, bool active, bool movementInform)
{
    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::Delete: deleting generator (Priority: {}, Flags: {}, BaseUnitState: {}, Type: {}), owner: '{}'",
        movement->Priority, movement->Flags, movement->BaseUnitState, movement->GetMovementGeneratorType(), _owner->GetGUID().ToString());

    movement->Finalize(_owner, active, movementInform);
    _ClearBaseUnitState(movement);
    MovementGeneratorPointerDeleter(movement);
}

void MotionMaster::_DeleteDefault(bool active, bool movementInform)
{
    TC_LOG_DEBUG("movement.motionmaster", "MotionMaster::_DeleteDefault: deleting generator (Priority: {}, Flags: {}, BaseUnitState: {}, Type: {}), owner: '{}'",
        _defaultGenerator->Priority, _defaultGenerator->Flags, _defaultGenerator->BaseUnitState, _defaultGenerator->GetMovementGeneratorType(), _owner->GetGUID().ToString());

    _defaultGenerator->Finalize(_owner, active, movementInform);
    _defaultGenerator = MovementGeneratorPointer(GetIdleMovementGenerator());
    AddFlag(MOTIONMASTER_FLAG_STATIC_INITIALIZATION_PENDING);
}

void MotionMaster::_AddBaseUnitState(MovementGenerator const* movement)
{
    if (!movement || !movement->BaseUnitState)
        return;

    _baseUnitStatesMap.emplace(movement->BaseUnitState, movement);
    _owner->AddUnitState(movement->BaseUnitState);
}

void MotionMaster::_ClearBaseUnitState(MovementGenerator const* movement)
{
    if (!movement || !movement->BaseUnitState)
        return;

    Trinity::Containers::MultimapErasePair(_baseUnitStatesMap, movement->BaseUnitState, movement);
    if (_baseUnitStatesMap.count(movement->BaseUnitState) == 0)
        _owner->ClearUnitState(movement->BaseUnitState);
}

void MotionMaster::_ClearBaseUnitStates()
{
    uint32 unitState = 0;
    for (auto itr = _baseUnitStatesMap.begin(); itr != _baseUnitStatesMap.end(); ++itr)
        unitState |= itr->first;

    _owner->ClearUnitState(unitState);
    _baseUnitStatesMap.clear();
}
