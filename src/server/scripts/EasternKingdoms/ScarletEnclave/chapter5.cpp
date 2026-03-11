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
#include "Creature.h"
#include "GameObject.h"
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "TemporarySummon.h"


enum mograine
{
    ENCOUNTER_DK_NUMBER               = 5,  // how many player queue to start the quest, or -
    ENCOUNTER_DK_TIMER                = 10, // *every 5 minutes. These have to be done in instance data
    ENCOUNTER_DEFENDER_NUMBER         = 30, // how many of defender
    ENCOUNTER_EARTHSHATTER_NUMBER     = 1, // how many of earthshatter
    ENCOUNTER_ABOMINATION_NUMBER      = 3,  // how many of abomination
    ENCOUNTER_BEHEMOTH_NUMBER         = 2,  // how many of behemoth
    ENCOUNTER_GHOUL_NUMBER            = 10, // how many of ghoul
    ENCOUNTER_WARRIOR_NUMBER          = 3,  // how many of warrior

    WORLD_STATE_REMAINS               = 3592,
    WORLD_STATE_COUNTDOWN             = 3603,
    WORLD_STATE_EVENT_BEGIN           = 3605,

    SAY_LIGHT_OF_DAWN01               = 0, // pre text
    SAY_LIGHT_OF_DAWN02               = 1,
    SAY_LIGHT_OF_DAWN03               = 2,
    SAY_LIGHT_OF_DAWN04               = 3, // intro
    SAY_LIGHT_OF_DAWN05               = 4,
    SAY_LIGHT_OF_DAWN06               = 5,
    SAY_LIGHT_OF_DAWN07               = 6, // During the fight - Korfax, Champion of the Light
    SAY_LIGHT_OF_DAWN08               = 7, // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN09               = 8, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN25               = 24, // After the fight
    SAY_LIGHT_OF_DAWN26               = 25, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN27               = 26, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN28               = 27, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN29               = 28, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN30               = 29, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN31               = 30, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN32               = 31, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN33               = 32, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN34               = 33, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN35               = 34, // Darion Mograine
    SAY_LIGHT_OF_DAWN36               = 35, // Darion Mograine
    SAY_LIGHT_OF_DAWN37               = 36, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN38               = 37, // Darion Mograine
    SAY_LIGHT_OF_DAWN39               = 38, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN40               = 39, // Darion Mograine
    SAY_LIGHT_OF_DAWN41               = 40, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN42               = 41, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN43               = 42, // The Lich King
    SAY_LIGHT_OF_DAWN44               = 43, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN45               = 44, // The Lich King
    SAY_LIGHT_OF_DAWN46               = 45, // The Lich King
    SAY_LIGHT_OF_DAWN47               = 46, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN48               = 47, // The Lich King
    SAY_LIGHT_OF_DAWN49               = 48, // The Lich King
    SAY_LIGHT_OF_DAWN50               = 49, // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN51               = 50, // The Lich King
    SAY_LIGHT_OF_DAWN52               = 51, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN53               = 52, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN54               = 53, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN55               = 54, // The Lich King
    SAY_LIGHT_OF_DAWN56               = 55, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN57               = 56, // The Lich King
    SAY_LIGHT_OF_DAWN58               = 57, // The Lich King
    SAY_LIGHT_OF_DAWN59               = 58, // The Lich King
    SAY_LIGHT_OF_DAWN60               = 59, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN61               = 60, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN62               = 61, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN63               = 62, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN64               = 63, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN65               = 64, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN66               = 65, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN67               = 66, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN68               = 67, // Highlord Darion Mograine

    EMOTE_LIGHT_OF_DAWN01             = 68,  // Emotes
    EMOTE_LIGHT_OF_DAWN02             = 69,
    EMOTE_LIGHT_OF_DAWN03             = 70,
    EMOTE_LIGHT_OF_DAWN04             = 71,
    EMOTE_LIGHT_OF_DAWN05             = 72,
    EMOTE_LIGHT_OF_DAWN06             = 73,
    EMOTE_LIGHT_OF_DAWN07             = 74,
    EMOTE_LIGHT_OF_DAWN08             = 75,
    EMOTE_LIGHT_OF_DAWN09             = 76,
    EMOTE_LIGHT_OF_DAWN10             = 77,
    EMOTE_LIGHT_OF_DAWN11             = 78,
    EMOTE_LIGHT_OF_DAWN12             = 79,
    EMOTE_LIGHT_OF_DAWN13             = 80,
    EMOTE_LIGHT_OF_DAWN14             = 81,
    EMOTE_LIGHT_OF_DAWN15             = 82,
    EMOTE_LIGHT_OF_DAWN16             = 83,
    EMOTE_LIGHT_OF_DAWN17             = 84,
    EMOTE_LIGHT_OF_DAWN18             = 85,

    GO_LIGHT_OF_DAWN                  = 191330,
    SPELL_THE_LIGHT_OF_DAWN_Q         = 53606, // quest credit

    // ---- Dark Knight npc --------------------
    // Highlord Darion Mograine
    NPC_HIGHLORD_DARION_MOGRAINE      = 29173,
    SPELL_ANTI_MAGIC_ZONE1            = 52893,
    SPELL_DEATH_STRIKE                = 53639,
    SPELL_DEATH_EMBRACE               = 53635,
    SPELL_ICY_TOUCH1                  = 49723,
    SPELL_THE_LIGHT_OF_DAWN           = 53658,
    SPELL_THE_MIGHT_OF_MOGRAINE       = 53642, // on players when begins
    SPELL_UNHOLY_BLIGHT               = 53640,
    SPELL_ALEXANDROS_MOGRAINE_SPAWN   = 53667,
    SPELL_MOGRAINE_CHARGE             = 53679,
    SPELL_ASHBRINGER                  = 53701,

    // Koltira Deathweaver & Orbaz Bloodbane are using the same abilities
    NPC_KOLTIRA_DEATHWEAVER           = 29199,
    NPC_ORBAZ_BLOODBANE               = 29204, // this guy fleed
    NPC_THASSARIAN                    = 29200, // he also does SPELL_THE_LIGHT_OF_DAWN 53658
    SPELL_BLOOD_STRIKE1               = 52374,
    SPELL_DEATH_GRIP                  = 49576,
    SPELL_ICY_TOUCH2                  = 52372,
    SPELL_PLAGUE_STRIKE1              = 50668,
    // all do SPELL_HERO_AGGRO_AURA    53627

    // Lich King
    NPC_THE_LICH_KING                 = 29183, // show up at end
    SPELL_APOCALYPSE                  = 53210,
    SPELL_TELEPORT_VISUAL             = 52233,
    SPELL_SOUL_FEAST_ALEX             = 53677, // on Alexandros
    SPELL_SOUL_FEAST_TIRION           = 53685, // on Tirion
    SPELL_ICEBOUND_VISAGE             = 53274, // not sure what is it for
    SPELL_REBUKE                      = 53680,

    // others
    NPC_RAMPAGING_ABOMINATION         = 29186,
    SPELL_CLEAVE1                     = 53633,
    SPELL_SCOURGE_HOOK                = 50335,
    SPELL_SCOURGE_AGGRO_AURA          = 53624,

