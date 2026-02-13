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

#include "ScriptMgr.h"
#include "CellImpl.h"
#include "Containers.h"
#include "GridNotifiersImpl.h"
#include "Log.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "WorldSession.h"

enum ExorcismSpells
{
    SPELL_JULES_GOES_PRONE = 39283,
    SPELL_JULES_THREATENS_AURA = 39284,
    SPELL_JULES_GOES_UPRIGHT = 39294,
    SPELL_JULES_VOMITS_AURA = 39295,

    SPELL_BARADAS_COMMAND = 39277,
    SPELL_BARADA_FALTERS = 39278,
};

enum ExorcismTexts
{
    SAY_BARADA_1 = 0,
    SAY_BARADA_2 = 1,
    SAY_BARADA_3 = 2,
    SAY_BARADA_4 = 3,
    SAY_BARADA_5 = 4,
    SAY_BARADA_6 = 5,
    SAY_BARADA_7 = 6,
    SAY_BARADA_8 = 7,
    SAY_JULES_1 = 0,
    SAY_JULES_2 = 1,
    SAY_JULES_3 = 2,
    SAY_JULES_4 = 3,
    SAY_JULES_5 = 4,
};

Position const exorcismPos[12] =
{
    { -707.42f,  2747.98f,  101.59f,  4.577416f },
    { -711.20f,  2747.75f,  101.59f,  1.51f },
    { -710.84f,  2749.55f,  101.59f,  1.63f },
    { -710.332f, 2754.394f, 102.948f, 3.207566f },
    { -714.261f, 2747.754f, 103.391f, 0.0f },
    { -713.113f, 2750.194f, 103.391f, 0.0f },
    { -710.385f, 2750.896f, 103.391f, 0.0f },
    { -708.309f, 2750.062f, 103.391f, 0.0f },
    { -707.401f, 2747.696f, 103.391f, 0.0f },
    { -708.591f, 2745.266f, 103.391f, 0.0f },
    { -710.597f, 2744.035f, 103.391f, 0.0f },
    { -713.089f, 2745.302f, 103.391f, 0.0f },
};

enum ExorcismMisc
{
    NPC_COLONEL_JULES = 22432,
    NPC_DARKNESS_RELEASED = 22507,
    NPC_FOUL_PURGE = 22506,
    NPC_THE_EXORCISM_BUBBLING_SLIMER_BUNNY = 22505,

    ACTION_START_EVENT = 1,
    ACTION_JULES_HOVER = 2,
    ACTION_JULES_FLIGHT = 3,
    ACTION_JULES_MOVE_HOME = 4,
    ACTION_SUCCESS = 5,
    ACTION_FAIL = 6
};

enum ExorcismEvents
{
    EVENT_BARADAS_1 = 1,
    EVENT_BARADAS_2 = 2,
    EVENT_BARADAS_3 = 3,
    EVENT_BARADAS_4 = 4,
    EVENT_BARADAS_5 = 5,
    EVENT_BARADAS_6 = 6,
    EVENT_BARADAS_7 = 7,
    EVENT_BARADAS_8 = 8,
    EVENT_BARADAS_9 = 9,
    EVENT_BARADAS_10 = 10,
    EVENT_BARADAS_11 = 11,
    EVENT_BARADAS_12 = 12,
    EVENT_BARADAS_13 = 13,
    EVENT_BARADAS_14 = 14,
    EVENT_BARADAS_15 = 15,
    EVENT_BARADAS_16 = 16,
    EVENT_BARADAS_17 = 17,
    EVENT_BARADAS_18 = 18,
    EVENT_BARADAS_19 = 19,
    EVENT_BARADAS_20 = 20,
    EVENT_BARADAS_21 = 21,
    EVENT_BARADAS_22 = 22,
    EVENT_RESET = 23
};

