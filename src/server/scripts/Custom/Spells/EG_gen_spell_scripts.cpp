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
#include "Cell.h"
#include "CellImpl.h"
#include "Containers.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "InstanceScript.h"
#include "MotionMaster.h"
#include "MovementTypedefs.h"
#include "ObjectAccessor.h"
#include "Optional.h"
#include "PathGenerator.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Unit.h"
#include "Vehicle.h"
#include <algorithm>
#include <array>
#include <list>
#include <vector>

// 45775 - Cosmetic - Divine Shield Blue
class EG_spell_cosmetic___divine_shield_blue : public AuraScript
{
    PrepareAuraScript(EG_spell_cosmetic___divine_shield_blue);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* target = GetTarget();
        target->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(EG_spell_cosmetic___divine_shield_blue::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectRemove += AuraEffectRemoveFn(EG_spell_cosmetic___divine_shield_blue::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }
};

enum DestroyDeathforgedInfernal
{
    SPELL_RANDOM_ROCKET_MISSILE = 38054,
    SPELL_FRS_QUEST_CREDIT = 38022,
    SPELL_FEL_REAVER_CONTROLLER_1 = 38002,
    SPELL_FEL_REAVER_CONTROLLER_1_1 = 38003,
    SPELL_FEL_REAVER_CONTROLLER_2 = 38120,
    SPELL_FEL_REAVER_CONTROLLER_2_1 = 38121,
    SPELL_FEL_REAVER_CONTROLLER_3 = 38122,
    SPELL_FEL_REAVER_CONTROLLER_3_1 = 38123,
    SPELL_FEL_REAVER_CONTROLLER_4 = 38125,
    SPELL_FEL_REAVER_CONTROLLER_4_1 = 38126,
    SPELL_FEL_REAVER_CONTROLLER_5 = 38127,
    SPELL_FEL_REAVER_CONTROLLER_5_1 = 38128,
    SPELL_FEL_REAVER_CONTROLLER_6 = 38129,
    SPELL_FEL_REAVER_CONTROLLER_6_1 = 38130
};

// 38055 - Destroy Deathforged Infernal
class EG_spell_destroy_deathforged_infernal : public SpellScript
{
    PrepareSpellScript(EG_spell_destroy_deathforged_infernal);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_RANDOM_ROCKET_MISSILE });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(nullptr, SPELL_RANDOM_ROCKET_MISSILE, CastSpellExtraArgs(TriggerCastFlags(TRIGGERED_IGNORE_GCD | TRIGGERED_IGNORE_SPELL_AND_CATEGORY_CD | TRIGGERED_IGNORE_CAST_IN_PROGRESS)));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_destroy_deathforged_infernal::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 38020 - Fel Reaver Sentinel Tag
class EG_spell_fel_reaver_sentinel_tag : public SpellScript
{
    PrepareSpellScript(EG_spell_fel_reaver_sentinel_tag);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_FRS_QUEST_CREDIT });
    }

    void HandleSpell(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        target->CastSpell(nullptr, SPELL_FRS_QUEST_CREDIT);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_fel_reaver_sentinel_tag::HandleSpell, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 38002 - Fel Reaver Controller
// 38120 - Fel Reaver Controller
// 38122 - Fel Reaver Controller
// 38125 - Fel Reaver Controller
// 38127 - Fel Reaver Controller
// 38129 - Fel Reaver Controller
class EG_spell_fel_reaver_controller : public SpellScript
{
    PrepareSpellScript(EG_spell_fel_reaver_controller);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_FEL_REAVER_CONTROLLER_1_1,
            SPELL_FEL_REAVER_CONTROLLER_2_1,
            SPELL_FEL_REAVER_CONTROLLER_3_1,
            SPELL_FEL_REAVER_CONTROLLER_4_1,
            SPELL_FEL_REAVER_CONTROLLER_5_1,
            SPELL_FEL_REAVER_CONTROLLER_6_1
        });
    }

    void HandleSpell(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        uint32 spelld = GetSpellInfo()->Id + 1;
        target->CastSpell(nullptr, spelld);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_fel_reaver_controller::HandleSpell, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

enum CharmChannel
{
    SPELL_CHARM_DRAKURU_SERVANT = 52390,
    NPC_SERVANT_OF_DRAKURU = 28802
};

// 52389 - Charm Channel
class EG_spell_charm_channel : public AuraScript
{
    PrepareAuraScript(EG_spell_charm_channel);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();
        if (Unit* caster = GetCaster())
            caster->CastSpell(GetOwner(), SPELL_CHARM_DRAKURU_SERVANT, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_charm_channel::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 52390 - Charm Drakuru Servant
class EG_spell_charm_drakuru_servant : public AuraScript
{
    PrepareAuraScript(EG_spell_charm_drakuru_servant);

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetOwner()->GetEntry() == NPC_SERVANT_OF_DRAKURU)
            PreventDefaultAction();
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(EG_spell_charm_drakuru_servant::OnApply, EFFECT_0, SPELL_AURA_MOD_CHARM, AURA_EFFECT_HANDLE_REAL);
    }
};

enum SpellRitualPrayerBeads
{
    SPELL_HEAL_BARADA = 39322,
    NPC_ANCHORITE_BARADA = 22431
};

// 39371 - Prayer Beads
class EG_spell_prayer_beads : public SpellScript
{
    PrepareSpellScript(EG_spell_prayer_beads);

    void HandleHit()
    {
        Unit* caster = GetCaster();
        Creature* target = GetHitCreature();
        if (!caster || !target)
            return;

        uint32 entry = target->GetEntry();
        if (entry == NPC_ANCHORITE_BARADA)
            caster->CastSpell(target, SPELL_HEAL_BARADA, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(EG_spell_prayer_beads::HandleHit);
    }
};

enum FieryLance
{
    NPC_DRAGONFLAYER_DEFENDER = 24533
};

// 43997 - Fiery Lance
class EG_spell_fiery_lance : public SpellScript
{
    PrepareSpellScript(EG_spell_fiery_lance);

    SpellCastResult CheckRequirement()
    {
        if (Unit* target = GetExplTargetUnit())
            if (target->GetEntry() == NPC_DRAGONFLAYER_DEFENDER)
                return SPELL_CAST_OK;
        return SPELL_FAILED_BAD_TARGETS;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(EG_spell_fiery_lance::CheckRequirement);
    }
};

enum LavaStrike
{
    DATA_GONNA_GO_WHEN_THE_VOLCANO_BLOWS = 6
};

class EG_spell_lava_strike : public SpellScript
{
    PrepareSpellScript(EG_spell_lava_strike);

    void HandleEffect(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();

        if (InstanceScript* instance = target->GetInstanceScript())
            instance->SetGuidData(DATA_GONNA_GO_WHEN_THE_VOLCANO_BLOWS, target->GetGUID());
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_lava_strike::HandleEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 1010 - Curse of Idiocy (Mr. Bigglesworth debuff)
class EG_spell_naxx_bigglesworth_curse : public AuraScript
{
    PrepareAuraScript(EG_spell_naxx_bigglesworth_curse);

    bool Load() override
    {
        return GetUnitOwner() && GetUnitOwner()->GetMapId() == 533;
    }

    void MakePermanent(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        SetMaxDuration(-1);
        SetDuration(-1);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_spell_naxx_bigglesworth_curse::MakePermanent, EFFECT_0, SPELL_AURA_MOD_PERCENT_STAT, AURA_EFFECT_HANDLE_REAL);
    }
};

// 65099 - Deploy Salvage Saws
class EG_spell_deploy_salvage_saws : public SpellScript
{
    PrepareSpellScript(EG_spell_deploy_salvage_saws);

    void SelectRandomTargetDest(SpellDestination& dest)
    {
        Creature* caster = GetCaster() ? GetCaster()->ToCreature() : nullptr;
        if (!caster || !caster->IsAIEnabled())
            return;

        if (Unit* target = caster->AI()->SelectTarget(SelectTargetMethod::Random, 0, 40.0f, true))
            dest.Relocate(*target);
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(EG_spell_deploy_salvage_saws::SelectRandomTargetDest, EFFECT_0, TARGET_DEST_DEST_RADIUS);
    }
};

// 64783 - Displacement Device
class EG_spell_displacement_device : public SpellScript
{
    PrepareSpellScript(EG_spell_displacement_device);

    void RaiseDestination(SpellDestination& dest)
    {
        dest.RelocateOffset({ 0.f, 0.f, 2.f, 0.f });
    }

    void Register() override
    {
        OnDestinationTargetSelect += SpellDestinationTargetSelectFn(EG_spell_displacement_device::RaiseDestination, EFFECT_0, TARGET_DEST_TARGET_ENEMY);
    }
};

enum TwilightTorment
{
    SPELL_TWILIGHT_TORMENT_VESP = 57948,
    SPELL_TWILIGHT_TORMENT_PROC = 57935,
    SPELL_TWILIGHT_TORMENT_ACO = 58853,
    SPELL_TWILIGHT_TORMENT_ACO_PROC = 58835,
    PHASEMASK_TWILIGHT_REALM = 16
};

// 57988 - Twilight Torment
class EG_spell_twilight_torment_damage : public SpellScript
{
    PrepareSpellScript(EG_spell_twilight_torment_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_TWILIGHT_TORMENT_PROC, SPELL_TWILIGHT_TORMENT_ACO_PROC });
    }

    void RemoveTormentSelfBonus()
    {
        Unit* target = GetHitUnit();
        if (!target)
            return;

        float factor = 1.0f;
        if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_TWILIGHT_TORMENT_PROC, EFFECT_0))
            AddPct(factor, aurEff->GetAmount());
        if (AuraEffect const* aurEff = target->GetAuraEffect(SPELL_TWILIGHT_TORMENT_ACO_PROC, EFFECT_0))
            AddPct(factor, aurEff->GetAmount());

        if (factor > 1.0f)
            SetHitDamage(int32(GetHitDamage() / factor));
    }

    void Register() override
    {
        OnHit += SpellHitFn(EG_spell_twilight_torment_damage::RemoveTormentSelfBonus);
    }
};

