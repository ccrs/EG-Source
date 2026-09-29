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
#include "Containers.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GameTime.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "World.h"
#include "WowTime.h"
#include <algorithm>

enum BrewfestDarkIronAttackMisc
{
    SPELL_ATTACK_KEG = 42393,
    SPELL_DRINK = 42436,
    SPELL_THROW_MUG = 42300,
    SPELL_CREATE_SAMPLER = 42518,
    SPELL_MUG_BOUNCE = 42522,
    SPELL_WEAK_ALCOHOL = 42523,

    NPC_DARK_IRON_GUZZLER = 23709,
    NPC_DARK_IRON_HERALD = 24536,
    NPC_BREWFEST_REVELER = 24484,
    NPC_BARLEYBREW_FESTIVE_KEG = 23700,
    NPC_THUNDERBREW_FESTIVE_KEG = 23702,
    NPC_GORDOK_FESTIVE_KEG = 23706,
    NPC_DROHN_FESTIVE_KEG = 24372,
    NPC_TCHALI_FESTIVE_KEG = 24373,
    NPC_MAEVE_BARLEYBREW = 23683,
    NPC_ITA_THUNDERBREW = 23684,
    NPC_GORDOK_BREW_BARKER = 23685,
    NPC_DROHN_BARKER = 24492,
    NPC_TCHALI_BARKER = 24493,

    GO_DARK_IRON_MOLE_MACHINE = 186685,
    GO_SUPER_BREW_STEIN = 186478,
    GO_MOLE_MACHINE_WRECKAGE_ALLIANCE = 189989,
    GO_MOLE_MACHINE_WRECKAGE_HORDE = 189990,

    SAY_HERALD_VERSE_FIRST = 0,
    SAY_HERALD_VERSE_LAST = 4,
    SAY_HERALD_RETREAT,
    SAY_HERALD_VICTORY,

    SAY_REVELER_FLEE = 0,

    SAY_BARKER_CHUG_AND_CHUCK = 1,
    SAY_BARKER_SUPER_BREW,

    SAY_GUZZLER_ARRIVE = 0,

    WORLD_STATE_GUZZLERS_LOST = 3096,

    MAP_KALIMDOR = 1,

    POINT_KEG = 1,
    POINT_FLEE,

    GROUP_ATTACK = 1,
    GROUP_MOLE_MACHINES
};

static constexpr float KegSearchRange = 80.0f;
static constexpr float KegApproachDistance = 2.0f;
static constexpr float KegAttackRange = 5.0f;
static constexpr float RevelerSearchRange = 100.0f;
static constexpr float RevelerFleeDistance = 20.0f;
static constexpr float BarkerSearchRange = 100.0f;
static constexpr float MoleMachineMinDistance = 5.0f;
static constexpr float MoleMachineMaxDistance = 20.0f;
static constexpr float SuperBrewDistance = 10.0f;
static constexpr float MugRefillRange = 40.0f;

static FindCreatureOptions const FestiveKegOptions = { .CreatureIds = { NPC_BARLEYBREW_FESTIVE_KEG, NPC_THUNDERBREW_FESTIVE_KEG, NPC_GORDOK_FESTIVE_KEG, NPC_DROHN_FESTIVE_KEG, NPC_TCHALI_FESTIVE_KEG }, .IsAlive = true };
static FindCreatureOptions const BarkerOptions = { .CreatureIds = { NPC_MAEVE_BARLEYBREW, NPC_ITA_THUNDERBREW, NPC_GORDOK_BREW_BARKER, NPC_DROHN_BARKER, NPC_TCHALI_BARKER }, .IsAlive = true };

// 23703 - [DND] Brewfest Dark Iron Event Generator
struct EG_npc_brewfest_dark_iron_attack_generator : public ScriptedAI
{
    EG_npc_brewfest_dark_iron_attack_generator(Creature* creature) : ScriptedAI(creature), _summons(me), _lastAttackMinute(0), _guzzlersLost(0), _attackInProgress(false) { }

