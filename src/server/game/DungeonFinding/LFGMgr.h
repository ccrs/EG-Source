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

#ifndef _LFGMGR_H
#define _LFGMGR_H

#include "Common.h"
#include "DatabaseEnvFwd.h"
#include "LFG.h"
#include "LFGQueue.h"
#include "LFGGroupData.h"
#include "LFGPlayerData.h"
#include "SharedDefines.h"
#include <unordered_map>
#include <map>
#include <mutex>

class Group;
class Player;
class Quest;
class Map;
struct LFGDungeonEntry;
enum Difficulty : uint8;

/********************************************************************************************************************************************************\
 *                                                    DEV DOCUMENTATION: DUNGEON FINDER (LFG) SYSTEM                                                    *
 *                                         (future devs: please keep this up-to-date if you change the system)                                          *
 * COMPONENTS                                                                                                                                           *
 *  - LFGMgr (sLFGMgr): owner of all LFG state. Stores, all keyed by guid:                                                                              *
 *    - PlayersStore (LfgPlayerData): state, old state, team, current group, roles, selected dungeons, comment, party size at join.                     *
 *    - GroupsStore (LfgGroupData): state, old state, leader, member set, dungeon entry, kicks left, vote kick flag, solo composition tracking.         *
 *    - RoleChecksStore, ProposalsStore, BootsStore: the live role checks, proposals and vote kicks.                                                    *
 *    - PendingTeleportInStore: deferred teleports into a fresh instance copy.                                                                          *
 *    - _finishedQueueCancels: groups that finished their dungeon while queued/rolechecking/proposed, filled from map threads under a mutex.            *
 *    - QueuesStore: one LFGQueue per queue id.                                                                                                         *
 *  - LFGQueue: QueueDataStore (one LfgQueueData per queued entity), newToQueueStore (entities not yet searched), currentQueueStore (waiting entities,  *
 *    oldest first), _rematchStore (entities re-added after a failed proposal), CompatibleMapStore (match cache, cleared on every FindGroups call),     *
 *    wait time averages, _lockedOut (entities emptied by save revalidation).                                                                           *
 *  - LFGScripts: player hooks (login, logout, map change) and group hooks (add/remove member, disband, leader change, invite) that keep LFG data in    *
 *    sync with Group. Every group hook returns early for battleground and battlefield groups.                                                          *
 *  - LFGHandler.cpp: opcode handlers and packet builders (WorldSession::SendLfg*).                                                                     *
 *  - LFGRandomReward (Custom/LFGRandomReward.cpp): bonus loot rolled from FinishDungeon.                                                               *
 *                                                                                                                                                      *
 * IDENTIFIERS                                                                                                                                          *
 *  - A queue entity is either a player guid (solo) or a group guid (party). A queued party is one entity with a frozen roles snapshot of its members.  *
 *  - Dungeon "id" = LFGDungeons.dbc ID. Dungeon "entry" = id | (type << 24) (LFGDungeonData::Entry, GetLFGDungeonEntry). Selected dungeons, queue      *
 *    dungeon sets and lfg_data store ids; LfgGroupData stores the entry (GetDungeon(guid) returns the id, GetDungeon(guid, false) the entry); lock     *
 *    maps are keyed by entry. Packet builders convert ids to entries where the client expects them (update, role check, proposal packets).             *
 *  - Queue id = player team set at login (SetTeam), forced to 0 when AllowTwoSide.Interaction.Group is on (one shared queue). A group uses the team of *
 *    the lowest guid in its LfgGroupData member set.                                                                                                   *
 *                                                                                                                                                      *
 * STATES (LfgState): NONE, ROLECHECK, QUEUED, PROPOSAL, DUNGEON, FINISHED_DUNGEON (RAIDBROWSER unused)                                                 *
 *  - SetState(NONE | DUNGEON | FINISHED_DUNGEON) also latches OldState; ROLECHECK/QUEUED/PROPOSAL leave OldState untouched. RestoreState = OldState.   *
 *  - Player SetState(NONE | FINISHED_DUNGEON) clears roles, selected dungeons and comment. Group SetState(NONE) clears dungeon and kicks left.         *
 *  - LfgGroupData::IsLfgGroup() is OldState != NONE. Group::isLFGGroup() is the GROUPTYPE_LFG flag. Both exist; matching uses the first one.           *
 *  - A backfill (LFG group asking for a replacement) runs with State ROLECHECK/QUEUED/PROPOSAL and OldState DUNGEON.                                   *
 *  - Reads (GetState, GetDungeon, GetGroup, GetPlayers, ...) go through GetGroupData/GetPlayerData and never create entries; a missing entry reads as  *
 *    default data. Setters and GroupsStore[]/PlayersStore[] create entries. PlayersStore entries are never erased; GroupsStore entries are erased on   *
 *    disband (RemoveGroupData).                                                                                                                        *
 *                                                                                                                                                      *
 * THREADS                                                                                                                                              *
 *  - Every LFG mutation runs on the world thread: World::Update runs sessions (all LFG opcodes are PROCESS_THREADUNSAFE), then the maps (joined), then *
 *    groups, then sLFGMgr->Update. Player hooks and the group hooks of non-battleground groups also run on the world thread.                           *
 *  - Exceptions that run on map threads (MapUpdate.Threads > 1 runs maps in parallel):                                                                 *
 *    - FinishDungeon (InstanceScript::UpdateEncounterState, seasonal boss scripts). It writes only existing group/player nodes, writes lfg_data        *
 *      through the async DB queue, and hands queue work to the world thread through _finishedQueueCancels.                                             *
 *    - Read-only users: Player::CanRollForItemInLFG (loot), Group::SendUpdate from Group::UpdateLooterGuid, old_hillsbrad TeleportPlayer(out),         *
 *      LFGRandomReward::TryReward.                                                                                                                     *
 *  - Map-thread code must never touch the queues, proposals, role checks, boots, or insert into PlayersStore/GroupsStore.                              *
 *                                                                                                                                                      *
 * UPDATE TICK (LFGMgr::Update, skipped entirely while the finder is disabled)                                                                          *
 *  1. ProcessPendingTeleportIns, ProcessFinishedQueueCancels.                                                                                          *
 *  2. Role checks past 45 s fail with MISSING_ROLE; proposals past 45 s fail (pending answers count as declines); vote kicks past 30 s close.          *
 *  3. FindGroups on every queue, then ProcessLockedOutQueueEntries.                                                                                    *
 *  4. Every proposal created in this tick: backfills get the completed encounter mask of the group's loaded instance, every player and their group go  *
 *     to PROPOSAL and receive PROPOSAL_BEGIN plus the proposal.                                                                                        *
 *  5. Every 15 s: UpdateQueueTimers on every queue (queue status packets).                                                                             *
 *                                                                                                                                                      *
 * JOIN (CMSG_LFG_JOIN -> JoinLfg)                                                                                                                      *
 *  - A grouped non-leader may only join for a non-full LFG group. Roles are filtered by class. A pending proposal of the entity is declined first.     *
 *  - An LFG group in DUNGEON (after TryFinishDungeonFromCurrentInstance had its chance to mark it finished) is a continue: its dungeon replaces the    *
 *    selection, the random cooldown is not checked and own instance saves do not lock it.                                                              *
 *  - Validation: RBAC, hardcore dead, battleground/arena/arena queue (other BG queues are allowed), deserter, random cooldown, freeze, group size,     *
 *    every member online, dungeon type mix, then GetCompatibleDungeons (locks of every player). A random id is expanded to its dungeon set.            *
 *  - Only after validation an existing queue entry of the entity is replaced. A raid selection returns without joining (raid browser not implemented). *
 *  - Solo: AddQueueData (join time, expanded dungeons, roles, status dungeon = random id or first dungeon), state QUEUED, selection = random id.       *
 *  - Party: role check (all members ROLECHECK, roles 0, leader's role set at once). Each CMSG_LFG_SET_ROLES updates it (non-members ignored); when     *
 *    every member chose and CheckGroupRoles passes, the group entity is queued with the members' roles. NO_ROLE, WRONG_ROLES, abort or timeout restore *
 *    every member and the group.                                                                                                                       *
 *                                                                                                                                                      *
 * MATCHING (LFGQueue::FindGroups, once per tick per queue)                                                                                             *
 *  - First every _rematchStore entity still listed is searched against the rest of currentQueueStore, then every newcomer against currentQueueStore.   *
 *    An unmatched newcomer is appended to currentQueueStore. Only these two passes create proposals.                                                   *
 *  - Search (FindNewGroups): depth-first over subsets in queue order, the first MATCH wins. At every node candidates are skipped when the players      *
 *    would exceed 5, when the check already holds an LFG group and the candidate is one, or when the candidate shares no dungeon with the check.       *
 *    LfgRoleCoverage then cuts a branch unless, for at least one common dungeon, the check plus the remaining candidates queued for it can still       *
 *    supply 1 tank, 1 healer, 3 damage (also pairwise: T+H 2, T+D 4, H+D 4) and 5 players.                                                             *
 *  - Each search counts its CheckCompatibility calls against LFG_MATCH_CHECK_BUDGET (reset per searched entity; the child checks of the last counted   *
 *    call still run); when exhausted it stops, logs a WARN and the entity stays queued.                                                                *
 *  - CheckCompatibility(check), in order: size 1..5; for 3+ entities the subset without the first entity must be compatible; every entity has queue    *
 *    data; at most one LFG group (it becomes proposal.group); at most 5 players; no player twice, no ignores; CheckGroupRoles assigns 1/1/3; a common  *
 *    dungeon exists; exactly 5 players; every entity is QUEUED (else BAD_STATES); instance saves are checked again (GetCompatibleDungeons).            *
 *  - Save revalidation removes newly locked dungeons from the affected entries (never from the backfilled group's own entry); an entry left with no    *
 *    dungeon goes to _lockedOut and ProcessLockedOutQueueEntries removes it from the queue with a NOT_MEET_REQS join result carrying the locks.        *
 *  - On MATCH a proposal is built: random dungeon from the common set, leader chosen at random among players with the leader flag (any player when     *
 *    none has it), isNew unless proposal.group's OldState is DUNGEON (backfill), backfill members auto-accept. The entities are unlisted but keep      *
 *    their queue data: the proposal owns it until it succeeds or fails.                                                                                *
 *  - CheckCompatibility results are cached by the sorted guid key; the cache is cleared at the start of every FindGroups call (queue changes between   *
 *    ticks still invalidate the previous tick's entries); cached MATCH and BAD_STATES are always recomputed.                                           *
 *                                                                                                                                                      *
 * PROPOSAL                                                                                                                                             *
 *  - CMSG_LFG_PROPOSAL_RESULT -> UpdateProposal. A decline, a timeout, a member leaving its group (OnRemoveMember -> LeaveLfg) or a re-join declines.  *
 *  - RemoveProposal: entities with a decline (or whose group no longer exists) are removed from the queue and restored; every other entity returns to  *
 *    QUEUED at the front of currentQueueStore and is matched again in the next tick (_rematchStore), keeping its join time.                            *
 *  - Success (everyone accepted): an existing group that would exceed 5 members declines instead. Otherwise every player gets GROUP_FOUND, a wait time *
 *    sample (seconds since queue join, keyed by the entity's status dungeon, per single role or average), party size at join (joining players only,    *
 *    solo = 1), state DUNGEON; the entities leave the queue, non-arena BG queues are flushed, MakeNewGroup runs; a new group starts solo composition   *
 *    tracking with every solo-queued player as an original solo member.                                                                                *
 *                                                                                                                                                      *
 * GROUP FORMING (MakeNewGroup, _formingGroup is true for its whole run)                                                                                *
 *  - Players are ordered leader, tanks, healers, damage. Each one leaves any other group and joins the existing group or a new GROUPTYPE_LFG group     *
 *    (created in PROPOSAL state). Members whose selection is a random dungeon get the random cooldown (backfills recast it on existing members).       *
 *  - Group dungeon = proposal entry, state DUNGEON, lfg_data saved, group update sent.                                                                 *
 *  - Teleported: everyone for a new group or a fresh run, otherwise only players that were not members yet.                                            *
 *  - Fresh run = existing LFG group whose OldState is not DUNGEON or whose dungeon changed. Before any teleport the group bind to the previous dungeon *
 *    is reset and unbound (TryClearOldLfgGroupBind: normal difficulty, non-raid, non-permanent, resettable only; heroic and raid binds stay), so       *
 *    players outside the old copy are teleported at once into a new copy. Players still inside the old copy leave through their entry point and wait   *
 *    in PendingTeleportInStore (30 s); once off the dungeon map the bind check runs again for the exact instance they left (it finds no bind or the    *
 *    new copy) and they are teleported in. Expiry while still inside sends TELEPORT_DENIED. The LFG teleport opcode is refused while an entry is       *
 *    pending.                                                                                                                                          *
 *                                                                                                                                                      *
 * TELEPORT (TeleportPlayer)                                                                                                                            *
 *  - In: tournament gear check, then dead/falling/fatigue/vehicle/charm/freeze errors. A player already on the dungeon map is not moved (error) unless *
 *    it is a fresh run. Destination: a member already inside (automatic teleports of an ongoing run) or the dungeon entrance. The entry point and      *
 *    mount are captured unless the player is inside the group's current or previous LFG dungeon.                                                       *
 *  - Out: TeleportToBGEntryPoint, only while on the dungeon map.                                                                                       *
 *                                                                                                                                                      *
 * IN THE DUNGEON                                                                                                                                       *
 *  - Leaving (GROUP_REMOVEMETHOD_LEAVE) casts deserter when the group is in DUNGEON, or in a backfill with OldState DUNGEON, and 3+ members remain. A  *
 *    removed member inside a dungeon is teleported out; the leader gets OFFER_CONTINUE unless the dungeon is finished.                                 *
 *  - Vote kick: a KICK removal of an LFG group member starts InitBoot instead (Player::CanUninviteFromGroup gates it: kicks left, no vote running,     *
 *    more than 3 members, not finished, no loot roll, nobody in combat). Every member votes, victim auto-deny, kicker auto-agree; it ends at 3 agrees  *
 *    or when 3 agrees became impossible; it lasts 30 s. A passing vote removes the victim with KICK_LFG (only while still a member), which removes     *
 *    their random cooldown. The victim leaving the group ends the vote (inProgress false to the voters); another member leaving gets a closed update   *
 *    and loses their vote (OnBootMemberRemoved).                                                                                                       *
 *  - Joining a group while it is QUEUED, ROLECHECK or PROPOSAL ends that queue, role check or proposal (except the adds done by MakeNewGroup).         *
 *  - Changing map: in the LFG dungeon without a group -> homebind; outside it a 1-member group is disbanded. An LFG group left with one member also    *
 *    disbands when the leader is offline or alive outside the dungeon map.                                                                             *
 *                                                                                                                                                      *
 * FINISH (FinishDungeon, map thread)                                                                                                                   *
 *  - Ignored when the finished dungeon is not the group's dungeon (ERROR), when already FINISHED, or when OldState is FINISHED (a finished group       *
 *    queued for a fresh run). A pending backfill is recorded for ProcessFinishedQueueCancels (QUEUED -> dequeued, ROLECHECK -> aborted, PROPOSAL ->    *
 *    declined); OldState FINISHED keeps every restored state FINISHED.                                                                                 *
 *  - Group and members -> FINISHED, lfg_data saved. Members on the finished map whose selection is a random or seasonal dungeon lose the random        *
 *    cooldown, get the first or other reward quest, the LFD heroic achievement criterion and SMSG_LFG_PLAYER_REWARD (strangers = 5 - party size at     *
 *    join). LFGRandomReward rolls for level 80 heroic randoms when the player is an original solo member and the composition is intact (one voluntary  *
 *    leave plus one replacement tolerated; a vote kick, a second leave or any other add breaks it). Composition tracking ends with the finish.         *
 *                                                                                                                                                      *
 * LEAVING AND CLEANUP                                                                                                                                  *
 *  - LeaveLfg(guid): QUEUED -> entity dequeued (the whole party for a party member); the group and every member go back to DUNGEON/FINISHED for an LFG *
 *    group, to NONE otherwise. A member leaving the group (OnRemoveMember) ends in NONE: when the restore put them back to DUNGEON/FINISHED, a second  *
 *    LeaveLfg clears it. ROLECHECK -> aborted. PROPOSAL -> declined. DUNGEON/FINISHED player -> NONE unless disconnected. Only the leader may send     *
 *    CMSG_LFG_LEAVE.                                                                                                                                   *
 *  - Logout: UPDATE_SEARCH false and LeaveLfg for every player; grouped players keep DUNGEON/FINISHED (disconnected = true).                           *
 *  - Disband (RemoveGroupData): role check aborted, queue entry removed unless a proposal owns it, members detached (NONE + REMOVED_FROM_QUEUE unless  *
 *    PROPOSAL), vote kick closed, group data erased. Group::Disband deletes lfg_data.                                                                  *
 *  - BG co-queue: accepting a non-arena BG invite leaves LFG; DUNGEON/FINISHED players cannot join BGs; LFG groups cannot queue as a group (a normal   *
 *    party queued in LFG can).                                                                                                                         *
 *  - `.lfg clean` fails every proposal, aborts every role check and LeaveLfg's every queued entity before dropping the queues; `.lfg options` runs it  *
 *    when the finder gets disabled. While disabled, Update and the group, login and logout hooks do nothing (membership changes are not tracked).      *
 *                                                                                                                                                      *
 * QUEUE STATUS (UpdateQueueTimers, every 15 s)                                                                                                         *
 *  - An entry whose owner is neither QUEUED nor PROPOSAL is logged as orphaned and removed. Entries without dungeons are skipped.                      *
 *  - SMSG_LFG_QUEUE_STATUS to every player of the entity (also during a proposal): status dungeon (random id for random queues, the first dungeon      *
 *    otherwise; a backfill reports its specific dungeon), wait times read under the status dungeon (single role -> that role, else average), time in   *
 *    queue, and roles needed from UpdateStatusNeeds: a greedy partial group of the entity plus waiting entities in queue order (common dungeon, at     *
 *    most 5 players and one LFG group, no ignores or duplicates, roles assignable).                                                                    *
 *                                                                                                                                                      *
 * PERSISTENCE (characters.lfg_data: group db id, dungeon id, state)                                                                                    *
 *  - Written by MakeNewGroup (DUNGEON) and FinishDungeon (FINISHED), deleted by Group::Disband. At startup groups load it (_LoadFromDB: leader,        *
 *    dungeon entry, DUNGEON/FINISHED) and SetupGroupMember gives every member of every group its group and state; the selection is {group dungeon id}  *
 *    for DUNGEON groups and empty otherwise (SetState(NONE | FINISHED_DUNGEON) clears it). Queues, role checks, proposals, boots, wait times and       *
 *    composition tracking are memory only.                                                                                                             *
 *                                                                                                                                                      *
 * KEY RULES FOR FUTURE CHANGES                                                                                                                         *
 *  - LeaveLfg, UpdateRoleCheck, RemoveProposal, Group::AddMember/RemoveMember/Disband and Player::RemoveFromGroup re-enter LFGMgr through the group    *
 *    hooks and can erase store entries, including the group's own. Never hold a store iterator or reference across them (finish the entry first).      *
 *  - FindNewGroups pruning must stay a set of necessary conditions of a MATCH in CheckCompatibility: any rule change there (sizes, roles, dungeons,    *
 *    LFG groups) must be mirrored in the candidate filter and LfgRoleCoverage, or valid groups are skipped silently.                                   *
 *  - Queue data of an entity in PROPOSAL belongs to the proposal; remove it only through the proposal paths.                                           *
 *  - A queued party's roles snapshot is never updated: any membership change while QUEUED/ROLECHECK/PROPOSAL must end the queue for the group.         *
 *  - Wait times are keyed by the status dungeon; status counters come from UpdateStatusNeeds, not from the match search.                               *
 *  - After a restart members of a DUNGEON group have the specific dungeon as selection, so a random run interrupted by a restart gives no random       *
 *    reward at its finish.                                                                                                                             *
 *  - Unused: RemoveQueueData, RemovePlayerData (PlayersStore never shrinks), ClearState (declared only), LfgProposal::showorder, raid browser.         *
 *  - The group list LFG block (Group::SendUpdateToPlayer) sends status 2 when FINISHED, else 0, and the group dungeon entry.                           *
\********************************************************************************************************************************************************/