    NPC_FLESH_BEHEMOTH                = 29190, // giant guy
    SPELL_STOMP                       = 53634,
    SPELL_THUNDERCLAP                 = 36706,
    SPELL_HERO_AGGRO_AURA             = 53627,

    NPC_ACHERUS_GHOUL                 = 29219, // just ghoul....
    SPELL_GHOULPLOSION                = 53632,

    NPC_WARRIOR_OF_THE_FROZEN_WASTES  = 29206, // use SPELL_CLEAVE 53631

    NPC_HIGHLORD_ALEXANDROS_MOGRAINE  = 29227, // ghost
    NPC_DARION_MOGRAINE               = 29228, // ghost

    // ---- Dawn npc --------------------
    // Highlord Tirion Fordring
    NPC_HIGHLORD_TIRION_FORDRING      = 29175,
    EQUIP_HIGHLORD_TIRION_FORDRING    = 13262,
    SPELL_LAY_ON_HANDS                = 53778,
    SPELL_REBIRTH_OF_THE_ASHBRINGER   = 53702,
    SPELL_TIRION_CHARGE               = 53705,
    SPELL_TIRION_CHARGE_VISUAL        = 53706,

    // others
    NPC_KORFAX_CHAMPION_OF_THE_LIGHT  = 29176,
    SPELL_CLEAVE                      = 53631,
    SPELL_HEROIC_LEAP                 = 53625,

    NPC_LORD_MAXWELL_TYROSUS          = 29178,
    NPC_LEONID_BARTHALOMEW_THE_REVERED = 29179,
    NPC_DUKE_NICHOLAS_ZVERENHOFF      = 29180,

    NPC_COMMANDER_ELIGOR_DAWNBRINGER  = 29177,
    SPELL_HOLY_LIGHT2                 = 37979,

    NPC_RAYNE                         = 29181,
    SPELL_REJUVENATION                = 20664,
    SPELL_STARFALL                    = 20678,
    SPELL_TRANQUILITY                 = 25817,
    SPELL_WRATH                       = 21807,

    NPC_DEFENDER_OF_THE_LIGHT         = 29174, // also does SPELL_HEROIC_LEAP 53625
    SPELL_HOLY_LIGHT1                 = 29427,
    SPELL_HOLY_STRIKE                 = 53643,
    SPELL_HOLY_WRATH                  = 53638,
    SPELL_UPPERCUT                    = 53629,

    NPC_RIMBLAT_EARTHSHATTER          = 29182,
    SPELL_CHAIN_HEAL                  = 33642,
    SPELL_THUNDER                     = 53630,

    ACTION_START_BATTLE               = 1
};

void UpdateWorldState(Map* map, uint32 id, uint32 state)
{
    Map::PlayerList const& players = map->GetPlayers();

    if (!players.isEmpty())
    {
        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* player = itr->GetSource())
                player->SendUpdateWorldState(id, state);
        }
    }
}

Position const LightofDawnLoc[] =
{
    {2281.335f, -5300.409f, 85.170f, 0},     // 0 Tirion Fordring loc
    {2283.896f, -5287.914f, 83.066f, 1.55f}, // 1 Tirion Fordring loc2
    {2281.461f, -5263.014f, 81.164f, 0},     // 2 Tirion charges
    {2262.277f, -5293.477f, 82.167f, 0},     // 3 Tirion run
    {2270.286f, -5287.73f, 82.262f, 0},      // 4 Tirion relocate
    {2269.511f, -5288.289f, 82.225f, 0},     // 5 Tirion forward
    {2262.277f, -5293.477f, 82.167f, 0},     // 6 Tirion runs to Darion
    {2270.286f, -5287.73f, 82.262f, 0},
    {2269.511f, -5288.289f, 82.225f, 0},
    {2273.205f, -5288.848f, 82.617f, 0},     // 9 Korfax loc1
    {2274.739f, -5287.926f, 82.684f, 0},     // 10 Korfax loc2
    {2253.673f, -5318.004f, 81.724f, 0},     // 11 Korfax kicked
    {2287.028f, -5309.644f, 87.253f, 0},     // 12 Maxwell loc1
    {2286.978f, -5308.025f, 86.83f, 0},      // 13 Maxwell loc2
    {2248.877f, -5307.586f, 82.166f, 0},     // 14 maxwell kicked
    {2278.58f, -5316.933f, 88.319f, 0},      // 15 Eligor loc1
    {2278.535f, -5315.479f, 88.08f, 0},      // 16 Eligor loc2
    {2259.416f, -5304.505f, 82.149f, 0},     // 17 eligor kicked
    {2289.259f, -5280.355f, 82.112f, 0},     // 18 Koltira loc1
    {2289.02f, -5281.985f, 82.207f, 0},      // 19 Koltira loc2
    {2273.289f, -5273.675f, 81.701f, 0},     // 20 Thassarian loc1
    {2273.332f, -5275.544f, 81.849f, 0},     // 21 Thassarian loc2
    {2281.198f, -5257.397f, 80.224f, 4.66f}, // 22 Alexandros loc1
    {2281.156f, -5259.934f, 80.647f, 0},     // 23 Alexandros loc2
    {2281.294f, -5281.895f, 82.445f, 1.35f}, // 24 Darion loc1
    {2281.093f, -5263.013f, 81.125f, 0},     // 25 Darion loc1
    {2281.313f, -5250.282f, 79.322f, 4.69f}, // 26 Lich King spawns
    {2281.523f, -5261.058f, 80.877f, 0},     // 27 Lich king move forwards
    {2272.709f, -5255.552f, 78.226f, 0},     // 28 Lich king kicked
    {2273.972f, -5257.676f, 78.862f, 0},     // 29 Lich king moves forward
};

static constexpr uint32 PATH_ESCORT_MOGRAINE = 233386;

class npc_highlord_darion_mograine : public CreatureScript
{
public:
    npc_highlord_darion_mograine() : CreatureScript("npc_highlord_darion_mograine") { }

    struct npc_highlord_darion_mograineAI : public EscortAI
    {
        npc_highlord_darion_mograineAI(Creature* creature) : EscortAI(creature)
        {
            SetDespawnAtFar(false);
            SetDespawnAtEnd(false);
        }