    void Reset() override
    {
        _scheduler.CancelAll();
        _summons.DespawnAll();
        _heraldGUID.Clear();
        _kegGUIDs.clear();
        _attackInProgress = false;

        _scheduler.Schedule(1s, [this](TaskContext context)
        {
            time_t const minute = GameTime::GetGameTime() / MINUTE;
            if (!_attackInProgress && GameTime::GetWowTime()->GetMinute() % 30 == 0 && minute != _lastAttackMinute)
            {
                _lastAttackMinute = minute;

                std::vector<Creature*> kegs;
                me->GetCreatureListWithOptionsInGrid(kegs, KegSearchRange, FestiveKegOptions);
                for (Creature* keg : kegs)
                {
                    keg->SetReactState(REACT_PASSIVE);
                    _kegGUIDs.push_back(keg->GetGUID());
                }

                if (!_kegGUIDs.empty())
                {
                    _attackInProgress = true;
                    _guzzlersLost = 0;

                    std::vector<Creature*> revelers;
                    me->GetCreatureListWithEntryInGrid(revelers, NPC_BREWFEST_REVELER, RevelerSearchRange);
                    for (Creature* reveler : revelers)
                    {
                        if (!reveler->IsAlive())
                            continue;

                        if (roll_chance_i(25))
                            reveler->AI()->Talk(SAY_REVELER_FLEE);

                        reveler->SetWalk(false);
                        reveler->GetMotionMaster()->MovePoint(POINT_FLEE, reveler->GetFirstCollisionPosition(RevelerFleeDistance, reveler->GetRelativeAngle(me) + float(M_PI)));
                        reveler->DespawnOrUnsummon(5s);
                    }

                    if (Creature* herald = me->SummonCreature(NPC_DARK_IRON_HERALD, me->GetPosition(), TEMPSUMMON_MANUAL_DESPAWN))
                        _heraldGUID = herald->GetGUID();

                    context.Schedule(1500ms, GROUP_ATTACK, [this](TaskContext song)
                    {
                        uint8 const verse = uint8(SAY_HERALD_VERSE_FIRST + song.GetRepeatCounter());
                        if (Creature* herald = ObjectAccessor::GetCreature(*me, _heraldGUID))
                            herald->AI()->Talk(verse);

                        if (verse < SAY_HERALD_VERSE_LAST)
                            song.Repeat(28s);
                    }).Schedule(1500ms, GROUP_MOLE_MACHINES, [this](TaskContext moleMachine)
                    {
                        float const distance = frand(MoleMachineMinDistance, MoleMachineMaxDistance);
                        float const angle = frand(0.0f, 2.0f * float(M_PI));
                        Position const& center = me->GetHomePosition();
                        Position pos(center.GetPositionX() + distance * std::cos(angle), center.GetPositionY() + distance * std::sin(angle), center.GetPositionZ(), angle);
                        me->UpdateGroundPositionZ(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
                        me->SummonGameObject(GO_DARK_IRON_MOLE_MACHINE, pos, QuaternionData::fromEulerAnglesZYX(angle, 0.0f, 0.0f), 6s, GO_SUMMON_TIMED_DESPAWN);

                        for (Seconds delay : { 2s, 4s })
                        {
                            moleMachine.Schedule(delay, GROUP_ATTACK, [this, pos](TaskContext /*guzzler*/)
                            {
                                me->SummonCreature(NPC_DARK_IRON_GUZZLER, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 6s);
                            });
                        }

                        moleMachine.Repeat(3s);
                    }).Schedule(5s, GROUP_ATTACK, [this](TaskContext shout)
                    {
                        std::vector<Creature*> barkers;
                        me->GetCreatureListWithOptionsInGrid(barkers, BarkerSearchRange, BarkerOptions);

                        if (!barkers.empty())
                        {
                            Creature* barker = Trinity::Containers::SelectRandomContainerElement(barkers);
                            if (shout.GetRepeatCounter() % 3 == 2)
                            {
                                barker->AI()->Talk(SAY_BARKER_SUPER_BREW);
                                Position const pos = barker->GetFirstCollisionPosition(SuperBrewDistance, 0.0f);
                                barker->SummonGameObject(GO_SUPER_BREW_STEIN, pos, QuaternionData::fromEulerAnglesZYX(pos.GetOrientation(), 0.0f, 0.0f), 30s, GO_SUMMON_TIMED_DESPAWN);
                            }
                            else
                                barker->AI()->Talk(SAY_BARKER_CHUG_AND_CHUCK);
                        }

                        shout.Repeat(12s);
                    }).Schedule(1s, GROUP_ATTACK, [this](TaskContext kegCheck)
                    {
                        bool const anyKegAlive = std::any_of(_kegGUIDs.begin(), _kegGUIDs.end(), [this](ObjectGuid const& guid)
                        {
                            Creature const* keg = ObjectAccessor::GetCreature(*me, guid);
                            return keg && keg->IsAlive();
                        });

                        if (!anyKegAlive)
                        {
                            EndAttack(kegCheck, false);
                            return;
                        }

                        kegCheck.Repeat();
                    }).Schedule(280s, GROUP_ATTACK, [](TaskContext stopSpawning)
                    {
                        stopSpawning.CancelGroup(GROUP_MOLE_MACHINES);
                    }).Schedule(300s, GROUP_ATTACK, [this](TaskContext attackEnd)
                    {
                        EndAttack(attackEnd, true);
                    });
                }
            }

            context.Repeat();
        });
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        _summons.Despawn(summon);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == NPC_DARK_IRON_GUZZLER)
            ++_guzzlersLost;
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    void EndAttack(TaskContext& context, bool defended)
    {
        if (!_attackInProgress)
            return;

        _attackInProgress = false;
        context.CancelGroup(GROUP_ATTACK);
        context.CancelGroup(GROUP_MOLE_MACHINES);

        if (Creature* herald = ObjectAccessor::GetCreature(*me, _heraldGUID))
        {
            std::vector<Player*> players;
            me->GetPlayerListInGrid(players, sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL), false);
            for (Player const* player : players)
                player->SendUpdateWorldState(WORLD_STATE_GUZZLERS_LOST, _guzzlersLost);

            herald->AI()->Talk(defended ? SAY_HERALD_RETREAT : SAY_HERALD_VICTORY);
            herald->DespawnOrUnsummon(5s);
        }

        if (defended)
            me->SummonGameObject(me->GetMapId() == MAP_KALIMDOR ? GO_MOLE_MACHINE_WRECKAGE_HORDE : GO_MOLE_MACHINE_WRECKAGE_ALLIANCE, me->GetPosition(), QuaternionData::fromEulerAnglesZYX(me->GetOrientation(), 0.0f, 0.0f), 15min, GO_SUMMON_TIMED_DESPAWN);

        _summons.DespawnEntry(NPC_DARK_IRON_GUZZLER);
        _heraldGUID.Clear();
        _kegGUIDs.clear();
    }

