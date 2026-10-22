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
#include "Map.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"
#include "Timer.h"
#include "WorldStatePackets.h"
#include <algorithm>
#include <array>
#include <map>
#include <set>
#include <tuple>
#include <vector>

enum TheLightOfDawnMisc
{
    SPELL_THE_MIGHT_OF_MOGRAINE = 53642,
    SPELL_THE_LIGHT_OF_DAWN = 53658,
    SPELL_THE_LIGHT_OF_DAWN_CREDIT = 53606,
    SPELL_SHAKE_CAMERA = 33271,
    SPELL_ANTI_MAGIC_ZONE = 52893,
    SPELL_DEATH_STRIKE = 53639,
    SPELL_DEATHS_EMBRACE = 53635,
    SPELL_ICY_TOUCH = 49723,
    SPELL_UNHOLY_BLIGHT = 53640,
    SPELL_ALEXANDROS_MOGRAINE_SPAWN = 53667,
    SPELL_SOUL_FEAST_ALEXANDROS = 53677,
    SPELL_MOGRAINE_CHARGE = 53679,
    SPELL_SOUL_FEAST_TIRION = 53685,
    SPELL_ASHBRINGER = 53701,
    SPELL_REBIRTH_OF_THE_ASHBRINGER = 53702,
    SPELL_TIRION_CHARGE = 53705,
    SPELL_SOUL_RELEASE = 41542,
    SPELL_TELEPORT_VISUAL = 52233,
    SPELL_HERO_AGGRO_AURA = 53627,
    SPELL_THE_LIGHT_OF_DAWN_DAMAGE_LOSS = 53645,
    SPELL_CLEAVE = 53631,
    SPELL_HEROIC_LEAP = 53625,
    SPELL_HOLY_LIGHT = 37979,
    SPELL_FIREBALL = 20678,
    SPELL_WRATH = 21807,
    SPELL_REJUVENATION = 20664,
    SPELL_TRANQUILITY = 25817,
    SPELL_THUNDER = 53630,
    SPELL_CHAIN_HEAL = 33642,

    NPC_HIGHLORD_DARION_MOGRAINE = 29173,
    NPC_HIGHLORD_TIRION_FORDRING = 29175,
    NPC_KORFAX_CHAMPION_OF_THE_LIGHT = 29176,
    NPC_COMMANDER_ELIGOR_DAWNBRINGER = 29177,
    NPC_LORD_MAXWELL_TYROSUS = 29178,
    NPC_LEONID_BARTHALOMEW_THE_REVERED = 29179,
    NPC_DUKE_NICHOLAS_ZVERENHOFF = 29180,
    NPC_RAYNE = 29181,
    NPC_RIMBLAT_EARTHSHATTER = 29182,
    NPC_DEFENDER_OF_THE_LIGHT = 29174,
    NPC_THE_LICH_KING = 29183,
    NPC_FLESH_BEHEMOTH = 29190,
    NPC_KOLTIRA_DEATHWEAVER = 29199,
    NPC_THASSARIAN = 29200,
    NPC_ORBAZ_BLOODBANE = 29204,
    NPC_WARRIOR_OF_THE_FROZEN_WASTES = 29206,
    NPC_VOLATILE_GHOUL = 29219,
    NPC_HIGHLORD_ALEXANDROS_MOGRAINE = 29227,
    NPC_DARION_MOGRAINE = 29228,

    GO_LIGHT_OF_DAWN = 191330,

    EQUIP_ASHBRINGER = 13262,

    SAY_DARION_STAND_READY = 0,
    SAY_DARION_SKY_WEEPS = 1,
    SAY_DARION_HEAR_THE_CALL = 3,
    SAY_DARION_RISE = 4,
    SAY_DARION_SKIES_TURN_RED = 5,
    SAY_DARION_BATTLE = 8,
    SAY_DARION_DEFEAT = 9,
    SAY_DARION_STAND_DOWN = 26,
    SAY_DARION_SAVE_YOUR_BREATH = 30,
    SAY_DARION_FATHER = 32,
    SAY_DARION_WHAT_IS = 33,
    SAY_DARION_BETRAYED = 43,
    SAY_DARION_NOT_TODAY = 51,
    SAY_DARION_TIRION = 52,
    SAY_DARION_EBON_BLADE = 67,
    EMOTE_DARION_KNEELS_IN_DEFEAT = 72,
    EMOTE_DARION_ANGRY = 77,
    EMOTE_DARION_THROWS_ASHBRINGER = 81,
    EMOTE_DARION_COLLAPSES = 82,

    SAY_TIRION_CANNOT_WIN = 24,
    SAY_TIRION_BRING_THEM = 25,
    SAY_TIRION_LEARNED_NOTHING = 27,
    SAY_TIRION_YOUR_MASTER_KNOWS = 28,
    SAY_TIRION_ANGUISH = 29,
    SAY_TIRION_MONSTER = 46,
    SAY_TIRION_ARTHAS = 53,
    SAY_TIRION_YOUR_END = 55,
    SAY_TIRION_RISE_DARION = 59,
    SAY_TIRION_TERRIBLE_TRAGEDY = 60,
    SAY_TIRION_REMAIN_VIGILANT = 61,
    SAY_TIRION_MUST_ANSWER = 62,
    SAY_TIRION_PROMISE = 63,
    SAY_TIRION_COME_TOGETHER = 64,
    SAY_TIRION_ICECROWN = 65,
    SAY_TIRION_ARGENT_CRUSADE = 66,
    EMOTE_TIRION_GASPS = 79,
    EMOTE_TIRION_CHARGES = 83,

    EMOTE_CHAMPION_FALLS_UNCONSCIOUS = 0,

    SAY_KORFAX_SCOURGE_APPROACH = 6,

    SAY_MAXWELL_STAND_FAST = 7,
    SAY_MAXWELL_ATTACK = 49,

    SAY_LICH_KING_TOUCHING = 42,
    SAY_LICH_KING_HE_IS_MINE = 44,
    SAY_LICH_KING_PATHETIC = 45,
    SAY_LICH_KING_YOU_WERE_RIGHT = 47,
    SAY_LICH_KING_HOW_SIMPLE = 48,
    SAY_LICH_KING_APOCALYPSE = 50,
    SAY_LICH_KING_WHAT_IS_THIS = 54,
    SAY_LICH_KING_IMPOSSIBLE = 56,
    SAY_LICH_KING_NOT_OVER = 57,
    SAY_LICH_KING_NEXT_WE_MEET = 58,
    EMOTE_LICH_KING_CASTS_ON_TIRION = 78,
    EMOTE_LICH_KING_KILLS_DEFENDERS = 80,
    EMOTE_LICH_KING_DISAPPEARS = 84,

    EMOTE_ORBAZ_FLEES = 71,

    SAY_ALEXANDROS_MY_SON = 31,
    SAY_ALEXANDROS_MY_HOME = 36,
    SAY_ALEXANDROS_BARELY_OF_AGE = 38,
    SAY_ALEXANDROS_ASHBRINGER = 40,
    SAY_ALEXANDROS_DO_NOT_FORGET = 41,
    EMOTE_ALEXANDROS_ARRIVES = 73,
    EMOTE_ALEXANDROS_DISAPPEARS = 76,

    SAY_YOUNG_DARION_RETURNED = 34,
    SAY_YOUNG_DARION_GONE_LONG = 35,
    SAY_YOUNG_DARION_JOIN_THE_WAR = 37,
    SAY_YOUNG_DARION_ON_MY_FEET = 39,
    EMOTE_YOUNG_DARION_SHADE = 74,
    EMOTE_YOUNG_DARION_HUGS = 75,

    GOSSIP_MENU_LIGHT_OF_DAWN = 9795,
    GOSSIP_OPTION_START_BATTLE = 0,

