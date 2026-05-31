#include "Creature.h"
#include "CreatureAI.h"
#include "CommonHelpers.h"
#include "Containers.h"
#include "GameObject.h"
#include "GameObjectData.h"
#include "MotionMaster.h"
#include "ObjectAccessor.h"
#include "PassiveAI.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "SmartAI.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "StringFormat.h"
#include "TemporarySummon.h"
#include "Vehicle.h"

enum TestDummyModes
{
    MODE_NULL,
    MODE_DPS
};

struct EG_npc_damage_test_dummy : public NullCreatureAI
{
    friend struct EG_npc_damage_test_controller;

    EG_npc_damage_test_dummy(Creature* creature) : NullCreatureAI(creature), _mode(MODE_NULL), _attemptCountdown(0), _attemptTimer(0), _attemptDuration(0), _healthUpdateInterval(1000), _rageGainInterval(3000) { }

    void InitializeAI() override
    {
        me->SetReactState(REACT_PASSIVE);
        me->SetRegenerateHealth(false);
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
        if (!_currentPlayer.IsEmpty())
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE_2);
        else
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE_2);
    }

    void BeginAttempt(Player* target, TestDummyModes /*mode*/, Milliseconds timer)
    {
        _currentPlayer = target->GetGUID();
        _attemptScores.clear();
        _attemptCountdown = 10 * IN_MILLISECONDS;
        _attemptDuration = _attemptTimer = timer.count();
        _healthUpdateInterval = 1000;
        _rageGainInterval = 3000;
        me->SetLevel(std::min<uint8>(252, target->GetLevel()) + 3);
        me->SetHealth(me->GetMaxHealth()); // snap bar to full so the 10s countdown shows a healthy dummy
        UpdateFlags();
        me->Say(Trinity::StringFormat("OK, {}. Starting attempt in 10 seconds, get ready...", target->GetName()), LANG_UNIVERSAL);
    }

    void CancelAttempt()
    {
        me->Say("OK. Attempt canceled.", LANG_UNIVERSAL);
        _currentPlayer.Clear();
        me->GetThreatManager().ClearAllThreat();
        me->CombatStop();
        me->SetHealth(me->GetMaxHealth());
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
                me->Say(Trinity::StringFormat("Attempt concluded - {} players participated:", num), LANG_UNIVERSAL);
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
                    me->Say(Trinity::StringFormat("{:02}. {} {} (per sec: {})", ++index, pair.second, pretty(pair.first), pretty(pair.first / float(_attemptDuration / 1000))), LANG_UNIVERSAL);
                else if (num > 1)
                    me->Say(Trinity::StringFormat("{}. {} {} (per sec: {})", ++index, pair.second, pretty(pair.first), pretty(pair.first / float(_attemptDuration / 1000))), LANG_UNIVERSAL);
                else
                    me->Say(Trinity::StringFormat("{} - {} (per sec: {})", pair.second, pretty(pair.first), pretty(pair.first / float(_attemptDuration / 1000))), LANG_UNIVERSAL);
            }
            _currentPlayer.Clear();
            me->GetThreatManager().ClearAllThreat();
            me->CombatStop();
            me->SetHealth(me->GetMaxHealth());
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

    void EnterEvadeMode(EvadeReason /*why*/) override { }

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
            result.append(Trinity::StringFormat(",{:03}", value / itr));
        }
        return result;
    }

    std::string pretty(float value)
    {
        std::string result = pretty(uint32(value + 0.005f));
        result.append(Trinity::StringFormat("{:.2f}", value - std::floor(value)), 1);
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

enum TestDummyGossipOffsets : uint32
{
    GOSSIP_OFFSET_CANCEL,
    GOSSIP_OFFSET_ATTEMPT_MENU,
    GOSSIP_OFFSET_ATTEMPT_DPS_30SEC,
    GOSSIP_OFFSET_ATTEMPT_DPS_150SEC,
    GOSSIP_OFFSET_ATTEMPT_DPS_360SEC,
    GOSSIP_OFFSET_AURA_MENU,
    GOSSIP_OFFSET_AURA_RESET_DEBUFFS,
    GOSSIP_OFFSET_AURA_MENU_DEBUFFS,
    GOSSIP_OFFSET_ALL_DEBUFFS,
    GOSSIP_OFFSET_FIRST_DEBUFF = 2000
};

struct TestDummyBuffInfo
{
    uint32 spellId;
    uint32 castDummy;
    TestDummyBuffInfo(uint32 id, uint32 dummy) : spellId(id), castDummy(dummy) { }
    TestDummyBuffInfo(uint32 id) : spellId(id), castDummy(0) { }
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

struct npc_damage_test_buffdummy : public NullCreatureAI
{
    npc_damage_test_buffdummy(Creature* creature) : NullCreatureAI(creature) { }

    void InitializeAI() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_UNINTERACTIBLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2);
    }
};

