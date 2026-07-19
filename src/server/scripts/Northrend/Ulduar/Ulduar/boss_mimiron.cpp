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
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GridNotifiers.h"
#include "InstanceScript.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "TemporarySummon.h"
#include "Vehicle.h"

enum MimironYells
{
    SAY_AGGRO                                   = 0,
    SAY_HARDMODE_ON                             = 1,
    SAY_MKII_ACTIVATE                           = 2,
    SAY_MKII_SLAY                               = 3,
    SAY_MKII_DEATH                              = 4,
    SAY_VX001_ACTIVATE                          = 5,
    SAY_VX001_SLAY                              = 6,
    SAY_VX001_DEATH                             = 7,
    SAY_AERIAL_ACTIVATE                         = 8,
    SAY_AERIAL_SLAY                             = 9,
    SAY_AERIAL_DEATH                            = 10,
    SAY_V07TRON_ACTIVATE                        = 11,
    SAY_V07TRON_SLAY                            = 12,
    SAY_V07TRON_DEATH                           = 13,
    SAY_BERSERK                                 = 14
};

enum MimironComputerYells
{
    SAY_SELF_DESTRUCT_INITIATED                 = 0,
    SAY_SELF_DESTRUCT_TERMINATED                = 1,
    SAY_SELF_DESTRUCT_10                        = 2,
    SAY_SELF_DESTRUCT_9                         = 3,
    SAY_SELF_DESTRUCT_8                         = 4,
    SAY_SELF_DESTRUCT_7                         = 5,
    SAY_SELF_DESTRUCT_6                         = 6,
    SAY_SELF_DESTRUCT_5                         = 7,
    SAY_SELF_DESTRUCT_4                         = 8,
    SAY_SELF_DESTRUCT_3                         = 9,
    SAY_SELF_DESTRUCT_2                         = 10,
    SAY_SELF_DESTRUCT_1                         = 11,
    SAY_SELF_DESTRUCT_FINALIZED                 = 12
};

enum MimironSpells
{
    // Mimiron
    SPELL_WELD                                  = 63339, // Idle aura.
    SPELL_SEAT_1                                = 52391, // Cast on all vehicles, Cycled on MKII
    SPELL_SEAT_2                                = 63313, // Cast on MKII and VX-001, Cycled on MKII
    SPELL_SEAT_3                                = 63314, // Cast on MKII, Cycled on MKII
    SPELL_SEAT_5                                = 63316, // Cast on MKII and VX-001, Cycled on MKII
    SPELL_SEAT_6                                = 63344, // Cast on MKII
    SPELL_SEAT_7                                = 63345, // Cast on MKII
    SPELL_JETPACK                               = 63341,
    SPELL_DESPAWN_ASSAULT_BOTS                  = 64463, // only despawns assault bots... no equivalent spell for the other adds...
    SPELL_TELEPORT_VISUAL                       = 41232,
    SPELL_SLEEP_VISUAL_1                        = 64393,
    SPELL_SLEEP_VISUAL_2                        = 64394,

    // Leviathan MK II
    SPELL_FLAME_SUPPRESSANT_MK                  = 64570,
    SPELL_NAPALM_SHELL                          = 63666,
    SPELL_FORCE_CAST_NAPALM_SHELL               = 64539,
    SPELL_PLASMA_BLAST                          = 62997,
    SPELL_SCRIPT_EFFECT_PLASMA_BLAST            = 64542,
    SPELL_SHOCK_BLAST                           = 63631,
    SPELL_SHOCK_BLAST_AURA                      = 63632, // Deprecated? It is never cast.

    // VX-001
    SPELL_FLAME_SUPPRESSANT_VX                  = 65192,
    SPELL_SPINNING_UP                           = 63414,
    SPELL_HEAT_WAVE_AURA                        = 63679,
    SPELL_HAND_PULSE_LEFT                       = 64348,
    SPELL_HAND_PULSE_RIGHT                      = 64352,
    SPELL_MOUNT_MKII                            = 64387,
    SPELL_TORSO_DISABLED                        = 64120,

    // Aerial Command Unit
    SPELL_PLASMA_BALL_P1                        = 63689,
    SPELL_PLASMA_BALL_P2                        = 65647,
    SPELL_MOUNT_VX_001                          = 64388,

    // Proximity Mines
    SPELL_PROXIMITY_MINES                       = 63027, // Cast by Leviathan MK II
    SPELL_PROXIMITY_MINE_EXPLOSION              = 66351,
    SPELL_PROXIMITY_MINE_TRIGGER                = 65346,
    SPELL_PROXIMITY_MINE_PERIODIC_TRIGGER       = 65345,
    SPELL_PERIODIC_PROXIMITY_AURA               = 65345,
    SPELL_SUMMON_PROXIMITY_MINE                 = 65347,

    // Rapid Burst
    SPELL_RAPID_BURST_LEFT                      = 63387,
    SPELL_RAPID_BURST_RIGHT                     = 64019,
    SPELL_RAPID_BURST                           = 63382, // Cast by VX-001
    SPELL_RAPID_BURST_TARGET_ME                 = 64841, // Cast by Burst Target
    SPELL_SUMMON_BURST_TARGET                   = 64840, // Cast by VX-001

    // Rocket Strike
    SPELL_SUMMON_ROCKET_STRIKE                  = 63036,
    SPELL_SCRIPT_EFFECT_ROCKET_STRIKE           = 63681, // Cast by Rocket (Mimiron Visual)
    SPELL_ROCKET_STRIKE                         = 64064, // Added in creature_template_addon
    SPELL_ROCKET_STRIKE_SINGLE                  = 64402, // Cast by VX-001
    SPELL_ROCKET_STRIKE_BOTH                    = 65034, // Cast by VX-001

    // Flames
    SPELL_FLAMES_PERIODIC_TRIGGER               = 64561, // Added in creature_template_addon
    SPELL_SUMMON_FLAMES_SPREAD_TRIGGER          = 64562,
    SPELL_SUMMON_FLAMES_INITIAL                 = 64563,
    SPELL_SUMMON_FLAMES_SPREAD                  = 64564,
    SPELL_FLAMES                                = 64566,
    SPELL_SCRIPT_EFFECT_SUMMON_FLAMES_INITIAL   = 64567,

    // Frost Bomb
    SPELL_SCRIPT_EFFECT_FROST_BOMB              = 64623, // Cast by VX-001
    SPELL_FROST_BOMB_LINKED                     = 64624, // Added in creature_template_addon
    SPELL_FROST_BOMB_DUMMY                      = 64625,
    SPELL_SUMMON_FROST_BOMB                     = 64627, // Cast by VX-001
    SPELL_FROST_BOMB_EXPLOSION                  = 64626,
    SPELL_CLEAR_FIRES                           = 65354,

    // Bots
    SPELL_SUMMON_FIRE_BOT                       = 64622,
    SPELL_SUMMON_FIRE_BOT_DUMMY                 = 64621,
    SPELL_SUMMON_FIRE_BOT_TRIGGER               = 64620, // Cast by Areal Command Unit
    SPELL_DEAFENING_SIREN                       = 64616, // Added in creature_template_addon
    SPELL_FIRE_SEARCH_AURA                      = 64617, // Added in creature_template_addon
    SPELL_FIRE_SEARCH                           = 64618,
    SPELL_WATER_SPRAY                           = 64619,

    SPELL_SUMMON_JUNK_BOT                       = 63819,
    SPELL_SUMMON_JUNK_BOT_TRIGGER               = 63820, // Cast by Areal Command Unit
    SPELL_SUMMON_JUNK_BOT_DUMMY                 = 64398,

    SPELL_SUMMON_ASSAULT_BOT_TRIGGER            = 64425, // Cast by Areal Command Unit
    SPELL_SUMMON_ASSAULT_BOT_DUMMY              = 64426,
    SPELL_SUMMON_ASSAULT_BOT                    = 64427,
    SPELL_MAGNETIC_FIELD                        = 64668,

    SPELL_SUMMON_BOMB_BOT                       = 63811, // Cast by Areal Command Unit
    SPELL_BOMB_BOT_AURA                         = 63767, // Added in creature_template_addon

    // Miscellaneous
    SPELL_SELF_DESTRUCTION_AURA                 = 64610,
    SPELL_SELF_DESTRUCTION_VISUAL               = 64613,
    SPELL_NOT_SO_FRIENDLY_FIRE                  = 65040,
    SPELL_ELEVATOR_KNOCKBACK                    = 65096, // Cast by worldtrigger.
    SPELL_VEHICLE_DAMAGED                       = 63415,
    SPELL_EMERGENCY_MODE                        = 64582, // mkii, vx001, aerial, assault, junk
    SPELL_EMERGENCY_MODE_TURRET                 = 65101, // Cast by Leviathan MK II, only hits Leviathan MK II turret
    SPELL_BERSERK                               = 64238, // Cast on the machines when the enrage timer expires
    SPELL_SELF_REPAIR                           = 64383,
    SPELL_MAGNETIC_CORE                         = 64436,
    SPELL_MAGNETIC_CORE_VISUAL                  = 64438,
    SPELL_HALF_HEAL                             = 64188,
    SPELL_CLEAR_ALL_DEBUFFS                     = 34098, /// @todo: make use of this spell...
    SPELL_FREEZE_ANIM_STUN                      = 63354, // used to prevent mkii from doing stuff?..
    SPELL_FREEZE_ANIM                           = 16245  // Idle aura. Freezes animation.
};

enum MimironData
{
    DATA_SETUP_MINE,
    DATA_SETUP_BOMB,
    DATA_SETUP_ROCKET,
    DATA_NOT_SO_FRIENDLY_FIRE,
    DATA_FIREFIGHTER,
    DATA_WATERSPRAY,
    DATA_MOVE_NEW,
    DATA_BERSERK
};

enum MimironEvents
{
    // Leviathan MK II
    EVENT_PROXIMITY_MINE = 1,
    EVENT_NAPALM_SHELL,
    EVENT_PLASMA_BLAST,
    EVENT_SHOCK_BLAST,
    EVENT_FLAME_SUPPRESSANT_MK,
    EVENT_MOVE_POINT_2,
    EVENT_MOVE_POINT_3,
    EVENT_MOVE_POINT_5,

    // VX-001
    EVENT_RAPID_BURST,
    EVENT_SPINNING_UP,
    EVENT_ROCKET_STRIKE,
    EVENT_HAND_PULSE,
    EVENT_FROST_BOMB,
    EVENT_FLAME_SUPPRESSANT_VX,
    EVENT_RELOAD,

    // Aerial Command Unit
    EVENT_SUMMON_FIRE_BOTS,
    EVENT_SUMMON_JUNK_BOT,
    EVENT_SUMMON_ASSAULT_BOT,
    EVENT_SUMMON_BOMB_BOT,