    WORLD_STATE_FORCES_LIGHT = 3590,
    WORLD_STATE_FORCES_SCOURGE = 3591,
    WORLD_STATE_FORCES_SHOW = 3592,
    WORLD_STATE_COUNTDOWN_SHOW = 3603,
    WORLD_STATE_COUNTDOWN_TIME = 3604,
    WORLD_STATE_BATTLE_BEGUN = 3605,

    MAX_FORCES_LIGHT = 300,
    MAX_FORCES_SCOURGE = 10000,
    DEFENDERS_KILLED_TO_END = 100,

    EVENT_STATE_NOT_STARTED = 0,
    EVENT_STATE_COUNTDOWN,
    EVENT_STATE_INTRO,
    EVENT_STATE_BATTLE,
    EVENT_STATE_OUTRO,

    MAP_SCARLET_ENCLAVE = 609,
    ZONE_THE_SCARLET_ENCLAVE = 4298,
    PHASE_CHAPTER_IV_EPILOGUE = 128,

    SUMMON_GROUP_ARMY_FIRST = 0,
    SUMMON_GROUP_ARMY_LAST = 4,
    SUMMON_GROUP_DAWN = 5,

    DATA_EVENT_STATE = 1,
    DATA_KOLTIRA_DEATHWEAVER,
    DATA_ORBAZ_BLOODBANE,
    DATA_THASSARIAN,
    DATA_FLESH_BEHEMOTH_1,
    DATA_FLESH_BEHEMOTH_2,

    ACTION_REVIVE_CHAMPION = 1,
    ACTION_COUNTDOWN_FINISHED,
    ACTION_BATTLE_LOST,

    POINT_BATTLEFIELD = 1,
    POINT_CHAPEL,

    GROUP_BATTLE = 1,
    GROUP_YELLS
};

Position const DarionBattlefieldPos = { 2319.24f, -5266.49f, 82.825f };
Position const DarionChapelPos = { 2280.81f, -5284.09f, 82.608f };
Position const DarionKneelPos = { 2280.73f, -5286.84f, 82.93f };
Position const DarionChargePos = { 2281.46f, -5263.01f, 81.164f };
Position const DarionKickedPos = { 2257.48f, -5296.7f, 82.165f };
Position const DarionThrowPos = { 2261.24f, -5294.98f, 82.167f };

Position const DarionLeashPos = { 2280.159f, -5263.561f, 81.15f };
Position const OrbazFleePos = { 2169.1f, -5227.1f, 82.59f };
Position const LightOfDawnPos = { 2283.896f, -5287.914f, 83.066f };

Position const TirionSpawnPos = { 2281.335f, -5300.409f, 85.170f, 1.528f };
Position const TirionForwardPos = { 2283.896f, -5287.914f, 83.066f, 1.55f };
Position const TirionChargePos = { 2281.461f, -5263.014f, 81.164f };
Position const TirionRunPos = { 2262.277f, -5293.477f, 82.167f };
Position const TirionApproachPos = { 2270.286f, -5287.73f, 82.262f };
Position const TirionSpeechPos = { 2269.511f, -5288.289f, 82.225f };

Position const KorfaxRetreatPos = { 2273.205f, -5288.848f, 82.617f };
Position const KorfaxChapelPos = { 2274.739f, -5287.926f, 82.684f };
Position const KorfaxSpeechPos = { 2253.673f, -5318.004f, 81.724f };
Position const MaxwellRetreatPos = { 2287.028f, -5309.644f, 87.253f };
Position const MaxwellChapelPos = { 2286.978f, -5308.025f, 86.83f };
Position const MaxwellSpeechPos = { 2248.877f, -5307.586f, 82.166f };
Position const EligorRetreatPos = { 2278.58f, -5316.933f, 88.319f };
Position const EligorChapelPos = { 2278.535f, -5315.479f, 88.08f };
Position const EligorSpeechPos = { 2259.416f, -5304.505f, 82.149f };

Position const KoltiraRetreatPos = { 2289.259f, -5280.355f, 82.112f };
Position const KoltiraChapelPos = { 2289.02f, -5281.985f, 82.207f };
Position const ThassarianRetreatPos = { 2273.289f, -5273.675f, 81.701f };
Position const ThassarianChapelPos = { 2273.332f, -5275.544f, 81.849f };

Position const AlexandrosSpawnPos = { 2281.198f, -5257.397f, 80.224f, 4.66f };
Position const AlexandrosForwardPos = { 2281.156f, -5259.934f, 80.647f };
Position const YoungDarionSpawnPos = { 2281.294f, -5281.895f, 82.445f, 1.35f };
Position const YoungDarionHugPos = { 2281.093f, -5263.013f, 81.125f };

Position const LichKingSpawnPos = { 2281.313f, -5250.282f, 79.322f, 4.69f };
Position const LichKingForwardPos = { 2281.523f, -5261.058f, 80.877f };
Position const LichKingKnockedPos = { 2272.709f, -5255.552f, 78.226f };
Position const LichKingRecoverPos = { 2273.972f, -5257.676f, 78.862f };

Position const OutroDefenderPos[] =
{
    { 2276.66f, -5273.60f, 81.86f, 5.14f },
    { 2272.11f, -5279.08f, 82.01f, 5.69f },
    { 2285.11f, -5276.73f, 82.08f, 4.23f },
    { 2290.06f, -5286.41f, 82.51f, 3.16f }
};

std::array<uint32, 8> const DawnForces = { NPC_DEFENDER_OF_THE_LIGHT, NPC_KORFAX_CHAMPION_OF_THE_LIGHT, NPC_COMMANDER_ELIGOR_DAWNBRINGER, NPC_LORD_MAXWELL_TYROSUS, NPC_LEONID_BARTHALOMEW_THE_REVERED, NPC_DUKE_NICHOLAS_ZVERENHOFF, NPC_RAYNE, NPC_RIMBLAT_EARTHSHATTER };
std::array<uint32, 4> const SceneSpawns = { NPC_FLESH_BEHEMOTH, NPC_KOLTIRA_DEATHWEAVER, NPC_THASSARIAN, NPC_ORBAZ_BLOODBANE };
Seconds const SceneRespawnHold = 15min;

// 29173 - Highlord Darion Mograine
struct npc_highlord_darion_mograine : public ScriptedAI
{
    npc_highlord_darion_mograine(Creature* creature) : ScriptedAI(creature), _summons(me), _eventInProgress(false), _battleInProgress(false) { }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        ZoneScript* zoneScript = me->GetZoneScript();
        if (!zoneScript || menuId != GOSSIP_MENU_LIGHT_OF_DAWN || gossipListId != GOSSIP_OPTION_START_BATTLE || _eventInProgress || !player->PlayerTalkClass->GetGossipMenu().GetItem(gossipListId))
            return false;

        CloseGossipMenuFor(player);

