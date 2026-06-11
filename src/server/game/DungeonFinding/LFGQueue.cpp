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
    : joinTime(GameTime::GetGameTime()), tanks(LFG_TANKS_NEEDED), healers(LFG_HEALERS_NEEDED), dps(LFG_DPS_NEEDED)
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
        AddToFrontCurrentQueue(guid);
    else
        AddToNewQueue(guid);
}

void LFGQueue::RemoveFromQueue(ObjectGuid guid)
{
    RemoveFromNewQueue(guid);
    RemoveFromCurrentQueue(guid);

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

void LFGQueue::AddQueueData(ObjectGuid guid, time_t joinTime, LfgDungeonSet const& dungeons, LfgRolesMap const& rolesMap)
{
    // EG - replacing queue data for the same guid invalidates every cached [compatibility result and best-compatible status]
    // that involved the previous roles/dungeon set/group composition, so erase the old data before invalidating to prevent recomputation from picking up stale data for this guid
    RemoveFromNewQueue(guid);
    RemoveFromCurrentQueue(guid);
    QueueDataStore.erase(guid);
    InvalidateCompatibleData(guid);

    QueueDataStore[guid] = LfgQueueData(joinTime, dungeons, rolesMap);
    AddToQueue(guid);
}

void LFGQueue::RemoveQueueData(ObjectGuid guid)
{
    // Keep this safe even if callers bypass RemoveFromQueue().
    RemoveFromNewQueue(guid);
    RemoveFromCurrentQueue(guid);

    QueueDataStore.erase(guid);
    InvalidateCompatibleData(guid);
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
            itr->second.bestCompatible.clear();
            itr->second.tanks = LFG_TANKS_NEEDED;
            itr->second.healers = LFG_HEALERS_NEEDED;
            itr->second.dps = LFG_DPS_NEEDED;

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

    // Try to match with queued groups. Do not let recursive calls consume this
    // frame's candidate list by reference; each branch gets its own remaining
    // list, which avoids skipping valid combinations after an incomplete branch.
    for (GuidList::const_iterator itr = all.begin(); itr != all.end(); ++itr)
    {
        check.push_back(*itr);

        GuidList remaining;
        GuidList::const_iterator rest = itr;
        ++rest;
        for (; rest != all.end(); ++rest)
            remaining.push_back(*rest);

        LfgCompatibility subcompatibility = FindNewGroups(check, remaining);
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
                        TC_LOG_ERROR("lfg.queue.match.compatibility.check", "Guids: ERROR! Player multiple times in queue! [{}]", itRoles->first.ToString());
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

    ObjectGuid gguid = *check.begin();
    proposal.queues = check;
    proposal.isNew = numLfgGroups != 1 || sLFGMgr->GetOldState(gguid) != LFG_STATE_DUNGEON;

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
                        InvalidateCompatibleData(queueGuid);
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
    for (LfgQueueDataContainer::iterator itQueue = QueueDataStore.begin(); itQueue != QueueDataStore.end(); ++itQueue)
    {
        LfgQueueData& queueinfo = itQueue->second;

        if (queueinfo.dungeons.empty())
            continue;

        uint32 dungeonId = (*queueinfo.dungeons.begin());
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

        if (queueinfo.bestCompatible.empty())
            FindBestCompatibleInQueue(itQueue);

        LfgQueueStatusData queueData(dungeonId, waitTime, wtAvg, wtTank, wtHealer, wtDps, queuedTime, queueinfo.tanks, queueinfo.healers, queueinfo.dps);
        for (LfgRolesMap::const_iterator itPlayer = queueinfo.roles.begin(); itPlayer != queueinfo.roles.end(); ++itPlayer)
        {
            ObjectGuid pguid = itPlayer->first;
            LFGMgr::SendLfgQueueStatus(pguid, queueData);
        }
    }
}

time_t LFGQueue::GetJoinTime(ObjectGuid guid) const
{
    LfgQueueDataContainer::const_iterator itr = QueueDataStore.find(guid);
    if (itr != QueueDataStore.end())
        return itr->second.joinTime;

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

} // namespace lfg