    // Mimiron
    EVENT_SUMMON_FLAMES,
    EVENT_INTRO_1,
    EVENT_INTRO_2,
    EVENT_INTRO_3,

    EVENT_VX001_ACTIVATION_1,
    EVENT_VX001_ACTIVATION_2,
    EVENT_VX001_ACTIVATION_3,
    EVENT_VX001_ACTIVATION_4,
    EVENT_VX001_ACTIVATION_5,
    EVENT_VX001_ACTIVATION_6,
    EVENT_VX001_ACTIVATION_7,
    EVENT_VX001_ACTIVATION_8,
    EVENT_VX001_ACTIVATION_9,

    EVENT_AERIAL_ACTIVATION_1,
    EVENT_AERIAL_ACTIVATION_2,
    EVENT_AERIAL_ACTIVATION_3,
    EVENT_AERIAL_ACTIVATION_4,
    EVENT_AERIAL_ACTIVATION_5,
    EVENT_AERIAL_ACTIVATION_6,

    EVENT_VOL7RON_ACTIVATION_1,
    EVENT_VOL7RON_ACTIVATION_2,
    EVENT_VOL7RON_ACTIVATION_3,
    EVENT_VOL7RON_ACTIVATION_4,
    EVENT_VOL7RON_ACTIVATION_5,
    EVENT_VOL7RON_ACTIVATION_6,
    EVENT_VOL7RON_ACTIVATION_7,

    EVENT_OUTTRO_1,
    EVENT_OUTTRO_2,
    EVENT_OUTTRO_3,

    // Computer
    EVENT_SELF_DESTRUCT_10,
    EVENT_SELF_DESTRUCT_9,
    EVENT_SELF_DESTRUCT_8,
    EVENT_SELF_DESTRUCT_7,
    EVENT_SELF_DESTRUCT_6,
    EVENT_SELF_DESTRUCT_5,
    EVENT_SELF_DESTRUCT_4,
    EVENT_SELF_DESTRUCT_3,
    EVENT_SELF_DESTRUCT_2,
    EVENT_SELF_DESTRUCT_1,
    EVENT_SELF_DESTRUCT_FINALIZED,

    // Misc
    EVENT_MAGNETIC_FIELD,
    EVENT_SPREAD_FLAMES,
    EVENT_FROST_BOMB_EXPLOSION,
    EVENT_FROST_BOMB_CLEAR_FIRES,
    EVENT_PROXIMITY_MINE_ARM,
    EVENT_PROXIMITY_MINE_DETONATION,
    EVENT_SEARCH_FLAMES,
    EVENT_WATER_SPRAY,

    EVENT_CHECK_PLAYERS,
    EVENT_BERSERK
};

enum MimironActions
{
    DO_START_MKII,
    DO_HARDMODE_MKII,

    DO_ACTIVATE_VX001,
    DO_START_VX001,
    DO_HARDMODE_VX001,

    DO_ACTIVATE_AERIAL,
    DO_START_AERIAL,
    DO_HARDMODE_AERIAL,
    DO_DISABLE_AERIAL,
    DO_ENABLE_AERIAL,

    DO_ACTIVATE_V0L7R0N_1,
    DO_ACTIVATE_V0L7R0N_2,
    DO_ASSEMBLED_COMBAT, // All 3 parts use this action, its done on purpose.

    DO_ACTIVATE_HARD_MODE,
    DO_ACTIVATE_COMPUTER,
    DO_DEACTIVATE_COMPUTER,
    DO_ACTIVATE_SELF_DESTRUCT,

    DO_ENCOUNTER_DONE,
    DO_RESET_MKII
};

enum MimironPhases
{
    PHASE_LEVIATHAN_MK_II = 1,
    PHASE_VX_001,
    PHASE_AERIAL_COMMAND_UNIT,
    PHASE_VOL7RON
};

enum MimironWaypoints
{
    WP_MKII_P1_IDLE = 1,
    WP_MKII_P4_POS_1,
    WP_MKII_P4_POS_2,
    WP_MKII_P4_POS_3,
    WP_MKII_P4_POS_4,
    WP_MKII_P4_POS_5,
    WP_AERIAL_P4_POS
};

enum MimironAerialPoints
{
    POINT_AERIAL_CHASE = 1, // reposition near the ranged tank when it drifts out of range
    POINT_AERIAL_GROUND, // pulled to the ground by Magnetic Core
    POINT_AERIAL_AIR // ascend back into the air afterwards
};

enum MimironSeatIds : int8
{
    MKII_SEAT_CANNON  = 3,
    ROCKET_SEAT_LEFT  = 5,
    ROCKET_SEAT_RIGHT = 6
};

uint32 const MimironRepairSpells[4] =
{
    SPELL_SEAT_1,
    SPELL_SEAT_2,
    SPELL_SEAT_3,
    SPELL_SEAT_5
};

Position const MimironVehicleRelocation[] =
{
    { 0.0f, 0.0f, 0.0f},
    { 2792.070f, 2596.320f, 364.3136f }, // WP_MKII_P1_IDLE
    { 2765.945f, 2571.095f, 364.0636f }, // WP_MKII_P4_POS_1
    { 2768.195f, 2573.095f, 364.0636f }, // WP_MKII_P4_POS_2
    { 2763.820f, 2568.870f, 364.3136f }, // WP_MKII_P4_POS_3
    { 2761.215f, 2568.875f, 364.0636f }, // WP_MKII_P4_POS_4
    { 2744.610f, 2569.380f, 364.3136f }, // WP_MKII_P4_POS_5
    { 2744.65f,  2569.46f,  381.34f   }  // WP_AERIAL_P4_POS
};

Position const VX001SummonPos = { 2744.431f, 2569.385f, 364.3968f, 3.141593f };
Position const ACUSummonPos   = { 2744.650f, 2569.460f, 380.0000f, 3.141593f };

float const MKII_FACING_VX001_ACTIVATION = 3.686f; // MK-II orientation as it reveals the VX-001
uint32 const MODEL_INVISIBLE = 11686;              // invisible display model

static bool MimironIsEncounterFinished(Unit* who)
{
    InstanceScript* instance = who->GetInstanceScript();

    Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II);
    Creature* vx001 = instance->GetCreature(DATA_VX_001);
    Creature* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT);
    if (!mkii || !vx001 || !aerial)
        return false;

    if (mkii->GetStandState() == UNIT_STAND_STATE_DEAD &&
        vx001->GetStandState() == UNIT_STAND_STATE_DEAD &&
        aerial->GetStandState() == UNIT_STAND_STATE_DEAD)
    {
        if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
            mimiron->AI()->JustDied(who);
        Unit::Kill(who, mkii);
        Unit::Kill(who, vx001);
        Unit::Kill(who, aerial);
        mkii->DespawnOrUnsummon(120s);
        vx001->DespawnOrUnsummon(120s);
        aerial->DespawnOrUnsummon(120s);
        return true;
    }
    return false;
}

static void MimironApplyBerserkIfActive(Creature* machine)
{
    if (InstanceScript* instance = machine->GetInstanceScript())
        if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
            if (mimiron->AI()->GetData(DATA_BERSERK))
                machine->CastSpell(machine, SPELL_BERSERK, true);
}

struct boss_mimiron : public BossAI
{
    boss_mimiron(Creature* creature) : BossAI(creature, DATA_MIMIRON)
    {
        me->SetReactState(REACT_PASSIVE);
        _fireFighter = false;
        _berserk = false;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case DO_ACTIVATE_VX001:
                events.ScheduleEvent(EVENT_VX001_ACTIVATION_1, 1s);
                break;
            case DO_ACTIVATE_AERIAL:
                events.ScheduleEvent(EVENT_AERIAL_ACTIVATION_1, 5s);
                break;
            case DO_ACTIVATE_V0L7R0N_1:
                Talk(SAY_AERIAL_DEATH);
                if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                    mkii->GetMotionMaster()->MovePoint(WP_MKII_P4_POS_1, MimironVehicleRelocation[WP_MKII_P4_POS_1]);
                break;
            case DO_ACTIVATE_V0L7R0N_2:
                events.ScheduleEvent(EVENT_VOL7RON_ACTIVATION_1, 1s);
                break;
            case DO_ACTIVATE_HARD_MODE:
                _fireFighter = true;
                DoZoneInCombat(me);
                break;
            default:
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        if (type == DATA_BERSERK)
            return _berserk ? 1 : 0;
        return 0;
    }

    void JustEngagedWith(Unit* who) override
    {
        if (!me->GetVehicleBase())
            return;

        BossAI::JustEngagedWith(who);
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
        me->RemoveAurasDueToSpell(SPELL_WELD);
        DoCast(me->GetVehicleBase(), SPELL_SEAT_6);

        if (GameObject* button = instance->GetGameObject(DATA_MIMIRON_BUTTON))
            button->SetFlag(GO_FLAG_NOT_SELECTABLE);

        if (_fireFighter)
            events.ScheduleEvent(EVENT_SUMMON_FLAMES, 3s);
        events.ScheduleEvent(EVENT_INTRO_1, 1500ms);
        events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5s);
        events.ScheduleEvent(EVENT_BERSERK, _fireFighter ? RAID_MODE(8min, 10min) : 15min);
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (instance->GetBossState(DATA_MIMIRON) == DONE || why == EVADE_REASON_VEHICLE_EVADE)
            return;