/*######
## npc_colonel_jules
######*/
struct npc_colonel_jules : public ScriptedAI
{
    npc_colonel_jules(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        _point = 4;
        _success = false;
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        me->AddAura(SPELL_JULES_GOES_PRONE, me);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_JULES_HOVER:
                me->AddAura(SPELL_JULES_THREATENS_AURA, me);
                me->SetCanFly(true);
                me->SetWalk(true);
                me->SetFacingTo(3.207566f);
                me->GetMotionMaster()->MoveJump(exorcismPos[3], 2.0f, 2.0f);
                _success = false;
                break;
            case ACTION_JULES_FLIGHT:
                me->RemoveAura(SPELL_JULES_GOES_PRONE);
                me->AddAura(SPELL_JULES_GOES_UPRIGHT, me);
                me->AddAura(SPELL_JULES_VOMITS_AURA, me);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(4, exorcismPos[4]);
                break;
            case ACTION_JULES_MOVE_HOME:
            {
                me->SetWalk(true);
                me->GetMotionMaster()->MoveTargetedHome();
                me->SetCanFly(false);
                me->AddAura(SPELL_JULES_GOES_PRONE, me);
                me->RemoveAura(SPELL_JULES_GOES_UPRIGHT);
                me->RemoveAura(SPELL_JULES_VOMITS_AURA);
                me->RemoveAura(SPELL_JULES_THREATENS_AURA);

                std::list<Creature*> npcs;
                me->GetCreatureListWithOptionsInGrid(npcs, 40.f, FindCreatureOptions{ .CreatureIds = { NPC_DARKNESS_RELEASED, NPC_FOUL_PURGE, NPC_THE_EXORCISM_BUBBLING_SLIMER_BUNNY } });
                for (Creature* npc : npcs)
                    npc->DespawnOrUnsummon();
                break;
            }
            case ACTION_SUCCESS:
                _success = true;
                break;
            case ACTION_FAIL:
                _success = false;
                break;
            default:
                break;
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id == 9)
            _point = 4;

        me->GetMotionMaster()->MovePoint(_point++, exorcismPos[_point]);
    }

    bool OnGossipHello(Player* player) override
    {
        if (_success)
            player->KilledMonsterCredit(NPC_COLONEL_JULES, ObjectGuid::Empty);

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

private:
    uint8 _point;
    bool _success;
};

/*######
## npc_barada
######*/
struct npc_barada : public ScriptedAI
{
    npc_barada(Creature* creature) : ScriptedAI(creature) {}

    void Reset() override
    {
        _events.Reset();
        _playerGUID.Clear();
        me->RemoveUnitFlag(UNIT_FLAG_PACIFIED);
        me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        ClearGossipMenuFor(player);
        if (gossipListId == 1)
        {
            if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                jules->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            player->PlayerTalkClass->SendCloseGossip();
            me->AI()->Talk(SAY_BARADA_1);
            me->AI()->DoAction(ACTION_START_EVENT);
        }
        return false;
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_START_EVENT)
        {
            if (Creature* jules = me->FindNearestCreature(NPC_COLONEL_JULES, 20.0f))
            {
                _julesGUID = jules->GetGUID();
                jules->AI()->Talk(SAY_JULES_1);
            }
            me->SetWalk(true);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            Talk(SAY_BARADA_2);
            me->SetUnitFlag(UNIT_FLAG_PACIFIED);
            me->GetMotionMaster()->MovePoint(0, exorcismPos[0]);
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id == 0)
            me->GetMotionMaster()->MovePoint(1, exorcismPos[1]);
        else if (id == 1)
            me->GetMotionMaster()->MovePoint(2, exorcismPos[2]);
        else if (id == 2)
            _events.ScheduleEvent(EVENT_BARADAS_1, 2s);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
        {
            jules->AI()->DoAction(ACTION_JULES_MOVE_HOME);
            jules->RemoveAllAuras();
            jules->AddAura(SPELL_JULES_GOES_PRONE, jules);
        }
        me->DespawnOrUnsummon(5s, 10s);

