#include "Creature.h"
#include "CreatureAI.h"
#include "ObjectAccessor.h"
#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "StringFormat.h"
#include "TemporarySummon.h"UNIT_FLAG_UNINTERACTIBLE

enum TestDummyModes
{
    MODE_NULL,
    MODE_DPS
};

class EG_npc_damage_test_dummy : public CreatureScript
{
public:
    EG_npc_damage_test_dummy() : CreatureScript("EG_npc_damage_test_dummy") {}

    struct EG_npc_damage_test_dummyAI : public NullCreatureAI
    {
        friend class EG_npc_damage_test_controller;

        EG_npc_damage_test_dummyAI(Creature* creature) : NullCreatureAI(creature), _mode(MODE_NULL), _attemptCountdown(0), _attemptTimer(0), _attemptDuration(0), _healthUpdateInterval(1000), _rageGainInterval(3000) { }

        void InitializeAI() override
        {
            me->SetReactState(REACT_PASSIVE);
            me->SetMaxHealth(1000000000u);
            me->SetHealth(1000000000u);
            UpdateFlags();
        }

        void HealthUpdate()
        {
            uint32 hp = me->GetMaxHealth() / (_attemptDuration / IN_MILLISECONDS) * ((_attemptTimer + 999) / IN_MILLISECONDS);
            if (!hp)
                hp = 1;
            me->SetHealth(hp);
        }

        void RageGainTick()
        {
            std::list<Player*> players;
            me->GetPlayerListInGrid(players, 250.0f);
            for (Player* player : players)
            {
                if (player->GetPowerType() == POWER_RAGE && _attemptScores.find(player->GetGUID()) != _attemptScores.end())
                    player->RewardRage(3000, 1, false);
            }
        }

