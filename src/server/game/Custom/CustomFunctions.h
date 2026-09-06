#ifndef CUSTOM_FUNCTIONS
#define CUSTOM_FUNCTIONS

#include "Object.h"
#include "SharedDefines.h"
#include <chrono>
#include <vector>

class Unit;
class WorldObject;

namespace EG
{

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
}

#endif // CUSTOM_FUNCTIONS