// 57948 - Twilight Torment (Vesperon carrier)  -> triggered debuff 57935 (normal world)
// 58853 - Twilight Torment (Acolyte carrier)   -> triggered debuff 58835 (twilight realm)
class EG_spell_twilight_torment_carrier : public SpellScript
{
    PrepareSpellScript(EG_spell_twilight_torment_carrier);

    void SelectAllPlayers(std::list<WorldObject*>& targets)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        float radius = GetSpellInfo()->GetEffect(EFFECT_0).CalcRadius(caster);
        targets.clear();

        std::list<Player*> players;
        Trinity::AnyPlayerInPositionRangeCheck check(caster, radius);
        Trinity::PlayerListSearcher<Trinity::AnyPlayerInPositionRangeCheck> searcher(PHASEMASK_ANYWHERE, players, check);
        Cell::VisitWorldObjects(caster, searcher, radius);

        for (Player* player : players)
            if (!player->IsGameMaster())
                targets.push_back(player);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_twilight_torment_carrier::SelectAllPlayers, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

class EG_spell_twilight_torment_phase : public AuraScript
{
    PrepareAuraScript(EG_spell_twilight_torment_phase);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_TWILIGHT_TORMENT_PROC, SPELL_TWILIGHT_TORMENT_ACO_PROC });
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 500;
    }

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        Unit* owner = GetTarget();
        bool isTwilightCarrier = (GetId() == SPELL_TWILIGHT_TORMENT_ACO);
        uint32 triggered = isTwilightCarrier ? SPELL_TWILIGHT_TORMENT_ACO_PROC : SPELL_TWILIGHT_TORMENT_PROC;

        if (owner->InSamePhase(PHASEMASK_TWILIGHT_REALM) == isTwilightCarrier)
        {
            if (!owner->HasAura(triggered))
                if (Unit* caster = GetCaster())
                    caster->AddAura(triggered, owner);
        }
        else
            owner->RemoveAurasDueToSpell(triggered);
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(EG_spell_twilight_torment_phase::CalcPeriodic, EFFECT_0, SPELL_AURA_DUMMY);
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_twilight_torment_phase::PeriodicTick, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 57935 - Twilight Torment debuff (shadow / normal world)
// 58835 - Twilight Torment debuff (fire+shadow / twilight realm)
class EG_spell_twilight_torment_trigger : public AuraScript
{
    PrepareAuraScript(EG_spell_twilight_torment_trigger);

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        isPeriodic = true;
        amplitude = 500;
    }

    void EnforcePhase(AuraEffect const* /*aurEff*/)
    {
        bool isTwilightDebuff = (GetId() == SPELL_TWILIGHT_TORMENT_ACO_PROC);
        if (GetTarget()->InSamePhase(PHASEMASK_TWILIGHT_REALM) != isTwilightDebuff)
            Remove();
    }

    void Register() override
    {
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(EG_spell_twilight_torment_trigger::CalcPeriodic, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_twilight_torment_trigger::EnforcePhase, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
    }
};