        BossAI::EnterEvadeMode(why);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        damage = 0;
    }

    void JustDied(Unit* /*killer*/) override
    {
        instance->SetBossState(DATA_MIMIRON, DONE);
        events.Reset();
        me->CombatStop(true);
        me->SetDisableGravity(false);
        DoCast(me, SPELL_SLEEP_VISUAL_1);
        DoCastAOE(SPELL_DESPAWN_ASSAULT_BOTS);
        me->ExitVehicle();
        // ExitVehicle() offset position is not implemented, so we make up for that with MoveJump()...
        me->GetMotionMaster()->MoveJump(me->GetPositionX() + (10.f * std::cos(me->GetOrientation())), me->GetPositionY() + (10.f * std::sin(me->GetOrientation())), me->GetPositionZ(), me->GetOrientation(), 10.f, 5.f);
        events.ScheduleEvent(EVENT_OUTTRO_1, 7s);
    }

    void Reset() override
    {
        if (instance->GetBossState(DATA_MIMIRON) == DONE) // Mimiron will attempt to reset because he is not dead and will be set to friendly before despawning.
            return;

        if (Creature* vx001 = instance->GetCreature(DATA_VX_001))
            vx001->AI()->EnterEvadeMode();
        if (Creature* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT))
            aerial->AI()->EnterEvadeMode();
        if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
            mkii->AI()->DoAction(DO_RESET_MKII);

        _Reset();
        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);

        if (GameObject* elevator = instance->GetGameObject(DATA_MIMIRON_ELEVATOR))
            elevator->SetGoState(GO_STATE_ACTIVE);

        if (_fireFighter)
            if (Creature* computer = instance->GetCreature(DATA_COMPUTER))
                computer->AI()->DoAction(DO_DEACTIVATE_COMPUTER);

        if (GameObject* button = instance->GetGameObject(DATA_MIMIRON_BUTTON))
        {
            button->SetGoState(GO_STATE_READY);
            button->RemoveFlag(GO_FLAG_NOT_SELECTABLE);
        }

        _fireFighter = false;
        _berserk = false;
        DoCast(me, SPELL_WELD);

        if (Unit* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
            DoCast(mkii, SPELL_SEAT_3);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim() && instance->GetBossState(DATA_MIMIRON) != DONE)
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SUMMON_FLAMES:
                    if (Creature* worldtrigger = instance->GetCreature(DATA_MIMIRON_WORLD_TRIGGER))
                        worldtrigger->CastSpell(nullptr, SPELL_SCRIPT_EFFECT_SUMMON_FLAMES_INITIAL, CastSpellExtraArgs(me->GetGUID()).AddSpellMod(SPELLVALUE_MAX_TARGETS, 3));
                    events.RescheduleEvent(EVENT_SUMMON_FLAMES, 28s);
                    break;
                case EVENT_INTRO_1:
                    Talk(_fireFighter ? SAY_HARDMODE_ON : SAY_MKII_ACTIVATE);
                    events.ScheduleEvent(EVENT_INTRO_2, 5s);
                    break;
                case EVENT_INTRO_2:
                    if (Unit* mkii = me->GetVehicleBase())
                    {
                        DoCast(mkii, SPELL_SEAT_7);
                        mkii->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                        mkii->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                        events.ScheduleEvent(EVENT_INTRO_3, 2s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_INTRO_3:
                    if (Creature* mkii = me->GetVehicleCreatureBase())
                        mkii->AI()->DoAction(_fireFighter ? DO_HARDMODE_MKII : DO_START_MKII);
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VX001_ACTIVATION_1:
                    if (Unit* mkii = me->GetVehicleBase())
                    {
                        mkii->SetFacingTo(MKII_FACING_VX001_ACTIVATION);
                        events.ScheduleEvent(EVENT_VX001_ACTIVATION_2, 1s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VX001_ACTIVATION_2:
                    if (Unit* mkii = me->GetVehicleBase())
                    {
                        DoCast(mkii, SPELL_SEAT_6);
                        events.ScheduleEvent(EVENT_VX001_ACTIVATION_3, 1s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VX001_ACTIVATION_3:
                    Talk(SAY_MKII_DEATH);
                    events.ScheduleEvent(EVENT_VX001_ACTIVATION_4, 5s);
                    break;
                case EVENT_VX001_ACTIVATION_4:
                    if (GameObject* elevator = instance->GetGameObject(DATA_MIMIRON_ELEVATOR))
                        elevator->SetGoState(GO_STATE_READY);
                    if (Creature* worldtrigger = instance->GetCreature(DATA_MIMIRON_WORLD_TRIGGER))
                        worldtrigger->CastSpell(worldtrigger, SPELL_ELEVATOR_KNOCKBACK);
                    events.ScheduleEvent(EVENT_VX001_ACTIVATION_5, 6s);
                    break;
                case EVENT_VX001_ACTIVATION_5:
                    if (GameObject* elevator = instance->GetGameObject(DATA_MIMIRON_ELEVATOR))
                        elevator->SetGoState(GO_STATE_DESTROYED);
                    if (Creature* vx001 = me->SummonCreature(NPC_VX_001, VX001SummonPos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120s))
                        vx001->CastSpell(vx001, SPELL_FREEZE_ANIM);
                    events.ScheduleEvent(EVENT_VX001_ACTIVATION_6, 19s);
                    break;
                case EVENT_VX001_ACTIVATION_6:
                    if (Unit* vx001 = instance->GetCreature(DATA_VX_001))
                    {
                        DoCast(vx001, SPELL_SEAT_1);
                        events.ScheduleEvent(EVENT_VX001_ACTIVATION_7, 3500ms);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VX001_ACTIVATION_7:
                    Talk(SAY_VX001_ACTIVATE);
                    events.ScheduleEvent(EVENT_VX001_ACTIVATION_8, 4s);
                    break;
                case EVENT_VX001_ACTIVATION_8:
                    if (Unit* vx001 = me->GetVehicleBase())
                    {
                        DoCast(vx001, SPELL_SEAT_2);
                        events.ScheduleEvent(EVENT_VX001_ACTIVATION_9, 3s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VX001_ACTIVATION_9:
                    if (Creature* vx001 = me->GetVehicleCreatureBase())
                        vx001->AI()->DoAction(_fireFighter ? DO_HARDMODE_VX001 : DO_START_VX001);
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_AERIAL_ACTIVATION_1:
                    if (Unit* mkii = me->GetVehicleBase())
                    {
                        DoCast(mkii, SPELL_SEAT_5);
                        events.ScheduleEvent(EVENT_AERIAL_ACTIVATION_2, 2500ms);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_AERIAL_ACTIVATION_2:
                    Talk(SAY_VX001_DEATH);
                    events.ScheduleEvent(EVENT_AERIAL_ACTIVATION_3, 5s);
                    break;
                case EVENT_AERIAL_ACTIVATION_3:
                    me->SummonCreature(NPC_AERIAL_COMMAND_UNIT, ACUSummonPos, TEMPSUMMON_MANUAL_DESPAWN);
                    events.ScheduleEvent(EVENT_AERIAL_ACTIVATION_4, 5s);
                    break;
                case EVENT_AERIAL_ACTIVATION_4:
                    if (Unit* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT))
                    {
                        me->CastSpell(aerial, SPELL_SEAT_1);
                        events.ScheduleEvent(EVENT_AERIAL_ACTIVATION_5, 2s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_AERIAL_ACTIVATION_5:
                    Talk(SAY_AERIAL_ACTIVATE);
                    events.ScheduleEvent(EVENT_AERIAL_ACTIVATION_6, 8s);
                    break;
                case EVENT_AERIAL_ACTIVATION_6:
                    if (Creature* acu = me->GetVehicleCreatureBase())
                        acu->AI()->DoAction(_fireFighter ? DO_HARDMODE_AERIAL : DO_START_AERIAL);
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VOL7RON_ACTIVATION_1:
                    if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                    {
                        mkii->SetFacingTo(float(M_PI));
                        events.ScheduleEvent(EVENT_VOL7RON_ACTIVATION_2, 1s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VOL7RON_ACTIVATION_2:
                {
                    Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II);
                    Creature* vx001 = instance->GetCreature(DATA_VX_001);
                    if (!mkii || !vx001)
                    {
                        events.Repeat(1s);
                        break;
                    }
                    vx001->RemoveAurasDueToSpell(SPELL_TORSO_DISABLED);
                    vx001->CastSpell(mkii, SPELL_MOUNT_MKII);
                    events.ScheduleEvent(EVENT_VOL7RON_ACTIVATION_3, 4500ms);
                    break;
                }
                case EVENT_VOL7RON_ACTIVATION_3:
                    if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                    {
                        mkii->GetMotionMaster()->MovePoint(WP_MKII_P4_POS_4, MimironVehicleRelocation[WP_MKII_P4_POS_4]);
                        events.ScheduleEvent(EVENT_VOL7RON_ACTIVATION_4, 5s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VOL7RON_ACTIVATION_4:
                {
                    Creature* vx001 = instance->GetCreature(DATA_VX_001);
                    Creature* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT);
                    if (!vx001 || !aerial)
                    {
                        events.Repeat(1s);
                        break;
                    }
                    aerial->SetDisableGravity(false);
                    aerial->CastSpell(vx001, SPELL_MOUNT_VX_001);
                    aerial->CastSpell(aerial, SPELL_HALF_HEAL);
                    events.ScheduleEvent(EVENT_VOL7RON_ACTIVATION_5, 4s);
                    break;
                }
                case EVENT_VOL7RON_ACTIVATION_5:
                    Talk(SAY_V07TRON_ACTIVATE);
                    events.ScheduleEvent(EVENT_VOL7RON_ACTIVATION_6, 3s);
                    break;
                case EVENT_VOL7RON_ACTIVATION_6:
                    if (Creature* vx001 = instance->GetCreature(DATA_VX_001))
                    {
                        DoCast(vx001, SPELL_SEAT_2);
                        events.ScheduleEvent(EVENT_VOL7RON_ACTIVATION_7, 5s);
                    }
                    else
                        events.Repeat(1s);
                    break;
                case EVENT_VOL7RON_ACTIVATION_7:
                {
                    Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II);
                    Creature* vx001 = instance->GetCreature(DATA_VX_001);
                    Creature* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT);
                    if (!mkii || !vx001 || !aerial)
                    {
                        events.Repeat(1s);
                        break;
                    }
                    mkii->AI()->DoAction(DO_ASSEMBLED_COMBAT);
                    vx001->AI()->DoAction(DO_ASSEMBLED_COMBAT);
                    aerial->AI()->DoAction(DO_ASSEMBLED_COMBAT);
                    break;
                }
                case EVENT_OUTTRO_1:
                    me->RemoveAurasDueToSpell(SPELL_SLEEP_VISUAL_1);
                    DoCast(me, SPELL_SLEEP_VISUAL_2);
                    me->SetFaction(FACTION_FRIENDLY);
                    events.ScheduleEvent(EVENT_OUTTRO_2, 3s);
                    break;
                case EVENT_OUTTRO_2:
                    Talk(SAY_V07TRON_DEATH);
                    if (_fireFighter)
                    {
                        if (Creature* computer = instance->GetCreature(DATA_COMPUTER))
                            computer->AI()->DoAction(DO_DEACTIVATE_COMPUTER);
                        me->SummonGameObject(RAID_MODE(GO_CACHE_OF_INNOVATION_FIREFIGHTER, GO_CACHE_OF_INNOVATION_FIREFIGHTER_HERO), 2744.040f, 2569.352f, 364.3135f, 3.124123f, QuaternionData(0.f, 0.f, 0.9999619f, 0.008734641f), 420_days, GO_SUMMON_TIMED_DESPAWN);
                    }
                    else
                        me->SummonGameObject(RAID_MODE(GO_CACHE_OF_INNOVATION, GO_CACHE_OF_INNOVATION_HERO), 2744.040f, 2569.352f, 364.3135f, 3.124123f, QuaternionData(0.f, 0.f, 0.9999619f, 0.008734641f), 420_days, GO_SUMMON_TIMED_DESPAWN);
                    events.ScheduleEvent(EVENT_OUTTRO_3, 11s);
                    break;
                case EVENT_OUTTRO_3:
                    DoCast(me, SPELL_TELEPORT_VISUAL);
                    me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                    me->DespawnOrUnsummon(1s); // sniffs say 6 sec after, but it doesnt matter.
                    break;
                case EVENT_CHECK_PLAYERS:
                    {
                        Map::PlayerList const &playerList = me->GetMap()->GetPlayers();
                        if (playerList.isEmpty())
                        {
                            EnterEvadeMode(EVADE_REASON_NO_HOSTILES);
                            return;
                        }
                        uint8 alive = 0;
                        for (auto i = playerList.begin(); i != playerList.end(); ++i)
                        {
                            if (Player* player = i->GetSource())
                            {
                                if (!player->IsGameMaster() && player->IsAlive() && IsInBoundary(player))
                                    ++alive;
                            }
                        }
                        if (!alive)
                            EnterEvadeMode(EVADE_REASON_NO_HOSTILES);
                        else
                        {
                            for (uint32 data : { DATA_LEVIATHAN_MK_II, DATA_VX_001, DATA_AERIAL_COMMAND_UNIT })
                                if (Creature* machine = instance->GetCreature(data))
                                    if (machine->IsAlive() && !machine->HasUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2) && !machine->IsImmuneToPC())
                                        DoZoneInCombat(machine);

                            events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5s);
                        }
                    }
                    break;
                case EVENT_BERSERK:
                    if (!_berserk)
                    {
                        _berserk = true;
                        Talk(SAY_BERSERK);
                    }

                    if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                        mkii->CastSpell(mkii, SPELL_BERSERK, true);
                    if (Creature* vx001 = instance->GetCreature(DATA_VX_001))
                        vx001->CastSpell(vx001, SPELL_BERSERK, true);
                    if (Creature* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT))
                        aerial->CastSpell(aerial, SPELL_BERSERK, true);
                    events.Repeat(30s);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
    }

private:
    bool _fireFighter;
    bool _berserk;
};

struct boss_leviathan_mk_ii : public BossAI
{
    boss_leviathan_mk_ii(Creature* creature) : BossAI(creature, DATA_MIMIRON)
    {
        _fireFighter = false;
        _setupMine = true;
        _setupBomb = true;
        _setupRocket = true;
    }

    void DamageTaken(Unit* who, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (damage >= me->GetHealth())
        {
            damage = me->GetHealth() - 1; // Let creature fall to 1 hp, but do not let it die or damage itself with SetHealth().
            if (me->GetStandState() == UNIT_STAND_STATE_DEAD)
                return;
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
            me->InterruptNonMeleeSpells(true);
            me->AttackStop();
            me->SetTarget(ObjectGuid::Empty);
            me->SetReactState(REACT_PASSIVE);

            me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            DoCastSelf(SPELL_VEHICLE_DAMAGED, true);

            if (events.IsInPhase(PHASE_LEVIATHAN_MK_II))
            {
                me->CastStop();
                if (Unit* turret = me->GetVehicleKit()->GetPassenger(MKII_SEAT_CANNON))
                    turret->KillSelf();

                me->SetSpeedRate(MOVE_RUN, 1.5f);
                me->GetMotionMaster()->MovePoint(WP_MKII_P1_IDLE, MimironVehicleRelocation[WP_MKII_P1_IDLE]);
            }
            else if (events.IsInPhase(PHASE_VOL7RON))
            {
                me->SetStandState(UNIT_STAND_STATE_DEAD);

                Unit* ref = who ? who : me;
                if (MimironIsEncounterFinished(ref))
                    return;

                me->CastStop();
                DoCast(me, SPELL_SELF_REPAIR);
            }
            events.Reset();
        }
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case DO_HARDMODE_MKII:
                _fireFighter = true;
                DoCast(me, SPELL_EMERGENCY_MODE);
                DoCastAOE(SPELL_EMERGENCY_MODE_TURRET);
                events.ScheduleEvent(EVENT_FLAME_SUPPRESSANT_MK, 1min, 0, PHASE_LEVIATHAN_MK_II);
                [[fallthrough]];
            case DO_START_MKII:
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat();
                events.SetPhase(PHASE_LEVIATHAN_MK_II);

                events.ScheduleEvent(EVENT_NAPALM_SHELL, 3s, 0, PHASE_LEVIATHAN_MK_II);
                events.ScheduleEvent(EVENT_PLASMA_BLAST, 15s, 0, PHASE_LEVIATHAN_MK_II);
                events.ScheduleEvent(EVENT_PROXIMITY_MINE, 5s);
                events.ScheduleEvent(EVENT_SHOCK_BLAST, 18s);
                MimironApplyBerserkIfActive(me);
                break;
            case DO_ASSEMBLED_COMBAT:
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                me->SetReactState(REACT_AGGRESSIVE);

                events.SetPhase(PHASE_VOL7RON);
                events.ScheduleEvent(EVENT_PROXIMITY_MINE, 15s);
                events.ScheduleEvent(EVENT_SHOCK_BLAST, 45s);
                MimironApplyBerserkIfActive(me);
                break;
            case DO_RESET_MKII:
                BossAI::EnterEvadeMode(EVADE_REASON_OTHER);
                break;
            default:
                break;
        }
    }

    uint32 GetData(uint32 type) const override
    {
        switch (type)
        {
            case DATA_SETUP_MINE:
                return _setupMine;
            case DATA_SETUP_BOMB:
                return _setupBomb;
            case DATA_SETUP_ROCKET:
                return _setupRocket;
            case DATA_FIREFIGHTER:
                return _fireFighter;
            default:
                return 0;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                mimiron->AI()->Talk(events.IsInPhase(PHASE_LEVIATHAN_MK_II) ? SAY_MKII_SLAY : SAY_V07TRON_SLAY);
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        summons.DespawnAll();
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (point)
        {
            case WP_MKII_P1_IDLE:
                me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                DoCast(me, SPELL_HALF_HEAL);

                if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                    mimiron->AI()->DoAction(DO_ACTIVATE_VX001);
                break;
            case WP_MKII_P4_POS_1:
                events.ScheduleEvent(EVENT_MOVE_POINT_2, 1ms);
                break;
            case WP_MKII_P4_POS_2:
                events.ScheduleEvent(EVENT_MOVE_POINT_3, 1ms);
                break;
            case WP_MKII_P4_POS_3:
                if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                    mimiron->AI()->DoAction(DO_ACTIVATE_V0L7R0N_2);
                break;
            case WP_MKII_P4_POS_4:
                events.ScheduleEvent(EVENT_MOVE_POINT_5, 1ms);
                break;
            default:
                break;
        }
    }

    void Reset() override
    {
        _Reset();
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
        me->SetReactState(REACT_PASSIVE);
        _fireFighter = false;
        _setupMine = true;
        _setupBomb = true;
        _setupRocket = true;
        DoCast(me, SPELL_FREEZE_ANIM);
    }

    void SetData(uint32 id, uint32 data) override
    {
        switch (id)
        {
            case DATA_SETUP_MINE:
                _setupMine = data != 0;
                break;
            case DATA_SETUP_BOMB:
                _setupBomb = data != 0;
                break;
            case DATA_SETUP_ROCKET:
                _setupRocket = data != 0;
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PROXIMITY_MINE:
                    DoCastAOE(SPELL_PROXIMITY_MINES);
                    events.RescheduleEvent(EVENT_PROXIMITY_MINE, 35s);
                    break;
                case EVENT_PLASMA_BLAST:
                    DoCastVictim(SPELL_SCRIPT_EFFECT_PLASMA_BLAST);
                    events.RescheduleEvent(EVENT_PLASMA_BLAST, 30s, 45s, 0, PHASE_LEVIATHAN_MK_II);

                    if (events.GetTimeUntilEvent(EVENT_NAPALM_SHELL) < 9s)
                        events.RescheduleEvent(EVENT_NAPALM_SHELL, 9s, 0, PHASE_LEVIATHAN_MK_II); // The actual spell is cast by the turret, we should not let it interrupt itself.
                    break;
                case EVENT_SHOCK_BLAST:
                    DoCastAOE(SPELL_SHOCK_BLAST);
                    events.RescheduleEvent(EVENT_SHOCK_BLAST, 34s, 36s);
                    break;
                case EVENT_FLAME_SUPPRESSANT_MK:
                    DoCastAOE(SPELL_FLAME_SUPPRESSANT_MK);
                    events.RescheduleEvent(EVENT_FLAME_SUPPRESSANT_MK, 60s, 0, PHASE_LEVIATHAN_MK_II);
                    break;
                case EVENT_NAPALM_SHELL:
                    DoCastAOE(SPELL_FORCE_CAST_NAPALM_SHELL);
                    events.RescheduleEvent(EVENT_NAPALM_SHELL, 6s, 15s, 0, PHASE_LEVIATHAN_MK_II);

                    if (events.GetTimeUntilEvent(EVENT_PLASMA_BLAST) < 2s)
                        events.RescheduleEvent(EVENT_PLASMA_BLAST, 2s, 0, PHASE_LEVIATHAN_MK_II);  // The actual spell is cast by the turret, we should not let it interrupt itself.
                    break;
                case EVENT_MOVE_POINT_2:
                    me->GetMotionMaster()->MovePoint(WP_MKII_P4_POS_2, MimironVehicleRelocation[WP_MKII_P4_POS_2]);
                    break;
                case EVENT_MOVE_POINT_3:
                    me->GetMotionMaster()->MovePoint(WP_MKII_P4_POS_3, MimironVehicleRelocation[WP_MKII_P4_POS_3]);
                    break;
                case EVENT_MOVE_POINT_5:
                    me->GetMotionMaster()->MovePoint(WP_MKII_P4_POS_5, MimironVehicleRelocation[WP_MKII_P4_POS_5]);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
        DoMeleeAttackIfReady();
    }

private:
    bool _fireFighter;
    bool _setupMine;
    bool _setupBomb;
    bool _setupRocket;
};

struct boss_vx_001 : public BossAI
{
    boss_vx_001(Creature* creature) : BossAI(creature, DATA_MIMIRON)
    {
        me->SetDisableGravity(true, false);
        me->SetReactState(REACT_PASSIVE);
        _fireFighter = false;
        me->SetImmuneToPC(true);
    }

    void DamageTaken(Unit* who, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (damage >= me->GetHealth())
        {
            damage = me->GetHealth() - 1; // Let creature fall to 1 hp, but do not let it die or damage itself with SetHealth().
            if (me->GetStandState() == UNIT_STAND_STATE_DEAD)
                return;
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
            me->AttackStop();
            me->InterruptNonMeleeSpells(true);
            me->SetTarget(ObjectGuid::Empty);

            me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            DoCast(me, SPELL_VEHICLE_DAMAGED, true);

            if (events.IsInPhase(PHASE_VX_001))
            {
                me->CastStop();
                me->DoNotReacquireSpellFocusTarget();
                me->SetTarget(ObjectGuid::Empty);
                me->SetFacingTo(me->GetHomePosition().GetOrientation());
                DoCast(me, SPELL_HALF_HEAL, true); // has no effect, wat
                DoCast(me, SPELL_TORSO_DISABLED);
                if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                    mimiron->AI()->DoAction(DO_ACTIVATE_AERIAL);
            }
            else if (events.IsInPhase(PHASE_VOL7RON))
            {
                me->SetStandState(UNIT_STAND_STATE_DEAD);

                Unit* ref = who ? who : me;
                if (MimironIsEncounterFinished(ref))
                    return;

                me->CastStop();
                DoCast(me, SPELL_SELF_REPAIR);
            }
            events.Reset();
        }
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case DO_HARDMODE_VX001:
                _fireFighter = true;
                DoCast(me, SPELL_EMERGENCY_MODE);
                events.ScheduleEvent(EVENT_FROST_BOMB, 1s);
                events.ScheduleEvent(EVENT_FLAME_SUPPRESSANT_VX, 6s);
                [[fallthrough]];
            case DO_START_VX001:
                me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                me->SetImmuneToPC(false);
                me->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetHover(true, false);
                me->SetAnimTier(AnimTier::Hover);
                DoCast(me, SPELL_HEAT_WAVE_AURA);

                events.SetPhase(PHASE_VX_001);
                events.ScheduleEvent(EVENT_ROCKET_STRIKE, 20s);
                events.ScheduleEvent(EVENT_SPINNING_UP, 30s, 35s);
                events.ScheduleEvent(EVENT_RAPID_BURST, 500ms, 0, PHASE_VX_001);
                MimironApplyBerserkIfActive(me);
                break;
            case DO_ASSEMBLED_COMBAT:
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetHover(true, false);
                me->SetAnimTier(AnimTier::Hover);
                me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);

                events.SetPhase(PHASE_VOL7RON);
                events.ScheduleEvent(EVENT_ROCKET_STRIKE, 20s);
                events.ScheduleEvent(EVENT_SPINNING_UP, 30s, 35s);
                events.ScheduleEvent(EVENT_HAND_PULSE, 500ms, 0, PHASE_VOL7RON);
                if (_fireFighter)
                    events.ScheduleEvent(EVENT_FROST_BOMB, 1s);
                MimironApplyBerserkIfActive(me);
                break;
            default:
                break;
        }
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        summons.DespawnAll();
    }

    void JustSummoned(Creature* summon) override
    {
        summons.Summon(summon);
        if (summon->GetEntry() == NPC_BURST_TARGET)
            summon->CastSpell(me, SPELL_RAPID_BURST_TARGET_ME);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                mimiron->AI()->Talk(events.IsInPhase(PHASE_VX_001) ? SAY_VX001_SLAY : SAY_V07TRON_SLAY);
    }

    void SpellHit(WorldObject* caster, SpellInfo const* /*spellInfo*/) override
    {
        if (caster->GetEntry() == NPC_BURST_TARGET && !me->HasUnitState(UNIT_STATE_CASTING))
            DoCast(caster->ToUnit(), SPELL_RAPID_BURST);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_RAPID_BURST:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 120, true))
                        DoCast(target, SPELL_SUMMON_BURST_TARGET);
                    events.RescheduleEvent(EVENT_RAPID_BURST, 3s, 0, PHASE_VX_001);
                    break;
                case EVENT_ROCKET_STRIKE:
                    DoCastAOE(events.IsInPhase(PHASE_VX_001) ? SPELL_ROCKET_STRIKE_SINGLE : SPELL_ROCKET_STRIKE_BOTH);
                    events.ScheduleEvent(EVENT_RELOAD, 10s);
                    events.RescheduleEvent(EVENT_ROCKET_STRIKE, 20s, 25s);
                    break;
                case EVENT_RELOAD:
                    for (int8 seat = ROCKET_SEAT_LEFT; seat <= ROCKET_SEAT_RIGHT; ++seat)
                        if (Unit* rocket = me->GetVehicleKit()->GetPassenger(seat))
                            rocket->SetDisplayId(rocket->GetNativeDisplayId());
                    break;
                case EVENT_HAND_PULSE:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 120, true))
                        DoCast(target, RAND(SPELL_HAND_PULSE_LEFT, SPELL_HAND_PULSE_RIGHT));
                    events.RescheduleEvent(EVENT_HAND_PULSE, 3s, 0, PHASE_VOL7RON);
                    break;
                case EVENT_FROST_BOMB:
                    DoCastAOE(SPELL_SCRIPT_EFFECT_FROST_BOMB);
                    events.RescheduleEvent(EVENT_FROST_BOMB, 45s);
                    break;
                case EVENT_SPINNING_UP:
                    if (DoCastAOE(SPELL_SPINNING_UP) == SpellCastResult::SPELL_CAST_OK)
                    {
                        events.DelayEvents(14s);
                        if (events.IsInPhase(PHASE_VOL7RON))
                            if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                            {
                                mkii->SetReactState(REACT_PASSIVE);
                                mkii->GetMotionMaster()->Clear(MOTION_PRIORITY_NORMAL);
                                mkii->SetTarget(ObjectGuid::Empty);
                                DoAddEvent(14s, new Trinity::Helpers::Events::GenericEvent(mkii, [](WorldObject* owner)
                                {
                                    Creature* mkii = owner->ToCreature();
                                    if (mkii->IsAlive() && mkii->GetStandState() != UNIT_STAND_STATE_DEAD)
                                    {
                                        mkii->SetReactState(REACT_AGGRESSIVE);
                                        if (mkii->GetVictim() && mkii->IsAIEnabled())
                                            mkii->AI()->AttackStart(mkii->GetVictim());
                                    }
                                    return true;
                                }));
                            }
                    }
                    events.RescheduleEvent(EVENT_SPINNING_UP, 55s, 65s);
                    break;
                case EVENT_FLAME_SUPPRESSANT_VX:
                    DoCastAOE(SPELL_FLAME_SUPPRESSANT_VX);
                    events.RescheduleEvent(EVENT_FLAME_SUPPRESSANT_VX, 10s, 12s, 0, PHASE_VX_001);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
    }

private:
    bool _fireFighter;
};

struct boss_aerial_command_unit : public BossAI
{
    boss_aerial_command_unit(Creature* creature) : BossAI(creature, DATA_MIMIRON)
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetDisableGravity(true);
        me->SetHoverHeight(15.0f);
        fireFigther = false;
        moving = false;
        magneticPull = false;
    }

    void DamageTaken(Unit* who, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (damage >= me->GetHealth())
        {
            damage = me->GetHealth() - 1; // Let creature fall to 1 hp, but do not let it die or damage itself with SetHealth().
            if (me->GetStandState() == UNIT_STAND_STATE_DEAD)
                return;
            magneticPull = false;
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
            me->SetReactState(REACT_PASSIVE);
            me->AttackStop();
            me->InterruptNonMeleeSpells(true);
            me->SetTarget(ObjectGuid::Empty);
            me->SetAnimTier(AnimTier::Ground);

            me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            DoCastSelf(SPELL_VEHICLE_DAMAGED, true);

            if (events.IsInPhase(PHASE_AERIAL_COMMAND_UNIT))
            {
                me->CombatStop(true);
                me->GetMotionMaster()->Clear(MOTION_PRIORITY_NORMAL);
                me->SetDisableGravity(true);
                DoAddEvent(1s, new Trinity::Helpers::Events::GenericEvent(me, [](WorldObject* owner)
                {
                    Creature* ownerCre = owner->ToCreature();
                    ownerCre->GetMotionMaster()->MovePoint(WP_AERIAL_P4_POS, MimironVehicleRelocation[WP_AERIAL_P4_POS], true, ACUSummonPos.GetOrientation());
                    return true;
                }));
            }
            else if (events.IsInPhase(PHASE_VOL7RON))
            {
                me->SetStandState(UNIT_STAND_STATE_DEAD);

                Unit* ref = who ? who : me;
                if (MimironIsEncounterFinished(ref))
                    return;

                me->CastStop();
                DoCast(me, SPELL_SELF_REPAIR);
            }
            events.Reset();
        }
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case DO_HARDMODE_AERIAL:
                fireFigther = true;
                DoCast(me, SPELL_EMERGENCY_MODE);
                events.ScheduleEvent(EVENT_SUMMON_FIRE_BOTS, 1s, 0, PHASE_AERIAL_COMMAND_UNIT);
                [[fallthrough]];
            case DO_START_AERIAL:
                me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                me->SetImmuneToPC(false);
                me->SetReactState(REACT_AGGRESSIVE);
                me->GetMotionMaster()->Clear(MOTION_PRIORITY_NORMAL);
                magneticPull = true;

                events.SetPhase(PHASE_AERIAL_COMMAND_UNIT);
                events.ScheduleEvent(EVENT_SUMMON_JUNK_BOT, 5s, 0, PHASE_AERIAL_COMMAND_UNIT);
                events.ScheduleEvent(EVENT_SUMMON_ASSAULT_BOT, 9s, 0, PHASE_AERIAL_COMMAND_UNIT);
                events.ScheduleEvent(EVENT_SUMMON_BOMB_BOT, 9s, 0, PHASE_AERIAL_COMMAND_UNIT);
                MimironApplyBerserkIfActive(me);
                break;
            case DO_DISABLE_AERIAL:
                me->CastStop();
                me->InterruptNonMeleeSpells(true);
                me->AttackStop();
                me->SetTarget(ObjectGuid::Empty);
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->Clear(MOTION_PRIORITY_NORMAL);
                me->SetDisableGravity(false);
                me->GetMotionMaster()->MoveFall(POINT_AERIAL_GROUND, 1.0f + me->GetFloorZ());
                events.DelayEvents(23s);
                break;
            case DO_ENABLE_AERIAL:
                if (magneticPull)
                {
                    Position air = me->GetPosition();
                    air.m_positionZ = ACUSummonPos.GetPositionZ();
                    me->GetMotionMaster()->MoveIdle();
                    me->SetDisableGravity(true);
                    me->GetMotionMaster()->MovePoint(POINT_AERIAL_AIR, air);
                }
                break;
            case DO_ASSEMBLED_COMBAT:
                events.SetPhase(PHASE_VOL7RON);
                me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->SetImmuneToAll(false);
                DoZoneInCombat();
                MimironApplyBerserkIfActive(me);
                break;
            default:
                break;
        }
    }

    void EnterEvadeMode(EvadeReason /*why*/) override
    {
        summons.DespawnAll();
    }

    void JustSummoned(Creature* summon) override
    {
        if (fireFigther && (summon->GetEntry() == NPC_ASSAULT_BOT || summon->GetEntry() == NPC_JUNK_BOT))
            summon->CastSpell(summon, SPELL_EMERGENCY_MODE);
        BossAI::JustSummoned(summon);
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                mimiron->AI()->Talk(events.IsInPhase(PHASE_AERIAL_COMMAND_UNIT) ? SAY_AERIAL_SLAY : SAY_V07TRON_SLAY);
    }

    void AttackStart(Unit* who) override
    {
        if (who)
            me->Attack(who, true);
    }

    void MovementInform(uint32 type, uint32 point) override
    {
        if (type == POINT_MOTION_TYPE && point == WP_AERIAL_P4_POS)
        {
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
            DoCastSelf(SPELL_CLEAR_ALL_DEBUFFS);

            if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                mimiron->AI()->DoAction(DO_ACTIVATE_V0L7R0N_1);
        }
        else if (type == POINT_MOTION_TYPE && point == POINT_AERIAL_CHASE)
        {
            if (me->GetVictim() && me->GetDistance(me->GetVictim()) > 30.0f)
            {
                Position pos = me->GetVictim()->GetNearPosition(10.0f, 0.0f);
                pos.m_positionZ = ACUSummonPos.GetPositionZ();
                me->GetMotionMaster()->MovePoint(POINT_AERIAL_CHASE, pos);
            }
            else
                moving = false;
        }
        else if (type == EFFECT_MOTION_TYPE && point == POINT_AERIAL_GROUND)
            me->GetMotionMaster()->MoveRotate(0, 15 * IN_MILLISECONDS, urand(0, 1) ? ROTATE_DIRECTION_LEFT : ROTATE_DIRECTION_RIGHT);
        else if (type == POINT_MOTION_TYPE && point == POINT_AERIAL_AIR)
        {
            me->SetReactState(REACT_AGGRESSIVE);
            if (me->GetVictim())
                me->SetTarget(me->GetVictim()->GetGUID());
            moving = false;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (events.IsInPhase(PHASE_AERIAL_COMMAND_UNIT) && me->HasReactState(REACT_AGGRESSIVE) && !moving && me->GetVictim() && me->GetDistance(me->GetVictim()) > 30.0f)
        {
            moving = true;
            Position pos = me->GetVictim()->GetNearPosition(10.0f, 0.0f);
            pos.m_positionZ = ACUSummonPos.GetPositionZ();
            me->GetMotionMaster()->MovePoint(POINT_AERIAL_CHASE, pos);
        }

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SUMMON_FIRE_BOTS:
                    DoCastAOE(SPELL_SUMMON_FIRE_BOT_TRIGGER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_MAX_TARGETS, 3));
                    events.RescheduleEvent(EVENT_SUMMON_FIRE_BOTS, 45s, 0, PHASE_AERIAL_COMMAND_UNIT);
                    break;
                case EVENT_SUMMON_JUNK_BOT:
                    DoCastAOE(SPELL_SUMMON_JUNK_BOT_TRIGGER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_MAX_TARGETS, 1));
                    events.RescheduleEvent(EVENT_SUMMON_JUNK_BOT, 11s, 12s, 0, PHASE_AERIAL_COMMAND_UNIT);
                    break;
                case EVENT_SUMMON_ASSAULT_BOT:
                    DoCastAOE(SPELL_SUMMON_ASSAULT_BOT_TRIGGER, CastSpellExtraArgs(TRIGGERED_FULL_MASK).AddSpellMod(SPELLVALUE_MAX_TARGETS, 1));
                    events.RescheduleEvent(EVENT_SUMMON_ASSAULT_BOT, 30s, 0, PHASE_AERIAL_COMMAND_UNIT);
                    break;
                case EVENT_SUMMON_BOMB_BOT:
                    DoCast(me, SPELL_SUMMON_BOMB_BOT);
                    events.RescheduleEvent(EVENT_SUMMON_BOMB_BOT, 15s, 20s, 0, PHASE_AERIAL_COMMAND_UNIT);
                    break;
                default:
                    break;
            }

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }
        DoSpellAttackIfReady(events.IsInPhase(PHASE_AERIAL_COMMAND_UNIT) ? SPELL_PLASMA_BALL_P1 : SPELL_PLASMA_BALL_P2);
    }

