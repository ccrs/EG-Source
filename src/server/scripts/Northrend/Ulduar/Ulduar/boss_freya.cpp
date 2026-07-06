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

#include "ulduar.h"
#include "Containers.h"
#include "GameTime.h"
#include "InstanceScript.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "TemporarySummon.h"

enum FreyaYells
{
    // Freya
    SAY_AGGRO                                    = 0,
    SAY_AGGRO_WITH_ELDER                         = 1,
    SAY_SLAY                                     = 2,
    SAY_DEATH                                    = 3,
    SAY_BERSERK                                  = 4,
    SAY_SUMMON_CONSERVATOR                       = 5,
    SAY_SUMMON_TRIO                              = 6,
    SAY_SUMMON_LASHERS                           = 7,
    EMOTE_LIFEBINDERS_GIFT                       = 8,
    EMOTE_ALLIES_OF_NATURE                       = 9,
    EMOTE_GROUND_TREMOR                          = 10,
    EMOTE_IRON_ROOTS                             = 11,

    // Elder Brightleaf / Elder Ironbranch / Elder Stonebark
    SAY_ELDER_AGGRO                              = 0,
    SAY_ELDER_SLAY                               = 1,
    SAY_ELDER_DEATH                              = 2
};

enum FreyaSpells
{
    // Freya
    SPELL_ATTUNED_TO_NATURE                      = 62519,
    SPELL_TOUCH_OF_EONAR                         = 62528,
    SPELL_SUNBEAM                                = 62623,
    SPELL_ENRAGE                                 = 47008,
    SPELL_FREYA_GROUND_TREMOR                    = 62437,
    SPELL_ROOTS_FREYA                            = 62283,
    SPELL_STONEBARK_ESSENCE                      = 62483,
    SPELL_IRONBRANCH_ESSENCE                     = 62484,
    SPELL_BRIGHTLEAF_ESSENCE                     = 62485,
    SPELL_DRAINED_OF_POWER                       = 62467,
    SPELL_SUMMON_EONAR_GIFT                      = 62572,

    // Stonebark
    SPELL_FISTS_OF_STONE                         = 62344,
    SPELL_GROUND_TREMOR                          = 62325,
    SPELL_PETRIFIED_BARK                         = 62337,
    SPELL_PETRIFIED_BARK_DMG                     = 62379,

    // Ironbranch
    SPELL_IMPALE                                 = 62310,
    SPELL_ROOTS_IRONBRANCH                       = 62438,
    SPELL_THORN_SWARM                            = 62285,

    // Brightleaf
    SPELL_FLUX_AURA                              = 62239,
    SPELL_FLUX                                   = 62262,
    SPELL_FLUX_PLUS                              = 62251,
    SPELL_FLUX_MINUS                             = 62252,
    SPELL_SOLAR_FLARE                            = 62240,
    SPELL_UNSTABLE_SUN_BEAM_SUMMON               = 62207, // Trigger 62221

    // Stack Removing of Attuned to Nature
    SPELL_REMOVE_25STACK                         = 62521,
    SPELL_REMOVE_10STACK                         = 62525,
    SPELL_REMOVE_2STACK                          = 62524,

    // Achievement spells
    SPELL_DEFORESTATION_CREDIT                   = 65015,
    SPELL_KNOCK_ON_WOOD_CREDIT                   = 65074,

    // Wave summoning spells
    SPELL_SUMMON_LASHERS                         = 62687,
    SPELL_SUMMON_TRIO                            = 62686,
    SPELL_SUMMON_ANCIENT_CONSERVATOR             = 62685,

    // Detonating Lasher
    SPELL_DETONATE                               = 62598,
    SPELL_FLAME_LASH                             = 62608,

    // Ancient Water Spirit
    SPELL_TIDAL_WAVE                             = 62653,
    SPELL_TIDAL_WAVE_EFFECT                      = 62654,

    // Storm Lasher
    SPELL_LIGHTNING_LASH                         = 62648,
    SPELL_STORMBOLT                              = 62649,

    // Snaplasher
    SPELL_HARDENED_BARK                          = 62664,
    SPELL_BARK_AURA                              = 62663,

    // Ancient Conservator
    SPELL_CONSERVATOR_GRIP                       = 62532,
    SPELL_NATURE_FURY                            = 62589,
    SPELL_SUMMON_PERIODIC                        = 62566,

    // Healthly Spore
    SPELL_HEALTHY_SPORE_VISUAL                   = 62538,
    SPELL_GROW                                   = 62559,
    SPELL_POTENT_PHEROMONES                      = 62541,

    // Eonar's Gift
    SPELL_LIFEBINDERS_GIFT                       = 62584,
    SPELL_PHEROMONES                             = 62619,
    SPELL_EONAR_VISUAL                           = 62579,

    // Nature Bomb
    SPELL_NATURE_BOMB                            = 64587,
    SPELL_OBJECT_BOMB                            = 64600,
    SPELL_SUMMON_NATURE_BOMB                     = 64604,

    // Unstable Sun Beam
    SPELL_UNSTABLE_SUN_BEAM                      = 62211,
    SPELL_UNSTABLE_ENERGY                        = 62217,
    SPELL_PHOTOSYNTHESIS                         = 62209,
    SPELL_UNSTABLE_SUN_BEAM_TRIGGERED            = 62243,
    SPELL_FREYA_UNSTABLE_SUNBEAM                 = 62450,