namespace lfg
{

enum LfgOptions
{
    LFG_OPTION_ENABLE_DUNGEON_FINDER             = 0x01,
    LFG_OPTION_ENABLE_RAID_BROWSER               = 0x02,
};

enum LFGMgrEnum
{
    LFG_TIME_ROLECHECK                           = 45,
    LFG_TIME_BOOT                                = 30, // EG
    LFG_TIME_PROPOSAL                            = 45,
    LFG_TIME_PENDING_TELEPORT_IN                 = 30,
    LFG_QUEUEUPDATE_INTERVAL                     = 15 * IN_MILLISECONDS,
    LFG_SPELL_DUNGEON_COOLDOWN                   = 71328,
    LFG_SPELL_DUNGEON_DESERTER                   = 71041,
    LFG_SPELL_LUCK_OF_THE_DRAW                   = 72221,
    LFG_GROUP_KICK_VOTES_NEEDED                  = 3
};

enum LfgFlags
{
    LFG_FLAG_UNK1                                = 0x1,
    LFG_FLAG_UNK2                                = 0x2,
    LFG_FLAG_SEASONAL                            = 0x4,
    LFG_FLAG_UNK3                                = 0x8
};

/// Determines the type of instance
enum LfgType
{
    LFG_TYPE_NONE                                = 0,
    LFG_TYPE_DUNGEON                             = 1,
    LFG_TYPE_RAID                                = 2,
    LFG_TYPE_HEROIC                              = 5,
    LFG_TYPE_RANDOM                              = 6
};

/// Proposal states
enum LfgProposalState
{
    LFG_PROPOSAL_INITIATING                      = 0,
    LFG_PROPOSAL_FAILED                          = 1,
    LFG_PROPOSAL_SUCCESS                         = 2
};

/// Teleport errors
enum LfgTeleportError
{
    // 7 = "You can't do that right now" | 5 = No client reaction
    LFG_TELEPORTERROR_OK                         = 0,      // Internal use
    LFG_TELEPORTERROR_PLAYER_DEAD                = 1,
    LFG_TELEPORTERROR_FALLING                    = 2,
    LFG_TELEPORTERROR_IN_VEHICLE                 = 3,
    LFG_TELEPORTERROR_FATIGUE                    = 4,
    LFG_TELEPORTERROR_INVALID_LOCATION           = 6,
    LFG_TELEPORTERROR_CHARMING                   = 8       // FIXME - It can be 7 or 8 (Need proper data)
};

/// Queue join results
enum LfgJoinResult
{
    // 3 = No client reaction | 18 = "Rolecheck failed"
    LFG_JOIN_OK                                  = 0,      // Joined (no client msg)
    LFG_JOIN_FAILED                              = 1,      // RoleCheck Failed
    LFG_JOIN_GROUPFULL                           = 2,      // Your group is full
    LFG_JOIN_INTERNAL_ERROR                      = 4,      // Internal LFG Error
    LFG_JOIN_NOT_MEET_REQS                       = 5,      // You do not meet the requirements for the chosen dungeons
    LFG_JOIN_PARTY_NOT_MEET_REQS                 = 6,      // One or more party members do not meet the requirements for the chosen dungeons
    LFG_JOIN_MIXED_RAID_DUNGEON                  = 7,      // You cannot mix dungeons, raids, and random when picking dungeons
    LFG_JOIN_MULTI_REALM                         = 8,      // The dungeon you chose does not support players from multiple realms
    LFG_JOIN_DISCONNECTED                        = 9,      // One or more party members are pending invites or disconnected
    LFG_JOIN_PARTY_INFO_FAILED                   = 10,     // Could not retrieve information about some party members
    LFG_JOIN_DUNGEON_INVALID                     = 11,     // One or more dungeons was not valid
    LFG_JOIN_DESERTER                            = 12,     // You can not queue for dungeons until your deserter debuff wears off
    LFG_JOIN_PARTY_DESERTER                      = 13,     // One or more party members has a deserter debuff
    LFG_JOIN_RANDOM_COOLDOWN                     = 14,     // You can not queue for random dungeons while on random dungeon cooldown
    LFG_JOIN_PARTY_RANDOM_COOLDOWN               = 15,     // One or more party members are on random dungeon cooldown
    LFG_JOIN_TOO_MUCH_MEMBERS                    = 16,     // You can not enter dungeons with more that 5 party members
    LFG_JOIN_USING_BG_SYSTEM                     = 17      // You can not use the dungeon system while in BG or arenas
};

/// Role check states
enum LfgRoleCheckState
{
    LFG_ROLECHECK_DEFAULT                        = 0,      // Internal use = Not initialized.
    LFG_ROLECHECK_FINISHED                       = 1,      // Role check finished
    LFG_ROLECHECK_INITIALITING                   = 2,      // Role check begins
    LFG_ROLECHECK_MISSING_ROLE                   = 3,      // Someone didn't selected a role after 2 mins
    LFG_ROLECHECK_WRONG_ROLES                    = 4,      // Can't form a group with that role selection
    LFG_ROLECHECK_ABORTED                        = 5,      // Someone leave the group
    LFG_ROLECHECK_NO_ROLE                        = 6       // Someone selected no role
};

enum LfgRoleClasses {
    TANK = (1 << (CLASS_WARRIOR - 1)) |
           (1 << (CLASS_PALADIN - 1)) |
           (1 << (CLASS_DEATH_KNIGHT - 1)) |
           (1 << (CLASS_DRUID - 1)),