        void UpdateFlags()
        {
            if (_currentPlayer)
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PC);
            else
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PC);
        }

        void BeginAttempt(Player* target, TestDummyModes mode, Milliseconds timer)
        {
            _currentPlayer = target->GetGUID();
            _attemptScores.clear();
            _attemptCountdown = 10 * IN_MILLISECONDS;
            _attemptDuration = _attemptTimer = timer.count();
            _healthUpdateInterval = 1000;
            _rageGainInterval = 3000;
            me->SetLevel(std::min<uint8>(252, target->GetLevel()) + 3);
            UpdateFlags();
            me->Say(Trinity::StringFormat("OK, %s. Starting attempt in 10 seconds, get ready...", target->GetName().c_str()), LANG_UNIVERSAL);
        }

        void CancelAttempt()
        {
            me->Say("OK. Attempt canceled.", LANG_UNIVERSAL);
            _currentPlayer.Clear();
            me->GetThreatManager().ClearAllThreat();
            me->CombatStop();
            UpdateFlags();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!_currentPlayer)
                return;
            if (_attemptCountdown)
            {
                uint32 old = _attemptCountdown;
                if (_attemptCountdown > diff)
                    _attemptCountdown -= diff;
                else
                {
                    diff -= _attemptCountdown;
                    _attemptCountdown = 0;
                }
                char const* text = nullptr;
                if (!_attemptCountdown)
                    text = "Alright - GO!!!";
                else if (_attemptCountdown <= 1000 && old > 1000)
                    text = "1 second...";
                else if (_attemptCountdown <= 2000 && old > 2000)
                    text = "2 seconds...";
                else if (_attemptCountdown <= 3000 && old > 3000)
                    text = "3 seconds...";
                else if (_attemptCountdown <= 5000 && old > 5000)
                    text = "5 seconds...";
                else if (_attemptCountdown <= 7000 && old > 7000)
                    text = "7 seconds...";
                if (text)
                    me->Say(text, LANG_UNIVERSAL);
            }
            if (_attemptCountdown)
                return;
            if (diff < _attemptTimer)
            {
                _attemptTimer -= diff;
            }
            else
            {
                size_t const num = _attemptScores.size();
                if (num > 1)
                    me->Say(Trinity::StringFormat("Attempt concluded - %u players participated:", num), LANG_UNIVERSAL);
                else
                    me->Say("Attempt concluded:", LANG_UNIVERSAL);
                std::vector<std::pair<decltype(_attemptScores)::value_type::second_type::second_type, decltype(_attemptScores)::value_type::second_type::first_type>> _sortable;
                for (auto const& pair : _attemptScores)
                    _sortable.emplace_back(pair.second.second, pair.second.first);
                std::sort(_sortable.begin(), _sortable.end(), std::greater<decltype(_sortable)::value_type>());
                uint32 index = 0;
                for (auto const& pair : _sortable)
                {
                    if (num >= 10)
                        me->Say(Trinity::StringFormat("%02d. %s %s (per sec: %s)", ++index, pair.second, pretty(pair.first), pretty(pair.first / float(_attemptDuration / 1000))), LANG_UNIVERSAL);
                    else if (num > 1)
                        me->Say(Trinity::StringFormat("%d. %s %s (per sec: %s)", ++index, pair.second, pretty(pair.first), pretty(pair.first / float(_attemptDuration / 1000))), LANG_UNIVERSAL);
                    else
                        me->Say(Trinity::StringFormat("%s - %s (per sec: %s)", pair.second, pretty(pair.first), pretty(pair.first / float(_attemptDuration / 1000))), LANG_UNIVERSAL);
                }
                _currentPlayer.Clear();
                me->GetThreatManager().ClearAllThreat();
                me->CombatStop();
                UpdateFlags();
                return;
            }
            if (_healthUpdateInterval <= diff)
            {
                _healthUpdateInterval = 1000;
                HealthUpdate();
            }
            else
                _healthUpdateInterval -= diff;

            if (_rageGainInterval <= diff)
            {
                _rageGainInterval = 3000;
                RageGainTick();
            }
            else
                _rageGainInterval -= diff;
        }

        void DamageTaken(Unit* attacker, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
        {
            if (!_attemptCountdown && _attemptTimer)
            {
                ObjectGuid const who = attacker->GetCharmerOrOwnerOrOwnGUID();
                if (!who.IsPlayer())
                    return;
                auto it = _attemptScores.find(who);
                if (it != _attemptScores.end())
                    it->second.second += damage;
                else
                {
                    Player const* player = ObjectAccessor::GetPlayer(*attacker, who);
                    _attemptScores.emplace(who, decltype(_attemptScores)::value_type::second_type({ player ? player->GetName() : "<unknown>", damage }));
                }
            }
            damage = 0;
        }

        void EnterEvadeMode(EvadeReason why) { }

    private:
        std::string pretty(uint32 value)
        {
            std::string result;
            uint32 itr = 1000;
            while (itr <= value)
                itr *= 1000;
            itr /= 1000;
            result.append(std::to_string(value / itr));
            while (itr > 1)
            {
                value %= itr;
                itr /= 1000;
                result.append(Trinity::StringFormat(",%03d", value / itr));
            }
            return result;
        }

        std::string pretty(float value)
        {
            std::string result = pretty(uint32(value + 0.005f));
            result.append(Trinity::StringFormat("%.2f", value - std::floor(value)), 1);
            return result;
        }

        ObjectGuid _currentPlayer;
        TestDummyModes _mode;
        std::unordered_map<ObjectGuid, std::pair<std::string, uint32>> _attemptScores;
        uint32 _attemptCountdown;
        uint32 _attemptTimer;
        uint32 _attemptDuration;
        uint32 _healthUpdateInterval;
        uint32 _rageGainInterval;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new EG_npc_damage_test_dummyAI(creature);
    }
};

enum TestDummyGossipOffsets
{
    GOSSIP_OFFSET_CANCEL,
    GOSSIP_OFFSET_ATTEMPT_MENU,
    GOSSIP_OFFSET_ATTEMPT_DPS_30SEC,
    GOSSIP_OFFSET_ATTEMPT_DPS_150SEC,
    GOSSIP_OFFSET_ATTEMPT_DPS_360SEC,
    GOSSIP_OFFSET_AURA_MENU,
    GOSSIP_OFFSET_AURA_RESET_BUFFS,
    GOSSIP_OFFSET_AURA_RESET_DEBUFFS,
    GOSSIP_OFFSET_AURA_MENU_BUFFS,
    GOSSIP_OFFSET_AURA_MENU_DEBUFFS,
    GOSSIP_OFFSET_ALL_BUFFS,
    GOSSIP_OFFSET_ALL_DEBUFFS,
    GOSSIP_OFFSET_FIRST_BUFF = 1000,
    GOSSIP_OFFSET_FIRST_DEBUFF = 2000
};

struct TestDummyBuffInfo
{
    uint32 spellId;
    uint32 castDummy;
    TestDummyBuffInfo(uint32 id, uint32 dummy) : spellId(id), castDummy(dummy) { }
    TestDummyBuffInfo(uint32 id) : spellId(id), castDummy(0) { }
};