struct EG_npc_damage_test_controller : public NullCreatureAI
{
    typedef EG_npc_damage_test_dummy FriendAI;

    EG_npc_damage_test_controller(Creature* creature) : NullCreatureAI(creature) { }

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
            me->AIM_Initialize(new npc_damage_test_buffdummy(me));
            return;
        }
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NON_ATTACKABLE_2);
        float x, y, z;
        for (int8 i = 0; i < NUM_DUMMY; ++i)
        {
            me->GetClosePoint(x, y, z, 1.0f, 0.0f, float((i + 1) * M_PI_2));
            _Dummy[i] = me->SummonCreature(me->GetEntry(), x, y, z + 2.0f, me->GetOrientation())->GetGUID();
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

    bool OnGossipHello(Player* player) override
    {
        FriendAI* dummyAI = _GetFriendAI();
        if (!dummyAI)
        {
            SendGossipMenuFor(player, player->GetTeam() == TEAM_ALLIANCE ? 13761 : 14172, me->GetGUID()); // Sorry, friend. Only certified officers of the H/A can authorize the purchase of a vehicle.
            return true;
        }

        ObjectGuid const& current = dummyAI->_currentPlayer;
        if (!current.IsEmpty())
        {
            if (player->GetGUID() == current)
                AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Cancel current attempt", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_CANCEL));
        }
        else
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Begin attempt", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_ATTEMPT_MENU));
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Modify auras", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_AURA_MENU));
        }

        SendGossipMenuFor(player, 7381, me->GetGUID()); // Hello friend.
        return true;
    }

    bool OnGossipSelect(Player* player, uint32 /*sender*/, uint32 listId) override
    {
        uint32 const action = GetGossipActionFor(player, listId);
        ClearGossipMenuFor(player);

        FriendAI* dummyAI = _GetFriendAI();
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
            if (!current.IsEmpty())
                break;
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "[Modify auras]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_AURA_MENU));
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Burst DPS (30 sec)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_ATTEMPT_DPS_30SEC));
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Standard DPS (150 sec)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_ATTEMPT_DPS_150SEC));
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Sustained DPS (6 min)", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_ATTEMPT_DPS_360SEC));
            SendGossipMenuFor(player, 7381, me->GetGUID()); // Hello friend.
            break;
        case GOSSIP_OFFSET_ATTEMPT_DPS_30SEC:
            if (!current.IsEmpty())
                break;
            CloseGossipMenuFor(player);
            dummyAI->BeginAttempt(player, TestDummyModes::MODE_DPS, Seconds(30));
            break;
        case GOSSIP_OFFSET_ATTEMPT_DPS_150SEC:
            if (!current.IsEmpty())
                break;
            CloseGossipMenuFor(player);
            dummyAI->BeginAttempt(player, TestDummyModes::MODE_DPS, Seconds(150));
            break;
        case GOSSIP_OFFSET_ATTEMPT_DPS_360SEC:
            if (!current.IsEmpty())
                break;
            CloseGossipMenuFor(player);
            dummyAI->BeginAttempt(player, TestDummyModes::MODE_DPS, Minutes(6));
            break;
        case GOSSIP_OFFSET_AURA_RESET_DEBUFFS:
            _ClearAllDebuffs(dummyAI);
            _SendAuraMenu(player, dummyAI);
            break;
        case GOSSIP_OFFSET_AURA_MENU:
            _SendAuraMenu(player, dummyAI);
            break;
        case GOSSIP_OFFSET_AURA_MENU_DEBUFFS:
            _SendDebuffMenu(player, dummyAI);
            break;
        case GOSSIP_OFFSET_ALL_DEBUFFS:
            for (TestDummyBuffInfo info : debuffs)
                _AddDebuff(dummyAI, info);
            _SendAuraMenu(player, dummyAI);
            break;
        default:
            if (action >= GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_FIRST_DEBUFF))
            {
                uint32 const offset = action - GOSSIP_ACTION_INFO_DEF - GOSSIP_OFFSET_FIRST_DEBUFF;
                if (offset < debuffs.size())
                {
                    _AddDebuff(dummyAI, debuffs[offset]);
                    _SendDebuffMenu(player, dummyAI);
                    break;
                }
            }
            CloseGossipMenuFor(player);
            break;
        }
        return false;
    }

