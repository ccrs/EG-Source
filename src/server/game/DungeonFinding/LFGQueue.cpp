/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ObjectDefines.h"
#include "Containers.h"
#include "DBCStructure.h"
#include "DBCStores.h"
#include "GameTime.h"
#include "Group.h"
#include "LFGQueue.h"
#include "LFGMgr.h"
#include "Log.h"

namespace lfg
{

/**
   Given a list of guids returns the concatenation using | as delimiter

   @param[in]     check list of guids
   @returns Concatenated string
*/
std::string ConcatenateGuids(GuidList const& check)
{
    if (check.empty())
        return "";

    // need the guids in order to avoid duplicates
    GuidSet guids(check.begin(), check.end());

    std::ostringstream o;

    GuidSet::const_iterator it = guids.begin();
    o << it->GetRawValue();
    for (++it; it != guids.end(); ++it)
        o << '|' << it->GetRawValue();

    return o.str();
}

char const* GetCompatibleString(LfgCompatibility compatibles)
{
    switch (compatibles)
    {
        case LFG_COMPATIBILITY_PENDING:
            return "Pending";
        case LFG_COMPATIBLES_BAD_STATES:
            return "Compatibles (Bad States)";
        case LFG_COMPATIBLES_MATCH:
            return "Match";
        case LFG_COMPATIBLES_WITH_LESS_PLAYERS:
            return "Compatibles (Not enough players)";
        case LFG_INCOMPATIBLES_HAS_IGNORES:
            return "Has ignores";
        case LFG_INCOMPATIBLES_MULTIPLE_LFG_GROUPS:
            return "Multiple Lfg Groups";
        case LFG_INCOMPATIBLES_NO_DUNGEONS:
            return "Incompatible dungeons";
        case LFG_INCOMPATIBLES_NO_ROLES:
            return "Incompatible roles";
        case LFG_INCOMPATIBLES_TOO_MUCH_PLAYERS:
            return "Too many players";
        case LFG_INCOMPATIBLES_WRONG_GROUP_SIZE:
            return "Wrong group size";
        default:
            return "Unknown";
    }
}

LfgQueueData::LfgQueueData()
    : joinTime(GameTime::GetGameTime()), tanks(LFG_TANKS_NEEDED), healers(LFG_HEALERS_NEEDED), dps(LFG_DPS_NEEDED), statusDungeon(0) // EG
{
}

std::string LFGQueue::GetDetailedMatchRoles(GuidList const& check) const
{
    if (check.empty())
        return "";

    // need the guids in order to avoid duplicates
    GuidSet guids(check.begin(), check.end());

    std::ostringstream o;

    GuidSet::const_iterator it = guids.begin();
    o << it->GetRawValue();
    LfgQueueDataContainer::const_iterator itQueue = QueueDataStore.find(*it);
    if (itQueue != QueueDataStore.end())
    {
        // skip leader flag, log only dps/tank/healer
        auto role = itQueue->second.roles.find(*it);
        if (role != itQueue->second.roles.end())
            o << ' ' << GetRolesString(itQueue->second.roles.at(*it) & uint8(~PLAYER_ROLE_LEADER));
    }

    for (++it; it != guids.end(); ++it)
    {
        o << '|' << it->GetRawValue();
        itQueue = QueueDataStore.find(*it);
        if (itQueue != QueueDataStore.end())
        {
            // skip leader flag, log only dps/tank/healer
            auto role = itQueue->second.roles.find(*it);
            if (role != itQueue->second.roles.end())
                o << ' ' << GetRolesString(itQueue->second.roles.at(*it) & uint8(~PLAYER_ROLE_LEADER));
        }
    }

    return o.str();
}

void LFGQueue::AddToQueue(ObjectGuid guid, bool reAdd)
{
    LfgQueueDataContainer::iterator itQueue = QueueDataStore.find(guid);
    if (itQueue == QueueDataStore.end())
    {
        TC_LOG_ERROR("lfg.queue.add", "Queue data not found for [{}]", guid.ToString());
        return;
    }

    if (reAdd)
    {
        AddToFrontCurrentQueue(guid);
        // EG - only newcomers trigger matching, a readded entry would otherwise wait for one
        _rematchStore.remove(guid);
        _rematchStore.push_back(guid);
    }
    else
        AddToNewQueue(guid);
}

void LFGQueue::RemoveFromQueue(ObjectGuid guid)
{
    RemoveFromNewQueue(guid);
    RemoveFromCurrentQueue(guid);
    _rematchStore.remove(guid); // EG

    // Remove authoritative queue data before invalidating dependent cached/best-compatible
    // information, so recomputation cannot select the removed guid again.
    QueueDataStore.erase(guid);
    InvalidateCompatibleData(guid);
}

void LFGQueue::AddToNewQueue(ObjectGuid guid)
{
    WarnIfListed(guid, "AddToNewQueue");
    currentQueueStore.remove(guid);
    newToQueueStore.remove(guid);
    newToQueueStore.push_back(guid);
}

void LFGQueue::RemoveFromNewQueue(ObjectGuid guid)
{
    newToQueueStore.remove(guid);
}

void LFGQueue::AddToCurrentQueue(ObjectGuid guid)
{
    newToQueueStore.remove(guid);
    currentQueueStore.remove(guid);
    currentQueueStore.push_back(guid);
}

void LFGQueue::AddToFrontCurrentQueue(ObjectGuid guid)
{
    WarnIfListed(guid, "AddToFrontCurrentQueue");
    newToQueueStore.remove(guid);
    currentQueueStore.remove(guid);
    currentQueueStore.push_front(guid);
}

void LFGQueue::WarnIfListed(ObjectGuid guid, char const* context) const
{
    if (std::find(newToQueueStore.begin(), newToQueueStore.end(), guid) != newToQueueStore.end()
        || std::find(currentQueueStore.begin(), currentQueueStore.end(), guid) != currentQueueStore.end())
        TC_LOG_WARN("lfg.queue.add", "{}: [{}] is already listed in the queue - untraced double-add path, report this", context, guid.ToString());
}

void LFGQueue::RemoveFromCurrentQueue(ObjectGuid guid)
{
    currentQueueStore.remove(guid);
}

void LFGQueue::AddQueueData(ObjectGuid guid, time_t joinTime, LfgDungeonSet const& dungeons, LfgRolesMap const& rolesMap, uint32 statusDungeon)
{
    // EG - replacing queue data for the same guid invalidates every cached [compatibility result and best-compatible status]
    // that involved the previous roles/dungeon set/group composition, so erase the old data before invalidating to prevent recomputation from picking up stale data for this guid
    RemoveFromNewQueue(guid);
    RemoveFromCurrentQueue(guid);
    _rematchStore.remove(guid); // EG
    QueueDataStore.erase(guid);
    InvalidateCompatibleData(guid);

    QueueDataStore[guid] = LfgQueueData(joinTime, dungeons, rolesMap, statusDungeon); // EG
    // EG - the queue status must always count the entry's own roles
    ResetBestCompatibleToSelf(QueueDataStore.find(guid));
    AddToQueue(guid);
}

void LFGQueue::RemoveQueueData(ObjectGuid guid)
{
    // Keep this safe even if callers bypass RemoveFromQueue().
    RemoveFromNewQueue(guid);
    RemoveFromCurrentQueue(guid);
    _rematchStore.remove(guid); // EG

    QueueDataStore.erase(guid);
    InvalidateCompatibleData(guid);
}

bool LFGQueue::IsQueued(ObjectGuid guid) const
{
    return QueueDataStore.find(guid) != QueueDataStore.end();
}

void LFGQueue::UpdateStatusNeeds(LfgQueueDataContainer::iterator itQueue)
{
    LfgQueueData& queueData = itQueue->second;
    LfgRolesMap roles = queueData.roles;
    LfgDungeonSet dungeons = queueData.dungeons;
    bool hasLfgGroup = sLFGMgr->IsLfgGroup(itQueue->first);

    for (ObjectGuid const& guid : currentQueueStore)
    {
        if (roles.size() >= MAX_GROUP_SIZE)
            break;

        if (guid == itQueue->first)
            continue;

        LfgQueueDataContainer::const_iterator itOther = QueueDataStore.find(guid);
        if (itOther == QueueDataStore.end())
            continue;

        LfgQueueData const& other = itOther->second;
        if (roles.size() + other.roles.size() > MAX_GROUP_SIZE)
            continue;

        if (hasLfgGroup && sLFGMgr->IsLfgGroup(guid))
            continue;

        // EG - cheap checks first, ignores only for a merge that can stand
        if (std::none_of(other.dungeons.begin(), other.dungeons.end(), [&dungeons](uint32 dungeonId) { return dungeons.count(dungeonId) != 0; }))
            continue;

        LfgRolesMap merged = roles;
        bool duplicate = false;
        for (LfgRolesMap::const_iterator itRole = other.roles.begin(); itRole != other.roles.end() && !duplicate; ++itRole)
        {
            duplicate = roles.count(itRole->first) != 0;
            merged[itRole->first] = itRole->second;
        }

        if (duplicate)
            continue;

        LfgRolesMap assigned = merged;
        if (!LFGMgr::CheckGroupRoles(assigned))
            continue;

        bool ignored = false;
        for (LfgRolesMap::const_iterator itRole = other.roles.begin(); itRole != other.roles.end() && !ignored; ++itRole)
            for (LfgRolesMap::const_iterator itMember = roles.begin(); itMember != roles.end() && !ignored; ++itMember)
                ignored = sLFGMgr->HasIgnore(itRole->first, itMember->first);

        if (ignored)
            continue;

        LfgDungeonSet common;
        std::set_intersection(dungeons.begin(), dungeons.end(), other.dungeons.begin(), other.dungeons.end(), std::inserter(common, common.begin()));

        roles = merged;
        dungeons = common;
        hasLfgGroup = hasLfgGroup || sLFGMgr->IsLfgGroup(guid);
    }

    queueData.tanks = LFG_TANKS_NEEDED;
    queueData.healers = LFG_HEALERS_NEEDED;
    queueData.dps = LFG_DPS_NEEDED;

    if (!LFGMgr::CheckGroupRoles(roles))
        return;

    for (LfgRolesMap::const_iterator itRole = roles.begin(); itRole != roles.end(); ++itRole)
    {
        if (itRole->second & PLAYER_ROLE_TANK)
            --queueData.tanks;
        else if (itRole->second & PLAYER_ROLE_HEALER)
            --queueData.healers;
        else
            --queueData.dps;
    }
}

GuidList LFGQueue::GetQueuedGuids() const
{
    GuidList guids;
    for (LfgQueueDataContainer::const_iterator itr = QueueDataStore.begin(); itr != QueueDataStore.end(); ++itr)
        guids.push_back(itr->first);

    return guids;
}

std::map<ObjectGuid, LfgLockPartyMap> LFGQueue::TakeLockedOutEntries()
{
    std::map<ObjectGuid, LfgLockPartyMap> lockedOut;
    lockedOut.swap(_lockedOut);
    return lockedOut;
}

void LFGQueue::UpdateWaitTimeAvg(int32 waitTime, uint32 dungeonId)
{
    LfgWaitTime& wt = waitTimesAvgStore[dungeonId];
    uint32 old_number = wt.number++;
    wt.time = int32((wt.time * old_number + waitTime) / wt.number);
}

void LFGQueue::UpdateWaitTimeTank(int32 waitTime, uint32 dungeonId)
{
    LfgWaitTime& wt = waitTimesTankStore[dungeonId];
    uint32 old_number = wt.number++;
    wt.time = int32((wt.time * old_number + waitTime) / wt.number);
}

void LFGQueue::UpdateWaitTimeHealer(int32 waitTime, uint32 dungeonId)
{
    LfgWaitTime& wt = waitTimesHealerStore[dungeonId];
    uint32 old_number = wt.number++;
    wt.time = int32((wt.time * old_number + waitTime) / wt.number);
}

void LFGQueue::UpdateWaitTimeDps(int32 waitTime, uint32 dungeonId)
{
    LfgWaitTime& wt = waitTimesDpsStore[dungeonId];
    uint32 old_number = wt.number++;
    wt.time = int32((wt.time * old_number + waitTime) / wt.number);
}

/**
   Remove from cached compatible dungeons any entry that contains the given guid

   @param[in]     guid Guid to remove from compatible cache
*/
void LFGQueue::RemoveFromCompatibles(ObjectGuid guid)
{
    TC_LOG_DEBUG("lfg.queue.data.compatibles.remove", "Removing {}", guid.ToString());
    for (LfgCompatibleContainer::iterator itNext = CompatibleMapStore.begin(); itNext != CompatibleMapStore.end();)
    {
        LfgCompatibleContainer::iterator it = itNext++;
        if (CompatibleKeyContainsGuid(it->first, guid))
            CompatibleMapStore.erase(it);
    }
}

void LFGQueue::InvalidateCompatibleData(ObjectGuid guid)
{
    RemoveFromCompatibles(guid);

    for (LfgQueueDataContainer::iterator itr = QueueDataStore.begin(); itr != QueueDataStore.end(); ++itr)
    {
        if (CompatibleKeyContainsGuid(itr->second.bestCompatible, guid))
        {
            // EG - the per-pass cache rarely still holds the entry's own result, rebuild it directly
            ResetBestCompatibleToSelf(itr);
            FindBestCompatibleInQueue(itr);
        }
    }
}

/**
   Stores the compatibility of a list of guids

   @param[in]     key String concatenation of guids (| used as separator)
   @param[in]     compatibles type of compatibility
*/
void LFGQueue::SetCompatibles(std::string const& key, LfgCompatibility compatibles)
{
    LfgCompatibilityData& data = CompatibleMapStore[key];
    data.compatibility = compatibles;
}

void LFGQueue::SetCompatibilityData(std::string const& key, LfgCompatibilityData const& data)
{
    CompatibleMapStore[key] = data;
}

/**
   Get the compatibility of a group of guids

   @param[in]     key String concatenation of guids (| used as separator)
   @return LfgCompatibility type of compatibility
*/
LfgCompatibility LFGQueue::GetCompatibles(std::string const& key)
{
    LfgCompatibleContainer::iterator itr = CompatibleMapStore.find(key);
    if (itr != CompatibleMapStore.end())
        return itr->second.compatibility;

    return LFG_COMPATIBILITY_PENDING;
}

LfgCompatibilityData* LFGQueue::GetCompatibilityData(std::string const& key)
{
    LfgCompatibleContainer::iterator itr = CompatibleMapStore.find(key);
    if (itr != CompatibleMapStore.end())
        return &(itr->second);

    return nullptr;
}

uint8 LFGQueue::FindGroups()
{
    // Scope the cache to this call.
    CompatibleMapStore.clear();
    if (CompatibleMapStore.bucket_count() < 4096)
        CompatibleMapStore.reserve(4096);

    uint8 proposals = 0;

    // EG - match readded entries against the rest of the queue without moving them from their place
    while (!_rematchStore.empty())
    {
        ObjectGuid guid = _rematchStore.front();
        _rematchStore.pop_front();

        if (std::find(currentQueueStore.begin(), currentQueueStore.end(), guid) == currentQueueStore.end())
            continue;

        TC_LOG_DEBUG("lfg.queue.match.check.new", "Rechecking readded [{}] currentQueue({})", guid.ToString(), uint32(currentQueueStore.size()));

        GuidList check(1, guid);
        GuidList others;
        for (ObjectGuid const& other : currentQueueStore)
            if (other != guid)
                others.push_back(other);

        if (FindNewGroups(check, others) == LFG_COMPATIBLES_MATCH)
            ++proposals;
    }

    GuidList firstNew;
    while (!newToQueueStore.empty())
    {
        ObjectGuid frontguid = newToQueueStore.front();
        TC_LOG_DEBUG("lfg.queue.match.check.new", "Checking [{}] newToQueue({}), currentQueue({})", frontguid.ToString(),
            uint32(newToQueueStore.size()), uint32(currentQueueStore.size()));

        firstNew.clear();
        firstNew.push_back(frontguid);
        RemoveFromNewQueue(frontguid);

        GuidList temporalList = currentQueueStore;
        LfgCompatibility compatibles = FindNewGroups(firstNew, temporalList);
        if (compatibles == LFG_COMPATIBLES_MATCH)
            ++proposals;
        else
            AddToCurrentQueue(frontguid); // Lfg group not found, add this group to the queue.
    }

    return proposals;
}

/**
   Checks que main queue to try to form a Lfg group. Returns first match found (if any)

   @param[in]     check List of guids trying to match with other groups
   @param[in]     all List of all other guids in main queue to match against
   @return LfgCompatibility type of compatibility between groups
*/
LfgCompatibility LFGQueue::FindNewGroups(GuidList& check, GuidList& all)
{
    // EG - pruned match search
    LfgDungeonSet dungeons;
    LfgRoleCoverage coverage;
    bool hasLfgGroup = false;
    bool first = true;
    for (ObjectGuid const& guid : check)
    {
        LfgQueueDataContainer::const_iterator itQueue = QueueDataStore.find(guid);
        if (itQueue == QueueDataStore.end())
            continue;

        if (first)
            dungeons = itQueue->second.dungeons;
        else
        {
            LfgDungeonSet common;
            std::set_intersection(dungeons.begin(), dungeons.end(), itQueue->second.dungeons.begin(), itQueue->second.dungeons.end(), std::inserter(common, common.begin()));
            dungeons = common;
        }

        first = false;
        coverage.Add(itQueue->second.roles);
        hasLfgGroup = hasLfgGroup || sLFGMgr->IsLfgGroup(guid);
    }

    _remainingChecks = LFG_MATCH_CHECK_BUDGET;
    LfgCompatibility compatibles = FindNewGroups(check, all, dungeons, coverage, hasLfgGroup);
    if (compatibles != LFG_COMPATIBLES_MATCH && !_remainingChecks)
        TC_LOG_WARN("lfg.queue.match.check", "Match search for [{}] stopped after {} compatibility checks, queue has {} entries - report this", check.front().ToString(), uint32(LFG_MATCH_CHECK_BUDGET), uint32(currentQueueStore.size()));

    return compatibles;
}

LfgCompatibility LFGQueue::FindNewGroups(GuidList& check, GuidList const& all, LfgDungeonSet const& dungeons, LfgRoleCoverage const& coverage, bool hasLfgGroup)
{
    std::string strGuids = ConcatenateGuids(check);
    LfgCompatibility compatibles = GetCompatibles(strGuids);

    TC_LOG_DEBUG("lfg.queue.match.check", "Guids: ({}): {} - all({})",
        GetDetailedMatchRoles(check), GetCompatibleString(compatibles), GetDetailedMatchRoles(all));

    if (compatibles == LFG_COMPATIBILITY_PENDING)
        compatibles = CheckCompatibility(check);
    else if (compatibles == LFG_COMPATIBLES_MATCH || compatibles == LFG_COMPATIBLES_BAD_STATES)
    {
        // EG - a cached MATCH cannot be reused as a result because proposal creation happens inside CheckCompatibility()
        // --> returning it directly would make FindGroups() count a proposal that was never created,
        // and BAD_STATES can become valid again when proposal/rolecheck state changes, so both need recomputation instead of being returned directly
        TC_LOG_DEBUG("lfg.queue.match.check", "Guids: ({}) cached {} is being recalculated", GetDetailedMatchRoles(check), GetCompatibleString(compatibles));
        compatibles = CheckCompatibility(check);
    }

    if (compatibles != LFG_COMPATIBLES_WITH_LESS_PLAYERS)
        return compatibles;

    // EG
    if (!_remainingChecks)
        return compatibles;

    // EG - pruned match search
    std::vector<ObjectGuid> candidates;
    std::vector<LfgQueueData const*> candidateData;
    std::vector<LfgRoleCoverage> candidateCoverage;
    for (ObjectGuid const& guid : all)
    {
        LfgQueueDataContainer::const_iterator itQueue = QueueDataStore.find(guid);
        if (itQueue == QueueDataStore.end())
        {
            TC_LOG_ERROR("lfg.queue.match.compatibility.check", "Guid: [{}] is not queued but listed as queued!", guid.ToString());
            RemoveFromQueue(guid);
            continue;
        }

        LfgQueueData const& queueData = itQueue->second;
        if (coverage.Players + queueData.roles.size() > MAX_GROUP_SIZE)
            continue;

        if (hasLfgGroup && sLFGMgr->IsLfgGroup(guid))
            continue;

        if (std::none_of(queueData.dungeons.begin(), queueData.dungeons.end(), [&dungeons](uint32 dungeonId) { return dungeons.count(dungeonId) != 0; }))
            continue;

        LfgRoleCoverage roles;
        roles.Add(queueData.roles);
        candidates.push_back(guid);
        candidateData.push_back(&queueData);
        candidateCoverage.push_back(roles);
    }

    // EG - pruned match search
    std::vector<uint32> dungeonIds(dungeons.begin(), dungeons.end());
    std::vector<std::vector<LfgRoleCoverage>> suffix(dungeonIds.size(), std::vector<LfgRoleCoverage>(candidates.size() + 1));
    for (std::size_t d = 0; d < dungeonIds.size(); ++d)
    {
        for (std::size_t i = candidates.size(); i > 0; --i)
        {
            suffix[d][i - 1] = suffix[d][i];
            if (candidateData[i - 1]->dungeons.count(dungeonIds[d]))
                suffix[d][i - 1].Add(candidateCoverage[i - 1]);
        }
    }

    for (std::size_t i = 0; i < candidates.size() && _remainingChecks; ++i)
    {
        bool reachable = false;
        for (std::size_t d = 0; d < dungeonIds.size() && !reachable; ++d)
            reachable = coverage.CanCompleteGroupWith(suffix[d][i]);

        if (!reachable)
            break;

        LfgRoleCoverage nextCoverage = coverage;
        nextCoverage.Add(candidateCoverage[i]);

        LfgDungeonSet nextDungeons;
        std::set_intersection(dungeons.begin(), dungeons.end(), candidateData[i]->dungeons.begin(), candidateData[i]->dungeons.end(), std::inserter(nextDungeons, nextDungeons.begin()));

        bool completable = false;
        for (std::size_t d = 0; d < dungeonIds.size() && !completable; ++d)
            completable = nextDungeons.count(dungeonIds[d]) && nextCoverage.CanCompleteGroupWith(suffix[d][i + 1]);

        if (!completable)
            continue;

        check.push_back(candidates[i]);

        GuidList remaining(candidates.begin() + i + 1, candidates.end());
        LfgCompatibility subcompatibility = FindNewGroups(check, remaining, nextDungeons, nextCoverage, hasLfgGroup || sLFGMgr->IsLfgGroup(candidates[i]));
        if (subcompatibility == LFG_COMPATIBLES_MATCH)
            return LFG_COMPATIBLES_MATCH;

        check.pop_back();
    }

    return compatibles;
}

/**
   Check compatibilities between groups. If group is Matched proposal will be created

   @param[in]     check List of guids to check compatibilities
   @return LfgCompatibility type of compatibility
*/
LfgCompatibility LFGQueue::CheckCompatibility(GuidList check)
{
    // EG
    if (_remainingChecks)
        --_remainingChecks;

    std::string strGuids = ConcatenateGuids(check);
    LfgProposal proposal;
    LfgDungeonSet proposalDungeons;
    LfgGroupsMap proposalGroups;
    LfgRolesMap proposalRoles;

    // Check for correct size
    if (check.size() > MAX_GROUP_SIZE || check.empty())
    {
        TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}): Size wrong - Not compatibles", GetDetailedMatchRoles(check));
        return LFG_INCOMPATIBLES_WRONG_GROUP_SIZE;
    }

    // Check all-but-new compatibility
    if (check.size() > 2)
    {
        ObjectGuid frontGuid = check.front();
        check.pop_front();

        // Check all-but-new compatibilities (New, A, B, C, D) --> check(A, B, C, D)
        LfgCompatibility child_compatibles = CheckCompatibility(check);
        if (child_compatibles < LFG_COMPATIBLES_WITH_LESS_PLAYERS) // Group not compatible
        {
            TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) child {} not compatibles", strGuids, GetDetailedMatchRoles(check));
            SetCompatibles(strGuids, child_compatibles);
            return child_compatibles;
        }
        check.push_front(frontGuid);
    }

    // Check if more than one LFG group and number of players joining
    uint8 numPlayers = 0;
    uint8 numLfgGroups = 0;
    for (GuidList::const_iterator it = check.begin(); it != check.end() && numLfgGroups < 2 && numPlayers <= MAX_GROUP_SIZE; ++it)
    {
        ObjectGuid guid = *it;
        LfgQueueDataContainer::iterator itQueue = QueueDataStore.find(guid);
        if (itQueue == QueueDataStore.end())
        {
            TC_LOG_ERROR("lfg.queue.match.compatibility.check", "Guid: [{}] is not queued but listed as queued!", guid.ToString());
            RemoveFromQueue(guid);
            return LFG_COMPATIBILITY_PENDING;
        }

        // Store group so we don't need to call Mgr to get it later (if it's player group will be 0 otherwise would have joined as group)
        for (LfgRolesMap::const_iterator it2 = itQueue->second.roles.begin(); it2 != itQueue->second.roles.end(); ++it2)
            proposalGroups[it2->first] = itQueue->first.IsGroup() ? itQueue->first : ObjectGuid::Empty;

        numPlayers += itQueue->second.roles.size();

        if (sLFGMgr->IsLfgGroup(guid))
        {
            if (!numLfgGroups)
                proposal.group = guid;
            ++numLfgGroups;
        }
    }

    // Group with less than MAX_GROUP_SIZE members always compatible
    if (check.size() == 1 && numPlayers != MAX_GROUP_SIZE)
    {
        TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) single group. Compatibles", GetDetailedMatchRoles(check));
        LfgQueueDataContainer::iterator itQueue = QueueDataStore.find(check.front());

        LfgCompatibilityData data(LFG_COMPATIBLES_WITH_LESS_PLAYERS);
        data.roles = itQueue->second.roles;
        LFGMgr::CheckGroupRoles(data.roles);

        UpdateBestCompatibleInQueue(itQueue, strGuids, data.roles);
        SetCompatibilityData(strGuids, data);
        return LFG_COMPATIBLES_WITH_LESS_PLAYERS;
    }

    if (numLfgGroups > 1)
    {
        TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) More than one Lfggroup ({})", GetDetailedMatchRoles(check), numLfgGroups);
        SetCompatibles(strGuids, LFG_INCOMPATIBLES_MULTIPLE_LFG_GROUPS);
        return LFG_INCOMPATIBLES_MULTIPLE_LFG_GROUPS;
    }

    if (numPlayers > MAX_GROUP_SIZE)
    {
        TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) Too many players ({})", GetDetailedMatchRoles(check), numPlayers);
        SetCompatibles(strGuids, LFG_INCOMPATIBLES_TOO_MUCH_PLAYERS);
        return LFG_INCOMPATIBLES_TOO_MUCH_PLAYERS;
    }

    // If it's single group no need to check for duplicate players, ignores, bad roles or bad dungeons as it's been checked before joining
    if (check.size() > 1)
    {
        for (GuidList::const_iterator it = check.begin(); it != check.end(); ++it)
        {
            LfgRolesMap const& roles = QueueDataStore[(*it)].roles;
            for (LfgRolesMap::const_iterator itRoles = roles.begin(); itRoles != roles.end(); ++itRoles)
            {
                LfgRolesMap::const_iterator itPlayer;
                for (itPlayer = proposalRoles.begin(); itPlayer != proposalRoles.end(); ++itPlayer)
                {
                    if (itRoles->first == itPlayer->first)
                        TC_LOG_ERROR("lfg.queue.match.compatibility.check", "Guids: ERROR! Player multiple times in queue! Player [{}] re-introduced by queue entity [{}] in combo ({})", itRoles->first.ToString(), (*it).ToString(), GetDetailedMatchRoles(check));
                    else if (sLFGMgr->HasIgnore(itRoles->first, itPlayer->first))
                        break;
                }
                if (itPlayer == proposalRoles.end())
                    proposalRoles[itRoles->first] = itRoles->second;
            }
        }

        if (uint8 playersize = numPlayers - proposalRoles.size())
        {
            TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) not compatible, {} players are ignoring each other", GetDetailedMatchRoles(check), playersize);
            SetCompatibles(strGuids, LFG_INCOMPATIBLES_HAS_IGNORES);
            return LFG_INCOMPATIBLES_HAS_IGNORES;
        }

        LfgRolesMap debugRoles = proposalRoles;
        if (!LFGMgr::CheckGroupRoles(proposalRoles))
        {
            std::ostringstream o;
            for (LfgRolesMap::const_iterator it = debugRoles.begin(); it != debugRoles.end(); ++it)
                o << ", " << it->first.GetRawValue() << ": " << GetRolesString(it->second);

            TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) Roles not compatible{}", GetDetailedMatchRoles(check), o.str());
            SetCompatibles(strGuids, LFG_INCOMPATIBLES_NO_ROLES);
            return LFG_INCOMPATIBLES_NO_ROLES;
        }

        GuidList::iterator itguid = check.begin();
        proposalDungeons = QueueDataStore[*itguid].dungeons;
        std::ostringstream o;
        o << ", " << itguid->GetRawValue() << ": (" << ConcatenateDungeons(proposalDungeons) << ")";
        for (++itguid; itguid != check.end(); ++itguid)
        {
            LfgDungeonSet temporal;
            LfgDungeonSet& dungeons = QueueDataStore[*itguid].dungeons;
            o << ", " << itguid->GetRawValue() << ": (" << ConcatenateDungeons(dungeons) << ")";
            std::set_intersection(proposalDungeons.begin(), proposalDungeons.end(), dungeons.begin(), dungeons.end(), std::inserter(temporal, temporal.begin()));
            proposalDungeons = temporal;
        }

        if (proposalDungeons.empty())
        {
            TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) No compatible dungeons{}", GetDetailedMatchRoles(check), o.str());
            SetCompatibles(strGuids, LFG_INCOMPATIBLES_NO_DUNGEONS);
            return LFG_INCOMPATIBLES_NO_DUNGEONS;
        }
    }
    else
    {
        ObjectGuid gguid = *check.begin();
        LfgQueueData const& queue = QueueDataStore[gguid];
        proposalDungeons = queue.dungeons;
        proposalRoles = queue.roles;
        LFGMgr::CheckGroupRoles(proposalRoles); // assign new roles

        if (proposalDungeons.empty())
        {
            TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) No dungeons available", GetDetailedMatchRoles(check));
            SetCompatibles(strGuids, LFG_INCOMPATIBLES_NO_DUNGEONS);
            return LFG_INCOMPATIBLES_NO_DUNGEONS;
        }
    }

    // Enough players?
    if (numPlayers != MAX_GROUP_SIZE)
    {
        TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) Compatibles but not enough players({})", GetDetailedMatchRoles(check), numPlayers);
        LfgCompatibilityData data(LFG_COMPATIBLES_WITH_LESS_PLAYERS);
        data.roles = proposalRoles;

        for (GuidList::const_iterator itr = check.begin(); itr != check.end(); ++itr)
            UpdateBestCompatibleInQueue(QueueDataStore.find(*itr), strGuids, data.roles);

        SetCompatibilityData(strGuids, data);
        return LFG_COMPATIBLES_WITH_LESS_PLAYERS;
    }

    proposal.queues = check;
    // EG - check.front() is the newest queue entry, not the matched group, so read the state of the group that was actually matched
    proposal.isNew = numLfgGroups != 1 || sLFGMgr->GetOldState(proposal.group) != LFG_STATE_DUNGEON;

    if (!sLFGMgr->AllQueued(check))
    {
        TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) Group MATCH but can't create proposal!", GetDetailedMatchRoles(check));
        SetCompatibles(strGuids, LFG_COMPATIBLES_BAD_STATES);
        return LFG_COMPATIBLES_BAD_STATES;
    }

    // Re-validate instance saves. QueueDataStore dungeons are set at join time and never refreshed,
    // so a player who acquired a heroic save after joining can still appear eligible for that dungeon.
    {
        GuidSet playerGuids;
        for (LfgRolesMap::const_iterator it = proposalRoles.begin(); it != proposalRoles.end(); ++it)
            playerGuids.insert(it->first);

        LfgDungeonSet filteredDungeons = proposalDungeons;
        LfgLockPartyMap lockUpdates;
        sLFGMgr->GetCompatibleDungeons(filteredDungeons, playerGuids, lockUpdates, !proposal.isNew);

        if (filteredDungeons.size() < proposalDungeons.size())
        {
            proposalDungeons = filteredDungeons;

            if (proposalDungeons.empty())
            {
                // GetCompatibleDungeons only populates lockUpdates when all dungeons are eliminated,
                // which is exactly this case. Use it to prune the affected queue entries so the next
                // tick produces a correct intersection instead of hitting BAD_STATES repeatedly.
                for (LfgLockPartyMap::const_iterator itLock = lockUpdates.begin(); itLock != lockUpdates.end(); ++itLock)
                {
                    LfgGroupsMap::const_iterator itGroup = proposalGroups.find(itLock->first);
                    ObjectGuid queueGuid = (itGroup != proposalGroups.end() && !itGroup->second.IsEmpty())
                        ? itGroup->second : itLock->first;

                    // EG - the backfilled group's own saves were accepted above, never prune its entry for them
                    if (!proposal.isNew && queueGuid == proposal.group)
                        continue;

                    LfgQueueDataContainer::iterator itData = QueueDataStore.find(queueGuid);
                    if (itData == QueueDataStore.end())
                        continue;

                    bool changed = false;
                    for (LfgLockMap::const_iterator itDungeon = itLock->second.begin(); itDungeon != itLock->second.end(); ++itDungeon)
                    {
                        uint32 dungeonId = itDungeon->first & 0x00FFFFFF;
                        changed |= (itData->second.dungeons.erase(dungeonId) > 0);
                    }

                    if (changed)
                    {
                        InvalidateCompatibleData(queueGuid);
                        // EG
                        if (itData->second.dungeons.empty())
                            _lockedOut[queueGuid][itLock->first] = itLock->second;
                    }
                }

                TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) MATCH cancelled: all dungeons locked by saves acquired after queue join", GetDetailedMatchRoles(check));
                SetCompatibles(strGuids, LFG_COMPATIBLES_BAD_STATES);
                return LFG_COMPATIBLES_BAD_STATES;
            }
        }
    }

    // Create a new proposal
    proposal.cancelTime = GameTime::GetGameTime() + LFG_TIME_PROPOSAL;
    proposal.state = LFG_PROPOSAL_INITIATING;
    proposal.leader.Clear();
    proposal.dungeonId = Trinity::Containers::SelectRandomContainerElement(proposalDungeons);

    bool leader = false;
    for (LfgRolesMap::const_iterator itRoles = proposalRoles.begin(); itRoles != proposalRoles.end(); ++itRoles)
    {
        // Assign new leader
        if (itRoles->second & PLAYER_ROLE_LEADER)
        {
            if (!leader || !proposal.leader || urand(0, 1))
                proposal.leader = itRoles->first;
            leader = true;
        }
        else if (!leader && (!proposal.leader || urand(0, 1)))
            proposal.leader = itRoles->first;

        // Assign player data and roles
        LfgProposalPlayer& data = proposal.players[itRoles->first];
        data.role = itRoles->second;
        data.group = proposalGroups.find(itRoles->first)->second;
        if (!proposal.isNew && !data.group.IsEmpty() && data.group == proposal.group) // Player from existing group, autoaccept
            data.accept = LFG_ANSWER_AGREE;
    }

    // Mark proposal members as not queued (but not remove queue data)
    for (GuidList::const_iterator itQueue = proposal.queues.begin(); itQueue != proposal.queues.end(); ++itQueue)
    {
        ObjectGuid guid = (*itQueue);
        RemoveFromNewQueue(guid);
        RemoveFromCurrentQueue(guid);
    }

    sLFGMgr->AddProposal(proposal);

    TC_LOG_DEBUG("lfg.queue.match.compatibility.check", "Guids: ({}) MATCH! Group formed", GetDetailedMatchRoles(check));
    SetCompatibles(strGuids, LFG_COMPATIBLES_MATCH);
    return LFG_COMPATIBLES_MATCH;
}

