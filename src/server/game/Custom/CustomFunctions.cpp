#include "Unit.h"
#include "CustomFunctions.h"

bool EG::MostHPMissingFriendlyUnitInRangeSearcher::operator()(Unit* unit)
{
    if (_source == unit && (unit->GetMaxHealth() - unit->GetHealth() > _hp))
    {
        _hp = unit->GetMaxHealth() - unit->GetHealth();
        return true;
    }

    if ((_playerOnly && unit->IsPlayer() || !_playerOnly)
        && unit->IsAlive()
        && unit->IsInCombat()
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _range)
        && (unit->GetMaxHealth() - unit->GetHealth() > _hp)
    )
    {
        _hp = unit->GetMaxHealth() - unit->GetHealth();
        return true;
    }

    return false;
}
