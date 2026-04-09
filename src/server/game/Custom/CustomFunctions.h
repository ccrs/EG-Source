#ifndef CUSTOM_FUNCTIONS
#define CUSTOM_FUNCTIONS

#include "SharedDefines.h"

class Unit;
class WorldObject;

namespace EG
{
    class TC_GAME_API MostHPMissingFriendlyUnitInRangeSearcher
    {
    public:
        MostHPMissingFriendlyUnitInRangeSearcher(Unit const* source, float range, bool playerOnly = false, bool includeSelf = false) : _source(source), _range(range), _playerOnly(playerOnly), _includeSelf(includeSelf) , _hp(100.f){ }
        bool operator()(Unit* unit);

    private:
        Unit const* _source;
        float _range;
        bool _playerOnly;
        bool _includeSelf;
        float _hp;
    };

    class TC_GAME_API AnyFriendlyUnitInObjectRangeCheck
    {
    public:
        AnyFriendlyUnitInObjectRangeCheck(Unit const* source, float range, bool playerOnly = false, bool includeSelf = false) : _source(source), _range(range), _playerOnly(playerOnly), _includeSelf(includeSelf) { }

        bool operator()(Unit* u) const;

    private:
        Unit const* _source;
        float _range;
        bool _playerOnly;
        bool _includeSelf;
    };
}

#endif // CUSTOM_FUNCTIONS