void LFGQueue::UpdateQueueTimers(time_t currTime)
{
    TC_LOG_TRACE("lfg.queue.timers.update", "Updating queue timers...");
    GuidList orphans;
    for (LfgQueueDataContainer::iterator itQueue = QueueDataStore.begin(); itQueue != QueueDataStore.end(); ++itQueue)
    {
        LfgQueueData& queueinfo = itQueue->second;

        // EG - an entry whose owner is neither queued nor in a proposal is orphaned, drop it instead of reporting it to its former members
        LfgState state = sLFGMgr->GetState(itQueue->first);
        if (state != LFG_STATE_QUEUED && state != LFG_STATE_PROPOSAL)
        {
            TC_LOG_ERROR("lfg.queue.timers.update", "Orphaned queue entry [{}] in state {} removed, its owner left the queue without dequeuing - report this", itQueue->first.ToString(), GetStateString(state));
            orphans.push_back(itQueue->first);
            continue;
        }

        if (queueinfo.dungeons.empty())
            continue;

        uint32 dungeonId = queueinfo.statusDungeon; // EG
        uint32 queuedTime = uint32(currTime - queueinfo.joinTime);
        uint8 role = PLAYER_ROLE_NONE;
        int32 waitTime = -1;
        int32 wtTank = waitTimesTankStore[dungeonId].time;
        int32 wtHealer = waitTimesHealerStore[dungeonId].time;
        int32 wtDps = waitTimesDpsStore[dungeonId].time;
        int32 wtAvg = waitTimesAvgStore[dungeonId].time;

        for (LfgRolesMap::const_iterator itPlayer = queueinfo.roles.begin(); itPlayer != queueinfo.roles.end(); ++itPlayer)
            role |= itPlayer->second;
        role &= ~PLAYER_ROLE_LEADER;

        switch (role)
        {
            case PLAYER_ROLE_NONE: // Should not happen - just in case
                waitTime = -1;
                break;
            case PLAYER_ROLE_TANK:
                waitTime = wtTank;
                break;
            case PLAYER_ROLE_HEALER:
                waitTime = wtHealer;
                break;
            case PLAYER_ROLE_DAMAGE:
                waitTime = wtDps;
                break;
            default:
                waitTime = wtAvg;
                break;
        }

        UpdateStatusNeeds(itQueue); // EG

        LfgQueueStatusData queueData(dungeonId, waitTime, wtAvg, wtTank, wtHealer, wtDps, queuedTime, queueinfo.tanks, queueinfo.healers, queueinfo.dps);
        for (LfgRolesMap::const_iterator itPlayer = queueinfo.roles.begin(); itPlayer != queueinfo.roles.end(); ++itPlayer)
        {
            ObjectGuid pguid = itPlayer->first;
            LFGMgr::SendLfgQueueStatus(pguid, queueData);
        }
    }

    for (ObjectGuid const& guid : orphans)
        RemoveFromQueue(guid);
}

