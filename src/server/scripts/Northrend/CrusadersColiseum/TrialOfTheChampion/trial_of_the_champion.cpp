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

#include "trial_of_the_champion.h"
#include "Containers.h"
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellScript.h"

enum TrialOfTheChampionHeraldMisc
{
    SPELL_SPECTATOR_RANDOM_TARGET = 66810,

    SAY_CROWD_CHEER = 0,

    GOSSIP_MENU_HERALD = 10614,
    GOSSIP_TEXT_NOT_MOUNTED_ALLIANCE = 14757,
    GOSSIP_TEXT_NOT_MOUNTED_HORDE = 15043,
    GOSSIP_TEXT_FIRST_CHALLENGE = 14688,
    GOSSIP_TEXT_NEXT_CHALLENGE = 14737,
    GOSSIP_TEXT_FINAL_CHALLENGE = 14738
};

struct npc_announcer_toc5 : public ScriptedAI
{
    npc_announcer_toc5(Creature* creature) : ScriptedAI(creature), _instance(creature->GetInstanceScript()) { }

    void JustAppeared() override
    {
        ScriptedAI::JustAppeared();

        me->SetReactState(REACT_PASSIVE);

        me->SetWalk(true);

        if (_instance->GetBossState(DATA_BLACK_KNIGHT) == DONE)
        {
            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
            me->NearTeleportTo(721.252f, 597.551f, 411.649f, me->GetOrientation());
            DoCastSelf(SPELL_HERALD_FEIGN_DEATH, true);
        }
        else if (_instance->GetBossState(DATA_ARGENT_CHALLENGE) == DONE)
            me->GetMotionMaster()->MovePoint(0, { .Destination = HeraldArgentSpot, .GeneratePath = true, .FinalOrientation = HeraldArgentSpot.GetOrientation(), .Velocity = HeraldVelocity });
        else if (_instance->GetBossState(DATA_GRAND_CHAMPIONS) == DONE)
            me->GetMotionMaster()->MovePoint(0, { .Destination = HeraldCenterSpot, .GeneratePath = true, .FinalOrientation = HeraldCenterSpot.GetOrientation(), .Velocity = HeraldVelocity });
    }

    bool OnGossipHello(Player* player) override
    {
        InitGossipMenuFor(player, GOSSIP_MENU_HERALD);

        bool const paused = _instance->GetData(DATA_STAGE_PAUSED) != 0;
        EncounterState const championsState = _instance->GetBossState(DATA_GRAND_CHAMPIONS);
        EncounterState const argentState = _instance->GetBossState(DATA_ARGENT_CHALLENGE);

        uint32 textId = 0;
        if (championsState == NOT_STARTED || (championsState == SPECIAL && paused))
        {
            if (player->GetVehicle())
            {
                textId = GOSSIP_TEXT_FIRST_CHALLENGE;
                AddGossipItemFor(player, GOSSIP_MENU_HERALD, GOSSIP_OPTION_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + uint32(GOSSIP_OPTION_START));
                if (championsState == NOT_STARTED)
                    AddGossipItemFor(player, GOSSIP_MENU_HERALD, GOSSIP_OPTION_START_SKIP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + uint32(GOSSIP_OPTION_START_SKIP));
            }
            else
                textId = me->GetEntry() == NPC_ARELAS_BRIGHTSTAR ? GOSSIP_TEXT_NOT_MOUNTED_ALLIANCE : GOSSIP_TEXT_NOT_MOUNTED_HORDE;
        }
        else if (championsState == DONE && argentState == NOT_STARTED)
        {
            textId = GOSSIP_TEXT_NEXT_CHALLENGE;
            AddGossipItemFor(player, GOSSIP_MENU_HERALD, GOSSIP_OPTION_NEXT_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + uint32(GOSSIP_OPTION_NEXT_CHALLENGE));
        }
        else if (argentState == DONE && _instance->GetBossState(DATA_BLACK_KNIGHT) == NOT_STARTED)
        {
            textId = GOSSIP_TEXT_FINAL_CHALLENGE;
            AddGossipItemFor(player, GOSSIP_MENU_HERALD, GOSSIP_OPTION_FINAL_CHALLENGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + uint32(GOSSIP_OPTION_FINAL_CHALLENGE));
        }

        if (!textId)
            return true;

        SendGossipMenuFor(player, textId, me);
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const option = player->PlayerTalkClass->GetGossipOptionAction(gossipListId) - GOSSIP_ACTION_INFO_DEF;
        CloseGossipMenuFor(player);
        _instance->SetData(DATA_HERALD_GOSSIP, option);
        return true;
    }