    HEALER = (1 << (CLASS_PALADIN - 1)) |
             (1 << (CLASS_PRIEST - 1)) |
             (1 << (CLASS_SHAMAN - 1)) |
             (1 << (CLASS_DRUID - 1)),
};

// Forward declaration (just to have all typedef together)
struct LFGDungeonData;
struct LfgReward;
struct LfgQueueInfo;
struct LfgRoleCheck;
struct LfgProposal;
struct LfgProposalPlayer;
struct LfgPlayerBoot;

typedef std::map<uint8, LFGQueue> LfgQueueContainer;
typedef std::multimap<uint32, LfgReward const*> LfgRewardContainer;
typedef std::pair<LfgRewardContainer::const_iterator, LfgRewardContainer::const_iterator> LfgRewardContainerBounds;
typedef std::map<uint8, LfgDungeonSet> LfgCachedDungeonContainer;
typedef std::map<ObjectGuid, LfgAnswer> LfgAnswerContainer;
typedef std::map<ObjectGuid, LfgRoleCheck> LfgRoleCheckContainer;
typedef std::map<uint32, LfgProposal> LfgProposalContainer;
typedef std::map<ObjectGuid, LfgProposalPlayer> LfgProposalPlayerContainer;
typedef std::map<ObjectGuid, LfgPlayerBoot> LfgPlayerBootContainer;
typedef std::map<ObjectGuid, LfgGroupData> LfgGroupDataContainer;
typedef std::map<ObjectGuid, LfgPlayerData> LfgPlayerDataContainer;
typedef std::unordered_map<uint32, LFGDungeonData> LFGDungeonContainer;

// Data needed by SMSG_LFG_JOIN_RESULT
struct LfgJoinResultData
{
    LfgJoinResultData(LfgJoinResult _result = LFG_JOIN_OK, LfgRoleCheckState _state = LFG_ROLECHECK_DEFAULT):
        result(_result), state(_state) { }
    LfgJoinResult result;
    LfgRoleCheckState state;
    LfgLockPartyMap lockmap;
};

// Data needed by SMSG_LFG_UPDATE_PARTY and SMSG_LFG_UPDATE_PLAYER
struct LfgUpdateData
{
    LfgUpdateData(LfgUpdateType _type = LFG_UPDATETYPE_DEFAULT): updateType(_type), state(LFG_STATE_NONE), comment("") { }
    LfgUpdateData(LfgUpdateType _type, LfgDungeonSet const& _dungeons, std::string const& _comment):
        updateType(_type), state(LFG_STATE_NONE), dungeons(_dungeons), comment(_comment) { }
    LfgUpdateData(LfgUpdateType _type, LfgState _state, LfgDungeonSet const& _dungeons, std::string const& _comment = ""):
        updateType(_type), state(_state), dungeons(_dungeons), comment(_comment) { }