enum EnergySap
{
    SPELL_SAPPER_EXPLOSION = 64873
};

// 64740, 64876 - Energy Sap
class EG_spell_energy_sap : public AuraScript
{
    PrepareAuraScript(EG_spell_energy_sap);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SAPPER_EXPLOSION });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_EXPIRE && removeMode != AURA_REMOVE_BY_CANCEL)
            return;

        GetTarget()->CastSpell(nullptr, SPELL_SAPPER_EXPLOSION, CastSpellExtraArgs(aurEff).SetOriginalCaster(GetCasterGUID()));
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(EG_spell_energy_sap::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

enum HealthySporeSummonPeriodic
{
    SPELL_SPORE_SUMMON_NW = 62582,
    SPELL_SPORE_SUMMON_NE = 62591,
    SPELL_SPORE_SUMMON_SE = 62592,
    SPELL_SPORE_SUMMON_SW = 62593
};

// 62566 - Healthy Spore Summon Periodic
class EG_spell_freya_summon_healthy_spore : public AuraScript
{
    PrepareAuraScript(EG_spell_freya_summon_healthy_spore);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SPORE_SUMMON_NW, SPELL_SPORE_SUMMON_NE, SPELL_SPORE_SUMMON_SE, SPELL_SPORE_SUMMON_SW });
    }

    void SummonSpores()
    {
        Unit* target = GetTarget();
        target->CastSpell(target, SPELL_SPORE_SUMMON_NW, true);
        target->CastSpell(target, SPELL_SPORE_SUMMON_NE, true);
        target->CastSpell(target, SPELL_SPORE_SUMMON_SE, true);
        target->CastSpell(target, SPELL_SPORE_SUMMON_SW, true);
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        SummonSpores();
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        SummonSpores();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_spell_freya_summon_healthy_spore::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_freya_summon_healthy_spore::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 64770 - Damaged
class EG_spell_arachnopod_damaged : public AuraScript
{
    PrepareAuraScript(EG_spell_arachnopod_damaged);

    void PeriodicTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();
        if (Unit* owner = GetUnitOwner())
            Unit::Kill(owner, owner, false);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_arachnopod_damaged::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 62221 - Unstable Sun Beam
class EG_spell_brightleaf_unstable_sun_beam_forced : public SpellScript
{
    PrepareSpellScript(EG_spell_brightleaf_unstable_sun_beam_forced);

    void SetDest()
    {
        WorldLocation dest(GetCaster()->GetMapId(), GetCaster()->GetPosition());
        SetExplTargetDest(dest);
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(EG_spell_brightleaf_unstable_sun_beam_forced::SetDest);
    }
};

enum PetrifiedBarkSpells
{
    SPELL_PETRIFIED_BARK_DAMAGE = 62379
};

// 62337, 62933 - Petrified Bark (Elder Stonebark)
class EG_spell_stonebark_petrified_bark : public AuraScript
{
    PrepareAuraScript(EG_spell_stonebark_petrified_bark);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PETRIFIED_BARK_DAMAGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetDamage())
            return;

        Unit* attacker = eventInfo.GetActor();
        if (!attacker)
            return;

        CastSpellExtraArgs args(aurEff, GetTarget()->GetGUID());
        args.AddSpellBP0(int32(damageInfo->GetDamage()));
        attacker->CastSpell(attacker, SPELL_PETRIFIED_BARK_DAMAGE, args);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(EG_spell_stonebark_petrified_bark::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 57731 - Web Grab
class EG_spell_hadronox_web_grab : public SpellScript
{
    PrepareSpellScript(EG_spell_hadronox_web_grab);

    Position GetPullPosition(Unit* caster, Unit* target) const
    {
        Position pullPosition = caster->GetPosition();

        PathGenerator path(caster);
        path.CalculatePath(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
        if (path.GetPathType() & (PATHFIND_SHORTCUT | PATHFIND_NOPATH | PATHFIND_NOT_USING_PATH | PATHFIND_FARFROMPOLY_START))
            return pullPosition;

        float const maxDistSq = caster->GetCombatReach() * caster->GetCombatReach();
        for (G3D::Vector3 const& point : path.GetPath())
        {
            if (caster->GetExactDistSq(point.x, point.y, point.z) > maxDistSq)
                break;

            pullPosition.Relocate(point.x, point.y, point.z);
        }

        return pullPosition;
    }

    void HandlePull(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);

        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        if (!caster || !target)
            return;

        Position const pullPosition = GetPullPosition(caster, target);
        float const dist = target->GetExactDist(pullPosition);
        if (dist < 0.001f)
            return;

        float const speedXY = GetEffectInfo().MiscValue ? GetEffectInfo().MiscValue / 10.0f : 30.0f;
        float const speedZ = (2 * speedXY * speedXY * (pullPosition.GetPositionZ() - target->GetPositionZ()) + Movement::gravity * dist * dist) / (2 * speedXY * dist);
        if (!std::isfinite(speedZ))
            return;

        if (target->GetTypeId() == TYPEID_UNIT)
            target->GetMotionMaster()->MoveJump(pullPosition, speedXY, speedZ);
        else
            target->JumpTo(speedXY, speedZ, true, pullPosition);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_hadronox_web_grab::HandlePull, EFFECT_0, SPELL_EFFECT_PULL_TOWARDS);
    }
};

enum AzureRingCaptainMisc
{
    NPC_AZURE_RING_CAPTAIN = 28236,

    DATA_ARCANE_BEAM_TARGET = 1, // npc_azure_ring_captain::GetGUID in boss_varos.cpp
    DATA_ARCANE_BEAM
};

// 51014 - Summon Arcane Beam
class EG_spell_varos_summon_arcane_beam : public SpellScript
{
    PrepareSpellScript(EG_spell_varos_summon_arcane_beam);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_UNIT;
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        ObjectGuid const targetGuid = GetCaster()->ToCreature()->AI()->GetGUID(DATA_ARCANE_BEAM_TARGET);
        targets.remove_if([targetGuid](WorldObject* target)
        {
            return target->GetGUID() != targetGuid;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_varos_summon_arcane_beam::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
    }
};

// 51022 - Arcane Beam Spawn Trigger
class EG_spell_varos_arcane_beam_spawn_trigger : public SpellScript
{
    PrepareSpellScript(EG_spell_varos_arcane_beam_spawn_trigger);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_UNIT && GetCaster()->ToCreature()->IsSummon();
    }

    void SelectCaptain(WorldObject*& target)
    {
        target = nullptr;
        WorldObject* summoner = GetCaster()->ToCreature()->ToTempSummon()->GetSummoner();
        if (!summoner)
            return;

        std::list<Creature*> captains;
        summoner->GetCreatureListWithOptionsInGrid(captains, 150.0f, { .CreatureId = NPC_AZURE_RING_CAPTAIN, .IsAlive = true });
        for (Creature* captain : captains)
            if (captain->IsAIEnabled() && captain->AI()->GetGUID(DATA_ARCANE_BEAM_TARGET) == summoner->GetGUID())
                target = captain;
    }

    void HandleForceCast(SpellEffIndex /*effIndex*/)
    {
        GetHitCreature()->AI()->SetGUID(GetCaster()->GetGUID(), DATA_ARCANE_BEAM);
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(EG_spell_varos_arcane_beam_spawn_trigger::SelectCaptain, EFFECT_0, TARGET_UNIT_NEARBY_ENTRY);
        OnEffectHitTarget += SpellEffectFn(EG_spell_varos_arcane_beam_spawn_trigger::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
    }
};

// 51024 - Arcane Beam Visual
class EG_spell_varos_arcane_beam_visual : public SpellScript
{
    PrepareSpellScript(EG_spell_varos_arcane_beam_visual);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_UNIT;
    }

    void SelectBeam(WorldObject*& target)
    {
        target = ObjectAccessor::GetCreature(*GetCaster(), GetCaster()->ToCreature()->AI()->GetGUID(DATA_ARCANE_BEAM));
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(EG_spell_varos_arcane_beam_visual::SelectBeam, EFFECT_0, TARGET_UNIT_NEARBY_ENTRY);
    }
};

enum AQ20AttackOrderMisc
{
    SPELL_ATTACK_ORDER_TAUNT = 25473
};

// 25471 - Attack Order
class EG_spell_ruins_of_ahnqiraj_attack_order : public AuraScript
{
    PrepareAuraScript(EG_spell_ruins_of_ahnqiraj_attack_order);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_ATTACK_ORDER_TAUNT });
    }

    void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(nullptr, SPELL_ATTACK_ORDER_TAUNT, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_spell_ruins_of_ahnqiraj_attack_order::AfterApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum NetherstormRingOfFlameMisc
{
    SPELL_RING_OF_FLAME_IMMOLATION = 35995
};

// 35831 - Ring of Flame
class EG_spell_netherstorm_ring_of_flame : public AuraScript
{
    PrepareAuraScript(EG_spell_netherstorm_ring_of_flame);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_RING_OF_FLAME_IMMOLATION });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetAura()->IsRemoved())
            return;

        GetTarget()->CastSpell(nullptr, SPELL_RING_OF_FLAME_IMMOLATION, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(EG_spell_netherstorm_ring_of_flame::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

enum RocketPropelledGoblinGrenade
{
    SPELL_RP_GG_TRIGGER_MISSILE = 49769
};

static bool IsVehicleBase(WorldObject const* object)
{
    Creature const* creature = object->ToCreature();
    return creature && creature->IsVehicle() && !creature->GetVehicle();
}

// 49761 - Rocket-Propelled Goblin Grenade
class EG_spell_wintergrasp_rp_gg : public SpellScript
{
    PrepareSpellScript(EG_spell_wintergrasp_rp_gg);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_RP_GG_TRIGGER_MISSILE });
    }

    void FilterVehicleTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target) { return !IsVehicleBase(target); });
    }

    void SelectTrackedVehicle(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        WorldLocation const* dest = GetExplTargetDest();
        auto nearest = std::ranges::min_element(targets, { }, [dest](WorldObject const* target) { return target->GetExactDistSq(dest); });
        _vehiclePosition = (*nearest)->GetPosition();
    }

    void LaunchMissile(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(_vehiclePosition.value_or(*GetExplTargetDest()), SPELL_RP_GG_TRIGGER_MISSILE, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_wintergrasp_rp_gg::FilterVehicleTargets, EFFECT_1, TARGET_UNIT_CONE_ENEMY_24);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_wintergrasp_rp_gg::SelectTrackedVehicle, EFFECT_1, TARGET_UNIT_CONE_ENEMY_24);
        OnEffectLaunch += SpellEffectFn(EG_spell_wintergrasp_rp_gg::LaunchMissile, EFFECT_0, SPELL_EFFECT_DUMMY);
    }

    Optional<Position> _vehiclePosition;
};