private:
    ObjectGuid _CasterGUID(TestDummyBuffInfo info) const
    {
        return (info.castDummy && info.castDummy <= NUM_DUMMY) ? _Dummy[info.castDummy - 1] : me->GetGUID();
    }

    bool _IsCasterGUID(ObjectGuid const& guid) const
    {
        if (guid == me->GetGUID())
            return true;

        for (int8 i = 0; i < NUM_DUMMY; ++i)
            if (guid == _Dummy[i])
                return true;

        return false;
    }

    bool _HasAnyDebuff(FriendAI* ai) const
    {
        for (auto const& pair : ai->me->GetOwnedAuras())
            if (_IsCasterGUID(pair.second->GetCasterGUID()))
                return true;
        return false;
    }

    FriendAI* _GetFriendAI() const
    {
        std::list<Creature*> list;
        me->GetCreatureListWithEntryInGrid(list, 0);
        for (Creature* c : list)
            if (c->IsAIEnabled())
                if (auto* ai = dynamic_cast<FriendAI*>(c->AI()))
                    return ai;
        return nullptr;
    }

    void _AddDebuff(FriendAI* ai, TestDummyBuffInfo info)
    {
        Unit* caster = me;
        if (info.castDummy && info.castDummy <= NUM_DUMMY)
            caster = ObjectAccessor::GetUnit(*me, _Dummy[info.castDummy - 1]);
        if (!caster)
            return;
        Aura* debuff = caster->AddAura(info.spellId, const_cast<Creature*>(ai->me));
        if (!debuff)
            return;
        debuff->SetMaxDuration(-1);
        debuff->SetDuration(-1);
    }

    void _ClearAllDebuffs(FriendAI* ai)
    {
        std::list<Aura*> toRemove;
        for (auto const& pair : ai->me->GetOwnedAuras())
            if (_IsCasterGUID(pair.second->GetCasterGUID()))
                toRemove.push_back(pair.second);
        for (Aura* aura : toRemove)
            aura->Remove();
    }

    void _SendAuraMenu(Player* player, FriendAI* dummyAI)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "[Begin attempt]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_ATTEMPT_MENU));

        if (_HasAnyDebuff(dummyAI))
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Reset debuff settings", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_AURA_RESET_DEBUFFS));

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Add raid debuffs to dummy", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_AURA_MENU_DEBUFFS));
        SendGossipMenuFor(player, 7381, me->GetGUID()); // Hello friend.
    }

    void _SendDebuffMenu(Player* player, FriendAI* dummyAI)
    {
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "[Go back]", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_AURA_MENU));
        bool main = false;
        for (uint32 i = 0, n = debuffs.size(); i < n; ++i)
        {
            uint32 const spellId = debuffs[i].spellId;
            if (dummyAI->me->HasAura(spellId, _CasterGUID(debuffs[i])))
                continue;
            SpellInfo const* spell = sSpellMgr->GetSpellInfo(spellId);
            if (!spell)
                continue;
            if (!main)
            {
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Add all raid debuffs", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_ALL_DEBUFFS));
                main = true;
            }
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, Trinity::StringFormat("Add '{}'", spell->SpellName[0]), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + AsUnderlyingType(GOSSIP_OFFSET_FIRST_DEBUFF) + i);
        }
        SendGossipMenuFor(player, 7381, me->GetGUID());
    }

    ObjectGuid _Dummy[NUM_DUMMY];
};

enum EvolvingEctoplasm
{
    SPELL_IMMUNITY_SHADOW       = 7743,
    SPELL_IMMUNITY_FROST        = 7940,
    SPELL_IMMUNITY_NATURE       = 7941,
    SPELL_IMMUNITY_FIRE         = 7942,

    SPELL_TRANSFORM_BLACK       = 7946,
    SPELL_TRANSFORM_BLUE        = 7944,
    SPELL_TRANSFORM_GREEN       = 7945,
    SPELL_TRANSFORM_RED         = 7943
};

