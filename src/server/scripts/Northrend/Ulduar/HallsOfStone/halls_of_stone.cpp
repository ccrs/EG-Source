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

#include "halls_of_stone.h"
#include "InstanceScript.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "TemporarySummon.h"

enum BrannHoSTexts
{
    SAY_KILL                            = 0,
    SAY_LOW_HEALTH                      = 1,
    SAY_DEATH                           = 2,
    SAY_PLAYER_DEATH                    = 3,
    SAY_ESCORT_START                    = 4,

    SAY_SPAWN_DWARF                     = 5,
    SAY_SPAWN_TROGG                     = 6,
    SAY_SPAWN_OOZE                      = 7,
    SAY_SPAWN_EARTHEN                   = 8,

    SAY_EVENT_INTRO_1                   = 9,
    SAY_EVENT_INTRO_2                   = 10,
    SAY_EVENT_A_1                       = 11,
    SAY_EVENT_A_3                       = 12,
    SAY_EVENT_B_1                       = 13,
    SAY_EVENT_B_3                       = 14,
    SAY_EVENT_C_1                       = 15,
    SAY_EVENT_C_3                       = 16,
    SAY_EVENT_D_1                       = 17,
    SAY_EVENT_D_3                       = 18,

    SAY_EVENT_END_01                    = 19,
    SAY_EVENT_END_02                    = 20,
    SAY_EVENT_END_04                    = 21,
    SAY_EVENT_END_06                    = 22,
    SAY_EVENT_END_08                    = 23,
    SAY_EVENT_END_10                    = 24,
    SAY_EVENT_END_12                    = 25,
    SAY_EVENT_END_14                    = 26,
    SAY_EVENT_END_16                    = 27,
    SAY_EVENT_END_18                    = 28,
    SAY_EVENT_END_20                    = 29,

    SAY_VICTORY_SJONNIR_1               = 30,
    SAY_VICTORY_SJONNIR_2               = 31,
    SAY_ENTRANCE_MEET                   = 32,

    SAY_EVENT_INTRO_3_ABED              = 0,
    SAY_EVENT_C_2_ABED                  = 1,
    SAY_EVENT_D_2_ABED                  = 2,
    SAY_EVENT_D_4_ABED                  = 3,
    SAY_EVENT_END_03_ABED               = 4,
    SAY_EVENT_END_05_ABED               = 5,
    SAY_EVENT_END_07_ABED               = 6,
    SAY_EVENT_END_21_ABED               = 7,

    SAY_EVENT_A_2_KADD                  = 0,
    SAY_EVENT_END_09_KADD               = 1,
    SAY_EVENT_END_11_KADD               = 2,
    SAY_EVENT_END_13_KADD               = 3,

    SAY_EVENT_B_2_MARN                  = 0,
    SAY_EVENT_END_15_MARN               = 1,
    SAY_EVENT_END_17_MARN               = 2,
    SAY_EVENT_END_19_MARN               = 3,

    TEXT_ID_START                       = 13100,
    TEXT_ID_PROGRESS                    = 13101
};

enum BrannHoSCreatures
{
    NPC_BRANN_BRONZEBEARD          = 28070,
    NPC_DARK_MATTER_TARGET         = 28237,
    NPC_SEARING_GAZE_TARGET        = 28265,
    NPC_DARK_RUNE_PROTECTOR        = 27983,
    NPC_DARK_RUNE_STORMCALLER      = 27984,
    NPC_IRON_GOLEM_CUSTODIAN       = 27985,
};

enum BrannHoSSpells
{
    SPELL_STEALTH                       = 58506,

    // Kadrak
    SPELL_GLARE_OF_THE_TRIBUNAL         = 50988,

    // Marnak
    SPELL_SUMMON_DARK_MATTER_TARGET     = 51003,
    SPELL_DARK_MATTER_START             = 51001,
    SPELL_DARK_MATTER                   = 51012,

    // Abedneum
    SPELL_SUMMON_SEARING_GAZE           = 51146,

    SPELL_REWARD_ACHIEVEMENT            = 59046,
};

enum BrannHoSMisc
{
    QUEST_HALLS_OF_STONE                = 13207,

    DATA_BRANN_SPARKLIN_NEWS            = 1
};

enum BrannHoSGossip
{
    GOSSIP_ITEM_START_MID               = 9669,     //Brann, it would be our honor!
    GOSSIP_ITEM_START_OID               = 0,
    GOSSIP_ITEM_PROGRESS_MID            = 9670,    //Let's move Brann, enough of the history lessons!
    GOSSIP_ITEM_PROGRESS_OID            = 0
};

