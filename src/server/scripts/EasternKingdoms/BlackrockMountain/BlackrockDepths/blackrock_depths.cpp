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

#include "blackrock_depths.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "InstanceScript.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "Random.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"
#include "WorldSession.h"

class go_shadowforge_brazier : public GameObjectScript
{
public:
    go_shadowforge_brazier() : GameObjectScript("go_shadowforge_brazier") { }

    struct go_shadowforge_brazierAI : public GameObjectAI
    {
        go_shadowforge_brazierAI(GameObject* go) : GameObjectAI(go), _instance(go->GetInstanceScript()) { }

        bool OnGossipHello(Player* /*player*/) override
        {
            if (_instance->GetData(TYPE_LYCEUM) == IN_PROGRESS)
                _instance->SetData(TYPE_LYCEUM, DONE);
            else
                _instance->SetData(TYPE_LYCEUM, IN_PROGRESS);

            if (_instance->GetData(TYPE_LYCEUM) == DONE)
            {
                _instance->HandleGameObject(_instance->GetGuidData(DATA_GOLEM_DOOR_N), true);
                _instance->HandleGameObject(_instance->GetGuidData(DATA_GOLEM_DOOR_S), true);
            }

            return false;
        }

    private:
        InstanceScript* _instance;
    };

    GameObjectAI* GetAI(GameObject* go) const override
    {
        return GetBlackrockDepthsAI<go_shadowforge_brazierAI>(go);
    }
};

enum GrimstoneNPCs
{
    NPC_GRIMSTONE = 10096,
    NPC_THELDREN = 16059,

    MAX_RING_MOB_COUNT = 4
};

enum GrimstoneTexts
{
    SAY_TEXT1 = 0,
    SAY_TEXT2,
    SAY_TEXT3,
    SAY_TEXT4,
    SAY_TEXT5,
    SAY_TEXT6
};

enum GrimstoneEvents
{
    EVENT_START = 1,
    EVENT_RESUME_WALK,
    EVENT_OPEN_ARENA_1,
    EVENT_SUMMON_WAVE_1,
    EVENT_SUMMON_WAVE_2,
    EVENT_SUMMON_WAVE_3,
    EVENT_GRIMSTONE_RETURN,
    EVENT_OPEN_ARENA_2,
    EVENT_SUMMON_BOSS,
    EVENT_COMPLETE
};

static constexpr uint32 PATH_ESCORT_GRIMSTONE = 80770;

uint32 RingMob[] =
{
    8925,   // Dredge Worm
    8926,   // Deep Stinger
    8927,   // Dark Screecher
    8928,   // Burrowing Thundersnout
    8933,   // Cave Creeper
    8932,   // Borer Beetle
};

uint32 RingBoss[] =
{
    9027,   // Gorosh
    9028,   // Grizzle
    9029,   // Eviscerator
    9030,   // Ok'thor
    9031,   // Anub'shiah
    9032,   // Hedrum
};

Position const GrimstoneSpawnPos = { 625.559f, -205.618f, -52.735f, 2.609f };
Position const RingMobSpawnPos   = { 608.960f, -235.322f, -53.907f, 1.857f };
Position const RingBossSpawnPos  = { 644.300f, -175.989f, -53.739f, 3.418f };

class at_ring_of_law : public AreaTriggerScript
{
public:
    at_ring_of_law() : AreaTriggerScript("at_ring_of_law") { }

    bool OnTrigger(Player* player, AreaTriggerEntry const* /*at*/) override
    {
        if (InstanceScript* instance = player->GetInstanceScript())
        {
            if (instance->GetData(TYPE_RING_OF_LAW) == IN_PROGRESS || instance->GetData(TYPE_RING_OF_LAW) == DONE)
                return false;

            instance->SetData(TYPE_RING_OF_LAW, IN_PROGRESS);
            player->SummonCreature(NPC_GRIMSTONE, GrimstoneSpawnPos, TEMPSUMMON_DEAD_DESPAWN);
        }
        return false;
    }
};