struct EG_npc_evolving_ectoplasm : public ScriptedAI
{
    EG_npc_evolving_ectoplasm(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _scheduler.CancelAll();
        DoCastSelf(EvolvingEctoplasm::SPELL_IMMUNITY_FROST);
        DoCastSelf(EvolvingEctoplasm::SPELL_TRANSFORM_BLUE);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(10s, 15s, [this](TaskContext task)
        {
            me->RemoveAllAuras();

            if (_color)
            {
                std::list<uint8> colors{ 0, 1, 2, 3 };
                colors.remove(_color);
                _color = Trinity::Containers::SelectRandomContainerElement(colors);
            }
            else
                _color = urand(0, 3);

            switch (_color)
            {
                case 0:
                    DoCastSelf(EvolvingEctoplasm::SPELL_IMMUNITY_FROST);
                    DoCastSelf(EvolvingEctoplasm::SPELL_TRANSFORM_BLUE);
                    break;
                case 1:
                    DoCastSelf(EvolvingEctoplasm::SPELL_IMMUNITY_SHADOW);
                    DoCastSelf(EvolvingEctoplasm::SPELL_TRANSFORM_BLACK);
                    break;
                case 2:
                    DoCastSelf(EvolvingEctoplasm::SPELL_IMMUNITY_NATURE);
                    DoCastSelf(EvolvingEctoplasm::SPELL_TRANSFORM_GREEN);
                    break;
                case 3:
                    DoCastSelf(EvolvingEctoplasm::SPELL_IMMUNITY_FIRE);
                    DoCastSelf(EvolvingEctoplasm::SPELL_TRANSFORM_RED);
                    break;
                default:
                    break;
            }

            task.Repeat();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this]
        {
            DoMeleeAttackIfReady();
        });
    }

private:
    TaskScheduler _scheduler;
    uint8 _color;
};

enum PlagueSlime
{
    SPELL_PLAGUESLIME_IMMUNITY_SHADOW       = 7743,
    SPELL_PLAGUESLIME_IMMUNITY_FROST        = 7940,
    SPELL_PLAGUESLIME_IMMUNITY_FIRE         = 7942,
    SPELL_PLAGUESLIME_IMMUNITY_NATURE       = 7941,

    SPELL_PLAGUESLIME_TRANSFORM_BLACK       = 28987,
    SPELL_PLAGUESLIME_TRANSFORM_BLUE        = 28988,
    SPELL_PLAGUESLIME_TRANSFORM_RED         = 28990,
    SPELL_PLAGUESLIME_TRANSFORM_GREEN       = 28989
};

struct EG_npc_plague_slime : public ScriptedAI
{
    enum SlimeColor : uint8
    {
        COLOR_BLACK = 0,
        COLOR_BLUE,
        COLOR_GREEN,
        COLOR_RED
    };

    EG_npc_plague_slime(Creature* creature) : ScriptedAI(creature), _color(COLOR_BLACK) { }

    void Reset() override
    {
        _scheduler.CancelAll();
        _color = urand(COLOR_BLACK, COLOR_RED);
        _ApplyColor(_color);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(25s, 30s, [this](TaskContext task)
        {
            // Pick any color different from the current one — random rotation, no fixed cycle.
            std::list<uint8> colors{ COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_RED };
            colors.remove(_color);
            _color = Trinity::Containers::SelectRandomContainerElement(colors);

            me->RemoveAllAuras();
            _ApplyColor(_color);

            task.Repeat();
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this]
        {
            DoMeleeAttackIfReady();
        });
    }

private:
    void _ApplyColor(uint8 color)
    {
        switch (color)
        {
            case COLOR_BLACK:
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_IMMUNITY_SHADOW);
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_TRANSFORM_BLACK);
                break;
            case COLOR_BLUE:
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_IMMUNITY_FROST);
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_TRANSFORM_BLUE);
                break;
            case COLOR_GREEN:
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_IMMUNITY_NATURE);
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_TRANSFORM_GREEN);
                break;
            case COLOR_RED:
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_IMMUNITY_FIRE);
                DoCastSelf(PlagueSlime::SPELL_PLAGUESLIME_TRANSFORM_RED);
                break;
            default:
                break;
        }
    }

    TaskScheduler _scheduler;
    uint8 _color;
};

