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

/*
 * Comment: Timer check pending
 */

#include "ScriptMgr.h"
#include "halls_of_lightning.h"
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"

enum IonarSpells
{
    SPELL_BALL_LIGHTNING                          = 52780,
    SPELL_STATIC_OVERLOAD                         = 52658,

    SPELL_DISPERSE                                = 52770,
    SPELL_SUMMON_SPARK                            = 52746,
    SPELL_SPARK_DESPAWN                           = 52776,

    // Spark of Ionar
    SPELL_SPARK_VISUAL_TRIGGER                    = 52667,
    SPELL_RANDOM_LIGHTNING_VISUAL                 = 52663
};

enum IonarTexts
{
    SAY_AGGRO                                     = 0,
    SAY_SPLIT                                     = 1,
    SAY_SLAY                                      = 2,
    SAY_DEATH                                     = 3
};

enum IonarCreatures
{
    NPC_SPARK_OF_IONAR                            = 28926
};

enum IonarMisc
{
    DATA_MAX_SPARKS                               = 5,
    DATA_POINT_CALLBACK                           = 0
};

static constexpr float DATA_MAX_SPARK_DISTANCE = 90.f; // Distance to boss - prevent runs through the whole instance

/*######
## Boss Ionar
######*/

struct boss_ionar : public BossAI
{
    boss_ionar(Creature* creature) : BossAI(creature, BOSS_IONAR)
    {
        Initialize();
    }

    void Initialize()
    {
        _splitPhase = true;
        _dispersed = false;

        _splitTimer = 25 * IN_MILLISECONDS;

        _staticOverloadTimer = urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS);
        _ballLightningTimer = urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS);

        _disperseHealth = 45 + urand(0, 10);
        _playerCount = 0;
    }

    void Reset() override
    {
        BossAI::Reset();

        Initialize();

        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE);
        me->SetControlled(false, UNIT_STATE_ROOT);

        if (!me->IsVisible())
            me->SetVisible(true);
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        Talk(SAY_AGGRO);
    }

    void JustDied(Unit* killer) override
    {
        BossAI::JustDied(killer);
        Talk(SAY_DEATH);
    }

    void KilledUnit(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_SLAY);
    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_DISPERSE)
        {
            for (uint8 i = 0; i < DATA_MAX_SPARKS; ++i)
                me->CastSpell(me, SPELL_SUMMON_SPARK, true);

            me->AttackStop();
            me->SetVisible(false);
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE);
            me->SetControlled(true, UNIT_STATE_ROOT);

            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
        }
    }

    void CallBackSparks()
    {
        Position pos = me->GetPosition();
        for (SummonList::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
        {
            if (Creature* pSpark = ObjectAccessor::GetCreature(*me, *itr))
            {
                if (pSpark->IsAlive())
                {
                    pSpark->SetReactState(REACT_PASSIVE);
                    pSpark->SetSpeedRate(MOVE_RUN, 2.0f);
                    pSpark->GetMotionMaster()->Clear();
                    pSpark->GetMotionMaster()->MovePoint(DATA_POINT_CALLBACK, pos);
                }
                else
                    pSpark->DespawnOrUnsummon();
            }
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        if (summoned->GetEntry() == NPC_SPARK_OF_IONAR)
        {
            summons.Summon(summoned);
            summoned->SetReactState(REACT_PASSIVE);

            if (_playerCount < instance->instance->GetPlayers().getSize())
            {
                MapRefManager::const_iterator it = instance->instance->GetPlayers().begin();
                if (_playerCount > 0)
                {
                    uint8 counter = _playerCount;
                    while (counter > 0)
                    {
                        --counter;
                        ++it;
                    }
                }

                Player* currentPlayer = it->GetSource();
                if (currentPlayer && currentPlayer->IsInWorld() && currentPlayer->IsAlive() && !currentPlayer->IsGameMaster())
                {
                    summoned->SetInCombatWith(currentPlayer);
                    summoned->GetMotionMaster()->MoveChase(currentPlayer, 0.f, false);
                }
                ++_playerCount;
            }
        }
    }

    void SummonedCreatureDespawn(Creature* summoned) override
    {
        summons.Despawn(summoned);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (!me->IsVisible())
            damage = 0;

        if (!_dispersed && me->HealthBelowPctDamaged(_disperseHealth, damage))
        {
            _dispersed = true;

            Talk(SAY_SPLIT);

            _playerCount = 0;
            me->InterruptNonMeleeSpells(false);
            DoCast(me, SPELL_DISPERSE, false);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        // Splitted
        if (!me->IsVisible())
        {
            if (_splitTimer <= diff)
            {
                _splitTimer = 2500;

                // Return sparks to where Ionar splitted
                if (_splitPhase)
                {
                    CallBackSparks();
                    _splitPhase = false;
                }
                else if (summons.empty()) // Lightning effect and restore Ionar
                {
                    me->SetVisible(true);
                    me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE);
                    me->SetControlled(false, UNIT_STATE_ROOT);

                    DoCastAOE(SPELL_SPARK_DESPAWN);

                    _splitTimer = 25 * IN_MILLISECONDS;
                    _splitPhase = true;

                    if (me->GetVictim())
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                }
            }
            else
                _splitTimer -= diff;

            return;
        }

        if (_staticOverloadTimer <= diff)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.f, true))
                DoCast(target, SPELL_STATIC_OVERLOAD);

            _staticOverloadTimer = urand(5 * IN_MILLISECONDS, 6 * IN_MILLISECONDS);
        }
        else
            _staticOverloadTimer -= diff;

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (_ballLightningTimer <= diff)
        {
            DoCastVictim(SPELL_BALL_LIGHTNING);
            _ballLightningTimer = urand(10 * IN_MILLISECONDS, 11 * IN_MILLISECONDS);
        }
        else
            _ballLightningTimer -= diff;


        DoMeleeAttackIfReady();
    }