        void Reset() override
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
            {
                inBattle = false;
                step = 0;
                phaseTimer = 3000;
                fightDuration = 300000; // 5 minutes
                summonCounter = 0;

                antiMagicZoneTimer = urand(1000, 6000);
                deathStrikeTimer = urand(5000, 10000);
                deathEmbraceTimer = urand(5000, 10000);
                icyTouchTimer = urand(5000, 10000);
                unholyBlightTimer = urand(5000, 10000);

                fightSpeechTimer = 15000;
                spawnCheckTimer = 1000;
                targetCheckTimer = 10000;

                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->Mount(25279);
                me->SetVisible(true);
                me->SetRespawnTime(1 * MINUTE * IN_MILLISECONDS);

                UpdateWorldState(me->GetMap(), WORLD_STATE_REMAINS, 0);
                //UpdateWorldState(me->GetMap(), WORLD_STATE_COUNTDOWN, 0);
                UpdateWorldState(me->GetMap(), WORLD_STATE_EVENT_BEGIN, 0);

                if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                    temp->DespawnOrUnsummon();
                if (Creature* temp = ObjectAccessor::GetCreature(*me, korfaxGUID))
                    temp->DespawnOrUnsummon();
                if (Creature* temp = ObjectAccessor::GetCreature(*me, maxwellGUID))
                    temp->DespawnOrUnsummon();
                if (Creature* temp = ObjectAccessor::GetCreature(*me, eligorGUID))
                    temp->DespawnOrUnsummon();
                if (Creature* temp = ObjectAccessor::GetCreature(*me, rayneGUID))
                    temp->DespawnOrUnsummon();

                tirionGUID.Clear();
                korfaxGUID.Clear();
                maxwellGUID.Clear();
                eligorGUID.Clear();
                rayneGUID.Clear();

                for (uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
                {
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, defenderGUIDs[i]))
                        temp->DespawnOrUnsummon();
                    defenderGUIDs[i].Clear();
                }
                for (uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
                {
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, earthshatterGUIDs[i]))
                        temp->DespawnOrUnsummon();
                    earthshatterGUIDs[i].Clear();
                }

                if (Creature* temp = ObjectAccessor::GetCreature(*me, koltiraGUID))
                    temp->Respawn();
                if (Creature* temp = ObjectAccessor::GetCreature(*me, orbazGUID))
                    temp->Respawn();
                if (Creature* temp = ObjectAccessor::GetCreature(*me, thassarianGUID))
                    temp->Respawn();
                if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                    temp->Respawn();