    LfgUpdateType updateType;
    LfgState state;
    LfgDungeonSet dungeons;
    std::string comment;
};

// Data needed by SMSG_LFG_QUEUE_STATUS
struct LfgQueueStatusData
{
    LfgQueueStatusData(uint32 _dungeonId = 0, int32 _waitTime = -1, int32 _waitTimeAvg = -1, int32 _waitTimeTank = -1, int32 _waitTimeHealer = -1,
        int32 _waitTimeDps = -1, uint32 _queuedTime = 0, uint8 _tanks = 0, uint8 _healers = 0, uint8 _dps = 0) :
        dungeonId(_dungeonId), waitTime(_waitTime), waitTimeAvg(_waitTimeAvg), waitTimeTank(_waitTimeTank), waitTimeHealer(_waitTimeHealer),
        waitTimeDps(_waitTimeDps), queuedTime(_queuedTime), tanks(_tanks), healers(_healers), dps(_dps) { }

    uint32 dungeonId;
    int32 waitTime;
    int32 waitTimeAvg;
    int32 waitTimeTank;
    int32 waitTimeHealer;
    int32 waitTimeDps;
    uint32 queuedTime;
    uint8 tanks;
    uint8 healers;
    uint8 dps;
};

struct LfgPlayerRewardData
{
    LfgPlayerRewardData(uint32 random, uint32 current, bool _done, Quest const* _quest, uint8 _strangers): // EG
        rdungeonEntry(random), sdungeonEntry(current), done(_done), quest(_quest), strangers(_strangers) { }
    uint32 rdungeonEntry;
    uint32 sdungeonEntry;
    bool done;
    Quest const* quest;
    uint8 strangers; // EG
};

/// Reward info
struct LfgReward
{
    LfgReward(uint32 _maxLevel = 0, uint32 _firstQuest = 0, uint32 _otherQuest = 0):
        maxLevel(_maxLevel), firstQuest(_firstQuest), otherQuest(_otherQuest) { }