// @todo implement quest part of event (different end boss)
struct npc_grimstone : public EscortAI
{
    npc_grimstone(Creature* creature) : EscortAI(creature), _instance(creature->GetInstanceScript()), _mobSpawnId(urand(0, 5)), _mobCount(0), _allMobsSummoned(false), _ringMobsDone(false) { }

    void Reset() override
    {
        _events.Reset();
        _mobCount = 0;
        _bossGUID.Clear();
        _allMobsSummoned = false;
        _ringMobsDone = false;
        _events.ScheduleEvent(EVENT_START, 1s);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetGUID() == _bossGUID)
        {
            _bossGUID.Clear();
            _events.ScheduleEvent(EVENT_COMPLETE, 5s);
            return;
        }

        if (--_mobCount == 0 && _allMobsSummoned)
            _events.ScheduleEvent(EVENT_GRIMSTONE_RETURN, 5s);
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
            case 0:
                Talk(SAY_TEXT1);
                SetEscortPaused(true);
                _events.ScheduleEvent(EVENT_RESUME_WALK, 5s);
                break;
            case 1:
                Talk(SAY_TEXT2);
                SetEscortPaused(true);
                _events.ScheduleEvent(EVENT_OPEN_ARENA_1, 7s);
                break;
            case 2:
                // hold position while ring mobs are alive; skip if they died before we arrived
                if (!_ringMobsDone)
                    SetEscortPaused(true);
                break;
            case 3:
                Talk(SAY_TEXT3);
                break;
            case 4:
                Talk(SAY_TEXT4);
                SetEscortPaused(true);
                _events.ScheduleEvent(EVENT_OPEN_ARENA_2, 5s);
                break;
            case 5:
                _instance->UpdateEncounterStateForKilledCreature(NPC_GRIMSTONE, me);
                _instance->SetData(TYPE_RING_OF_LAW, DONE);
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START:
                    Talk(SAY_TEXT5);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_ARENA4), false);
                    LoadPath(PATH_ESCORT_GRIMSTONE);
                    Start(false);
                    break;
                case EVENT_RESUME_WALK:
                    SetEscortPaused(false);
                    break;
                case EVENT_OPEN_ARENA_1:
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_ARENA1), true);
                    _events.ScheduleEvent(EVENT_SUMMON_WAVE_1, 3s);
                    break;
                case EVENT_SUMMON_WAVE_1:
                    me->SetVisible(false);
                    if (me->SummonCreature(RingMob[_mobSpawnId], RingMobSpawnPos, TEMPSUMMON_DEAD_DESPAWN))
                        ++_mobCount;
                    SetEscortPaused(false);
                    _events.ScheduleEvent(EVENT_SUMMON_WAVE_2, 8s);
                    break;
                case EVENT_SUMMON_WAVE_2:
                    if (me->SummonCreature(RingMob[_mobSpawnId], RingMobSpawnPos, TEMPSUMMON_DEAD_DESPAWN))
                        ++_mobCount;
                    if (me->SummonCreature(RingMob[_mobSpawnId], RingMobSpawnPos, TEMPSUMMON_DEAD_DESPAWN))
                        ++_mobCount;
                    _events.ScheduleEvent(EVENT_SUMMON_WAVE_3, 8s);
                    break;
                case EVENT_SUMMON_WAVE_3:
                    if (me->SummonCreature(RingMob[_mobSpawnId], RingMobSpawnPos, TEMPSUMMON_DEAD_DESPAWN))
                        ++_mobCount;
                    _allMobsSummoned = true;
                    // if all previous mobs died before this wave landed, kick off return now
                    if (_mobCount == 0)
                        _events.ScheduleEvent(EVENT_GRIMSTONE_RETURN, 5s);
                    break;
                case EVENT_GRIMSTONE_RETURN:
                    _ringMobsDone = true;
                    me->SetVisible(true);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_ARENA1), false);
                    Talk(SAY_TEXT6);
                    SetEscortPaused(false);
                    break;
                case EVENT_OPEN_ARENA_2:
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_ARENA2), true);
                    _events.ScheduleEvent(EVENT_SUMMON_BOSS, 5s);
                    break;
                case EVENT_SUMMON_BOSS:
                    me->SetVisible(false);
                    if (Creature* boss = me->SummonCreature(RingBoss[urand(0, 5)], RingBossSpawnPos, TEMPSUMMON_DEAD_DESPAWN))
                        _bossGUID = boss->GetGUID();
                    else
                        _events.ScheduleEvent(EVENT_COMPLETE, 1s);
                    break;
                case EVENT_COMPLETE:
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_ARENA2), false);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_ARENA3), true);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_ARENA4), true);
                    SetEscortPaused(false);
                    break;
                default:
                    break;
            }
        }

        EscortAI::UpdateAI(diff);
    }