    // Sun Beam
    SPELL_FREYA_UNSTABLE_ENERGY                  = 62451,
    SPELL_FREYA_UNSTABLE_ENERGY_VISUAL           = 62216,

    // Attuned To Nature spells
    SPELL_ATTUNED_TO_NATURE_2_DOSE_REDUCTION     = 62524,
    SPELL_ATTUNED_TO_NATURE_10_DOSE_REDUCTION    = 62525,
    SPELL_ATTUNED_TO_NATURE_25_DOSE_REDUCTION    = 62521
};

enum FreyaNpcs
{
    NPC_SUN_BEAM                                 = 33170,
    NPC_DETONATING_LASHER                        = 32918,
    NPC_ANCIENT_CONSERVATOR                      = 33203,
    NPC_ANCIENT_WATER_SPIRIT                     = 33202,
    NPC_STORM_LASHER                             = 32919,
    NPC_SNAPLASHER                               = 32916,
    NPC_NATURE_BOMB                              = 34129,
    NPC_EONARS_GIFT                              = 33228,
    NPC_HEALTHY_SPORE                            = 33215,
    NPC_UNSTABLE_SUN_BEAM                        = 33050,
    NPC_IRON_ROOTS                               = 33088,
    NPC_STRENGTHENED_IRON_ROOTS                  = 33168,

    OBJECT_NATURE_BOMB                           = 194902
};

enum FreyaActions
{
    ACTION_ELDER_FREYA_KILLED                    = 1
};

enum FreyaTrioLasherType
{
    LASHER_TYPE_WATER_SPIRIT                     = 1,
    LASHER_TYPE_STORM_LASHER                     = 2,
    LASHER_TYPE_SNAPLASHER                       = 4
};

constexpr std::size_t DEFORESTATION_TRIO_KILLS = 6;
constexpr uint32 DEFORESTATION_WINDOW_MS = 10000;

struct TrioDeath
{
    uint32 time;
    uint32 type;
};

struct TrioWave
{
    GuidVector members;       // the three elementals summoned for this wave
    bool defeated = false;    // all three killed within the window -> gone for good
    bool reviveArmed = false; // 12s revive timer running for this wave
};

enum FreyaEvents
{
    // Freya
    EVENT_WAVE                                   = 1,
    EVENT_EONAR_GIFT                             = 2,
    EVENT_NATURE_BOMB                            = 3,
    EVENT_UNSTABLE_ENERGY                        = 4,
    EVENT_STRENGTHENED_IRON_ROOTS                = 5,
    EVENT_GROUND_TREMOR                          = 6,
    EVENT_SUNBEAM                                = 7,
    EVENT_ENRAGE                                 = 8,

    // Elder Stonebark
    EVENT_TREMOR                                 = 9,
    EVENT_BARK                                   = 10,
    EVENT_FISTS                                  = 11,

    // Elder Ironbranch
    EVENT_IMPALE                                 = 12,
    EVENT_IRON_ROOTS                             = 13,
    EVENT_THORN_SWARM                            = 14,

    // Elder Brightleaf
    EVENT_SOLAR_FLARE                            = 15,
    EVENT_UNSTABLE_SUN_BEAM                      = 16,
    EVENT_FLUX                                   = 17,

    // Trio elemental revive windows (one per wave)
    EVENT_TRIO_REVIVE_0                          = 18,
    EVENT_TRIO_REVIVE_1                          = 19
};

enum FreyaMisc
{
    DATA_GETTING_BACK_TO_NATURE                 = 1,
    DATA_KNOCK_ON_WOOD                          = 2
};

constexpr Seconds FREYA_WAVE_TIME_MIN = 30s;
constexpr Seconds FREYA_WAVE_TIME_MAX = 60s;

struct npc_iron_roots : public ScriptedAI
{
    npc_iron_roots(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
    }

    void IsSummonedBy(WorldObject* summonerWO) override
    {
        Player* summoner = summonerWO->ToPlayer();
        if (!summoner)
            return;
        // Summoner is a player, who should have root aura on self
        summonerGUID = summoner->GetGUID();
        me->SetFacingToObject(summoner);
        me->SetInCombatWith(summoner);
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (Player* target = ObjectAccessor::GetPlayer(*me, summonerGUID))
        {
            target->RemoveAurasDueToSpell(SPELL_ROOTS_IRONBRANCH);
            target->RemoveAurasDueToSpell(SPELL_ROOTS_FREYA);
        }

        me->DespawnOrUnsummon();
    }

private:
    ObjectGuid summonerGUID;
};

struct boss_freya : public BossAI
{
    boss_freya(Creature* creature) : BossAI(creature, DATA_FREYA)
    {
        _encounterFinished = false;
        Initialize();
    }

    void Initialize()
    {
        trioWaveCount = 0;
        waveCount = 0;
        elderCount = 0;
        _natureBombsUnlocked = false;

        for (TrioWave& wave : _trioWaves)
        {
            wave.members.clear();
            wave.defeated = false;
            wave.reviveArmed = false;
        }
        _trioDeaths.clear();
        _waveBag.clear();
    }

    void Reset() override
    {
        if (_encounterFinished)
            return;

        _Reset();
        Initialize();
    }

