#include "CustomFunctions.h"
#include "Map.h"
#include "Object.h"
#include "Unit.h"


bool WorldObject::IsWithinLOSInMap(Position const* reference, WorldObject const* obj, LineOfSightChecks checks, VMAP::ModelIgnoreFlags ignoreFlags) const
{
    if (!IsInMap(obj))
        return false;

    float ox, oy, oz;
    if (obj->GetTypeId() == TYPEID_PLAYER)
    {
        obj->GetPosition(ox, oy, oz);
        oz += GetCollisionHeight();
    }
    else
        obj->GetHitSpherePointFor({ reference->GetPositionX(), reference->GetPositionY(), reference->GetPositionZ() + GetCollisionHeight() }, ox, oy, oz);

    float x, y, z;
    if (GetTypeId() == TYPEID_PLAYER)
    {
        reference->GetPosition(x, y, z);
        z += GetCollisionHeight();
    }
    else
        GetHitSpherePointFor({ reference->GetPositionX(), reference->GetPositionY(), reference->GetPositionZ() + obj->GetCollisionHeight() }, x, y, z);

    return GetMap()->isInLineOfSight(x, y, z, ox, oy, oz, GetPhaseMask(), checks, ignoreFlags);
}

bool EG::MostHPMissingFriendlyUnitInRangeSearcher::operator()(Unit* unit)
{
    if (_includeSelf && _source == unit && unit->GetHealthPct() < _hp)
    {
        _hp = unit->GetHealthPct();
        return true;
    }

    if (((_playerOnly && unit->IsPlayer()) || !_playerOnly)
        && unit->IsAlive()
        && unit->IsInCombat()
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _range)
        && unit->GetHealthPct() < _hp
    )
    {
        _hp = unit->GetHealthPct();
        return true;
    }

    return false;
}
