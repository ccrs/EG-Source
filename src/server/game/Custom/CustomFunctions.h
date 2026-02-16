#ifndef CUSTOM_FUNCTIONS
#define CUSTOM_FUNCTIONS

#include "EventProcessor.h"
#include "SharedDefines.h"

class Player;
class Unit;

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

    class TC_GAME_API SetRaceMasqueradeSetting : public BasicEvent
    {
    public:
        SetRaceMasqueradeSetting(Player* owner, Races selectedRace);

        bool Execute(uint64 /*time*/, uint32 /*diff*/) override;

    private:
        Player* _owner;
        Races const _selectedRace;
    };
}

#endif // CUSTOM_FUNCTIONS
