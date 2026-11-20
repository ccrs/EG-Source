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
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Battlefield/BattlefieldWG.h"
#include "CombatAI.h"
#include "ConditionMgr.h"
#include "Creature.h"
#include "DBCStructure.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SharedDefines.h"
#include "SpellAuraDefines.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Timer.h"
#include "Unit.h"
#include "Vehicle.h"
#include "WorldSession.h"
#include <vector>

enum ZoneWintergraspNPCTexts
{
    NPC_TEXT_HORDE_NOBATTLE              = 14775,
    NPC_TEXT_HORDE_QUEUE                 = 14790,
    NPC_TEXT_HORDE_BATTLE                = 14777,
    NPC_TEXT_ALLIANCE_NOBATTLE           = 14782,
    NPC_TEXT_ALLIANCE_QUEUE              = 14791,
    NPC_TEXT_ALLIANCE_BATTLE             = 14781
};

enum ZoneWintergraspGossipMenuOptionIds
{
    GOSSIP_MENU_OPTION_ENGINEER_CATAPULT     = 0,
    GOSSIP_MENU_OPTION_ENGINEER_DEMOLISHER   = 1,
    GOSSIP_MENU_OPTION_ENGINEER_SIEGE_ENGINE = 2
};

enum ZoneWintergraspSpells
{
    SPELL_BUILD_SIEGE_VEHICLE_FORCE_HORDE    = 61409, // Demolisher engineers spells
    SPELL_BUILD_SIEGE_VEHICLE_FORCE_ALLIANCE = 56662,
    SPELL_BUILD_CATAPULT_FORCE               = 56664,
    SPELL_BUILD_DEMOLISHER_FORCE             = 56659,
    SPELL_ACTIVATE_CONTROL_ARMS              = 49899,
    SPELL_RIDE_WINTERGRASP_VEHICLE           = 60968,
    SPELL_VEHICLE_TELEPORT                   = 49759,
    SPELL_CHANNEL_SPIRIT_HEAL                = 22011, // Spirit guide
    SPELL_WINTERGRASP_TELEPORT_TRIGGER       = 54643,
};

enum ZoneWintergraspCreatureEntries
{
    NPC_WORLD_TRIGGER_LARGE_AOI_NOT_IMMUNE_PC_NPC = 23472,
};

enum ZoneWintergraspQuestIds
{
    QUEST_BONES_AND_ARROWS_HORDE_ATT              = 13199,
    QUEST_JINXING_THE_WALLS_HORDE_ATT             = 13202,
    QUEST_SLAY_THEM_ALL_HORDE_ATT                 = 13180,
    QUEST_FUELING_THE_DEMOLISHERS_HORDE_ATT       = 13200,
    QUEST_HEALING_WITH_ROSES_HORDE_ATT            = 13201,
    QUEST_DEFEND_THE_SIEGE_HORDE_ATT              = 13223,

    QUEST_BONES_AND_ARROWS_HORDE_DEF              = 13193,
    QUEST_WARDING_THE_WALLS_HORDE_DEF             = 13192,
    QUEST_SLAY_THEM_ALL_HORDE_DEF                 = 13178,
    QUEST_FUELING_THE_DEMOLISHERS_HORDE_DEF       = 13191,
    QUEST_HEALING_WITH_ROSES_HORDE_DEF            = 13194,
    QUEST_TOPPLING_THE_TOWERS_HORDE_DEF           = 13539,
    QUEST_STOP_THE_SIEGE_HORDE_DEF                = 13185,

    QUEST_BONES_AND_ARROWS_ALLIANCE_ATT           = 13196,
    QUEST_WARDING_THE_WARRIORS_ALLIANCE_ATT       = 13198,
    QUEST_NO_MERCY_FOR_THE_MERCILESS_ALLIANCE_ATT = 13179,
    QUEST_DEFEND_THE_SIEGE_ALLIANCE_ATT           = 13222,
    QUEST_A_RARE_HERB_ALLIANCE_ATT                = 13195,

