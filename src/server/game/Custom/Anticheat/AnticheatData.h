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

#ifndef SC_ACDATA_H
#define SC_ACDATA_H

#include "Define.h"
#include "MovementInfo.h"

#define MAX_REPORT_TYPES 14

class AnticheatData
{
public:
    AnticheatData();
    ~AnticheatData();

    bool IsDirty() const { return _dirty; }

    void SetLastOpcode(uint32 opcode);
    uint32 GetLastOpcode() const;

    MovementInfo const& GetLastMovementInfo() const;
    void SetLastMovementInfo(MovementInfo const& moveInfo);

    uint32 GetLastRawMovementFlags() const;
    void SetLastRawMovementFlags(uint32 flags);

    void SetPosition(float x, float y, float z, float o);

    uint32 GetTotalReports() const;
    void SetTotalReports(uint32 totalReports);

    uint32 GetTypeReports(uint32 type) const;
    void SetTypeReports(uint32 type, uint32 amount);

    float GetAverage() const;
    void SetAverage(float average);

    uint32 GetCreationTime() const;
    void SetCreationTime(uint32 creationTime);

    void SetTempReports(uint32 amount, uint8 type);
    uint32 GetTempReports(uint8 type) const;

    void SetTempReportsTimer(uint32 time, uint8 type);
    uint32 GetTempReportsTimer(uint8 type) const;

    void SetDailyReportState(bool b);
    bool GetDailyReportState() const;

    void ResetReports();
    void ResetTempReports(uint8 type);

    uint32 AlertCounter;

private:
    static bool IsValidReportType(uint32 type) { return type < MAX_REPORT_TYPES; }

    bool _dirty;
    uint32 _lastOpcode;
    MovementInfo _lastMovementInfo;
    uint32 _lastRawMovementFlags;
    uint32 _totalReports;
    uint32 _typeReports[MAX_REPORT_TYPES];
    float _average;
    uint32 _creationTime;
    uint32 _tempReports[MAX_REPORT_TYPES];
    uint32 _tempReportsTimer[MAX_REPORT_TYPES];
    bool _hasDailyReport;
};

#endif