        std::list<Creature*> npcs;
        me->GetCreatureListWithOptionsInGrid(npcs, 40.f, FindCreatureOptions{ .CreatureIds = { NPC_DARKNESS_RELEASED, NPC_FOUL_PURGE, NPC_THE_EXORCISM_BUBBLING_SLIMER_BUNNY } });
        for (Creature* npc : npcs)
            npc->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BARADAS_1:
                    me->SetFacingTo(1.513286f);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                    _events.ScheduleEvent(EVENT_BARADAS_2, 3s);
                    break;
                case EVENT_BARADAS_2:
                    DoCast(SPELL_BARADAS_COMMAND);
                    _events.ScheduleEvent(EVENT_BARADAS_3, 5s);
                    break;
                case EVENT_BARADAS_3:
                    Talk(SAY_BARADA_3);
                    _events.ScheduleEvent(EVENT_BARADAS_4, 7s);
                    break;
                case EVENT_BARADAS_4:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_2);
                    _events.ScheduleEvent(EVENT_BARADAS_5, 18s);
                    break;
                case EVENT_BARADAS_5:
                    DoCast(SPELL_BARADA_FALTERS);
                    me->HandleEmoteCommand(EMOTE_STAND_STATE_NONE);
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->DoAction(ACTION_JULES_HOVER);
                    _events.ScheduleEvent(EVENT_BARADAS_6, 11s);
                    break;
                case EVENT_BARADAS_6:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_3);
                    _events.ScheduleEvent(EVENT_BARADAS_7, 13s);
                    break;
                case EVENT_BARADAS_7:
                    Talk(SAY_BARADA_4);
                    _events.ScheduleEvent(EVENT_BARADAS_8, 5s);
                    break;
                case EVENT_BARADAS_8:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_3);
                    _events.ScheduleEvent(EVENT_BARADAS_9, 13s);
                    break;
                case EVENT_BARADAS_9:
                    Talk(SAY_BARADA_4);
                    _events.ScheduleEvent(EVENT_BARADAS_10, 12s);
                    break;
                case EVENT_BARADAS_10:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_4);
                    _events.ScheduleEvent(EVENT_BARADAS_11, 12s);
                    break;
                case EVENT_BARADAS_11:
                    Talk(SAY_BARADA_4);
                    _events.ScheduleEvent(EVENT_BARADAS_12, 5s);
                    break;
                case EVENT_BARADAS_12:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->DoAction(ACTION_JULES_FLIGHT);
                    _events.ScheduleEvent(EVENT_BARADAS_13, 10s);
                    break;
                case EVENT_BARADAS_13:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_4);
                    _events.ScheduleEvent(EVENT_BARADAS_14, 8s);
                    break;
                case EVENT_BARADAS_14:
                    Talk(SAY_BARADA_5);
                    _events.ScheduleEvent(EVENT_BARADAS_15, 10s);
                    break;
                case EVENT_BARADAS_15:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_4);
                    _events.ScheduleEvent(EVENT_BARADAS_16, 10s);
                    break;
                case EVENT_BARADAS_16:
                    Talk(SAY_BARADA_6);
                    _events.ScheduleEvent(EVENT_BARADAS_17, 10s);
                    break;
                case EVENT_BARADAS_17:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_5);
                    _events.ScheduleEvent(EVENT_BARADAS_18, 10s);
                    break;
                case EVENT_BARADAS_18:
                    Talk(SAY_BARADA_7);
                    _events.ScheduleEvent(EVENT_BARADAS_19, 10s);
                    break;
                case EVENT_BARADAS_19:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->Talk(SAY_JULES_3);
                    _events.ScheduleEvent(EVENT_BARADAS_20, 10s);
                    break;
                case EVENT_BARADAS_20:
                    Talk(SAY_BARADA_7);
                    _events.ScheduleEvent(EVENT_BARADAS_21, 10s);
                    break;
                case EVENT_BARADAS_21:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                        jules->AI()->DoAction(ACTION_JULES_MOVE_HOME);
                    _events.ScheduleEvent(EVENT_BARADAS_22, 1s);
                    break;
                case EVENT_BARADAS_22:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                    {
                        jules->AI()->DoAction(ACTION_SUCCESS);
                        jules->RemoveAllAuras();
                        jules->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        jules->SetUnitFlag(UNIT_FLAG_STUNNED);
                        jules->AddAura(SPELL_JULES_GOES_PRONE, jules);
                    }
                    me->RemoveAura(SPELL_BARADAS_COMMAND);
                    me->RemoveUnitFlag(UNIT_FLAG_PACIFIED);
                    Talk(SAY_BARADA_8);
                    me->GetMotionMaster()->MoveTargetedHome();
                    EnterEvadeMode();
                    me->SetWalk(true);
                    _events.ScheduleEvent(EVENT_RESET, 45s);
                    break;
                case EVENT_RESET:
                    if (Creature* jules = ObjectAccessor::GetCreature(*me, _julesGUID))
                    {
                        jules->AI()->DoAction(ACTION_FAIL);
                        jules->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                        jules->RemoveUnitFlag(UNIT_FLAG_STUNNED);
                        jules->DespawnOrUnsummon(10s, 5s);
                    }
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap _events;
    ObjectGuid _julesGUID;
    ObjectGuid _playerGUID;
};