    QUEST_BONES_AND_ARROWS_ALLIANCE_DEF           = 13154,
    QUEST_WARDING_THE_WARRIORS_ALLIANCE_DEF       = 13153,
    QUEST_NO_MERCY_FOR_THE_MERCILESS_ALLIANCE_DEF = 13177,
    QUEST_SHOUTHERN_SABOTAGE_ALLIANCE_DEF         = 13538,
    QUEST_STOP_THE_SIEGE_ALLIANCE_DEF             = 13186,
    QUEST_A_RARE_HERB_ALLIANCE_DEF                = 13156,
};

static std::vector<uint32> const wintergraspVehicleIds =
{
    NPC_WINTERGRASP_CATAPULT,
    NPC_WINTERGRASP_DEMOLISHER,
    NPC_WINTERGRASP_SIEGE_ENGINE_ALLIANCE,
    NPC_WINTERGRASP_SIEGE_ENGINE_HORDE
};

struct npc_wg_demolisher_engineer : public ScriptedAI
{
    npc_wg_demolisher_engineer(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        CloseGossipMenuFor(player);
        switch (gossipListId)
        {
            case GOSSIP_MENU_OPTION_ENGINEER_CATAPULT:
                DoCast(player, SPELL_BUILD_CATAPULT_FORCE, true);
                break;
            case GOSSIP_MENU_OPTION_ENGINEER_DEMOLISHER:
                DoCast(player, SPELL_BUILD_DEMOLISHER_FORCE, true);
                break;
            case GOSSIP_MENU_OPTION_ENGINEER_SIEGE_ENGINE:
                DoCast(player, player->GetTeamId() == TEAM_ALLIANCE ? SPELL_BUILD_SIEGE_VEHICLE_FORCE_ALLIANCE : SPELL_BUILD_SIEGE_VEHICLE_FORCE_HORDE, true);
                break;
            default:
                break;
        }
        return true;
    }
};

struct npc_wg_spirit_guide : public ScriptedAI
{
    npc_wg_spirit_guide(Creature* creature) : ScriptedAI(creature), _graveyardId(0) { }

    void InitializeAI() override
    {
        if (Battlefield* wintergrasp = sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP))
            if (BattlefieldEntity const* owner = wintergrasp->FindObjectOwner(me).first)
                if (owner->Info.EntityType == BATTLEFIELD_ENTITY_TYPE_GRAVEYARD)
                    _graveyardId = owner->Info.Id;

        ScriptedAI::InitializeAI();
    }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!me->HasUnitState(UNIT_STATE_CASTING))
            DoCast(me, SPELL_CHANNEL_SPIRIT_HEAL);
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        if (_graveyardId)
            if (BattlefieldWintergrasp* wintergrasp = dynamic_cast<BattlefieldWintergrasp*>(sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP)))
                for (uint8 itr = GRAVEYARD_KEEP; itr <= GRAVEYARD_ALLIANCE; ++itr)
                {
                    if (BattlefieldGraveyard const* targetGraveyard = wintergrasp->GetEntity<BattlefieldGraveyard>(itr))
                        if (TeamIdByPvPTeamId(targetGraveyard->GetPvPTeamId()) == player->GetTeamId())
                            AddGossipItemFor(player, GOSSIP_ICON_CHAT, player->GetSession()->GetTrinityString(targetGraveyard->TextId), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + itr);
                }

        SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
        CloseGossipMenuFor(player);

        if (_graveyardId)
            if (BattlefieldWintergrasp* wintergrasp = dynamic_cast<BattlefieldWintergrasp*>(sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP)))
                if (BattlefieldGraveyard const* graveyard = wintergrasp->GetEntity<BattlefieldGraveyard>(action - GOSSIP_ACTION_INFO_DEF))
                    if (TeamIdByPvPTeamId(graveyard->GetPvPTeamId()) == player->GetTeamId())
                        DoCast(player, graveyard->GetTeleportSpellId(), TRIGGERED_IGNORE_CAST_IN_PROGRESS);
        return true;
    }