        _eventInProgress = true;
        me->setActive(true);
        me->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);
        Talk(SAY_DARION_STAND_READY);

        zoneScript->SetData(DATA_EVENT_STATE, EVENT_STATE_COUNTDOWN);
        _koltiraGUID = zoneScript->GetGuidData(DATA_KOLTIRA_DEATHWEAVER);
        _orbazGUID = zoneScript->GetGuidData(DATA_ORBAZ_BLOODBANE);
        _thassarianGUID = zoneScript->GetGuidData(DATA_THASSARIAN);
        _behemothGUIDs = { zoneScript->GetGuidData(DATA_FLESH_BEHEMOTH_1), zoneScript->GetGuidData(DATA_FLESH_BEHEMOTH_2) };

        _scheduler.Schedule(240s, [this](TaskContext /*context*/)
        {
            Talk(SAY_DARION_SKY_WEEPS);
        });

        return true;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_COUNTDOWN_FINISHED:
                me->ReplaceAllNpcFlags(UNIT_NPC_FLAG_NONE);
                Talk(SAY_DARION_HEAR_THE_CALL);

                _scheduler.Schedule(8s, [this](TaskContext context)
                {
                    uint8 const wave = uint8(context.GetRepeatCounter());
                    if (!wave)
                        Talk(SAY_DARION_RISE);

                    DoCastSelf(SPELL_SHAKE_CAMERA);
                    me->SummonCreatureGroup(SUMMON_GROUP_ARMY_FIRST + wave);
                    if (SUMMON_GROUP_ARMY_FIRST + wave < SUMMON_GROUP_ARMY_LAST)
                        context.Repeat(4s);
                })
                .Schedule(32s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_DARION_SKIES_TURN_RED);
                })
                .Schedule(35s, [this](TaskContext /*context*/)
                {
                    for (ObjectGuid const& guid : _summons)
                        if (Creature* summon = ObjectAccessor::GetCreature(*me, guid))
                            summon->HandleEmoteCommand(EMOTE_ONESHOT_BATTLE_ROAR);
                })
                .Schedule(37500ms, [this](TaskContext /*context*/)
                {
                    DoCastSelf(SPELL_THE_MIGHT_OF_MOGRAINE, true);
                    DoCastSelf(SPELL_HERO_AGGRO_AURA, true);

                    float const marchAngle = me->GetAbsoluteAngle(DarionBattlefieldPos);
                    for (ObjectGuid const& guid : GetMarcherGUIDs())
                    {
                        if (Creature* marcher = ObjectAccessor::GetCreature(*me, guid))
                        {
                            marcher->SetWalk(false);
                            marcher->GetMotionMaster()->MoveFormation(me, me->GetExactDist2d(marcher), Position::NormalizeOrientation(me->GetAbsoluteAngle(marcher) - marchAngle), 0, 0);
                        }
                    }

                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(POINT_BATTLEFIELD, DarionBattlefieldPos);
                });
                break;
            case ACTION_BATTLE_LOST:
            {
                DoCastSelf(SPELL_THE_LIGHT_OF_DAWN_DAMAGE_LOSS, true);

                if (TempSummon* tirion = me->SummonCreature(NPC_HIGHLORD_TIRION_FORDRING, TirionSpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20min))
                {
                    tirion->SetFaction(me->GetFaction());
                    tirion->AI()->Talk(SAY_TIRION_CANNOT_WIN);
                }

                _scheduler.CancelGroup(GROUP_YELLS);
                _scheduler.Schedule(1s, GROUP_YELLS, [this](TaskContext context)
                {
                    Talk(SAY_DARION_DEFEAT);
                    if (!me->IsWithinDist2d(&DarionLeashPos, 75.0f))
                        me->GetMotionMaster()->MovePoint(0, DarionLeashPos);
                    context.Repeat(5s, 7s);
                })
                .Schedule(10s, [this](TaskContext /*context*/)
                {
                    _battleInProgress = false;
                    _summons.DespawnEntry(NPC_DEFENDER_OF_THE_LIGHT);
                })
                .Schedule(20s, [this](TaskContext context)
                {
                    context.CancelGroup(GROUP_BATTLE).CancelGroup(GROUP_YELLS);

                    me->RemoveAllAuras();
                    me->CombatStop(true);
                    me->SetImmuneToNPC(true);
                    if (IsEngaged())
                        EngagementOver();
                    me->GetMotionMaster()->Clear();

                    for (uint32 entry : { NPC_VOLATILE_GHOUL, NPC_WARRIOR_OF_THE_FROZEN_WASTES, NPC_LEONID_BARTHALOMEW_THE_REVERED, NPC_DUKE_NICHOLAS_ZVERENHOFF, NPC_RAYNE, NPC_RIMBLAT_EARTHSHATTER })
                        _summons.DespawnEntry(entry);

                    for (ObjectGuid const& guid : _behemothGUIDs)
                        if (Creature* behemoth = ObjectAccessor::GetCreature(*me, guid))
                            behemoth->DespawnOrUnsummon(0s, SceneRespawnHold);

                    std::pair<ObjectGuid, Position> const retreats[] = { { _korfaxGUID, KorfaxRetreatPos }, { _maxwellGUID, MaxwellRetreatPos }, { _eligorGUID, EligorRetreatPos } };
                    for (auto const& [guid, pos] : retreats)
                    {
                        if (Creature* champion = ObjectAccessor::GetCreature(*me, guid))
                        {
                            champion->AI()->DoAction(ACTION_REVIVE_CHAMPION);
                            champion->SetFaction(me->GetFaction());
                            champion->AI()->EnterEvadeMode();
                            champion->SetWalk(false);
                            champion->GetMotionMaster()->MovePoint(0, pos, true, pos.GetAbsoluteAngle(DarionChapelPos));
                        }
                    }

                    for (ObjectGuid const& guid : { _koltiraGUID, _thassarianGUID })
                    {
                        if (Creature* hero = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (!hero->IsAlive())
                                continue;

                            hero->AI()->EnterEvadeMode();
                            hero->Dismount();
                            hero->GetMotionMaster()->Clear();
                        }
                    }

                    if (Creature* orbaz = ObjectAccessor::GetCreature(*me, _orbazGUID))
                    {
                        if (orbaz->IsAlive())
                        {
                            orbaz->AI()->Talk(EMOTE_ORBAZ_FLEES);
                            orbaz->AI()->EnterEvadeMode();
                            orbaz->GetMotionMaster()->Clear();
                            orbaz->SetWalk(false);
                            orbaz->GetMotionMaster()->MovePoint(0, OrbazFleePos);
                        }

                        orbaz->DespawnOrUnsummon(7s, SceneRespawnHold);
                    }

                    for (Position const& pos : OutroDefenderPos)
                    {
                        if (TempSummon* defender = me->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, pos, TEMPSUMMON_MANUAL_DESPAWN))
                        {
                            defender->SetFaction(me->GetFaction());
                            defender->SetReactState(REACT_PASSIVE);
                            defender->SetImmuneToAll(true);
                            defender->SetEmoteState(EMOTE_STATE_READY1H);
                            _outroDefenderGUIDs.push_back(defender->GetGUID());
                        }
                    }
                })
                .Schedule(22s, [this](TaskContext /*context*/)
                {
                    if (Creature* koltira = ObjectAccessor::GetCreature(*me, _koltiraGUID))
                    {
                        if (koltira->IsAlive())
                        {
                            koltira->SetWalk(true);
                            koltira->CastSpell(koltira, SPELL_THE_LIGHT_OF_DAWN, true);
                            koltira->GetMotionMaster()->MoveCharge(KoltiraRetreatPos.GetPositionX(), KoltiraRetreatPos.GetPositionY(), KoltiraRetreatPos.GetPositionZ(), 4.0f, 0, true);
                        }
                    }
                })
                .Schedule(23s, [this](TaskContext /*context*/)
                {
                    if (Creature* thassarian = ObjectAccessor::GetCreature(*me, _thassarianGUID))
                    {
                        if (thassarian->IsAlive())
                        {
                            thassarian->SetWalk(true);
                            thassarian->CastSpell(thassarian, SPELL_THE_LIGHT_OF_DAWN, true);
                            thassarian->GetMotionMaster()->MoveCharge(ThassarianRetreatPos.GetPositionX(), ThassarianRetreatPos.GetPositionY(), ThassarianRetreatPos.GetPositionZ(), 4.0f, 0, true);
                        }
                    }
                })
                .Schedule(24s, [this](TaskContext /*context*/)
                {
                    me->SetWalk(true);
                    DoCastSelf(SPELL_THE_LIGHT_OF_DAWN, true);
                    me->GetMotionMaster()->MoveCharge(DarionChapelPos.GetPositionX(), DarionChapelPos.GetPositionY(), DarionChapelPos.GetPositionZ(), 4.0f, POINT_CHAPEL, true);

                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_BRING_THEM);
                });
                break;
            }
            default:
                break;
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case POINT_BATTLEFIELD:
            {
                _battleInProgress = true;
                me->Dismount();
                me->SetImmuneToNPC(false);
                if (ZoneScript* zoneScript = me->GetZoneScript())
                    zoneScript->SetData(DATA_EVENT_STATE, EVENT_STATE_BATTLE);

                for (ObjectGuid const& guid : GetMarcherGUIDs())
                {
                    if (Creature* marcher = ObjectAccessor::GetCreature(*me, guid))
                    {
                        marcher->Dismount();
                        marcher->GetMotionMaster()->MoveIdle();
                    }
                }

                me->SummonCreatureGroup(SUMMON_GROUP_DAWN);

                if (Creature* korfax = ObjectAccessor::GetCreature(*me, _korfaxGUID))
                    korfax->AI()->Talk(SAY_KORFAX_SCOURGE_APPROACH);
                if (Creature* maxwell = ObjectAccessor::GetCreature(*me, _maxwellGUID))
                    maxwell->AI()->Talk(SAY_MAXWELL_STAND_FAST);

                _scheduler.Schedule(1s, GROUP_BATTLE, [this](TaskContext context)
                {
                    std::vector<Creature*> scourge = { me };
                    std::vector<Creature*> dawn;

                    GuidVector scourgeChampions = { _koltiraGUID, _orbazGUID, _thassarianGUID };
                    scourgeChampions.insert(scourgeChampions.end(), _behemothGUIDs.begin(), _behemothGUIDs.end());
                    for (ObjectGuid const& guid : scourgeChampions)
                    {
                        if (Creature* champion = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (champion->IsAlive())
                            {
                                champion->Dismount();
                                scourge.push_back(champion);
                            }
                        }
                    }

                    for (ObjectGuid const& guid : _summons)
                    {
                        if (Creature* summon = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (!summon->IsAlive() || summon->HasUnitFlag(UNIT_FLAG_UNINTERACTIBLE))
                                continue;

                            if (summon->GetFaction() == FACTION_UNDEAD_SCOURGE_3)
                                scourge.push_back(summon);
                            else if (summon->GetFaction() == FACTION_SCARLET_CRUSADE)
                                dawn.push_back(summon);
                        }
                    }

                    if (!dawn.empty())
                        for (Creature* attacker : scourge)
                            if (!attacker->GetVictim())
                                attacker->AI()->AttackStart(Trinity::Containers::SelectRandomContainerElement(dawn));

                    for (Creature* attacker : dawn)
                        if (!attacker->GetVictim())
                            attacker->AI()->AttackStart(Trinity::Containers::SelectRandomContainerElement(scourge));

                    context.Repeat(5s);
                })
                .Schedule(1s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastSelf(SPELL_ANTI_MAGIC_ZONE);
                    context.Repeat(35s, 40s);
                })
                .Schedule(1s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_DEATH_STRIKE);
                    context.Repeat(10s, 15s);
                })
                .Schedule(5s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_DEATHS_EMBRACE);
                    context.Repeat(50s, 60s);
                })
                .Schedule(5s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_ICY_TOUCH);
                    context.Repeat(25s);
                })
                .Schedule(6s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastSelf(SPELL_UNHOLY_BLIGHT);
                    context.Repeat(65s, 75s);
                })
                .Schedule(15s, GROUP_YELLS, [this](TaskContext context)
                {
                    Talk(SAY_DARION_BATTLE);
                    if (!me->IsWithinDist2d(&DarionLeashPos, 75.0f))
                        me->GetMotionMaster()->MovePoint(0, DarionLeashPos);
                    context.Repeat(15s, 20s);
                });
                break;
            }
            case POINT_CHAPEL:
            {
                Talk(EMOTE_DARION_KNEELS_IN_DEFEAT);
                me->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, DarionKneelPos, true, DarionKneelPos.GetAbsoluteAngle(TirionSpawnPos));

                std::tuple<ObjectGuid, Position, Position> const gatherings[] = { { _korfaxGUID, KorfaxChapelPos, DarionKneelPos }, { _maxwellGUID, MaxwellChapelPos, DarionKneelPos }, { _eligorGUID, EligorChapelPos, DarionKneelPos }, { _koltiraGUID, KoltiraChapelPos, TirionSpawnPos }, { _thassarianGUID, ThassarianChapelPos, TirionSpawnPos } };
                for (auto const& [guid, pos, facing] : gatherings)
                {
                    if (Creature* gathering = ObjectAccessor::GetCreature(*me, guid))
                    {
                        gathering->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                        gathering->SetWalk(true);
                        gathering->GetMotionMaster()->MovePoint(0, pos, true, pos.GetAbsoluteAngle(facing));
                    }
                }

                if (Creature* korfax = ObjectAccessor::GetCreature(*me, _korfaxGUID))
                    korfax->SetEmoteState(EMOTE_STATE_READY2H);

                _scheduler.Schedule(1500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_DARION_STAND_DOWN);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    for (ObjectGuid const& guid : { _koltiraGUID, _thassarianGUID })
                        if (Creature* hero = ObjectAccessor::GetCreature(*me, guid))
                            if (hero->IsAlive())
                                hero->SetStandState(UNIT_STAND_STATE_KNEEL);
                })
                .Schedule(10s, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->Dismount();
                        tirion->AI()->Talk(SAY_TIRION_LEARNED_NOTHING);
                    }
                })
                .Schedule(31s, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_YOUR_MASTER_KNOWS);
                })
                .Schedule(44s, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_ANGUISH);
                })
                .Schedule(57s, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    Talk(SAY_DARION_SAVE_YOUR_BREATH);
                })
                .Schedule(64s, [this](TaskContext /*context*/)
                {
                    if (TempSummon* alexandros = me->SummonCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, AlexandrosSpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20min))
                    {
                        alexandros->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                        alexandros->CastSpell(alexandros, SPELL_ALEXANDROS_MOGRAINE_SPAWN, true);
                        alexandros->AI()->Talk(EMOTE_ALEXANDROS_ARRIVES);
                    }
                })
                .Schedule(68s, [this](TaskContext /*context*/)
                {
                    if (Creature* alexandros = ObjectAccessor::GetCreature(*me, _alexandrosGUID))
                    {
                        alexandros->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                        alexandros->GetMotionMaster()->MovePoint(0, AlexandrosForwardPos, true, AlexandrosForwardPos.GetAbsoluteAngle(DarionChapelPos));
                        alexandros->AI()->Talk(SAY_ALEXANDROS_MY_SON);
                    }

                    me->GetMotionMaster()->MovePoint(0, DarionChapelPos, true, DarionChapelPos.GetAbsoluteAngle(AlexandrosForwardPos));
                })
                .Schedule(69500ms, [this](TaskContext /*context*/)
                {
                    Talk(SAY_DARION_FATHER);
                })
                .Schedule(73s, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    Talk(SAY_DARION_WHAT_IS);
                })
                .Schedule(78s, [this](TaskContext /*context*/)
                {
                    if (TempSummon* youngDarion = me->SummonCreature(NPC_DARION_MOGRAINE, YoungDarionSpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20min))
                    {
                        youngDarion->SetWalk(false);
                        youngDarion->AI()->Talk(SAY_YOUNG_DARION_RETURNED);
                    }
                })
                .Schedule(82s, [this](TaskContext /*context*/)
                {
                    if (Creature* youngDarion = ObjectAccessor::GetCreature(*me, _youngDarionGUID))
                    {
                        youngDarion->AI()->Talk(EMOTE_YOUNG_DARION_SHADE);
                        youngDarion->GetMotionMaster()->MovePoint(0, YoungDarionHugPos, true, YoungDarionHugPos.GetAbsoluteAngle(AlexandrosForwardPos));
                    }
                })
                .Schedule(86s, [this](TaskContext /*context*/)
                {
                    if (Creature* youngDarion = ObjectAccessor::GetCreature(*me, _youngDarionGUID))
                        youngDarion->AI()->Talk(SAY_YOUNG_DARION_GONE_LONG);
                })
                .Schedule(90s, [this](TaskContext /*context*/)
                {
                    if (Creature* youngDarion = ObjectAccessor::GetCreature(*me, _youngDarionGUID))
                        youngDarion->AI()->Talk(EMOTE_YOUNG_DARION_HUGS);
                })
                .Schedule(94s, [this](TaskContext /*context*/)
                {
                    if (Creature* alexandros = ObjectAccessor::GetCreature(*me, _alexandrosGUID))
                        alexandros->AI()->Talk(SAY_ALEXANDROS_MY_HOME);
                })
                .Schedule(102s, [this](TaskContext /*context*/)
                {
                    if (Creature* youngDarion = ObjectAccessor::GetCreature(*me, _youngDarionGUID))
                        youngDarion->AI()->Talk(SAY_YOUNG_DARION_JOIN_THE_WAR);
                })
                .Schedule(110s, [this](TaskContext /*context*/)
                {
                    if (Creature* alexandros = ObjectAccessor::GetCreature(*me, _alexandrosGUID))
                        alexandros->AI()->Talk(SAY_ALEXANDROS_BARELY_OF_AGE);
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->GetMotionMaster()->MovePoint(0, TirionForwardPos, true, TirionForwardPos.GetAbsoluteAngle(AlexandrosForwardPos));
                })
                .Schedule(125s, [this](TaskContext /*context*/)
                {
                    if (Creature* youngDarion = ObjectAccessor::GetCreature(*me, _youngDarionGUID))
                        youngDarion->AI()->Talk(SAY_YOUNG_DARION_ON_MY_FEET);
                })
                .Schedule(136s, [this](TaskContext /*context*/)
                {
                    if (Creature* alexandros = ObjectAccessor::GetCreature(*me, _alexandrosGUID))
                        alexandros->AI()->Talk(SAY_ALEXANDROS_ASHBRINGER);
                })
                .Schedule(151s, [this](TaskContext /*context*/)
                {
                    if (Creature* youngDarion = ObjectAccessor::GetCreature(*me, _youngDarionGUID))
                        youngDarion->DespawnOrUnsummon();
                })
                .Schedule(165s, [this](TaskContext /*context*/)
                {
                    if (Creature* alexandros = ObjectAccessor::GetCreature(*me, _alexandrosGUID))
                        alexandros->AI()->Talk(SAY_ALEXANDROS_DO_NOT_FORGET);
                })
                .Schedule(171s, [this](TaskContext /*context*/)
                {
                    if (TempSummon* lichKing = me->SummonCreature(NPC_THE_LICH_KING, LichKingSpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20min))
                    {
                        lichKing->AI()->Talk(SAY_LICH_KING_TOUCHING);
                        if (Creature* alexandros = ObjectAccessor::GetCreature(*me, _alexandrosGUID))
                            lichKing->CastSpell(alexandros, SPELL_SOUL_FEAST_ALEXANDROS);
                    }
                })
                .Schedule(173s, [this](TaskContext /*context*/)
                {
                    if (Creature* alexandros = ObjectAccessor::GetCreature(*me, _alexandrosGUID))
                    {
                        alexandros->AI()->Talk(EMOTE_ALEXANDROS_DISAPPEARS);
                        alexandros->DespawnOrUnsummon();
                    }

                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                    {
                        lichKing->InterruptNonMeleeSpells(false);
                        lichKing->AI()->Talk(SAY_LICH_KING_HE_IS_MINE);
                    }
                })
                .Schedule(176s, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    Talk(EMOTE_DARION_ANGRY);
                    Talk(SAY_DARION_BETRAYED);
                })
                .Schedule(179s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->GetMotionMaster()->MovePoint(0, LichKingForwardPos, true, LichKingForwardPos.GetAbsoluteAngle(DarionChapelPos));
                })
                .Schedule(185s, [this](TaskContext /*context*/)
                {
                    me->SetWalk(false);
                    DoCastSelf(SPELL_MOGRAINE_CHARGE);
                    me->GetMotionMaster()->MovePoint(0, DarionChargePos, true, DarionChargePos.GetAbsoluteAngle(LichKingForwardPos));
                })
                .Schedule(186s, [this](TaskContext /*context*/)
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SPECIALATTACK1H);
                })
                .Schedule(187s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                    {
                        lichKing->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                        lichKing->AI()->Talk(SAY_LICH_KING_PATHETIC);
                    }

                    Position kickedPos = DarionKickedPos;
                    kickedPos.SetOrientation(DarionKickedPos.GetAbsoluteAngle(LichKingForwardPos));
                    me->GetMotionMaster()->MoveJump(kickedPos, 20.0f, 10.0f, EVENT_JUMP, true);
                })
                .Schedule(190s, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                })
                .Schedule(193s, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_MONSTER);
                })
                .Schedule(201s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->AI()->Talk(SAY_LICH_KING_YOU_WERE_RIGHT);
                })
                .Schedule(216s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->AI()->Talk(SAY_LICH_KING_HOW_SIMPLE);
                })
                .Schedule(233s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                    {
                        lichKing->AI()->Talk(EMOTE_LICH_KING_CASTS_ON_TIRION);
                        lichKing->AI()->Talk(SAY_LICH_KING_APOCALYPSE);
                        if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        {
                            tirion->SetStandState(UNIT_STAND_STATE_KNEEL);
                            lichKing->CastSpell(tirion, SPELL_SOUL_FEAST_TIRION);
                            tirion->AI()->Talk(EMOTE_TIRION_GASPS);
                        }
                    }
                })
                .Schedule(236s, [this](TaskContext /*context*/)
                {
                    Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID);
                    if (!lichKing)
                        return;

                    _chargerGUIDs = { _korfaxGUID, _maxwellGUID, _eligorGUID };
                    _chargerGUIDs.insert(_chargerGUIDs.end(), _outroDefenderGUIDs.begin(), _outroDefenderGUIDs.end());
                    for (ObjectGuid const& guid : _chargerGUIDs)
                    {
                        if (Creature* charger = ObjectAccessor::GetCreature(*me, guid))
                        {
                            Position const strikePos = lichKing->GetNearPosition(3.0f, lichKing->GetRelativeAngle(charger));
                            charger->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
                            charger->GetMotionMaster()->MoveCharge(strikePos.GetPositionX(), strikePos.GetPositionY(), strikePos.GetPositionZ());
                        }
                    }

                    if (Creature* maxwell = ObjectAccessor::GetCreature(*me, _maxwellGUID))
                        maxwell->AI()->Talk(SAY_MAXWELL_ATTACK);
                })
                .Schedule(242s, [this](TaskContext /*context*/)
                {
                    Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID);
                    if (!lichKing)
                        return;

                    lichKing->AI()->Talk(EMOTE_LICH_KING_KILLS_DEFENDERS);
                    for (ObjectGuid const& guid : _chargerGUIDs)
                    {
                        if (Creature* charger = ObjectAccessor::GetCreature(*me, guid))
                        {
                            charger->SetEmoteState(EMOTE_ONESHOT_NONE);
                            charger->SetStandState(UNIT_STAND_STATE_DEAD);
                            charger->GetMotionMaster()->MoveKnockbackFrom(lichKing->GetPositionX(), lichKing->GetPositionY(), 6.0f, 12.0f);
                        }
                    }
                })
                .Schedule(246s, [this](TaskContext /*context*/)
                {
                    Talk(SAY_DARION_NOT_TODAY);
                })
                .Schedule(251s, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    Talk(SAY_DARION_TIRION);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, DarionThrowPos, true, DarionThrowPos.GetAbsoluteAngle(TirionForwardPos));
                })
                .Schedule(253s, [this](TaskContext /*context*/)
                {
                    me->SetVirtualItem(0, EQUIP_UNEQUIP);
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        me->CastSpell(tirion, SPELL_ASHBRINGER, true);
                    Talk(EMOTE_DARION_THROWS_ASHBRINGER);
                })
                .Schedule(258s, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
                    Talk(EMOTE_DARION_COLLAPSES);
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->SetStandState(UNIT_STAND_STATE_STAND);
                        tirion->SetVirtualItem(0, EQUIP_ASHBRINGER);
                        tirion->CastSpell(tirion, SPELL_REBIRTH_OF_THE_ASHBRINGER);
                    }
                })
                .Schedule(259s, [this](TaskContext /*context*/)
                {
                    me->SummonGameObject(GO_LIGHT_OF_DAWN, LightOfDawnPos, QuaternionData(), 30s);
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->RemoveAurasDueToSpell(SPELL_REBIRTH_OF_THE_ASHBRINGER);
                        tirion->CastSpell(tirion, SPELL_SOUL_RELEASE);
                        tirion->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                    }

                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->InterruptNonMeleeSpells(false);
                })
                .Schedule(261500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_ARTHAS);
                })
                .Schedule(265500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->AI()->Talk(SAY_LICH_KING_WHAT_IS_THIS);
                })
                .Schedule(270500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_YOUR_END);
                })
                .Schedule(271500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->AI()->Talk(EMOTE_TIRION_CHARGES);
                        tirion->CastSpell(tirion, SPELL_TIRION_CHARGE);
                        tirion->SetEmoteState(EMOTE_STATE_READY2H);
                        tirion->SetWalk(false);
                        tirion->GetMotionMaster()->MovePoint(0, TirionChargePos, true, TirionChargePos.GetAbsoluteAngle(LichKingRecoverPos));
                    }

                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->GetMotionMaster()->MoveJump(LichKingKnockedPos, 10.0f, 5.0f);
                })
                .Schedule(273s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->AI()->Talk(SAY_LICH_KING_IMPOSSIBLE);
                })
                .Schedule(274s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->GetMotionMaster()->MovePoint(0, LichKingRecoverPos, true, LichKingRecoverPos.GetAbsoluteAngle(TirionChargePos));
                })
                .Schedule(278s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->SetStandState(UNIT_STAND_STATE_KNEEL);
                })
                .Schedule(280s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->SetStandState(UNIT_STAND_STATE_STAND);
                })
                .Schedule(281500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->AI()->Talk(SAY_LICH_KING_NOT_OVER);
                })
                .Schedule(291500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->AI()->Talk(SAY_LICH_KING_NEXT_WE_MEET);
                })
                .Schedule(301500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                        lichKing->CastSpell(lichKing, SPELL_TELEPORT_VISUAL);

                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->SetEmoteState(EMOTE_ONESHOT_NONE);
                        tirion->GetMotionMaster()->MovePoint(0, TirionRunPos, true, TirionRunPos.GetAbsoluteAngle(DarionThrowPos));
                    }
                })
                .Schedule(304s, [this](TaskContext /*context*/)
                {
                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, _lichKingGUID))
                    {
                        lichKing->AI()->Talk(EMOTE_LICH_KING_DISAPPEARS);
                        lichKing->DespawnOrUnsummon();
                    }
                })
                .Schedule(314s, [this](TaskContext /*context*/)
                {
                    std::pair<ObjectGuid, Position> const speechSpots[] = { { _korfaxGUID, KorfaxSpeechPos }, { _maxwellGUID, MaxwellSpeechPos }, { _eligorGUID, EligorSpeechPos } };
                    for (auto const& [guid, pos] : speechSpots)
                    {
                        if (Creature* champion = ObjectAccessor::GetCreature(*me, guid))
                        {
                            champion->SetStandState(UNIT_STAND_STATE_STAND);
                            champion->GetMotionMaster()->MovePoint(0, pos, true, pos.GetAbsoluteAngle(TirionSpeechPos));
                        }
                    }

                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_RISE_DARION);
                })
                .Schedule(317s, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->SetWalk(true);
                        tirion->GetMotionMaster()->MovePoint(0, TirionApproachPos, true, TirionApproachPos.GetAbsoluteAngle(me));
                    }
                })
                .Schedule(322500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->GetMotionMaster()->MovePoint(0, TirionSpeechPos, true, TirionSpeechPos.GetAbsoluteAngle(me));
                        tirion->AI()->Talk(SAY_TIRION_TERRIBLE_TRAGEDY);
                    }
                })
                .Schedule(337500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_REMAIN_VIGILANT);
                })
                .Schedule(344500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_MUST_ANSWER);
                })
                .Schedule(354500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_PROMISE);
                })
                .Schedule(365500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_COME_TOGETHER);
                })
                .Schedule(375500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_ICECROWN);
                })
                .Schedule(383500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_ARGENT_CRUSADE);
                })
                .Schedule(393500ms, [this](TaskContext /*context*/)
                {
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    Talk(SAY_DARION_EBON_BLADE);
                })
                .Schedule(403500ms, [this](TaskContext /*context*/)
                {
                    std::vector<Player*> players;
                    me->GetPlayerListInGrid(players, 100.0f, false);
                    for (Player* player : players)
                        player->CastSpell(player, SPELL_THE_LIGHT_OF_DAWN_CREDIT, true);
                })
                .Schedule(404500ms, [this](TaskContext /*context*/)
                {
                    for (ObjectGuid const& guid : { _koltiraGUID, _thassarianGUID })
                        if (Creature* hero = ObjectAccessor::GetCreature(*me, guid))
                            hero->DespawnOrUnsummon(0s, SceneRespawnHold);

                    for (uint32 entry : { NPC_KORFAX_CHAMPION_OF_THE_LIGHT, NPC_LORD_MAXWELL_TYROSUS, NPC_COMMANDER_ELIGOR_DAWNBRINGER, NPC_DEFENDER_OF_THE_LIGHT })
                        _summons.DespawnEntry(entry);

                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->DespawnOrUnsummon(3min);

                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_EVENT_STATE, EVENT_STATE_NOT_STARTED);

                    me->ReplaceAllNpcFlags(UNIT_NPC_FLAG_QUESTGIVER);
                    me->DespawnOrUnsummon(3min, 10s);
                });
                break;
            }
            default:
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);

        switch (summon->GetEntry())
        {
            case NPC_HIGHLORD_TIRION_FORDRING:
                _tirionGUID = summon->GetGUID();
                break;
            case NPC_KORFAX_CHAMPION_OF_THE_LIGHT:
                _korfaxGUID = summon->GetGUID();
                break;
            case NPC_LORD_MAXWELL_TYROSUS:
                _maxwellGUID = summon->GetGUID();
                break;
            case NPC_COMMANDER_ELIGOR_DAWNBRINGER:
                _eligorGUID = summon->GetGUID();
                break;
            case NPC_HIGHLORD_ALEXANDROS_MOGRAINE:
                _alexandrosGUID = summon->GetGUID();
                break;
            case NPC_DARION_MOGRAINE:
                _youngDarionGUID = summon->GetGUID();
                break;
            case NPC_THE_LICH_KING:
                _lichKingGUID = summon->GetGUID();
                break;
            default:
                break;
        }

        if (summon->GetEntry() == NPC_VOLATILE_GHOUL || summon->GetEntry() == NPC_WARRIOR_OF_THE_FROZEN_WASTES)
            summon->SetFaction(FACTION_UNDEAD_SCOURGE_3);
        else if (_battleInProgress && std::find(DawnForces.begin(), DawnForces.end(), summon->GetEntry()) != DawnForces.end())
            summon->SetFaction(FACTION_SCARLET_CRUSADE);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        _summons.Despawn(summon);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (!_battleInProgress)
            return;

        if (summon->GetFaction() != FACTION_SCARLET_CRUSADE && summon->GetFaction() != FACTION_UNDEAD_SCOURGE_3)
            return;

        uint32 const entry = summon->GetEntry();
        Position const pos = summon->GetPosition();
        _scheduler.Schedule(3s, GROUP_BATTLE, [this, entry, pos](TaskContext /*context*/)
        {
            if (_battleInProgress)
                me->SummonCreature(entry, pos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3s);
        });
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        if (damage >= me->GetHealth())
            damage = me->GetHealth() - 1;
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (!_eventInProgress)
            ScriptedAI::EnterEvadeMode(why);
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!me->IsImmuneToNPC() && UpdateVictim())
            DoMeleeAttackIfReady();
    }