    void KilledUnit(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_SLAY);
    }

    void DamageTaken(Unit* who, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (damage >= me->GetHealth())
        {
            damage = 0;
            JustDied(who);
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        DoZoneInCombat();
        Creature* Elder[3];
        for (uint8 n = 0; n < 3; ++n)
        {
            Elder[n] = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BRIGHTLEAF + n));
            if (Elder[n] && Elder[n]->IsAlive())
            {
                me->AddAura(SPELL_DRAINED_OF_POWER, Elder[n]);
                Elder[n]->RemoveLootMode(LOOT_MODE_DEFAULT); // banished elders drop no loot in hard mode
                Elder[n]->AI()->AttackStart(who);
                AddThreat(who, 250.0f, Elder[n]);
                ++elderCount;
            }
        }

        if (Elder[0] && Elder[0]->IsAlive()) // Brightleaf - magic-damage essence + Sun Beam
        {
            Elder[0]->CastSpell(me, SPELL_BRIGHTLEAF_ESSENCE, true);
            events.ScheduleEvent(EVENT_UNSTABLE_ENERGY, 10s, 20s);
        }

        if (Elder[1] && Elder[1]->IsAlive()) // Ironbranch - allies' physical-damage essence + Iron Roots
        {
            Elder[1]->CastSpell(me, SPELL_IRONBRANCH_ESSENCE, true);
            events.ScheduleEvent(EVENT_STRENGTHENED_IRON_ROOTS, 10s, 20s);
        }

        if (Elder[2] && Elder[2]->IsAlive()) // Stonebark - Freya's physical-damage essence + Ground Tremor
        {
            Elder[2]->CastSpell(me, SPELL_STONEBARK_ESSENCE, true);
            events.ScheduleEvent(EVENT_GROUND_TREMOR, 10s, 20s);
        }

        if (elderCount == 0)
            Talk(SAY_AGGRO);
        else
            Talk(SAY_AGGRO_WITH_ELDER);

        CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
        args.AddSpellMod(SPELLVALUE_AURA_STACK, 150);
        me->CastSpell(me, SPELL_ATTUNED_TO_NATURE, args);

        me->CastSpell(me, SPELL_TOUCH_OF_EONAR, true); // permanent, unremovable self-heal aura

        events.ScheduleEvent(EVENT_WAVE, 10s);
        events.ScheduleEvent(EVENT_EONAR_GIFT, 25s);
        events.ScheduleEvent(EVENT_ENRAGE, 10min);
        events.ScheduleEvent(EVENT_SUNBEAM, 5s, 15s);
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case DATA_GETTING_BACK_TO_NATURE:
                if (Aura const* aura = me->GetAura(SPELL_ATTUNED_TO_NATURE))
                    return aura->GetStackAmount();
                return 0;
            case DATA_KNOCK_ON_WOOD:
                return elderCount;
        }

        return 0;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (!_natureBombsUnlocked && waveCount >= 6 && !me->HasAura(SPELL_ATTUNED_TO_NATURE))
        {
            _natureBombsUnlocked = true;
            events.ScheduleEvent(EVENT_NATURE_BOMB, 5s);
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ENRAGE:
                    Talk(SAY_BERSERK);
                    DoCast(me, SPELL_ENRAGE);
                    break;
                case EVENT_SUNBEAM:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                        DoCast(target, SPELL_SUNBEAM);
                    events.ScheduleEvent(EVENT_SUNBEAM, 10s, 15s);
                    break;
                case EVENT_NATURE_BOMB:
                    DoCastAOE(SPELL_SUMMON_NATURE_BOMB, true);
                    events.ScheduleEvent(EVENT_NATURE_BOMB, 10s, 12s);
                    break;
                case EVENT_UNSTABLE_ENERGY:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                        DoCast(target, SPELL_FREYA_UNSTABLE_SUNBEAM, true);
                    events.ScheduleEvent(EVENT_UNSTABLE_ENERGY, 15s, 20s);
                    break;
                case EVENT_WAVE:
                    SpawnWave();
                    if (waveCount < 6)
                        events.ScheduleEvent(EVENT_WAVE, FREYA_WAVE_TIME_MIN, FREYA_WAVE_TIME_MAX);
                    break;
                case EVENT_EONAR_GIFT:
                    Talk(EMOTE_LIFEBINDERS_GIFT);
                    DoCast(me, SPELL_SUMMON_EONAR_GIFT);
                    events.ScheduleEvent(EVENT_EONAR_GIFT, 40s, 50s);
                    break;
                case EVENT_STRENGTHENED_IRON_ROOTS:
                    Talk(EMOTE_IRON_ROOTS);
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true, true, -SPELL_ROOTS_FREYA))
                        target->CastSpell(target, SPELL_ROOTS_FREYA, true); // This must be cast by Target self
                    events.ScheduleEvent(EVENT_STRENGTHENED_IRON_ROOTS, 12s, 20s);
                    break;
                case EVENT_GROUND_TREMOR:
                    Talk(EMOTE_GROUND_TREMOR);
                    DoCastAOE(SPELL_FREYA_GROUND_TREMOR);
                    events.ScheduleEvent(EVENT_GROUND_TREMOR, 25s, 28s);
                    break;
                case EVENT_TRIO_REVIVE_0:
                case EVENT_TRIO_REVIVE_1:
                    ReviveTrio(uint8(eventId - EVENT_TRIO_REVIVE_0));
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (_encounterFinished)
            return;

        _encounterFinished = true;

        //! Freya's chest is dynamically spawned on death by different spells.
        uint32 const summonSpell[2][4] =
        {
                      /* 0Elder, 1Elder, 2Elder, 3Elder */
            /* 10N */    {62950, 62952, 62953, 62954},
            /* 25N */    {62955, 62956, 62957, 62958}
        };

        me->CastSpell(nullptr, summonSpell[me->GetMap()->GetDifficultyID()][elderCount], true);

        Talk(SAY_DEATH);

        me->SetReactState(REACT_PASSIVE);
        me->InterruptNonMeleeSpells(true);
        me->RemoveAllAttackers();
        me->AttackStop();
        me->SetFaction(FACTION_FRIENDLY);
        me->DespawnOrUnsummon(7500ms);
        me->CastSpell(me, SPELL_KNOCK_ON_WOOD_CREDIT, true);
        _JustDied();

        for (uint8 n = 0; n < 3; ++n)
        {
            Creature* Elder = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_BRIGHTLEAF + n));
            if (Elder && Elder->IsAlive())
                Elder->AI()->DoAction(ACTION_ELDER_FREYA_KILLED);
        }
    }

    void JustSummoned(Creature* summoned) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_SNAPLASHER:
            case NPC_ANCIENT_WATER_SPIRIT:
            case NPC_STORM_LASHER:
                if (trioWaveCount < _trioWaves.size())
                    _trioWaves[trioWaveCount].members.push_back(summoned->GetGUID());
                summons.Summon(summoned);
                break;
            case NPC_DETONATING_LASHER:
            case NPC_ANCIENT_CONSERVATOR:
                summons.Summon(summoned);
                break;
            default:
                summons.Summon(summoned);
                return;
        }

        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 250.0f, true))
        {
            summoned->AI()->AttackStart(target);
            AddThreat(target, 250.0f, summoned);
            DoZoneInCombat(summoned);
        }
    }

    void SummonedCreatureDies(Creature* summoned, Unit* who) override
    {
        switch (summoned->GetEntry())
        {
            case NPC_DETONATING_LASHER:
                summoned->CastSpell(me, SPELL_REMOVE_2STACK, true);
                summoned->CastSpell(who, SPELL_DETONATE, true);
                summoned->DespawnOrUnsummon(5s);
                summons.Despawn(summoned);
                break;
            case NPC_ANCIENT_CONSERVATOR:
                summoned->CastSpell(me, SPELL_REMOVE_25STACK, true);
                summoned->DespawnOrUnsummon(5s);
                summons.Despawn(summoned);
                break;
        }
    }

    void SetGUID(ObjectGuid const& guid, int32 id) override
    {
        LasherDead(uint32(id));

        int8 const wave = GetTrioWave(guid);
        if (wave < 0 || _trioWaves[wave].defeated)
            return;

        if (IsTrioDead(wave))
            DefeatTrio(wave);
        else if (!_trioWaves[wave].reviveArmed)
        {
            _trioWaves[wave].reviveArmed = true;
            events.ScheduleEvent(EVENT_TRIO_REVIVE_0 + wave, 12s);
        }
    }

