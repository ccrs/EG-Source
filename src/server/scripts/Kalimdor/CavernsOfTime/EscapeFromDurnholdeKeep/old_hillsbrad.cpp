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

#include "old_hillsbrad.h"
#include "Group.h"
#include "TemporarySummon.h"
#include "InstanceScript.h"
#include "LFGMgr.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"

/*######
## npc_erozion
######*/

enum ErozionMisc
{
    QUEST_ENTRY_HILLSBRAD = 10282,
    QUEST_ENTRY_DIVERSION = 10283,
    QUEST_ENTRY_ESCAPE = 10284,
    QUEST_ENTRY_RETURN = 10285,
    ITEM_ENTRY_BOMBS = 25853,
    GOSSIP_MENU_EROZION = 7769,
    GOSSIP_OPTION_BOMB = 0
};

struct npc_erozion : public ScriptedAI
{
    npc_erozion(Creature* creature) : ScriptedAI(creature), instance(creature->GetInstanceScript()) { }

    InstanceScript* instance;

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (instance->GetBossState(DATA_EPOCH_HUNTER) == DONE)
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Take me back to my time.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        else if (instance->GetData(TYPE_BARREL_DIVERSION) != DONE && !player->HasItemCount(ITEM_ENTRY_BOMBS))
            AddGossipItemFor(player, GOSSIP_MENU_EROZION, GOSSIP_OPTION_BOMB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        SendGossipMenuFor(player, 9778, me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            ItemPosCountVec dest;
            if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_ENTRY_BOMBS, 1) == EQUIP_ERR_OK)
                player->StoreNewItem(dest, ITEM_ENTRY_BOMBS, true);

            SendGossipMenuFor(player, 9515, me->GetGUID());
        }
        else if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            CloseGossipMenuFor(player);
            Group* group = player->GetGroup();
            if (group && group->isLFGGroup())
                sLFGMgr->TeleportPlayer(player, true);
            else if (AreaTriggerTeleport const* entrance = sObjectMgr->GetMapEntranceTrigger(me->GetMapId()))
                player->TeleportTo(entrance->target_mapId, entrance->target_X, entrance->target_Y, entrance->target_Z, entrance->target_Orientation);
        }

        return true;
    }
};

/*######
## npc_thrall_old_hillsbrad
######*/

enum ThrallTexts
{
    SAY_TH_START_EVENT_PART1 = 0,
    SAY_TH_ARMORY,
    SAY_TH_SKARLOC_MEET,
    SAY_TH_SKARLOC_TAUNT,
    SAY_TH_START_EVENT_PART2,
    SAY_TH_MOUNTS_UP,
    SAY_TH_CHURCH_END,
    SAY_TH_MEET_TARETHA,
    SAY_TH_EPOCH_WONDER,
    SAY_TH_EPOCH_KILL_TARETHA,
    SAY_TH_EVENT_COMPLETE,
    SAY_TH_RANDOM_LOW_HP,
    SAY_TH_RANDOM_DIE,
    SAY_TH_RANDOM_AGGRO,
    SAY_TH_RANDOM_KILL,
    SAY_TH_LEAVE_COMBAT
};

// Must be declared before npc_thrall_old_hillsbrad since Thrall calls Talk on Taretha's AI
enum TarethaTexts
{
    SAY_TA_FREE = 0,
    SAY_TA_ESCAPED
};

enum ThrallSpells
{
    SPELL_STRIKE = 14516,
    SPELL_SHIELD_BLOCK = 12169
};

enum ThrallEvents
{
    EVENT_STRIKE = 1,
    EVENT_SHIELD_BLOCK,
    EVENT_SKARLOC_TAUNT,
    EVENT_SPAWN_WAVE_2,
    EVENT_EPOCH_ENGAGE
};

enum ThrallPaths
{
    PATH_ESCORT_THRALL = 143010,
    PATH_ESCORT_TARETHA = 151098
};

enum ThrallMisc
{
    // Equipment display items
    THRALL_WEAPON_ITEM = 927,
    THRALL_SHIELD_ITEM = 2129,
    THRALL_MODEL_UNEQUIPPED = 17292,
    THRALL_MODEL_EQUIPPED = 18165,