    uint32 maxLevel;
    uint32 firstQuest;
    uint32 otherQuest;
};

/// Stores player data related to proposal to join
struct LfgProposalPlayer
{
    LfgProposalPlayer(): role(0), accept(LFG_ANSWER_PENDING), group() { }
    uint8 role;                                            /// Proposed role
    LfgAnswer accept;                                      /// Accept status (-1 not answer | 0 Not agree | 1 agree)
    ObjectGuid group;                                      /// Original group guid. 0 if no original group
};

/// Stores group data related to proposal to join
struct LfgProposal
{
    LfgProposal(uint32 dungeon = 0): id(0), dungeonId(dungeon), state(LFG_PROPOSAL_INITIATING),
        group(), leader(), cancelTime(0), encounters(0), isNew(true)
        { }

    uint32 id;                                             /// Proposal Id
    uint32 dungeonId;                                      /// Dungeon to join
    LfgProposalState state;                                /// State of the proposal
    ObjectGuid group;                                      /// Proposal group (0 if new)
    ObjectGuid leader;                                     /// Leader guid.
    time_t cancelTime;                                     /// Time when we will cancel this proposal
    uint32 encounters;                                     /// Dungeon Encounters
    bool isNew;                                            /// Determines if it's new group or not
    GuidList queues;                                       /// Queue Ids to remove/readd
    GuidList showorder;                                    /// Show order in update window
    LfgProposalPlayerContainer players;                    /// Players data
};

/// Stores all rolecheck info of a group that wants to join
struct LfgRoleCheck
{
    time_t cancelTime;                                     /// Time when the rolecheck will fail
    LfgRolesMap roles;                                     /// Player selected roles
    LfgRoleCheckState state;                               /// State of the rolecheck
    LfgDungeonSet dungeons;                                /// Dungeons group is applying for (expanded random dungeons)
    uint32 rDungeonId;                                     /// Random Dungeon Id.
    ObjectGuid leader;                                     /// Leader of the group
};

/// Stores information of a current vote to kick someone from a group
struct LfgPlayerBoot
{
    time_t cancelTime;                                     /// Time left to vote
    bool inProgress;                                       /// Vote in progress
    LfgAnswerContainer votes;                              /// Player votes (-1 not answer | 0 Not agree | 1 agree)
    ObjectGuid victim;                                     /// Player guid to be kicked (can't vote)
    std::string reason;                                    /// kick reason
};

struct LFGDungeonData
{
    LFGDungeonData();
    LFGDungeonData(LFGDungeonEntry const* dbc);