time_t LFGQueue::GetJoinTime(ObjectGuid guid) const
{
    LfgQueueDataContainer::const_iterator itr = QueueDataStore.find(guid);
    if (itr != QueueDataStore.end())
        return itr->second.joinTime;

    return 0;
}

uint32 LFGQueue::GetStatusDungeon(ObjectGuid guid) const
{
    LfgQueueDataContainer::const_iterator itr = QueueDataStore.find(guid);
    if (itr != QueueDataStore.end())
        return itr->second.statusDungeon;

    return 0;
}

std::string LFGQueue::DumpQueueInfo() const
{
    uint32 players = 0;
    uint32 groups = 0;
    uint32 playersInGroup = 0;

    for (uint8 i = 0; i < 2; ++i)
    {
        GuidList const& queue = i ? newToQueueStore : currentQueueStore;
        for (GuidList::const_iterator it = queue.begin(); it != queue.end(); ++it)
        {
            ObjectGuid guid = *it;
            if (guid.IsGroup())
            {
                groups++;
                playersInGroup += sLFGMgr->GetPlayerCount(guid);
            }
            else
                players++;
        }
    }
    std::ostringstream o;
    o << "Queued Players: " << players << " (in group: " << playersInGroup << ") Groups: " << groups << "\n";
    return o.str();
}

