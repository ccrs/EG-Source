#ifndef CUSTOM_FUNCTIONS
#define CUSTOM_FUNCTIONS

#include "SharedDefines.h"

class Unit;

namespace EG
{
    class TC_GAME_API MostHPMissingFriendlyUnitInRangeSearcher
    {
    public:
        MostHPMissingFriendlyUnitInRangeSearcher(Unit const* source, float range, bool playerOnly = false) : _source(source), _range(range), _playerOnly(playerOnly), _hp(0) { }
        bool operator()(Unit* unit);

    private:
        Unit const* _source;
        float _range;
        bool _playerOnly;
        uint32 _hp;
    };
}

#endif // CUSTOM_FUNCTIONS
