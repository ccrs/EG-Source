#include "ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "InstanceScript.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Unit.h"

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
}