std::string LFGQueue::DumpCompatibleInfo(bool full /* = false */) const
{
    std::ostringstream o;
    o << "Compatible Map size: " << CompatibleMapStore.size() << "\n";
    if (full)
    {
        for (LfgCompatibleContainer::const_iterator itr = CompatibleMapStore.begin(); itr != CompatibleMapStore.end(); ++itr)
        {
            o << "(" << itr->first << "): " << GetCompatibleString(itr->second.compatibility);
            if (!itr->second.roles.empty())
            {
                o << " (";
                bool first = true;
                for (auto const& role : itr->second.roles)
                {
                    if (!first)
                        o << "|";
                    o << role.first.GetRawValue() << " " << GetRolesString(role.second & uint8(~PLAYER_ROLE_LEADER));
                    first = false;
                }
                o << ")";
            }
            o << "\n";
        }
    }

    return o.str();
}

void LFGQueue::FindBestCompatibleInQueue(LfgQueueDataContainer::iterator itrQueue)
{
    TC_LOG_DEBUG("lfg.queue.compatibles.find", "{}", itrQueue->first.ToString());

    for (LfgCompatibleContainer::const_iterator itr = CompatibleMapStore.begin(); itr != CompatibleMapStore.end(); ++itr)
    {
        if (itr->second.compatibility == LFG_COMPATIBLES_WITH_LESS_PLAYERS && CompatibleKeyContainsGuid(itr->first, itrQueue->first))
            UpdateBestCompatibleInQueue(itrQueue, itr->first, itr->second.roles);
    }
}

