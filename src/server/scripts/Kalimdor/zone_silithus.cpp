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

/* ScriptData
SDName: Silithus
SD%Complete: 100
SDComment: Quest support: 8348,8352,8361,8519
SDCategory: Silithus
EndScriptData */

/* ContentData
A Pawn on the Eternal Board - creatures, gameobjects and defines
quest_a_pawn_on_the_eternal_board
npc_qiraj_war_spawn : Adds that are summoned in the Qiraj gates battle.
npc_anachronos_the_ancient : Creature that controls the event.
npc_anachronos_quest_trigger: controls the spawning of the BG War mobs.
go_crystalline_tear : GameObject that begins the event and hands out quest
TO DO: get correct spell IDs and timings for the dragon transformation casts
TO DO: dragons' EMOTE_ONESHOT_LIFTOFF after transformation reportedly does not fire
EndContentData */

#include "ScriptMgr.h"
#include "CreatureAIImpl.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "Group.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "TaskScheduler.h"
#include "TemporarySummon.h"

/*#####
# Quest: A Pawn on the Eternal Board
#####*/

enum EternalBoard
{
    QUEST_A_PAWN_ON_THE_ETERNAL_BOARD = 8519,

    EVENT_AREA_RADIUS = 65, // yards

    NPC_ANACHRONOS = 15381,
    NPC_FANDRAL_STAGHELM = 15382,
    NPC_ARYGOS = 15380,
    NPC_MERITHRA_OF_THE_DREAM = 15378,
    NPC_CAELESTRASZ = 15379,

    ANACHRONOS_SAY_1 = 0,
    ANACHRONOS_SAY_2,
    ANACHRONOS_SAY_3,
    ANACHRONOS_SAY_4,
    ANACHRONOS_SAY_5,
    ANACHRONOS_SAY_6,
    ANACHRONOS_SAY_7,
    ANACHRONOS_SAY_8,
    ANACHRONOS_SAY_9,
    ANACHRONOS_SAY_10,
    ANACHRONOS_EMOTE_1,
    ANACHRONOS_EMOTE_2,
    ANACHRONOS_EMOTE_3,

    FANDRAL_SAY_1 = 0,
    FANDRAL_SAY_2,
    FANDRAL_SAY_3,
    FANDRAL_SAY_4,
    FANDRAL_SAY_5,
    FANDRAL_SAY_6,
    FANDRAL_EMOTE_1,
    FANDRAL_EMOTE_2,

    CAELESTRASZ_SAY_1 = 0,
    CAELESTRASZ_SAY_2,
    CAELESTRASZ_YELL_1,

    ARYGOS_SAY_1 = 0,
    ARYGOS_YELL_1,
    ARYGOS_EMOTE_1,

    MERITHRA_SAY_1 = 0,
    MERITHRA_SAY_2,
    MERITHRA_YELL_1,
    MERITHRA_EMOTE_1,

    GO_GATE_OF_AHN_QIRAJ = 176146,
    GO_GLYPH_OF_AHN_QIRAJ = 176148,
    GO_ROOTS_OF_AHN_QIRAJ = 176147
};

enum PawnBattleCreatures
{
    NPC_QIRAJI_WASP = 15414,
    NPC_QIRAJI_TANK = 15422,
    NPC_KALDOREI_INFANTRY = 15423,
    NPC_ANUBISATH_CONQUEROR = 15424
};

// Delay (ms) before the next cinematic step, indexed by step; see npc_anachronos_the_ancient::RunStep.
static uint32 const StepDelay[] =
{
    2000, 4000, 500, 500, 2000, 8000, 6000, 2000, 2500, 3000,
    4000, 4000, 1250, 250, 3000, 2000, 3000, 3000, 4000, 4000,
    1000, 1000, 5000, 3000, 3000, 3000, 4000, 2500, 2000, 250,
    25, 3000, 10000, 2000, 8000, 4000, 2000, 5000, 7000, 4000,
    2000, 4000, 3000, 3000, 3000, 3000, 8000, 1000, 3000, 3000,
    3000, 3000, 3000, 3000, 2000, 2000, 4000, 3000, 2000, 3000,
    15000, 5000, 3500, 5000, 3500, 5000
};