std::vector<TestDummyBuffInfo> const buffs =
{
    58646, // strength of earth
    48932, // blessing of might
    { 48936,1 }, // blessing of wisdom
    19506, // trueshot aura
    31583, // arcane empowerment
    { 20911,2 }, // blessing of sanctuary
    54043, // retribution aura (swift retribution)
    29801, // rampage
    8515, // windfury totem
    51470, // elemental oath
    54648, // focus magic
    48161, // power word: fortitude
    48469, // mark of the wild
    { 20217,3 }, // blessing of kings
    57669, // replenishment
    6562, // heroic presence
    73828, // strength of wrynn
    73822  // hellscream's warsong
};
std::vector<TestDummyBuffInfo> const debuffs =
{
    8647, // expose armor
    770, // faerie fire
    48564, // mangle
    54499, // heart of the crusader
    30070, // blood frenzy
    22959, // improved scorch
    47865, // curse of the elements
    33198, // misery
    20186, // judgement of wisdom
    { 20185,1 }, // judgement of light
    58181, // infected wounds
    772  // rend
};

int8 constexpr NUM_DUMMY = 3;

class EG_npc_damage_test_controller : public CreatureScript
{
public:
    EG_npc_damage_test_controller() : CreatureScript("EG_npc_damage_test_controller") {}

    struct EG_npc_damage_test_controllerAI : public NullCreatureAI
    {
        typedef EG_npc_damage_test_dummy::EG_npc_damage_test_dummyAI FriendAI;

        EG_npc_damage_test_controllerAI(Creature* creature) : NullCreatureAI(creature) { }

        void InitializeAI() override
        {
            bool isDummy = false;
            if (TempSummon* temp = me->ToTempSummon())
            {
                if (WorldObject* summoner = temp->GetSummoner())
                {
                    if (Creature* creaSummoner = summoner->ToCreature())
                        if (creaSummoner->GetEntry() == me->GetEntry())
                            isDummy = true;
                }
                else
                    isDummy = true;
            }
            if (isDummy) // we can't determine this in getai yet since creature isn't fully formed
            {
                me->AIM_Initialize(new npc_damage_test_buffdummyAI(me));
                return;
            }
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
            float x, y, z;
            for (int8 i = 0; i < NUM_DUMMY; ++i)
            {
                me->GetClosePoint(x, y, z, 1.0f, 0.0f, float((i + 1) * M_PI_2));
                dummy[i] = me->SummonCreature(me->GetEntry(), x, y, z + 2.0f, me->GetOrientation())->GetGUID();
            }
            Reset();
        }

        void Reset() override
        {
            me->AddAura(20045, me); // Improved Blessing of Might (Rank 2)
            me->AddAura(20245, me); // Improved Blessing of Wisdom (Rank 2);
            me->AddAura(52456, me); // Enhancing Totems (Rank 3)
            me->AddAura(53648, me); // Swift Retribution (Rank 3)
            me->AddAura(29193, me); // Improved Windfury Totem (Rank 2)
            me->AddAura(14767, me); // Improved Power Word: Fortitude (Rank 2)
            me->AddAura(17051, me); // Improved Mark of the Wild
        }

        void UpdateAI(uint32 diff)
        {
            if (_timer <= diff)
            {
                _timer = 5000;
                std::list<Player*> players;
                me->GetPlayerListInGrid(players, 250.0f);
                for (Player* player : players)
                    RefreshBuffs(player);
            }
            else
                _timer -= diff;
        }