private:
    GuidVector GetMarcherGUIDs()
    {
        GuidVector marchers(_summons.begin(), _summons.end());
        marchers.insert(marchers.end(), { _koltiraGUID, _orbazGUID, _thassarianGUID });
        marchers.insert(marchers.end(), _behemothGUIDs.begin(), _behemothGUIDs.end());
        return marchers;
    }

    TaskScheduler _scheduler;
    SummonList _summons;
    ObjectGuid _koltiraGUID;
    ObjectGuid _orbazGUID;
    ObjectGuid _thassarianGUID;
    ObjectGuid _tirionGUID;
    ObjectGuid _korfaxGUID;
    ObjectGuid _maxwellGUID;
    ObjectGuid _eligorGUID;
    ObjectGuid _alexandrosGUID;
    ObjectGuid _youngDarionGUID;
    ObjectGuid _lichKingGUID;
    GuidVector _behemothGUIDs;
    GuidVector _outroDefenderGUIDs;
    GuidVector _chargerGUIDs;
    bool _eventInProgress;
    bool _battleInProgress;
};

// 29175 - Highlord Tirion Fordring
// 29183 - The Lich King
struct npc_the_lich_king_tirion_dawn : public PassiveAI
{
    npc_the_lich_king_tirion_dawn(Creature* creature) : PassiveAI(creature) { }
};