private:
    void LasherDead(uint32 type)
    {
        _trioDeaths.push_back({ GameTime::GetGameTimeMS(), type });
        if (_trioDeaths.size() > DEFORESTATION_TRIO_KILLS)
            _trioDeaths.erase(_trioDeaths.begin());

        if (!instance || _trioDeaths.size() < DEFORESTATION_TRIO_KILLS)
            return;

        uint32 typeSum = 0;
        for (TrioDeath const& death : _trioDeaths)
            typeSum += death.type;

        uint32 const allTypesTwice = (LASHER_TYPE_WATER_SPIRIT + LASHER_TYPE_STORM_LASHER + LASHER_TYPE_SNAPLASHER) * 2;
        if (typeSum == allTypesTwice && (_trioDeaths.back().time - _trioDeaths.front().time) < DEFORESTATION_WINDOW_MS)
            instance->DoCastSpellOnPlayers(SPELL_DEFORESTATION_CREDIT);
    }

    uint8 GetWaveId()
    {
        if (_waveBag.empty())
        {
            _waveBag = { 0, 1, 2 };
            Trinity::Containers::RandomShuffle(_waveBag);
        }

        uint8 const id = _waveBag.back();
        _waveBag.pop_back();
        return id;
    }

    void SpawnWave()
    {
        switch (GetWaveId())
        {
            case 0:
                Talk(SAY_SUMMON_LASHERS);
                for (uint8 n = 0; n < 10; ++n)
                    DoCast(SPELL_SUMMON_LASHERS);
                break;
            case 1:
                Talk(SAY_SUMMON_TRIO);
                DoCast(SPELL_SUMMON_TRIO);
                trioWaveCount++;
                break;
            case 2:
                Talk(SAY_SUMMON_CONSERVATOR);
                DoCast(SPELL_SUMMON_ANCIENT_CONSERVATOR);
                break;
        }
        Talk(EMOTE_ALLIES_OF_NATURE);
        waveCount++;
    }

    int8 GetTrioWave(ObjectGuid const& guid) const
    {
        for (std::size_t wave = 0; wave < _trioWaves.size(); ++wave)
        {
            GuidVector const& members = _trioWaves[wave].members;
            if (std::find(members.begin(), members.end(), guid) != members.end())
                return int8(wave);
        }
        return -1;
    }

    bool IsTrioDead(uint8 wave) const
    {
        for (ObjectGuid const& guid : _trioWaves[wave].members)
        {
            Creature const* elemental = ObjectAccessor::GetCreature(*me, guid);
            if (!elemental || elemental->IsAlive())
                return false;
        }
        return true;
    }

    void DefeatTrio(uint8 wave)
    {
        _trioWaves[wave].defeated = true;
        events.CancelEvent(EVENT_TRIO_REVIVE_0 + wave);
        for (ObjectGuid const& guid : _trioWaves[wave].members)
            if (Creature* elemental = ObjectAccessor::GetCreature(*me, guid))
            {
                elemental->CastSpell(me, SPELL_REMOVE_10STACK, true);
                summons.Despawn(elemental);
                elemental->DespawnOrUnsummon(5s);
            }
    }

    void ReviveTrio(uint8 wave)
    {
        _trioWaves[wave].reviveArmed = false;
        if (_trioWaves[wave].defeated)
            return;

        std::vector<Creature*> elementals;
        for (ObjectGuid const& guid : _trioWaves[wave].members)
        {
            Creature* elemental = ObjectAccessor::GetCreature(*me, guid);
            if (!elemental)
                return;
            elementals.push_back(elemental);
        }

        for (Creature* elemental : elementals)
        {
            if (elemental->IsAlive())
                elemental->SetHealth(elemental->GetMaxHealth());
            else
            {
                elemental->setDeathState(ALIVE);
                elemental->SetHealth(elemental->GetMaxHealth());
                DoZoneInCombat(elemental);
            }
        }
    }

    uint8 trioWaveCount;
    uint8 waveCount;
    uint8 elderCount;
    bool _encounterFinished;
    bool _natureBombsUnlocked;
    std::array<TrioWave, 2> _trioWaves; // the two trio waves: members + defeated/revive state
    std::vector<uint8> _waveBag; // shuffled bag of remaining wave ids for the current cycle
    std::vector<TrioDeath> _trioDeaths; // recent trio deaths (time + type) for the Deforestation achievement
};

