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

#include "Arena1v1Mgr.h"
#include "Creature.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

enum Arena1v1GossipAction
{
    ARENA_1V1_GOSSIP_STATS = 1,
    ARENA_1V1_GOSSIP_JOIN,
    ARENA_1V1_GOSSIP_LEAVE
};

struct EG_npc_arena1v1 : public ScriptedAI
{
    EG_npc_arena1v1(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "What's my 1v1 rating?", GOSSIP_SENDER_MAIN, ARENA_1V1_GOSSIP_STATS);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Queue me for 1v1 arena.", GOSSIP_SENDER_MAIN, ARENA_1V1_GOSSIP_JOIN);
        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Leave the 1v1 queue.", GOSSIP_SENDER_MAIN, ARENA_1V1_GOSSIP_LEAVE);
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        CloseGossipMenuFor(player);

        switch (action)
        {
            case ARENA_1V1_GOSSIP_STATS:
                sArena1v1Mgr->WhisperStats(player, me);
                break;
            case ARENA_1V1_GOSSIP_JOIN:
                sArena1v1Mgr->HandleJoinQueue(player, me);
                break;
            case ARENA_1V1_GOSSIP_LEAVE:
                sArena1v1Mgr->HandleLeaveQueue(player);
                break;
            default:
                break;
        }
        return true;
    }
};

class EG_groupscript_arena1v1 : public GroupScript
{
public:
    EG_groupscript_arena1v1() : GroupScript("EG_groupscript_arena1v1") { }

    void OnAddMember(Group* /*group*/, ObjectGuid guid) override
    {
        sArena1v1Mgr->CancelQueueIfPresent(guid);
    }
};

void AddSC_EG_npc_arena1v1()
{
    RegisterCreatureAI(EG_npc_arena1v1);
    new EG_groupscript_arena1v1();
}