// Spawn coordinates
Position const SpawnLocation[] =
{
    {-8085.0f, 1528.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8080.0f, 1526.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8085.0f, 1524.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8080.0f, 1522.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8085.0f, 1520.0f, 2.61f, 3.141592f}, // Kaldorei Infantry

    {-8085.0f, 1524.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8080.0f, 1522.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8085.0f, 1520.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8080.0f, 1518.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8085.0f, 1516.0f, 2.61f, 3.141592f}, // Kaldorei Infantry

    {-8085.0f, 1518.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8080.0f, 1516.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8080.0f, 1520.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8080.0f, 1424.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8085.0f, 1422.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    // 2 waves of warriors
    {-8082.0f, 1528.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1525.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1524.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1526.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1527.0f, 2.61f, 3.141592f}, // Kaldorei Infantry

    {-8082.0f, 1524.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1522.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1520.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1518.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1516.0f, 2.61f, 3.141592f}, // Kaldorei Infantry

    {-8082.0f, 1523.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1521.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1528.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1519.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1526.0f, 2.61f, 3.141592f}, // Kaldorei Infantry

    {-8082.0f, 1524.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1522.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1520.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8078.0f, 1518.0f, 2.61f, 3.141592f}, // Kaldorei Infantry
    {-8082.0f, 1516.0f, 2.61f, 3.141592f}, // Kaldorei Infantry

    {-8088.0f, 1510.0f, 2.61f, 0.0f}, // Anubisath Conqueror
    {-8084.0f, 1520.0f, 2.61f, 0.0f}, // Anubisath Conqueror
    {-8088.0f, 1530.0f, 2.61f, 0.0f}, // Anubisath Conqueror

    {-8080.0f, 1513.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8082.0f, 1523.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8085.0f, 1518.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8082.0f, 1516.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8085.0f, 1520.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8080.0f, 1528.0f, 2.61f, 0.0f}, // Qiraj Wasp

    {-8082.0f, 1513.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8079.0f, 1523.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8080.0f, 1531.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8079.0f, 1516.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8082.0f, 1520.0f, 2.61f, 0.0f}, // Qiraj Wasp
    {-8080.0f, 1518.0f, 2.61f, 0.0f}, // Qiraj Wasp

    {-8081.0f, 1514.0f, 2.61f, 0.0f}, // Qiraj Tank
    {-8081.0f, 1520.0f, 2.61f, 0.0f}, // Qiraj Tank
    {-8081.0f, 1526.0f, 2.61f, 0.0f}, // Qiraj Tank
    {-8081.0f, 1512.0f, 2.61f, 0.0f}, // Qiraj Tank
    {-8082.0f, 1520.0f, 2.61f, 0.0f}, // Qiraj Tank
    {-8081.0f, 1528.0f, 2.61f, 0.0f}, // Qiraj Tank

    {-8082.0f, 1513.0f, 2.61f, 3.141592f}, // Anubisath Conqueror
    {-8082.0f, 1520.0f, 2.61f, 3.141592f}, // Anubisath Conqueror
    {-8082.0f, 1527.0f, 2.61f, 3.141592f}, // Anubisath Conqueror
};

enum KaldoreiInfantryModels
{
    MODEL_KALDOREI_INFANTRY_1 = 15427,
    MODEL_KALDOREI_INFANTRY_2,
    MODEL_KALDOREI_INFANTRY_3,
    MODEL_KALDOREI_INFANTRY_4
};

struct WaveData
{
    uint8 SpawnCount;
    uint8 SpawnPointIndex;
    uint32 CreatureId;
    Milliseconds NextWaveDelay;
};

static WaveData const WavesInfo[] =
{
    { 30, 0, NPC_KALDOREI_INFANTRY, 0ms },
    { 3, 35, NPC_ANUBISATH_CONQUEROR, 0ms },
    { 12, 38, NPC_QIRAJI_WASP, 0ms },
    { 6, 50, NPC_QIRAJI_TANK, 0ms },
    { 15, 15, NPC_KALDOREI_INFANTRY, 0ms }
};

/*#####
# npc_anachronos_the_ancient
######*/

enum AnachronosSpells
{
    SPELL_ARCANE_CHANNELING = 23017,
    SPELL_NOXIOUS_BREATH = 24818,
    SPELL_GREEN_DRAGON_TRANSFORM_DND = 25105,
    SPELL_RED_DRAGON_TRANSFORM_DND = 25106,
    SPELL_BLUE_DRAGON_TRANSFORM_DND = 25107,
    SPELL_TIME_STOP = 25158,
    SPELL_CALL_PRISMATIC_BARRIER = 25159,
    SPELL_CALL_GLYPHS_OF_WARDING = 25166,
    SPELL_CALL_ANCIENTS = 25167,
    SPELL_THROW_HAMMER = 33806,
    SPELL_FROST_BREATH = 50505,
    SPELL_FLAME_BREATH = 54293
};

enum AnachronosModels
{
    MODEL_ANACHRONOS_DRAGON = 15500
};

enum AnachronosCinematicData
{
    DATA_PLAYER = 0,
    DATA_MERITHRA,
    DATA_CAELESTRASZ,
    DATA_ARYGOS,
    DATA_FANDRAL
};

enum AnachronosCinematicAction
{
    ACTION_START_CINEMATIC = 1
};

struct npc_anachronos_the_ancient : public ScriptedAI
{
    npc_anachronos_the_ancient(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _scheduler.CancelAll();
        _step = 0;
        _playerGUID.Clear();
        _merithraGUID.Clear();
        _caelestraszGUID.Clear();
        _arygosGUID.Clear();
        _fandralGUID.Clear();

        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
    }

    // Each actor GUID is pushed in by go_crystalline_tear before ACTION_START_CINEMATIC.
    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        switch (id)
        {
            case DATA_PLAYER:
                _playerGUID = guid;
                break;
            case DATA_MERITHRA:
                _merithraGUID = guid;
                break;
            case DATA_CAELESTRASZ:
                _caelestraszGUID = guid;
                break;
            case DATA_ARYGOS:
                _arygosGUID = guid;
                break;
            case DATA_FANDRAL:
                _fandralGUID = guid;
                break;
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_START_CINEMATIC)
        {
            _step = 0;
            _scheduler.Schedule(1500ms, [this](TaskContext /*context*/)
            {
                RunStep();
            });
        }
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    void RunStep()
    {
        if (_step > 65)
        {
            EndEvent();
            return;
        }

        Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
        Creature* fandral = ObjectAccessor::GetCreature(*me, _fandralGUID);
        Creature* merithra = ObjectAccessor::GetCreature(*me, _merithraGUID);
        Creature* arygos = ObjectAccessor::GetCreature(*me, _arygosGUID);
        Creature* caelestrasz = ObjectAccessor::GetCreature(*me, _caelestraszGUID);

        // The cinematic cannot continue without the questing player or any of its actors.
        if (!player || !fandral || !merithra || !arygos || !caelestrasz)
        {
            EndEvent();
            return;
        }

        switch (_step)
        {
            case 0:
                Talk(ANACHRONOS_SAY_1, fandral);
                break;
            case 1:
                fandral->SetTarget(me->GetGUID());
                fandral->AI()->Talk(FANDRAL_SAY_1, me);
                break;
            case 2:
                fandral->SetTarget(ObjectGuid::Empty);
                merithra->AI()->Talk(MERITHRA_EMOTE_1);
                break;
            case 3:
                merithra->AI()->Talk(MERITHRA_SAY_1);
                break;
            case 4:
                arygos->AI()->Talk(ARYGOS_EMOTE_1);
                break;
            case 5:
                caelestrasz->SetTarget(fandral->GetGUID());
                caelestrasz->AI()->Talk(CAELESTRASZ_SAY_1);
                break;
            case 6:
                merithra->AI()->Talk(MERITHRA_SAY_2);
                break;
            case 7:
                caelestrasz->SetTarget(ObjectGuid::Empty);
                merithra->GetMotionMaster()->MoveCharge(-8065.0f, 1530.0f, 2.61f, 10.0f);
                break;
            case 8:
                merithra->AI()->Talk(MERITHRA_YELL_1);
                break;
            case 9:
                merithra->CastSpell(merithra, SPELL_GREEN_DRAGON_TRANSFORM_DND, true);
                break;
            case 10:
                merithra->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                merithra->SetDisableGravity(true);
                merithra->GetMotionMaster()->MoveCharge(-8065.0f, 1530.0f, 6.61f, 3.0f);
                break;
            case 11:
                merithra->CastSpell(merithra, SPELL_NOXIOUS_BREATH, false);
                break;
            case 12:
                merithra->GetMotionMaster()->MoveCharge(-8100.0f, 1530.0f, 50.0f, 42.0f);
                break;
            case 13:
                break;
            case 14:
                arygos->AI()->Talk(ARYGOS_SAY_1);
                merithra->SetVisible(false);
                break;
            case 15:
                arygos->GetMotionMaster()->MoveCharge(-8065.0f, 1530.0f, 2.61f, 10.0f);
                merithra->GetMotionMaster()->MoveCharge(-8034.535f, 1535.14f, 2.61f, 42.0f);
                break;
            case 16:
                arygos->AI()->Talk(ARYGOS_YELL_1);
                break;
            case 17:
                arygos->CastSpell(arygos, SPELL_BLUE_DRAGON_TRANSFORM_DND, true);
                break;
            case 18:
                arygos->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                arygos->SetDisableGravity(true);
                arygos->GetMotionMaster()->MoveCharge(-8065.0f, 1530.0f, 6.61f, 42.0f);
                break;
            case 19:
                arygos->CastSpell(arygos, SPELL_FROST_BREATH, false);
                break;
            case 20:
                arygos->GetMotionMaster()->MoveCharge(-8095.0f, 1530.0f, 50.0f, 42.0f);
                break;
            case 21:
                break;
            case 22:
                caelestrasz->AI()->Talk(CAELESTRASZ_SAY_2, fandral);
                break;
            case 23:
                caelestrasz->GetMotionMaster()->MoveCharge(-8065.0f, 1530.0f, 2.61f, 10.0f);
                arygos->SetVisible(false);
                arygos->GetMotionMaster()->MoveCharge(-8034.535f, 1535.14f, 2.61f, 10.0f);
                break;
            case 24:
                caelestrasz->AI()->Talk(CAELESTRASZ_YELL_1);
                break;
            case 25:
                caelestrasz->CastSpell(caelestrasz, SPELL_RED_DRAGON_TRANSFORM_DND, true);
                break;
            case 26:
                caelestrasz->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                caelestrasz->SetDisableGravity(true);
                caelestrasz->GetMotionMaster()->MoveCharge(-8065.0f, 1530.0f, 7.61f, 4.0f);
                break;
            case 27:
                caelestrasz->CastSpell(caelestrasz, SPELL_FLAME_BREATH, false);
                break;
            case 28:
                Talk(ANACHRONOS_SAY_2, fandral);
                break;
            case 29:
                caelestrasz->GetMotionMaster()->MoveCharge(-8095.0f, 1530.0f, 50.0f, 42.0f);
                fandral->AI()->Talk(FANDRAL_SAY_2);
                break;
            case 30:
                break;
            case 31:
                Talk(ANACHRONOS_SAY_3, fandral);
                break;
            case 32:
                caelestrasz->SetVisible(false);
                caelestrasz->GetMotionMaster()->MoveCharge(-8034.535f, 1535.14f, 2.61f, 42.0f);
                fandral->GetMotionMaster()->MoveCharge(-8108.0f, 1529.0f, 2.77f, 8.0f);
                me->GetMotionMaster()->MoveCharge(-8113.0f, 1525.0f, 2.77f, 8.0f);
                break; // both run to the gate
            case 33:
                Talk(ANACHRONOS_SAY_4);
                caelestrasz->GetMotionMaster()->MoveCharge(-8050.0f, 1473.0f, 65.0f, 15.0f);
                break;
            case 34:
                DoCast(player, SPELL_ARCANE_CHANNELING, true);
                break;
            case 35:
                me->CastSpell(Position(-8088.0f, 1520.43f, 2.67f), SPELL_TIME_STOP, true);
                break;
            case 36:
                DoCast(player, SPELL_CALL_PRISMATIC_BARRIER, true);
                break;
            case 37:
                me->SummonGameObject(GO_GATE_OF_AHN_QIRAJ, Position(-8130.0f, 1525.0f, 17.5f, 0.0f), QuaternionData(), 0s);
                break;
            case 38:
                DoCast(player, SPELL_CALL_GLYPHS_OF_WARDING, true);
                me->SummonGameObject(GO_GLYPH_OF_AHN_QIRAJ, Position(-8130.0f, 1525.0f, 17.5f, 0.0f), QuaternionData(), 0s);
                break;
            case 39:
                Talk(ANACHRONOS_SAY_5, fandral);
                break;
            case 40:
                fandral->CastSpell(me, SPELL_CALL_ANCIENTS, true);
                break;
            case 41:
                fandral->SummonGameObject(GO_ROOTS_OF_AHN_QIRAJ, Position(-8130.0f, 1525.0f, 17.5f, 0.0f), QuaternionData(), 0s);
                fandral->AI()->Talk(FANDRAL_SAY_3);
                break;
            case 42:
                me->CastStop();
                fandral->AI()->Talk(FANDRAL_EMOTE_1);
                break;
            case 43:
                fandral->CastStop();
                break;
            case 44:
                Talk(ANACHRONOS_SAY_6);
                break;
            case 45:
                Talk(ANACHRONOS_SAY_7);
                break;
            case 46:
                Talk(ANACHRONOS_SAY_8);
                me->GetMotionMaster()->MoveCharge(-8110.0f, 1527.0f, 2.77f, 4.0f);
                break;
            case 47:
                Talk(ANACHRONOS_EMOTE_1);
                break;
            case 48:
                fandral->AI()->Talk(FANDRAL_SAY_4, me);
                break;
            case 49:
                fandral->AI()->Talk(FANDRAL_SAY_5, me);
                break;
            case 50:
                fandral->AI()->Talk(FANDRAL_EMOTE_2);
                fandral->CastSpell(Position(-8127.0f, 1525.0f, 17.5f), SPELL_THROW_HAMMER, true);
                break;
            case 51:
            {
                std::list<Creature*> mobs;
                me->GetCreatureListWithOptionsInGrid(mobs, 100.0f, FindCreatureOptions{ .CreatureIds = { NPC_KALDOREI_INFANTRY, NPC_ANUBISATH_CONQUEROR, NPC_QIRAJI_WASP, NPC_QIRAJI_TANK } });
                for (Creature* mob : mobs)
                    mob->DespawnOrUnsummon();
                break;
            }
            case 52:
                fandral->GetMotionMaster()->MoveCharge(-8028.75f, 1538.795f, 2.61f, 4.0f);
                Talk(ANACHRONOS_SAY_9);
                break;
            case 53:
                fandral->AI()->Talk(FANDRAL_SAY_6);
                break;
            case 54:
                Talk(ANACHRONOS_EMOTE_2);
                break;
            case 55:
                fandral->SetVisible(false);
                break;
            case 56:
                Talk(ANACHRONOS_EMOTE_3);
                me->GetMotionMaster()->MoveCharge(-8116.0f, 1522.0f, 3.65f, 4.0f);
                break;
            case 57:
                me->GetMotionMaster()->MoveCharge(-8116.7f, 1527.0f, 3.7f, 4.0f);
                break;
            case 58:
                me->GetMotionMaster()->MoveCharge(-8112.67f, 1529.9f, 2.86f, 4.0f);
                break;
            case 59:
                me->GetMotionMaster()->MoveCharge(-8117.99f, 1532.24f, 3.94f, 4.0f);
                break;
            case 60:
                Talk(ANACHRONOS_SAY_10, player);
                me->GetMotionMaster()->MoveCharge(-8113.46f, 1524.16f, 2.89f, 4.0f);
                break;
            case 61:
                me->GetMotionMaster()->MoveCharge(-8057.1f, 1470.32f, 2.61f, 6.0f);
                if (player->IsInRange(me, 0.0f, 40.0f))
                    player->GroupEventHappens(QUEST_A_PAWN_ON_THE_ETERNAL_BOARD, me);
                break;
            case 62:
                me->SetDisplayId(MODEL_ANACHRONOS_DRAGON);
                break;
            case 63:
                me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                me->SetDisableGravity(true);
                break;
            case 64:
                me->GetMotionMaster()->MoveCharge(-8000.0f, 1400.0f, 150.0f, 9.0f);
                break;
            case 65:
                me->SetVisible(false);
                break;
        }

        uint32 const delay = StepDelay[_step];
        ++_step;
        if (delay)
            _scheduler.Schedule(Milliseconds(delay), [this](TaskContext /*context*/)
            {
                RunStep();
            });
        else
            EndEvent();
    }

    void EndEvent()
    {
        _scheduler.CancelAll();
        me->DespawnOrUnsummon();
    }

    TaskScheduler _scheduler;
    uint8 _step = 0;

    ObjectGuid _playerGUID;
    ObjectGuid _merithraGUID;
    ObjectGuid _caelestraszGUID;
    ObjectGuid _arygosGUID;
    ObjectGuid _fandralGUID;
};

/*######
# npc_qiraj_war_spawn
######*/

enum QirajWarSpawn
{
    SPELL_STONED_CHANNEL_CAST_VISUAL = 15533,
    SPELL_SUMMON_POISON_CLOUD = 24319,
    SPELL_POISON_CLOUD = 28528,
    SPELL_STONED = 33652,
    SPELL_FROST_DEBUFF = 35871, // placeholder; retail spell unknown
    SPELL_FIRE_EXPLOSION = 42075 // placeholder; retail spell unknown
};

enum QirajWarSpawnData
{
    DATA_MOB_GUID = 0
};

enum AnachronosTriggerAction
{
    ACTION_START_WAVES = 1
};

enum AnachronosTriggerData
{
    DATA_EVENT_IN_PROGRESS = 0,
    DATA_BATTLE_MOB_DIED
};

struct npc_qiraj_war_spawn : public ScriptedAI
{
    npc_qiraj_war_spawn(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _scheduler.CancelAll();
        _mobGUID.Clear();
        _hasTarget = false;

        me->SetImmuneToPC(true);
    }

    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        if (id == DATA_MOB_GUID)
            _mobGUID = guid;
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        if (IsQiraji())
        {
            _scheduler.Schedule(38500ms, [this](TaskContext context)
            {
                DoCastSelf(SPELL_POISON_CLOUD);
                DoCastSelf(SPELL_SUMMON_POISON_CLOUD);
                context.Repeat(5min);
            });

            _scheduler.Schedule(58s, [this](TaskContext context)
            {
                DoCastSelf(SPELL_FROST_DEBUFF);
                context.Repeat(5min);
            });

            _scheduler.Schedule(80950ms, [this](TaskContext context)
            {
                DoCastSelf(SPELL_FIRE_EXPLOSION);
                context.Repeat(5min);
            });
        }

        _scheduler.Schedule(100s, [this](TaskContext context)
        {
            me->RemoveAllAttackers();
            me->AttackStop();
            DoCastSelf(SPELL_STONED_CHANNEL_CAST_VISUAL);
            context.Repeat(2s);
        });
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Creature* trigger = ObjectAccessor::GetCreature(*me, _mobGUID))
            if (CreatureAI* triggerAI = trigger->AI())
                triggerAI->SetData(DATA_BATTLE_MOB_DIED, me->GetEntry());

        me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        AcquireTargetIfNeeded();

        // The Qiraji adds petrify once the red drake (Caelestrasz) is no longer overhead.
        if (!me->FindNearestCreature(NPC_CAELESTRASZ, CaelestraszPresenceRange))
            DoCastSelf(SPELL_STONED);

        if (!UpdateVictim())
        {
            _hasTarget = false;
            return;
        }

        DoMeleeAttackIfReady();
    }