struct boss_elder_brightleaf : public BossAI
{
    boss_elder_brightleaf(Creature* creature) : BossAI(creature, DATA_BRIGHTLEAF)
    {
    }

    void Reset() override
    {
        _Reset();
        if (me->HasAura(SPELL_DRAINED_OF_POWER))
            me->RemoveAurasDueToSpell(SPELL_DRAINED_OF_POWER);
        events.ScheduleEvent(EVENT_SOLAR_FLARE, 5s, 7s);
        events.ScheduleEvent(EVENT_UNSTABLE_SUN_BEAM, 7s, 12s);
        events.ScheduleEvent(EVENT_FLUX, 5s);
    }

    void KilledUnit(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_ELDER_SLAY);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
        Talk(SAY_ELDER_DEATH);
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        if (!me->HasAura(SPELL_DRAINED_OF_POWER))
            Talk(SAY_ELDER_AGGRO);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_UNSTABLE_SUN_BEAM:
                    me->CastSpell(me, SPELL_UNSTABLE_SUN_BEAM_SUMMON, true);
                    events.ScheduleEvent(EVENT_UNSTABLE_SUN_BEAM, 10s, 15s);
                    break;
                case EVENT_SOLAR_FLARE:
                {
                    uint8 stackAmount = 0;
                    if (Aura const* aura = me->GetAura(SPELL_FLUX_AURA))
                        stackAmount = aura->GetStackAmount();
                    CastSpellExtraArgs args;
                    args.AddSpellMod(SPELLVALUE_MAX_TARGETS, stackAmount);
                    me->CastSpell(me, SPELL_SOLAR_FLARE, args);
                    events.ScheduleEvent(EVENT_SOLAR_FLARE, 5s, 10s);
                    break;
                }
                case EVENT_FLUX:
                    me->RemoveAurasDueToSpell(SPELL_FLUX_AURA);
                    me->AddAura(SPELL_FLUX_AURA, me);
                    if (Aura* Flux = me->GetAura(SPELL_FLUX_AURA))
                        Flux->SetStackAmount(urand(1, 8));
                    events.ScheduleEvent(EVENT_FLUX, 7500ms);
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ELDER_FREYA_KILLED:
                me->RemoveAllAuras();
                me->AttackStop();
                me->CombatStop(true);
                me->DespawnOrUnsummon(10s);
                _JustDied();
                break;
        }
    }
};

struct boss_elder_stonebark : public BossAI
{
    boss_elder_stonebark(Creature* creature) : BossAI(creature, DATA_STONEBARK)
    {
    }

    void Reset() override
    {
        _Reset();
        if (me->HasAura(SPELL_DRAINED_OF_POWER))
            me->RemoveAurasDueToSpell(SPELL_DRAINED_OF_POWER);
        events.ScheduleEvent(EVENT_TREMOR, 10s, 12s);
        events.ScheduleEvent(EVENT_FISTS, 25s, 35s);
        events.ScheduleEvent(EVENT_BARK, 37500ms, 40s);
    }