void LFGQueue::UpdateBestCompatibleInQueue(LfgQueueDataContainer::iterator itrQueue, std::string const& key, LfgRolesMap const& roles)
{
    if (itrQueue == QueueDataStore.end())
        return;

    LfgQueueData& queueData = itrQueue->second;
    uint8 storedSize = CountGuidsInCompatibleKey(queueData.bestCompatible);
    uint8 size = CountGuidsInCompatibleKey(key);

    if (size <= storedSize)
        return;

    TC_LOG_DEBUG("lfg.queue.compatibles.update", "Changed ({}) to ({}) as best compatible group for {}",
        queueData.bestCompatible, key, itrQueue->first.ToString());

    queueData.bestCompatible = key;
    queueData.tanks = LFG_TANKS_NEEDED;
    queueData.healers = LFG_HEALERS_NEEDED;
    queueData.dps = LFG_DPS_NEEDED;
    for (LfgRolesMap::const_iterator it = roles.begin(); it != roles.end(); ++it)
    {
        uint8 role = it->second;
        if (role & PLAYER_ROLE_TANK)
            --queueData.tanks;
        else if (role & PLAYER_ROLE_HEALER)
            --queueData.healers;
        else
            --queueData.dps;
    }
}

void LFGQueue::ResetBestCompatibleToSelf(LfgQueueDataContainer::iterator itrQueue)
{
    if (itrQueue == QueueDataStore.end())
        return;

    LfgQueueData& queueData = itrQueue->second;
    queueData.bestCompatible.clear();
    queueData.tanks = LFG_TANKS_NEEDED;
    queueData.healers = LFG_HEALERS_NEEDED;
    queueData.dps = LFG_DPS_NEEDED;

    LfgRolesMap roles = queueData.roles;
    LFGMgr::CheckGroupRoles(roles);
    UpdateBestCompatibleInQueue(itrQueue, ConcatenateGuids(GuidList(1, itrQueue->first)), roles);
}