// 29176 - Korfax, Champion of the Light
// 29177 - Commander Eligor Dawnbringer
// 29178 - Lord Maxwell Tyrosus
// 29179 - Leonid Barthalomew the Revered
// 29180 - Duke Nicholas Zverenhoff
// 29181 - Rayne
// 29182 - Rimblat Earthshatter
struct npc_light_of_dawn_champion : public ScriptedAI
{
    npc_light_of_dawn_champion(Creature* creature) : ScriptedAI(creature), _downPending(false), _downed(false) { }

    void Reset() override
    {
        _scheduler.CancelAll();
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        switch (me->GetEntry())
        {
            case NPC_KORFAX_CHAMPION_OF_THE_LIGHT:
                _scheduler.Schedule(10s, 20s, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_CLEAVE);
                    context.Repeat(10s);
                })
                .Schedule(10s, 20s, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_HEROIC_LEAP);
                    context.Repeat(10s);
                });
                break;
            case NPC_COMMANDER_ELIGOR_DAWNBRINGER:
                _scheduler.Schedule(1s, [this](TaskContext context)
                {
                    Unit* friendly = me->DoFindLowestHPFriendlyInRange(40.0f);
                    if (friendly && friendly->GetHealthPct() <= 20.0f)
                    {
                        DoCast(friendly, SPELL_HOLY_LIGHT);
                        context.Repeat(5s, 10s);
                    }
                    else
                        context.Repeat(1s);
                });
                break;
            case NPC_RAYNE:
                _scheduler.Schedule(10s, 20s, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_FIREBALL);
                    context.Repeat(10s, 20s);
                })
                .Schedule(10s, 20s, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_WRATH);
                    context.Repeat(10s, 20s);
                })
                .Schedule(1s, [this](TaskContext context)
                {
                    Unit* friendly = me->DoFindLowestHPFriendlyInRange(40.0f);
                    if (friendly && friendly->GetHealthPct() <= 20.0f)
                    {
                        DoCast(friendly, SPELL_REJUVENATION);
                        DoCastSelf(SPELL_TRANQUILITY);
                        context.Repeat(5s, 10s);
                    }
                    else
                        context.Repeat(1s);
                });
                break;
            case NPC_RIMBLAT_EARTHSHATTER:
                _scheduler.Schedule(10s, 20s, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_THUNDER);
                    context.Repeat(10s);
                })
                .Schedule(1s, [this](TaskContext context)
                {
                    Unit* friendly = me->DoFindLowestHPFriendlyInRange(40.0f);
                    if (friendly && friendly->GetHealthPct() <= 20.0f)
                    {
                        DoCast(friendly, SPELL_CHAIN_HEAL);
                        context.Repeat(5s, 10s);
                    }
                    else
                        context.Repeat(1s);
                });
                break;
            default:
                break;
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo*/) override
    {
        uint32 const minHealth = me->CountPctFromMaxHealth(1);
        if (me->GetHealth() <= minHealth + damage)
            damage = me->GetHealth() > minHealth ? me->GetHealth() - minHealth : 0;

        if (!_downed && me->HealthBelowPctDamaged(3, damage))
            _downPending = true;
    }

    void DoAction(int32 action) override
    {
        if (action != ACTION_REVIVE_CHAMPION)
            return;

        _downPending = false;
        if (_downed)
        {
            _downed = false;
            me->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
            me->SetImmuneToAll(false);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetEmoteState(EMOTE_ONESHOT_NONE);
        }

        me->SetFullHealth();
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (!_downed)
            ScriptedAI::EnterEvadeMode(why);
    }

    void UpdateAI(uint32 diff) override
    {
        if (_downPending)
        {
            _downPending = false;
            _downed = true;
            _scheduler.CancelAll();
            me->InterruptNonMeleeSpells(false);
            me->SetImmuneToAll(true);
            if (IsEngaged())
                EngagementOver();
            me->GetMotionMaster()->Clear();
            me->SetUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
            me->SetStandState(UNIT_STAND_STATE_DEAD);
            me->SetEmoteState(EMOTE_STATE_DEAD);
            Talk(EMOTE_CHAMPION_FALLS_UNCONSCIOUS);
            return;
        }

        if (_downed)
            return;

        _scheduler.Update(diff);

        if (UpdateVictim())
            DoMeleeAttackIfReady();
    }