// 49760 - Rocket-Propelled Goblin Grenade
class EG_spell_wintergrasp_rp_gg_impact : public SpellScript
{
    PrepareSpellScript(EG_spell_wintergrasp_rp_gg_impact);

    void FilterKnockBackTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if(IsVehicleBase);
    }

    void FilterDamageTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject const* target) { return !IsVehicleBase(target); });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_wintergrasp_rp_gg_impact::FilterKnockBackTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(EG_spell_wintergrasp_rp_gg_impact::FilterDamageTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

enum InfraGreenBomberSpellMisc
{
    SPELL_WAITING_FOR_BOMBER_ALLIANCE = 59563,
    SPELL_WAITING_FOR_BOMBER_HORDE = 59779,
    SPELL_SUMMON_AIRSHIP_COOLDOWN = 59558,
    SPELL_REQUEST_AIRSHIP_ALLIANCE = 59554,
    SPELL_REQUEST_AIRSHIP_HORDE = 59781,
    SPELL_BOMBER_GRANTED = 61373,
    SPELL_SUMMON_AIRSHIP_ALLIANCE = 59552,
    SPELL_SUMMON_AIRSHIP_HORDE = 59780,
    SPELL_SUMMON_AIRSHIP_LONG_ALLIANCE = 61151,
    SPELL_SUMMON_AIRSHIP_LONG_HORDE = 61152,
    SPELL_FLIGHT_ORDERS = 61281,
    SPELL_DRIVE_BOMBER = 59050,
    SPELL_SYNCH_HEALTH = 59413,
    SPELL_SEND_HATE = 60124,
    SPELL_INFRA_GREEN_SHIELD = 59288,
    SPELL_AIRSHIP_SHIELD_RECENTLY_HIT_SMALL = 60339,
    SPELL_AIRSHIP_SHIELD_RECENTLY_HIT = 59290,
    SPELL_AA_SHOT_TRIGGER_MISSILE = 59623,
    SPELL_ENGINEERING = 59193,
    SPELL_BOMBER_BAY = 59194,
    SPELL_SWITCH_SEATS = 59186,
    SPELL_QUEST_CREDIT_INFANTRY = 60961,
    SPELL_QUEST_CREDIT_TO_PLAYER_INFANTRY = 60965,
    SPELL_QUEST_CREDIT_CAPTAIN = 60972,
    SPELL_QUEST_CREDIT_TO_PLAYER_CAPTAIN = 60973,
    SPELL_SMOLDER = 60935,
    SPELL_BURNING = 61171,
    SPELL_EXTINGUISH_FIRE = 61172,
    SPELL_AIRSHIP_EXPLODES = 60853,
    SPELL_AIRSHIP_EXPLODES_BIG = 60855,
    SPELL_AIRSHIP_EXPLODES_HUGE = 60860,
    SPELL_AIRSHIP_EXPLODES_WITH_FIREBALL = 60861,
    SPELL_BOARD_RESCUE_CRAFT = 60785,
    SPELL_PARACHUTE_BUFF = 44795,
    SPELL_PARACHUTE = 45472,
    SPELL_REPORTED_KILLS = 61127,

    NPC_INFRA_GREEN_FLIGHT_MASTER = 31690,
    NPC_STITCHED_BRUTE = 32179,
    NPC_ENRAGED_FLESHRENDER = 32182,
    NPC_SKELETAL_FOOTSOLDIER = 32183,

    QUEST_AMPED_FOR_REVOLT = 13374,
    QUEST_TOTAL_OHMAGE = 13376,
    QUEST_WATTS_MY_TARGET = 13381,
    QUEST_PUTTING_THE_HERTZ = 13382,

    SAY_BOMBER_READY = 0,

    SEAT_BOMBER_BAY = 0,
    SEAT_TURRET = 1,
    SEAT_ENGINEERING = 2,
    SEAT_BANNER_HANGING = 3,
    SEAT_BANNER_SIDE_LEFT = 4,
    SEAT_BANNER_SIDE_RIGHT = 5,
    SEAT_SHIELD_VISUAL = 6
};

// 59563, 59779 - Waiting for Bomber
class EG_spell_infra_green_bomber_waiting : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_waiting);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_AIRSHIP_COOLDOWN, SPELL_REQUEST_AIRSHIP_ALLIANCE, SPELL_REQUEST_AIRSHIP_HORDE });
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* pilot = GetTarget();
        Creature* flightMaster = pilot->FindNearestCreature(NPC_INFRA_GREEN_FLIGHT_MASTER, 30.0f);
        if (!flightMaster)
        {
            Remove();
            return;
        }

        if (flightMaster->HasAura(SPELL_SUMMON_AIRSHIP_COOLDOWN))
            return;

        pilot->CastSpell(flightMaster, SPELL_SUMMON_AIRSHIP_COOLDOWN, true);
        pilot->CastSpell(flightMaster, GetId() == SPELL_WAITING_FOR_BOMBER_ALLIANCE ? SPELL_REQUEST_AIRSHIP_ALLIANCE : SPELL_REQUEST_AIRSHIP_HORDE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_bomber_waiting::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 59558 - Summon Airship, Cooldown
class EG_spell_infra_green_bomber_pad_cooldown : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_pad_cooldown);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Remove();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_bomber_pad_cooldown::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 59554, 59781 - Request Airship
class EG_spell_infra_green_bomber_request : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_request);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BOMBER_GRANTED });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Creature* flightMaster = GetHitCreature();
        if (!flightMaster || !flightMaster->IsAIEnabled())
            return;

        flightMaster->AI()->Talk(SAY_BOMBER_READY, GetCaster());
        flightMaster->CastSpell(GetCaster(), SPELL_BOMBER_GRANTED, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_request::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 61373 - Bomber Granted (DND)
class EG_spell_infra_green_bomber_granted : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_granted);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_AIRSHIP_ALLIANCE, SPELL_SUMMON_AIRSHIP_HORDE, SPELL_SUMMON_AIRSHIP_LONG_ALLIANCE, SPELL_SUMMON_AIRSHIP_LONG_HORDE });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetHitPlayer();
        if (!player)
            return;

        bool const alliance = player->HasAura(SPELL_WAITING_FOR_BOMBER_ALLIANCE);
        if (!alliance && !player->HasAura(SPELL_WAITING_FOR_BOMBER_HORDE))
            return;

        player->RemoveAurasDueToSpell(alliance ? SPELL_WAITING_FOR_BOMBER_ALLIANCE : SPELL_WAITING_FOR_BOMBER_HORDE);

        auto inLog = [player](uint32 questId)
        {
            QuestStatus const status = player->GetQuestStatus(questId);
            return status == QUEST_STATUS_INCOMPLETE || status == QUEST_STATUS_COMPLETE;
        };

        bool const longRoute = alliance ? inLog(QUEST_WATTS_MY_TARGET) || inLog(QUEST_PUTTING_THE_HERTZ) : inLog(QUEST_AMPED_FOR_REVOLT) || inLog(QUEST_TOTAL_OHMAGE);
        uint32 const summonSpell = alliance ? (longRoute ? SPELL_SUMMON_AIRSHIP_LONG_ALLIANCE : SPELL_SUMMON_AIRSHIP_ALLIANCE) : (longRoute ? SPELL_SUMMON_AIRSHIP_LONG_HORDE : SPELL_SUMMON_AIRSHIP_HORDE);
        player->CastSpell(player, summonSpell, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_granted::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 59552, 59780 - Summon Airship / 61151, 61152 - Summon Airship, Long
class EG_spell_infra_green_bomber_summon : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_summon);

    void SelectLaunchPad(WorldObject*& target)
    {
        std::list<Creature*> flightMasters;
        GetCaster()->GetCreatureListWithEntryInGrid(flightMasters, NPC_INFRA_GREEN_FLIGHT_MASTER, 40.0f);
        auto launchPad = std::ranges::max_element(flightMasters, { }, [](Creature const* flightMaster) { return flightMaster->GetPositionZ(); });
        target = launchPad != flightMasters.end() ? *launchPad : nullptr;
    }

    void Register() override
    {
        OnObjectTargetSelect += SpellObjectTargetSelectFn(EG_spell_infra_green_bomber_summon::SelectLaunchPad, EFFECT_0, TARGET_DEST_NEARBY_ENTRY);
    }
};

