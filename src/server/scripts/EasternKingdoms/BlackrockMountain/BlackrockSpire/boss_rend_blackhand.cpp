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

#include "blackrock_spire.h"
#include "CommonHelpers.h"
#include "GameObject.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"

enum Spells
{
    SPELL_WHIRLWIND                 = 13736, // sniffed
    SPELL_CLEAVE                    = 15284,
    SPELL_MORTAL_STRIKE             = 16856,
    SPELL_FRENZY                    = 8269,
    SPELL_KNOCKDOWN                 = 13360  // On spawn during Gyth fight
};

enum Says
{
    // Rend Blackhand
    SAY_BLACKHAND_1                 = 0,
    SAY_BLACKHAND_2                 = 1,
    EMOTE_BLACKHAND_DISMOUNT        = 2,
    // Victor Nefarius
    SAY_NEFARIUS_0                  = 0,
    SAY_NEFARIUS_1                  = 1,
    SAY_NEFARIUS_2                  = 2,
    SAY_NEFARIUS_3                  = 3,
    SAY_NEFARIUS_4                  = 4,
    SAY_NEFARIUS_5                  = 5,
    SAY_NEFARIUS_6                  = 6,
    SAY_NEFARIUS_7                  = 7,
    SAY_NEFARIUS_8                  = 8,
    SAY_NEFARIUS_9                  = 9,
};

enum Adds
{
    NPC_CHROMATIC_WHELP             = 10442,
    NPC_CHROMATIC_DRAGONSPAWN       = 10447,
    NPC_BLACKHAND_DRAGON_HANDLER    = 10742
};

enum Misc
{
    NEFARIUS_PATH_1                 = 11037360,
    NEFARIUS_PATH_2                 = 11037368,
    NEFARIUS_PATH_3                 = 11037376,
    REND_PATH_1                     = 11037440,
    REND_PATH_2                     = 11037448,
};

Position const GythLoc =      { 211.762f,  -397.5885f, 111.1817f,  4.747295f   };
Position const Teleport1Loc = { 194.2993f, -474.0814f, 121.4505f, -0.01225555f };
Position const Teleport2Loc = { 216.485f,  -434.93f,   110.888f,  -0.01225555f };
Position const RendCenterPosition = { 124.1f,  -420.4f, 110.5f,  3.1f };

enum Events
{
    EVENT_START_1                   = 1,
    EVENT_START_2                   = 2,
    EVENT_START_3                   = 3,
    EVENT_START_4                   = 4,
    EVENT_TURN_TO_REND              = 5,
    EVENT_TURN_TO_PLAYER            = 6,
    EVENT_TURN_TO_FACING_1          = 7,
    EVENT_TURN_TO_FACING_2          = 8,
    EVENT_TURN_TO_FACING_3          = 9,
    EVENT_WAVE_1                    = 10,
    EVENT_WAVE_2                    = 11,
    EVENT_WAVE_3                    = 12,
    EVENT_WAVE_4                    = 13,
    EVENT_WAVE_5                    = 14,
    EVENT_WAVE_6                    = 15,
    EVENT_WAVES_TEXT_1              = 16,
    EVENT_WAVES_TEXT_2              = 17,
    EVENT_WAVES_TEXT_3              = 18,
    EVENT_WAVES_TEXT_4              = 19,
    EVENT_WAVES_TEXT_5              = 20,
    EVENT_WAVES_COMPLETE_TEXT_1     = 21,
    EVENT_WAVES_COMPLETE_TEXT_2     = 22,
    EVENT_WAVES_COMPLETE_TEXT_3     = 23,
    EVENT_WAVES_EMOTE_1             = 24,
    EVENT_WAVES_EMOTE_2             = 25,
    EVENT_PATH_REND                 = 26,
    EVENT_PATH_NEFARIUS             = 27,
    EVENT_TELEPORT_1                = 28,
    EVENT_TELEPORT_2                = 29,
    EVENT_WHIRLWIND                 = 30,
    EVENT_CLEAVE                    = 31,
    EVENT_MORTAL_STRIKE             = 32,
    EVENT_WAVE_7                    = 33,
};

struct boss_rend_blackhand : public BossAI
{
    boss_rend_blackhand(Creature* creature) : BossAI(creature, DATA_WARCHIEF_REND_BLACKHAND)
    {
        _finalWave = false;
        _victorGUID.Clear();
        _portcullisGUID.Clear();
    }