    uint32 id;
    std::string name;
    uint32 map;
    uint8 type;
    uint8 expansion;
    uint8 group;
    std::array<uint8, MAX_EXPANSIONS> minlevel;
    std::array<uint8, MAX_EXPANSIONS> maxlevel;
    Difficulty difficulty;
    bool seasonal;
    float x, y, z, o;

    // Helpers
    uint32 Entry() const { return id + (type << 24); }
};

class TC_GAME_API LFGMgr
{
    private:
        LFGMgr();
        ~LFGMgr();

    public:
        static LFGMgr* instance();

        // Functions used outside lfg namespace
        void Update(uint32 diff);

        // World.cpp
        /// Finish the dungeon for the given group. All check are performed using internal lfg data
        void FinishDungeon(ObjectGuid gguid, uint32 dungeonId, Map const* currMap);
        /// Loads rewards for random dungeons
        void LoadRewards();
        /// Loads dungeons from dbc and adds teleport coords
        void LoadLFGDungeons();

        // Multiple files
        /// Check if given guid applied for random dungeon
        bool selectedRandomLfgDungeon(ObjectGuid guid);
        /// Check if given guid applied for given map and difficulty. Used to know
        bool inLfgDungeonMap(ObjectGuid guid, uint32 map, Difficulty difficulty);
        /// Get selected dungeons
        LfgDungeonSet const& GetSelectedDungeons(ObjectGuid guid);
        /// Get current lfg state
        LfgState GetState(ObjectGuid guid);
        /// Get current vote kick state
        bool IsVoteKickActive(ObjectGuid gguid);
        /// Get current dungeon
        uint32 GetDungeon(ObjectGuid guid, bool asId = true);
        /// Get the map id of the current dungeon
        uint32 GetDungeonMapId(ObjectGuid guid);
        /// Get kicks left in current group
        uint8 GetKicksLeft(ObjectGuid gguid);
        /// Load Lfg group info from DB
        void _LoadFromDB(Field* fields, ObjectGuid guid);
        /// Initializes player data after loading group data from DB
        void SetupGroupMember(ObjectGuid guid, ObjectGuid gguid);
        /// Return Lfg dungeon entry for given dungeon id
        uint32 GetLFGDungeonEntry(uint32 id);