    void UpdateAI(uint32 /*diff*/) override { }

private:
    InstanceScript* _instance;
};

// 66654, 66671, 66673 - Argent Dungeon - Summon Champion
class spell_toc5_summon_champion : public SpellScript
{
    PrepareSpellScript(spell_toc5_summon_champion);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        InstanceScript* instance = GetCaster()->GetInstanceScript();
        if (!instance)
            return;

        switch (GetSpellInfo()->Id)
        {
            case SPELL_SUMMON_CHAMPION_1:
                instance->SetData(DATA_SUMMON_CHAMPION, 0);
                break;
            case SPELL_SUMMON_CHAMPION_2:
                instance->SetData(DATA_SUMMON_CHAMPION, 1);
                break;
            case SPELL_SUMMON_CHAMPION_3:
                instance->SetData(DATA_SUMMON_CHAMPION, 2);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_summon_champion::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 67295 - Argent Dungeon - Summon Champion Retainer Wave
class spell_toc5_summon_champion_retainer_wave : public SpellScript
{
    PrepareSpellScript(spell_toc5_summon_champion_retainer_wave);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            instance->SetData(DATA_RETAINER_WAVE, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_summon_champion_retainer_wave::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 67396 - Argent Dungeon - Summon Boss 4
class spell_toc5_summon_boss_4 : public SpellScript
{
    PrepareSpellScript(spell_toc5_summon_boss_4);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            instance->SetData(DATA_SUMMON_ARGENT_CHAMPION, 0);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_summon_boss_4::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66810 - Argent Raid - Spectator - Master - FX - Bunny - Random Target, Spell
// 67551 - Argent Raid - Spectator - Master - FX - Salute (Random Player)
class spell_toc5_spectator_random_target : public SpellScript
{
    PrepareSpellScript(spell_toc5_spectator_random_target);

    bool Validate(SpellInfo const* spellInfo) override
    {
        return ValidateSpellInfo({ uint32(spellInfo->GetEffect(EFFECT_0).CalcValue()) });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        Trinity::Containers::RandomResize(targets, 1);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        uint32 const spellId = uint32(GetEffectValue());
        ObjectGuid const targetGUID = GetHitUnit()->GetGUID();
        caster->m_Events.AddEventAtOffset([caster, targetGUID, spellId]
        {
            if (Unit* target = ObjectAccessor::GetUnit(*caster, targetGUID))
                caster->CastSpell(target, spellId, true);
        }, GetSpellInfo()->Id == SPELL_SPECTATOR_RANDOM_TARGET ? randtime(400ms, 800ms) : 400ms);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_toc5_spectator_random_target::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_toc5_spectator_random_target::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66461 - Argent Raid - Spectator - Master - FX - Random All
class spell_toc5_spectator_master_fx_random : public SpellScript
{
    PrepareSpellScript(spell_toc5_spectator_master_fx_random);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MASTER_FX_CHEER, SPELL_MASTER_FX_SALUTE, SPELL_MASTER_FX_RUDE, SPELL_MASTER_FX_CHANT });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        ObjectGuid const targetGUID = GetHitUnit()->GetGUID();
        caster->m_Events.AddEventAtOffset([caster, targetGUID]
        {
            if (Unit* target = ObjectAccessor::GetUnit(*caster, targetGUID))
                caster->CastSpell(target, RAND(SPELL_MASTER_FX_CHEER, SPELL_MASTER_FX_SALUTE, SPELL_MASTER_FX_RUDE, SPELL_MASTER_FX_CHANT), true);
        }, 800ms);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_spectator_master_fx_random::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66410 - Argent Raid - Spectator - Master - FX - Cheer
// 66417 - Argent Raid - Spectator - Master - FX - Salute
// 66420 - Argent Raid - Spectator - Master - FX - Rude
// 66423 - Argent Raid - Spectator - Master - FX - Chant
class spell_toc5_spectator_master_fx : public SpellScript
{
    PrepareSpellScript(spell_toc5_spectator_master_fx);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetHitPlayer();
        if (!player)
            return;

        bool const alliance = player->GetTeamId() == TEAM_ALLIANCE;
        uint32 spellId = 0;
        switch (GetSpellInfo()->Id)
        {
            case SPELL_MASTER_FX_CHEER:
                spellId = alliance ? SPELL_FORCE_RESPONSE_CHEER_ALLIANCE : SPELL_FORCE_RESPONSE_CHEER_HORDE;
                break;
            case SPELL_MASTER_FX_SALUTE:
                spellId = alliance ? SPELL_FORCE_RESPONSE_SALUTE_ALLIANCE : SPELL_FORCE_RESPONSE_SALUTE_HORDE;
                break;
            case SPELL_MASTER_FX_RUDE:
                spellId = alliance ? SPELL_FORCE_RESPONSE_RUDE_HORDE : SPELL_FORCE_RESPONSE_RUDE_ALLIANCE;
                break;
            case SPELL_MASTER_FX_CHANT:
                switch (player->GetRace())
                {
                    case RACE_HUMAN:
                        spellId = SPELL_FORCE_CHANT_HUMAN;
                        break;
                    case RACE_ORC:
                        spellId = SPELL_FORCE_CHANT_ORC;
                        break;
                    case RACE_DWARF:
                        spellId = SPELL_FORCE_CHANT_DWARF;
                        break;
                    case RACE_NIGHTELF:
                        spellId = SPELL_FORCE_CHANT_NIGHT_ELF;
                        break;
                    case RACE_UNDEAD_PLAYER:
                        spellId = SPELL_FORCE_CHANT_UNDEAD;
                        break;
                    case RACE_TAUREN:
                        spellId = SPELL_FORCE_CHANT_TAUREN;
                        break;
                    case RACE_GNOME:
                        spellId = SPELL_FORCE_CHANT_GNOME;
                        break;
                    case RACE_TROLL:
                        spellId = SPELL_FORCE_CHANT_TROLL;
                        break;
                    case RACE_BLOODELF:
                        spellId = SPELL_FORCE_CHANT_BLOOD_ELF;
                        break;
                    case RACE_DRAENEI:
                        spellId = SPELL_FORCE_CHANT_DRAENEI;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }

        if (!spellId)
            return;

        player->m_Events.AddEventAtOffset([player, spellId]
        {
            player->CastSpell(player, spellId, true);
        }, 400ms);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_spectator_master_fx::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66382, 66383 - [DND] Argent Raid - Spectator - Force Response - Salute
// 66384, 66385 - [DND] Argent Raid - Spectator - Force Response - Cheer
// 66386, 66387 - [DND] Argent Raid - Spectator - Force Response - Rude
class spell_toc5_spectator_force_response : public SpellScript
{
    PrepareSpellScript(spell_toc5_spectator_force_response);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Creature* spectator = GetHitCreature();
        if (!spectator)
            return;

        spectator->SetFacingToObject(GetCaster());

        switch (GetSpellInfo()->Id)
        {
            case SPELL_FORCE_RESPONSE_SALUTE_ALLIANCE:
            case SPELL_FORCE_RESPONSE_SALUTE_HORDE:
                spectator->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                break;
            case SPELL_FORCE_RESPONSE_CHEER_ALLIANCE:
            case SPELL_FORCE_RESPONSE_CHEER_HORDE:
                spectator->HandleEmoteCommand(EMOTE_ONESHOT_CHEER_NO_SHEATHE);
                if (spectator->GetEntry() == NPC_CROWD_ALLIANCE || spectator->GetEntry() == NPC_CROWD_HORDE)
                    spectator->AI()->Talk(SAY_CROWD_CHEER, GetCaster());
                break;
            case SPELL_FORCE_RESPONSE_RUDE_ALLIANCE:
            case SPELL_FORCE_RESPONSE_RUDE_HORDE:
                spectator->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_spectator_force_response::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66345, 66348, 66349, 66350, 66352, 66353, 66354, 66355, 66356, 66357 - Spectator - Force Chant
class spell_toc5_spectator_force_chant : public SpellScript
{
    PrepareSpellScript(spell_toc5_spectator_force_chant);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Creature* spectator = GetHitCreature();
        if (!spectator)
            return;

        switch (spectator->GetEntry())
        {
            case NPC_CROWD_HUMAN:
            case NPC_CROWD_ORC:
            case NPC_CROWD_TROLL:
            case NPC_CROWD_TAUREN:
            case NPC_CROWD_BLOOD_ELF:
            case NPC_CROWD_UNDEAD:
            case NPC_CROWD_DWARF:
            case NPC_CROWD_DRAENEI:
            case NPC_CROWD_NIGHT_ELF:
            case NPC_CROWD_GNOME:
                spectator->AI()->Talk(SAY_CROWD_CHEER, GetCaster());
                return;
            default:
                break;
        }

        Emote emote = EMOTE_ONESHOT_NONE;
        Milliseconds duration = 0ms;
        switch (GetSpellInfo()->Id)
        {
            case SPELL_FORCE_CHANT_DWARF:
                spectator->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
                return;
            case SPELL_FORCE_CHANT_TAUREN:
                spectator->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);
                return;
            case SPELL_FORCE_CHANT_TROLL:
                emote = EMOTE_STATE_DANCE;
                duration = 13s;
                break;
            case SPELL_FORCE_CHANT_GNOME:
                emote = EMOTE_STATE_WORK;
                duration = 15s;
                break;
            case SPELL_FORCE_CHANT_DRAENEI:
                emote = EMOTE_STATE_APPLAUD;
                duration = 5s;
                break;
            case SPELL_FORCE_CHANT_HUMAN:
                emote = EMOTE_STATE_DANCESPECIAL;
                duration = 15s;
                break;
            case SPELL_FORCE_CHANT_NIGHT_ELF:
                emote = EMOTE_STATE_APPLAUD;
                duration = 15s;
                break;
            case SPELL_FORCE_CHANT_BLOOD_ELF:
                emote = EMOTE_STATE_WORK;
                duration = 6s;
                break;
            case SPELL_FORCE_CHANT_ORC:
                emote = EMOTE_STATE_SPELL_CHANNEL_OMNI;
                duration = 6s;
                break;
            case SPELL_FORCE_CHANT_UNDEAD:
                emote = EMOTE_STATE_WORK;
                duration = 5s;
                break;
            default:
                return;
        }

        spectator->SetEmoteState(emote);
        spectator->m_Events.AddEventAtOffset([spectator]()
        {
            spectator->SetEmoteState(EMOTE_ONESHOT_NONE);
        }, duration);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_toc5_spectator_force_chant::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

void AddSC_trial_of_the_champion()
{
    RegisterTrialOfTheChampionCreatureAI(npc_announcer_toc5);
    RegisterSpellScript(spell_toc5_summon_champion);
    RegisterSpellScript(spell_toc5_summon_champion_retainer_wave);
    RegisterSpellScript(spell_toc5_summon_boss_4);
    RegisterSpellScript(spell_toc5_spectator_random_target);
    RegisterSpellScript(spell_toc5_spectator_master_fx_random);
    RegisterSpellScript(spell_toc5_spectator_master_fx);
    RegisterSpellScript(spell_toc5_spectator_force_response);
    RegisterSpellScript(spell_toc5_spectator_force_chant);
}