// 59051 - Bomber onSpawn
class EG_spell_infra_green_bomber_on_spawn : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_on_spawn);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_FLIGHT_ORDERS, SPELL_DRIVE_BOMBER });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* player = GetHitPlayer();
        Vehicle* bomber = GetCaster()->GetVehicleKit();
        if (!player || !bomber)
            return;

        player->CastSpell(player, SPELL_FLIGHT_ORDERS, true);
        if (Unit* engineeringSeat = bomber->GetPassenger(SEAT_ENGINEERING))
            player->CastSpell(engineeringSeat, SPELL_DRIVE_BOMBER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_on_spawn::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 59341 - Synch Ship with Seats
class EG_spell_infra_green_bomber_synch_ship : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_synch_ship);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SYNCH_HEALTH, SPELL_INFRA_GREEN_SHIELD });
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* bomber = GetTarget();
        Vehicle* vehicle = bomber->GetVehicleKit();
        if (!vehicle)
            return;

        Aura const* shield = bomber->GetAura(SPELL_INFRA_GREEN_SHIELD);
        for (int8 seatId : { SEAT_BOMBER_BAY, SEAT_TURRET, SEAT_ENGINEERING })
        {
            Unit* seat = vehicle->GetPassenger(seatId);
            if (!seat)
                continue;

            bomber->CastSpell(seat, SPELL_SYNCH_HEALTH, true);
            Aura const* seatShield = seat->GetAura(SPELL_INFRA_GREEN_SHIELD);
            if (!shield)
                seat->RemoveAurasDueToSpell(SPELL_INFRA_GREEN_SHIELD);
            else if (!seatShield || seatShield->GetStackAmount() != shield->GetStackAmount())
                seat->SetAuraStack(SPELL_INFRA_GREEN_SHIELD, seat, shield->GetStackAmount());
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_bomber_synch_ship::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 59413 - Synch Health
class EG_spell_infra_green_bomber_synch_health : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_synch_health);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* seat = GetHitUnit();
        seat->SetHealth(seat->CountPctFromMaxHealth(GetCaster()->GetHealthPct()));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_synch_health::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 60125 - Send Hate Aura