        void GhettoSpellMod(Aura* buff, SpellEffIndex index)
        {
            SpellInfo const* buffInfo = buff->GetSpellInfo();
            if (!buffInfo->IsAffectedBySpellMods())
                return;
            AuraEffect* effect = buff->GetEffect(index);
            if (!effect)
                return;
            uint32 amount = effect->GetAmount();
            uint32 flat = 0;
            float mul = 1.0f;
            for (auto const& pair : me->GetOwnedAuras())
            {
                if (SpellInfo const* ourInfo = pair.second->GetSpellInfo())
                {
                    for (int8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                        if (AuraEffect* effect = pair.second->GetEffect(i))
                        {
                            if (!((effect->GetMiscValue() == SPELLMOD_ALL_EFFECTS) ||
                                ((index == EFFECT_0) && (effect->GetMiscValue() == SPELLMOD_EFFECT1)) &&
                                ((index == EFFECT_1) && (effect->GetMiscValue() == SPELLMOD_EFFECT2)) &&
                                ((index == EFFECT_2) && (effect->GetMiscValue() == SPELLMOD_EFFECT3))))
                                continue;

                            if (!effect->IsAffectedOnSpell(buffInfo))
                                continue;

                            switch (effect->GetAuraType())
                            {
                            case SPELL_AURA_ADD_PCT_MODIFIER:
                                mul += float(effect->GetAmount()) * 0.01f;
                                break;
                            case SPELL_AURA_ADD_FLAT_MODIFIER:
                                flat += effect->GetAmount();
                                break;
                            default:
                                break;
                            }
                        }
                }
            }
            effect->ChangeAmount(uint32(float(amount + flat) * mul));
        }

        void AddBuff(Unit* unit, TestDummyBuffInfo info)
        {
            Unit* caster = me;
            if (info.castDummy && info.castDummy <= NUM_DUMMY)
                caster = ObjectAccessor::GetUnit(*me, dummy[info.castDummy - 1]);
            if (!caster)
                return;
            Aura* buff = caster->AddAura(info.spellId, unit);
            if (!buff)
                return;
            GhettoSpellMod(buff, EFFECT_0);
            GhettoSpellMod(buff, EFFECT_1);
            GhettoSpellMod(buff, EFFECT_2);
            buff->SetMaxDuration(17 * IN_MILLISECONDS);
            buff->SetDuration(17 * IN_MILLISECONDS);
            for (Unit* minion : unit->m_Controlled)
                AddBuff(minion, info);
        }

        void RefreshBuffs(Unit* target)
        {
            for (auto const& pair : target->GetOwnedAuras())
                if (iscasterguid(pair.second->GetCasterGUID()))
                {
                    uint32 duration = pair.second->GetDuration();
                    while (duration < 16 * IN_MILLISECONDS)
                        duration += 1 * IN_MILLISECONDS;
                    pair.second->SetDuration(duration);
                }
            for (Unit* minion : target->m_Controlled)
                RefreshBuffs(minion);
        }

        void ClearAllBuffs(Unit* target)
        {
            std::list<Aura*> toRemove;
            for (auto const& pair : target->GetOwnedAuras())
                if (iscasterguid(pair.second->GetCasterGUID()))
                    toRemove.push_back(pair.second);
            for (Aura* aura : toRemove)
                aura->Remove();
            for (Unit* minion : target->m_Controlled)
                ClearAllBuffs(minion);
        }

        void AddDebuff(FriendAI* ai, TestDummyBuffInfo info)
        {
            Unit* caster = me;
            if (info.castDummy && info.castDummy <= NUM_DUMMY)
                caster = ObjectAccessor::GetUnit(*me, dummy[info.castDummy - 1]);
            if (!caster)
                return;
            Aura* debuff = caster->AddAura(info.spellId, const_cast<Creature*>(ai->me));
            if (!debuff)
                return;
            debuff->SetMaxDuration(-1);
            debuff->SetDuration(-1);
        }

        void ClearAllDebuffs(FriendAI* ai)
        {
            std::list<Aura*> toRemove;
            for (auto const& pair : ai->me->GetOwnedAuras())
                if (iscasterguid(pair.second->GetCasterGUID()))
                    toRemove.push_back(pair.second);
            for (Aura* aura : toRemove)
                aura->Remove();
        }

        void SendAuraMenu(Player* player, FriendAI* dummyAI)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "[Begin attempt]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_ATTEMPT_MENU);
            //if (HasAnyBuff(player))
                //AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Reset my buff settings", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_RESET_BUFFS);
            if (HasAnyDebuff(dummyAI))
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Reset debuff settings", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_RESET_DEBUFFS);
            //AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Add raid buffs to yourself", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_MENU_BUFFS);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Add raid debuffs to dummy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_MENU_DEBUFFS);
            SendGossipMenuFor(player, 7381, me->GetGUID()); // Hello friend.
        }

