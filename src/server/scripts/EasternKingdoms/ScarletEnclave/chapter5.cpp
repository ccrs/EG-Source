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
#include "MapReference.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "TemporarySummon.h"
#include "WorldStatePackets.h"

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

    NPC_HIGHLORD_DARION_MOGRAINE = 29173,
    NPC_HIGHLORD_TIRION_FORDRING = 29175,
    NPC_KORFAX_CHAMPION_OF_THE_LIGHT = 29176,
    NPC_COMMANDER_ELIGOR_DAWNBRINGER = 29177,
    NPC_LORD_MAXWELL_TYROSUS = 29178,
    NPC_RAYNE = 29181,
    NPC_RIMBLAT_EARTHSHATTER = 29182,
    NPC_DEFENDER_OF_THE_LIGHT = 29174,
    NPC_THE_LICH_KING = 29183,
    NPC_RAMPAGING_ABOMINATION = 29186,
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

    SAY_DARION_HEAR_THE_CALL = 3,
    SAY_DARION_RISE = 4,
    SAY_DARION_SKIES_TURN_RED = 5,
    SAY_DARION_BATTLE = 8,
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

    WORLD_STATE_BATTLE_BEGUN = 3605,

    MAP_SCARLET_ENCLAVE = 609,
    ZONE_THE_SCARLET_ENCLAVE = 4298,
    PHASE_CHAPTER_IV_EPILOGUE = 128,

    DATA_EVENT_IN_PROGRESS = 1,
    DATA_KOLTIRA_DEATHWEAVER,
    DATA_ORBAZ_BLOODBANE,
    DATA_THASSARIAN,

    POINT_BATTLEFIELD = 1,
    POINT_CHAPEL,

    GROUP_BATTLE = 1
};

Position const DarionBattlefieldPos = { 2319.24f, -5266.49f, 82.825f };
Position const DarionChapelPos = { 2280.81f, -5284.09f, 82.608f };
Position const DarionKneelPos = { 2280.73f, -5286.84f, 82.93f };
Position const DarionChargePos = { 2281.46f, -5263.01f, 81.164f };
Position const DarionKickedPos = { 2257.48f, -5296.7f, 82.165f };
Position const DarionThrowPos = { 2261.24f, -5294.98f, 82.167f };

Position const DawnSpawnCenterPos = { 2296.335f, -5285.409f, 85.170f };
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

std::pair<uint32, uint8> const ScourgeArmy[] =
{
    { NPC_VOLATILE_GHOUL, 10 },
    { NPC_RAMPAGING_ABOMINATION, 3 },
    { NPC_WARRIOR_OF_THE_FROZEN_WASTES, 3 },
    { NPC_FLESH_BEHEMOTH, 2 }
};

// 29173 - Highlord Darion Mograine
struct npc_highlord_darion_mograine : public ScriptedAI
{
    npc_highlord_darion_mograine(Creature* creature) : ScriptedAI(creature), _summons(me), _eventInProgress(false), _battleInProgress(false) { }