private:
    bool IsQiraji() const
    {
        return me->GetEntry() == NPC_ANUBISATH_CONQUEROR || me->GetEntry() == NPC_QIRAJI_TANK || me->GetEntry() == NPC_QIRAJI_WASP;
    }

    void AcquireTargetIfNeeded()
    {
        if (_hasTarget)
            return;

        Unit* target = nullptr;
        if (IsQiraji())
            target = me->FindNearestCreature(NPC_KALDOREI_INFANTRY, TargetSearchRange, true);
        else
            target = me->FindNearestCreature(RAND(NPC_QIRAJI_TANK, NPC_ANUBISATH_CONQUEROR, NPC_QIRAJI_WASP), TargetSearchRange, true);

        _hasTarget = true;
        if (target)
            AttackStart(target);
    }

    static constexpr float TargetSearchRange = 20.0f;
    static constexpr float CaelestraszPresenceRange = 60.0f;

    TaskScheduler _scheduler;
    ObjectGuid _mobGUID;
    bool _hasTarget = false;
};

/*#####
# npc_anachronos_quest_trigger
#####*/

struct npc_anachronos_quest_trigger : public ScriptedAI
{
    npc_anachronos_quest_trigger(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _scheduler.CancelAll();
        DespawnBattle();
        _playerGUID.Clear();
        _waveIndex = 0;
        _eventStarted = false;
        _failed = false;

        me->SetVisible(false);
    }