private:
    bool fireFigther;
    bool moving;
    bool magneticPull;
};

struct npc_mimiron_assault_bot : public ScriptedAI
{
    npc_mimiron_assault_bot(Creature* creature) : ScriptedAI(creature)
    {
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_MAGNETIC_FIELD, 14s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_ROOT))
        {
            if (Unit* newTarget = SelectTarget(SelectTargetMethod::MinDistance, 0, 30.0f, true))
            {
                me->GetThreatManager().ResetAllThreat();
                AttackStart(newTarget);
            }
        }

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_MAGNETIC_FIELD:
                    DoCastVictim(SPELL_MAGNETIC_FIELD);
                    events.RescheduleEvent(EVENT_MAGNETIC_FIELD, 30s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap events;
};

struct npc_mimiron_emergency_fire_bot : public ScriptedAI
{
    npc_mimiron_emergency_fire_bot(Creature* creature) : ScriptedAI(creature)
    {
        me->SetReactState(REACT_PASSIVE);
        isWaterSprayReady = true;
        moveNew = true;
    }

    uint32 GetData(uint32 id) const override
    {
        if (id == DATA_WATERSPRAY)
            return isWaterSprayReady;
        if (id == DATA_MOVE_NEW)
            return moveNew;
        return 0;
    }