                koltiraGUID.Clear();
                orbazGUID.Clear();
                thassarianGUID.Clear();
                lichKingGUID.Clear();
                for (uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                {
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, abominationGUIDs[i]))
                        temp->DespawnOrUnsummon();
                    abominationGUIDs[i].Clear();
                }
                for (uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                {
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, behemothGUIDs[i]))
                        temp->DespawnOrUnsummon();
                    behemothGUIDs[i].Clear();
                }
                for (uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                {
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, ghoulGUIDs[i]))
                        temp->DespawnOrUnsummon();
                    ghoulGUIDs[i].Clear();
                }
                for (uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                {
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, warriorGUIDs[i]))
                        temp->DespawnOrUnsummon();
                    warriorGUIDs[i].Clear();
                }
            }
        }

        void DoAction(int32 action) override
        {
            switch (action)
            {
                case ACTION_START_BATTLE:
                    step = 1;
                    Start(true);
                    break;
                default:
                    break;
            }
        }

        void WaypointReached(uint32 waypointId, uint32 /*pathId*/) override
        {
            switch (waypointId)
            {
                case 0:
                    me->SetWalk(false);
                    SetEscortPaused(true);
                    break;
                case 1:
                    SetEscortPaused(true);
                    SpawnNPC();
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, korfaxGUID))
                        temp->AI()->Talk(SAY_LIGHT_OF_DAWN07);
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, maxwellGUID))
                        temp->AI()->Talk(SAY_LIGHT_OF_DAWN08);

                    for (uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                        NPCChangeTarget(ghoulGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                        NPCChangeTarget(warriorGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                        NPCChangeTarget(abominationGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                        NPCChangeTarget(behemothGUIDs[i]);
                    NPCChangeTarget(koltiraGUID);
                    NPCChangeTarget(orbazGUID);
                    NPCChangeTarget(thassarianGUID);

                    me->Dismount();
                    me->CastSpell(me, SPELL_THE_MIGHT_OF_MOGRAINE, true); // need to fix, on player only

                    if (Creature* temp = ObjectAccessor::GetCreature(*me, koltiraGUID))
                        temp->Dismount();
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, thassarianGUID))
                        temp->Dismount();
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, orbazGUID))
                        temp->Dismount();

                    inBattle = true;
                    break;
                case 2:
                    me->SetWalk(true);
                    DoCast(me, SPELL_THE_LIGHT_OF_DAWN);
                    break;
                case 3:
                {
                    Talk(EMOTE_LIGHT_OF_DAWN05);
                    if (me->HasAura(SPELL_THE_LIGHT_OF_DAWN))
                        me->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, koltiraGUID))
                    {
                        if (temp->HasAura(SPELL_THE_LIGHT_OF_DAWN))
                            temp->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                        temp->SetWalk(true);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[19]);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, thassarianGUID))
                    {
                        if (temp->HasAura(SPELL_THE_LIGHT_OF_DAWN))
                            temp->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                        temp->SetWalk(true);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[21]);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, korfaxGUID))
                    {
                        temp->SetWalk(true);
                        temp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[10]);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, maxwellGUID))
                    {
                        temp->SetWalk(true);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[13]);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, eligorGUID))
                    {
                        temp->SetWalk(true);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[16]);
                    }
                    JumpToNextStep(10000);
                    break;
                }
                case 4:
                    Talk(SAY_LIGHT_OF_DAWN27);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);

                    if (Creature* temp = ObjectAccessor::GetCreature(*me, koltiraGUID))
                        temp->SetStandState(UNIT_STAND_STATE_KNEEL);
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, thassarianGUID))
                        temp->SetStandState(UNIT_STAND_STATE_KNEEL);
                    SetEscortPaused(true);
                    break;
                case 5:
                    Talk(SAY_LIGHT_OF_DAWN33);
                    SetEscortPaused(true);
                    break;
                case 6:
                    SetEscortPaused(true);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SPECIALATTACK1H);
                    JumpToNextStep(1000);
                    break;
                case 7:
                    SetEscortPaused(true);
                    JumpToNextStep(2000);
                    break;
                case 8:
                    me->SetVirtualItem(0, uint32(EQUIP_UNEQUIP));
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                        me->CastSpell(temp, SPELL_ASHBRINGER, true);
                    Talk(EMOTE_LIGHT_OF_DAWN14);
                    SetEscortPaused(true);
                    break;
                default:
                    break;
            }
        }

        void EnterEvadeMode(EvadeReason why) override
        {
            if (!inBattle) //do not reset self if we are in battle
                EscortAI::EnterEvadeMode(why);
        }

        void UpdateAI(uint32 diff) override
        {
            EscortAI::UpdateAI(diff);

            if (!inBattle)
            {
                if (phaseTimer <= diff)
                {
                    // ******* Before battle *****************************************************************
                    switch (step)
                    {
                        case 0:  // countdown
                            //UpdateWorldState(me->GetMap(), WORLD_STATE_COUNTDOWN, 1);
                            break;
                        case 1:  // just delay
                            //UpdateWorldState(me->GetMap(), WORLD_STATE_REMAINS, 1);
                            UpdateWorldState(me->GetMap(), WORLD_STATE_COUNTDOWN, 0);
                            UpdateWorldState(me->GetMap(), WORLD_STATE_EVENT_BEGIN, 1);
                            me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                            JumpToNextStep(3000);
                            break;
                        case 2:
                            Talk(SAY_LIGHT_OF_DAWN04);
                            if (Creature* pKoltira = GetClosestCreatureWithEntry(me, NPC_KOLTIRA_DEATHWEAVER, 50.0f))
                            {
                                if (!pKoltira->IsAlive())
                                    pKoltira->Respawn();
                                koltiraGUID = pKoltira->GetGUID();
                            }
                            if (Creature* pOrbaz = GetClosestCreatureWithEntry(me, NPC_ORBAZ_BLOODBANE, 50.0f))
                            {
                                if (!pOrbaz->IsAlive())
                                    pOrbaz->Respawn();
                                orbazGUID = pOrbaz->GetGUID();
                            }
                            if (Creature* pThassarian = GetClosestCreatureWithEntry(me, NPC_THASSARIAN, 50.0f))
                            {
                                if (!pThassarian->IsAlive())
                                    pThassarian->Respawn();
                                thassarianGUID = pThassarian->GetGUID();
                            }
                            JumpToNextStep(10000);
                            break;
                        case 3: // rise
                            Talk(SAY_LIGHT_OF_DAWN05);
                            JumpToNextStep(3000);
                            break;
                        case 4: // summon ghoul
                            // Dunno whats the summon spell, so workaround
                            DoCast(me, 33271); // shack effect
                            phaseTimer = 500;
                            if (summonCounter < ENCOUNTER_GHOUL_NUMBER)
                            {
                                Unit* temp = me->SummonCreature(NPC_ACHERUS_GHOUL, (me->GetPositionX() - 20) + rand32() % 40, (me->GetPositionY() - 20) + rand32() % 40, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                                temp->SetWalk(false);
                                temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                                ghoulGUIDs[summonCounter] = temp->GetGUID();
                                ++summonCounter;
                            }
                            else
                            {
                                summonCounter = 0;
                                ++step;
                            }
                            break;
                        case 5: // summon abomination
                            DoCast(me, 33271); // shack effect
                            phaseTimer = 500;
                            if (summonCounter < ENCOUNTER_ABOMINATION_NUMBER)
                            {
                                Unit* temp = me->SummonCreature(NPC_RAMPAGING_ABOMINATION, (me->GetPositionX() - 20) + rand32() % 40, (me->GetPositionY() - 20) + rand32() % 40, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                                temp->SetWalk(false);
                                temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                                abominationGUIDs[summonCounter] = temp->GetGUID();
                                ++summonCounter;
                            }
                            else
                            {
                                summonCounter = 0;
                                ++step;
                            }
                            break;
                        case 6: // summon warrior
                            DoCast(me, 33271); // shack effect
                            phaseTimer = 500;
                            if (summonCounter < ENCOUNTER_WARRIOR_NUMBER)
                            {
                                Unit* temp = me->SummonCreature(NPC_WARRIOR_OF_THE_FROZEN_WASTES, (me->GetPositionX() - 20) + rand32() % 40, (me->GetPositionY() - 20) + rand32() % 40, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                                temp->SetWalk(false);
                                temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                                warriorGUIDs[summonCounter] = temp->GetGUID();
                                ++summonCounter;
                            }
                            else
                            {
                                summonCounter = 0;
                                ++step;
                            }
                            break;
                        case 7: // summon warrior
                            DoCast(me, 33271); // shack effect
                            phaseTimer = 500;
                            if (summonCounter < ENCOUNTER_BEHEMOTH_NUMBER)
                            {
                                Unit* temp = me->SummonCreature(NPC_FLESH_BEHEMOTH, (me->GetPositionX() - 20) + rand32() % 40, (me->GetPositionY() - 20) + rand32() % 40, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                                temp->SetWalk(false);
                                temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                                behemothGUIDs[summonCounter] = temp->GetGUID();
                                ++summonCounter;
                            }
                            else
                            {
                                summonCounter = 0;
                                ++step;
                            }
                            break;
                        case 8: // summon announce
                            Talk(SAY_LIGHT_OF_DAWN06);
                            JumpToNextStep(5000);
                            break;
                        case 9: // charge begins
                            SetEscortPaused(false);
                            me->SetWalk(false);
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, koltiraGUID))
                            {
                                temp->SetWalk(false);
                                temp->GetMotionMaster()->MoveFormation(me, temp->GetDistance(me), temp->GetAbsoluteAngle(me), 0, 0);
                            }
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, orbazGUID))
                            {
                                temp->SetWalk(false);
                                temp->GetMotionMaster()->MoveFormation(me, temp->GetDistance(me), temp->GetAbsoluteAngle(me), 0, 0);
                            }
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, thassarianGUID))
                            {
                                temp->SetWalk(false);
                                temp->GetMotionMaster()->MoveFormation(me, temp->GetDistance(me), temp->GetAbsoluteAngle(me), 0, 0);
                            }
                            for (uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, abominationGUIDs[i]))
                                    temp->GetMotionMaster()->MoveFormation(me, temp->GetDistance(me), temp->GetAbsoluteAngle(me), 0, 0);
                            for (uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, behemothGUIDs[i]))
                                    temp->GetMotionMaster()->MoveFormation(me, temp->GetDistance(me), temp->GetAbsoluteAngle(me), 0, 0);
                            for (uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, ghoulGUIDs[i]))
                                    temp->GetMotionMaster()->MoveFormation(me, temp->GetDistance(me), temp->GetAbsoluteAngle(me), 0, 0);
                            for (uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, warriorGUIDs[i]))
                                    temp->GetMotionMaster()->MoveFormation(me, temp->GetDistance(me), temp->GetAbsoluteAngle(me), 0, 0);
                            JumpToNextStep(5000);
                            break;
                        // ******* After battle *****************************************************************
                        case 11: // Tirion starts to speak
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                            {
                                temp->Dismount();
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN28);
                            }
                            JumpToNextStep(21000);
                            break;
                        case 12:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN29);
                            JumpToNextStep(13000);
                            break;
                        case 13:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN30);
                            JumpToNextStep(13000);
                            break;
                        case 14:
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            Talk(SAY_LIGHT_OF_DAWN31);
                            JumpToNextStep(7000);
                            break;
                        case 15: // summon gate
                            if (Creature* temp = me->SummonCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, LightofDawnLoc[22], TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min))
                            {
                                temp->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                                temp->CastSpell(temp, SPELL_ALEXANDROS_MOGRAINE_SPAWN, true);
                                temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN06);
                                alexandrosGUID = temp->GetGUID();
                            }
                            JumpToNextStep(4000);
                            break;
                        case 16: // Alexandros out
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, alexandrosGUID))
                            {
                                temp->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[23]);
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN32);
                            }
                            SetEscortPaused(false); // makes darion turns back
                            JumpToNextStep(5000);
                            break;
                        case 17:
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            Talk(SAY_LIGHT_OF_DAWN34);
                            JumpToNextStep(5000);
                            break;
                        case 18: // Darion's spirit out
                            if (Creature* temp = me->SummonCreature(NPC_DARION_MOGRAINE, LightofDawnLoc[24], TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min))
                            {
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN35);
                                temp->SetWalk(false);
                                darionGUID = temp->GetGUID();
                            }
                            JumpToNextStep(4000);
                            break;
                        case 19: // runs to father
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, darionGUID))
                            {
                                temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN07);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[25]);
                            }
                            JumpToNextStep(4000);
                            break;
                        case 20:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, darionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN36);
                            JumpToNextStep(4000);
                            break;
                        case 21:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, darionGUID))
                                temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN08);
                            JumpToNextStep(4000);
                            break;
                        case 22:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, alexandrosGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN37);
                            JumpToNextStep(8000);
                            break;
                        case 23:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, darionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN38);
                            JumpToNextStep(8000);
                            break;
                        case 24:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, alexandrosGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN39);

                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID)) // Tirion moves forward here
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[1]);

                            JumpToNextStep(15000);
                            break;
                        case 25:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, darionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN40);
                            JumpToNextStep(11000);
                            break;
                        case 26:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, alexandrosGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN41);
                            JumpToNextStep(5000);
                            break;
                        case 27:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, darionGUID))
                                temp->KillSelf();
                            JumpToNextStep(24000);
                            break;
                        case 28:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, alexandrosGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN42);
                            JumpToNextStep(6000);
                            break;
                        case 29: // lich king spawns
                            if (Creature* temp = me->SummonCreature(NPC_THE_LICH_KING, LightofDawnLoc[26], TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min))
                            {
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN43);
                                lichKingGUID = temp->GetGUID();
                                if (Unit* pAlex = ObjectAccessor::GetCreature(*me, alexandrosGUID))
                                    temp->CastSpell(pAlex, SPELL_SOUL_FEAST_ALEX, false);
                            }
                            JumpToNextStep(2000);
                            break;
                        case 30:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, alexandrosGUID)) // just hide him
                            {
                                temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN09);
                                temp->SetVisible(false);
                            }
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            {
                                temp->InterruptNonMeleeSpells(false);
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN45);
                            }
                            JumpToNextStep(3000);
                            break;
                        case 31:
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            Talk(EMOTE_LIGHT_OF_DAWN10);
                            Talk(SAY_LIGHT_OF_DAWN44);
                            JumpToNextStep(3000);
                            break;
                        case 32:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[27]);
                            JumpToNextStep(6000);
                            break;
                        case 33: // Darion supports to jump to lich king here
                            if (ObjectAccessor::GetCreature(*me, lichKingGUID))
                                DoCast(me, SPELL_MOGRAINE_CHARGE); // jumping charge
                            // doesn't make it looks well, so workarounds, Darion charges, looks better
                            me->SetSpeedRate(MOVE_RUN, 3.0f);
                            me->SetWalk(false);
                            SetEscortPaused(false);
                            JumpToNextStep(0);
                            break;
                        case 35: // Lich king counterattacks
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            {
                                temp->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN46);
                            }
                            me->SetSpeedRate(MOVE_RUN, 6.0f);
                            me->SetStandState(UNIT_STAND_STATE_DEAD);
                            SetEscortPaused(false); // Darion got kicked by lich king
                            JumpToNextStep(0);
                            break;
                        case 37: // Lich king counterattacks
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            JumpToNextStep(3000);
                            break;
                        case 38:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN47);
                            JumpToNextStep(8000);
                            break;
                        case 39:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN48);
                            JumpToNextStep(15000);
                            break;
                        case 40:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN49);
                            JumpToNextStep(17000);
                            break;
                        case 41: // Lich king - Apocalypse
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            {
                                temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN11);
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN51);
                                if (Creature* pTirion = ObjectAccessor::GetCreature(*me, tirionGUID))
                                {
                                    pTirion->SetStandState(UNIT_STAND_STATE_KNEEL);
                                    //temp->CastSpell(pTirion, SPELL_APOCALYPSE, false); // not working
                                    temp->CastSpell(pTirion, SPELL_SOUL_FEAST_TIRION, false);
                                    pTirion->AI()->Talk(EMOTE_LIGHT_OF_DAWN12);
                                }
                            }
                            JumpToNextStep(3000);
                            break;
                        case 42: // Maxwell yells for attack
                            {
                                Position lichPosition = { 0.0f, 0.0f, 0.0f, 0.0f };
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                    lichPosition = temp->GetPosition();

                                if (lichPosition.GetPositionX() != 0.0f)
                                {
                                    if (TempSummon* temp = me->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 10), float(rand32() % 10), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10s))
                                    {
                                        temp->SetFaction(me->GetFaction());
                                        temp->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
                                        temp->GetMotionMaster()->MoveCharge(lichPosition.GetPositionX(), lichPosition.GetPositionY(), lichPosition.GetPositionZ());
                                        defenderGUIDs[0] = temp->GetGUID();
                                    }                                    
                                    if (TempSummon* temp = me->SummonCreature(NPC_RIMBLAT_EARTHSHATTER, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 10), float(rand32() % 10), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10s))
                                    {
                                        temp->SetFaction(me->GetFaction());
                                        temp->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
                                        temp->GetMotionMaster()->MoveCharge(lichPosition.GetPositionX(), lichPosition.GetPositionY(), lichPosition.GetPositionZ());
                                        earthshatterGUIDs[0] = temp->GetGUID();
                                    }
                                    if (Creature* temp = ObjectAccessor::GetCreature(*me, maxwellGUID))
                                    {
                                        temp->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
                                        temp->GetMotionMaster()->MoveCharge(lichPosition.GetPositionX(), lichPosition.GetPositionY(), lichPosition.GetPositionZ());
                                        temp->AI()->Talk(SAY_LIGHT_OF_DAWN50);
                                    }
                                    if (Creature* temp = ObjectAccessor::GetCreature(*me, korfaxGUID))
                                    {
                                        temp->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
                                        temp->HandleEmoteCommand(EMOTE_STATE_ATTACK_UNARMED);
                                        temp->GetMotionMaster()->MoveCharge(lichPosition.GetPositionX(), lichPosition.GetPositionY(), lichPosition.GetPositionZ());
                                    }
                                    if (Creature* temp = ObjectAccessor::GetCreature(*me, eligorGUID))
                                    {
                                        temp->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
                                        temp->GetMotionMaster()->MoveCharge(lichPosition.GetPositionX(), lichPosition.GetPositionY(), lichPosition.GetPositionZ());
                                    }
                                }
                            }
                            JumpToNextStep(6000);
                            break;
                        case 43: // They all got kicked
                            if (Creature* lich = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            {
                                lich->AI()->Talk(EMOTE_LIGHT_OF_DAWN13);

                                if (Creature* temp = ObjectAccessor::GetCreature(*me, maxwellGUID))
                                {
                                    temp->SetEmoteState(EMOTE_ONESHOT_NONE);
                                    temp->SetStandState(UNIT_STAND_STATE_DEAD);
                                    temp->GetMotionMaster()->MoveKnockbackFrom(lich->GetPositionX(), lich->GetPositionY(), 6.0f, 12.0f);
                                }
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, korfaxGUID))
                                {
                                    temp->SetEmoteState(EMOTE_ONESHOT_NONE);
                                    temp->SetStandState(UNIT_STAND_STATE_DEAD);
                                    temp->GetMotionMaster()->MoveKnockbackFrom(lich->GetPositionX(), lich->GetPositionY(), 6.0f, 12.0f);
                                }
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, eligorGUID))
                                {
                                    temp->SetEmoteState(EMOTE_ONESHOT_NONE);
                                    temp->SetStandState(UNIT_STAND_STATE_DEAD);
                                    temp->GetMotionMaster()->MoveKnockbackFrom(lich->GetPositionX(), lich->GetPositionY(), 6.0f, 12.0f);
                                }
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, defenderGUIDs[0]))
                                {
                                    temp->SetStandState(UNIT_STAND_STATE_DEAD);
                                    temp->GetMotionMaster()->MoveKnockbackFrom(lich->GetPositionX(), lich->GetPositionY(), 6.0f, 12.0f);
                                }
                                if (Creature* temp = ObjectAccessor::GetCreature(*me, earthshatterGUIDs[0]))
                                {
                                    temp->SetStandState(UNIT_STAND_STATE_DEAD);
                                    temp->GetMotionMaster()->MoveKnockbackFrom(lich->GetPositionX(), lich->GetPositionY(), 6.0f, 12.0f);
                                }
                            }
                            JumpToNextStep(3000);
                            break;
                        case 44:
                            JumpToNextStep(1000);
                            break;
                        case 45:
                            Talk(SAY_LIGHT_OF_DAWN52);
                            JumpToNextStep(5000);
                            break;
                        case 46: // Darion stand up, "not today"
                            me->SetSpeedRate(MOVE_RUN, 1.0f);
                            me->SetWalk(true);
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            Talk(SAY_LIGHT_OF_DAWN53);
                            SetEscortPaused(false); // Darion throws sword
                            JumpToNextStep(7000);
                            break;
                        case 47: // Ashbringer rebirth
                            me->SetStandState(UNIT_STAND_STATE_KNEEL);
                            Talk(EMOTE_LIGHT_OF_DAWN15);
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                            {
                                temp->SetStandState(UNIT_STAND_STATE_STAND);
                                temp->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_HIGHLORD_TIRION_FORDRING));
                                temp->CastSpell(temp, SPELL_REBIRTH_OF_THE_ASHBRINGER, false);
                            }
                            JumpToNextStep(1000);
                            break;
                        case 48:
                            me->SummonGameObject(GO_LIGHT_OF_DAWN, 2283.896f, -5287.914f, 83.066f, 0.f, QuaternionData(), 30s);
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                            {
                                if (temp->HasAura(SPELL_REBIRTH_OF_THE_ASHBRINGER))
                                    temp->RemoveAurasDueToSpell(SPELL_REBIRTH_OF_THE_ASHBRINGER);
                                temp->CastSpell(temp, 41542, false); // workarounds, light expoded, makes it cool
                                temp->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                            }
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->InterruptNonMeleeSpells(false);
                            JumpToNextStep(2500);
                            break;
                        case 49:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN54);
                            JumpToNextStep(4000);
                            break;
                        case 50:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN55);
                            JumpToNextStep(5000);
                            break;
                        case 51:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN56);
                            JumpToNextStep(1000);
                            break;
                        case 52: // Tiron charges
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                            {
                                temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN16);
                                temp->CastSpell(temp, SPELL_TIRION_CHARGE, false); // jumping charge
                                temp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                                temp->SetSpeedRate(MOVE_RUN, 3.0f); // workarounds, make Tirion still running
                                temp->SetWalk(false);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[2]);
                                if (Creature* lktemp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                    lktemp->Relocate(LightofDawnLoc[28]); // workarounds, he should kick back by Tirion, but here we relocate him
                            }
                            JumpToNextStep(1500);
                            break;
                        case 53:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN57);
                            JumpToNextStep(1000);
                            break;
                        case 54:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                            {
                                temp->SetSpeedRate(MOVE_RUN, 1.0f);
                                me->SetWalk(true);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[29]); // 26
                            }
                            JumpToNextStep(4000);
                            break;
                        case 55:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->SetStandState(UNIT_STAND_STATE_KNEEL);
                            JumpToNextStep(2000);
                            break;
                        case 56:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->SetStandState(UNIT_STAND_STATE_STAND);
                            JumpToNextStep(1500);
                            break;
                        case 57:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN58);
                            JumpToNextStep(10000);
                            break;
                        case 58:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN59);
                            JumpToNextStep(10000);
                            break;
                        case 59:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID))
                                temp->CastSpell(temp, SPELL_TELEPORT_VISUAL, false);
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID)) // Tirion runs to Darion
                            {
                                temp->SetEmoteState(EMOTE_ONESHOT_NONE);
                                temp->SetSpeedRate(MOVE_RUN, 1.0f);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[6]);
                            }
                            JumpToNextStep(2500);
                            break;
                        case 60:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, lichKingGUID)) // Lich king disappears here
                            {
                                temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN17);
                                temp->KillSelf();
                            }
                            JumpToNextStep(10000);
                            break;
                        case 61:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, maxwellGUID))
                            {
                                temp->SetStandState(UNIT_STAND_STATE_STAND);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[14]);
                            }
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, korfaxGUID))
                            {
                                temp->SetStandState(UNIT_STAND_STATE_STAND);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[11]);
                            }
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, eligorGUID))
                            {
                                temp->SetStandState(UNIT_STAND_STATE_STAND);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[17]);
                            }
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN60);
                            JumpToNextStep(3000);
                            break;
                        case 62:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                            {
                                temp->SetWalk(true);
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[7]);
                            }
                            JumpToNextStep(5500);
                            break;
                        case 63:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                            {
                                temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[8]);
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN61);
                            }
                            JumpToNextStep(15000);
                            break;
                        case 64:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN62);
                            JumpToNextStep(7000);
                            break;
                        case 65:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN63);
                            JumpToNextStep(10000);
                            break;
                        case 66:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN64);
                            JumpToNextStep(11000);
                            break;
                        case 67:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN65);
                            JumpToNextStep(10000);
                            break;
                        case 68:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN66);
                            JumpToNextStep(8000);
                            break;
                        case 69:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->AI()->Talk(SAY_LIGHT_OF_DAWN67);
                            JumpToNextStep(10000);
                            break;
                        case 70:
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            Talk(SAY_LIGHT_OF_DAWN68);
                            JumpToNextStep(10000);
                            break;
                        case 71:
                        {
                            std::list<Player*> players;
                            me->GetPlayerListInGrid(players, 100.0f);
                            for (Player* player : players)
                                player->CastSpell(player, SPELL_THE_LIGHT_OF_DAWN_Q, true);

                            JumpToNextStep(1000);
                            break;
                        }
                        case 72:
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, koltiraGUID))
                                temp->DespawnOrUnsummon();
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, orbazGUID))
                                temp->DespawnOrUnsummon();
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, thassarianGUID))
                                temp->DespawnOrUnsummon();
                            DespawnNPC(maxwellGUID);
                            DespawnNPC(korfaxGUID);
                            DespawnNPC(eligorGUID);

                            me->SummonCreature(NPC_HIGHLORD_DARION_MOGRAINE, me->GetPosition(), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 3min);
                            if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                                temp->DespawnOrUnsummon(3min);
                            me->SetRespawnTime(4 * MINUTE * IN_MILLISECONDS);
                            me->SetRespawnDelay(1 * IN_MILLISECONDS);
                            me->DespawnOrUnsummon();
                            break;
                        default:
                            break;
                    }

                } else phaseTimer -= diff;
            }
            // ******* During battle *****************************************************************
            else
            {
                if (antiMagicZoneTimer <= diff)
                {
                    DoCast(me, SPELL_ANTI_MAGIC_ZONE1);
                    antiMagicZoneTimer = urand(25000, 30000);
                }
                else
                    antiMagicZoneTimer -= diff;

                if (deathStrikeTimer <= diff)
                {
                    DoCastVictim(SPELL_DEATH_STRIKE);
                    deathStrikeTimer = urand(5000, 10000);
                }
                else
                    deathStrikeTimer -= diff;

                if (deathEmbraceTimer <= diff)
                {
                    DoCastVictim(SPELL_DEATH_EMBRACE);
                    deathEmbraceTimer = urand(5000, 10000);
                }
                else
                    deathEmbraceTimer -= diff;

                if (icyTouchTimer <= diff)
                {
                    DoCastVictim(SPELL_ICY_TOUCH1);
                    icyTouchTimer = urand(5000, 10000);
                }
                else
                    icyTouchTimer -= diff;

                if (unholyBlightTimer <= diff)
                {
                    DoCastVictim(SPELL_UNHOLY_BLIGHT);
                    unholyBlightTimer = urand(5000, 10000);
                }
                else
                    unholyBlightTimer -= diff;

                if (fightSpeechTimer <= diff)
                {
                    Talk(SAY_LIGHT_OF_DAWN09);
                    fightSpeechTimer = urand(15000, 20000);
                }
                else
                    fightSpeechTimer -= diff;

                // Check spawns
                if (spawnCheckTimer <= diff)
                {
                    SpawnNPC();
                    spawnCheckTimer = 1000;
                }
                else
                    spawnCheckTimer -= diff;

                // Check targets
                if (targetCheckTimer <= diff)
                {
                    for (uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                        NPCChangeTarget(ghoulGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                        NPCChangeTarget(warriorGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                        NPCChangeTarget(abominationGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                        NPCChangeTarget(behemothGUIDs[i]);
                    NPCChangeTarget(koltiraGUID);
                    NPCChangeTarget(orbazGUID);
                    NPCChangeTarget(thassarianGUID);
                    targetCheckTimer = 10000;
                }
                else
                    targetCheckTimer -= diff;

                // Battle end
                if (fightDuration <= diff + 5000)
                {
                    if (tirionGUID.IsEmpty())
                    {
                        if (Creature* temp = me->SummonCreature(NPC_HIGHLORD_TIRION_FORDRING, LightofDawnLoc[0].GetPositionWithOffset({ 0.0f, 0.0f, 0.0f, 1.528f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10min))
                        {
                            temp->SetFaction(me->GetFaction());
                            temp->SetVirtualItem(0, uint32(EQUIP_UNEQUIP));
                            temp->AI()->Talk(SAY_LIGHT_OF_DAWN25);
                            tirionGUID = temp->GetGUID();
                        }
                    }
                }
                if (fightDuration <= diff)
                {
                    inBattle = false;
                    fightDuration = 300000;
                    me->RemoveAllAuras();
                    me->GetThreatManager().ClearAllThreat();
                    me->CombatStop(true);
                    me->InterruptNonMeleeSpells(false);
                    me->SetWalk(false);
                    for (uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
                        DespawnNPC(defenderGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
                        DespawnNPC(earthshatterGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                        DespawnNPC(abominationGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                        DespawnNPC(behemothGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                        DespawnNPC(ghoulGUIDs[i]);
                    for (uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                        DespawnNPC(warriorGUIDs[i]);
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, korfaxGUID))
                    {
                        temp->RemoveAllAuras();
                        temp->GetThreatManager().ClearAllThreat();
                        temp->CombatStop(true);
                        temp->AttackStop();
                        temp->SetFaction(me->GetFaction());
                        temp->SetWalk(false);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[9]);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, maxwellGUID))
                    {
                        temp->RemoveAllAuras();
                        temp->GetThreatManager().ClearAllThreat();
                        temp->CombatStop(true);
                        temp->AttackStop();
                        temp->SetFaction(me->GetFaction());
                        temp->SetWalk(false);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[12]);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, eligorGUID))
                    {
                        temp->RemoveAllAuras();
                        temp->GetThreatManager().ClearAllThreat();
                        temp->CombatStop(true);
                        temp->AttackStop();
                        temp->SetFaction(me->GetFaction());
                        temp->SetWalk(false);
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[15]);
                    }
                    DespawnNPC(rayneGUID);
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, koltiraGUID))
                    {
                        temp->RemoveAllAuras();
                        temp->GetThreatManager().ClearAllThreat();
                        temp->CombatStop(true);
                        temp->AttackStop();
                        temp->SetFaction(me->GetFaction());
                        temp->SetWalk(false);
                        temp->Dismount();
                        temp->ClearUnitState(UNIT_STATE_EVADE);
                        temp->SetSpawnHealth();
                        temp->GetMotionMaster()->MoveIdle();
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[18]);
                        temp->CastSpell(temp, SPELL_THE_LIGHT_OF_DAWN, false);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, orbazGUID))
                    {
                        temp->RemoveAllAuras();
                        temp->GetThreatManager().ClearAllThreat();
                        temp->CombatStop(true);
                        temp->AI()->Talk(EMOTE_LIGHT_OF_DAWN04);
                        temp->Dismount();
                        temp->ClearUnitState(UNIT_STATE_EVADE);
                        temp->GetMotionMaster()->MoveIdle();
                        temp->DespawnOrUnsummon(5s);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, thassarianGUID))
                    {
                        temp->RemoveAllAuras();
                        temp->GetThreatManager().ClearAllThreat();
                        temp->CombatStop(true);
                        temp->AttackStop();
                        temp->SetFaction(me->GetFaction());
                        temp->SetWalk(false);
                        temp->Dismount();
                        temp->ClearUnitState(UNIT_STATE_EVADE);
                        temp->SetSpawnHealth();
                        temp->GetMotionMaster()->MoveIdle();
                        temp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[20]);
                        temp->CastSpell(temp, SPELL_THE_LIGHT_OF_DAWN, false);
                    }
                    if (Creature* temp = ObjectAccessor::GetCreature(*me, tirionGUID))
                        temp->AI()->Talk(SAY_LIGHT_OF_DAWN26);
                    SetEscortPaused(false);
                }
                else
                    fightDuration -= diff;

                DoMeleeAttackIfReady();
            }
        }

        void JumpToNextStep(uint32 uiTimer)
        {
            phaseTimer = uiTimer;
            ++step;
        }

        void NPCChangeTarget(ObjectGuid guid)
        {
            if (Creature* temp = ObjectAccessor::GetCreature(*me, guid))
                if (temp->IsAlive())
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0))
                        if (target->IsAlive())
                        {
                            temp->GetThreatManager().AddThreat(target, 1000.0f);
                            target->SetInCombatWith(temp);
                            temp->AI()->AttackStart(target);
                        }
        }

        void SpawnNPC()
        {
            Unit* temp = nullptr;

            // Death
            for (uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
            {
                temp = ObjectAccessor::GetCreature(*me, ghoulGUIDs[i]);
                if (!temp)
                {
                    temp = me->SummonCreature(NPC_ACHERUS_GHOUL, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                    temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                    ghoulGUIDs[i] = temp->GetGUID();
                }
            }
            for (uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
            {
                temp = ObjectAccessor::GetCreature(*me, abominationGUIDs[i]);
                if (!temp)
                {
                    temp = me->SummonCreature(NPC_WARRIOR_OF_THE_FROZEN_WASTES, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                    temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                    abominationGUIDs[i] = temp->GetGUID();
                }
            }
            for (uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
            {
                temp = ObjectAccessor::GetCreature(*me, warriorGUIDs[i]);
                if (!temp)
                {
                    temp = me->SummonCreature(NPC_RAMPAGING_ABOMINATION, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                    temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                    warriorGUIDs[i] = temp->GetGUID();
                }
            }
            for (uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
            {
                temp = ObjectAccessor::GetCreature(*me, behemothGUIDs[i]);
                if (!temp)
                {
                    temp = me->SummonCreature(NPC_FLESH_BEHEMOTH, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                    temp->SetFaction(FACTION_UNDEAD_SCOURGE_3);
                    behemothGUIDs[i] = temp->GetGUID();
                }
            }

            // Dawn
            for (uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
            {
                temp = ObjectAccessor::GetCreature(*me, defenderGUIDs[i]);
                if (!temp)
                {
                    temp = me->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                    temp->SetFaction(FACTION_SCARLET_CRUSADE);
                    AddThreat(temp, 0.0f);
                    defenderGUIDs[i] = temp->GetGUID();
                }
            }
            for (uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
            {
                temp = ObjectAccessor::GetCreature(*me, earthshatterGUIDs[i]);
                if (!temp)
                {
                    temp = me->SummonCreature(NPC_RIMBLAT_EARTHSHATTER, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                    temp->SetFaction(FACTION_SCARLET_CRUSADE);
                    AddThreat(temp, 0.0f);
                    earthshatterGUIDs[i] = temp->GetGUID();
                }
            }
            temp = ObjectAccessor::GetCreature(*me, korfaxGUID);
            if (!temp)
            {
                temp = me->SummonCreature(NPC_KORFAX_CHAMPION_OF_THE_LIGHT, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10min);
                temp->SetFaction(FACTION_SCARLET_CRUSADE);
                AddThreat(temp, 0.0f);
                korfaxGUID = temp->GetGUID();
            }
            temp = ObjectAccessor::GetCreature(*me, maxwellGUID);
            if (!temp)
            {
                temp = me->SummonCreature(NPC_LORD_MAXWELL_TYROSUS, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10min);
                temp->SetFaction(FACTION_SCARLET_CRUSADE);
                AddThreat(temp, 0.0f);
                maxwellGUID = temp->GetGUID();
            }
            temp = ObjectAccessor::GetCreature(*me, eligorGUID);
            if (!temp)
            {
                temp = me->SummonCreature(NPC_COMMANDER_ELIGOR_DAWNBRINGER, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10min);
                temp->SetFaction(FACTION_SCARLET_CRUSADE);
                AddThreat(temp, 0.0f);
                eligorGUID = temp->GetGUID();
            }
            temp = ObjectAccessor::GetCreature(*me, rayneGUID);
            if (!temp)
            {
                temp = me->SummonCreature(NPC_RAYNE, LightofDawnLoc[0].GetPositionWithOffset({ float(rand32() % 30), float(rand32() % 30), 0.0f, 0.0f }), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 5min);
                temp->SetFaction(FACTION_SCARLET_CRUSADE);
                AddThreat(temp, 0.0f);
                rayneGUID = temp->GetGUID();
            }
        }

        void DespawnNPC(ObjectGuid guid)
        {
            if (Creature* temp = ObjectAccessor::GetCreature(*me, guid))
                temp->DespawnOrUnsummon();
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    CloseGossipMenuFor(player);
                    step = 1;
                    LoadPath(PATH_ESCORT_MOGRAINE);
                    Start(true, player->GetGUID());
                    break;
            }
            return true;
        }

        bool OnGossipHello(Player* player) override
        {
            if (me->IsQuestGiver())
                player->PrepareQuestMenu(me->GetGUID());

            if (player->GetQuestStatus(12801) == QUEST_STATUS_INCOMPLETE)
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "I am ready.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            SendGossipMenuFor(player, player->GetGossipTextId(me), me->GetGUID());

            return true;
        }

    private:
        bool inBattle;
        uint32 step;
        uint32 phaseTimer;
        uint32 fightDuration;
        uint32 summonCounter;

        // Darion Mograine
        uint32 antiMagicZoneTimer;
        uint32 deathStrikeTimer;
        uint32 deathEmbraceTimer;
        uint32 icyTouchTimer;
        uint32 unholyBlightTimer;
        uint32 fightSpeechTimer;
        uint32 spawnCheckTimer;
        uint32 targetCheckTimer;

        // Dawn
        ObjectGuid tirionGUID;
        ObjectGuid alexandrosGUID;
        ObjectGuid darionGUID;
        ObjectGuid korfaxGUID;
        ObjectGuid maxwellGUID;
        ObjectGuid eligorGUID;
        ObjectGuid rayneGUID;
        ObjectGuid defenderGUIDs[ENCOUNTER_DEFENDER_NUMBER];
        ObjectGuid earthshatterGUIDs[ENCOUNTER_EARTHSHATTER_NUMBER];

        // Death
        ObjectGuid koltiraGUID;
        ObjectGuid orbazGUID;
        ObjectGuid thassarianGUID;
        ObjectGuid lichKingGUID;
        ObjectGuid abominationGUIDs[ENCOUNTER_ABOMINATION_NUMBER];
        ObjectGuid behemothGUIDs[ENCOUNTER_BEHEMOTH_NUMBER];
        ObjectGuid ghoulGUIDs[ENCOUNTER_GHOUL_NUMBER];
        ObjectGuid warriorGUIDs[ENCOUNTER_WARRIOR_NUMBER];
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_highlord_darion_mograineAI(creature);
    }
};

/*######
## npc the lich king in dawn of light
######*/

class npc_the_lich_king_tirion_dawn : public CreatureScript
{
    public:
        npc_the_lich_king_tirion_dawn() : CreatureScript("npc_the_lich_king_tirion_dawn") { }

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new npc_the_lich_king_tirion_dawnAI(creature);
        }

        struct npc_the_lich_king_tirion_dawnAI : public PassiveAI
        {
            npc_the_lich_king_tirion_dawnAI(Creature* creature) : PassiveAI(creature) { }
        };
};

void AddSC_the_scarlet_enclave_c5()
{
    new npc_highlord_darion_mograine();
    new npc_the_lich_king_tirion_dawn();
}