    void KilledUnit(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_ELDER_SLAY);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
        Talk(SAY_ELDER_DEATH);
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        if (!me->HasAura(SPELL_DRAINED_OF_POWER))
            Talk(SAY_ELDER_AGGRO);
    }

    void DamageTaken(Unit* who, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (!who || who == me)
            return;

        ///HACK: should be handled by proc
        if (me->HasAura(SPELL_PETRIFIED_BARK))
        {
            CastSpellExtraArgs args(TRIGGERED_FULL_MASK);
            args.AddSpellBP0(damage);
            who->CastSpell(who, SPELL_PETRIFIED_BARK_DMG, args);
            damage = 0;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BARK:
                    DoCast(me, SPELL_PETRIFIED_BARK);
                    events.ScheduleEvent(EVENT_BARK, 30s, 50s);
                    break;
                case EVENT_FISTS:
                    DoCastVictim(SPELL_FISTS_OF_STONE);
                    events.ScheduleEvent(EVENT_FISTS, 20s, 30s);
                    break;
                case EVENT_TREMOR:
                    if (!me->HasAura(SPELL_FISTS_OF_STONE))
                        DoCastVictim(SPELL_GROUND_TREMOR);
                    events.ScheduleEvent(EVENT_TREMOR, 10s, 20s);
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ELDER_FREYA_KILLED:
                me->RemoveAllAuras();
                me->AttackStop();
                me->CombatStop(true);
                me->DespawnOrUnsummon(10s);
                _JustDied();
                break;
        }
    }
};

struct boss_elder_ironbranch : public BossAI
{
    boss_elder_ironbranch(Creature* creature) : BossAI(creature, DATA_IRONBRANCH)
    {
    }

    void Reset() override
    {
        _Reset();
        if (me->HasAura(SPELL_DRAINED_OF_POWER))
            me->RemoveAurasDueToSpell(SPELL_DRAINED_OF_POWER);
        events.ScheduleEvent(EVENT_IMPALE, 18s, 22s);
        events.ScheduleEvent(EVENT_IRON_ROOTS, 12s, 17s);
        events.ScheduleEvent(EVENT_THORN_SWARM, 7500ms, 12500ms);
    }

    void KilledUnit(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
            Talk(SAY_ELDER_SLAY);
    }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
        Talk(SAY_ELDER_DEATH);
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        if (!me->HasAura(SPELL_DRAINED_OF_POWER))
            Talk(SAY_ELDER_AGGRO);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() || me->HasAura(SPELL_DRAINED_OF_POWER))
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_IMPALE:
                    DoCastVictim(SPELL_IMPALE);
                    events.ScheduleEvent(EVENT_IMPALE, 15s, 25s);
                    break;
                case EVENT_IRON_ROOTS:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true, true, -SPELL_ROOTS_IRONBRANCH))
                        target->CastSpell(target, SPELL_ROOTS_IRONBRANCH, true);
                    events.ScheduleEvent(EVENT_IRON_ROOTS, 10s, 20s);
                    break;
                case EVENT_THORN_SWARM:
                    DoCastVictim(SPELL_THORN_SWARM);
                    events.ScheduleEvent(EVENT_THORN_SWARM, 8s, 13s);
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_ELDER_FREYA_KILLED:
                me->RemoveAllAuras();
                me->AttackStop();
                me->CombatStop(true);
                me->DespawnOrUnsummon(10s);
                _JustDied();
                break;
        }
    }
};

struct npc_detonating_lasher : public ScriptedAI
{
    npc_detonating_lasher(Creature* creature) : ScriptedAI(creature)
    {
        Initialize();
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
    }

    void Initialize()
    {
        lashTimer = 5000;
        changeTargetTimer = 7500;
    }

    void Reset() override
    {
        Initialize();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (lashTimer <= diff)
        {
            DoCast(SPELL_FLAME_LASH);
            lashTimer = urand(5000, 10000);
        }
        else
            lashTimer -= diff;

        if (changeTargetTimer <= diff)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 100.0f, true))
            {
                AddThreat(target, GetThreat(me->GetVictim()) * 1.2f);
                AttackStart(target);
            }
            changeTargetTimer = urand(5000, 10000);
        }
        else
            changeTargetTimer -= diff;

        DoMeleeAttackIfReady();
    }

private:
    uint32 lashTimer;
    uint32 changeTargetTimer;
};

struct npc_ancient_water_spirit : public ScriptedAI
{
    npc_ancient_water_spirit(Creature* creature) : ScriptedAI(creature)
    {
        Initialize();
    }

    void Initialize()
    {
        tidalWaveTimer = 10000;
    }

    void Reset() override
    {
        Initialize();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (tidalWaveTimer <= diff)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
            {
                DoCast(target, SPELL_TIDAL_WAVE);
                DoCast(target, SPELL_TIDAL_WAVE_EFFECT, true);
            }
            tidalWaveTimer = urand(12000, 25000);
        }
        else
            tidalWaveTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (InstanceScript* instance = me->GetInstanceScript())
            if (Creature* freya = instance->GetCreature(DATA_FREYA))
                freya->AI()->SetGUID(me->GetGUID(), LASHER_TYPE_WATER_SPIRIT);
    }

private:
    uint32 tidalWaveTimer;
};

struct npc_storm_lasher : public ScriptedAI
{
    npc_storm_lasher(Creature* creature) : ScriptedAI(creature)
    {
        Initialize();
    }

    void Initialize()
    {
        lightningLashTimer = 10000;
        stormboltTimer = 5000;
    }