enum Aledis
{
    SAY_CHALLENGE = 0,
    SAY_DEFEATED = 1,
    EVENT_TALK = 1,
    EVENT_ATTACK = 2,
    EVENT_EVADE = 3,
    EVENT_FIREBALL = 4,
    EVENT_FROSTNOVA = 5,
    SPELL_FIREBALL = 20823,
    SPELL_FROSTNOVA = 11831
};

class npc_magister_aledis : public CreatureScript
{
public:
    npc_magister_aledis() : CreatureScript("npc_magister_aledis") { }

    struct npc_magister_aledisAI : public ScriptedAI
    {
        npc_magister_aledisAI(Creature* creature) : ScriptedAI(creature) { }

        void StartFight(Player* player)
        {
            me->Dismount();
            me->SetFacingToObject(player);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            _playerGUID = player->GetGUID();
            _events.ScheduleEvent(EVENT_TALK, 2s);
        }

        void Reset() override
        {
            me->RestoreFaction();
            me->RemoveNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
            me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->SetImmuneToPC(true);
        }

        void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (damage > me->GetHealth() || me->HealthBelowPctDamaged(20, damage))
            {
                damage = 0;

                _events.Reset();
                me->RestoreFaction();
                me->RemoveAllAuras();
                me->CombatStop(true);
                EngagementOver();
                me->SetNpcFlag(UNIT_NPC_FLAG_QUESTGIVER);
                me->SetImmuneToPC(true);
                Talk(SAY_DEFEATED);

                _events.ScheduleEvent(EVENT_EVADE, 1min);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            _events.Update(diff);

            while (uint32 eventId = _events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_TALK:
                    Talk(SAY_CHALLENGE);
                    _events.ScheduleEvent(EVENT_ATTACK, 2s);
                    break;
                case EVENT_ATTACK:
                    me->SetImmuneToPC(false);
                    me->SetFaction(FACTION_MONSTER_2);
                    me->EngageWithTarget(ObjectAccessor::GetPlayer(*me, _playerGUID));
                    _events.ScheduleEvent(EVENT_FIREBALL, 1ms);
                    _events.ScheduleEvent(EVENT_FROSTNOVA, 5s);
                    break;
                case EVENT_FIREBALL:
                    DoCast(SPELL_FIREBALL);
                    _events.ScheduleEvent(EVENT_FIREBALL, 10s);
                    break;
                case EVENT_FROSTNOVA:
                    DoCastAOE(SPELL_FROSTNOVA);
                    _events.ScheduleEvent(EVENT_FROSTNOVA, 20s);
                    break;
                case EVENT_EVADE:
                    EnterEvadeMode();
                    break;
                }
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
        {
            CloseGossipMenuFor(player);
            me->StopMoving();
            StartFight(player);
            return true;
        }

    private:
        EventMap _events;
        ObjectGuid _playerGUID;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_magister_aledisAI(creature);
    }
};

enum WatchCommanderLeonus
{
    SAY_COVER = 0,

    EVENT_START = 1,
    EVENT_CAST  = 2,
    EVENT_END   = 3,

    GAME_EVENT_HELLFIRE = 85,

    NPC_INFERNAL_RAIN   = 18729,
    NPC_FEAR_CONTROLLER = 19393,
    SPELL_INFERNAL_RAIN = 33814,
    SPELL_FEAR          = 33815  // Serverside spell
};

struct npc_watch_commander_leonus : public ScriptedAI
{
    npc_watch_commander_leonus(Creature* creature) : ScriptedAI(creature) { }