    void SetGUID(ObjectGuid const& guid, int32 /*id*/) override
    {
        _playerGUID = guid;
    }

    uint32 GetData(uint32 id) const override
    {
        return (id == DATA_EVENT_IN_PROGRESS && _eventStarted) ? 1 : 0;
    }

    void SetData(uint32 id, uint32 value) override
    {
        if (id == DATA_BATTLE_MOB_DIED && _eventStarted)
            SummonBattleMob(value, RandomSpawnPointFor(value));
    }

    void DoAction(int32 action) override
    {
        if (action != ACTION_START_WAVES || _eventStarted)
            return;

        _eventStarted = true;
        _failed = false;
        _waveIndex = 0;
        _scheduler.Schedule(2s, [this](TaskContext /*context*/)
        {
            SummonWave();
        }).Schedule(245s, [this](TaskContext /*context*/)
        {
            EnterEvadeMode();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!_eventStarted)
            return;

        _scheduler.Update(diff);

        CheckEventFailed();
    }

private:
    void SummonWave()
    {
        WaveData const& wave = WavesInfo[_waveIndex];

        for (uint8 i = 0; i < wave.SpawnCount; ++i)
            SummonBattleMob(wave.CreatureId, SpawnLocation[wave.SpawnPointIndex + i]);

        if (++_waveIndex < std::size(WavesInfo))
            _scheduler.Schedule(wave.NextWaveDelay, [this](TaskContext /*context*/)
            {
                SummonWave();
            });
    }

    void SummonBattleMob(uint32 entry, Position const& pos)
    {
        Creature* spawn = me->SummonCreature(entry, pos, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 5min);
        if (!spawn)
            return;

        if (entry == NPC_KALDOREI_INFANTRY)
            spawn->SetDisplayId(MODEL_KALDOREI_INFANTRY_1 + urand(0, 3));

        if (CreatureAI* spawnAI = spawn->AI())
            spawnAI->SetGUID(me->GetGUID(), DATA_MOB_GUID);
    }

    Position const& RandomSpawnPointFor(uint32 entry) const
    {
        for (WaveData const& wave : WavesInfo)
            if (wave.CreatureId == entry)
                return SpawnLocation[wave.SpawnPointIndex + urand(0, wave.SpawnCount - 1)];

        return SpawnLocation[0];
    }

    void DespawnBattle()
    {
        std::list<Creature*> mobs;
        me->GetCreatureListWithOptionsInGrid(mobs, 100.0f, FindCreatureOptions{ .CreatureIds = { NPC_KALDOREI_INFANTRY, NPC_ANUBISATH_CONQUEROR, NPC_QIRAJI_WASP, NPC_QIRAJI_TANK } });
        for (Creature* mob : mobs)
            mob->DespawnOrUnsummon();
    }

    bool CheckEventFailed()
    {
        Player* player = ObjectAccessor::GetPlayer(*me, _playerGUID);
        if (!player)
            return _failed;

        Group* group = player->GetGroup();
        if (!group)
            return _failed;

        uint8 memberCount = 0;
        uint8 failedCount = 0;

        for (Group::MemberSlot const& slot : group->GetMemberSlots())
        {
            Player* member = ObjectAccessor::GetPlayer(*me, slot.guid);
            if (!member)
                continue;

            if (!member->IsWithinDistInMap(me, EVENT_AREA_RADIUS) && member->GetQuestStatus(QUEST_A_PAWN_ON_THE_ETERNAL_BOARD) == QUEST_STATUS_INCOMPLETE)
            {
                member->FailQuest(QUEST_A_PAWN_ON_THE_ETERNAL_BOARD);
                ++failedCount;
            }
            ++memberCount;
        }

        if (memberCount == failedCount || !player->IsWithinDistInMap(me, EVENT_AREA_RADIUS))
            _failed = true; // only so the event can restart

        return _failed;
    }

    TaskScheduler _scheduler;
    ObjectGuid _playerGUID;
    uint8 _waveIndex = 0;
    bool _eventStarted = false;
    bool _failed = false;
};

/*#####
# go_crystalline_tear
######*/

enum CrystallineTear
{
    ARYGOS_GNOME_FORM = 15418,
    CAELESTRASZ_NIGHT_ELF_FORM = 15419,
    MERITHRA_NIGHT_ELF_FORM = 15420,
    ANACHRONOS_QUEST_TRIGGER_INVISIBLE = 15454
};

struct go_crystalline_tear : public GameObjectAI
{
    go_crystalline_tear(GameObject* go) : GameObjectAI(go) { }