    void Reset() override
    {
        Initialize();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (lightningLashTimer <= diff)
        {
            DoCast(SPELL_LIGHTNING_LASH);
            lightningLashTimer = urand(7000, 14000);
        }
        else
            lightningLashTimer -= diff;

        if (stormboltTimer <= diff)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true))
                DoCast(target, SPELL_STORMBOLT);
            stormboltTimer = urand(8000, 12000);
        }
        else
            stormboltTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (InstanceScript* instance = me->GetInstanceScript())
            if (Creature* freya = instance->GetCreature(DATA_FREYA))
                freya->AI()->SetGUID(me->GetGUID(), LASHER_TYPE_STORM_LASHER);
    }

private:
    uint32 lightningLashTimer;
    uint32 stormboltTimer;
};

struct npc_snaplasher : public ScriptedAI
{
    npc_snaplasher(Creature* creature) : ScriptedAI(creature) { }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        if (!me->HasAura(SPELL_BARK_AURA))
            DoCast(SPELL_HARDENED_BARK);

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* /*killer*/) override
    {
        if (InstanceScript* instance = me->GetInstanceScript())
            if (Creature* freya = instance->GetCreature(DATA_FREYA))
                freya->AI()->SetGUID(me->GetGUID(), LASHER_TYPE_SNAPLASHER);
    }
};

struct npc_ancient_conservator : public ScriptedAI
{
    npc_ancient_conservator(Creature* creature) : ScriptedAI(creature)
    {
        Initialize();
    }

    void Initialize()
    {
        natureFuryTimer = 7500;
    }

    void Reset() override
    {
        Initialize();
        DoCastSelf(SPELL_SUMMON_PERIODIC, true);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCastSelf(SPELL_CONSERVATOR_GRIP, true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (natureFuryTimer <= diff)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 100.0f, true, true, -SPELL_NATURE_FURY))
                DoCast(target, SPELL_NATURE_FURY);
            me->AddAura(SPELL_CONSERVATOR_GRIP, me);
            natureFuryTimer = 5000;
        }
        else
            natureFuryTimer -= diff;

        DoMeleeAttackIfReady();
    }

private:
    uint32 natureFuryTimer;
};

struct npc_sun_beam : public ScriptedAI
{
    npc_sun_beam(Creature* creature) : ScriptedAI(creature) { }

    void JustAppeared() override
    {
        DoCastAOE(SPELL_FREYA_UNSTABLE_ENERGY_VISUAL, true);
        DoCast(SPELL_FREYA_UNSTABLE_ENERGY);
    }
};

struct npc_healthy_spore : public ScriptedAI
{
    npc_healthy_spore(Creature* creature) : ScriptedAI(creature)
    {
        lifeTimer = urand(22000, 30000);
    }

    void JustAppeared() override
    {
        DoCast(me, SPELL_HEALTHY_SPORE_VISUAL);
        DoCast(me, SPELL_POTENT_PHEROMONES);
        DoCast(me, SPELL_GROW);
    }

    void UpdateAI(uint32 diff) override
    {
        if (lifeTimer <= diff)
        {
            me->RemoveAurasDueToSpell(SPELL_GROW);
            me->DespawnOrUnsummon(2200ms);
            lifeTimer = urand(22000, 30000);
        }
        else
            lifeTimer -= diff;
    }

private:
    uint32 lifeTimer;
};

struct npc_eonars_gift : public ScriptedAI
{
    npc_eonars_gift(Creature* creature) : ScriptedAI(creature)
    {
        SetCombatMovement(false);
        me->SetReactState(REACT_PASSIVE);
        lifeBindersGiftTimer = 12000;
    }

    void JustAppeared() override
    {
        DoCast(me, SPELL_GROW);
        DoCast(me, SPELL_PHEROMONES, true);
        DoCast(me, SPELL_EONAR_VISUAL, true);
    }

    void UpdateAI(uint32 diff) override
    {
        if (lifeBindersGiftTimer <= diff)
        {
            me->RemoveAurasDueToSpell(SPELL_GROW);
            DoCast(SPELL_LIFEBINDERS_GIFT);
            me->DespawnOrUnsummon(2500ms);
            lifeBindersGiftTimer = 12000;
        }
        else
            lifeBindersGiftTimer -= diff;
    }

private:
    uint32 lifeBindersGiftTimer;
};

struct npc_nature_bomb : public ScriptedAI
{
    npc_nature_bomb(Creature* creature) : ScriptedAI(creature)
    {
        bombTimer = urand(8000, 10000);
    }

    void JustAppeared() override
    {
        DoCast(SPELL_OBJECT_BOMB);
    }

    void UpdateAI(uint32 diff) override
    {
        if (bombTimer <= diff)
        {
            DoCast(me, SPELL_NATURE_BOMB);
            if (GameObject* go = me->FindNearestGameObject(OBJECT_NATURE_BOMB, 5.0f))
                me->RemoveGameObject(go, true);
            me->DespawnOrUnsummon();
        }
        else
            bombTimer -= diff;
    }

private:
    uint32 bombTimer;
};

struct npc_unstable_sun_beam : public ScriptedAI
{
    npc_unstable_sun_beam(Creature* creature) : ScriptedAI(creature)
    {
        despawnTimer = urand(7000, 12000);
    }

    void JustAppeared() override
    {
        DoCast(me, SPELL_PHOTOSYNTHESIS);
        DoCast(me, SPELL_UNSTABLE_SUN_BEAM);
    }