    void OnGameEvent(bool start, uint16 eventId) override
    {
        if (eventId == GAME_EVENT_HELLFIRE && start)
        {
            _events.Reset();
            _events.ScheduleEvent(EVENT_START, 1s);
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
                {
                    Talk(SAY_COVER);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);

                    std::list<Creature*> dummies;
                    for (uint32 entry : { NPC_INFERNAL_RAIN, NPC_FEAR_CONTROLLER })
                    {
                        Trinity::AllCreaturesOfEntryInRange pred(me, entry);
                        Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, dummies, pred);
                        Cell::VisitAllObjects(me, searcher, 500.0f);
                    }

                    for (Creature* dummy : dummies)
                        if (dummy->GetCreatureData()->movementType == 0)
                            dummy->AI()->SetData(EVENT_START, 0);
                    break;
                }
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
};

struct npc_infernal_rain_hellfire : public ScriptedAI
{
    npc_infernal_rain_hellfire(Creature* creature) : ScriptedAI(creature) { }

    void SetData(uint32 type, uint32 /*data*/) override
    {
        if (type != EVENT_START)
            return;

        RebuildTargetList();
        _events.ScheduleEvent(EVENT_CAST, 0s, 1s);
        _events.ScheduleEvent(EVENT_END, 1min);
    }

    void RebuildTargetList()
    {
        _targets.clear();

        std::vector<Creature*> others;
        Trinity::AllCreaturesOfEntryInRange pred(me, NPC_INFERNAL_RAIN);
        Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, others, pred);
        Cell::VisitAllObjects(me, searcher, 500.0f);
        for (Creature* other : others)
            if (other->GetCreatureData()->movementType == 2)
                _targets.push_back(other->GetGUID());
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CAST:
                {
                    if (Creature* target = ObjectAccessor::GetCreature(*me, Trinity::Containers::SelectRandomContainerElement(_targets)))
                    {
                        CastSpellExtraArgs args;
                        args.AddSpellMod(SPELLVALUE_MAX_TARGETS, 1);
                        me->CastSpell(target, SPELL_INFERNAL_RAIN, args);
                    }

                    _events.Repeat(1s, 2s);
                    break;
                }
                case EVENT_END:
                    _events.Reset();
                    break;
            }
        }
    }

    private:
        EventMap _events;
        std::vector<ObjectGuid> _targets;
};

struct npc_fear_controller : public ScriptedAI
{
    npc_fear_controller(Creature* creature) : ScriptedAI(creature) { }

    void SetData(uint32 type, uint32 /*data*/) override
    {
        if (type != EVENT_START)
            return;

        _events.ScheduleEvent(EVENT_CAST, 0s, 1s);
        _events.ScheduleEvent(EVENT_END, 1min);
    }

    void UpdateAI(uint32 diff) override
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CAST:
                    DoCastAOE(SPELL_FEAR);
                    _events.Repeat(10s);
                    break;
                case EVENT_END:
                    _events.Reset();
                    break;
            }
        }
    }

    private:
        EventMap _events;
};

/*######
## Quest 10909: Fel Spirits
######*/

enum FelSpirits
{
    SPELL_SEND_VENGEANCE_TO_PLAYER   = 39202,
    SPELL_SUMMON_FEL_SPIRIT          = 39206
};

// 39190 - Send Vengeance
class spell_hellfire_peninsula_send_vengeance : public SpellScript
{
    PrepareSpellScript(spell_hellfire_peninsula_send_vengeance);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SEND_VENGEANCE_TO_PLAYER });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (TempSummon* target = GetHitUnit()->ToTempSummon())
            if (Unit* summoner = target->GetSummonerUnit())
                target->CastSpell(summoner, SPELL_SEND_VENGEANCE_TO_PLAYER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hellfire_peninsula_send_vengeance::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 39202 - Send Vengeance to Player
class spell_hellfire_peninsula_send_vengeance_to_player : public SpellScript
{
    PrepareSpellScript(spell_hellfire_peninsula_send_vengeance_to_player);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_FEL_SPIRIT });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SUMMON_FEL_SPIRIT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hellfire_peninsula_send_vengeance_to_player::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

enum Translocation
{
    SPELL_TRANSLOCATION_FALCON_WATCH_TOWER_DOWN     = 30140,
    SPELL_TRANSLOCATION_FALCON_WATCH_TOWER_UP       = 30141
};

// 25650 - Translocate
// 25652 - Translocate
class spell_hellfire_peninsula_translocation_falcon_watch : public SpellScript
{
    PrepareSpellScript(spell_hellfire_peninsula_translocation_falcon_watch);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ _triggeredSpellId });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), _triggeredSpellId);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_hellfire_peninsula_translocation_falcon_watch::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }

    uint32 _triggeredSpellId;