private:
    uint32 _graveyardId;
};

struct npc_wg_queue : public ScriptedAI
{
    npc_wg_queue(Creature* creature) : ScriptedAI(creature)
    {
    }

    void Reset() override
    {
    }

    void JustEngagedWith(Unit* /*who*/) override { }

    void UpdateAI(uint32 /*diff*/) override
    {
        DoMeleeAttackIfReady();
    }

    bool OnGossipHello(Player* player) override
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        Battlefield* wintergrasp = sBattlefieldMgr->GetEnabledBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP);
        if (!wintergrasp)
            return true;

        bool const horde = wintergrasp->GetControllingTeamId() == TEAM_HORDE;
        if ((wintergrasp->IsBattleTime() || wintergrasp->IsInPreparation()) && wintergrasp->CanJoinBattle(player))
            AddGossipItemFor(player, me->GetCreatureTemplate()->GossipMenuId, 0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        if (wintergrasp->IsBattleTime())
            SendGossipMenuFor(player, horde ? NPC_TEXT_HORDE_BATTLE : NPC_TEXT_ALLIANCE_BATTLE, me->GetGUID());
        else if (wintergrasp->IsInPreparation())
            SendGossipMenuFor(player, horde ? NPC_TEXT_HORDE_QUEUE : NPC_TEXT_ALLIANCE_QUEUE, me->GetGUID());
        else
            SendGossipMenuFor(player, horde ? NPC_TEXT_HORDE_NOBATTLE : NPC_TEXT_ALLIANCE_NOBATTLE, me->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 /*gossipListId*/) override
    {
        CloseGossipMenuFor(player);
        if (Battlefield* wintergrasp = sBattlefieldMgr->GetEnabledBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP))
            wintergrasp->HandleQueueRequest(player);
        return true;
    }
};

struct npc_wg_siege_machine : public VehicleAI
{
    npc_wg_siege_machine(Creature* creature) : VehicleAI(creature), _checkTimer(1000) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        Player* builder = summoner->ToPlayer();
        if (!builder)
            return;

        PvPTeamId const pvpTeamId = PvPTeamIdByTeamId(builder->GetTeamId());
        if (Battlefield* wintergrasp = sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP))
            me->SetFaction(wintergrasp->GetFaction(pvpTeamId));

        DoCast(builder, SPELL_WINTERGRASP_GRAB_PASSENGER, true);
        DoCastSelf(pvpTeamId == PVP_TEAM_HORDE ? SPELL_WINTERGRASP_HORDE_FLAG : SPELL_WINTERGRASP_ALLIANCE_FLAG, true);
    }

    void UpdateAI(uint32 diff) override
    {
        VehicleAI::UpdateAI(diff);

        _checkTimer.Update(diff);
        if (!_checkTimer.Passed())
            return;

        _checkTimer.Reset(1000);
        Vehicle* vehicle = me->GetVehicleKit();
        if (!vehicle)
            return;

        for (SeatMap::value_type const& seat : vehicle->Seats)
        {
            if (!seat.second.SeatInfo->HasFlag(VEHICLE_SEAT_FLAG_CAN_CONTROL))
                continue;

            Unit* driver = ObjectAccessor::GetUnit(*me, seat.second.Passenger.Guid);
            if (!driver || driver->HasAura(SPELL_WINTERGRASP_LIEUTENANT) || (me->GetEntry() == NPC_WINTERGRASP_CATAPULT && driver->HasAura(SPELL_WINTERGRASP_CORPORAL)))
                continue;

            driver->ExitVehicle();
            return;
        }
    }

private:
    TimeTracker _checkTimer;
};

struct go_wg_vehicle_teleporter : public GameObjectAI
{
    go_wg_vehicle_teleporter(GameObject* gameObject) : GameObjectAI(gameObject), _checkTimer(1000) { }