private:
    bool _splitPhase;
    bool _dispersed;
    uint32 _splitTimer;
    uint32 _staticOverloadTimer;
    uint32 _ballLightningTimer;
    uint32 _disperseHealth;
    uint8 _playerCount;
};

/*######
## npc_spark_of_ionar
######*/

struct npc_spark_of_ionar : public ScriptedAI
{
    npc_spark_of_ionar(Creature* creature) : ScriptedAI(creature)
    {
        Initialize();
        _instance = creature->GetInstanceScript();
    }

    void Initialize()
    {
        _checkTimer = 2 * IN_MILLISECONDS;
    }

    void Reset() override
    {
        Initialize();
        DoCastSelf(SPELL_SPARK_VISUAL_TRIGGER);
        DoCastSelf(SPELL_RANDOM_LIGHTNING_VISUAL);
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE || !_instance)
            return;

        if (id == DATA_POINT_CALLBACK)
            me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 uiDiff) override
    {
        // Despawn if the encounter is not running
        if (_instance->GetBossState(BOSS_IONAR) != IN_PROGRESS)
        {
            me->DespawnOrUnsummon();
            return;
        }

        // Prevent them to follow players through the whole instance
        if (_checkTimer <= uiDiff)
        {
            Creature* ionar = _instance->GetCreature(BOSS_IONAR);
            if (ionar && ionar->IsAlive())
            {
                if (me->GetDistance(ionar) > DATA_MAX_SPARK_DISTANCE)
                {
                    me->SetSpeedRate(MOVE_RUN, 2.0f);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(DATA_POINT_CALLBACK, ionar->GetPosition());
                }
            }
            else
                me->DespawnOrUnsummon();
            _checkTimer = 2*IN_MILLISECONDS;
        }
        else
            _checkTimer -= uiDiff;

        // No melee attack at all!
    }

private:
    InstanceScript* _instance;
    uint32 _checkTimer;
};

void AddSC_boss_ionar()
{
    RegisterHallsOfLightningCreatureAI(boss_ionar);
    RegisterHallsOfLightningCreatureAI(npc_spark_of_ionar);
}
