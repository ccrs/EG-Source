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

#include "CustomFunctions.h"
#include "Util.h"
#include "WowTime.h"
#include <G3D/g3dmath.h>
#include <array>
#include <cmath>

namespace
{
    using EG::HolidayRule;
    using EG::HolidayRuleType;

    // Julian Day of the unix epoch, the bridge between chrono and the astronomical formulae
    constexpr double JulianDayAtUnixEpoch = 2440587.5;

    // Mean lunations per year, seeds the new moon search
    constexpr double LunationsPerYear = 12.3685;

    // Mean lunations per Julian century, converts a lunation count into Meeus' time argument T
    constexpr double LunationsPerJulianCentury = 1236.85;

    // Julian Ephemeris Day of the new moon of 2000 January 6, the zero point of the lunation count
    constexpr double NewMoonEpochJde = 2451550.09766;

    // Mean length of a synodic month in days
    constexpr double SynodicMonthDays = 29.530588861;

    // Terrestrial Time minus Universal Time, near enough constant across the years the client can represent
    constexpr Seconds DeltaT = 70s;

    // Chinese New Year is resolved against China Standard Time
    constexpr Hours ChinaUtcOffset = 8h;

    double SinDegrees(double degrees)
    {
        return std::sin(G3D::toRadians(degrees));
    }

    double ToDayFraction(Seconds duration)
    {
        return std::chrono::duration_cast<std::chrono::duration<double, std::chrono::days::period>>(duration).count();
    }

    double ToJulianDay(std::chrono::sys_days date)
    {
        return double(date.time_since_epoch().count()) + JulianDayAtUnixEpoch;
    }

    std::chrono::year_month_day FromJulianDay(double julianDay)
    {
        return std::chrono::year_month_day{ std::chrono::sys_days{ std::chrono::days{ int64(std::floor(julianDay - JulianDayAtUnixEpoch)) } } };
    }

    // Julian Ephemeris Day of the new moon that closes the given lunation, Meeus chapter 49.
    // A mean phase from the linear lunation count, then periodic corrections from tables 49.A and 49.B.
    // lunation counts new moons from 2000 January 6 and may be negative for earlier dates.
    double CalculateNewMoon(double lunation)
    {
        // Meeus' time argument T, Julian centuries from the epoch, plus the powers the polynomials need
        double const centuries = lunation / LunationsPerJulianCentury;
        double const centuries2 = centuries * centuries;
        double const centuries3 = centuries2 * centuries;
        double const centuries4 = centuries3 * centuries;

        // Equation 49.1, where the new moon would fall if the moon orbited at a constant rate
        double const meanPhase = NewMoonEpochJde + SynodicMonthDays * lunation
            + 0.00015437 * centuries2 - 0.000000150 * centuries3 + 0.00000000073 * centuries4;

        // Eccentricity of the earth's orbit, scales the terms that depend on the sun's position
        double const eccentricity = 1.0 - 0.002516 * centuries - 0.0000074 * centuries2;
        double const eccentricitySquared = eccentricity * eccentricity;

        double const sunAnomaly = 2.5534 + 29.10535670 * lunation // equation 49.4
            - 0.0000014 * centuries2 - 0.00000011 * centuries3;
        double const moonAnomaly = 201.5643 + 385.81693528 * lunation // equation 49.5
            + 0.0107582 * centuries2 + 0.00001238 * centuries3 - 0.000000058 * centuries4;
        double const latitudeArgument = 160.7108 + 390.67050284 * lunation // equation 49.6
            - 0.0016118 * centuries2 - 0.00000227 * centuries3 + 0.000000011 * centuries4;
        double const ascendingNode = 124.7746 - 1.56375588 * lunation // equation 49.7
            + 0.0020672 * centuries2 + 0.00000215 * centuries3;

        // Table 49.A, the periodic terms that shift the true new moon away from the mean one
        double correction =
            - 0.40720 * SinDegrees(moonAnomaly)
            + 0.17241 * eccentricity * SinDegrees(sunAnomaly)
            + 0.01608 * SinDegrees(2 * moonAnomaly)
            + 0.01039 * SinDegrees(2 * latitudeArgument)
            + 0.00739 * eccentricity * SinDegrees(moonAnomaly - sunAnomaly)
            - 0.00514 * eccentricity * SinDegrees(moonAnomaly + sunAnomaly)
            + 0.00208 * eccentricitySquared * SinDegrees(2 * sunAnomaly)
            - 0.00111 * SinDegrees(moonAnomaly - 2 * latitudeArgument)
            - 0.00057 * SinDegrees(moonAnomaly + 2 * latitudeArgument)
            + 0.00056 * eccentricity * SinDegrees(2 * moonAnomaly + sunAnomaly)
            - 0.00042 * SinDegrees(3 * moonAnomaly)
            + 0.00042 * eccentricity * SinDegrees(sunAnomaly + 2 * latitudeArgument)
            + 0.00038 * eccentricity * SinDegrees(sunAnomaly - 2 * latitudeArgument)
            - 0.00024 * eccentricity * SinDegrees(2 * moonAnomaly - sunAnomaly)
            - 0.00017 * SinDegrees(ascendingNode);

        // Table 49.B, a weight and the polynomial coefficients of its argument in degrees. Only A1 has a quadratic term
        struct PlanetaryTerm
        {
            double Weight;
            double Constant;
            double Linear;
            double Quadratic;
        };

        constexpr std::array<PlanetaryTerm, 14> planetaryTerms
        {{
            { 0.000325, 299.77, 0.107408, -0.009173 },
            { 0.000165, 251.88, 0.016321, 0.0 },
            { 0.000164, 251.83, 26.651886, 0.0 },
            { 0.000126, 349.42, 36.412478, 0.0 },
            { 0.000110, 84.66, 18.206239, 0.0 },
            { 0.000062, 141.74, 53.303771, 0.0 },
            { 0.000060, 207.14, 2.453732, 0.0 },
            { 0.000056, 154.84, 7.306860, 0.0 },
            { 0.000047, 34.52, 27.261239, 0.0 },
            { 0.000042, 207.19, 0.121824, 0.0 },
            { 0.000040, 291.34, 1.844379, 0.0 },
            { 0.000037, 161.72, 24.198154, 0.0 },
            { 0.000035, 239.56, 25.513099, 0.0 },
            { 0.000023, 331.55, 3.592518, 0.0 }
        }};

        for (PlanetaryTerm const& term : planetaryTerms)
            correction += term.Weight * SinDegrees(term.Constant + term.Linear * lunation + term.Quadratic * centuries2);

        return meanPhase + correction;
    }