        void SendBuffMenu(Player* player, FriendAI* dummyAI)
        {
            if (!dummyAI->_currentPlayer)
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "[Go back]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_MENU);
            else if (HasAnyBuff(player))
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Reset my buff settings", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_RESET_BUFFS);
            bool main = false;
            for (uint32 i = 0, n = buffs.size(); i < n; ++i)
            {
                uint32 const spellId = buffs[i].spellId;
                if (UnavailableToTeam(player->GetTeamId(), spellId)) // heroic presence
                    continue;
                if (player->HasAura(spellId, casterguid(buffs[i])))
                    continue;
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);
                if (!spell)
                    continue;
                if (!main)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Add all raid buffs", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_ALL_BUFFS);
                    main = true;
                }
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, Trinity::StringFormat("Add '%s'", spell->SpellName[0]), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_FIRST_BUFF + i);
            }
            SendGossipMenuFor(player, 7381, me->GetGUID()); // Hello friend.
        }

        void SendDebuffMenu(Player* player, FriendAI* dummyAI)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "[Go back]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_MENU);
            bool main = false;
            for (uint32 i = 0, n = debuffs.size(); i < n; ++i)
            {
                uint32 const spellId = debuffs[i].spellId;
                if (dummyAI->me->HasAura(spellId, casterguid(debuffs[i])))
                    continue;
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);
                if (!spell)
                    continue;
                if (!main)
                {
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Add all raid debuffs", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_ALL_DEBUFFS);
                    main = true;
                }
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, Trinity::StringFormat("Add '%s'", spell->SpellName[0]), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_FIRST_DEBUFF + i);
            }
            SendGossipMenuFor(player, 7381, me->GetGUID());
        }

        bool OnGossipHello(Player* player) override
        {
            FriendAI* dummyAI = GetFriendAI();
            if (!dummyAI)
            {
                SendGossipMenuFor(player, player->GetTeam() == TEAM_ALLIANCE ? 13761 : 14172, me->GetGUID()); // Sorry, friend. Only certified officers of the H/A can authorize the purchase of a vehicle.
                return true;
            }

            ObjectGuid const& current = dummyAI->_currentPlayer;
            if (current)
            {
                if (player->GetGUID() == current)
                    AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Cancel current attempt", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_CANCEL);
                //AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Add raid buffs to yourself", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_MENU_BUFFS);
            }
            else
            {
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Begin attempt", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_ATTEMPT_MENU);
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Modify auras", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_MENU);
            }

            SendGossipMenuFor(player, 7381, me->GetGUID()); // Hello friend.
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 sender, uint32 listId) override
        {
            uint32 const action = GetGossipActionFor(player, listId);
            ClearGossipMenuFor(player);

            FriendAI* dummyAI = GetFriendAI();
            if (!dummyAI)
            {
                SendGossipMenuFor(player, player->GetTeam() == TEAM_ALLIANCE ? 13761 : 14172, me->GetGUID()); // Sorry, friend. Only certified officers of the H/A can authorize the purchase of a vehicle.
                return false;
            }

            ObjectGuid const& current = dummyAI->_currentPlayer;

            switch (action - GOSSIP_ACTION_INFO_DEF)
            {
            case GOSSIP_OFFSET_CANCEL:
                CloseGossipMenuFor(player);
                if (current == player->GetGUID())
                    dummyAI->CancelAttempt();
                break;
            case GOSSIP_OFFSET_ATTEMPT_MENU:
                if (current)
                    break;
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "[Modify auras]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_AURA_MENU);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Burst DPS (30 sec)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_ATTEMPT_DPS_30SEC);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Standard DPS (150 sec)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_ATTEMPT_DPS_150SEC);
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Sustained DPS (6 min)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_ATTEMPT_DPS_360SEC);
                SendGossipMenuFor(player, 7381, me->GetGUID()); // Hello friend.
                break;
            case GOSSIP_OFFSET_ATTEMPT_DPS_30SEC:
                if (current)
                    break;
                CloseGossipMenuFor(player);
                dummyAI->BeginAttempt(player, TestDummyModes::MODE_DPS, Seconds(30));
                break;
            case GOSSIP_OFFSET_ATTEMPT_DPS_150SEC:
                if (current)
                    break;
                CloseGossipMenuFor(player);
                dummyAI->BeginAttempt(player, TestDummyModes::MODE_DPS, Seconds(150));
                break;
            case GOSSIP_OFFSET_ATTEMPT_DPS_360SEC:
                if (current)
                    break;
                CloseGossipMenuFor(player);
                dummyAI->BeginAttempt(player, TestDummyModes::MODE_DPS, Minutes(6));
                break;
            case GOSSIP_OFFSET_AURA_RESET_BUFFS:
                ClearAllBuffs(player);
                if (dummyAI->_currentPlayer)
                    SendBuffMenu(player, dummyAI);
                else
                    SendAuraMenu(player, dummyAI);
                break;
            case GOSSIP_OFFSET_AURA_RESET_DEBUFFS:
                ClearAllDebuffs(dummyAI);
                SendAuraMenu(player, dummyAI);
                break;
            case GOSSIP_OFFSET_AURA_MENU:
                SendAuraMenu(player, dummyAI);
                break;
            case GOSSIP_OFFSET_AURA_MENU_BUFFS:
                SendBuffMenu(player, dummyAI);
                break;
            case GOSSIP_OFFSET_AURA_MENU_DEBUFFS:
                SendDebuffMenu(player, dummyAI);
                break;
            case GOSSIP_OFFSET_ALL_BUFFS:
                //for (TestDummyBuffInfo info : buffs)
                    //if (!UnavailableToTeam(player->GetTeamId(), info.spellId)) // heroic presence
                        //AddBuff(player, info);
                SendAuraMenu(player, dummyAI);
                break;
            case GOSSIP_OFFSET_ALL_DEBUFFS:
                for (TestDummyBuffInfo info : debuffs)
                    AddDebuff(dummyAI, info);
                SendAuraMenu(player, dummyAI);
                break;
            default:
                if (action >= GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_FIRST_BUFF)
                {
                    uint32 const offset = action - GOSSIP_ACTION_INFO_DEF - GOSSIP_OFFSET_FIRST_BUFF;
                    if (offset < buffs.size())
                    {
                        AddBuff(player, buffs[offset]);
                        SendBuffMenu(player, dummyAI);
                        break;
                    }
                }
                if (action >= GOSSIP_ACTION_INFO_DEF + GOSSIP_OFFSET_FIRST_DEBUFF)
                {
                    uint32 const offset = action - GOSSIP_ACTION_INFO_DEF - GOSSIP_OFFSET_FIRST_DEBUFF;
                    if (offset < debuffs.size())
                    {
                        AddDebuff(dummyAI, debuffs[offset]);
                        SendDebuffMenu(player, dummyAI);
                        break;
                    }
                }
                CloseGossipMenuFor(player);
                break;
            }
            return false;
        }

    private:
        ObjectGuid casterguid(TestDummyBuffInfo info) const
        {
            return (info.castDummy && info.castDummy <= NUM_DUMMY) ? dummy[info.castDummy - 1] : me->GetGUID();
        }

        bool iscasterguid(ObjectGuid const& guid) const
        {
            if (guid == me->GetGUID())
                return true;

            for (int8 i = 0; i < NUM_DUMMY; ++i)
                if (guid == dummy[i])
                    return true;

            return false;
        }

        bool HasAnyBuff(Unit* target) const
        {
            for (auto const& pair : target->GetOwnedAuras())
                if (iscasterguid(pair.second->GetCasterGUID()))
                    return true;
            for (Unit* minion : target->m_Controlled)
                if (HasAnyBuff(minion))
                    return true;
            return false;
        }

        bool HasAnyDebuff(FriendAI* ai) const
        {
            for (auto const& pair : ai->me->GetOwnedAuras())
                if (iscasterguid(pair.second->GetCasterGUID()))
                    return true;
            return false;
        }

        FriendAI* GetFriendAI() const
        {
            std::list<Creature*> list;
            me->GetCreatureListWithEntryInGrid(list, 0);
            for (Creature* c : list)
                if (c->IsAIEnabled())
                    if (auto* ai = dynamic_cast<FriendAI*>(c->AI()))
                        return ai;
            return nullptr;
        }

        bool UnavailableToTeam(TeamId team, uint32 spellId) const
        {
            switch (spellId)
            {
            case  6562: // heroic presence
            case 73828: // strength of wrynn
                return (team != TEAM_ALLIANCE);
            case 73822: // hellscream's warsong
                return (team != TEAM_HORDE);
            default:
                return false;
            }
        }

        ObjectGuid dummy[NUM_DUMMY];
        uint32 _timer = 5000;
    };

    struct npc_damage_test_buffdummyAI : public NullCreatureAI
    {
        npc_damage_test_buffdummyAI(Creature* creature) : NullCreatureAI(creature) { }

        void InitializeAI() override
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_NON_ATTACKABLE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new EG_npc_damage_test_controllerAI(creature);
    }
};

void AddSC_EG_gen_npc_scripts()
{
    new EG_npc_damage_test_controller();
    new EG_npc_damage_test_dummy();
}