class EG_spell_infra_green_bomber_send_hate_aura : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_send_hate_aura);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SEND_HATE });
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_SEND_HATE, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_bomber_send_hate_aura::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 59288 - Infra-Green Shield
class EG_spell_infra_green_shield : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_AIRSHIP_SHIELD_RECENTLY_HIT_SMALL });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        amount = 100000;
    }

    void HandleAbsorb(AuraEffect* /*aurEff*/, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        static constexpr std::array<std::pair<uint8, uint32>, 7> AbsorbPctByStacks =
        { {
            { 96, 100 },
            { 92, 95 },
            { 86, 90 },
            { 81, 85 },
            { 76, 80 },
            { 52, 70 },
            { 22, 60 }
        } };

        uint32 absorbPct = 50;
        for (auto const& [stacks, pct] : AbsorbPctByStacks)
        {
            if (GetStackAmount() >= stacks)
            {
                absorbPct = pct;
                break;
            }
        }

        absorbAmount = CalculatePct(dmgInfo.GetDamage(), absorbPct);
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(GetTarget(), SPELL_AIRSHIP_SHIELD_RECENTLY_HIT_SMALL, true);
        ModStackAmount(-2);
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        ModStackAmount(-1);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(EG_spell_infra_green_shield::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        OnEffectAbsorb += AuraEffectAbsorbFn(EG_spell_infra_green_shield::HandleAbsorb, EFFECT_0);
        OnEffectProc += AuraEffectProcFn(EG_spell_infra_green_shield::HandleProc, EFFECT_1, SPELL_AURA_PROC_TRIGGER_SPELL);
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_shield::HandlePeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 59061 - Charge Shield
class EG_spell_infra_green_bomber_charge_shield : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_charge_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_INFRA_GREEN_SHIELD });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* bomber = GetCaster()->GetVehicleBase();
        if (!bomber)
            return;

        int32 const stacks = bomber->HasAura(SPELL_INFRA_GREEN_SHIELD) ? GetEffectValue() - 1 : GetEffectValue();
        GetCaster()->CastSpell(bomber, SPELL_INFRA_GREEN_SHIELD, CastSpellExtraArgs(true).AddSpellMod(SPELLVALUE_AURA_STACK, stacks));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_charge_shield::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60674 - Icecrown Bomber. Airship Shield, on self
class EG_spell_infra_green_bomber_airship_shield : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_airship_shield);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_INFRA_GREEN_SHIELD });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* target = GetHitUnit();
        target->CastSpell(target, SPELL_INFRA_GREEN_SHIELD, CastSpellExtraArgs(true).AddSpellMod(SPELLVALUE_AURA_STACK, target->HasAura(SPELL_INFRA_GREEN_SHIELD) ? 29 : 30));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_airship_shield::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 60339 - Airship Shield Recently Hit, small