    time_t ToLocalTimestamp(std::chrono::year_month_day date, Seconds timeOfDay)
    {
        tm local = { };
        local.tm_year = int(date.year()) - 1900;
        local.tm_mon = int(unsigned(date.month())) - 1;
        local.tm_mday = int(unsigned(date.day()));
        local.tm_hour = int(std::chrono::duration_cast<Hours>(timeOfDay).count());
        local.tm_min = int(std::chrono::duration_cast<Minutes>(timeOfDay % 1h).count());
        local.tm_sec = int((timeOfDay % 1min).count());
        local.tm_isdst = -1;
        return mktime(&local);
    }

    std::chrono::year_month_day LocalDateOf(time_t timestamp)
    {
        tm const local = TimeBreakdown(timestamp);
        return std::chrono::year{ local.tm_year + 1900 } / std::chrono::month{ unsigned(local.tm_mon + 1) } / std::chrono::day{ unsigned(local.tm_mday) };
    }

    std::chrono::year_month_day NthWeekdayOfMonth(std::chrono::year targetYear, std::chrono::month targetMonth, std::chrono::weekday targetWeekday, uint8 occurrence)
    {
        std::chrono::year_month_weekday const nth = targetYear / targetMonth / targetWeekday[occurrence];
        if (!nth.ok())
            return std::chrono::year_month_day{ };

        return std::chrono::year_month_day{ std::chrono::sys_days{ nth } };
    }

    std::chrono::year_month_day WeekdayOnOrAfter(std::chrono::year_month_day date, std::chrono::weekday target)
    {
        std::chrono::sys_days const start{ date };
        return std::chrono::year_month_day{ start + (target - std::chrono::weekday{ start }) };
    }