    // NPC entries
    ENTRY_ARMORER = 18764,
    ENTRY_SKARLOC = 17862,
    ENTRY_EPOCH = 18096,
    EROZION_ENTRY = 18723,
    SKARLOC_MOUNT = 18798,
    SKARLOC_MOUNT_MODEL = 18223,

    NPC_RIFLE = 17820,
    NPC_WARDEN = 17833,
    NPC_VETERAN = 17860,
    NPC_WATCHMAN = 17814,
    NPC_SENTRY = 17815,

    NPC_INFINITE_SLAYER = 18170,
    NPC_INFINITE_DEFILER = 18171,
    NPC_INFINITE_SABOTEOR = 18172,

    NPC_TM_GUARDSMAN = 18092,
    NPC_TM_PROTECTOR = 18093,
    NPC_TM_LOOKOUT = 18094,

    NPC_BARN_GUARDSMAN = 23175,
    NPC_BARN_PROTECTOR = 23179,
    NPC_BARN_LOOKOUT = 23177,

    NPC_INN_GUARDSMAN = 23176,
    NPC_INN_PROTECTOR = 23180,
    NPC_INN_LOOKOUT = 23178,

    // Gossip menus and items
    GOSSIP_ID_START = 9568,
    GOSSIP_ID_SKARLOC1 = 9614,
    GOSSIP_ID_SKARLOC2 = 9579,
    GOSSIP_ID_SKARLOC3 = 9580,
    GOSSIP_ID_TARREN = 9597,
    GOSSIP_ITEM_WALKING_MID = 7499,
    GOSSIP_ITEM_DEFAULT_OP = 0,
    GOSSIP_ITEM_TARREN_MID = 7840,
    GOSSIP_ITEM_SKARLOC1_MID = 7830,
    GOSSIP_ITEM_SKARLOC2_MID = 7829
};

constexpr float ThrallSpeedRun = 1.0f;
constexpr float ThrallSpeedMount = 1.6f;

// Epoch Hunter arrival position — outside the inn in Tarren Mill
static Position const EpochSpawnPos = { 2639.13f, 698.55f, 65.43f, 4.59f };

struct npc_thrall_old_hillsbrad : public EscortAI
{
    npc_thrall_old_hillsbrad(Creature* creature) : EscortAI(creature), instance(creature->GetInstanceScript()), _summons(creature)
    {
        me->setActive(true);
        me->SetFarVisible(true);
        _hadMount = false;
        _lowHp = false;
        _epochWave = 0;
        _infiniteAlive = 0;
    }

    InstanceScript* instance;

    void InitializeAI() override
    {
        EscortAI::InitializeAI();
        me->SetRespawnCompatibilityMode(true);
    }

    void DoMount()
    {
        me->Mount(SKARLOC_MOUNT_MODEL);
        me->SetSpeedRate(MOVE_RUN, ThrallSpeedMount);
    }

    void DoUnmount()
    {
        me->Dismount();
        me->SetSpeedRate(MOVE_RUN, ThrallSpeedRun);
    }