    void Reset() override
    {
        if (instance->GetBossState(DATA_GYTH) != DONE)
        {
            _finalWave = false;
            if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
            {
                victor->GetMotionMaster()->MoveIdle();
                victor->GetMotionMaster()->MovePoint(0, victor->GetHomePosition());
            }

            if (instance->GetBossState(DATA_WARCHIEF_REND_BLACKHAND) != IN_PROGRESS)
            {
                ObjectGuid door = instance->GetGuidData(GO_PORTCULLIS_ACTIVE);
                if (!door.IsEmpty())
                    instance->HandleGameObject(door, true);
            }

            summons.DespawnAll();
        }
        else
        {
            if (Creature* victor = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS, 50.0f, true))
                _victorGUID = victor->GetGUID();
            if (GameObject* portcullis = me->FindNearestGameObject(GO_DR_PORTCULLIS, 50.0f))
                _portcullisGUID = portcullis->GetGUID();

            if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
            {
                victor->GetMotionMaster()->MoveIdle();
                victor->GetMotionMaster()->MovePoint(0, victor->GetHomePosition());
            }
        }

        instance->SetBossState(DATA_WARCHIEF_REND_BLACKHAND, NOT_STARTED);
        events.Reset();
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        events.ScheduleEvent(EVENT_WHIRLWIND, 13s, 15s);
        events.ScheduleEvent(EVENT_CLEAVE, 15s, 17s);
        events.ScheduleEvent(EVENT_MORTAL_STRIKE, 17s, 19s);
    }

    void IsSummonedBy(WorldObject* /*summoner*/) override
    {
        me->SetImmuneToPC(false);
        DoZoneInCombat();
        instance->SetBossState(DATA_WARCHIEF_REND_BLACKHAND, IN_PROGRESS);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        summons.Despawn(summon);

        if (_finalWave && summons.empty())
        {
            events.ScheduleEvent(EVENT_WAVES_COMPLETE_TEXT_1, 20s);
            _finalWave = false;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);

        if (summon->GetEntry() == NPC_GYTH)
            return;

        Position summonPos = me->GetPosition();
        summon->SetWalk(true);
        Position destination = RendCenterPosition;
        summon->MovePosition(destination, frand(0.f, 20.f), destination.GetOrientation());
        summon->GetMotionMaster()->MovePoint(0, destination);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* victor = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS, 150.0f, true))
        {
            victor->GetMotionMaster()->MoveIdle();
            DoAddEvent(5s, new Trinity::Helpers::Events::GenericEvent(victor, [](WorldObject* owner)
            {
                if (Creature* victor = owner->ToCreature())
                {
                    if (Unit* player = victor->SelectNearestPlayer(60.0f))
                        victor->SetFacingToObject(player);
                    victor->AI()->Talk(10);
                    if (GameObject* portcullis1 = victor->FindNearestGameObject(GO_PORTCULLIS_ACTIVE, 65.0f))
                        portcullis1->SetGoState(GO_STATE_ACTIVE);
                    if (GameObject* portcullis2 = victor->FindNearestGameObject(GO_PORTCULLIS_TOBOSSROOMS, 80.0f))
                        portcullis2->SetGoState(GO_STATE_ACTIVE);
                }
                return true;
            }), victor);
            DoAddEvent(5s + 4s, new Trinity::Helpers::Events::GenericEvent(victor, [](WorldObject* owner)
            {
                if (Creature* victor = owner->ToCreature())
                {
                    victor->CastSpell(victor, 16354);
                    victor->DespawnOrUnsummon(1s);
                }
                return true;
            }), victor);
        }
        _JustDied();
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == AREATRIGGER && data == AREATRIGGER_BLACKROCK_STADIUM)
        {
            if (instance->GetBossState(DATA_GYTH) != DONE && instance->GetBossState(DATA_GYTH) != IN_PROGRESS && instance->GetBossState(DATA_WARCHIEF_REND_BLACKHAND) != DONE)
            {
                if (Creature* victor = me->FindNearestCreature(NPC_LORD_VICTOR_NEFARIUS, 50.0f, true))
                    _victorGUID = victor->GetGUID();
                if (GameObject* portcullis = me->FindNearestGameObject(GO_DR_PORTCULLIS, 50.0f))
                    _portcullisGUID = portcullis->GetGUID();
                instance->SetBossState(DATA_GYTH, IN_PROGRESS);
                events.ScheduleEvent(EVENT_TURN_TO_PLAYER, 0s);
                events.ScheduleEvent(EVENT_START_1, 1s);
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == WAYPOINT_MOTION_TYPE)
        {
            switch (id)
            {
                case 5:
                    events.ScheduleEvent(EVENT_TELEPORT_1, 2s);
                    break;
                case 11:
                    if (Creature* gyth = me->FindNearestCreature(NPC_GYTH, 10.0f, true))
                        gyth->AI()->SetData(1, 1);
                    me->DespawnOrUnsummon(1s, 7_days);
                    break;
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (instance->GetBossState(DATA_WARCHIEF_REND_BLACKHAND) != IN_PROGRESS && instance->GetBossState(DATA_WARCHIEF_REND_BLACKHAND) != DONE)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_START_1:
                    {
                        ObjectGuid door = instance->GetGuidData(GO_PORTCULLIS_ACTIVE);
                        if (!door.IsEmpty())
                            instance->HandleGameObject(door, false);

                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_0);
                        events.ScheduleEvent(EVENT_START_2, 4s);
                        break;
                    }
                    case EVENT_START_2:
                        events.ScheduleEvent(EVENT_TURN_TO_PLAYER, 0s);
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                        events.ScheduleEvent(EVENT_START_3, 4s);
                        break;
                    case EVENT_START_3:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_1);
                        events.ScheduleEvent(EVENT_WAVE_1, 2s);
                        events.ScheduleEvent(EVENT_TURN_TO_REND, 4s);
                        events.ScheduleEvent(EVENT_WAVES_TEXT_1, 56s);
                        break;
                    case EVENT_TURN_TO_REND:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                        {
                            victor->SetFacingToObject(me);
                            victor->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        }
                        break;
                    case EVENT_TURN_TO_PLAYER:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            if (Unit* player = victor->SelectNearestPlayer(60.0f))
                                victor->SetFacingToObject(player);
                        break;
                    case EVENT_TURN_TO_FACING_1:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->SetFacingTo(1.518436f);
                        break;
                    case EVENT_TURN_TO_FACING_2:
                        me->SetFacingTo(1.658063f);
                        break;
                    case EVENT_TURN_TO_FACING_3:
                        me->SetFacingTo(1.500983f);
                        break;
                    case EVENT_WAVES_EMOTE_1:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->HandleEmoteCommand(EMOTE_ONESHOT_QUESTION);
                        break;
                    case EVENT_WAVES_EMOTE_2:
                            me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        break;
                    case EVENT_WAVES_TEXT_1:
                        events.ScheduleEvent(EVENT_TURN_TO_PLAYER, 0s);
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                                victor->AI()->Talk(SAY_NEFARIUS_2);
                        me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
                        events.ScheduleEvent(EVENT_TURN_TO_FACING_1, 4s);
                        events.ScheduleEvent(EVENT_WAVES_EMOTE_1, 5s);
                        events.ScheduleEvent(EVENT_WAVE_2, 2s);
                        events.ScheduleEvent(EVENT_WAVES_TEXT_2, 30s);
                        break;
                    case EVENT_WAVES_TEXT_2:
                        events.ScheduleEvent(EVENT_TURN_TO_PLAYER, 0s);
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_3);
                        events.ScheduleEvent(EVENT_TURN_TO_FACING_1, 4s);
                        events.ScheduleEvent(EVENT_WAVE_3, 2s);
                        events.ScheduleEvent(EVENT_WAVES_TEXT_3, 50s);
                        break;
                    case EVENT_WAVES_TEXT_3:
                        events.ScheduleEvent(EVENT_TURN_TO_PLAYER, 0s);
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_4);
                        events.ScheduleEvent(EVENT_TURN_TO_FACING_1, 4s);
                        events.ScheduleEvent(EVENT_WAVE_4, 2s);
                        events.ScheduleEvent(EVENT_WAVES_TEXT_4, 56s);
                        break;
                    case EVENT_WAVES_TEXT_4:
                        Talk(SAY_BLACKHAND_1);
                        events.ScheduleEvent(EVENT_WAVES_EMOTE_2, 4s);
                        events.ScheduleEvent(EVENT_TURN_TO_FACING_3, 8s);
                        events.ScheduleEvent(EVENT_WAVE_5, 2s);
                        events.ScheduleEvent(EVENT_WAVES_TEXT_5, 56s);
                        break;
                    case EVENT_WAVES_TEXT_5:
                        events.ScheduleEvent(EVENT_TURN_TO_PLAYER, 0s);
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_5);
                        events.ScheduleEvent(EVENT_TURN_TO_FACING_1, 4s);
                        events.ScheduleEvent(EVENT_WAVE_6, 2s);
                        events.ScheduleEvent(EVENT_WAVE_7, 45s);
                        break;
                    case EVENT_WAVES_COMPLETE_TEXT_1:
                        events.ScheduleEvent(EVENT_TURN_TO_PLAYER, 0s);
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_6);
                        events.ScheduleEvent(EVENT_TURN_TO_FACING_1, 4s);
                        events.ScheduleEvent(EVENT_WAVES_COMPLETE_TEXT_2, 13s);
                        break;
                    case EVENT_WAVES_COMPLETE_TEXT_2:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_7);
                        Talk(SAY_BLACKHAND_2);
                        events.ScheduleEvent(EVENT_PATH_REND, 1s);
                        events.ScheduleEvent(EVENT_WAVES_COMPLETE_TEXT_3, 4s);
                        break;
                    case EVENT_WAVES_COMPLETE_TEXT_3:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->AI()->Talk(SAY_NEFARIUS_8);
                        events.ScheduleEvent(EVENT_PATH_NEFARIUS, 1s);
                        events.ScheduleEvent(EVENT_PATH_REND, 1s);
                        break;
                    case EVENT_PATH_NEFARIUS:
                        if (Creature* victor = ObjectAccessor::GetCreature(*me, _victorGUID))
                            victor->GetMotionMaster()->MovePath(NEFARIUS_PATH_1, true);
                        break;
                    case EVENT_PATH_REND:
                        me->GetMotionMaster()->MovePath(REND_PATH_1, false);
                        break;
                    case EVENT_TELEPORT_1:
                        me->NearTeleportTo(194.2993f, -474.0814f, 121.4505f, -0.01225555f);
                        events.ScheduleEvent(EVENT_TELEPORT_2, 50s);
                        break;
                    case EVENT_TELEPORT_2:
                        me->NearTeleportTo(216.485f, -434.93f, 110.888f, -0.01225555f);
                        me->SummonCreature(NPC_GYTH, 211.762f, -397.5885f, 111.1817f, 4.747295f);
                        break;
                    case EVENT_WAVE_1:
                    case EVENT_WAVE_2:
                        if (GameObject* portcullis = ObjectAccessor::GetGameObject(*me, _portcullisGUID))
                            portcullis->UseDoorOrButton();
                        me->SummonCreatureGroup(0);
                        break;
                    case EVENT_WAVE_3:
                    case EVENT_WAVE_4:
                    case EVENT_WAVE_5:
                        if (GameObject* portcullis = ObjectAccessor::GetGameObject(*me, _portcullisGUID))
                            portcullis->UseDoorOrButton();
                        me->SummonCreatureGroup(1);
                        break;
                    case EVENT_WAVE_6:
                        if (GameObject* portcullis = ObjectAccessor::GetGameObject(*me, _portcullisGUID))
                            portcullis->UseDoorOrButton();
                        me->SummonCreatureGroup(2);
                        break;
                    case EVENT_WAVE_7:
                        _finalWave = true;
                        if (GameObject* portcullis = ObjectAccessor::GetGameObject(*me, _portcullisGUID))
                            portcullis->UseDoorOrButton();
                        me->SummonCreatureGroup(3);
                        break;
                    default:
                        break;
                }
            }
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_WHIRLWIND:
                    DoCast(SPELL_WHIRLWIND);
                    events.ScheduleEvent(EVENT_WHIRLWIND, 13s, 18s);
                    break;
                case EVENT_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    events.ScheduleEvent(EVENT_CLEAVE, 10s, 14s);
                    break;
                case EVENT_MORTAL_STRIKE:
                    DoCastVictim(SPELL_MORTAL_STRIKE);
                    events.ScheduleEvent(EVENT_MORTAL_STRIKE, 14s, 16s);
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
        DoMeleeAttackIfReady();
    }

private:
    bool _finalWave;
    ObjectGuid _victorGUID;
    ObjectGuid _portcullisGUID;
};

void AddSC_boss_rend_blackhand()
{
    RegisterBlackrockSpireCreatureAI(boss_rend_blackhand);
}