    std::chrono::year_month_day BaseDate(HolidayRule const& rule, std::chrono::year targetYear, std::chrono::month targetMonth)
    {
        switch (rule.Type)
        {
            case HolidayRuleType::FixedDate:
                return targetYear / targetMonth / rule.Day;
            case HolidayRuleType::NthWeekday:
                return NthWeekdayOfMonth(targetYear, targetMonth, rule.Weekday, rule.Occurrence);
            case HolidayRuleType::FirstWeekdayOfMonth:
            case HolidayRuleType::QuarterMonthFirstWeekday:
                return NthWeekdayOfMonth(targetYear, targetMonth, rule.Weekday, 1);
            case HolidayRuleType::WeekdayOnOrAfter:
                return WeekdayOnOrAfter(targetYear / targetMonth / rule.Day, rule.Weekday);
            case HolidayRuleType::EasterOffset:
                return EG::HolidayCalendar::CalculateEasterSunday(targetYear);
            case HolidayRuleType::LunarNewYear:
                return EG::HolidayCalendar::CalculateLunarNewYear(targetYear);
            default:
                return std::chrono::year_month_day{ };
        }
    }
}

namespace EG::HolidayCalendar
{
    std::chrono::year_month_day CalculateEasterSunday(std::chrono::year targetYear)
    {
        int32 const yearNumber = int(targetYear);

        // Position in the Metonic cycle, which is what fixes the moon phase for a given year
        int32 const metonicPosition = yearNumber % 19;

        int32 const century = yearNumber / 100;
        int32 const yearInCentury = yearNumber % 100;

        // Gregorian leap rule, a century year is only a leap year every fourth century
        int32 const centuryLeapCycles = century / 4;
        int32 const centuryLeapRemainder = century % 4;

        // Lunar equation, correcting the Metonic cycle for its slow drift against the real moon
        int32 const lunarCorrectionTerm = (century + 8) / 25;
        int32 const lunarCorrection = (century - lunarCorrectionTerm + 1) / 3;

        // Days from March 21 to the paschal full moon
        int32 const paschalFullMoon = (19 * metonicPosition + century - centuryLeapCycles - lunarCorrection + 15) % 30;

        int32 const yearLeapCycles = yearInCentury / 4;
        int32 const yearLeapRemainder = yearInCentury % 4;

        // Days from the paschal full moon to the Sunday that follows it
        int32 const daysToSunday = (32 + 2 * centuryLeapRemainder + 2 * yearLeapCycles - paschalFullMoon - yearLeapRemainder) % 7;

        // Pulls back the two dates the plain formula overshoots, April 26 to April 19 and April 25 to April 18
        int32 const lateAprilCorrection = (metonicPosition + 11 * paschalFullMoon + 22 * daysToSunday) / 451;

        // Encoded so that the quotient by 31 is the month and the remainder is the day, 114 being March 22
        int32 const encodedDate = paschalFullMoon + daysToSunday - 7 * lateAprilCorrection + 114;

        return targetYear / std::chrono::month{ unsigned(encodedDate / 31) } / std::chrono::day{ unsigned(encodedDate % 31) + 1 };
    }

    std::chrono::year_month_day CalculateLunarNewYear(std::chrono::year targetYear)
    {
        // Chinese New Year is the new moon falling between January 21 and February 20
        double const rangeStart = ToJulianDay(std::chrono::sys_days{ targetYear / std::chrono::January / 21 });
        double const rangeEnd = ToJulianDay(std::chrono::sys_days{ targetYear / std::chrono::February / 21 });
        double const baseLunation = std::floor((int(targetYear) - 2000.0) * LunationsPerYear);

        // The lunation estimate can be off by one either way, so probe the neighbours until one lands in the window
        for (int32 lunationOffset = -2; lunationOffset <= 2; ++lunationOffset)
        {
            // Meeus works in Terrestrial Time, the calendar date wanted here is the civil one in China
            double const newMoon = CalculateNewMoon(baseLunation + lunationOffset) - ToDayFraction(DeltaT) + ToDayFraction(ChinaUtcOffset);

            if (newMoon >= rangeStart && newMoon < rangeEnd)
                return FromJulianDay(newMoon);
        }

        // Unreachable for every year the client can represent, kept so callers always receive a usable date
        return targetYear / std::chrono::January / 25;
    }