enum BrannHoSActions
{
    ACTION_ACTIVATE_KADDRAK = 1,
    ACTION_ACTIVATE_MARNAK,
    ACTION_ACTIVATE_ABEDNEUM,
    ACTION_RESET,
    ACTION_DARK_MATTER
};

static Position const SpawnLocations[]=
{
    { 945.2f, 400.8f, 206.0f, 4.4f },
    { 960.2f, 380.6f, 206.0f, 3.3f }
};

struct npc_tribuna_controller : public ScriptedAI
{
    npc_tribuna_controller(Creature* creature) : ScriptedAI(creature)
    {
        Initialize();
        _instance = creature->GetInstanceScript();
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
        me->SetImmuneToAll(true);
    }

    void Initialize()
    {
        _kaddrakEncounterTimer = 1500;
        _marnakEncounterTimer = 10000;
        _abedneumEncounterTimer = 10000;

        _kaddrakActivated = false;
        _marnakActivated = false;
        _abedneumActivated = false;
    }

    void Reset() override
    {
        Initialize();

        _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_KADDRAK), false);
        _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_MARNAK), false);
        _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_ABEDNEUM), false);
        _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_SKY_FLOOR), false);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ACTIVATE_KADDRAK:
                _kaddrakActivated = true;
                break;
            case ACTION_ACTIVATE_MARNAK:
                _marnakActivated = true;
                break;
            case ACTION_ACTIVATE_ABEDNEUM:
                _abedneumActivated = true;
                break;
            case ACTION_RESET:
                _kaddrakActivated = false;
                _marnakActivated = false;
                _abedneumActivated = false;
                _kaddrakEncounterTimer = 1500;
                _marnakEncounterTimer = 10000;
                _abedneumEncounterTimer = 10000;
                ForceCombatStop(_instance->GetCreature(DATA_KADDRAK));
                ForceCombatStop(_instance->GetCreature(DATA_MARNAK));
                ForceCombatStop(_instance->GetCreature(DATA_ABEDNEUM));
                ForceCombatStop(_instance->GetCreature(DATA_DARK_MATTER));
                ForceCombatStop(me);
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (_kaddrakActivated)
        {
            if (_kaddrakEncounterTimer <= diff)
            {
                if (Creature* kaddrak = _instance->GetCreature(DATA_KADDRAK))
                    kaddrak->CastSpell(nullptr, SPELL_GLARE_OF_THE_TRIBUNAL);
                _kaddrakEncounterTimer = 1500;
            }
            else
                _kaddrakEncounterTimer -= diff;
        }
        if (_marnakActivated)
        {
            if (_marnakEncounterTimer <= diff)
            {
                if (Creature* darkMatter = _instance->GetCreature(DATA_DARK_MATTER))
                    darkMatter->AI()->DoAction(ACTION_DARK_MATTER);
                _marnakEncounterTimer = urand(30000, 31000);
            }
            else
                _marnakEncounterTimer -= diff;
        }
        if (_abedneumActivated)
        {
            if (_abedneumEncounterTimer <= diff)
            {
                if (Creature* abedneum = _instance->GetCreature(DATA_ABEDNEUM))
                    abedneum->CastSpell(nullptr, SPELL_SUMMON_SEARING_GAZE);
                _abedneumEncounterTimer = urand(30000, 31000);
            }
            else
                _abedneumEncounterTimer -= diff;
        }
    }

private:
    InstanceScript* _instance;
    uint32 _kaddrakEncounterTimer;
    uint32 _marnakEncounterTimer;
    uint32 _abedneumEncounterTimer;
    bool _kaddrakActivated;
    bool _marnakActivated;
    bool _abedneumActivated;
};

static constexpr uint32 PATH_ESCORT_BRANN = 224562;

struct npc_brann_hos : public EscortAI
{
    npc_brann_hos(Creature* creature) : EscortAI(creature)
    {
        Initialize();
        _instance = creature->GetInstanceScript();
        SetDespawnAtEnd(false);
        SetDespawnAtFar(false);
    }

    void Initialize()
    {
        _lowHP = false;
        _battle = false;
        _step = 0;
        _phaseTimer = 0;
        _brannSparklinNew = true;
    }

    void Reset() override
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            Initialize();

            _DespawnDwarf();