enum CrystallineFrayerMisc
{
    SPELL_AURA_OF_REGENERATION = 52067,
    SPELL_AURA_OF_REGENERATION_HC = 57056,
    SPELL_CRYSTAL_BLOOM = 48058,
    SPELL_ENSNARE = 48053,
    SPELL_SEED_POD = 48082,
    SPELL_SUMM_SEED_POD = 52796,

    GO_POD = 191016,

    EVENT_ENSNARE = 1,
    EVENT_AURA_CHECK = 2,

    DATA_ORMOROK = 3,
};

struct EG_npc_crystalline_frayer : public ScriptedAI
{
    EG_npc_crystalline_frayer(Creature* creature) : ScriptedAI(creature)
    {
        _setPod = 0;
    }

    void Reset() override
    {
        me->SetVisible(true);
        _events.Reset();
        if (GameObject* pod = ObjectAccessor::GetGameObject(*me, _pod))
        {
            pod->SetRespawnTime(0);
            pod->Delete();
        }
        _pod.Clear();
        me->RemoveAllAuras();
        _setPod = 0;
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        DoCast(me, SPELL_CRYSTAL_BLOOM);
        _events.ScheduleEvent(EVENT_ENSNARE, 2s, 5s);
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (_setPod == 0 && damage >= me->GetHealth())
        {
            me->RemoveAllAuras();
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
            me->SetImmuneToAll(true);
            me->SetReactState(REACT_PASSIVE);
            _setPod = 1;
            damage = 0;
            EnterEvadeMode(EVADE_REASON_OTHER);
            DoCast(me, SPELL_SEED_POD, true);
            DoCast(me, IsHeroic() ? SPELL_AURA_OF_REGENERATION_HC : SPELL_AURA_OF_REGENERATION, true);
            _SummPod();
            me->SetVisible(false);
            _events.Reset();
            _events.ScheduleEvent(EVENT_AURA_CHECK, 1s);
        }
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (!_EnterEvadeMode(why))
            return;

        if (_setPod == 1)
            return;

        if (why == EVADE_REASON_VEHICLE_EVADE)
        {
            Reset();
            return;
        }

        if (me->GetVehicle()) // otherwise me will be in evade mode forever
        {
            Reset();
            return;
        }

        if (Unit* owner = me->GetCharmerOrOwner())
        {
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle());
        }
        else
        {
            me->AddUnitState(UNIT_STATE_EVADE);
            me->GetMotionMaster()->MoveTargetedHome();
        }

        Reset();
    }

    void UpdateAI(uint32 diff) override
    {
        if (_setPod != 1 && !UpdateVictim())
            return;

        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_ENSNARE:
                    DoCastVictim(SPELL_ENSNARE);
                    _events.ScheduleEvent(EVENT_ENSNARE, 3s, 6s);
                    break;
                case EVENT_AURA_CHECK:
                    if (!me->HasAura(SPELL_SEED_POD))
                    {
                        if (GameObject* pod = ObjectAccessor::GetGameObject(*me, _pod))
                        {
                            pod->SetRespawnTime(0);
                            pod->Delete();
                        }
                        _pod.Clear();
                        me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE_2);
                        me->SetVisible(true);
                        me->SetImmuneToAll(false);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoCast(me, SPELL_CRYSTAL_BLOOM);
                        _events.ScheduleEvent(EVENT_ENSNARE, 2s, 5s);
                        _setPod = 2;
                    }
                    else
                        _events.ScheduleEvent(EVENT_AURA_CHECK, 1s);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

private:
    void _SummPod()
    {
        QuaternionData rotation = QuaternionData::fromEulerAnglesZYX(me->GetOrientation(), 0.f, 0.f);
        if (GameObject* go = me->SummonGameObject(GO_POD, me->GetPosition(), rotation, 90s))
        {
            go->SetSpellId(SPELL_SUMM_SEED_POD);
            _pod = go->GetGUID();
        }
    }

    EventMap _events;
    ObjectGuid _pod;
    int _setPod;
};

struct EG_npc_eidolon_watcher : public SmartAI
{
    EG_npc_eidolon_watcher(Creature* creature) : SmartAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        SmartAI::IsSummonedBy(summoner);
        Unit* summonerUnit = summoner->ToUnit();
        if (!summonerUnit)
            return;