    void SetData(uint32 id, uint32 data) override
    {
        if (id == DATA_WATERSPRAY)
            isWaterSprayReady = false;
        else if (id == DATA_MOVE_NEW)
            moveNew = data ? true : false;
    }

    void Reset() override
    {
        events.ScheduleEvent(EVENT_WATER_SPRAY, 7s);
        isWaterSprayReady = true;
        moveNew = true;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!isWaterSprayReady)
            events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_WATER_SPRAY:
                    isWaterSprayReady = true;
                    events.RescheduleEvent(EVENT_WATER_SPRAY, 7s, 9s);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap events;
    bool isWaterSprayReady;
    bool moveNew;
};

struct npc_mimiron_computer : public ScriptedAI
{
    npc_mimiron_computer(Creature* creature) : ScriptedAI(creature)
    {
        instance = me->GetInstanceScript();
        me->SetReactState(REACT_PASSIVE);
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case DO_ACTIVATE_COMPUTER:
                Talk(SAY_SELF_DESTRUCT_INITIATED);
                events.ScheduleEvent(EVENT_SELF_DESTRUCT_10, 3s);
                break;
            case DO_DEACTIVATE_COMPUTER:
                Talk(SAY_SELF_DESTRUCT_TERMINATED);
                me->RemoveAurasDueToSpell(SPELL_SELF_DESTRUCTION_AURA);
                me->RemoveAurasDueToSpell(SPELL_SELF_DESTRUCTION_VISUAL);
                events.Reset();
                break;
            default:
                break;
        }
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SELF_DESTRUCT_10:
                    Talk(SAY_SELF_DESTRUCT_10);
                    if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                        mimiron->AI()->DoAction(DO_ACTIVATE_HARD_MODE);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_9, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_9:
                    Talk(SAY_SELF_DESTRUCT_9);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_8, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_8:
                    Talk(SAY_SELF_DESTRUCT_8);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_7, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_7:
                    Talk(SAY_SELF_DESTRUCT_7);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_6, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_6:
                    Talk(SAY_SELF_DESTRUCT_6);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_5, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_5:
                    Talk(SAY_SELF_DESTRUCT_5);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_4, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_4:
                    Talk(SAY_SELF_DESTRUCT_4);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_3, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_3:
                    Talk(SAY_SELF_DESTRUCT_3);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_2, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_2:
                    Talk(SAY_SELF_DESTRUCT_2);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_1, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_1:
                    Talk(SAY_SELF_DESTRUCT_1);
                    events.ScheduleEvent(EVENT_SELF_DESTRUCT_FINALIZED, 60s);
                    break;
                case EVENT_SELF_DESTRUCT_FINALIZED:
                    Talk(SAY_SELF_DESTRUCT_FINALIZED);
                    if (Creature* mimiron = instance->GetCreature(DATA_MIMIRON))
                        mimiron->AI()->DoAction(DO_ACTIVATE_SELF_DESTRUCT);
                    DoCast(me, SPELL_SELF_DESTRUCTION_AURA);
                    DoCast(me, SPELL_SELF_DESTRUCTION_VISUAL);
                    break;
                default:
                    break;
            }
        }
    }