bool LFGQueue::CompatibleKeyContainsGuid(std::string const& key, ObjectGuid guid)
{
    std::string const wanted = std::to_string(guid.GetRawValue());
    std::string::size_type begin = 0;

    while (begin <= key.size())
    {
        std::string::size_type end = key.find('|', begin);
        std::string token = end == std::string::npos ? key.substr(begin) : key.substr(begin, end - begin);

        if (token == wanted)
            return true;

        if (end == std::string::npos)
            break;

        begin = end + 1;
    }

    return false;
}

uint8 LFGQueue::CountGuidsInCompatibleKey(std::string const& key)
{
    if (key.empty())
        return 0;

    return uint8(std::count(key.begin(), key.end(), '|') + 1);
}

void LfgRoleCoverage::Add(LfgRolesMap const& roles)
{
    for (LfgRolesMap::const_iterator itr = roles.begin(); itr != roles.end(); ++itr)
    {
        bool tank = (itr->second & PLAYER_ROLE_TANK) != 0;
        bool healer = (itr->second & PLAYER_ROLE_HEALER) != 0;
        bool damage = (itr->second & PLAYER_ROLE_DAMAGE) != 0;

        Tank += tank;
        Healer += healer;
        Damage += damage;
        TankOrHealer += tank || healer;
        TankOrDamage += tank || damage;
        HealerOrDamage += healer || damage;
        ++Players;
    }
}

void LfgRoleCoverage::Add(LfgRoleCoverage const& other)
{
    Tank += other.Tank;
    Healer += other.Healer;
    Damage += other.Damage;
    TankOrHealer += other.TankOrHealer;
    TankOrDamage += other.TankOrDamage;
    HealerOrDamage += other.HealerOrDamage;
    Players += other.Players;
}

bool LfgRoleCoverage::CanCompleteGroupWith(LfgRoleCoverage const& other) const
{
    return Tank + other.Tank >= LFG_TANKS_NEEDED
        && Healer + other.Healer >= LFG_HEALERS_NEEDED
        && Damage + other.Damage >= LFG_DPS_NEEDED
        && TankOrHealer + other.TankOrHealer >= LFG_TANKS_NEEDED + LFG_HEALERS_NEEDED
        && TankOrDamage + other.TankOrDamage >= LFG_TANKS_NEEDED + LFG_DPS_NEEDED
        && HealerOrDamage + other.HealerOrDamage >= LFG_HEALERS_NEEDED + LFG_DPS_NEEDED
        && Players + other.Players >= MAX_GROUP_SIZE;
}

} // namespace lfg