public:
    explicit spell_hellfire_peninsula_translocation_falcon_watch(Translocation triggeredSpellId) : _triggeredSpellId(triggeredSpellId) { }
};

/*######
## Quest 9361: Helboar, the Other White Meat
######*/

enum HelboarTheOtherWhiteMeat
{
    SPELL_SUMMON_PURIFIED_HELBOAR_MEAT      = 29277,
    SPELL_SUMMON_TOXIC_HELBOAR_MEAT         = 29278
};

// 29200 - Purify Helboar Meat
class spell_hellfire_peninsula_purify_helboar_meat : public SpellScript
{
    PrepareSpellScript(spell_hellfire_peninsula_purify_helboar_meat);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_PURIFIED_HELBOAR_MEAT, SPELL_SUMMON_TOXIC_HELBOAR_MEAT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), roll_chance_i(50) ? SPELL_SUMMON_PURIFIED_HELBOAR_MEAT : SPELL_SUMMON_TOXIC_HELBOAR_MEAT);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_hellfire_peninsula_purify_helboar_meat::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

/*######
## Quest 10813: The Eyes of Grillok
######*/

enum TheEyesOfGrillok
{
    SPELL_EYE_OF_GRILLOK      = 38495
};

// 38554 - Absorb Eye of Grillok
class spell_hellfire_peninsula_absorb_eye_of_grillok : public AuraScript
{
    PrepareAuraScript(spell_hellfire_peninsula_absorb_eye_of_grillok);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EYE_OF_GRILLOK });
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        PreventDefaultAction();

        if (Unit* caster = GetCaster())
            GetTarget()->CastSpell(caster, SPELL_EYE_OF_GRILLOK, aurEff);

        if (Creature* target = GetTarget()->ToCreature())
        {
            /// @todo: This is a hack, in flight missiles of spells of despawned creatures get cancelled - delay despawning by the duration of SPELL_EYE_OF_GRILLOK aura
            target->SetVisible(false);
            target->DespawnOrUnsummon(5s);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_hellfire_peninsula_absorb_eye_of_grillok::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

/*######
## Quest 10838: The Demoniac Scryer
######*/

enum TheDemoniacScryer
{
    SPELL_SUMMON_DEMONAIC_VISITATION      = 38991
};

// 38708 - Demonaic Visitation
class spell_hellfire_peninsula_demonaic_visitation : public AuraScript
{
    PrepareAuraScript(spell_hellfire_peninsula_demonaic_visitation);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_DEMONAIC_VISITATION });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_SUMMON_DEMONAIC_VISITATION, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_hellfire_peninsula_demonaic_visitation::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

void AddSC_hellfire_peninsula()
{
    RegisterCreatureAI(npc_colonel_jules);
    RegisterCreatureAI(npc_barada);
    new npc_magister_aledis();
    RegisterCreatureAI(npc_watch_commander_leonus);
    RegisterCreatureAI(npc_infernal_rain_hellfire);
    RegisterCreatureAI(npc_fear_controller);
    RegisterSpellScript(spell_hellfire_peninsula_send_vengeance);
    RegisterSpellScript(spell_hellfire_peninsula_send_vengeance_to_player);
    RegisterSpellScriptWithArgs(spell_hellfire_peninsula_translocation_falcon_watch, "spell_hellfire_peninsula_translocation_falcon_watch_tower_down", SPELL_TRANSLOCATION_FALCON_WATCH_TOWER_DOWN);
    RegisterSpellScriptWithArgs(spell_hellfire_peninsula_translocation_falcon_watch, "spell_hellfire_peninsula_translocation_falcon_watch_tower_up", SPELL_TRANSLOCATION_FALCON_WATCH_TOWER_UP);
    RegisterSpellScript(spell_hellfire_peninsula_purify_helboar_meat);
    RegisterSpellScript(spell_hellfire_peninsula_absorb_eye_of_grillok);
    RegisterSpellScript(spell_hellfire_peninsula_demonaic_visitation);
    RegisterSpellScript(spell_hellfire_peninsula_demonaic_visitation);
}