    void OnQuestAccept(Player* player, Quest const* quest) override
    {
        if (quest->GetQuestId() != QUEST_A_PAWN_ON_THE_ETERNAL_BOARD)
            return;

        Creature* trigger = me->FindNearestCreature(ANACHRONOS_QUEST_TRIGGER_INVISIBLE, 100.0f);
        if (!trigger)
            return;

        if (CreatureAI* triggerAI = trigger->AI())
            if (triggerAI->GetData(DATA_EVENT_IN_PROGRESS))
                return;

        Creature* merithra = trigger->SummonCreature(NPC_MERITHRA_OF_THE_DREAM, -8034.535f, 1535.14f, 2.61f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240s);
        Creature* caelestrasz = trigger->SummonCreature(NPC_CAELESTRASZ, -8032.767f, 1533.148f, 2.61f, 1.5f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240s);
        Creature* arygos = trigger->SummonCreature(NPC_ARYGOS, -8034.52f, 1537.843f, 2.61f, 5.7f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240s);
        Creature* fandral = trigger->SummonCreature(NPC_FANDRAL_STAGHELM, -8028.462f, 1535.843f, 2.61f, 3.141592f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240s);
        Creature* anachronos = trigger->SummonCreature(NPC_ANACHRONOS, -8028.75f, 1538.795f, 2.61f, 4.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240s);

        if (!merithra || !caelestrasz || !arygos || !fandral || !anachronos)
            return;

        for (Creature* dragonkin : { merithra, caelestrasz, arygos })
        {
            dragonkin->ReplaceAllNpcFlags(UNIT_NPC_FLAG_NONE);
            dragonkin->SetStandState(UNIT_STAND_STATE_STAND);
            dragonkin->SetFaction(FACTION_FRIENDLY);
        }

        merithra->SetDisplayId(MERITHRA_NIGHT_ELF_FORM);
        caelestrasz->SetDisplayId(CAELESTRASZ_NIGHT_ELF_FORM);
        arygos->SetDisplayId(ARYGOS_GNOME_FORM);

        if (CreatureAI* triggerAI = trigger->AI())
        {
            triggerAI->SetGUID(player->GetGUID());
            triggerAI->DoAction(ACTION_START_WAVES);
        }

        if (CreatureAI* anachronosAI = anachronos->AI())
        {
            anachronosAI->SetGUID(player->GetGUID(), DATA_PLAYER);
            anachronosAI->SetGUID(merithra->GetGUID(), DATA_MERITHRA);
            anachronosAI->SetGUID(caelestrasz->GetGUID(), DATA_CAELESTRASZ);
            anachronosAI->SetGUID(arygos->GetGUID(), DATA_ARYGOS);
            anachronosAI->SetGUID(fandral->GetGUID(), DATA_FANDRAL);
            anachronosAI->DoAction(ACTION_START_CINEMATIC);
        }
    }
};

/*###
## go_wind_stone
###*/

enum WSSpells
{
    AURA_TWILIGHT_SET      = 24746,
    AURA_MEDALLION         = 24748,
    AURA_RING              = 24782,