    void UpdateAI(uint32 diff)
    {
        _checkTimer.Update(diff);
        if (_checkTimer.Passed())
        {
            Battlefield* wintergrasp = sBattlefieldMgr->GetEnabledBattlefield(me->GetZoneId());
            if (!wintergrasp || !wintergrasp->IsBattleTime())
            {
                _checkTimer.Reset(60000);
                return;
            }

            Creature* teleportTrigger = me->FindNearestCreature(NPC_WORLD_TRIGGER_LARGE_AOI_NOT_IMMUNE_PC_NPC, 5.0f, true);
            if (!teleportTrigger)
            {
                _checkTimer.Reset(60000);
                return;
            }

            for (uint32 entry : wintergraspVehicleIds)
            {
                if (Creature* vehicle = me->FindNearestCreature(entry, 5.0f, true))
                    HandleTeleport(vehicle, teleportTrigger, wintergrasp);
            }

            _checkTimer.Reset(2000);
        }
    }

private:
    void HandleTeleport(Creature* vehicle, Creature* teleportTrigger, Battlefield* wintergrasp)
    {
        if (vehicle->HasAura(SPELL_VEHICLE_TELEPORT))
            return;

        Player* owner = vehicle->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!owner)
            return;

        if (wintergrasp->GetAttackingTeamId() != owner->GetTeamId())
            return;

        teleportTrigger->CastSpell(vehicle, SPELL_VEHICLE_TELEPORT, true);
    }

    TimeTracker _checkTimer;
};

struct go_wg_titan_relic : public GameObjectAI
{
    go_wg_titan_relic(GameObject* gameObject) : GameObjectAI(gameObject) { }

    bool OnGossipHello(Player* player) override
    {
        BattlefieldWintergrasp* wintergrasp = dynamic_cast<BattlefieldWintergrasp*>(sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP));
        return !wintergrasp || !wintergrasp->CanUseTitanRelic(player);
    }
};

// 56659 - Build Demolisher (Force)
// 56662 - Build Siege Vehicle (Force)
// 56664 - Build Catapult (Force)
// 61409 - Build Siege Vehicle (Force)
class spell_wintergrasp_force_building : public SpellScript
{
    PrepareSpellScript(spell_wintergrasp_force_building);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo(
        {
            SPELL_BUILD_CATAPULT_FORCE,
            SPELL_BUILD_DEMOLISHER_FORCE,
            SPELL_BUILD_SIEGE_VEHICLE_FORCE_HORDE,
            SPELL_BUILD_SIEGE_VEHICLE_FORCE_ALLIANCE,
            SPELL_ACTIVATE_CONTROL_ARMS
        });
    }

    void HandleScript(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        Unit* target = GetHitUnit();
        if (target->CastSpell(target, GetEffectValue(), false) == SPELL_CAST_OK)
            target->CastSpell(nullptr, SPELL_ACTIVATE_CONTROL_ARMS, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_wintergrasp_force_building::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 56575 - Build Demolisher
// 56661 - Build Siege Engine
// 56663 - Build Catapult
// 61408 - Build Siege Engine
class spell_wintergrasp_build_vehicle : public SpellScript
{
    PrepareSpellScript(spell_wintergrasp_build_vehicle);

    SpellCastResult CheckCast()
    {
        Player* player = GetCaster()->ToPlayer();
        BattlefieldWintergrasp* wintergrasp = dynamic_cast<BattlefieldWintergrasp*>(sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP));
        if (player && wintergrasp && wintergrasp->CanBuildVehicle(PvPTeamIdByTeamId(player->GetTeamId())))
            return SPELL_CAST_OK;

        SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_BUILD_MORE_VEHICLES);
        return SPELL_FAILED_CUSTOM_ERROR;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_wintergrasp_build_vehicle::CheckCast);
    }
};

