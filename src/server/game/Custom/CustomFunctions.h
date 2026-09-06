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
#include <chrono>
#include <vector>

class Group;
class Player;
class Unit;
class WorldObject;
struct AchievementEntry;

namespace EG
{
    TC_GAME_API bool IsBoostedDay();
    TC_GAME_API void SetBoostedDay(bool active);
    TC_GAME_API bool CanEarnRealmFirst(Player const* player, AchievementEntry const* achievement);

    constexpr std::chrono::year HolidayPackedDateMaxYear{ 2031 };

    enum class HolidayRuleType : uint8
    {
        FixedDate = 0, // Month + Day, the same date every year
        NthWeekday, // Occurrence-th Weekday of Month
        FirstWeekdayOfMonth, // First Weekday of Month
        WeekdayOnOrAfter, // First Weekday falling on or after Month/Day
        EasterOffset, // Easter Sunday
        LunarNewYear, // Chinese New Year
        QuarterMonthFirstWeekday, // First Weekday of every month where month % 3 == Occurrence
        WeeklyWeekday, // Every Weekday, no yearly anchor
        Max
    };

    struct HolidayRule
    {
        uint32 HolidayId = 0;
        HolidayRuleType Type = HolidayRuleType::FixedDate;
        std::chrono::month Month = std::chrono::January;
        std::chrono::day Day{ 1 };
        std::chrono::weekday Weekday = std::chrono::Sunday;
        std::chrono::days DayOffset{ 0 }; // applied once the base date is resolved
        Minutes TimeOfDay{ 0 }; // wall clock offset into the start day, in the server's local zone
        uint8 Occurrence = 0; // nth for NthWeekday, month modulus for QuarterMonthFirstWeekday

        constexpr bool IsWeekly() const { return Type == HolidayRuleType::WeeklyWeekday; }
    };

    namespace HolidayCalendar
    {
        TC_GAME_API std::chrono::year_month_day CalculateEasterSunday(std::chrono::year year);
        TC_GAME_API std::chrono::year_month_day CalculateLunarNewYear(std::chrono::year year);
        TC_GAME_API std::vector<time_t> GetYearOccurrences(HolidayRule const& rule, std::chrono::year year);
        TC_GAME_API Optional<time_t> GetWeeklyAnchor(HolidayRule const& rule, time_t referenceTime);
        TC_GAME_API Optional<time_t> GetLocalWallClockAnchor(time_t originalStart, Minutes period, time_t referenceTime);
        TC_GAME_API Optional<uint32> PackDate(time_t localTime);
    }

    namespace LFGRandomReward
    {
        TC_GAME_API void TryReward(Player* player, Group* group);
    }

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