    SPELL_TEMPLAR_RANDOM   = 24745,
    SPELL_TEMPLAR_FIRE     = 24747,
    SPELL_TEMPLAR_AIR      = 24757,
    SPELL_TEMPLAR_EARTH    = 24759,
    SPELL_TEMPLAR_WATER    = 24761,

    SPELL_DUKE_RANDOM      = 24762,
    SPELL_DUKE_FIRE        = 24766,
    SPELL_DUKE_AIR         = 24769,
    SPELL_DUKE_EARTH       = 24771,
    SPELL_DUKE_WATER       = 24773,

    SPELL_ROYAL_RANDOM     = 24785,
    SPELL_ROYAL_FIRE       = 24787,
    SPELL_ROYAL_AIR        = 24791,
    SPELL_ROYAL_EARTH      = 24792,
    SPELL_ROYAL_WATER      = 24793,

    SPELL_PUNISHMENT       = 24803
};

enum WSGossip
{
    OPTION_ID_WS_RANDOM    = 0,
    OPTION_ID_1_CRIMSON    = 1,
    OPTION_ID_2_AZURE      = 2,
    OPTION_ID_3_EARTHEN    = 3,
    OPTION_ID_4_HOARY      = 4,
    OPTION_ID_1_CYNDERS    = 1,
    OPTION_ID_2_FATHOMS    = 2,
    OPTION_ID_3_SHARDS     = 3,
    OPTION_ID_4_ZEPHYRS    = 4,
    OPTION_ID_1_SKALDRENOX = 1,
    OPTION_ID_2_SKWOL      = 2,
    OPTION_ID_3_KAZUM      = 3,
    OPTION_ID_4_WHIRLAXIS  = 4,
    GOSSIP_ID_LESSER_WS    = 6540,
    GOSSIP_ID_WIND_STONE   = 6542,
    GOSSIP_ID_GREATER_WS   = 6543
};

enum WSCreatures
{
    NPC_TEMPLAR_FIRE       = 15209,
    NPC_TEMPLAR_WATER      = 15211,
    NPC_TEMPLAR_AIR        = 15212,
    NPC_TEMPLAR_EARTH      = 15307,

    NPC_DUKE_FIRE          = 15206,
    NPC_DUKE_WATER         = 15207,
    NPC_DUKE_EARTH         = 15208,
    NPC_DUKE_AIR           = 15220,

    NPC_ROYAL_FIRE         = 15203,
    NPC_ROYAL_AIR          = 15204,
    NPC_ROYAL_EARTH        = 15205,
    NPC_ROYAL_WATER        = 15305
};

enum WSItems
{
    ITEM_TEMPLAR_FIRE      = 20416,
    ITEM_TEMPLAR_AIR       = 20418,
    ITEM_TEMPLAR_EARTH     = 20419,
    ITEM_TEMPLAR_WATER     = 20420,

    ITEM_DUKE_FIRE         = 20432,
    ITEM_DUKE_AIR          = 20433,
    ITEM_DUKE_EARTH        = 20435,
    ITEM_DUKE_WATER        = 20436,

    ITEM_ROYAL_FIRE        = 20447,
    ITEM_ROYAL_AIR         = 20448,
    ITEM_ROYAL_EARTH       = 20449,
    ITEM_ROYAL_WATER       = 20450
};

enum WS
{
    TEMPLAR                = 0,
    DUKE                   = 1,
    ROYAL                  = 2,

    FIRE                   = 0x1,
    WATER                  = 0x2,
    EARTH                  = 0x4,
    AIR                    = 0x8
};

class go_wind_stone : public GameObjectScript
{
    public:
        go_wind_stone() : GameObjectScript("go_wind_stone") { }