private:
    TaskScheduler _scheduler;
    bool _downPending;
    bool _downed;
};

// 4298 - Plaguelands: The Scarlet Enclave
class zone_the_scarlet_enclave : public WorldZoneScript
{
public:
    zone_the_scarlet_enclave() : WorldZoneScript("zone_the_scarlet_enclave", MAP_SCARLET_ENCLAVE, { ZONE_THE_SCARLET_ENCLAVE }, PHASE_CHAPTER_IV_EPILOGUE), _worldStates({ { WORLD_STATE_FORCES_LIGHT, 0 }, { WORLD_STATE_FORCES_SCOURGE, 0 }, { WORLD_STATE_FORCES_SHOW, 0 }, { WORLD_STATE_COUNTDOWN_SHOW, 0 }, { WORLD_STATE_COUNTDOWN_TIME, 0 }, { WORLD_STATE_BATTLE_BEGUN, 0 } }), _eventState(EVENT_STATE_NOT_STARTED), _defendersKilled(0), _scourgeKilled(0), _abortPending(false), _sceneRespawnPending(false) { }

    void OnCreatureCreate(Creature* creature) override
    {
        if (TempSummon const* summon = creature->ToTempSummon())
        {
            if (!_darionGUID.IsEmpty() && summon->GetSummonerGUID() == _darionGUID)
                _eventSummonGUIDs.insert(creature->GetGUID());
            return;
        }

        switch (creature->GetEntry())
        {
            case NPC_HIGHLORD_DARION_MOGRAINE:
                _darionGUID = creature->GetGUID();
                _sceneRespawnPending = true;
                break;
            case NPC_KOLTIRA_DEATHWEAVER:
                _koltiraGUID = creature->GetGUID();
                break;
            case NPC_ORBAZ_BLOODBANE:
                _orbazGUID = creature->GetGUID();
                break;
            case NPC_THASSARIAN:
                _thassarianGUID = creature->GetGUID();
                break;
            case NPC_FLESH_BEHEMOTH:
                if (_behemothGUIDs[0].IsEmpty())
                    _behemothGUIDs[0] = creature->GetGUID();
                else if (_behemothGUIDs[1].IsEmpty())
                    _behemothGUIDs[1] = creature->GetGUID();
                break;
            default:
                break;
        }
    }