        _passengerInitialPosition = summonerUnit->GetPosition();
    }

    void GetUnitVehicleExitParameters(UnitVehicleExitParameters& parameters, Unit* passenger) override
    {
        parameters.ExitSpline = false;
        parameters.ResummonPet = false;

        passenger->m_Events.AddEventAtOffset(new Trinity::Helpers::Events::GenericEvent(passenger, [destination = _passengerInitialPosition](WorldObject* owner)
        {
            if (Player* ownerPlayer = owner->ToPlayer())
                ownerPlayer->NearTeleportTo(destination);
            return true;
        }), 1ms);
    }

private:
    Position _passengerInitialPosition;
};

enum UlduarTowerGauntletGeneratorMisc
{
    NPC_DEMOLISHER = 33109,
    NPC_CHOPPER = 33062,
    NPC_SIEGE_ENGINE = 33060,
    NPC_STEELFORGED_DEFENDER = 33572,
};

struct EG_npc_ulduar_tower_gauntlet_generator : public ScriptedAI
{
    EG_npc_ulduar_tower_gauntlet_generator(Creature* creature) : ScriptedAI(creature)
    {
        _summonTimer = 2000;
        _summon = false;
    }

    void Reset() override
    {
        GameObject* beacon = me->FindNearestGameObjectWithOptions(10.f, FindGameObjectOptions{ .GameObjectIds = { 194375, 194370, 194371, 194377,
            194398, 194399, 194400, 194401, 194402, 194403, 194404, 194405, 194406, 194407, 194408, 194409, 194410, 194411, 194412, 194413, 194414, 194415, 194506 } });
        if (beacon)
        {
            if (beacon->GetDestructibleState() == GO_DESTRUCTIBLE_DESTROYED)
            {
                me->DespawnOrUnsummon();
                _summon = false;
            }
            _summon = true;
            _beaconGuid = beacon->GetGUID();
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (_summon && _summonTimer <= diff)
        {
            GameObject* beacon = ObjectAccessor::GetGameObject(*me, _beaconGuid);
            if (!_beaconGuid.IsEmpty() && (!beacon || beacon->GetDestructibleState() == GO_DESTRUCTIBLE_DESTROYED))
            {
                me->DespawnOrUnsummon();
                _summon = false;
                return;
            }
            std::list<Creature*> targets;
            me->GetCreatureListWithOptionsInGrid(targets, 70.0f, FindCreatureOptions{ .CreatureIds = { NPC_DEMOLISHER, NPC_CHOPPER, NPC_SIEGE_ENGINE }, .IsAlive = true });
            if (!targets.empty())
                if (Creature* summon = me->SummonCreature(NPC_STEELFORGED_DEFENDER, me->GetPosition(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10s))
                {
                    summon->AI()->AttackStart(Trinity::Containers::SelectRandomContainerElement(targets));
                    if (!summon->IsEngaged())
                        summon->DespawnOrUnsummon();
                }

            _summonTimer = 2000;
        }
        else
            _summonTimer -= diff;
    }
private:
    uint32 _summonTimer;
    bool _summon;
    ObjectGuid _beaconGuid;
};

enum ArachnopodDestroyerMisc
{
    NPC_CLOCKWORK_MECHANIC = 34184,

    SPELL_FLAME_SPRAY = 64717,
    SPELL_CHARGED_LEAP = 64779,
    SPELL_MACHINE_GUN = 64776,
    SPELL_DAMAGED = 64770,
    SPELL_STUN_SELF = 25900,

    EVENT_FLAME_SPRAY = 1,
    EVENT_CHARGED_LEAP,
    EVENT_MACHINE_GUN,
};

class ArachnopodDestroyerChargeTargetSelector
{
    public:
        explicit ArachnopodDestroyerChargeTargetSelector(Creature* owner) : _owner(owner) { };

        bool operator()(WorldObject* object) const
        {
            if (Unit* unit = object->ToUnit())
            {
                if (unit->GetTypeId() != TYPEID_PLAYER)
                    return false;

                if (!_owner->CanCreatureAttack(unit))
                    return false;

                float distance = _owner->GetDistance(object);
                if (distance > 10.0f && distance < 40.0f)
                    return true;
            }

            return false;
        }
    private:
        Creature const* _owner;
};

struct EG_npc_arachnopod_destroyer : public ScriptedAI
{
    EG_npc_arachnopod_destroyer(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        ScriptedAI::Reset();
        _events.Reset();
        _damaged = false;
    }

    void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply) override
    {
        if (!apply)
        {
            if (Creature* mechanic = who->ToCreature())
                if (mechanic->GetEntry() == NPC_CLOCKWORK_MECHANIC)
                {
                    mechanic->RemoveAurasDueToSpell(SPELL_STUN_SELF);
                    mechanic->RemoveUnitFlag(UNIT_FLAG_UNINTERACTIBLE);
                }
        }
    }

    void DamageTaken(Unit* /*attacker*/, uint32& damage, DamageEffectType /*damageType*/, SpellInfo const* /*spellInfo = nullptr*/) override
    {
        if (!_damaged && me->HealthBelowPctDamaged(20, damage))
        {
            _damaged = true;
            _events.Reset();
            DoCastSelf(SPELL_DAMAGED, true);
            if (Vehicle* vechicle = me->GetVehicleKit())
                vechicle->RemoveAllPassengers();

            me->SetRegenerateHealth(false);
            me->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_ACTIVE);
            me->GetCombatManager().RevalidateCombat();
        }
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (me->GetFaction() == FACTION_ESCORTEE_N_NEUTRAL_ACTIVE)
            _EnterEvadeMode();
        else
            ScriptedAI::EnterEvadeMode(why);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _events.ScheduleEvent(EVENT_FLAME_SPRAY, 2s, 8s);
        _events.ScheduleEvent(EVENT_CHARGED_LEAP, 5s, 15s);
        _events.ScheduleEvent(EVENT_MACHINE_GUN, 1s, 6s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FLAME_SPRAY:
                    DoCastVictim(SPELL_FLAME_SPRAY);
                    _events.ScheduleEvent(EVENT_FLAME_SPRAY, 8s, 12s);
                    break;
                case EVENT_CHARGED_LEAP:
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, ArachnopodDestroyerChargeTargetSelector(me)))
                        DoCast(target, SPELL_CHARGED_LEAP);
                    _events.ScheduleEvent(EVENT_CHARGED_LEAP, 25s, 35s);
                    break;
                case EVENT_MACHINE_GUN:
                    DoCastVictim(SPELL_MACHINE_GUN);
                    _events.ScheduleEvent(EVENT_MACHINE_GUN, 6s, 10s);
                    break;
            }
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;
        }

        DoMeleeAttackIfReady();
    }