            _instance->SetBossState(DATA_TRIBUNAL_OF_AGES, NOT_STARTED);
        }
    }

    void JustDied(Unit* killer) override
    {
        _step = 0;
        _phaseTimer = 0;

        if (Creature* temp = _instance->GetCreature(DATA_TRIBUNAL_OF_THE_AGES))
            temp->AI()->DoAction(ACTION_RESET);

        EscortAI::JustDied(killer);
    }

    void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
    {
        switch (waypointId)
        {
            case 13:
                Talk(SAY_EVENT_INTRO_1);
                _instance->SetBossState(DATA_TRIBUNAL_OF_AGES, IN_PROGRESS);
                SetEscortPaused(true);
                _JumpToNextStep(20000);
                // @todo: There should be a pause here and a gossip should start the next step.
                break;
            case 17:
                Talk(SAY_EVENT_INTRO_2);
                _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_TRIBUNAL_CONSOLE), true);
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                SetEscortPaused(true);
                _JumpToNextStep(8500);
                break;
            case 18:
                SetEscortPaused(true);
                break;
        }
     }

    void JustSummoned(Creature* summoned) override
    {
        _dwarfGUIDList.push_back(summoned->GetGUID());
        AddThreat(me, 0.0f, summoned);
        summoned->AI()->AttackStart(me);
    }

    void DamageTaken(Unit* /*done_by*/, uint32& /*damage*/, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (_instance->GetBossState(DATA_TRIBUNAL_OF_AGES) == IN_PROGRESS)
            _brannSparklinNew = false;
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == DATA_BRANN_SPARKLIN_NEWS)
            return _brannSparklinNew ? 1 : 0;

        return 0;
    }

    void UpdateEscortAI(uint32 diff) override
    {
        if (_phaseTimer <= diff)
        {
            switch (_step)
            {
                case 1:
                    if (_instance->GetBossState(DATA_TRIBUNAL_OF_AGES) != NOT_STARTED)
                        return;
                    _battle = false;
                    Talk(SAY_ESCORT_START);
                    _JumpToNextStep(0);
                    break;
                case 3:
                    SetEscortPaused(false);
                    _JumpToNextStep(0);
                    break;
                case 5:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_INTRO_3_ABED);
                    _JumpToNextStep(8500);
                    break;
                case 6:
                    Talk(SAY_EVENT_A_1);
                    _JumpToNextStep(6500);
                    break;
                case 7:
                    if (Creature* temp = _instance->GetCreature(DATA_KADDRAK))
                        temp->AI()->Talk(SAY_EVENT_A_2_KADD);
                    _JumpToNextStep(12500);
                    break;
                case 8:
                    Talk(SAY_EVENT_A_3);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_KADDRAK), true);
                    if (Creature* temp = _instance->GetCreature(DATA_TRIBUNAL_OF_THE_AGES))
                        temp->AI()->DoAction(ACTION_ACTIVATE_KADDRAK);
                    _JumpToNextStep(5000);
                    break;
                case 9:
                    me->SetReactState(REACT_PASSIVE);
                    _SpawnDwarf(1);
                    _JumpToNextStep(20000);
                    break;
                case 10:
                    Talk(SAY_EVENT_B_1);
                    _JumpToNextStep(6000);
                    break;
                case 11:
                    if (Creature* temp = _instance->GetCreature(DATA_MARNAK))
                        temp->AI()->Talk(SAY_EVENT_B_2_MARN);
                    _SpawnDwarf(1);
                    _JumpToNextStep(20000);
                    break;
                case 12:
                    Talk(SAY_EVENT_B_3);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_MARNAK), true);
                    if (Creature* temp = _instance->GetCreature(DATA_TRIBUNAL_OF_THE_AGES))
                        temp->AI()->DoAction(ACTION_ACTIVATE_MARNAK);
                    _JumpToNextStep(10000);
                    break;
                case 13:
                    _SpawnDwarf(1);
                    _JumpToNextStep(10000);
                    break;
                case 14:
                    _SpawnDwarf(2);
                    _JumpToNextStep(20000);
                    break;
                case 15:
                    Talk(SAY_EVENT_C_1);
                    _SpawnDwarf(1);
                    _JumpToNextStep(10000);
                    break;
                case 16:
                    _SpawnDwarf(2);
                    _JumpToNextStep(20000);
                    break;
                case 17:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_C_2_ABED);
                    _SpawnDwarf(1);
                    _JumpToNextStep(20000);
                    break;
                case 18:
                    Talk(SAY_EVENT_C_3);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_ABEDNEUM), true);
                    if (Creature* temp = _instance->GetCreature(DATA_TRIBUNAL_OF_THE_AGES))
                        temp->AI()->DoAction(ACTION_ACTIVATE_ABEDNEUM);
                    _JumpToNextStep(5000);
                    break;
                case 19:
                    _SpawnDwarf(2);
                    _JumpToNextStep(10000);
                    break;
                case 20:
                    _SpawnDwarf(1);
                    _JumpToNextStep(15000);
                    break;
                case 21:
                    Talk(SAY_EVENT_D_1);
                    _SpawnDwarf(3);
                    _JumpToNextStep(20000);
                    break;
                case 22:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_D_2_ABED);
                    _SpawnDwarf(1);
                    _JumpToNextStep(5000);
                    break;
                case 23:
                    _SpawnDwarf(2);
                    _JumpToNextStep(15000);
                    break;
                case 24:
                    Talk(SAY_EVENT_D_3);
                    _SpawnDwarf(3);
                    _JumpToNextStep(5000);
                    break;
                case 25:
                    _SpawnDwarf(1);
                    _JumpToNextStep(5000);
                    break;
                case 26:
                    _SpawnDwarf(2);
                    _JumpToNextStep(10000);
                    break;
                case 27:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_D_4_ABED);
                    _SpawnDwarf(1);
                    _JumpToNextStep(10000);
                    break;
                case 28:
                    me->SetReactState(REACT_DEFENSIVE);
                    Talk(SAY_EVENT_END_01);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_SKY_FLOOR), true);

                    if (Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_HALLS_OF_STONE, me);

                    _battle = true;
                    SetEscortPaused(false);
                    _JumpToNextStep(6500);
                    break;
                case 29:
                    Talk(SAY_EVENT_END_02);
                    if (Creature* temp = _instance->GetCreature(DATA_TRIBUNAL_OF_THE_AGES))
                        temp->AI()->DoAction(ACTION_RESET);
                    _instance->SetBossState(DATA_TRIBUNAL_OF_AGES, DONE);
                    me->CastSpell(me, SPELL_REWARD_ACHIEVEMENT, true);
                    me->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    _JumpToNextStep(5500);
                    break;
                case 30:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_END_03_ABED);
                    _JumpToNextStep(8500);
                    break;
                case 31:
                    Talk(SAY_EVENT_END_04);
                    _JumpToNextStep(11500);
                    break;
                case 32:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_END_05_ABED);
                    _JumpToNextStep(11500);
                    break;
                case 33:
                    Talk(SAY_EVENT_END_06);
                    _JumpToNextStep(4500);
                    break;
                case 34:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_END_07_ABED);
                    _JumpToNextStep(22500);
                    break;
                case 35:
                    Talk(SAY_EVENT_END_08);
                    _JumpToNextStep(7500);
                    break;
                case 36:
                    if (Creature* temp = _instance->GetCreature(DATA_KADDRAK))
                        temp->AI()->Talk(SAY_EVENT_END_09_KADD);
                    _JumpToNextStep(18500);
                    break;
                case 37:
                    Talk(SAY_EVENT_END_10);
                    _JumpToNextStep(5500);
                    break;
                case 38:
                    if (Creature* temp = _instance->GetCreature(DATA_KADDRAK))
                        temp->AI()->Talk(SAY_EVENT_END_11_KADD);
                    _JumpToNextStep(20500);
                    break;
                case 39:
                    Talk(SAY_EVENT_END_12);
                    _JumpToNextStep(2500);
                    break;
                case 40:
                    if (Creature* temp = _instance->GetCreature(DATA_KADDRAK))
                        temp->AI()->Talk(SAY_EVENT_END_13_KADD);
                    _JumpToNextStep(19500);
                    break;
                case 41:
                    Talk(SAY_EVENT_END_14);
                    _JumpToNextStep(10500);
                    break;
                case 42:
                    if (Creature* temp = _instance->GetCreature(DATA_MARNAK))
                        temp->AI()->Talk(SAY_EVENT_END_15_MARN);
                    _JumpToNextStep(6500);
                    break;
                case 43:
                    Talk(SAY_EVENT_END_16);
                    _JumpToNextStep(6500);
                    break;
                case 44:
                    if (Creature* temp = _instance->GetCreature(DATA_MARNAK))
                        temp->AI()->Talk(SAY_EVENT_END_17_MARN);
                    _JumpToNextStep(25500);
                    break;
                case 45:
                    Talk(SAY_EVENT_END_18);
                    _JumpToNextStep(23500);
                    break;
                case 46:
                    if (Creature* temp = _instance->GetCreature(DATA_MARNAK))
                        temp->AI()->Talk(SAY_EVENT_END_19_MARN);
                    _JumpToNextStep(3500);
                    break;
                case 47:
                    Talk(SAY_EVENT_END_20);
                    _JumpToNextStep(8500);
                    break;
                case 48:
                    if (Creature* temp = _instance->GetCreature(DATA_ABEDNEUM))
                        temp->AI()->Talk(SAY_EVENT_END_21_ABED);
                    _JumpToNextStep(5500);
                    break;
                case 49:
                {
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_KADDRAK), false);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_MARNAK), false);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_ABEDNEUM), false);
                    _instance->HandleGameObject(_instance->GetGuidData(DATA_GO_SKY_FLOOR), false);
                    _JumpToNextStep(180000);
                    break;
                }
                case 50:
                    SetEscortPaused(false);
                    break;
            }
        }
        else
            _phaseTimer -= diff;

        if (!_lowHP && HealthBelowPct(30))
        {
            Talk(SAY_LOW_HEALTH);
            _lowHP = true;
        }
        else if (_lowHP && !HealthBelowPct(30))
            _lowHP = false;

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF + 1 || action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            CloseGossipMenuFor(player);
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            SetEscortPaused(false);
            _step = 1;
            LoadPath(PATH_ESCORT_BRANN);
            Start(true, player->GetGUID(), nullptr, true);
        }

        return true;
    }

    bool OnGossipHello(Player* player) override
    {
        InitGossipMenuFor(player, GOSSIP_ITEM_START_MID);
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        AddGossipItemFor(player, GOSSIP_ITEM_START_MID, GOSSIP_ITEM_START_OID, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        SendGossipMenuFor(player, TEXT_ID_START, me->GetGUID());

        return true;
    }

private:
    void _JumpToNextStep(uint32 uiTimer)
    {
        _phaseTimer = uiTimer;
        ++_step;
    }

    void _SpawnDwarf(uint32 type)
    {
        switch (type)
        {
            case 1:
            {
                uint32 spawnNumber = DUNGEON_MODE(2, 3);
                for (uint8 i = 0; i < spawnNumber; ++i)
                    me->SummonCreature(NPC_DARK_RUNE_PROTECTOR, me->GetRandomPoint(SpawnLocations[urand(0, 1)], frand(0.f, 3.f)), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                me->SummonCreature(NPC_DARK_RUNE_STORMCALLER, me->GetRandomPoint(SpawnLocations[urand(0, 1)], frand(0.f, 3.f)), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            }
            case 2:
                for (uint8 i = 0; i < 2; ++i)
                    me->SummonCreature(NPC_DARK_RUNE_STORMCALLER, me->GetRandomPoint(SpawnLocations[urand(0, 1)], frand(0.f, 3.f)), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
            case 3:
                me->SummonCreature(NPC_IRON_GOLEM_CUSTODIAN, me->GetRandomPoint(SpawnLocations[urand(0, 1)], frand(0.f, 3.f)), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30s);
                break;
        }
    }

    void _DespawnDwarf()
    {
        if (_dwarfGUIDList.empty())
            return;
        for (GuidList::const_iterator itr = _dwarfGUIDList.begin(); itr != _dwarfGUIDList.end(); ++itr)
        {
            Creature* temp = ObjectAccessor::GetCreature(*me, _instance ? (*itr) : ObjectGuid::Empty);
            if (temp && temp->IsAlive())
                temp->DespawnOrUnsummon();
        }
        _dwarfGUIDList.clear();
    }

    uint32 _step;
    uint32 _phaseTimer;
    GuidList _dwarfGUIDList;
    InstanceScript* _instance;
    bool _battle;
    bool _lowHP;
    bool _brannSparklinNew;
};

class achievement_brann_spankin_new : public AchievementCriteriaScript
{
    public:
        achievement_brann_spankin_new() : AchievementCriteriaScript("achievement_brann_spankin_new") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            if (!target)
                return false;

            if (Creature* Brann = target->ToCreature())
                if (Brann->AI()->GetData(DATA_BRANN_SPARKLIN_NEWS))
                    return true;

            return false;
        }
};

void AddSC_halls_of_stone()
{
    RegisterHallsOfStoneCreatureAI(npc_brann_hos);
    RegisterHallsOfStoneCreatureAI(npc_tribuna_controller);
    new achievement_brann_spankin_new();
}