    std::vector<time_t> GetYearOccurrences(HolidayRule const& rule, std::chrono::year targetYear)
    {
        std::vector<time_t> occurrences;

        if (rule.IsWeekly() || rule.Type >= HolidayRuleType::Max)
            return occurrences;

        auto resolve = [&rule](std::chrono::year_month_day base)
        {
            return ToLocalTimestamp(std::chrono::year_month_day{ std::chrono::sys_days{ base } + rule.DayOffset }, rule.TimeOfDay);
        };

        if (rule.Type == HolidayRuleType::QuarterMonthFirstWeekday)
        {
            occurrences.reserve(4);
            for (unsigned monthIndex = 1; monthIndex <= 12; ++monthIndex)
            {
                if (monthIndex % 3 != rule.Occurrence)
                    continue;

                std::chrono::year_month_day const quarterBase = BaseDate(rule, targetYear, std::chrono::month{ monthIndex });
                if (quarterBase.ok())
                    occurrences.push_back(resolve(quarterBase));
            }

            return occurrences;
        }

        std::chrono::year_month_day const base = BaseDate(rule, targetYear, rule.Month);
        if (base.ok())
            occurrences.push_back(resolve(base));

        return occurrences;
    }

    Optional<time_t> GetWeeklyAnchor(HolidayRule const& rule, time_t referenceTime)
    {
        if (!rule.IsWeekly())
            return { };

        std::chrono::sys_days const today{ LocalDateOf(referenceTime) };
        std::chrono::sys_days const target = today - (std::chrono::weekday{ today } - rule.Weekday);

        time_t anchor = ToLocalTimestamp(std::chrono::year_month_day{ target }, rule.TimeOfDay);
        if (anchor > referenceTime)
            anchor = ToLocalTimestamp(std::chrono::year_month_day{ target - std::chrono::days{ 7 } }, rule.TimeOfDay);

        return anchor;
    }

    Optional<time_t> GetLocalWallClockAnchor(time_t originalStart, Minutes period, time_t referenceTime)
    {
        constexpr Minutes oneDay = std::chrono::duration_cast<Minutes>(std::chrono::days{ 1 });

        if (period <= Minutes::zero())
            return { };

        // Nothing to re-anchor yet, the event has not reached its first occurrence
        if (referenceTime <= originalStart)
            return originalStart;

        tm const original = TimeBreakdown(originalStart);
        Seconds const originalTimeOfDay = Hours(original.tm_hour) + Minutes(original.tm_min) + Seconds(original.tm_sec);

        // Shorter than a day, so the pattern repeats within every local day and only its phase has to be kept
        if (period < oneDay && oneDay % period == Minutes::zero())
        {
            time_t anchor = ToLocalTimestamp(LocalDateOf(referenceTime), originalTimeOfDay % period);
            while (anchor > referenceTime)
                anchor -= std::chrono::duration_cast<Seconds>(period).count();

            return anchor;
        }

        // A whole number of days, so both the local time of day and the day the cycle lands on have to be kept
        if (period % oneDay == Minutes::zero())
        {
            std::chrono::days const periodDays{ period / oneDay };
            std::chrono::sys_days const originalDay{ LocalDateOf(originalStart) };
            std::chrono::sys_days const referenceDay{ LocalDateOf(referenceTime) };

            std::chrono::days const elapsed = referenceDay - originalDay;
            std::chrono::sys_days anchorDay = originalDay + periodDays * (elapsed / periodDays);

            time_t anchor = ToLocalTimestamp(std::chrono::year_month_day{ anchorDay }, originalTimeOfDay);
            if (anchor > referenceTime)
                anchor = ToLocalTimestamp(std::chrono::year_month_day{ anchorDay - periodDays }, originalTimeOfDay);

            return anchor;
        }

        return { };
    }

    Optional<uint32> PackDate(time_t localTime)
    {
        tm const local = TimeBreakdown(localTime);
        std::chrono::year const packedYear{ local.tm_year + 1900 };

        if (packedYear < std::chrono::year{ 2000 } || packedYear > EG::HolidayPackedDateMaxYear)
            return { };

        WowTime packed;
        packed.SetYear(int(packedYear) - 2000);
        packed.SetMonth(int8(local.tm_mon));
        packed.SetMonthDay(int8(local.tm_mday - 1));
        packed.SetWeekDay(int8(local.tm_wday));
        packed.SetHour(int8(local.tm_hour));
        packed.SetMinute(int8(local.tm_min));
        packed.SetFlags(0);

        return packed.GetPackedTime();
    }
}