        struct go_wind_stoneAI : public GameObjectAI
        {
            go_wind_stoneAI(GameObject* go) : GameObjectAI(go) { }

            private:
                bool isSummoning = false;

                uint8 GetPlayerTwilightSetRank(Player* player) // For random summoning
                {
                    bool setAura = player->HasAura(AURA_TWILIGHT_SET);
                    bool medallionAura = player->HasAura(AURA_MEDALLION);
                    bool ringAura = player->HasAura(AURA_RING);

                    if (setAura && medallionAura && ringAura)
                        return 3;
                    else if (setAura && medallionAura)
                        return 2;
                    else if (setAura)
                        return 1;
                    else
                        return 0;
                }

                uint8 GetSummonItems(Player* player, WS type)
                {
                    uint8 result = 0x0;

                    switch (type)
                    {
                        case TEMPLAR:
                        {
                            if (player->HasItemCount(ITEM_TEMPLAR_FIRE))
                                result |= FIRE;
                            if (player->HasItemCount(ITEM_TEMPLAR_WATER))
                                result |= WATER;
                            if (player->HasItemCount(ITEM_TEMPLAR_EARTH))
                                result |= EARTH;
                            if (player->HasItemCount(ITEM_TEMPLAR_AIR))
                                result |= AIR;
                            break;
                        }
                        case DUKE:
                        {
                            if (player->HasItemCount(ITEM_DUKE_FIRE))
                                result |= FIRE;
                            if (player->HasItemCount(ITEM_DUKE_WATER))
                                result |= WATER;
                            if (player->HasItemCount(ITEM_DUKE_EARTH))
                                result |= EARTH;
                            if (player->HasItemCount(ITEM_DUKE_AIR))
                                result |= AIR;
                            break;
                        }
                        case ROYAL:
                        {
                            if (player->HasItemCount(ITEM_ROYAL_FIRE))
                                result |= FIRE;
                            if (player->HasItemCount(ITEM_ROYAL_WATER))
                                result |= WATER;
                            if (player->HasItemCount(ITEM_ROYAL_EARTH))
                                result |= EARTH;
                            if (player->HasItemCount(ITEM_ROYAL_AIR))
                                result |= AIR;
                            break;
                        }
                        default:
                            break;
                    }
                    return result;
                }

                void SummonNPC(GameObject* go, Player* player, uint32 npc, uint32 spell)
                {
                    isSummoning = true;

                    player->SummonCreature(npc, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), player->GetOrientation() - float(M_PI), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5min);
                    me->CastSpell(player, spell);
                    me->DespawnOrUnsummon(1100ms);
                }