    bool OnGossipSelect(Player* player, uint32 menuId, uint32 gossipListId) override
    {
        if (menuId != GOSSIP_MENU_LIGHT_OF_DAWN || gossipListId != GOSSIP_OPTION_START_BATTLE || _eventInProgress || !player->PlayerTalkClass->GetGossipMenu().GetItem(gossipListId))
            return false;

        CloseGossipMenuFor(player);

        _eventInProgress = true;
        me->setActive(true);
        me->ReplaceAllNpcFlags(UNIT_NPC_FLAG_NONE);

        if (ZoneScript* zoneScript = me->GetZoneScript())
        {
            zoneScript->SetData(DATA_EVENT_IN_PROGRESS, 1);
            _koltiraGUID = zoneScript->GetGuidData(DATA_KOLTIRA_DEATHWEAVER);
            _orbazGUID = zoneScript->GetGuidData(DATA_ORBAZ_BLOODBANE);
            _thassarianGUID = zoneScript->GetGuidData(DATA_THASSARIAN);
        }

        _scheduler.Schedule(3s, [this](TaskContext /*context*/)
        {
            Talk(SAY_DARION_HEAR_THE_CALL);
        })
        .Schedule(13s, [this](TaskContext /*context*/)
        {
            Talk(SAY_DARION_RISE);
        })
        .Schedule(16s, [this](TaskContext context)
        {
            uint32 index = context.GetRepeatCounter();
            for (auto const& [entry, count] : ScourgeArmy)
            {
                if (index < count)
                {
                    DoCastSelf(SPELL_SHAKE_CAMERA);
                    SummonScourge(entry, me->GetPosition());
                    context.Repeat(500ms);
                    return;
                }

                index -= count;
            }
        })
        .Schedule(27s, [this](TaskContext /*context*/)
        {
            Talk(SAY_DARION_SKIES_TURN_RED);
        })
        .Schedule(32s, [this](TaskContext /*context*/)
        {
            float const marchAngle = me->GetAbsoluteAngle(DarionBattlefieldPos);

            GuidVector marchers(_summons.begin(), _summons.end());
            marchers.insert(marchers.end(), { _koltiraGUID, _orbazGUID, _thassarianGUID });
            for (ObjectGuid const& guid : marchers)
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

        return true;
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
                DoCastSelf(SPELL_THE_MIGHT_OF_MOGRAINE, true);

                for (ObjectGuid const& guid : { _koltiraGUID, _orbazGUID, _thassarianGUID })
                    if (Creature* hero = ObjectAccessor::GetCreature(*me, guid))
                        hero->Dismount();

                for (uint8 i = 0; i < 30; ++i)
                    SummonDawn(NPC_DEFENDER_OF_THE_LIGHT);

                for (uint32 entry : { NPC_RIMBLAT_EARTHSHATTER, NPC_KORFAX_CHAMPION_OF_THE_LIGHT, NPC_LORD_MAXWELL_TYROSUS, NPC_COMMANDER_ELIGOR_DAWNBRINGER, NPC_RAYNE })
                    SummonDawn(entry);

                if (Creature* korfax = ObjectAccessor::GetCreature(*me, _korfaxGUID))
                    korfax->AI()->Talk(SAY_KORFAX_SCOURGE_APPROACH);
                if (Creature* maxwell = ObjectAccessor::GetCreature(*me, _maxwellGUID))
                    maxwell->AI()->Talk(SAY_MAXWELL_STAND_FAST);

                _scheduler.Schedule(1s, GROUP_BATTLE, [this](TaskContext context)
                {
                    std::vector<Creature*> scourge = { me };
                    std::vector<Creature*> dawn;

                    for (ObjectGuid const& guid : { _koltiraGUID, _orbazGUID, _thassarianGUID })
                    {
                        if (Creature* hero = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (hero->IsAlive())
                            {
                                hero->Dismount();
                                scourge.push_back(hero);
                            }
                        }
                    }

                    for (ObjectGuid const& guid : _summons)
                    {
                        if (Creature* summon = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (!summon->IsAlive())
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
                .Schedule(1s, 6s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastSelf(SPELL_ANTI_MAGIC_ZONE);
                    context.Repeat(25s, 30s);
                })
                .Schedule(5s, 10s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_DEATH_STRIKE);
                    context.Repeat(5s, 10s);
                })
                .Schedule(5s, 10s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastSelf(SPELL_DEATHS_EMBRACE);
                    context.Repeat(5s, 10s);
                })
                .Schedule(5s, 10s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastVictim(SPELL_ICY_TOUCH);
                    context.Repeat(5s, 10s);
                })
                .Schedule(5s, 10s, GROUP_BATTLE, [this](TaskContext context)
                {
                    DoCastSelf(SPELL_UNHOLY_BLIGHT);
                    context.Repeat(5s, 10s);
                })
                .Schedule(15s, GROUP_BATTLE, [this](TaskContext context)
                {
                    Talk(SAY_DARION_BATTLE);
                    context.Repeat(15s, 20s);
                })
                .Schedule(295s, GROUP_BATTLE, [this](TaskContext /*context*/)
                {
                    if (TempSummon* tirion = me->SummonCreature(NPC_HIGHLORD_TIRION_FORDRING, TirionSpawnPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20min))
                    {
                        tirion->SetFaction(me->GetFaction());
                        tirion->AI()->Talk(SAY_TIRION_CANNOT_WIN);
                    }
                })
                .Schedule(300s, GROUP_BATTLE, [this](TaskContext context)
                {
                    _battleInProgress = false;
                    context.CancelGroup(GROUP_BATTLE);

                    me->RemoveAllAuras();
                    me->CombatStop(true);
                    me->SetImmuneToNPC(true);
                    EngagementOver();

                    for (uint32 entry : { NPC_VOLATILE_GHOUL, NPC_RAMPAGING_ABOMINATION, NPC_WARRIOR_OF_THE_FROZEN_WASTES, NPC_FLESH_BEHEMOTH, NPC_DEFENDER_OF_THE_LIGHT, NPC_RIMBLAT_EARTHSHATTER, NPC_RAYNE })
                        _summons.DespawnEntry(entry);

                    std::pair<ObjectGuid, Position> const retreats[] = { { _korfaxGUID, KorfaxRetreatPos }, { _maxwellGUID, MaxwellRetreatPos }, { _eligorGUID, EligorRetreatPos }, { _koltiraGUID, KoltiraRetreatPos }, { _thassarianGUID, ThassarianRetreatPos } };
                    for (auto const& [guid, pos] : retreats)
                    {
                        if (Creature* retreating = ObjectAccessor::GetCreature(*me, guid))
                        {
                            if (!retreating->IsAlive())
                                continue;

                            retreating->SetFaction(me->GetFaction());
                            retreating->AI()->EnterEvadeMode();
                            retreating->Dismount();
                            retreating->SetWalk(false);
                            retreating->GetMotionMaster()->MovePoint(0, pos);
                        }
                    }

                    for (ObjectGuid const& guid : { _koltiraGUID, _thassarianGUID })
                        if (Creature* hero = ObjectAccessor::GetCreature(*me, guid))
                            if (hero->IsAlive())
                                hero->CastSpell(hero, SPELL_THE_LIGHT_OF_DAWN, true);

                    if (Creature* orbaz = ObjectAccessor::GetCreature(*me, _orbazGUID))
                    {
                        if (orbaz->IsAlive())
                        {
                            orbaz->AI()->Talk(EMOTE_ORBAZ_FLEES);
                            orbaz->AI()->EnterEvadeMode();
                        }

                        orbaz->DespawnOrUnsummon(5s, 30s);
                    }

                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->AI()->Talk(SAY_TIRION_BRING_THEM);

                    DoCastSelf(SPELL_THE_LIGHT_OF_DAWN, true);
                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(POINT_CHAPEL, DarionChapelPos);
                });
                break;
            }
            case POINT_CHAPEL:
            {
                Talk(EMOTE_DARION_KNEELS_IN_DEFEAT);
                me->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                me->SetWalk(true);
                me->GetMotionMaster()->MovePoint(0, DarionKneelPos);

                std::pair<ObjectGuid, Position> const gatherings[] = { { _korfaxGUID, KorfaxChapelPos }, { _maxwellGUID, MaxwellChapelPos }, { _eligorGUID, EligorChapelPos }, { _koltiraGUID, KoltiraChapelPos }, { _thassarianGUID, ThassarianChapelPos } };
                for (auto const& [guid, pos] : gatherings)
                {
                    if (Creature* gathering = ObjectAccessor::GetCreature(*me, guid))
                    {
                        gathering->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                        gathering->SetWalk(true);
                        gathering->GetMotionMaster()->MovePoint(0, pos);
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
                        alexandros->GetMotionMaster()->MovePoint(0, AlexandrosForwardPos);
                        alexandros->AI()->Talk(SAY_ALEXANDROS_MY_SON);
                    }

                    me->GetMotionMaster()->MovePoint(0, DarionChapelPos);
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
                        youngDarion->GetMotionMaster()->MovePoint(0, YoungDarionHugPos);
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
                        tirion->GetMotionMaster()->MovePoint(0, TirionForwardPos);
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
                .Schedule(141s, [this](TaskContext /*context*/)
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
                        lichKing->GetMotionMaster()->MovePoint(0, LichKingForwardPos);
                })
                .Schedule(185s, [this](TaskContext /*context*/)
                {
                    me->SetWalk(false);
                    DoCastSelf(SPELL_MOGRAINE_CHARGE);
                    me->GetMotionMaster()->MovePoint(0, DarionChargePos);
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

                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->GetMotionMaster()->MoveJump(DarionKickedPos, 20.0f, 10.0f);
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
                    for (uint32 entry : { NPC_DEFENDER_OF_THE_LIGHT, NPC_RIMBLAT_EARTHSHATTER })
                    {
                        if (TempSummon* charger = me->SummonCreature(entry, me->GetRandomPoint(TirionSpawnPos, 5.0f), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 10s))
                        {
                            charger->SetFaction(me->GetFaction());
                            _chargerGUIDs.push_back(charger->GetGUID());
                        }
                    }

                    for (ObjectGuid const& guid : _chargerGUIDs)
                    {
                        if (Creature* charger = ObjectAccessor::GetCreature(*me, guid))
                        {
                            charger->SetEmoteState(EMOTE_STATE_ATTACK_UNARMED);
                            charger->GetMotionMaster()->MoveCharge(lichKing->GetPositionX(), lichKing->GetPositionY(), lichKing->GetPositionZ());
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
                    me->GetMotionMaster()->MovePoint(0, DarionThrowPos);
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
                        tirion->GetMotionMaster()->MovePoint(0, TirionChargePos);
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
                        lichKing->GetMotionMaster()->MovePoint(0, LichKingRecoverPos);
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
                        tirion->GetMotionMaster()->MovePoint(0, TirionRunPos);
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
                            champion->GetMotionMaster()->MovePoint(0, pos);
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
                        tirion->GetMotionMaster()->MovePoint(0, TirionApproachPos);
                    }
                })
                .Schedule(322500ms, [this](TaskContext /*context*/)
                {
                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                    {
                        tirion->GetMotionMaster()->MovePoint(0, TirionSpeechPos);
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
                            hero->DespawnOrUnsummon(0s, 30s);

                    for (uint32 entry : { NPC_KORFAX_CHAMPION_OF_THE_LIGHT, NPC_LORD_MAXWELL_TYROSUS, NPC_COMMANDER_ELIGOR_DAWNBRINGER })
                        _summons.DespawnEntry(entry);

                    if (Creature* tirion = ObjectAccessor::GetCreature(*me, _tirionGUID))
                        tirion->DespawnOrUnsummon(3min);

                    if (ZoneScript* zoneScript = me->GetZoneScript())
                        zoneScript->SetData(DATA_EVENT_IN_PROGRESS, 0);

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
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        _summons.Despawn(summon);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (!_battleInProgress)
            return;

        if (summon->GetFaction() == FACTION_UNDEAD_SCOURGE_3)
            SummonScourge(summon->GetEntry(), DarionBattlefieldPos);
        else if (summon->GetFaction() == FACTION_SCARLET_CRUSADE)
            SummonDawn(summon->GetEntry());
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

        if (_battleInProgress && UpdateVictim())
            DoMeleeAttackIfReady();
    }

private:
    void SummonScourge(uint32 entry, Position const& center)
    {
        if (TempSummon* summon = me->SummonCreature(entry, me->GetRandomPoint(center, 20.0f), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20min))
            summon->SetFaction(FACTION_UNDEAD_SCOURGE_3);
    }

    void SummonDawn(uint32 entry)
    {
        if (TempSummon* summon = me->SummonCreature(entry, me->GetRandomPoint(DawnSpawnCenterPos, 15.0f), TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20min))
            summon->SetFaction(FACTION_SCARLET_CRUSADE);
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

// 4298 - Plaguelands: The Scarlet Enclave
class zone_the_scarlet_enclave : public WorldZoneScript
{
public:
    zone_the_scarlet_enclave() : WorldZoneScript("zone_the_scarlet_enclave", MAP_SCARLET_ENCLAVE, { ZONE_THE_SCARLET_ENCLAVE }, PHASE_CHAPTER_IV_EPILOGUE), _eventInProgress(false), _worldStateChanged(false), _abortPending(false) { }

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
            if (_eventInProgress)
            {
                _eventInProgress = false;
                _worldStateChanged = true;
                _abortPending = true;
            }
        }
        else if (guid == _koltiraGUID)
            _koltiraGUID.Clear();
        else if (guid == _orbazGUID)
            _orbazGUID.Clear();
        else if (guid == _thassarianGUID)
            _thassarianGUID.Clear();
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
            default:
                return ObjectGuid::Empty;
        }
    }

    void SetData(uint32 type, uint32 value) override
    {
        if (type != DATA_EVENT_IN_PROGRESS || _eventInProgress == (value != 0))
            return;

        _eventInProgress = value != 0;
        _worldStateChanged = true;
    }

    void FillInitialWorldStates(Player* player, WorldPackets::WorldState::InitWorldStates& packet) override
    {
        if (player->InSamePhase(GetPhaseMask()))
            packet.Worldstates.emplace_back(WORLD_STATE_BATTLE_BEGUN, _eventInProgress ? 1 : 0);
    }

    void OnUpdate(Map* map, uint32 /*diff*/) override
    {
        if (_abortPending)
        {
            _abortPending = false;

            GuidUnorderedSet const summonGUIDs = std::move(_eventSummonGUIDs);
            _eventSummonGUIDs.clear();
            for (ObjectGuid const& guid : summonGUIDs)
                if (Creature* summon = map->GetCreature(guid))
                    summon->DespawnOrUnsummon();

            for (ObjectGuid const& guid : { _koltiraGUID, _orbazGUID, _thassarianGUID })
                if (Creature* hero = map->GetCreature(guid))
                    hero->DespawnOrUnsummon(0s, 5s);
        }

        if (_worldStateChanged)
        {
            _worldStateChanged = false;
            for (MapReference const& ref : map->GetPlayers())
                if (Player* player = ref.GetSource())
                    if (HasZone(player->GetZoneId()) && player->InSamePhase(GetPhaseMask()))
                        player->SendUpdateWorldState(WORLD_STATE_BATTLE_BEGUN, _eventInProgress ? 1 : 0);
        }
    }

private:
    ObjectGuid _darionGUID;
    ObjectGuid _koltiraGUID;
    ObjectGuid _orbazGUID;
    ObjectGuid _thassarianGUID;
    GuidUnorderedSet _eventSummonGUIDs;
    bool _eventInProgress;
    bool _worldStateChanged;
    bool _abortPending;
};

void AddSC_the_scarlet_enclave_c5()
{
    RegisterCreatureAI(npc_highlord_darion_mograine);
    RegisterCreatureAI(npc_the_lich_king_tirion_dawn);
    new zone_the_scarlet_enclave();
}