    void OnCreatureRemove(Creature* creature) override
    {
        ObjectGuid const guid = creature->GetGUID();
        _eventSummonGUIDs.erase(guid);

        if (guid == _darionGUID)
        {
            _darionGUID.Clear();
            if (_eventState != EVENT_STATE_NOT_STARTED)
                _abortPending = true;
        }
        else if (guid == _koltiraGUID)
            _koltiraGUID.Clear();
        else if (guid == _orbazGUID)
            _orbazGUID.Clear();
        else if (guid == _thassarianGUID)
            _thassarianGUID.Clear();
        else if (guid == _behemothGUIDs[0])
            _behemothGUIDs[0].Clear();
        else if (guid == _behemothGUIDs[1])
            _behemothGUIDs[1].Clear();
    }

    void OnUnitDeath(Unit* unit) override
    {
        if (_eventState != EVENT_STATE_BATTLE || !_eventSummonGUIDs.contains(unit->GetGUID()))
            return;

        switch (unit->GetEntry())
        {
            case NPC_DEFENDER_OF_THE_LIGHT:
                ++_defendersKilled;
                SetWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT - std::min<uint32>(_defendersKilled, MAX_FORCES_LIGHT));
                if (_defendersKilled >= DEFENDERS_KILLED_TO_END)
                    LoseBattle(ObjectAccessor::GetCreature(*unit, _darionGUID));
                break;
            case NPC_VOLATILE_GHOUL:
            case NPC_WARRIOR_OF_THE_FROZEN_WASTES:
                ++_scourgeKilled;
                SetWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE - std::min<uint32>(_scourgeKilled, MAX_FORCES_SCOURGE));
                break;
            default:
                break;
        }
    }

    ObjectGuid GetGuidData(uint32 type) const override
    {
        switch (type)
        {
            case DATA_KOLTIRA_DEATHWEAVER:
                return _koltiraGUID;
            case DATA_ORBAZ_BLOODBANE:
                return _orbazGUID;
            case DATA_THASSARIAN:
                return _thassarianGUID;
            case DATA_FLESH_BEHEMOTH_1:
                return _behemothGUIDs[0];
            case DATA_FLESH_BEHEMOTH_2:
                return _behemothGUIDs[1];
            default:
                return ObjectGuid::Empty;
        }
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type != DATA_EVENT_STATE || value == _eventState)
            return;

        _eventState = value;
        switch (_eventState)
        {
            case EVENT_STATE_NOT_STARTED:
                _eventTimer.Reset(0s);
                for (auto const& worldState : _worldStates)
                    SetWorldState(worldState.first, 0);
                break;
            case EVENT_STATE_COUNTDOWN:
                _defendersKilled = 0;
                _scourgeKilled = 0;
                _eventTimer.Reset(5min);
                SetWorldState(WORLD_STATE_FORCES_SHOW, 1);
                SetWorldState(WORLD_STATE_FORCES_LIGHT, MAX_FORCES_LIGHT);
                SetWorldState(WORLD_STATE_FORCES_SCOURGE, MAX_FORCES_SCOURGE);
                SetWorldState(WORLD_STATE_COUNTDOWN_SHOW, 1);
                SetWorldState(WORLD_STATE_COUNTDOWN_TIME, 5);
                break;
            case EVENT_STATE_BATTLE:
                _eventTimer.Reset(5min);
                break;
            default:
                break;
        }
    }

    void FillInitialWorldStates(Player* /*player*/, WorldPackets::WorldState::InitWorldStates& packet) override
    {
        for (auto const& [worldStateId, value] : _worldStates)
            packet.Worldstates.emplace_back(worldStateId, value);
    }

    void SendRemoveWorldStates(Player* player) override
    {
        for (auto const& worldState : _worldStates)
            player->SendUpdateWorldState(worldState.first, 0);
    }

    void OnUpdate(Map* map, uint32 diff) override
    {
        if (_abortPending)
        {
            _abortPending = false;
            SetData(DATA_EVENT_STATE, EVENT_STATE_NOT_STARTED);

            GuidUnorderedSet const summonGUIDs = std::move(_eventSummonGUIDs);
            _eventSummonGUIDs.clear();
            for (ObjectGuid const& guid : summonGUIDs)
                if (Creature* summon = map->GetCreature(guid))
                    summon->DespawnOrUnsummon();

            for (ObjectGuid const& guid : { _koltiraGUID, _orbazGUID, _thassarianGUID, _behemothGUIDs[0], _behemothGUIDs[1] })
                if (Creature* champion = map->GetCreature(guid))
                    champion->DespawnOrUnsummon(0s, 5s);
        }

        if (_sceneRespawnPending)
        {
            _sceneRespawnPending = false;

            std::vector<RespawnInfo const*> respawns;
            map->GetRespawnInfo(respawns, SPAWN_TYPEMASK_CREATURE);
            for (RespawnInfo const* respawn : respawns)
                if (std::find(SceneSpawns.begin(), SceneSpawns.end(), respawn->entry) != SceneSpawns.end())
                    map->Respawn(SPAWN_TYPE_CREATURE, respawn->spawnId);
        }

        _eventTimer.Update(diff);
        switch (_eventState)
        {
            case EVENT_STATE_COUNTDOWN:
                SetWorldState(WORLD_STATE_COUNTDOWN_TIME, uint32((_eventTimer.GetExpiry() + 1min - 1ms) / 1min));
                if (_eventTimer.Expired())
                {
                    _eventTimer.Reset(0s);
                    _eventState = EVENT_STATE_INTRO;
                    SetWorldState(WORLD_STATE_COUNTDOWN_SHOW, 0);
                    SetWorldState(WORLD_STATE_BATTLE_BEGUN, 1);
                    if (Creature* darion = map->GetCreature(_darionGUID))
                        darion->AI()->DoAction(ACTION_COUNTDOWN_FINISHED);
                }
                break;
            case EVENT_STATE_BATTLE:
                if (_eventTimer.Expired())
                    LoseBattle(map->GetCreature(_darionGUID));
                break;
            default:
                break;
        }

        if (_changedWorldStates.empty())
            return;

        for (ObjectGuid const& guid : GetPlayers())
            if (Player* player = ObjectAccessor::GetPlayer(map, guid))
                for (uint32 worldStateId : _changedWorldStates)
                    player->SendUpdateWorldState(worldStateId, _worldStates[worldStateId]);

        _changedWorldStates.clear();
    }