class EG_spell_infra_green_bomber_shield_hit : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_shield_hit);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_AIRSHIP_SHIELD_RECENTLY_HIT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Vehicle* bomber = GetCaster()->GetVehicleKit())
            if (Unit* shieldVisual = bomber->GetPassenger(SEAT_SHIELD_VISUAL))
                GetCaster()->CastSpell(shieldVisual, SPELL_AIRSHIP_SHIELD_RECENTLY_HIT, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_shield_hit::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 59193 - Engineering / 59194 - Bomber Bay / 59196 - Anti-Air Turret
class EG_spell_infra_green_bomber_switch_seat : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_switch_seat);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SWITCH_SEATS });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* seat = GetCaster();
        Unit* bomber = seat->GetVehicleBase();
        Vehicle* seatVehicle = seat->GetVehicleKit();
        Unit* pilot = seatVehicle ? seatVehicle->GetPassenger(0) : nullptr;
        if (!bomber || !bomber->GetVehicleKit() || !pilot)
            return;

        int8 const targetSeatId = GetSpellInfo()->Id == SPELL_ENGINEERING ? SEAT_ENGINEERING : (GetSpellInfo()->Id == SPELL_BOMBER_BAY ? SEAT_BOMBER_BAY : SEAT_TURRET);
        Unit* targetSeat = bomber->GetVehicleKit()->GetPassenger(targetSeatId);
        if (targetSeat && targetSeat != seat)
            pilot->CastSpell(targetSeat, SPELL_SWITCH_SEATS, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_switch_seat::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 59622 - Anti-Air Rocket
class EG_spell_infra_green_bomber_anti_air_rocket : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_anti_air_rocket);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_AA_SHOT_TRIGGER_MISSILE });
    }

    void HandleLaunch(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        if (WorldLocation const* dest = GetExplTargetDest())
            GetCaster()->CastSpell(*dest, SPELL_AA_SHOT_TRIGGER_MISSILE, true);
    }

    void Register() override
    {
        OnEffectLaunch += SpellEffectFn(EG_spell_infra_green_bomber_anti_air_rocket::HandleLaunch, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60523 - Bomb Blast
class EG_spell_infra_green_bomber_bomb_blast : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_bomb_blast);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_QUEST_CREDIT_TO_PLAYER_INFANTRY, SPELL_QUEST_CREDIT_TO_PLAYER_CAPTAIN });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Creature* target = GetHitCreature();
        if (!target)
            return;

        switch (target->GetEntry())
        {
            case NPC_ENRAGED_FLESHRENDER:
            case NPC_SKELETAL_FOOTSOLDIER:
                target->CastSpell(GetCaster(), SPELL_QUEST_CREDIT_TO_PLAYER_INFANTRY, true);
                break;
            case NPC_STITCHED_BRUTE:
                target->CastSpell(GetCaster(), SPELL_QUEST_CREDIT_TO_PLAYER_CAPTAIN, true);
                break;
            default:
                break;
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_bomb_blast::HandleDummy, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 60965 - Quest Credit to Player - Bombardment Infantry / 60973 - Quest Credit to Player - Bombardment Captain
class EG_spell_infra_green_bomber_credit_to_player : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_credit_to_player);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_QUEST_CREDIT_INFANTRY, SPELL_QUEST_CREDIT_CAPTAIN });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* seat = GetHitUnit();
        Vehicle* seatVehicle = seat->GetVehicleKit();
        Unit* pilot = seatVehicle ? seatVehicle->GetPassenger(0) : nullptr;
        if (pilot && pilot->IsPlayer())
            seat->CastSpell(pilot, GetSpellInfo()->Id == SPELL_QUEST_CREDIT_TO_PLAYER_INFANTRY ? SPELL_QUEST_CREDIT_INFANTRY : SPELL_QUEST_CREDIT_CAPTAIN, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_credit_to_player::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 60938 - Smolder Banner
class EG_spell_infra_green_bomber_smolder_banner : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_smolder_banner);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SMOLDER });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Vehicle* bomber = GetCaster()->GetVehicleKit();
        if (!bomber)
            return;

        std::vector<Unit*> banners;
        for (int8 seatId : { SEAT_BANNER_HANGING, SEAT_BANNER_SIDE_LEFT, SEAT_BANNER_SIDE_RIGHT })
            if (Unit* banner = bomber->GetPassenger(seatId))
                if (!banner->HasAura(SPELL_SMOLDER))
                    banners.push_back(banner);

        if (!banners.empty())
            GetCaster()->CastSpell(Trinity::Containers::SelectRandomContainerElement(banners), SPELL_SMOLDER, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_smolder_banner::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 60935 - Smolder
class EG_spell_infra_green_bomber_smolder : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_smolder);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BURNING });
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* banner = GetTarget();
        Unit* bomber = banner->GetVehicleBase();
        if (!bomber)
            return;

        banner->CastSpell(bomber, SPELL_BURNING, true);
        for (int8 seatId : { SEAT_BOMBER_BAY, SEAT_TURRET, SEAT_ENGINEERING })
            if (Unit* seat = bomber->GetVehicleKit()->GetPassenger(seatId))
                banner->CastSpell(seat, SPELL_BURNING, true);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* bomber = GetTarget()->GetVehicleBase();
        if (!bomber)
            return;

        Vehicle* vehicle = bomber->GetVehicleKit();
        for (int8 seatId : { SEAT_BANNER_HANGING, SEAT_BANNER_SIDE_LEFT, SEAT_BANNER_SIDE_RIGHT })
            if (Unit* banner = vehicle->GetPassenger(seatId))
                if (banner != GetTarget() && banner->HasAura(SPELL_SMOLDER))
                    return;

        bomber->RemoveAurasDueToSpell(SPELL_BURNING);
        for (int8 seatId : { SEAT_BOMBER_BAY, SEAT_TURRET, SEAT_ENGINEERING })
            if (Unit* seat = vehicle->GetPassenger(seatId))
                seat->RemoveAurasDueToSpell(SPELL_BURNING);
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        if (Unit* bomber = GetTarget()->GetVehicleBase())
            Unit::DealDamage(bomber, bomber, 1000);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_spell_infra_green_bomber_smolder::HandleApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(EG_spell_infra_green_bomber_smolder::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_bomber_smolder::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 59411 - Damage Control
class EG_spell_infra_green_bomber_damage_control : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_damage_control);

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* bomber = GetTarget();
        Vehicle* vehicle = bomber->GetVehicleKit();
        if (!vehicle)
            return;

        for (int8 seatId : { SEAT_BANNER_HANGING, SEAT_BANNER_SIDE_LEFT, SEAT_BANNER_SIDE_RIGHT })
            if (Unit* banner = vehicle->GetPassenger(seatId))
                if (banner->HasAura(SPELL_SMOLDER))
                    return;

        bomber->ModifyHealth(4000);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_bomber_damage_control::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 61093 - Fight Fire