private:
    InstanceScript* _instance;
    EventMap _events;
    ObjectGuid _bossGUID;
    uint8 _mobSpawnId;
    uint8 _mobCount;
    bool _allMobsSummoned;
    bool _ringMobsDone;
};

enum PhalanxSpells
{
    SPELL_THUNDERCLAP = 8732,
    SPELL_FIREBALLVOLLEY = 22425,
    SPELL_MIGHTYBLOW = 14099
};

enum PhalanxEvents
{
    EVENT_THUNDERCLAP = 1,
    EVENT_FIREBALL_VOLLEY,
    EVENT_MIGHTY_BLOW
};

struct npc_phalanx : public ScriptedAI
{
    npc_phalanx(Creature* creature) : ScriptedAI(creature), _fireballVolleyActive(false) { }

    void Reset() override
    {
        _events.Reset();
        _fireballVolleyActive = false;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _events.ScheduleEvent(EVENT_THUNDERCLAP, 12s);
        _events.ScheduleEvent(EVENT_MIGHTY_BLOW, 15s);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (!_fireballVolleyActive && me->HealthBelowPctDamaged(51, damage))
        {
            _fireballVolleyActive = true;
            _events.ScheduleEvent(EVENT_FIREBALL_VOLLEY, 0s);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_THUNDERCLAP:
                    DoCastVictim(SPELL_THUNDERCLAP);
                    _events.ScheduleEvent(EVENT_THUNDERCLAP, 10s);
                    break;
                case EVENT_FIREBALL_VOLLEY:
                    DoCastVictim(SPELL_FIREBALLVOLLEY);
                    _events.ScheduleEvent(EVENT_FIREBALL_VOLLEY, 15s);
                    break;
                case EVENT_MIGHTY_BLOW:
                    DoCastVictim(SPELL_MIGHTYBLOW);
                    _events.ScheduleEvent(EVENT_MIGHTY_BLOW, 10s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    bool _fireballVolleyActive;
};

enum LokhtosMisc
{
    QUEST_A_BINDING_CONTRACT = 7604,
    ITEM_SULFURON_INGOT = 17203,
    ITEM_THRORIUM_BROTHERHOOD_CONTRACT = 18628,
    SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND = 23059,
    GOSSIP_ITEM_SHOW_ACCESS_MID = 4781,
    GOSSIP_ITEM_SHOW_ACCESS_OID = 0,
};

#define GOSSIP_ITEM_GET_CONTRACT "Get Thorium Brotherhood Contract"

struct npc_lokhtos_darkbargainer : public ScriptedAI
{
    npc_lokhtos_darkbargainer(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            CloseGossipMenuFor(player);
            player->CastSpell(player, SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND, false);
        }
        else if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(me->GetGUID());

        return true;
    }

    bool OnGossipHello(Player* player) override
    {
        InitGossipMenuFor(player, GOSSIP_ITEM_SHOW_ACCESS_MID);
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (me->IsVendor() && player->GetReputationRank(59) >= REP_FRIENDLY)
            AddGossipItemFor(player, GOSSIP_ITEM_SHOW_ACCESS_MID, GOSSIP_ITEM_SHOW_ACCESS_OID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (!player->GetQuestRewardStatus(QUEST_A_BINDING_CONTRACT) &&
            !player->HasItemCount(ITEM_THRORIUM_BROTHERHOOD_CONTRACT, 1, true) &&
            player->HasItemCount(ITEM_SULFURON_INGOT))
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, GOSSIP_ITEM_GET_CONTRACT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }

        if (player->GetReputationRank(59) < REP_FRIENDLY)
            SendGossipMenuFor(player, 3673, me->GetGUID());
        else
            SendGossipMenuFor(player, 3677, me->GetGUID());

        return true;
    }
};

enum RocknotMisc
{
    SAY_GOT_BEER = 0,
    QUEST_ALE = 4295,
    SPELL_DRUNKEN_RAGE = 14872,
    PATH_ESCORT_ROCKNOT = 76026
};

enum RocknotEvents
{
    EVENT_BREAK_KEG = 1,
    EVENT_BREAK_DOOR
};

struct npc_rocknot : public EscortAI
{
    npc_rocknot(Creature* creature) : EscortAI(creature), _instance(creature->GetInstanceScript()) { }

    void Reset() override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;
        _events.Reset();
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
            case 1:
                me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                break;
            case 2:
                me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_UNARMED);
                break;
            case 3:
                me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_UNARMED);
                break;
            case 4:
                me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                break;
            case 5:
                me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                _events.ScheduleEvent(EVENT_BREAK_KEG, 2s);
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BREAK_KEG:
                    if (GameObject* keg = ObjectAccessor::GetGameObject(*me, _instance->GetGuidData(DATA_GO_BAR_KEG)))
                        keg->SetGoState(GO_STATE_ACTIVE);
                    _events.ScheduleEvent(EVENT_BREAK_DOOR, 1s);
                    break;
                case EVENT_BREAK_DOOR:
                    if (GameObject* door = ObjectAccessor::GetGameObject(*me, _instance->GetGuidData(DATA_GO_BAR_DOOR)))
                        door->SetGoState(GO_STATE_DESTROYED);
                    if (GameObject* trap = ObjectAccessor::GetGameObject(*me, _instance->GetGuidData(DATA_GO_BAR_KEG_TRAP)))
                        trap->SetGoState(GO_STATE_ACTIVE);
                    if (Unit* phalanx = ObjectAccessor::GetUnit(*me, _instance->GetGuidData(DATA_PHALANX)))
                        phalanx->SetFaction(FACTION_MONSTER);
                    _instance->SetData(TYPE_BAR, DONE);
                    break;
                default:
                    break;
            }
        }

        EscortAI::UpdateAI(diff);
    }

    void OnQuestReward(Player* /*player*/, Quest const* quest, uint32 /*item*/) override
    {
        if (_instance->GetData(TYPE_BAR) == DONE || _instance->GetData(TYPE_BAR) == SPECIAL)
            return;

        if (quest->GetQuestId() == QUEST_ALE)
        {
            if (_instance->GetData(TYPE_BAR) != IN_PROGRESS)
                _instance->SetData(TYPE_BAR, IN_PROGRESS);

            _instance->SetData(TYPE_BAR, SPECIAL);

            if (_instance->GetData(TYPE_BAR) == SPECIAL)
            {
                Talk(SAY_GOT_BEER);
                DoCastSelf(SPELL_DRUNKEN_RAGE, false);
                LoadPath(PATH_ESCORT_ROCKNOT);
                Start(false);
            }
        }
    }

private:
    InstanceScript* _instance;
    EventMap _events;
};

void AddSC_blackrock_depths()
{
    new go_shadowforge_brazier();
    new at_ring_of_law();
    RegisterBlackrockDepthsCreatureAI(npc_grimstone);
    RegisterBlackrockDepthsCreatureAI(npc_phalanx);
    RegisterBlackrockDepthsCreatureAI(npc_lokhtos_darkbargainer);
    RegisterBlackrockDepthsCreatureAI(npc_rocknot);
}