// 61178 - Grab Passenger
class spell_wintergrasp_grab_passenger : public SpellScript
{
    PrepareSpellScript(spell_wintergrasp_grab_passenger);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Player* target = GetHitPlayer())
            target->CastSpell(GetCaster(), SPELL_RIDE_WINTERGRASP_VEHICLE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_wintergrasp_grab_passenger::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 49899 - Activate Robotic Arms
class spell_wintergrasp_activate_robotic_arms : public SpellScript
{
    PrepareSpellScript(spell_wintergrasp_activate_robotic_arms);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->HandleEmoteCommand(EMOTE_ONESHOT_CUSTOM_SPELL_05);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_wintergrasp_activate_robotic_arms::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class achievement_wg_didnt_stand_a_chance : public AchievementCriteriaScript
{
    public:
        achievement_wg_didnt_stand_a_chance() : AchievementCriteriaScript("achievement_wg_didnt_stand_a_chance") { }

        bool OnCheck(Player* source, Unit* target) override
        {
            if (!target)
                return false;

            if (Player* victim = target->ToPlayer())
            {
                if (!victim->IsMounted())
                    return false;

                if (Vehicle* vehicle = source->GetVehicle())
                    if (vehicle->GetVehicleInfo()->ID == 244) // Wintergrasp Tower Cannon
                        return true;
            }

            return false;
        }
};

// 54640 - Teleport (Used also in Strand of the Ancients)
class spell_wintergrasp_defender_teleport : public SpellScript
{
    PrepareSpellScript(spell_wintergrasp_defender_teleport);

    SpellCastResult CheckCast()
    {
        if (Player* target = GetExplTargetUnit()->ToPlayer())
        {
            if (target->GetZoneId() == BATTLEFIELD_ZONEID_WINTERGRASP)
            {
                if (Battlefield* wintergrasp = sBattlefieldMgr->GetBattlefield(target->GetZoneId()))
                {
                    if (wintergrasp->GetControllingTeam() != PVP_TEAM_NEUTRAL && target->GetTeamId() != wintergrasp->GetControllingTeamId())
                        return SPELL_FAILED_BAD_TARGETS;

                    if (target->HasAura(SPELL_WINTERGRASP_TELEPORT_TRIGGER))
                        return SPELL_FAILED_BAD_TARGETS;

                    return SPELL_CAST_OK;
                }

                return SPELL_FAILED_DONT_REPORT;
            }
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_wintergrasp_defender_teleport::CheckCast);
    }
};

// 54643 - Teleport
class spell_wintergrasp_defender_teleport_trigger : public SpellScript
{
    PrepareSpellScript(spell_wintergrasp_defender_teleport_trigger);

    void HandleDummy(SpellEffIndex /*effindex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            WorldLocation destination = target->GetWorldLocation();
            SetExplTargetDest(destination);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_wintergrasp_defender_teleport_trigger::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 58549 - Tenacity
// 59911 - Tenacity
class spell_wintergrasp_tenacity_refresh : public AuraScript
{
    PrepareAuraScript(spell_wintergrasp_tenacity_refresh);

    bool Validate(SpellInfo const* spellInfo) override
    {
        uint32 triggeredSpellId = spellInfo->GetEffect(EFFECT_2).CalcValue();
        return !triggeredSpellId || ValidateSpellInfo({ triggeredSpellId });
    }

    void Refresh(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        Battlefield* wintergrasp = sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP); // EG
        Player* owner = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
        if (!wintergrasp || !wintergrasp->IsBattleTime() || !owner || !wintergrasp->IsPlayerInBattle(owner) || (GetTarget() != owner && !owner->m_Controlled.contains(GetTarget())))
        {
            Remove();
            return;
        }

        RefreshDuration();
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (uint32 triggeredSpellId = aurEff->GetSpellEffectInfo().CalcValue())
            GetTarget()->RemoveAurasDueToSpell(triggeredSpellId);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_wintergrasp_tenacity_refresh::Refresh, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_wintergrasp_tenacity_refresh::OnRemove, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 2584 - Waiting to Resurrect
class spell_wintergrasp_waiting_to_resurrect : public AuraScript
{
    PrepareAuraScript(spell_wintergrasp_waiting_to_resurrect);

    bool Load() override
    {
        Unit* owner = GetUnitOwner();
        return owner && owner->GetTypeId() == TYPEID_PLAYER && owner->GetZoneId() == BATTLEFIELD_ZONEID_WINTERGRASP;
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* owner = GetUnitOwner();
        if (!owner)
            return;

        if (Battlefield* wintergrasp = sBattlefieldMgr->GetBattlefield(owner->GetZoneId()))
            wintergrasp->RemovePlayerFromResurrectionQueue(owner->ToPlayer());
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_wintergrasp_waiting_to_resurrect::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

class condition_is_wintergrasp_horde : public ConditionScript
{
    public:
        condition_is_wintergrasp_horde() : ConditionScript("condition_is_wintergrasp_horde") { }

        bool OnConditionCheck(Condition const* /*condition*/, ConditionSourceInfo& /*sourceInfo*/)
        {
            Battlefield* wintergrasp = sBattlefieldMgr->GetEnabledBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP);
            if (wintergrasp && wintergrasp->GetControllingTeam() == PVP_TEAM_HORDE)
                return true;
            return false;
        }
};

class condition_is_wintergrasp_alliance : public ConditionScript
{
    public:
        condition_is_wintergrasp_alliance() : ConditionScript("condition_is_wintergrasp_alliance") { }

        bool OnConditionCheck(Condition const* /*condition*/, ConditionSourceInfo& /*sourceInfo*/)
        {
            Battlefield* wintergrasp = sBattlefieldMgr->GetEnabledBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP);
            if (wintergrasp && wintergrasp->GetControllingTeam() == PVP_TEAM_ALLIANCE)
                return true;
            return false;
        }
};