class EG_spell_infra_green_bomber_fight_fire : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_fight_fire);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_EXTINGUISH_FIRE });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* bomber = GetCaster()->GetVehicleBase();
        if (!bomber || !bomber->GetVehicleKit())
            return;

        for (int8 seatId : { SEAT_BANNER_HANGING, SEAT_BANNER_SIDE_LEFT, SEAT_BANNER_SIDE_RIGHT })
        {
            Unit* banner = bomber->GetVehicleKit()->GetPassenger(seatId);
            if (banner && banner->HasAura(SPELL_SMOLDER))
            {
                GetCaster()->CastSpell(banner, SPELL_EXTINGUISH_FIRE, true);
                return;
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_fight_fire::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 61172 - Extinguish Fire
class EG_spell_infra_green_bomber_extinguish_fire : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_extinguish_fire);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_SMOLDER);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_extinguish_fire::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 60852 - Airship Explodes Sequence
class EG_spell_infra_green_bomber_explodes_sequence : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_explodes_sequence);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_AIRSHIP_EXPLODES, SPELL_AIRSHIP_EXPLODES_BIG, SPELL_AIRSHIP_EXPLODES_HUGE, SPELL_AIRSHIP_EXPLODES_WITH_FIREBALL });
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        GetTarget()->CastSpell(GetTarget(), Trinity::Containers::SelectRandomContainerElement(std::array<uint32, 4>{ SPELL_AIRSHIP_EXPLODES, SPELL_AIRSHIP_EXPLODES_BIG, SPELL_AIRSHIP_EXPLODES_HUGE, SPELL_AIRSHIP_EXPLODES_WITH_FIREBALL }), true);
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Creature* bomber = GetTarget()->ToCreature())
            bomber->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_bomber_explodes_sequence::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(EG_spell_infra_green_bomber_explodes_sequence::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 61176 - Bomber Killed
class EG_spell_infra_green_bomber_killed : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_killed);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_REPORTED_KILLS });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_REPORTED_KILLS, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_killed::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 61282 - Mission Complete
class EG_spell_infra_green_bomber_mission_complete : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_bomber_mission_complete);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->RemoveAurasDueToSpell(SPELL_FLIGHT_ORDERS);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_bomber_mission_complete::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 60918 - Finished Sortie
class EG_spell_infra_green_bomber_finished_sortie : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_finished_sortie);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PARACHUTE });
    }

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell(GetTarget(), SPELL_PARACHUTE, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_spell_infra_green_bomber_finished_sortie::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 60816 - Eject Player From Craft
class EG_spell_infra_green_bomber_eject_player : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_bomber_eject_player);

    void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Vehicle* seat = GetTarget()->GetVehicleKit())
            seat->RemoveAllPassengers();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(EG_spell_infra_green_bomber_eject_player::HandleApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 60784 - Rescue Craft - Pick up Fallen Pilot
class EG_spell_infra_green_rescue_craft_pick_up : public SpellScript
{
    PrepareSpellScript(EG_spell_infra_green_rescue_craft_pick_up);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_BOARD_RESCUE_CRAFT });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetCaster(), SPELL_BOARD_RESCUE_CRAFT, CastSpellExtraArgs(true).AddSpellBP0(2));
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(EG_spell_infra_green_rescue_craft_pick_up::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 61360 - Parachute
class EG_spell_infra_green_rescue_parachute : public AuraScript
{
    PrepareAuraScript(EG_spell_infra_green_rescue_parachute);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_PARACHUTE_BUFF });
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* target = GetTarget();
        if (!target->IsFalling())
            return;

        Remove();
        target->CastSpell(target, SPELL_PARACHUTE_BUFF, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(EG_spell_infra_green_rescue_parachute::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

void AddSC_EG_gen_spell_scripts()
{
    RegisterSpellScript(EG_spell_cosmetic___divine_shield_blue);
    RegisterSpellScript(EG_spell_destroy_deathforged_infernal);
    RegisterSpellScript(EG_spell_fel_reaver_sentinel_tag);
    RegisterSpellScript(EG_spell_fel_reaver_controller);
    RegisterSpellScript(EG_spell_charm_channel);
    RegisterSpellScript(EG_spell_charm_drakuru_servant);
    RegisterSpellScript(EG_spell_prayer_beads);
    RegisterSpellScript(EG_spell_fiery_lance);
    RegisterSpellScript(EG_spell_lava_strike);
    RegisterSpellScript(EG_spell_naxx_bigglesworth_curse);
    RegisterSpellScript(EG_spell_deploy_salvage_saws);
    RegisterSpellScript(EG_spell_displacement_device);
    RegisterSpellScript(EG_spell_twilight_torment_damage);
    RegisterSpellScript(EG_spell_twilight_torment_carrier);
    RegisterSpellScript(EG_spell_twilight_torment_phase);
    RegisterSpellScript(EG_spell_twilight_torment_trigger);
    RegisterSpellScript(EG_spell_energy_sap);
    RegisterSpellScript(EG_spell_freya_summon_healthy_spore);
    RegisterSpellScript(EG_spell_arachnopod_damaged);
    RegisterSpellScript(EG_spell_brightleaf_unstable_sun_beam_forced);
    RegisterSpellScript(EG_spell_stonebark_petrified_bark);
    RegisterSpellScript(EG_spell_hadronox_web_grab);
    RegisterSpellScript(EG_spell_varos_summon_arcane_beam);
    RegisterSpellScript(EG_spell_varos_arcane_beam_spawn_trigger);
    RegisterSpellScript(EG_spell_varos_arcane_beam_visual);
    RegisterSpellScript(EG_spell_ruins_of_ahnqiraj_attack_order);
    RegisterSpellScript(EG_spell_netherstorm_ring_of_flame);
    RegisterSpellScript(EG_spell_wintergrasp_rp_gg);
    RegisterSpellScript(EG_spell_wintergrasp_rp_gg_impact);
    RegisterSpellScript(EG_spell_infra_green_bomber_waiting);
    RegisterSpellScript(EG_spell_infra_green_bomber_pad_cooldown);
    RegisterSpellScript(EG_spell_infra_green_bomber_request);
    RegisterSpellScript(EG_spell_infra_green_bomber_granted);
    RegisterSpellScript(EG_spell_infra_green_bomber_summon);
    RegisterSpellScript(EG_spell_infra_green_bomber_on_spawn);
    RegisterSpellScript(EG_spell_infra_green_bomber_synch_ship);
    RegisterSpellScript(EG_spell_infra_green_bomber_synch_health);
    RegisterSpellScript(EG_spell_infra_green_bomber_send_hate_aura);
    RegisterSpellScript(EG_spell_infra_green_shield);
    RegisterSpellScript(EG_spell_infra_green_bomber_charge_shield);
    RegisterSpellScript(EG_spell_infra_green_bomber_airship_shield);
    RegisterSpellScript(EG_spell_infra_green_bomber_shield_hit);
    RegisterSpellScript(EG_spell_infra_green_bomber_switch_seat);
    RegisterSpellScript(EG_spell_infra_green_bomber_anti_air_rocket);
    RegisterSpellScript(EG_spell_infra_green_bomber_bomb_blast);
    RegisterSpellScript(EG_spell_infra_green_bomber_credit_to_player);
    RegisterSpellScript(EG_spell_infra_green_bomber_smolder_banner);
    RegisterSpellScript(EG_spell_infra_green_bomber_smolder);
    RegisterSpellScript(EG_spell_infra_green_bomber_damage_control);
    RegisterSpellScript(EG_spell_infra_green_bomber_fight_fire);
    RegisterSpellScript(EG_spell_infra_green_bomber_extinguish_fire);
    RegisterSpellScript(EG_spell_infra_green_bomber_explodes_sequence);
    RegisterSpellScript(EG_spell_infra_green_bomber_killed);
    RegisterSpellScript(EG_spell_infra_green_bomber_mission_complete);
    RegisterSpellScript(EG_spell_infra_green_bomber_finished_sortie);
    RegisterSpellScript(EG_spell_infra_green_bomber_eject_player);
    RegisterSpellScript(EG_spell_infra_green_rescue_craft_pick_up);
    RegisterSpellScript(EG_spell_infra_green_rescue_parachute);
}
