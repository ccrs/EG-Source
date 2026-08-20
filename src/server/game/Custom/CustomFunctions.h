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

#ifndef CUSTOM_FUNCTIONS
#define CUSTOM_FUNCTIONS

#include "EventProcessor.h"
#include "Object.h"
#include "SharedDefines.h"

class Player;
class Unit;
class WorldObject;

namespace EG
{
    TC_GAME_API bool IsBoostedDay();
    TC_GAME_API void SetBoostedDay(bool active);

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
        AnyFriendlyUnitInObjectRangeCheck(WorldObject const* source, WorldObject::FriendlySearchOptions options) : _source(source), _options(options) { }

        bool operator()(Unit const* u) const;

    private:
        WorldObject const* _source;
        WorldObject::FriendlySearchOptions _options;
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

    class TC_GAME_API CleanRaceMasquerade : public BasicEvent
    {
    public:
        CleanRaceMasquerade(Player* owner);

        bool Execute(uint64 /*time*/, uint32 /*diff*/) override;

    private:
        Player* _owner;
    };
}

#endif // CUSTOM_FUNCTIONS