        // cs_lfg
        /// Get current player roles
        uint8 GetRoles(ObjectGuid guid);
        /// Get current player comment (used for LFR)
        std::string const& GetComment(ObjectGuid gguid);
        /// Gets current lfg options
        uint32 GetOptions();
        /// Sets new lfg options
        void SetOptions(uint32 options);
        /// Checks if given lfg option is enabled
        bool isOptionEnabled(uint32 option);
        bool IsFormingGroup() const { return _formingGroup; } // EG
        /// Clears queue - Only for internal testing
        void Clean();
        /// Dumps the state of the queue - Only for internal testing
        std::string DumpQueueInfo(bool full = false);

        // LFGScripts
        /// Get leader of the group (using internal data)
        ObjectGuid GetLeader(ObjectGuid guid);
        /// Sets player team
        void SetTeam(ObjectGuid guid, uint8 team);
        /// Sets player group
        void SetGroup(ObjectGuid guid, ObjectGuid group);
        /// Gets player group
        ObjectGuid GetGroup(ObjectGuid guid);
        /// Sets the leader of the group
        void SetLeader(ObjectGuid gguid, ObjectGuid leader);
        /// Removes saved group data
        void RemoveGroupData(ObjectGuid guid);
        /// Removes a player from a group
        uint8 RemovePlayerFromGroup(ObjectGuid gguid, ObjectGuid guid);
        /// Adds player to group
        void AddPlayerToGroup(ObjectGuid gguid, ObjectGuid guid);
        void OnBootMemberRemoved(ObjectGuid gguid, ObjectGuid guid); // EG

        // LFGHandler
        /// Get locked dungeons
        LfgLockMap const GetLockedDungeons(ObjectGuid guid);
        /// Returns current lfg status
        LfgUpdateData GetLfgStatus(ObjectGuid guid);
        /// Checks if Seasonal dungeon is active
        bool IsSeasonActive(uint32 dungeonId);
        /// Gets the random dungeon reward corresponding to given dungeon and player level
        LfgReward const* GetRandomDungeonReward(uint32 dungeon, uint8 level);
        /// Returns all random and seasonal dungeons for given level and expansion
        LfgDungeonSet GetRandomAndSeasonalDungeons(uint8 level, uint8 expansion);
        /// Teleport a player to/from selected dungeon
        void TeleportPlayer(Player* player, bool out, bool fromOpcode = false, bool forceNewInstance = false, uint32 previousDungeonId = 0);
        /// Inits new proposal to boot a player
        void InitBoot(ObjectGuid gguid, ObjectGuid kguid, ObjectGuid vguid, std::string const& reason);
        /// Updates player boot proposal with new player answer
        void UpdateBoot(ObjectGuid guid, bool accept);
        /// Updates proposal to join dungeon with player answer
        void UpdateProposal(uint32 proposalId, ObjectGuid guid, bool accept);
        /// Updates the role check with player answer
        void UpdateRoleCheck(ObjectGuid gguid, ObjectGuid guid = ObjectGuid::Empty, uint8 roles = PLAYER_ROLE_NONE);
        /// Sets player lfg roles
        void SetRoles(ObjectGuid guid, uint8 roles);
        /// Sets player lfr comment
        void SetComment(ObjectGuid guid, std::string const& comment);
        /// Join Lfg with selected roles, dungeons and comment
        void JoinLfg(Player* player, uint8 roles, LfgDungeonSet& dungeons, std::string const& comment);
        /// Leaves lfg
        void LeaveLfg(ObjectGuid guid, bool disconnected = false);