class condition_wintergrasp_vehicle_limit : public ConditionScript
{
    public:
        condition_wintergrasp_vehicle_limit(char const* scriptName, bool buildable) : ConditionScript(scriptName), _buildable(buildable) { }

        bool OnConditionCheck(Condition const* condition, ConditionSourceInfo& sourceInfo) override
        {
            WorldObject* target = sourceInfo.mConditionTargets[condition->ConditionTarget];
            Player* player = target ? target->ToPlayer() : nullptr;
            BattlefieldWintergrasp* wintergrasp = dynamic_cast<BattlefieldWintergrasp*>(sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP));
            if (!player || !wintergrasp)
                return false;

            return wintergrasp->CanBuildVehicle(PvPTeamIdByTeamId(player->GetTeamId())) == _buildable;
        }

    private:
        bool _buildable;
};

void AddSC_wintergrasp()
{
    RegisterCreatureAI(npc_wg_queue);
    RegisterCreatureAI(npc_wg_spirit_guide);
    RegisterCreatureAI(npc_wg_demolisher_engineer);
    RegisterCreatureAI(npc_wg_siege_machine);
    RegisterGameObjectAI(go_wg_vehicle_teleporter);
    RegisterGameObjectAI(go_wg_titan_relic);
    RegisterSpellScript(spell_wintergrasp_force_building);
    RegisterSpellScript(spell_wintergrasp_build_vehicle);
    RegisterSpellScript(spell_wintergrasp_grab_passenger);
    RegisterSpellScript(spell_wintergrasp_activate_robotic_arms);
    new achievement_wg_didnt_stand_a_chance();
    RegisterSpellScript(spell_wintergrasp_defender_teleport);
    RegisterSpellScript(spell_wintergrasp_defender_teleport_trigger);
    RegisterSpellScript(spell_wintergrasp_tenacity_refresh);
    RegisterSpellScript(spell_wintergrasp_waiting_to_resurrect);
    new condition_is_wintergrasp_horde();
    new condition_is_wintergrasp_alliance();
    new condition_wintergrasp_vehicle_limit("condition_wintergrasp_vehicle_buildable", true);
    new condition_wintergrasp_vehicle_limit("condition_wintergrasp_vehicle_limit_reached", false);
}