    TaskScheduler _scheduler;
    SummonList _summons;
    ObjectGuid _heraldGUID;
    GuidVector _kegGUIDs;
    time_t _lastAttackMinute;
    uint32 _guzzlersLost;
    bool _attackInProgress;
};

// 23709 - Dark Iron Guzzler
struct EG_npc_brewfest_dark_iron_guzzler : public ScriptedAI
{
    EG_npc_brewfest_dark_iron_guzzler(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* /*summoner*/) override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetWalk(true);
        if (roll_chance_i(5))
            Talk(SAY_GUZZLER_ARRIVE);

        MoveToNextKeg();
        _scheduler.Schedule(2s, [this](TaskContext context)
        {
            Creature const* keg = ObjectAccessor::GetCreature(*me, _kegGUID);
            if (!keg || !keg->IsAlive())
                MoveToNextKeg();
            else if (me->IsWithinDist(keg, KegAttackRange))
                DoCastAOE(SPELL_ATTACK_KEG);

            context.Repeat();
        });
    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id != SPELL_DRINK || !me->IsAlive())
            return;

        DoCastSelf(SPELL_MUG_BOUNCE, true);
        me->KillSelf();
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        damage = 0;
    }

    void AttackStart(Unit* /*target*/) override { }

    void EnterEvadeMode(EvadeReason /*why*/) override { }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    void MoveToNextKeg()
    {
        std::vector<Creature*> kegs;
        me->GetCreatureListWithOptionsInGrid(kegs, KegSearchRange, FestiveKegOptions);
        if (kegs.empty())
        {
            me->DespawnOrUnsummon();
            return;
        }

        Creature* keg = Trinity::Containers::SelectRandomContainerElement(kegs);
        _kegGUID = keg->GetGUID();
        me->GetMotionMaster()->MoveCloserAndStop(POINT_KEG, keg, KegApproachDistance);
    }

    TaskScheduler _scheduler;
    ObjectGuid _kegGUID;
};

// 186685 - Dark Iron Mole Machine
struct EG_go_brewfest_dark_iron_mole_machine : public GameObjectAI
{
    EG_go_brewfest_dark_iron_mole_machine(GameObject* go) : GameObjectAI(go) { }

    void Reset() override
    {
        me->SetLootState(GO_READY);
        _scheduler.Schedule(1s, [this](TaskContext /*context*/)
        {
            me->UseDoorOrButton(5 * IN_MILLISECONDS);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);
    }

private:
    TaskScheduler _scheduler;
};

// 42436 - Drink!
class EG_spell_brewfest_toss_mug : public SpellScript
{
    PrepareSpellScript(EG_spell_brewfest_toss_mug);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_THROW_MUG, SPELL_WEAK_ALCOHOL });
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (Creature* barker = caster->FindNearestCreatureWithOptions(MugRefillRange, BarkerOptions))
            barker->CastSpell(caster, SPELL_THROW_MUG, true);

        caster->CastSpell(caster, SPELL_WEAK_ALCOHOL, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(EG_spell_brewfest_toss_mug::HandleAfterCast);
    }
};

// 42300 - Brewfest - Throw Mug
class EG_spell_brewfest_add_mug : public SpellScript
{
    PrepareSpellScript(EG_spell_brewfest_add_mug);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_CREATE_SAMPLER });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Player* player = GetHitPlayer())
            player->CastSpell(player, SPELL_CREATE_SAMPLER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_brewfest_add_mug::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

void AddSC_EG_event_scripts()
{
    RegisterCreatureAI(EG_npc_brewfest_dark_iron_attack_generator);
    RegisterCreatureAI(EG_npc_brewfest_dark_iron_guzzler);
    RegisterGameObjectAI(EG_go_brewfest_dark_iron_mole_machine);
    RegisterSpellScript(EG_spell_brewfest_toss_mug);
    RegisterSpellScript(EG_spell_brewfest_add_mug);
}