private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_mimiron_flames : public ScriptedAI
{
    npc_mimiron_flames(Creature* creature) : ScriptedAI(creature)
    {
        instance = me->GetInstanceScript();
    }

    void Reset() override // Reset is possibly more suitable for this case.
    {
        events.ScheduleEvent(EVENT_SPREAD_FLAMES, 4s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (instance->GetBossState(DATA_MIMIRON) != IN_PROGRESS)
        {
            me->DespawnOrUnsummon();
            return;
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_SPREAD_FLAMES:
                    DoCastAOE(SPELL_SUMMON_FLAMES_SPREAD_TRIGGER);
                    break;
                default:
                    break;
            }
        }
    }

private:
    InstanceScript* instance;
    EventMap events;
};

struct npc_mimiron_frost_bomb : public ScriptedAI
{
    npc_mimiron_frost_bomb(Creature* creature) : ScriptedAI(creature)
    {
    }

    void Reset() override
    {
        events.ScheduleEvent(EVENT_FROST_BOMB_EXPLOSION, 10s);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FROST_BOMB_EXPLOSION:
                    DoCastAOE(SPELL_FROST_BOMB_EXPLOSION);
                    events.ScheduleEvent(EVENT_FROST_BOMB_CLEAR_FIRES, 3s);
                    break;
                case EVENT_FROST_BOMB_CLEAR_FIRES:
                    DoCastAOE(SPELL_CLEAR_FIRES);
                    me->DespawnOrUnsummon(3s);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap events;
};

struct npc_mimiron_proximity_mine : public ScriptedAI
{
    npc_mimiron_proximity_mine(Creature* creature) : ScriptedAI(creature)
    {
    }

    void JustAppeared() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_UNINTERACTIBLE);
    }

    void Reset() override
    {
        events.ScheduleEvent(EVENT_PROXIMITY_MINE_ARM, 1500ms);
    }

    void UpdateAI(uint32 diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PROXIMITY_MINE_ARM:
                    DoCast(me, SPELL_PROXIMITY_MINE_PERIODIC_TRIGGER);
                    events.ScheduleEvent(EVENT_PROXIMITY_MINE_DETONATION, 33500ms);
                    break;
                case EVENT_PROXIMITY_MINE_DETONATION:
                    if (me->HasAura(SPELL_PROXIMITY_MINE_PERIODIC_TRIGGER))
                        DoCastAOE(SPELL_PROXIMITY_MINE_EXPLOSION);
                    me->DespawnOrUnsummon(1s);
                    break;
                default:
                    break;
            }
        }
    }

private:
    EventMap events;
};

struct npc_mimiron_magnetic_core : public ScriptedAI
{
    npc_mimiron_magnetic_core(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->SetReactState(REACT_PASSIVE);
        DoCastAOE(SPELL_MAGNETIC_CORE, true);
    }
};