    void StartWP()
    {
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        SetEscortPaused(false);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_RESUME_ESCORT:
                me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                SetEscortPaused(false);
                break;
            case ACTION_SKARLOC_TAUNT:
                _events.ScheduleEvent(EVENT_SKARLOC_TAUNT, 3s);
                break;
        }
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
            case 8:
                me->SummonCreature(ENTRY_ARMORER, 2181.87f, 112.46f, 89.45f, 0.26f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 9:
                Talk(SAY_TH_ARMORY);
                me->SetVirtualItem(0, THRALL_WEAPON_ITEM);
                me->SetVirtualItem(1, THRALL_SHIELD_ITEM);
                break;
            case 10:
                me->SetDisplayId(THRALL_MODEL_EQUIPPED);
                break;
            case 15:
                me->SummonCreature(NPC_RIFLE, 2200.28f, 137.37f, 87.93f, 5.07f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_WARDEN, 2197.44f, 131.83f, 87.93f, 0.78f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_VETERAN, 2203.62f, 135.40f, 87.93f, 3.70f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_VETERAN, 2200.75f, 130.13f, 87.93f, 1.48f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 21:
                me->SummonCreature(NPC_RIFLE, 2135.80f, 154.01f, 67.45f, 4.98f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_WARDEN, 2144.36f, 151.87f, 67.74f, 4.46f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_VETERAN, 2142.12f, 154.41f, 67.12f, 4.56f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_VETERAN, 2138.08f, 155.38f, 67.24f, 4.60f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 25:
                me->SummonCreature(NPC_RIFLE, 2102.98f, 192.17f, 65.24f, 6.02f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_WARDEN, 2108.48f, 198.75f, 65.18f, 5.15f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_VETERAN, 2106.11f, 197.29f, 65.18f, 5.63f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_VETERAN, 2104.18f, 194.82f, 65.18f, 5.75f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 29:
                Talk(SAY_TH_SKARLOC_MEET);
                SetEscortPaused(true);
                if (instance->GetBossState(DATA_CAPTAIN_SKARLOC) != DONE)
                {
                    if (TempSummon* skarloc = me->SummonCreature(ENTRY_SKARLOC, 2036.48f, 271.22f, 63.43f, 5.27f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5min))
                    {
                        if (TempSummon* veteran = me->SummonCreature(NPC_VETERAN, 2032.58f, 275.10f, 63.43f, 5.50f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s))
                            veteran->GetMotionMaster()->MoveFollow(skarloc, veteran->GetDistance(skarloc), skarloc->GetRelativeAngle(veteran));
                        if (TempSummon* warden = me->SummonCreature(NPC_WARDEN, 2040.20f, 274.90f, 63.43f, 5.10f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s))
                            warden->GetMotionMaster()->MoveFollow(skarloc, warden->GetDistance(skarloc), skarloc->GetRelativeAngle(warden));
                    }
                }
                else
                    // Wipe recovery: Skarloc already dead, skip straight to mount-ride gossip
                    instance->SetData(TYPE_THRALL_EVENT, OH_ESCORT_HORSE_RIDE);
                break;
            case 31:
                Talk(SAY_TH_MOUNTS_UP);
                _hadMount = true;
                DoMount();
                break;
            case 37:
                me->SummonCreature(NPC_WATCHMAN, 2124.26f, 522.16f, 56.87f, 3.99f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_WATCHMAN, 2121.69f, 525.37f, 57.11f, 4.01f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_SENTRY, 2124.65f, 524.55f, 56.63f, 3.98f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 59:
                me->SummonCreature(SKARLOC_MOUNT, 2488.64f, 625.77f, 58.26f, 4.71f, TEMPSUMMON_TIMED_DESPAWN, 10s);
                DoUnmount();
                _hadMount = false;
                break;
            case 60:
                me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                SetEscortPaused(true);
                instance->SetData(TYPE_THRALL_EVENT, OH_ESCORT_BARN_TO_TARETHA);
                break;
            case 68:
                me->SummonCreature(NPC_BARN_PROTECTOR, 2500.22f, 692.60f, 55.50f, 2.84f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_BARN_LOOKOUT, 2500.13f, 696.55f, 55.51f, 3.38f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_BARN_GUARDSMAN, 2500.55f, 693.64f, 55.50f, 3.14f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_BARN_GUARDSMAN, 2500.94f, 695.81f, 55.50f, 3.14f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 79:
                me->SummonCreature(NPC_TM_GUARDSMAN, 2628.50f, 672.00f, 55.47f, 3.70f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_TM_GUARDSMAN, 2630.20f, 675.40f, 55.32f, 3.42f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_TM_PROTECTOR, 2625.80f, 668.30f, 55.12f, 4.05f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_TM_LOOKOUT, 2632.10f, 668.80f, 55.38f, 4.22f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 83:
                me->SummonCreature(NPC_BARN_PROTECTOR, 2627.33f, 646.82f, 56.03f, 4.28f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_BARN_LOOKOUT, 2624.14f, 648.03f, 56.03f, 4.50f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_BARN_GUARDSMAN, 2625.32f, 649.60f, 56.03f, 4.38f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_BARN_GUARDSMAN, 2627.22f, 649.00f, 56.03f, 4.34f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 84:
                Talk(SAY_TH_CHURCH_END);
                break;
            case 93:
                me->SummonCreature(NPC_INN_PROTECTOR, 2652.71f, 660.31f, 61.93f, 1.67f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_INN_LOOKOUT, 2648.96f, 662.59f, 61.93f, 0.79f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_INN_GUARDSMAN, 2657.36f, 662.34f, 61.93f, 2.68f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_INN_GUARDSMAN, 2656.39f, 659.77f, 61.93f, 2.61f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 94:
                if (Creature* taretha = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_TARETHA)))
                    taretha->AI()->Talk(SAY_TA_ESCAPED, me);
                break;
            case 95:
                Talk(SAY_TH_MEET_TARETHA);
                SetEscortPaused(true);
                instance->SetData(TYPE_THRALL_EVENT, OH_ESCORT_EPOCH_HUNTER);
                // Gossip to progress is on Taretha, not Thrall; no gossip flag needed here
                break;
            case 96:
                Talk(SAY_TH_EPOCH_WONDER);
                break;
            case 97:
                Talk(SAY_TH_EPOCH_KILL_TARETHA);
                break;
            case 104:
                // Epoch arrives immune to watch the fight; wave 1 spawns immediately after
                if (instance->GetBossState(DATA_EPOCH_HUNTER) != DONE)
                {
                    me->SummonCreature(ENTRY_EPOCH, EpochSpawnPos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5min);
                    _epochWave = 1;
                    _infiniteAlive = 3;
                    me->SummonCreature(NPC_INFINITE_DEFILER, 2634.90f, 655.11f, 55.34f, 1.57f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                    me->SummonCreature(NPC_INFINITE_SABOTEOR, 2636.48f, 661.75f, 55.22f, 1.55f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                    me->SummonCreature(NPC_INFINITE_SLAYER, 2629.36f, 658.43f, 55.17f, 1.54f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                    SetEscortPaused(true);
                }
                break;
            case 105:
                // Reached after Epoch dies via ACTION_RESUME_ESCORT; passes through in normal flow
                break;
            case 106:
            {
                for (auto const& ref : me->GetMap()->GetPlayers())
                    if (Player* player = ref.GetSource())
                        player->KilledMonsterCredit(20156);

                if (Creature* taretha = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_TARETHA)))
                    taretha->AI()->DoAction(ACTION_TARETHA_WALK);

                Talk(SAY_TH_EVENT_COMPLETE);
                me->SummonCreature(EROZION_ENTRY, 2646.47f, 680.416f, 55.38f, 4.16f, TEMPSUMMON_MANUAL_DESPAWN);
                break;
            }
            case 108:
                // End of escort — Thrall disappears into his future
                me->SetVisible(false);
                break;
        }
    }

    void Reset() override
    {
        _events.Reset();
        _lowHp = false;
        // Only called on full respawn (not during mid-escort evade; EscortAI::EnterEvadeMode
        // skips Reset() when STATE_ESCORT_ESCORTING is set and moves to ReturnToLastPoint instead)
        _hadMount = false;
        _epochWave = 0;
        _infiniteAlive = 0;
        DoUnmount();
        me->SetVirtualItem(0, 0);
        me->SetVirtualItem(1, 0);
        me->SetDisplayId(THRALL_MODEL_UNEQUIPPED);
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            Talk(SAY_TH_LEAVE_COMBAT);
        EscortAI::EnterEvadeMode(why);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        bool wasReturning = (type == POINT_MOTION_TYPE) && HasEscortState(STATE_ESCORT_RETURNING);
        EscortAI::MovementInform(type, id);
        // Re-mount after returning from mid-escort combat if Thrall was mounted before the fight
        if (wasReturning && !HasEscortState(STATE_ESCORT_RETURNING) && _hadMount)
            DoMount();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_TH_RANDOM_AGGRO);
        if (me->IsMounted())
            DoUnmount();

        _events.ScheduleEvent(EVENT_STRIKE, 5s, 8s);
        _events.ScheduleEvent(EVENT_SHIELD_BLOCK, 15s, 20s);
    }

    void JustSummoned(Creature* summoned) override
    {
        _summons.Summon(summoned);
        switch (summoned->GetEntry())
        {
            case NPC_TM_GUARDSMAN:
            case NPC_TM_PROTECTOR:
            case NPC_TM_LOOKOUT:
            case NPC_BARN_GUARDSMAN:
            case NPC_BARN_PROTECTOR:
            case NPC_BARN_LOOKOUT:
            case NPC_INN_GUARDSMAN:
            case NPC_INN_PROTECTOR:
            case NPC_INN_LOOKOUT:
            case EROZION_ENTRY:
            case ENTRY_EPOCH:
                break;
            case SKARLOC_MOUNT:
                summoned->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
                break;
            default:
                summoned->AI()->AttackStart(me);
                break;
        }
    }

    void KilledUnit(Unit* /*victim*/) override
    {
        Talk(SAY_TH_RANDOM_KILL);
    }

    void JustDied(Unit* killer) override
    {
        _summons.DespawnAll();
        instance->SetData(TYPE_THRALL_EVENT, OH_ESCORT_DEATH_EVENT);

        if (killer != me)
            Talk(SAY_TH_RANDOM_DIE);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        switch (summon->GetEntry())
        {
            case NPC_INFINITE_SLAYER:
            case NPC_INFINITE_DEFILER:
            case NPC_INFINITE_SABOTEOR:
                if (_infiniteAlive == 0 || _epochWave == 0)
                    return;
                if (--_infiniteAlive > 0)
                    return;

                if (_epochWave == 1)
                {
                    _epochWave = 0;
                    _events.ScheduleEvent(EVENT_SPAWN_WAVE_2, 8s);
                }
                else if (_epochWave == 2)
                {
                    _epochWave = 0;
                    _events.ScheduleEvent(EVENT_EPOCH_ENGAGE, 15s);
                }
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        EscortAI::UpdateAI(diff);

        _events.Update(diff);

        if (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPAWN_WAVE_2:
                    _epochWave = 2;
                    _infiniteAlive = 4;
                    me->SummonCreature(NPC_INFINITE_DEFILER,  2633.50f, 653.40f, 55.40f, 1.57f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                    me->SummonCreature(NPC_INFINITE_SABOTEOR, 2637.90f, 660.20f, 55.19f, 1.55f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                    me->SummonCreature(NPC_INFINITE_SLAYER,   2627.10f, 656.30f, 55.13f, 1.54f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                    me->SummonCreature(NPC_INFINITE_SLAYER,   2642.80f, 658.10f, 55.28f, 1.61f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                    break;
                case EVENT_EPOCH_ENGAGE:
                    if (Creature* epoch = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_EPOCH_HUNTER)))
                        epoch->AI()->DoAction(ACTION_EPOCH_ENGAGE);
                    break;
                case EVENT_SKARLOC_TAUNT:
                    Talk(SAY_TH_SKARLOC_TAUNT);
                    break;
                default:
                    break;
            }
        }

        if (!UpdateVictim())
            return;

        if (!_lowHp && HealthBelowPct(20))
        {
            Talk(SAY_TH_RANDOM_LOW_HP);
            _lowHp = true;
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_STRIKE:
                    DoCastVictim(SPELL_STRIKE);
                    _events.Repeat(5s, 8s);
                    break;
                case EVENT_SHIELD_BLOCK:
                    DoCastSelf(SPELL_SHIELD_BLOCK);
                    _events.Repeat(15s, 20s);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
        {
            player->PrepareQuestMenu(me->GetGUID());
            player->SendPreparedQuest(me->GetGUID());
        }

        uint32 const escortState = instance->GetData(TYPE_THRALL_EVENT);

        if (instance->GetBossState(DATA_LIEUTENANT_DRAKE) == DONE && escortState == OH_ESCORT_PRISON_TO_SKARLOC)
        {
            InitGossipMenuFor(player, GOSSIP_ITEM_WALKING_MID);
            AddGossipItemFor(player, GOSSIP_ITEM_WALKING_MID, GOSSIP_ITEM_DEFAULT_OP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, GOSSIP_ID_START, me->GetGUID());
        }
        else if (escortState == OH_ESCORT_HORSE_RIDE)
        {
            InitGossipMenuFor(player, GOSSIP_ITEM_SKARLOC1_MID);
            AddGossipItemFor(player, GOSSIP_ITEM_SKARLOC1_MID, GOSSIP_ITEM_DEFAULT_OP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            SendGossipMenuFor(player, GOSSIP_ID_SKARLOC1, me->GetGUID());
        }
        else if (escortState == OH_ESCORT_BARN_TO_TARETHA)
        {
            InitGossipMenuFor(player, GOSSIP_ITEM_TARREN_MID);
            AddGossipItemFor(player, GOSSIP_ITEM_TARREN_MID, GOSSIP_ITEM_DEFAULT_OP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            SendGossipMenuFor(player, GOSSIP_ID_TARREN, me->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                // Start the escort from prison
                CloseGossipMenuFor(player);
                Talk(SAY_TH_START_EVENT_PART1);
                LoadPath(PATH_ESCORT_THRALL);
                Start(true, player->GetGUID(), nullptr, true);
                SetDespawnAtEnd(false);
                SetDespawnAtFar(false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                // Post-Skarloc: first gossip step
                InitGossipMenuFor(player, GOSSIP_ITEM_SKARLOC2_MID);
                AddGossipItemFor(player, GOSSIP_ITEM_SKARLOC2_MID, GOSSIP_ITEM_DEFAULT_OP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 20);
                SendGossipMenuFor(player, GOSSIP_ID_SKARLOC2, me->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 20:
                // Post-Skarloc: second step — mount ride begins
                SendGossipMenuFor(player, GOSSIP_ID_SKARLOC3, me->GetGUID());
                me->SummonCreature(SKARLOC_MOUNT, 2038.81f, 270.26f, 63.20f, 5.41f, TEMPSUMMON_TIMED_DESPAWN, 12s);
                Talk(SAY_TH_START_EVENT_PART2);
                StartWP();
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                // Continue from Tarren Mill gate
                CloseGossipMenuFor(player);
                StartWP();
                break;
        }
        return true;
    }

private:
    EventMap _events;
    bool _hadMount;
    bool _lowHp;
    uint8 _epochWave;
    uint8 _infiniteAlive;
    SummonList _summons;
};

/*######
## npc_taretha
######*/

enum TarethaGossip
{
    GOSSIP_ID_EPOCH1 = 9610,
    GOSSIP_ID_EPOCH2 = 9613,
    GOSSIP_ITEM_EPOCH1_MID = 7849,
    GOSSIP_ITEM_EPOCH1_OID = 0,
    GOSSIP_ITEM_EPOCH2_MID = 7852,
    GOSSIP_ITEM_EPOCH2_OID = 0
};

struct npc_taretha : public EscortAI
{
    npc_taretha(Creature* creature) : EscortAI(creature), instance(creature->GetInstanceScript()) { }

    InstanceScript* instance;

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
            case 6:
                Talk(SAY_TA_FREE);
                break;
            case 7:
                me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                break;
        }
    }

    void Reset() override { }
    void JustEngagedWith(Unit* /*who*/) override { }

    void DoAction(int32 action) override
    {
        if (action == ACTION_TARETHA_WALK)
        {
            Map::PlayerList const& players = me->GetMap()->GetPlayers();
            ObjectGuid playerGuid;
            if (!players.isEmpty())
                playerGuid = players.begin()->GetSource()->GetGUID();
            LoadPath(PATH_ESCORT_TARETHA);
            Start(false, playerGuid);
            SetDespawnAtFar(false);
        }
    }

    bool OnGossipHello(Player* player) override
    {
        if (instance->GetData(TYPE_THRALL_EVENT) == OH_ESCORT_EPOCH_HUNTER &&
            instance->GetBossState(DATA_EPOCH_HUNTER) != DONE)
        {
            InitGossipMenuFor(player, GOSSIP_ITEM_EPOCH1_MID);
            AddGossipItemFor(player, GOSSIP_ITEM_EPOCH1_MID, GOSSIP_ITEM_EPOCH1_OID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            SendGossipMenuFor(player, GOSSIP_ID_EPOCH1, me->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            InitGossipMenuFor(player, GOSSIP_ITEM_EPOCH2_MID);
            AddGossipItemFor(player, GOSSIP_ITEM_EPOCH2_MID, GOSSIP_ITEM_EPOCH2_OID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            SendGossipMenuFor(player, GOSSIP_ID_EPOCH2, me->GetGUID());
        }
        else if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            CloseGossipMenuFor(player);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);

            // Resume Thrall's walk toward the Epoch Hunter confrontation at WP 104
            if (Creature* thrall = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_THRALL)))
                thrall->AI()->DoAction(ACTION_RESUME_ESCORT);
        }
        return true;
    }
};

/*######
## AddSC
######*/

void AddSC_old_hillsbrad()
{
    RegisterOldHillsbradCreatureAI(npc_erozion);
    RegisterOldHillsbradCreatureAI(npc_thrall_old_hillsbrad);
    RegisterOldHillsbradCreatureAI(npc_taretha);
}