private:
    void SetWorldState(uint32 worldStateId, uint32 value)
    {
        auto itr = _worldStates.find(worldStateId);
        if (itr == _worldStates.end() || itr->second == value)
            return;

        itr->second = value;
        _changedWorldStates.insert(worldStateId);
    }

    void LoseBattle(Creature* darion)
    {
        _eventState = EVENT_STATE_OUTRO;
        _eventTimer.Reset(0s);
        if (darion)
            darion->AI()->DoAction(ACTION_BATTLE_LOST);
    }

    ObjectGuid _darionGUID;
    ObjectGuid _koltiraGUID;
    ObjectGuid _orbazGUID;
    ObjectGuid _thassarianGUID;
    std::array<ObjectGuid, 2> _behemothGUIDs;
    GuidUnorderedSet _eventSummonGUIDs;
    std::map<uint32, uint32> _worldStates;
    std::set<uint32> _changedWorldStates;
    TimeTracker _eventTimer;
    uint32 _eventState;
    uint32 _defendersKilled;
    uint32 _scourgeKilled;
    bool _abortPending;
    bool _sceneRespawnPending;
};

void AddSC_the_scarlet_enclave_c5()
{
    RegisterCreatureAI(npc_highlord_darion_mograine);
    RegisterCreatureAI(npc_the_lich_king_tirion_dawn);
    RegisterCreatureAI(npc_light_of_dawn_champion);
    new zone_the_scarlet_enclave();
}