struct go_mimiron_hardmode_button : public GameObjectAI
{
    go_mimiron_hardmode_button(GameObject* go) : GameObjectAI(go), instance(go->GetInstanceScript()) { }

    InstanceScript* instance;

    bool OnGossipHello(Player* /*player*/) override
    {
        if (me->HasFlag(GO_FLAG_NOT_SELECTABLE))
            return true;

        if (Creature* computer = instance->GetCreature(DATA_COMPUTER))
            computer->AI()->DoAction(DO_ACTIVATE_COMPUTER);

        me->SetGoState(GO_STATE_ACTIVE);
        me->SetFlag(GO_FLAG_NOT_SELECTABLE);
        return true;
    }
};

// 63801 - Bomb Bot
class spell_mimiron_bomb_bot : public SpellScript
{
    PrepareSpellScript(spell_mimiron_bomb_bot);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (GetHitPlayer())
            if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                    mkii->AI()->SetData(DATA_SETUP_BOMB, 0);
    }

    void HandleDespawn(SpellEffIndex /*effIndex*/)
    {
        if (Creature* target = GetHitCreature())
        {
            target->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_PACIFIED);
            target->DespawnOrUnsummon(1s);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_bomb_bot::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_bomb_bot::HandleDespawn, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// 65192 - Flame Suppressant, 65224 - Clear Fires, 65354 - Clear Fires, 64619 - Water Spray
class spell_mimiron_clear_fires : public SpellScript
{
    PrepareSpellScript(spell_mimiron_clear_fires);

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (GetHitCreature())
            GetHitCreature()->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_clear_fires::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 64463 - Despawn Assault Bots
class spell_mimiron_despawn_assault_bots : public SpellScript
{
    PrepareSpellScript(spell_mimiron_despawn_assault_bots);

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (GetHitCreature())
            GetHitCreature()->DespawnOrUnsummon();
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_despawn_assault_bots::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 64618 - Fire Search
class spell_mimiron_fire_search : public SpellScript
{
    PrepareSpellScript(spell_mimiron_fire_search);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_WATER_SPRAY });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        _noTarget = targets.empty();
        if (_noTarget)
            return;

        WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);
        targets.clear();
        targets.push_back(target);
    }

    void HandleAftercast()
    {
        if (_noTarget)
            GetCaster()->GetMotionMaster()->MoveRandom(15.0f);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (UnitAI* ai = caster->GetAI())
        {
            if (caster->GetDistance2d(GetHitUnit()) <= 15.0f && ai->GetData(DATA_WATERSPRAY))
            {
                caster->CastSpell(GetHitUnit(), SPELL_WATER_SPRAY, true);
                ai->SetData(DATA_WATERSPRAY, 0);
                ai->SetData(DATA_MOVE_NEW, 1);
            }
            else if (caster->GetAI()->GetData(DATA_MOVE_NEW))
            {
                caster->GetMotionMaster()->MoveChase(GetHitUnit());
                ai->SetData(DATA_MOVE_NEW, 0);
            }
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mimiron_fire_search::HandleAftercast);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_fire_search::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_fire_search::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }

    bool _noTarget = false;
};

// 64444 - Magnetic Core Summon
class spell_mimiron_magnetic_core_summon : public SpellScript
{
    PrepareSpellScript(spell_mimiron_magnetic_core_summon);

    SpellCastResult CheckCast()
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            if (Creature* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT))
                if (GetCaster()->IsInRange2d(aerial->GetPositionX(), aerial->GetPositionY(), 0.0f, 8.0f))
                    return SPELL_CAST_OK;

        return SPELL_FAILED_OUT_OF_RANGE;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mimiron_magnetic_core_summon::CheckCast);
    }
};

// 64436 - Magnetic Core
class spell_mimiron_magnetic_core : public SpellScript
{
    PrepareSpellScript(spell_mimiron_magnetic_core);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.clear();

        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            if (WorldObject* aerial = instance->GetCreature(DATA_AERIAL_COMMAND_UNIT))
                if (GetCaster()->IsInRange2d(aerial->GetPositionX(), aerial->GetPositionY(), 0.0f, 8.0f))
                    targets.push_back(aerial);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_magnetic_core::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

class spell_mimiron_magnetic_core_aura : public AuraScript
{
    PrepareAuraScript(spell_mimiron_magnetic_core_aura);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGNETIC_CORE_VISUAL });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* target = GetTarget()->ToCreature())
        {
            target->AI()->DoAction(DO_DISABLE_AERIAL);
            target->CastSpell(target, SPELL_MAGNETIC_CORE_VISUAL, true);
        }
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Creature* target = GetTarget()->ToCreature())
        {
            target->AI()->DoAction(DO_ENABLE_AERIAL);
            target->RemoveAurasDueToSpell(SPELL_MAGNETIC_CORE_VISUAL);
        }
    }

    void OnRemoveSelf(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (TempSummon* summ = GetTarget()->ToTempSummon())
            summ->DespawnOrUnsummon();
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_mimiron_magnetic_core_aura::OnApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mimiron_magnetic_core_aura::OnRemove, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mimiron_magnetic_core_aura::OnRemoveSelf, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 63667 - Napalm Shell
class spell_mimiron_napalm_shell : public SpellScript
{
    PrepareSpellScript(spell_mimiron_napalm_shell);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_NAPALM_SHELL });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);

        targets.remove_if(Trinity::AllWorldObjectsInRange(GetCaster(), 15.0f));

        if (!targets.empty())
            target = Trinity::Containers::SelectRandomContainerElement(targets);

        targets.clear();
        targets.push_back(target);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_NAPALM_SHELL);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_napalm_shell::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_napalm_shell::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 63274 - P3Wx2 Laser Barrage -- HACK! Core will currently not set UNIT_FIELD_CHANNEL_OBJECT automatially if the spell targets more than a single target.
class spell_mimiron_p3wx2_laser_barrage : public SpellScript
{
    PrepareSpellScript(spell_mimiron_p3wx2_laser_barrage);

    void OnHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->SetChannelObjectGuid(GetHitUnit()->GetGUID());
        if (Creature* creatureCaster = GetCaster()->ToCreature())
        {
            creatureCaster->ReleaseSpellFocus(nullptr, false);
            creatureCaster->SetSpellFocus(GetSpell(), GetHitUnit());
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_p3wx2_laser_barrage::OnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 64542 - Plasma Blast
class spell_mimiron_plasma_blast : public SpellScript
{
    PrepareSpellScript(spell_mimiron_plasma_blast);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_PLASMA_BLAST });
    }

    bool Load() override
    {
        return GetCaster()->GetVehicleKit() != nullptr;
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* caster = GetCaster()->GetVehicleKit()->GetPassenger(MKII_SEAT_CANNON))
            caster->CastSpell(GetHitUnit(), SPELL_PLASMA_BLAST);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_plasma_blast::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 66351, 63009 - Explosion
class spell_mimiron_proximity_explosion : public SpellScript
{
    PrepareSpellScript(spell_mimiron_proximity_explosion);

    void OnHit(SpellEffIndex /*effIndex*/)
    {
        if (GetHitPlayer())
            if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                if (Creature* mkII = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                    mkII->AI()->SetData(DATA_SETUP_MINE, 0);
    }

    void HandleAura(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->RemoveAurasDueToSpell(SPELL_PROXIMITY_MINE_PERIODIC_TRIGGER);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_proximity_explosion::OnHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_proximity_explosion::HandleAura, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
    }
};

// 63027 - Proximity Mines
class spell_mimiron_proximity_mines : public SpellScript
{
    PrepareSpellScript(spell_mimiron_proximity_mines);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_PROXIMITY_MINE });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        for (uint8 i = 0; i < 10; ++i)
            GetCaster()->CastSpell(GetCaster(), SPELL_SUMMON_PROXIMITY_MINE, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_proximity_mines::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 65346 - Proximity Mine
class spell_mimiron_proximity_trigger : public SpellScript
{
    PrepareSpellScript(spell_mimiron_proximity_trigger);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_PROXIMITY_MINE_EXPLOSION });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove(GetExplTargetWorldObject());

        if (targets.empty())
            FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(nullptr, SPELL_PROXIMITY_MINE_EXPLOSION, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_proximity_trigger::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHit += SpellEffectFn(spell_mimiron_proximity_trigger::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 63382 - Rapid Burst
class spell_mimiron_rapid_burst : public AuraScript
{
    PrepareAuraScript(spell_mimiron_rapid_burst);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_RAPID_BURST_LEFT, SPELL_RAPID_BURST_RIGHT });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (TempSummon* summ = GetTarget()->ToTempSummon())
            summ->DespawnOrUnsummon();
    }

    void HandleDummyTick(AuraEffect const* aurEff)
    {
        if (GetCaster())
            GetCaster()->CastSpell(GetTarget(), aurEff->GetTickNumber() % 2 == 0 ? SPELL_RAPID_BURST_RIGHT : SPELL_RAPID_BURST_LEFT, aurEff);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mimiron_rapid_burst::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mimiron_rapid_burst::HandleDummyTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 64402 - Rocket Strike, 65034 - Rocket Strike
class spell_mimiron_rocket_strike : public SpellScript
{
    PrepareSpellScript(spell_mimiron_rocket_strike);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SCRIPT_EFFECT_ROCKET_STRIKE });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        if (m_scriptSpellId == SPELL_ROCKET_STRIKE_SINGLE && GetCaster()->IsVehicle())
            if (WorldObject* target = GetCaster()->GetVehicleKit()->GetPassenger(RAND(ROCKET_SEAT_LEFT, ROCKET_SEAT_RIGHT)))
            {
                targets.clear();
                targets.push_back(target);
            }
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(nullptr, SPELL_SCRIPT_EFFECT_ROCKET_STRIKE, GetCaster()->GetGUID());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_rocket_strike::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_rocket_strike::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 63041 - Rocket Strike
class spell_mimiron_rocket_strike_damage : public SpellScript
{
    PrepareSpellScript(spell_mimiron_rocket_strike_damage);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_NOT_SO_FRIENDLY_FIRE });
    }

    void HandleAfterCast()
    {
        if (TempSummon* summ = GetCaster()->ToTempSummon())
            summ->DespawnOrUnsummon();
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (GetHitPlayer())
            if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                if (Creature* mkii = instance->GetCreature(DATA_LEVIATHAN_MK_II))
                    mkii->AI()->SetData(DATA_SETUP_ROCKET, 0);
    }

    void HandleFriendlyFire(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(nullptr, SPELL_NOT_SO_FRIENDLY_FIRE, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mimiron_rocket_strike_damage::HandleAfterCast);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_rocket_strike_damage::HandleScript, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_rocket_strike_damage::HandleFriendlyFire, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

// 63681 - Rocket Strike
class spell_mimiron_rocket_strike_target_select : public SpellScript
{
    PrepareSpellScript(spell_mimiron_rocket_strike_target_select);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_ROCKET_STRIKE });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);

        targets.remove_if(Trinity::AllWorldObjectsInRange(GetCaster(), 15.0f));

        if (!targets.empty())
            target = Trinity::Containers::SelectRandomContainerElement(targets);

        targets.clear();
        targets.push_back(target);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (InstanceScript* instance = GetCaster()->GetInstanceScript())
            GetCaster()->CastSpell(GetHitUnit(), SPELL_SUMMON_ROCKET_STRIKE, instance->GetGuidData(DATA_VX_001));
        GetCaster()->SetDisplayId(MODEL_INVISIBLE);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_rocket_strike_target_select::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_rocket_strike_target_select::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 64383 - Self Repair