private:
    EventMap _events;
    bool _damaged;
};

enum StormTemperedKeeperMisc
{
    NPC_CHARGED_SPHERE = 33715,
    NPC_KEEPER_1 = 33699,
    NPC_KEEPER_2 = 33722,

    SPELL_FORKED_LIGHTNING = 63541,
    SPELL_SEPARATION_ANXIETY = 63539,
    SPELL_SUMMON_SPHERE = 63527,
    SPELL_VENGEFUL_SURGE = 63630,
    SPELL_CHARGED_SPHERE = 63537,
    SPELL_SUPERCHARGED = 63528,

    DATA_SPHERE_GUID = 1,

    EVENT_FORKED_LIGHTNING = 1,
    EVENT_SUMMON_SPHERE,
    EVENT_CHECK_KEEPER,
};

struct EG_npc_storm_tempered_keeper : public ScriptedAI
{
    EG_npc_storm_tempered_keeper(Creature* creature) : ScriptedAI(creature), _summons(creature) { }

    void Reset() override
    {
        _events.Reset();
        _summons.DespawnAll();
        _otherKeeper.Clear();
        _chargedSphere.Clear();
        _keeperDead = false;
        me->RemoveAurasDueToSpell(SPELL_VENGEFUL_SURGE);
        me->RemoveAurasDueToSpell(SPELL_SEPARATION_ANXIETY);
    }

    void JustSummoned(Creature* summon) override
    {
        _summons.Summon(summon);
        if (summon->GetEntry() == NPC_CHARGED_SPHERE)
        {
            summon->SetReactState(REACT_PASSIVE);
            summon->CastSpell(summon, SPELL_CHARGED_SPHERE);
            if (Creature* othKeeper = ObjectAccessor::GetCreature(*me, _otherKeeper))
            {
                summon->GetMotionMaster()->MoveChase(othKeeper, 0.f, false);
                othKeeper->AI()->SetGUID(summon->GetGUID(), DATA_SPHERE_GUID);
                DoAddEvent(2s, new Trinity::Helpers::Events::GenericEvent(summon, [targetGUID = _otherKeeper](WorldObject* owner)
                {
                    Creature* sphere = owner->ToCreature();
                    if (!sphere->IsAlive())
                        return true;
                    if (Creature* othKeeper = ObjectAccessor::GetCreature(*sphere, targetGUID))
                        if (othKeeper->IsAlive())
                            return false;

                    sphere->DespawnOrUnsummon();
                    return true;
                }), summon);
            }
            else
                summon->DespawnOrUnsummon();
        }
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        if (summon->GetEntry() == NPC_CHARGED_SPHERE)
        {
            _chargedSphere.Clear();
            if (Creature* othKeeper = ObjectAccessor::GetCreature(*me, _otherKeeper))
                othKeeper->AI()->SetGUID(ObjectGuid::Empty, DATA_SPHERE_GUID);
        }
    }