        // LfgQueue
        /// Get last lfg state (NONE, DUNGEON or FINISHED_DUNGEON)
        LfgState GetOldState(ObjectGuid guid);
        /// Check if given group guid is lfg
        bool IsLfgGroup(ObjectGuid guid);
        /// Gets the player count of given group
        uint8 GetPlayerCount(ObjectGuid guid);
        /// Add a new Proposal
        uint32 AddProposal(LfgProposal& proposal);
        /// Returns queue id
        uint8 GetQueueId(ObjectGuid guid);
        /// Checks if all players are queued
        bool AllQueued(GuidList const& check);
        /// Gets queue join time
        time_t GetQueueJoinTime(ObjectGuid guid);
        /// Checks if given roles match, modifies given roles map with new roles
        static bool CheckGroupRoles(LfgRolesMap &groles);
        /// Checks if given players are ignoring each other
        static bool HasIgnore(ObjectGuid guid1, ObjectGuid guid2);
        /// Sends queue status to player
        static void SendLfgQueueStatus(ObjectGuid guid, LfgQueueStatusData const& data);
        void GetCompatibleDungeons(LfgDungeonSet& dungeons, GuidSet const& players, LfgLockPartyMap& lockMap, bool isContinue);

    private:
        uint8 GetTeam(ObjectGuid guid);
        uint8 FilterClassRoles(Player* player, uint8 roles);
        void RestoreState(ObjectGuid guid, char const* debugMsg);
        void ClearState(ObjectGuid guid, char const* debugMsg);
        void SetDungeon(ObjectGuid guid, uint32 dungeon);
        void SetSelectedDungeons(ObjectGuid guid, LfgDungeonSet const& dungeons);
        void DecreaseKicksLeft(ObjectGuid guid);
        void SetState(ObjectGuid guid, LfgState state);
        void SetVoteKick(ObjectGuid gguid, bool active);
        void RemovePlayerData(ObjectGuid guid);
        void _SaveToDB(ObjectGuid guid, uint32 db_guid);
        LFGDungeonData const* GetLFGDungeon(uint32 id);
        bool TryFinishDungeonFromCurrentInstance(Group* group);
        enum class LfgBindCleanupResult
        {
            NoBind,
            Cleared,
            Protected
        };

        LfgBindCleanupResult TryClearOldLfgGroupBind(Group* group, uint32 lfgDungeonId, uint32 oldInstanceId); // EG
        void ProcessPendingTeleportIns(time_t currTime); // EG
        void ProcessFinishedQueueCancels(); // EG
        void ProcessLockedOutQueueEntries(); // EG
        uint32 GetDungeonEncounterMask(ObjectGuid gguid, uint32 dungeonId); // EG

        // Proposals
        void RemoveProposal(LfgProposalContainer::iterator itProposal, LfgUpdateType type);
        void MakeNewGroup(LfgProposal const& proposal);

        // Generic
        LFGQueue &GetQueue(ObjectGuid guid);
        LfgDungeonSet const& GetDungeonsByRandom(uint32 randomdungeon);
        LfgType GetDungeonType(uint32 dungeon);

        void SendLfgBootProposalUpdate(ObjectGuid guid, LfgPlayerBoot const& boot);
        void SendLfgJoinResult(ObjectGuid guid, LfgJoinResultData const& data);
        void SendLfgRoleChosen(ObjectGuid guid, ObjectGuid pguid, uint8 roles);
        void SendLfgRoleCheckUpdate(ObjectGuid guid, LfgRoleCheck const& roleCheck);
        void SendLfgUpdateParty(ObjectGuid guid, LfgUpdateData const& data);
        void SendLfgUpdatePlayer(ObjectGuid guid, LfgUpdateData const& data);
        void SendLfgUpdateProposal(ObjectGuid guid, LfgProposal const& proposal);

        GuidSet const& GetPlayers(ObjectGuid guid);
        LfgGroupData const& GetGroupData(ObjectGuid guid) const; // EG
        LfgPlayerData const& GetPlayerData(ObjectGuid guid) const; // EG

        // General variables
        uint32 m_QueueTimer;                               /// used to check interval of update
        uint32 m_lfgProposalId;                            /// used as internal counter for proposals
        uint32 m_options;                                  /// Stores config options

        LfgQueueContainer QueuesStore;                     /// Queues
        LfgCachedDungeonContainer CachedDungeonMapStore;   /// Stores all dungeons by groupType
        // Reward System
        LfgRewardContainer RewardMapStore;                 /// Stores rewards for random dungeons
        LFGDungeonContainer LfgDungeonStore;
        struct PendingTeleportInData
        {
            time_t ExpireTime = 0;
            uint32 PreviousDungeonId = 0;
            uint32 OldInstanceId = 0;
        };

        std::map<ObjectGuid, PendingTeleportInData> PendingTeleportInStore;
        // EG - groups whose dungeon finished while a backfill was queued, rolechecking or proposed, filled from map threads
        std::mutex _finishedQueueCancelsLock;
        std::map<ObjectGuid, LfgState> _finishedQueueCancels;
        bool _formingGroup = false; // EG
        // Rolecheck - Proposal - Vote Kicks
        LfgRoleCheckContainer RoleChecksStore;             /// Current Role checks
        LfgProposalContainer ProposalsStore;               /// Current Proposals
        LfgPlayerBootContainer BootsStore;                 /// Current player kicks
        LfgPlayerDataContainer PlayersStore;               /// Player data
        LfgGroupDataContainer GroupsStore;                 /// Group data
};

inline int32 format_as(LFGMgrEnum e) { return e; }
inline int32 format_as(LfgProposalState e) { return e; }
inline uint8 format_as(LfgTeleportError e) { return e; }
inline int32 format_as(LfgJoinResult e) { return e; }
inline int32 format_as(LfgRoleCheckState e) { return e; }
} // namespace lfg

#define sLFGMgr lfg::LFGMgr::instance()

#endif