class spell_mimiron_self_repair : public SpellScript
{
    PrepareSpellScript(spell_mimiron_self_repair);

    void HandleScript()
    {
        if (GetCaster()->GetAI())
            GetCaster()->GetAI()->DoAction(DO_ASSEMBLED_COMBAT);
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_mimiron_self_repair::HandleScript);
    }
};

// 63414 - Spinning Up -- HACK! Core will currently not set UNIT_FIELD_CHANNEL_OBJECT automatially if the spell targets more than a single target.
// eff0 will hit both caster and target due to hack in spellmgr.cpp, it is necessary because caster will interrupt itself if aura is not active on caster.
class spell_mimiron_spinning_up : public SpellScript
{
    PrepareSpellScript(spell_mimiron_spinning_up);

    void OnHit(SpellEffIndex /*effIndex*/)
    {
        if (GetHitUnit() != GetCaster())
        {
            GetCaster()->SetChannelObjectGuid(GetHitUnit()->GetGUID());
            if (Creature* creatureCaster = GetCaster()->ToCreature())
            {
                creatureCaster->ReleaseSpellFocus(nullptr, false);
                creatureCaster->SetSpellFocus(GetSpell(), GetHitUnit());
            }
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_spinning_up::OnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

// 64426 - Summon Scrap Bot
class spell_mimiron_summon_assault_bot : public AuraScript
{
    PrepareAuraScript(spell_mimiron_summon_assault_bot);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_ASSAULT_BOT });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (instance->GetBossState(DATA_MIMIRON) == IN_PROGRESS)
                    caster->CastSpell(caster, SPELL_SUMMON_ASSAULT_BOT, { aurEff, instance->GetGuidData(DATA_AERIAL_COMMAND_UNIT) });
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mimiron_summon_assault_bot::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 64425 - Summon Scrap Bot Trigger
class spell_mimiron_summon_assault_bot_target : public SpellScript
{
    PrepareSpellScript(spell_mimiron_summon_assault_bot_target);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_ASSAULT_BOT_DUMMY });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SUMMON_ASSAULT_BOT_DUMMY, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_summon_assault_bot_target::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 64621 - Summon Fire Bot
class spell_mimiron_summon_fire_bot : public AuraScript
{
    PrepareAuraScript(spell_mimiron_summon_fire_bot);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_FIRE_BOT });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (instance->GetBossState(DATA_MIMIRON) == IN_PROGRESS)
                    caster->CastSpell(caster, SPELL_SUMMON_FIRE_BOT, { aurEff, instance->GetGuidData(DATA_AERIAL_COMMAND_UNIT) });
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mimiron_summon_fire_bot::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 64620 - Summon Fire Bot Trigger
class spell_mimiron_summon_fire_bot_target : public SpellScript
{
    PrepareSpellScript(spell_mimiron_summon_fire_bot_target);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_FIRE_BOT_DUMMY });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SUMMON_FIRE_BOT_DUMMY, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_summon_fire_bot_target::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 64562 - Summon Flames Spread Trigger
class spell_mimiron_summon_flames_spread : public SpellScript
{
    PrepareSpellScript(spell_mimiron_summon_flames_spread);

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        // Flames must chase the closest player
        WorldObject* target = targets.front();

        for (std::list<WorldObject*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
            if (GetCaster()->GetDistance2d(*iter) < GetCaster()->GetDistance2d(target))
                target = *iter;

        targets.clear();
        targets.push_back(target);
    }

    void OnHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->SetInFront(GetHitUnit());
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_summon_flames_spread::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_summon_flames_spread::OnHit, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
    }
};

class spell_mimiron_summon_flames_spread_aura : public AuraScript
{
    PrepareAuraScript(spell_mimiron_summon_flames_spread_aura);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_FLAMES_SPREAD });
    }

    void HandleTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_FLAMES_PERIODIC_TRIGGER))
                caster->CastSpell(GetTarget(), SPELL_SUMMON_FLAMES_SPREAD, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mimiron_summon_flames_spread_aura::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 64623 - Frost Bomb
class spell_mimiron_summon_frost_bomb_target : public SpellScript
{
    PrepareSpellScript(spell_mimiron_summon_frost_bomb_target);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_FROST_BOMB });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (targets.empty())
            return;

        targets.remove_if(Trinity::AllWorldObjectsInRange(GetCaster(), 15.0f));

        if (targets.empty())
            return;

        WorldObject* target = Trinity::Containers::SelectRandomContainerElement(targets);

        targets.clear();
        targets.push_back(target);
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_SUMMON_FROST_BOMB, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mimiron_summon_frost_bomb_target::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_summon_frost_bomb_target::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

// 64398 - Summon Scrap Bot
class spell_mimiron_summon_junk_bot : public AuraScript
{
    PrepareAuraScript(spell_mimiron_summon_junk_bot);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_JUNK_BOT });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (InstanceScript* instance = caster->GetInstanceScript())
                if (instance->GetBossState(DATA_MIMIRON) == IN_PROGRESS)
                    caster->CastSpell(caster, SPELL_SUMMON_JUNK_BOT, { aurEff, instance->GetGuidData(DATA_AERIAL_COMMAND_UNIT) });
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mimiron_summon_junk_bot::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 63820 - Summon Scrap Bot Trigger
class spell_mimiron_summon_junk_bot_target : public SpellScript
{
    PrepareSpellScript(spell_mimiron_summon_junk_bot_target);

    bool Validate(SpellInfo const* /*spell*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_JUNK_BOT_DUMMY });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        GetHitUnit()->CastSpell(GetHitUnit(), SPELL_SUMMON_JUNK_BOT_DUMMY, true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mimiron_summon_junk_bot_target::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// 63339 - Weld
class spell_mimiron_weld : public AuraScript
{
    PrepareAuraScript(spell_mimiron_weld);

    void HandleTick(AuraEffect const* aurEff)
    {
        Unit* caster = GetTarget();
        if (Unit* vehicle = caster->GetVehicleBase())
        {
            if (aurEff->GetTickNumber() % 5 == 0)
                caster->CastSpell(vehicle, MimironRepairSpells[urand(0, 3)]);
            caster->SetFacingToObject(vehicle);
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mimiron_weld::HandleTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

class achievement_setup_boom : public AchievementCriteriaScript
{
    public:
        achievement_setup_boom() : AchievementCriteriaScript("achievement_setup_boom") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_SETUP_BOMB);
        }
};

class achievement_setup_mine : public AchievementCriteriaScript
{
    public:
        achievement_setup_mine() : AchievementCriteriaScript("achievement_setup_mine") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_SETUP_MINE);
        }
};

class achievement_setup_rocket : public AchievementCriteriaScript
{
    public:
        achievement_setup_rocket() : AchievementCriteriaScript("achievement_setup_rocket") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_SETUP_ROCKET);
        }
};

class achievement_firefighter : public AchievementCriteriaScript
{
    public:
        achievement_firefighter() : AchievementCriteriaScript("achievement_firefighter") { }

        bool OnCheck(Player* /*source*/, Unit* target) override
        {
            return target && target->GetAI()->GetData(DATA_FIREFIGHTER);
        }
};

void AddSC_boss_mimiron()
{
    RegisterUlduarCreatureAI(boss_aerial_command_unit);
    RegisterUlduarCreatureAI(boss_leviathan_mk_ii);
    RegisterUlduarCreatureAI(boss_mimiron);
    RegisterUlduarCreatureAI(boss_vx_001);

    RegisterUlduarCreatureAI(npc_mimiron_assault_bot);
    RegisterUlduarCreatureAI(npc_mimiron_emergency_fire_bot);
    RegisterUlduarCreatureAI(npc_mimiron_computer);
    RegisterUlduarCreatureAI(npc_mimiron_flames);
    RegisterUlduarCreatureAI(npc_mimiron_frost_bomb);
    RegisterUlduarCreatureAI(npc_mimiron_proximity_mine);
    RegisterUlduarCreatureAI(npc_mimiron_magnetic_core);

    RegisterUlduarGameObjectAI(go_mimiron_hardmode_button);

    RegisterSpellScript(spell_mimiron_bomb_bot);
    RegisterSpellScript(spell_mimiron_clear_fires);
    RegisterSpellScript(spell_mimiron_despawn_assault_bots);
    RegisterSpellScript(spell_mimiron_fire_search);
    RegisterSpellScript(spell_mimiron_magnetic_core_summon);
    RegisterSpellAndAuraScriptPair(spell_mimiron_magnetic_core, spell_mimiron_magnetic_core_aura);
    RegisterSpellScript(spell_mimiron_napalm_shell);
    RegisterSpellScript(spell_mimiron_p3wx2_laser_barrage);
    RegisterSpellScript(spell_mimiron_plasma_blast);
    RegisterSpellScript(spell_mimiron_proximity_explosion);
    RegisterSpellScript(spell_mimiron_proximity_mines);
    RegisterSpellScript(spell_mimiron_proximity_trigger);
    RegisterSpellScript(spell_mimiron_rapid_burst);
    RegisterSpellScript(spell_mimiron_rocket_strike);
    RegisterSpellScript(spell_mimiron_rocket_strike_damage);
    RegisterSpellScript(spell_mimiron_rocket_strike_target_select);
    RegisterSpellScript(spell_mimiron_self_repair);
    RegisterSpellScript(spell_mimiron_spinning_up);
    RegisterSpellScript(spell_mimiron_summon_assault_bot);
    RegisterSpellScript(spell_mimiron_summon_assault_bot_target);
    RegisterSpellScript(spell_mimiron_summon_fire_bot);
    RegisterSpellScript(spell_mimiron_summon_fire_bot_target);
    RegisterSpellAndAuraScriptPair(spell_mimiron_summon_flames_spread, spell_mimiron_summon_flames_spread_aura);
    RegisterSpellScript(spell_mimiron_summon_frost_bomb_target);
    RegisterSpellScript(spell_mimiron_summon_junk_bot);
    RegisterSpellScript(spell_mimiron_summon_junk_bot_target);
    RegisterSpellScript(spell_mimiron_weld);

    new achievement_setup_boom();
    new achievement_setup_mine();
    new achievement_setup_rocket();
    new achievement_firefighter();
}