    void SetGUID(ObjectGuid const& guid, int32 type) override
    {
        if (type == DATA_SPHERE_GUID)
            _chargedSphere = guid;
    }

    void JustEngagedWith(Unit* who) override
    {
        if (Creature* othKeeper = me->FindNearestCreature(me->GetEntry() == NPC_KEEPER_1 ? NPC_KEEPER_2 : NPC_KEEPER_1, 100.0f, true))
        {
            _otherKeeper = othKeeper->GetGUID();
            if (!othKeeper->IsInCombat())
            {
                othKeeper->EngageWithTarget(who);
                othKeeper->AI()->AttackStart(who);
            }
        }
        else
        {
            _keeperDead = true;
            DoCast(me, SPELL_SEPARATION_ANXIETY);
            DoCast(me, SPELL_VENGEFUL_SURGE);
        }
        _events.ScheduleEvent(EVENT_FORKED_LIGHTNING, 5s, 10s);
        _events.ScheduleEvent(EVENT_SUMMON_SPHERE, 10s, 30s);
        _events.ScheduleEvent(EVENT_CHECK_KEEPER, 1s);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetEntry() == NPC_CHARGED_SPHERE && who->GetGUID() == _chargedSphere)
        {
            if (who->GetDistance(me) <= 2.5f)
            {
                who->CastSpell(me, SPELL_SUPERCHARGED, true);
                who->KillSelf();
                _chargedSphere.Clear();
            }
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        if (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FORKED_LIGHTNING:
                    DoCastVictim(SPELL_FORKED_LIGHTNING);
                    _events.ScheduleEvent(EVENT_FORKED_LIGHTNING, 10s, 15s);
                    break;
                case EVENT_SUMMON_SPHERE:
                    if (!_keeperDead)
                        if (Creature* othKeeper = ObjectAccessor::GetCreature(*me, _otherKeeper))
                            if (!othKeeper->HasAura(SPELL_SUPERCHARGED))
                                DoCast(SPELL_SUMMON_SPHERE);
                    _events.ScheduleEvent(EVENT_SUMMON_SPHERE, 20s, 35s);
                    break;
                case EVENT_CHECK_KEEPER:
                    if (Creature* othKeeper = ObjectAccessor::GetCreature(*me, _otherKeeper))
                    {
                        if (!me->IsWithinDist(othKeeper, 20.0f) && !me->HasAura(SPELL_SEPARATION_ANXIETY))
                            DoCastSelf(SPELL_SEPARATION_ANXIETY);

                        if (!othKeeper->IsAlive() && !_keeperDead)
                        {
                            _keeperDead = true;
                            DoCastSelf(SPELL_VENGEFUL_SURGE, true);
                        }
                    }
                    _events.ScheduleEvent(EVENT_CHECK_KEEPER, 1s);
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
    EventMap _events;
    SummonList _summons;
    ObjectGuid _otherKeeper;
    ObjectGuid _chargedSphere;
    bool _keeperDead;
};

void AddSC_EG_gen_npc_scripts()
{
    RegisterCreatureAI(EG_npc_damage_test_controller);
    RegisterCreatureAI(EG_npc_damage_test_dummy);
    RegisterCreatureAI(EG_npc_evolving_ectoplasm);
    RegisterCreatureAI(EG_npc_plague_slime);
    RegisterCreatureAI(EG_npc_crystalline_frayer);
    RegisterCreatureAI(EG_npc_eidolon_watcher);
    RegisterCreatureAI(EG_npc_ulduar_tower_gauntlet_generator);
    RegisterCreatureAI(EG_npc_arachnopod_destroyer);
    RegisterCreatureAI(EG_npc_storm_tempered_keeper);
}