    void UpdateAI(uint32 diff) override
    {
        if (despawnTimer <= diff)
        {
            DoCastAOE(SPELL_UNSTABLE_ENERGY, true);
            me->DespawnOrUnsummon();
        }
        else
            despawnTimer -= diff;
    }

    void SpellHitTarget(WorldObject* target, SpellInfo const* spellInfo) override
    {
        Unit* unitTarget = target->ToUnit();
        if (!unitTarget)
            return;

        if (spellInfo->Id == SPELL_UNSTABLE_ENERGY)
        {
            unitTarget->RemoveAurasDueToSpell(SPELL_UNSTABLE_SUN_BEAM);
            unitTarget->RemoveAurasDueToSpell(SPELL_UNSTABLE_SUN_BEAM_TRIGGERED);
        }
    }

private:
    uint32 despawnTimer;
};

// 62521 - Attuned to Nature 25 Dose Reduction
// 62524 - Attuned to Nature 2 Dose Reduction
// 62525 - Attuned to Nature 10 Dose Reduction
class spell_freya_attuned_to_nature_dose_reduction : public SpellScript
{
    PrepareSpellScript(spell_freya_attuned_to_nature_dose_reduction);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();

        switch (GetSpellInfo()->Id)
        {
            case SPELL_ATTUNED_TO_NATURE_2_DOSE_REDUCTION:
                if (target->HasAura(GetEffectValue()))
                    for (uint8 n = 0; n < 2; ++n)
                        target->RemoveAuraFromStack(GetEffectValue());
                break;
            case SPELL_ATTUNED_TO_NATURE_10_DOSE_REDUCTION:
                if (target->HasAura(GetEffectValue()))
                    for (uint8 n = 0; n < 10; ++n)
                        target->RemoveAuraFromStack(GetEffectValue());
                break;
            case SPELL_ATTUNED_TO_NATURE_25_DOSE_REDUCTION:
                if (target->HasAura(GetEffectValue()))
                    for (uint8 n = 0; n < 25; ++n)
                        target->RemoveAuraFromStack(GetEffectValue());
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_freya_attuned_to_nature_dose_reduction::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 65158 - Strengthened Iron Roots Summon Effect
// 65160 - Iron Roots Summon Effect
class spell_freya_iron_roots : public SpellScript
{
    PrepareSpellScript(spell_freya_iron_roots);

    void HandleSummon(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        uint32 const entry = uint32(GetEffectInfo().MiscValue);

        Position const pos = GetCaster()->GetPosition();
        // Not good at all, but this prevents having roots in a different position then player
        if (Creature const* Roots = GetCaster()->SummonCreature(entry, pos))
            GetCaster()->NearTeleportTo(Roots->GetPositionX(), Roots->GetPositionY(), Roots->GetPositionZ(), GetCaster()->GetOrientation());
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_freya_iron_roots::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
    }
};

class achievement_getting_back_to_nature : public AchievementCriteriaScript
{
    public:
        achievement_getting_back_to_nature() : AchievementCriteriaScript("achievement_getting_back_to_nature") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_GETTING_BACK_TO_NATURE) >= 25;
        }
};

class achievement_knock_on_wood : public AchievementCriteriaScript
{
    public:
        achievement_knock_on_wood() : AchievementCriteriaScript("achievement_knock_on_wood") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_KNOCK_ON_WOOD) >= 1;
        }
};

class achievement_knock_knock_on_wood : public AchievementCriteriaScript
{
    public:
        achievement_knock_knock_on_wood() : AchievementCriteriaScript("achievement_knock_knock_on_wood") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_KNOCK_ON_WOOD) >= 2;
        }
};

class achievement_knock_knock_knock_on_wood : public AchievementCriteriaScript
{
    public:
        achievement_knock_knock_knock_on_wood() : AchievementCriteriaScript("achievement_knock_knock_knock_on_wood") { }

        bool OnCheck(Player* /*player*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_KNOCK_ON_WOOD) == 3;
        }
};

void AddSC_boss_freya()
{
    RegisterUlduarCreatureAI(boss_freya);
    RegisterUlduarCreatureAI(boss_elder_brightleaf);
    RegisterUlduarCreatureAI(boss_elder_ironbranch);
    RegisterUlduarCreatureAI(boss_elder_stonebark);
    RegisterUlduarCreatureAI(npc_ancient_conservator);
    RegisterUlduarCreatureAI(npc_snaplasher);
    RegisterUlduarCreatureAI(npc_storm_lasher);
    RegisterUlduarCreatureAI(npc_ancient_water_spirit);
    RegisterUlduarCreatureAI(npc_detonating_lasher);
    RegisterUlduarCreatureAI(npc_sun_beam);
    RegisterUlduarCreatureAI(npc_nature_bomb);
    RegisterUlduarCreatureAI(npc_eonars_gift);
    RegisterUlduarCreatureAI(npc_healthy_spore);
    RegisterUlduarCreatureAI(npc_unstable_sun_beam);
    RegisterUlduarCreatureAI(npc_iron_roots);
    RegisterSpellScript(spell_freya_attuned_to_nature_dose_reduction);
    RegisterSpellScript(spell_freya_iron_roots);
    new achievement_getting_back_to_nature();
    new achievement_knock_on_wood();
    new achievement_knock_knock_on_wood();
    new achievement_knock_knock_knock_on_wood();
}