            public:
                bool OnGossipHello(Player* player) override
                {
                    // fix possible exploit
                    if (isSummoning)
                    {
                        return true;
                    }
                    uint8 rank = GetPlayerTwilightSetRank(player);

                    uint32 gossipId = me->GetGOInfo()->GetGossipMenuId();

                    switch (gossipId)
                    {
                        case GOSSIP_ID_LESSER_WS:
                        {
                            InitGossipMenuFor(player, GOSSIP_ID_LESSER_WS);
                            if (rank >= 1) // 1 or 2 or 3
                                AddGossipItemFor(player, GOSSIP_ID_LESSER_WS, OPTION_ID_WS_RANDOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                            else
                            {
                                player->CastSpell(player, SPELL_PUNISHMENT, true);
                                break;
                            }

                            uint8 item = GetSummonItems(player, TEMPLAR);
                            if (item & FIRE)
                                AddGossipItemFor(player, GOSSIP_ID_LESSER_WS, OPTION_ID_1_CRIMSON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                            if (item & WATER)
                                AddGossipItemFor(player, GOSSIP_ID_LESSER_WS, OPTION_ID_2_AZURE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                            if (item & EARTH)
                                AddGossipItemFor(player, GOSSIP_ID_LESSER_WS, OPTION_ID_3_EARTHEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                            if (item & AIR)
                                AddGossipItemFor(player, GOSSIP_ID_LESSER_WS, OPTION_ID_4_HOARY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                            break;
                        }
                        case GOSSIP_ID_WIND_STONE:
                        {
                            InitGossipMenuFor(player, GOSSIP_ID_WIND_STONE);
                            if (rank >= 2) // 2 or 3
                                AddGossipItemFor(player, GOSSIP_ID_WIND_STONE, OPTION_ID_WS_RANDOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                            else
                            {
                                player->CastSpell(player, SPELL_PUNISHMENT, true);
                                break;
                            }

                            uint8 item = GetSummonItems(player, DUKE);
                            if (item & FIRE)
                                AddGossipItemFor(player, GOSSIP_ID_WIND_STONE, OPTION_ID_1_CYNDERS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                            if (item & WATER)
                                AddGossipItemFor(player, GOSSIP_ID_WIND_STONE, OPTION_ID_2_FATHOMS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
                            if (item & EARTH)
                                AddGossipItemFor(player, GOSSIP_ID_WIND_STONE, OPTION_ID_3_SHARDS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
                            if (item & AIR)
                                AddGossipItemFor(player, GOSSIP_ID_WIND_STONE, OPTION_ID_4_ZEPHYRS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
                            break;
                        }
                        case GOSSIP_ID_GREATER_WS:
                        {
                            InitGossipMenuFor(player, GOSSIP_ID_GREATER_WS);
                            if (rank == 3) // 3
                                AddGossipItemFor(player, GOSSIP_ID_GREATER_WS, OPTION_ID_WS_RANDOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                            else
                            {
                                player->CastSpell(player, SPELL_PUNISHMENT, true);
                                break;
                            }

                            uint8 item = GetSummonItems(player, ROYAL);
                            if (item & FIRE)
                                AddGossipItemFor(player, GOSSIP_ID_GREATER_WS, OPTION_ID_1_SKALDRENOX, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                            if (item & WATER)
                                AddGossipItemFor(player, GOSSIP_ID_GREATER_WS, OPTION_ID_2_SKWOL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                            if (item & EARTH)
                                AddGossipItemFor(player, GOSSIP_ID_GREATER_WS, OPTION_ID_3_KAZUM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                            if (item & AIR)
                                AddGossipItemFor(player, GOSSIP_ID_GREATER_WS, OPTION_ID_4_WHIRLAXIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
                            break;
                        }
                        default:
                            break;
                    }

                    SendGossipMenuFor(player, player->GetGossipTextId(gossipId, me), me->GetGUID());
                    return true;
                }

                bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
                {
                    uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
                    ClearGossipMenuFor(player);
                    player->PlayerTalkClass->SendCloseGossip();

                    // fix possible exploit
                    if (isSummoning)
                    {
                        return true;
                    }

                    uint8 rank = GetPlayerTwilightSetRank(player);

                    switch (action)
                    {
                        case GOSSIP_ACTION_INFO_DEF + 1:
                            if (rank >= 1)
                            {
                                SummonNPC(me, player, RAND(NPC_TEMPLAR_WATER, NPC_TEMPLAR_FIRE, NPC_TEMPLAR_EARTH, NPC_TEMPLAR_AIR), SPELL_TEMPLAR_RANDOM);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 2:
                            if (rank >= 1)
                            {
                                SummonNPC(me, player, NPC_TEMPLAR_FIRE, SPELL_TEMPLAR_FIRE);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 3:
                            if (rank >= 1)
                            {
                                SummonNPC(me, player, NPC_TEMPLAR_WATER, SPELL_TEMPLAR_WATER);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 4:
                            if (rank >= 1)
                            {
                                SummonNPC(me, player, NPC_TEMPLAR_EARTH, SPELL_TEMPLAR_EARTH);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 5:
                            if (rank >= 1)
                            {
                                SummonNPC(me, player, NPC_TEMPLAR_AIR, SPELL_TEMPLAR_AIR);
                            }
                            break;

                        case GOSSIP_ACTION_INFO_DEF + 6:
                            if (rank >= 2)
                            {
                                SummonNPC(me, player, RAND(NPC_DUKE_FIRE, NPC_DUKE_WATER, NPC_DUKE_EARTH, NPC_DUKE_AIR), SPELL_DUKE_RANDOM);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 7:
                            if (rank >= 2)
                            {
                                SummonNPC(me, player, NPC_DUKE_FIRE, SPELL_DUKE_FIRE);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 8:
                            if (rank >= 2)
                            {
                                SummonNPC(me, player, NPC_DUKE_WATER, SPELL_DUKE_WATER);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 9:
                            if (rank >= 2)
                            {
                                SummonNPC(me, player, NPC_DUKE_EARTH, SPELL_DUKE_EARTH);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 10:
                            if (rank >= 2)
                            {
                                SummonNPC(me, player, NPC_DUKE_AIR, SPELL_DUKE_AIR);
                            }
                            break;

                        case GOSSIP_ACTION_INFO_DEF + 11:
                            if (rank == 3)
                            {
                                SummonNPC(me, player, RAND(NPC_ROYAL_FIRE, NPC_ROYAL_AIR, NPC_ROYAL_EARTH, NPC_ROYAL_WATER), SPELL_ROYAL_RANDOM);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 12:
                            if (rank == 3)
                            {
                                SummonNPC(me, player, NPC_ROYAL_FIRE, SPELL_ROYAL_FIRE);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 13:
                            if (rank == 3)
                            {
                                SummonNPC(me, player, NPC_ROYAL_WATER, SPELL_ROYAL_WATER);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 14:
                            if (rank == 3)
                            {
                                SummonNPC(me, player, NPC_ROYAL_EARTH, SPELL_ROYAL_EARTH);
                            }
                            break;
                        case GOSSIP_ACTION_INFO_DEF + 15:
                            if (rank == 3)
                            {
                                SummonNPC(me, player, NPC_ROYAL_AIR, SPELL_ROYAL_AIR);
                            }
                            break;

                        default:
                            break;
                    }
                    return true;
                }

                void Reset() override
                {
                    // Allow despawning
                    me->RemoveFlag(GO_FLAG_NODESPAWN);
                    isSummoning = false;
                }
        };

        GameObjectAI* GetAI(GameObject* go) const override
        {
            return new go_wind_stoneAI(go);
        }
};

// 24745 - Summon Templar, Trigger
// 24747 - Summon Templar Fire, Trigger
// 24757 - Summon Templar Air, Trigger
// 24759 - Summon Templar Earth, Trigger
// 24761 - Summon Templar Water, Trigger
// 24762 - Summon Duke, Trigger
// 24766 - Summon Duke Fire, Trigger
// 24769 - Summon Duke Air, Trigger
// 24771 - Summon Duke Earth, Trigger
// 24773 - Summon Duke Water, Trigger
// 24785 - Summon Royal, Trigger
// 24787 - Summon Royal Fire, Trigger
// 24791 - Summon Royal Air, Trigger
// 24792 - Summon Royal Earth, Trigger
// 24793 - Summon Royal Water, Trigger
// 46595 - Summon Ice Stone Lieutenant, Trigger
class spell_silithus_summon_cultist_periodic : public AuraScript
{
    PrepareAuraScript(spell_silithus_summon_cultist_periodic);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ spellInfo->GetEffect(EFFECT_0).TriggerSpell });
    }

    void PeriodicTick(AuraEffect const* aurEff)
    {
        PreventDefaultAction();

        // The triggered spell needs reagents, so don't ignore reagent cost.
        GetTarget()->CastSpell(nullptr, aurEff->GetSpellEffectInfo().TriggerSpell, CastSpellExtraArgs(TRIGGERED_FULL_MASK & ~TRIGGERED_IGNORE_POWER_AND_REAGENT_COST).SetTriggeringAura(aurEff));
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_silithus_summon_cultist_periodic::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

void AddSC_silithus()
{
    RegisterGameObjectAI(go_crystalline_tear);
    RegisterCreatureAI(npc_anachronos_quest_trigger);
    RegisterCreatureAI(npc_anachronos_the_ancient);
    RegisterCreatureAI(npc_qiraj_war_spawn);
    new go_wind_stone();
    RegisterSpellScript(spell_silithus_summon_cultist_periodic);
}
