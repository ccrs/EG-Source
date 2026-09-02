#include "CustomFunctions.h"
#include "BattlegroundMgr.h"
#include "BattlegroundPackets.h"
#include "BattlegroundQueue.h"
#include "InstanceSaveMgr.h"
#include "InstanceScript.h"
#include "CellImpl.h"
#include "Channel.h"
#include "ChannelPackets.h"
#include "ChatPackets.h"
#include "Containers.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "GenericMovementGenerator.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Language.h"
#include "LFGMgr.h"
#include "LFGQueue.h"
#include "Map.h"
#include "MapManager.h"
#include "MotionMaster.h"
#include "MovementInfo.h"
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "Log.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Optional.h"
#include "Pet.h"
#include "Player.h"
#include "PlayerTaxi.h"
#include "ScriptedCreature.h"
#include "SharedDefines.h"
#include "SmartAI.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "StringConvert.h"
#include "TemporarySummon.h"
#include "Transmogrification.h"
#include "Transport.h"
#include "Unit.h"
#include "Util.h"
#include "Vehicle.h"
#include "VehicleDefines.h"
#include "World.h"
#include "WorldSession.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{
    bool BoostedDayActive = false;

    enum RealmFirstAchievements
    {
        ACHIEV_REALM_FIRST_LEVEL_80_DEATH_KNIGHT = 461
    };
}

bool EG::IsBoostedDay()
{
    return BoostedDayActive;
}

void EG::SetBoostedDay(bool active)
{
    BoostedDayActive = active;
}

bool EG::CanEarnRealmFirst(Player const* player, AchievementEntry const* achievement)
{
    if (achievement->Flags != ACHIEVEMENT_FLAG_REALM_FIRST_REACH)
        return true;

    if (player->HasCustomFlag(CustomFlagsIndex::CUSTOM_REALM_FIRST_BLOCK, CustomFlags::CUSTOM_FLAG_REALM_FIRST_BLOCKED))
        return false;

    if (player->GetCustomFlags(CustomFlagsIndex::CUSTOM_XPRATE_FLAGS) > CustomFlags::CUSTOM_FLAG_NONE)
        return false;

    if (player->GetClass() == CLASS_DEATH_KNIGHT && player->HasAccountUsedXPRate())
        return achievement->ID == ACHIEV_REALM_FIRST_LEVEL_80_DEATH_KNIGHT;

    return true;
}

void Creature::ProcessDelayedLOSEntries()
{
    if (_LOSQueue.empty() || !IsAlive() || HasUnitState(UNIT_STATE_SIGHTLESS) || !IsAIEnabled())
    {
        _LOSQueue = {};
        _uniqueLOSEntries.clear();
        _moveInLOSLockStatus = LOS_LOCK_NONE;
        return;
    }

    _moveInLOSLockStatus = LOS_LOCK_PROCESSING;
    for (auto itr = _LOSQueue.begin(); itr != _LOSQueue.end();)
    {
        if (Unit* current = ObjectAccessor::GetUnit(*this, *itr))
        {
            if (Creature* currentCreature = current->ToCreature())
                if (currentCreature->GetLOSLockStatus() == LOS_LOCK_SPAWN)
                {
                    ++itr;
                    continue;
                }

            if (current->IsAlive() && !current->IsInFlight() && CanSeeOrDetect(current, false, true))
                AI()->MoveInLineOfSight(current);
        }
        _uniqueLOSEntries.erase(*itr);
        itr = _LOSQueue.erase(itr);
    }

    _moveInLOSLockStatus = LOS_LOCK_NONE;
}

void Creature::InsertLOSEntry(ObjectGuid guid)
{
    if (!_uniqueLOSEntries.contains(guid))
    {
        _uniqueLOSEntries.insert(guid);
        _LOSQueue.push_back(guid);
    }
}

void Creature::LoadCreaturesAddonAuras()
{
    CreatureAddon const* creatureAddon = GetCreatureAddon();
    if (!creatureAddon || creatureAddon->auras.empty())
        return;

    for (std::vector<uint32>::const_iterator itr = creatureAddon->auras.begin(); itr != creatureAddon->auras.end(); ++itr)
    {
        SpellInfo const* AdditionalSpellInfo = sSpellMgr->GetSpellInfo(*itr);
        if (!AdditionalSpellInfo)
        {
            TC_LOG_ERROR("sql.sql", "Creature {} has wrong spell {} defined in `auras` field.", GetGUID().ToString(), *itr);
            continue;
        }

        // skip already applied aura
        if (HasAura(*itr))
            continue;

        AddAura(*itr, this);
        TC_LOG_DEBUG("entities.unit", "Spell: {} added to creature {}", *itr, GetGUID().ToString());
    }
}

Item* Player::GetWeaponForDamageMods(WeaponAttackType attackType) const
{
    uint8 slot;
    switch (attackType)
    {
        case BASE_ATTACK:   slot = EQUIPMENT_SLOT_MAINHAND; break;
        case OFF_ATTACK:    slot = EQUIPMENT_SLOT_OFFHAND;  break;
        case RANGED_ATTACK: slot = EQUIPMENT_SLOT_RANGED;   break;
        default: return nullptr;
    }

    Item* item = GetUseableItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!item || item->GetTemplate()->Class != ITEM_CLASS_WEAPON)
        return nullptr;

    if (item->IsBroken())
        return nullptr;

    return item;
}

void Player::_LoadCustomSettings(PreparedQueryResult result)
{
    if (!result)
        return;

    Field* fields = result->Fetch();
    std::string value = fields[0].GetString();
    if (!value.empty())
    {
        std::istringstream data(value);
        for (uint16 itr = 0; itr < CUSTOM_FLAGS_MAX; itr++)
        {
            uint16 temp = 0;
            data >> temp;
            _customFlags[itr] = temp;
        }
    }
}

void Player::_LoadAccountXPRate(PreparedQueryResult result)
{
    _accountUsedXPRate = result != nullptr;
}

void Player::_LoadTransmogrifications(PreparedQueryResult result)
{
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 lowGUID = fields[0].GetUInt32();
            uint32 fakeEntry = fields[1].GetUInt32();

            ObjectGuid itemGUID = ObjectGuid::Create<HighGuid::Item>(lowGUID);

            // Only load items that are in inventory / bank / etc
            if (sObjectMgr->GetItemTemplate(fakeEntry) && GetItemByGuid(itemGUID))
            {
                InsertTransmogrificationEntry(itemGUID, fakeEntry);
            }
            else
            {
                // Ignore, will be erased on next save.
                // Additionally this can happen if an item was deleted from DB but still exists for the player
                TC_LOG_DEBUG("transmogrification", "Item entry (Entry: {}, itemGUID: {}, playerGUID: {}) does not exist, ignoring.", fakeEntry, itemGUID.ToString(), GetGUID().ToString());
            }
        } while (result->NextRow());
    }
}

void Player::_LoadMasqueradeRace()
{
    uint16 storedValue = GetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE);
    if (storedValue > CustomFlags::CUSTOM_FLAG_RACE_MASQUERADE_HIDE)
    {
        uint8 index = 1;
        for (; index < 11; index++)
            if (storedValue & (1 << index))
                break;

        if (index > 10)
            return;

        if (index > 8)
            ++index;
        _masqueradeRace = Races(index);
        m_Events.AddEvent(new EG::SetRaceMasqueradeSetting(this, _masqueradeRace), m_Events.CalculateTime(1s));
    }
}

void Player::_LoadAccountSharedSpells(PreparedQueryResult result)
{
    if (result)
    {
        std::unordered_multimap<uint32/*team*/, uint32/*spellId*/> spellIdsByTeam;
        do
        {
            Field* fields = result->Fetch();
            uint32 spellId = fields[0].GetUInt32();
            uint8 race = fields[1].GetUInt8();
            spellIdsByTeam.emplace(Player::TeamForRace(race), spellId);
        }
        while (result->NextRow());

        uint32 playerTeam = GetTeam();
        bool searchForMounts = HasCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_MOUNT, CustomFlags::CUSTOM_FLAG_ACCOUNT_MOUNT_ACTIVE);
        bool searchForRiding = HasCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_RIDING, CustomFlags::CUSTOM_FLAG_ACCOUNT_RIDING_ACTIVE);
        bool searchForPets = HasCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_PET, CustomFlags::CUSTOM_FLAG_ACCOUNT_PET_ACTIVE);

        // Scripted mounts (spell_gen_mount): use script effects instead of SPELL_AURA_MOUNTED directly
        static constexpr uint32 scriptedMountIds[] =
        {
            58983, // spell_big_blizzard_bear
            71342, // spell_big_love_rocket
            74856, // spell_blazing_hippogryph
            75614, // spell_celestial_steed
            48025, // spell_headless_horseman_mount
            72286, // spell_invincible
            47977, // spell_magic_broom
            54729, // spell_winged_steed_of_the_ebon_blade
            75973, // spell_x53_touring_rocket
        };

        for (std::unordered_multimap<uint32/*team*/, uint32/*spellId*/>::value_type const& currentValue : spellIdsByTeam)
        {
            uint32 team = currentValue.first;
            uint32 spellId = currentValue.second;
            SpellInfo const* relatedInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!relatedInfo)
                continue;
            if (searchForMounts
                && team == playerTeam
                && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).Effect == SPELL_EFFECT_APPLY_AURA
                && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).ApplyAuraName == SPELL_AURA_MOUNTED
            )
            {
                LearnSpell(relatedInfo->Id, false);
                continue;
            }
            if (searchForMounts
                && std::find(std::begin(scriptedMountIds), std::end(scriptedMountIds), relatedInfo->Id) != std::end(scriptedMountIds)
            )
            {
                LearnSpell(relatedInfo->Id, false);
                continue;
            }
            if (searchForPets
                && relatedInfo->IsAbilityOfSkillType(SkillType::SKILL_COMPANIONS)
                && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).Effect == SPELL_EFFECT_SUMMON)
            {
                LearnSpell(relatedInfo->Id, false);
                continue;
            }
            if (searchForRiding)
            {
                switch (relatedInfo->Id)
                {
                    case 33388: // Apprentice Riding (Apprentice)
                    case 5784: // Felsteed (Summon)
                    case 13819: // Warhorse (Summon)
                    case 34769: // Summon Warhorse (Summon)
                        if (GetLevel() >= 20)
                            LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                        break;
                    case 33391: // Journeyman Riding (Journeyman)
                    case 23161: // Dreadsteed (Summon)
                    case 23214: // Charger (Summon)
                    case 34767: // Summon Charger (Summon)
                    case 48778: // Acherus Deathcharger (Summon)
                        if (GetLevel() >= 40)
                            LearnSpell(33391, false); // Journeyman Riding (Journeyman)
                        else if (GetLevel() >= 20)
                            LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                        break;
                    case 34090: // Expert Riding (Expert)
                    case 33943: // Flight Form (Shapeshift)
                        if (GetLevel() >= 60)
                            LearnSpell(34090, false); // Expert Riding (Expert)
                        else if (GetLevel() >= 40)
                            LearnSpell(33391, false); // Journeyman Riding (Journeyman)
                        else if (GetLevel() >= 20)
                            LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                        break;
                    case 34091: // Artisan Riding (Artisan)
                    case 40120: // Swift Flight Form (Shapeshift)
                        if (GetLevel() >= 70)
                            LearnSpell(34091, false);
                        else if (GetLevel() >= 60)
                            LearnSpell(34090, false); // Expert Riding (Expert)
                        else if (GetLevel() >= 40)
                            LearnSpell(33391, false); // Journeyman Riding (Journeyman)
                        else if (GetLevel() >= 20)
                            LearnSpell(33388, false); // Apprentice Riding (Apprentice)
                        break;
                    case 54197: // Cold Weather Flying (Passive)
                        if (GetLevel() >= 68)
                            LearnSpell(relatedInfo->Id, false);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

void Player::_LoadAccountCharacters(PreparedQueryResult result)
{
    if (result)
    {
        if (GetClass() != CLASS_DEATH_KNIGHT && HasCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_TAXI, CustomFlags::CUSTOM_FLAG_ACCOUNT_TAXI_ACTIVE))
        {
            do
            {
                Field* fields = result->Fetch();
                std::string taximask = fields[0].GetString();
                uint8 race = fields[1].GetUInt8();
                uint8 characterClass = fields[2].GetUInt8();
                if (Player::TeamForRace(race) != GetTeam())
                    continue;
                if (characterClass == CLASS_DEATH_KNIGHT)
                    continue;

                std::vector<std::string_view> tokens = Trinity::Tokenize(taximask, ' ', false);
                for (size_t index = 0; (index < m_taxi.size()) && (index < tokens.size()); ++index)
                {
                    if (Optional<uint32> mask = Trinity::StringTo<uint32>(tokens[index]))
                        m_taxi.AddSubmaskToTaximask(index, *mask);
                }
            }
            while (result->NextRow());
        }
    }
}

void Player::_SaveCustomSettings()
{
    std::ostringstream data;
    for (uint16 value : _customFlags)
        data << value << ' ';

    if (CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CUSTOM_SETTINGS))
    {
        stmt->setUInt32(0, GetGUID().GetCounter());
        stmt->setString(1, data.str());
        CharacterDatabase.Execute(stmt);
    }
}

void Player::_SaveTransmogrifications()
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    uint32 lowguid = GetGUID().GetCounter();
    
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_TRANSMOGRIFICATIONS);
    stmt->setUInt32(0, lowguid);
    trans->Append(stmt);

    if (HasTransmogrifications())
    {
        std::unordered_map<ObjectGuid, uint32> _map = GetTransmogrificationContainer();

        // Only save items that are in inventory / bank / etc
        std::vector<ObjectGuid> items = sTransmogrification->GetItemList(this);
        for (ObjectGuid const& itemGUID : items)
        {
            auto itr = _map.find(itemGUID);
            if (itr == _map.end())
                continue;

            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TRANSMOGRIFICATION);
            stmt->setUInt32(0, itr->first.GetCounter());
            stmt->setUInt32(1, itr->second);
            stmt->setUInt32(2, lowguid);
            trans->Append(stmt);
        }
    }

    CharacterDatabase.CommitTransaction(trans);
}

bool Player::HasCustomFlag(CustomFlagsIndex const i, CustomFlags const f) const
{
    uint16 index = i;
    uint16 value = f;

    if ((_customFlags[index] & value) != 0)
        return true;

    return false;
}

void Player::SetCustomFlags(CustomFlagsIndex const i, CustomFlags const f)
{
    uint16 index = i;
    uint16 value = f;
    _customFlags[index] = value;
}

void Player::AddCustomFlag(CustomFlagsIndex const i, CustomFlags const f)
{
    uint16 index = i;
    uint16 value = f;
    _customFlags[index] |= value;
}

void Player::RemoveCustomFlag(CustomFlagsIndex const i, CustomFlags const f)
{
    uint16 index = i;
    uint16 value = f;
    _customFlags[index] &= ~value;
}

uint16 Player::GetCustomFlags(CustomFlagsIndex const i) const
{
    uint16 index = i;
    return _customFlags[index];
}

void Player::ActivateHardcore()
{
    bool hadVisuals = GetCustomFlags(CustomFlagsIndex::CUSTOM_VISUALS) != 0;

    for (uint16 i = 0; i < static_cast<uint16>(CustomFlagsIndex::CUSTOM_FLAGS_MAX); ++i)
        if (i != CustomFlagsIndex::CUSTOM_HARDCORE)
            SetCustomFlags(CustomFlagsIndex(i), CustomFlags::CUSTOM_FLAG_NONE);

    SetCustomFlags(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags(CUSTOM_FLAG_TRANSMOG_HIDE | CUSTOM_FLAG_TRANSMOG_HIDE_LEGENDARY));
    SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags::CUSTOM_FLAG_RACE_MASQUERADE_HIDE);
    AddCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE);

    StoredLootView.clear();
    StoredLoot.clear();
    if (hadVisuals)
        if (Pet* pet = GetPet())
            pet->Remove(PET_SAVE_NOT_IN_SLOT, true);
    if (IsMasqueradingRace())
        SetMasqueradeRace(RACE_NONE);

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_EXISTING_CHARACTER_SPELLS);
    stmt->setUInt32(0, GetSession()->GetAccountId());
    stmt->setUInt32(1, GetGUID().GetCounter());

    static constexpr uint32 scriptedMountIds[] =
    {
        58983, // spell_big_blizzard_bear
        71342, // spell_big_love_rocket
        74856, // spell_blazing_hippogryph
        75614, // spell_celestial_steed
        48025, // spell_headless_horseman_mount
        72286, // spell_invincible
        47977, // spell_magic_broom
        54729, // spell_winged_steed_of_the_ebon_blade
        75973, // spell_x53_touring_rocket
    };

    static constexpr uint32 ridingSpellIds[] =
    {
        33388, // Apprentice Riding (Apprentice)
        33391, // Journeyman Riding (Journeyman)
        34090, // Expert Riding (Expert)
        34091, // Artisan Riding (Artisan)
        54197, // Cold Weather Flying (Passive)
    };

    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 spellId = fields[0].GetUInt32();
            if (!HasSpell(spellId))
                continue;
            SpellInfo const* relatedInfo = sSpellMgr->GetSpellInfo(spellId);
            if (!relatedInfo)
                continue;

            bool isAccountShared = (relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).Effect == SPELL_EFFECT_APPLY_AURA && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).ApplyAuraName == SPELL_AURA_MOUNTED)
                || std::find(std::begin(scriptedMountIds), std::end(scriptedMountIds), spellId) != std::end(scriptedMountIds)
                || (relatedInfo->IsAbilityOfSkillType(SkillType::SKILL_COMPANIONS) && relatedInfo->GetEffect(SpellEffIndex::EFFECT_0).Effect == SPELL_EFFECT_SUMMON)
                || std::find(std::begin(ridingSpellIds), std::end(ridingSpellIds), spellId) != std::end(ridingSpellIds);

            if (isAccountShared)
                RemoveSpell(spellId, false, false);
        }
        while (result->NextRow());
    }

    m_taxi.ResetTaximask();
    InitTaxiNodesForLevel();

    _SaveCustomSettings();
}

void Player::DisableHardcore()
{
    if (!GetHardcoreGraceSecondsLeft())
        return;

    SetCustomFlags(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_COMPLETED);
    SetCustomFlags(CustomFlagsIndex::CUSTOM_TRANSMOG_FLAGS, CustomFlags::CUSTOM_FLAG_NONE);
    SetCustomFlags(CustomFlagsIndex::CUSTOM_RACE_MASQUERADE, CustomFlags::CUSTOM_FLAG_NONE);

    _SaveCustomSettings();

    RefreshForcedPvPState(false);
}

uint32 Player::GetHardcoreGraceSecondsLeft() const
{
    if (!HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE))
        return 0;

    if (HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD))
        return 0;

    if (GetLevel() < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        return 0;

    uint32 grace = sWorld->getIntConfig(CONFIG_HARDCORE_GRACE_PERIOD) * HOUR;
    if (!grace)
        return 0;

    uint32 played = GetLevelPlayedTime();
    if (played >= grace)
        return 0;

    return grace - played;
}

void Player::HandleHardcoreDeath(Unit* killer)
{
    if (!HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE) || HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD))
        return;

    AddCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD);
    _SaveCustomSettings();

    Player* killerPlayer = killer ? killer->GetCharmerOrOwnerPlayerOrPlayerItself() : nullptr;
    if (killerPlayer && killerPlayer != this)
        sWorld->SendWorldText(LANG_HARDCORE_DEATH_PLAYER, GetName().c_str(), uint32(GetLevel()), killerPlayer->GetName().c_str());
    else if (killer && killer != this && killer->GetTypeId() == TYPEID_UNIT)
        sWorld->SendWorldText(LANG_HARDCORE_DEATH_CREATURE, GetName().c_str(), uint32(GetLevel()), killer->GetName().c_str());
    else
        sWorld->SendWorldText(LANG_HARDCORE_DEATH_GENERIC, GetName().c_str(), uint32(GetLevel()));
}

/*static*/ bool Player::IsHardcoreCharacter(ObjectGuid guid)
{
    if (Player* player = ObjectAccessor::FindConnectedPlayer(guid))
        return player->HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE);

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CUSTOM_SETTINGS);
    stmt->setUInt32(0, guid.GetCounter());
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (!result)
        return false;

    std::vector<std::string_view> tokens = Trinity::Tokenize(result->Fetch()[0].GetStringView(), ' ', false);
    if (tokens.size() <= CustomFlagsIndex::CUSTOM_HARDCORE)
        return false;

    Optional<uint16> hardcoreFlags = Trinity::StringTo<uint16>(tokens[CustomFlagsIndex::CUSTOM_HARDCORE]);
    return hardcoreFlags && (*hardcoreFlags & CUSTOM_FLAG_HARDCORE_ACTIVE);
}

void Player::ClearHardcoreDeath()
{
    RemoveCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_DEAD);
    _SaveCustomSettings();
}

/*static*/ void Player::OfflineClearHardcoreDeath(ObjectGuid::LowType guid)
{
    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CUSTOM_SETTINGS);
    stmt->setUInt32(0, guid);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
    if (!result)
        return;

    std::vector<std::string_view> tokens = Trinity::Tokenize(result->Fetch()[0].GetStringView(), ' ', false);
    if (tokens.size() <= CustomFlagsIndex::CUSTOM_HARDCORE)
        return;

    Optional<uint16> hardcoreFlags = Trinity::StringTo<uint16>(tokens[CustomFlagsIndex::CUSTOM_HARDCORE]);
    if (!hardcoreFlags || !(*hardcoreFlags & CUSTOM_FLAG_HARDCORE_DEAD))
        return;

    std::ostringstream data;
    for (size_t i = 0; i < tokens.size(); ++i)
    {
        if (i == CustomFlagsIndex::CUSTOM_HARDCORE)
            data << uint16(*hardcoreFlags & ~CUSTOM_FLAG_HARDCORE_DEAD) << ' ';
        else
            data << tokens[i] << ' ';
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CUSTOM_SETTINGS);
    stmt->setUInt32(0, guid);
    stmt->setString(1, data.str());
    CharacterDatabase.Execute(stmt);
}

uint32 Player::GetTransmogrificationEntry(ObjectGuid itemGUID) const
{
    auto it = _transmogrificationMap.find(itemGUID);
    if (it == _transmogrificationMap.end())
        return 0;

    return it->second;
}

bool Player::EraseTransmogrificationEntry(ObjectGuid itemGUID)
{
    return _transmogrificationMap.erase(itemGUID) != 0;
}

void Player::InsertTransmogrificationEntry(ObjectGuid itemGUID, uint32 entry)
{
    _transmogrificationMap[itemGUID] = entry;
}

uint32 Player::GetHiddenTransmogrificationEntry(uint8 itemIndex) const
{
    auto it = _transmogrificationHideMap.find(itemIndex);
    if (it == _transmogrificationHideMap.end())
        return 0;

    return it->second;
}

Loot* Player::GetLootFromAOELoot(ObjectGuid lootGUID) const
{
    if (!HasCustomFlag(CUSTOM_AOELOOT_FLAGS, CUSTOM_FLAG_AOELOOT_ACTIVE))
        return nullptr;

    if (lootGUID.IsEmpty())
        return nullptr;

    if (StoredLoot.empty() || StoredLoot.size() < 2)
        return nullptr;

    for (LootReference const& currentLoot : StoredLoot)
    {
        if (currentLoot.ContainerEntityGUID == lootGUID)
            return currentLoot.RelatedLoot;
    }
    return nullptr;
}

Optional<uint8> Player::GetLootViewSlot(Loot const* loot, uint8 itemIndex) const
{
    for (auto const& [viewSlot, reference] : StoredLootView)
        if (reference.RelatedLoot == loot && reference.ItemIndex == itemIndex)
            return viewSlot;

    return {};
}

uint32 Player::GetOriginalDisplayId() const
{
    PlayerInfo const* info = sObjectMgr->GetPlayerInfo(GetRace(), GetClass());
    if (!info)
    {
        TC_LOG_ERROR("entities.player", "Player::InitDisplayIds: Player '{}' ({}) has incorrect race/class pair. Can't init display ids.", GetName(), GetGUID().ToString());
        return 0;
    }

    uint8 gender = GetNativeGender();
    switch (gender)
    {
        case GENDER_FEMALE:
            return info->displayId_f;
        case GENDER_MALE:
        default:
            return info->displayId_m;
    }
}

void Player::SetMasqueradeRace(Races race)
{
    _masqueradeRace = race;
    InitDisplayIds();
    RestoreDisplayId();

    _changesMask.SetBit(UNIT_FIELD_BYTES_0);
    _changesMask.SetBit(UNIT_FIELD_DISPLAYID);
    _changesMask.SetBit(UNIT_FIELD_NATIVEDISPLAYID);
    AddToObjectUpdateIfNeeded();
}

Races Player::GetMasqueradeRace() const
{
    if (!_masqueradeRace)
        return Races(GetRace());
    return _masqueradeRace;
}

bool Player::CleanMasqueradeRaceValue()
{
    if (!IsInWorld() || !HaveAtClient(this))
        return false;

    ForceValuesUpdateAtIndex(UNIT_FIELD_BYTES_0);
    return true;
}

bool Player::InArenaQueue() const
{
    for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
        if (GetBattlegroundQueueTypeId(i).BattlemasterListId == BATTLEGROUND_AA)
            return true;
    return false;
}

void Player::UpdateHostileAreaState(AreaTableEntry const* zone)
{
    // a boosted day forces the realm to behave like a PvP realm for flagging purposes
    bool const boostedForcing = EG::IsBoostedDay() && !HasCustomFlag(CustomFlagsIndex::CUSTOM_HARDCORE, CustomFlags::CUSTOM_FLAG_HARDCORE_ACTIVE);
    bool const pvpRules = sWorld->IsPvPRealm() || boostedForcing;

    // in PvP, any not controlled zone (except zone->FactionGroupMask == 6, default case)
    // in PvE, only opposition team capital
    switch (zone->FactionGroupMask)
    {
        case AREATEAM_ALLY:
            pvpInfo.IsInHostileArea = GetTeam() != ALLIANCE && (pvpRules || zone->Flags & AREA_FLAG_CAPITAL);
            break;
        case AREATEAM_HORDE:
            pvpInfo.IsInHostileArea = GetTeam() != HORDE && (pvpRules || zone->Flags & AREA_FLAG_CAPITAL);
            break;
        case AREATEAM_NONE:
            // overwrite for battlegrounds, maybe batter some zone flags but current known not 100% fit to this
            pvpInfo.IsInHostileArea = pvpRules || InBattleground() || zone->Flags & AREA_FLAG_WINTERGRASP;
            break;
        default:                                            // 6 in fact
            pvpInfo.IsInHostileArea = false;
            break;
    }

    // Treat players having a quest flagging for PvP as always in hostile area
    pvpInfo.IsHostile = pvpInfo.IsInHostileArea || HasPvPForcingQuest();
}

void Player::RefreshForcedPvPState(bool hardClear)
{
    AreaTableEntry const* zone = sAreaTableStore.LookupEntry(GetZoneId());
    if (!zone)
        return;

    UpdateHostileAreaState(zone);

    if (!hardClear || pvpInfo.IsHostile)
        UpdatePvPState();
    else
        UpdatePvP(false, true);
}

void Player::FlushNonArenaBattlegroundQueues()
{
    for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattlegroundQueueTypeId qid = GetBattlegroundQueueTypeId(i);
        if (qid == BATTLEGROUND_QUEUE_NONE || qid.BattlemasterListId == BATTLEGROUND_AA)
            continue;

        BattlegroundQueue& bgQueue = sBattlegroundMgr->GetBattlegroundQueue(qid);
        bgQueue.RemovePlayer(GetGUID(), true);
        RemoveBattlegroundQueueId(qid);

        WorldPackets::Battleground::BattlefieldStatusNone status;
        BattlegroundMgr::BuildBattlegroundStatusNone(&status, i);
        SendDirectMessage(status.Write());
    }
}

void Unit::ExitVehicleHandling(Vehicle* vehicle, Position const& pos, UnitVehicleExitParameters params)
{
    if (params.ExitSpline && IsInWorld() && !m_Events.HasEventType(EventType::EVENT_TYPE_VEHICLE_JOIN))
    {
        Position safePos = pos;
        bool const posValid = !(std::fabs(pos.GetPositionX()) < 0.01f && std::fabs(pos.GetPositionY()) < 0.01f) && pos.GetPositionZ() > INVALID_HEIGHT;
        if (!posValid)
        {
            TC_LOG_WARN("entities.vehicle", "Unit {} vehicle exit pos invalid ({:.2f}, {:.2f}, {:.2f}); falling back to passenger pos ({:.2f}, {:.2f}, {:.2f})",
                GetGUID().ToString(), pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), GetPositionX(), GetPositionY(), GetPositionZ());
            safePos = GetPosition();
        }

        float const vehicleCollisionHeight = vehicle->GetBase()->GetCollisionHeight();

        if (!IsAlive() && GetTypeId() == TYPEID_PLAYER)
        {
            float const startHeight = safePos.GetPositionZ() + vehicleCollisionHeight;
            float groundHeight = startHeight;
            float const waterOrGroundLevel = GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), safePos.GetPositionX(), safePos.GetPositionY(), startHeight, &groundHeight, false, DEFAULT_COLLISION_HEIGHT, 1000.0f);

            if (waterOrGroundLevel > INVALID_HEIGHT)
            {
                Position groundPos(safePos.GetPositionX(), safePos.GetPositionY(), groundHeight, safePos.GetOrientation());
                DisableSpline();
                GetMotionMaster()->InterruptOnTeleport();
                SendTeleportPacket(groundPos);
                UpdatePosition(groundPos, true);
                UpdateObjectVisibility();
            }
        }
        else
        {
            bool const serverDrivenFallEligible = !IsAlive() || (GetTypeId() == TYPEID_UNIT && !CanFly());

            std::function<void(Movement::MoveSplineInit&)> initializer = [safePos, vehicleCollisionHeight, serverDrivenFallEligible, this](Movement::MoveSplineInit& init)
            {
                float const startHeight = safePos.GetPositionZ() + vehicleCollisionHeight;
                float groundHeight = startHeight;
                float const waterOrGroundLevel = GetMap()->GetWaterOrGroundLevel(GetPhaseMask(), safePos.GetPositionX(), safePos.GetPositionY(), startHeight, &groundHeight, false, DEFAULT_COLLISION_HEIGHT, 1000.0f);

                float const dropTolerance = 10.0f;
                bool const groundFound = waterOrGroundLevel > INVALID_HEIGHT;
                bool const safePlacement = groundFound && (safePos.GetPositionZ() - waterOrGroundLevel) <= dropTolerance;

                if (safePlacement)
                    init.MoveTo(safePos.GetPositionX(), safePos.GetPositionY(), groundHeight + 0.25f, false);
                else if (groundFound && serverDrivenFallEligible)
                {
                    init.SetFall();
                    init.MoveTo(safePos.GetPositionX(), safePos.GetPositionY(), groundHeight, false);
                }
                else
                    init.MoveTo(safePos.GetPositionX(), safePos.GetPositionY(), safePos.GetPositionZ(), false);

                init.SetFacing(safePos.GetOrientation());
                init.SetTransportExit();
            };

            GenericMovementGenerator* movement = new GenericMovementGenerator(std::move(initializer), EFFECT_MOTION_TYPE, EVENT_VEHICLE_EXIT);
            movement->Priority = MOTION_PRIORITY_HIGHEST;
            movement->Mode = MOTION_MODE_OVERRIDE;
            if (serverDrivenFallEligible)
                movement->AddFlag(MOVEMENTGENERATOR_FLAG_PERSIST_ON_DEATH);
            GetMotionMaster()->Add(movement);
        }
    }

    if (Player* player = ToPlayer())
    {
        player->SetCanTeleport(true);
        if (params.ResummonPet && !m_Events.HasEventType(EventType::EVENT_TYPE_VEHICLE_JOIN))
            player->ResummonPetTemporaryUnSummonedIfAny();
    }

    bool despawn = false;
    if (params.Despawn)
    {
        if (vehicle->GetBase()->HasUnitTypeMask(UNIT_MASK_MINION) && vehicle->GetBase()->GetTypeId() == TYPEID_UNIT)
            if (((Minion*)vehicle->GetBase())->GetOwner() == this)
                vehicle->GetBase()->ToCreature()->DespawnOrUnsummon(vehicle->GetDespawnDelay());

        if (HasUnitTypeMask(UNIT_MASK_ACCESSORY))
        {
            // Vehicle just died, we die too
            if (vehicle->GetBase()->getDeathState() == JUST_DIED)
                setDeathState(JUST_DIED);
            // If for other reason we as minion are exiting the vehicle (ejected, master dismounted) - unsummon
            else
                ToTempSummon()->UnSummon(2000); // Approximation
            despawn = true;
        }
    }

    if (IsAlive() && !despawn && params.Evade && GetTypeId() == TYPEID_UNIT)
    {
        Creature* toCreature = ToCreature();
        toCreature->SetSpawnHealth();
        toCreature->LoadCreaturesAddon();
        if (toCreature->IsVehicle())
            toCreature->GetVehicleKit()->Reset(true);
    }
}

void Unit::InterruptSpellsCastedOnMe(bool killDelayed, bool interruptFriendlySpells)
{
    UnitList targets;
    Trinity::AnyUnitInObjectRangeCheck u_check(this, 100.0f);
    Trinity::UnitListSearcher<Trinity::AnyUnitInObjectRangeCheck> searcher(this, targets, u_check);
    Cell::VisitAllObjects(this, searcher, GetMap()->GetVisibilityRange());

    for (const auto& target : targets)
    {
        if (target->GetVictim() == this)
        {
            if (Player* player = target->ToPlayer())
            {
                player->ClearUnitState(UNIT_STATE_MELEE_ATTACKING);
                player->InterruptSpell(CURRENT_MELEE_SPELL);
                player->SendMeleeAttackStop(this);
                player->SendAttackSwingCancelAttack();
            }
            else
            {
                target->InterruptSpell(CURRENT_MELEE_SPELL);
                target->SendMeleeAttackStop(this);
            }
        }

        if (!interruptFriendlySpells && IsFriendlyTo(target))
            continue;

        for (uint32 itr = CURRENT_FIRST_NON_MELEE_SPELL; itr < CURRENT_MAX_SPELL; ++itr)
            if (Spell* spell = target->GetCurrentSpell(CurrentSpellTypes(itr)))
                if (spell->m_targets.GetUnitTargetGUID() == GetGUID())
                    if (killDelayed || (spell->getState() == SPELL_STATE_PREPARING && spell->GetTimer()) || itr == CURRENT_CHANNELED_SPELL)
                        target->InterruptSpell(CurrentSpellTypes(itr), true);

        if (!killDelayed)
            continue;

        std::multimap<uint64, BasicEvent*> toIterate = target->m_Events.GetEvents();
        for (auto itr = toIterate.begin(); itr != toIterate.end(); ++itr)
            if (itr->second->Type == EventType::EVENT_TYPE_SPELL)
                if (SpellEvent* event = dynamic_cast<SpellEvent*>(itr->second))
                    if (event->GetSpell()->m_targets.GetUnitTargetGUID() == GetGUID())
                        if (event->GetSpell()->getState() != SPELL_STATE_FINISHED)
                            event->GetSpell()->cancel();
    }
}

bool Unit::HasShieldedCast() const
{
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_AUTOREPEAT_SPELL; ++i)
        if (Spell const* spell = m_currentSpells[i])
            if (spell->m_spellInfo->HasAttribute(SPELL_ATTR0_CU_UNINTERRUPTIBLE))
                return true;

    return false;
}

/*static*/ float Vehicle::GetSeatOrientationOffset(VehicleSeatEntry const* seatInfo, VehicleSeatAddon const* seatAddon)
{
    if (seatAddon)
        return seatAddon->SeatOrientationOffset;

    if (seatInfo && std::isfinite(seatInfo->PassengerYaw))
        return seatInfo->PassengerYaw;

    return 0.0f;
}

float Vehicle::GetSeatOrientationOffsetForPassenger(Unit const* passenger) const
{
    return GetSeatOrientationOffset(GetSeatForPassenger(passenger), GetSeatAddonForSeatOfPassenger(passenger));
}

bool Vehicle::NormalizePassengerMovementInfo(Unit const* passenger, MovementInfo& movementInfo) const
{
    SeatMap::const_iterator seat = Seats.end();

    for (SeatMap::const_iterator itr = Seats.begin(); itr != Seats.end(); ++itr)
    {
        if (itr->second.Passenger.Guid == passenger->GetGUID())
        {
            seat = itr;
            break;
        }
    }

    if (seat == Seats.end())
        return false;

    VehicleSeatEntry const* seatInfo = seat->second.SeatInfo;
    VehicleSeatAddon const* seatAddon = seat->second.SeatAddon;

    if (!seatAddon && (seatInfo->Flags & VEHICLE_SEAT_FLAG_ALLOW_TURNING))
    {
        static std::unordered_set<uint32> warnedIds;
        if (warnedIds.insert(seatInfo->ID).second)
            TC_LOG_WARN("vehicles", "VehicleSeat ID {} has VEHICLE_SEAT_FLAG_ALLOW_TURNING but no VehicleSeatAddon DB row; orientation offset defaults to 0.0f", seatInfo->ID);
    }

    float localX = seatInfo->AttachmentOffset.X;
    float localY = seatInfo->AttachmentOffset.Y;
    float localZ = seatInfo->AttachmentOffset.Z;
    // EG - sniff-derived seat position override (see VehicleSeatAddon::AttachmentOffset)
    if (seatAddon && seatAddon->AttachmentOffset)
    {
        localX = seatAddon->AttachmentOffset->GetPositionX();
        localY = seatAddon->AttachmentOffset->GetPositionY();
        localZ = seatAddon->AttachmentOffset->GetPositionZ();
    }
    float localO = GetSeatOrientationOffset(seatInfo, seatAddon);

    // For turning seats, preserve only the passenger's local orientation.
    // Never preserve client-sent local x/y/z.
    if (seatInfo->Flags & VEHICLE_SEAT_FLAG_ALLOW_TURNING)
    {
        if (movementInfo.HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
            localO = movementInfo.transport.pos.GetOrientation();
        else
            localO = passenger->m_movementInfo.transport.pos.GetOrientation();
    }

    localO = Position::NormalizeOrientation(localO);

    float worldX = localX;
    float worldY = localY;
    float worldZ = localZ;
    float worldO = localO;

    CalculatePassengerPosition(worldX, worldY, worldZ, &worldO);

    if (!Trinity::IsValidMapCoord(worldX, worldY, worldZ, worldO))
        return false;

    movementInfo.AddMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
    movementInfo.transport.guid = GetBase()->GetGUID();
    movementInfo.transport.seat = seat->first;
    movementInfo.transport.time = 0;
    movementInfo.transport.pos.Relocate(localX, localY, localZ, localO);
    movementInfo.pos.Relocate(worldX, worldY, worldZ, worldO);

    return true;
}

Unit* WorldObject::DoFindLowestHPFriendlyInRange(FriendlySearchOptions options) const
{
    std::vector<Unit*> potentialFriendlies;
    EG::AnyFriendlyUnitInObjectRangeCheck checker(this, options);
    Trinity::UnitListSearcher<EG::AnyFriendlyUnitInObjectRangeCheck> searcher(this, potentialFriendlies, checker);
    Cell::VisitAllObjects(this, searcher, options.Range);
    if (potentialFriendlies.empty())
        return nullptr;

    Unit* unit = nullptr;
    float hp = 100.f;
    Trinity::Containers::RandomShuffle(potentialFriendlies);
    for (Unit* potential : potentialFriendlies)
    {
        if (potential->GetHealthPct() < hp)
        {
            unit = potential;
            hp = unit->GetHealthPct();
        }
    }
    if (!unit)
        unit = *potentialFriendlies.begin();

    return unit;
}

void WorldObject::GetNearPoint2D(WorldObject const* searcher, Position const* reference, float& x, float& y, float distance, float absAngle) const
{
    float effectiveReach = GetCombatReach();

    if (searcher)
    {
        effectiveReach += searcher->GetCombatReach();

        if (this != searcher)
        {
            float myHover = 0.0f, searcherHover = 0.0f;
            if (Unit const* unit = ToUnit())
                myHover = unit->GetHoverOffset();
            if (Unit const* searchUnit = searcher->ToUnit())
                searcherHover = searchUnit->GetHoverOffset();

            float hoverDelta = myHover - searcherHover;
            if (hoverDelta != 0.0f)
                effectiveReach = std::sqrt(std::max(effectiveReach * effectiveReach - hoverDelta * hoverDelta, 0.0f));
        }
    }

    x = reference->GetPositionX() + (effectiveReach + distance) * std::cos(absAngle);
    y = reference->GetPositionY() + (effectiveReach + distance) * std::sin(absAngle);

    Trinity::NormalizeMapCoord(x);
    Trinity::NormalizeMapCoord(y);
}

void WorldObject::GetNearPoint(WorldObject const* searcher, Position const* reference, float& x, float& y, float& z, float distance2d, float absAngle) const
{
    GetNearPoint2D(searcher, reference, x, y, distance2d, absAngle);
    z = reference->GetPositionZ();
    (searcher ? searcher : this)->UpdateAllowedPositionZ(x, y, z);

    // if detection disabled, return first point
    if (!sWorld->getBoolConfig(CONFIG_DETECT_POS_COLLISION))
        return;

    // return if the point is already in LoS
    Position destination(x, y, z);
    if (IsWithinLOSInMap(&destination, searcher))
        return;

    // remember first point
    float first_x = x;
    float first_y = y;
    float first_z = z;

    // loop in a circle to look for a point in LoS using small steps
    for (float angle = float(M_PI) / 8; angle < float(M_PI) * 2; angle += float(M_PI) / 8)
    {
        GetNearPoint2D(searcher, reference, x, y, distance2d, absAngle + angle);
        z = reference->GetPositionZ();
        (searcher ? searcher : this)->UpdateAllowedPositionZ(x, y, z);
        destination.Relocate(x, y, z);
        if (IsWithinLOSInMap(&destination, searcher))
            return;
    }

    // still not in LoS, give up and return first position found
    x = first_x;
    y = first_y;
    z = first_z;
}

bool WorldObject::IsWithinLOSInMap(Position const* reference, WorldObject const* obj, LineOfSightChecks checks, VMAP::ModelIgnoreFlags ignoreFlags) const
{
    if (!IsInMap(obj))
        return false;

    float ox, oy, oz;
    if (obj->GetTypeId() == TYPEID_PLAYER)
    {
        obj->GetPosition(ox, oy, oz);
        oz += GetCollisionHeight();
    }
    else
        obj->GetHitSpherePointFor({ reference->GetPositionX(), reference->GetPositionY(), reference->GetPositionZ() + GetCollisionHeight() }, ox, oy, oz);

    float x, y, z;
    if (GetTypeId() == TYPEID_PLAYER)
    {
        reference->GetPosition(x, y, z);
        z += GetCollisionHeight();
    }
    else
        GetHitSpherePointFor({ reference->GetPositionX(), reference->GetPositionY(), reference->GetPositionZ() + obj->GetCollisionHeight() }, x, y, z);

    return GetMap()->isInLineOfSight(x, y, z, ox, oy, oz, GetPhaseMask(), checks, ignoreFlags);
}

bool EG::MostHPMissingFriendlyUnitInRangeSearcher::operator()(Unit* unit)
{
    if (_includeSelf && _source == unit && unit->GetHealthPct() < _hp)
    {
        _hp = unit->GetHealthPct();
        return true;
    }

    if (((_playerOnly && unit->IsPlayer()) || !_playerOnly)
        && unit->IsAlive()
        && !unit->IsCritter()
        && ((_source->IsInCombat() && unit->IsInCombat()) || (!_source->IsInCombat()))
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _range)
        && _source->IsWithinLOSInMap(unit)
        && unit->GetHealthPct() < _hp
    )
    {
        _hp = unit->GetHealthPct();
        return true;
    }

    return false;
}

bool EG::AnyFriendlyUnitInObjectRangeCheck::operator()(Unit const* unit) const
{
    if (_options.Alive && !unit->IsAlive())
        return false;

    if (_options.IncludeSelf && _source == unit)
        return true;

    if (unit->IsCritter())
        return false;

    if (!_options.ExcludedEntries.empty() && _options.ExcludedEntries.contains(unit->GetEntry()))
        return false;

    if (((_options.PlayerOnly && unit->IsPlayer()) || !_options.PlayerOnly)
        && (!_source->IsUnit() || (_source->ToUnit()->IsInCombat() && unit->IsInCombat()) || (!_source->ToUnit()->IsInCombat()))
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _options.Range)
        && _source->IsWithinLOSInMap(unit)
    )
        return true;

    return false;
}

void SmartAI::SetCombatMovement()
{
    if (!me->IsAlive() || !CanCombatMove() || !me->GetVictim())
        return;

    Optional<ChaseRange> chaseRange = _combatChaseParameters.CombatDistance ? ChaseRange(_combatChaseParameters.CombatDistance) : Optional<ChaseRange>{ };
    Optional<ChaseAngle> chaseAngle = _combatChaseParameters.CombatAngle ? ChaseAngle(_combatChaseParameters.CombatAngle) : Optional<ChaseAngle>{ };
    me->GetMotionMaster()->MoveChase(me->GetVictim(), chaseRange, chaseAngle);
}

bool WorldSession::NormalizeTransportMovementInfo(Unit* mover, MovementInfo& movementInfo)
{
    Player* plrMover = mover->ToPlayer();

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT))
    {
        Transport* transport = nullptr;
        Transport* currentTransport = plrMover ? plrMover->GetTransport() : nullptr;

        if (plrMover)
        {
            transport = plrMover->GetMap()->GetTransport(movementInfo.transport.guid);

            if (!transport && currentTransport && currentTransport->GetGUID() == movementInfo.transport.guid)
                transport = currentTransport;
        }
        else if (Transport* moverTransport = mover->GetTransport())
        {
            if (moverTransport->GetGUID() == movementInfo.transport.guid)
            {
                movementInfo.transport.guid = moverTransport->GetGUID();
                movementInfo.transport.pos  = mover->m_movementInfo.transport.pos;

                float wx = movementInfo.transport.pos.GetPositionX();
                float wy = movementInfo.transport.pos.GetPositionY();
                float wz = movementInfo.transport.pos.GetPositionZ();
                float wo = movementInfo.transport.pos.GetOrientation();
                moverTransport->CalculatePassengerPosition(wx, wy, wz, &wo);
                movementInfo.pos.Relocate(wx, wy, wz, wo);
                return true;
            }
        }

        float localX = movementInfo.transport.pos.GetPositionX();
        float localY = movementInfo.transport.pos.GetPositionY();
        float localZ = movementInfo.transport.pos.GetPositionZ();

        // Reject bogus transport offsets before changing transport membership.
        if (std::fabs(localX) > 75.0f || std::fabs(localY) > 75.0f || std::fabs(localZ) > 75.0f)
            return false;

        if (transport)
        {
            float worldX = movementInfo.transport.pos.GetPositionX();
            float worldY = movementInfo.transport.pos.GetPositionY();
            float worldZ = movementInfo.transport.pos.GetPositionZ();
            float worldO = movementInfo.transport.pos.GetOrientation();

            transport->CalculatePassengerPosition(worldX, worldY, worldZ, &worldO);

            // Reject invalid derived world coordinates before changing transport membership.
            if (!Trinity::IsValidMapCoord(worldX, worldY, worldZ, worldO))
                return false;

            if (plrMover)
            {
                if (!currentTransport)
                    transport->AddPassenger(plrMover);
                else if (currentTransport != transport)
                {
                    currentTransport->RemovePassenger(plrMover);
                    transport->AddPassenger(plrMover);
                }
            }

            movementInfo.transport.guid = transport->GetGUID();
            movementInfo.pos.Relocate(worldX, worldY, worldZ, worldO);
        }
        else
        {
            // No server-side Transport object for this GUID.
            // This can still be a non-map GAMEOBJECT_TYPE_TRANSPORT, so preserve that behavior.
            bool validGameObjectTransport = false;

            if (GameObject* go = mover->GetMap()->GetGameObject(movementInfo.transport.guid))
                validGameObjectTransport = go->GetGoType() == GAMEOBJECT_TYPE_TRANSPORT;

            if (plrMover && currentTransport && currentTransport->GetGUID() != movementInfo.transport.guid)
                currentTransport->RemovePassenger(plrMover);

            if (!validGameObjectTransport)
            {
                movementInfo.RemoveMovementFlag(MOVEMENTFLAG_ONTRANSPORT);
                movementInfo.transport.Reset();
            }
        }
    }
    else if (plrMover && plrMover->GetTransport())
    {
        plrMover->GetTransport()->RemovePassenger(plrMover);
        movementInfo.transport.Reset();
    }

    return true;
}

bool Channel::CanSpeak(ObjectGuid const& guid) const
{
    PlayerContainer::const_iterator itr = _playersStore.find(guid);
    return itr != _playersStore.end() && !itr->second.IsMuted();
}

void Channel::SayRemote(std::string const& senderName, std::string const& what, uint8 chatTag) const
{
    if (what.empty() || senderName.empty())
        return;

    ObjectGuid const senderGuid = ObjectGuid::Create<HighGuid::Player>(CROSS_REALM_CHAT_SENDER_GUID);

    auto builder = [&](WorldPacket& data, LocaleConstant locale)
    {
        LocaleConstant localeIdx = sWorld->GetAvailableDbcLocale(locale);

        WorldPackets::Chat::Chat packet;
        packet.Initialize(CHAT_MSG_CHANNEL, LANG_UNIVERSAL, nullptr, nullptr, what, 0, GetName(localeIdx), DEFAULT_LOCALE);
        packet.SenderGUID = senderGuid;
        packet.TargetGUID = senderGuid;
        packet.SetRemoteSender(senderName);
        packet.ChatTag = chatTag;

        packet.Write();

        data = packet.Move();
    };

    // Inlined rather than SendToAll, that template is only defined in Channel.cpp
    Trinity::LocalizedPacketDo<decltype(builder)> localizer(builder);
    for (PlayerContainer::value_type const& member : _playersStore)
        if (Player* player = ObjectAccessor::FindConnectedPlayer(member.first))
            localizer(player);
}

void WorldSession::SendWorldChannelInvite()
{
    if (_worldChatInvited || !sWorld->getBoolConfig(CONFIG_WORLD_CHAT))
        return;

    _worldChatInvited = true;

    Player* player = GetPlayer();
    if (!player || !player->IsInWorld())
        return;

    player->m_Events.AddEventAtOffset([player]
    {
        for (Channel* channel : player->GetJoinedChannels())
            if (Channel::IsWorldChat(channel->GetName()))
                return;

        WorldPackets::Channel::ChannelNotify notify;
        notify.Type = CHAT_INVITE_NOTICE;
        notify._Channel = std::string(WORLD_CHAT);
        notify.SenderGuid = ObjectGuid::Create<HighGuid::Player>(WORLD_CHAT_INVITER_GUID);
        player->GetSession()->SendPacket(notify.Write());
    }, 5s);
}

void InstanceScript::ForceRespawnQueuedCreaturesByEntry(std::vector<uint32> const& entries)
{
    std::vector<RespawnInfo const*> respawnData;
    instance->GetRespawnInfo(respawnData, SPAWN_TYPEMASK_CREATURE);
    for (RespawnInfo const* info : respawnData)
    {
        bool matched = false;
        for (uint32 entry : entries)
            if (info->entry == entry) { matched = true; break; }
        if (!matched)
            continue;

        bool aliveExists = false;
        std::vector<Creature*> deadCopies;
        auto bounds = instance->GetCreatureBySpawnIdStore().equal_range(info->spawnId);
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
        {
            Creature* creature = itr->second;
            if (!creature)
                continue;
            if (creature->IsAlive())
                aliveExists = true;
            else
                deadCopies.push_back(creature);
        }

        for (Creature* creature : deadCopies)
            creature->RemoveCorpse(false, true);

        if (!aliveExists)
            instance->Respawn(info->type, info->spawnId);
    }
}

bool lfg::LFGQueue::CompatibleKeyContainsGuid(std::string const& key, ObjectGuid guid)
{
    std::string const wanted = std::to_string(guid.GetRawValue());
    std::string::size_type begin = 0;

    while (begin <= key.size())
    {
        std::string::size_type end = key.find('|', begin);
        std::string token = end == std::string::npos ? key.substr(begin) : key.substr(begin, end - begin);

        if (token == wanted)
            return true;

        if (end == std::string::npos)
            break;

        begin = end + 1;
    }

    return false;
}

uint8 lfg::LFGQueue::CountGuidsInCompatibleKey(std::string const& key)
{
    if (key.empty())
        return 0;

    return uint8(std::count(key.begin(), key.end(), '|') + 1);
}

lfg::LFGMgr::LfgBindCleanupResult lfg::LFGMgr::TryClearOldLfgGroupBind(Group* group, uint32 lfgDungeonId, uint32 oldInstanceId)
{
    if (!group || !group->isLFGGroup() || !lfgDungeonId)
        return LfgBindCleanupResult::NoBind;

    LFGDungeonData const* dungeon = GetLFGDungeon(lfgDungeonId);
    if (!dungeon)
        return LfgBindCleanupResult::NoBind;

    MapEntry const* mapEntry = sMapStore.LookupEntry(dungeon->map);
    if (!mapEntry || !mapEntry->IsDungeon())
        return LfgBindCleanupResult::NoBind;

    // LFG requeue must never become a way to bypass raid or protected instance locks.
    if (mapEntry->IsRaid())
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing old group bind for raid map {}", dungeon->map);
        return LfgBindCleanupResult::Protected;
    }

    Difficulty difficulty = Difficulty(dungeon->difficulty);
    GetDownscaledMapDifficultyData(dungeon->map, difficulty);

    // Match existing reset-all semantics: do not clear heroic/protected dungeon binds here.
    if (difficulty > DUNGEON_DIFFICULTY_NORMAL)
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing old group bind for heroic/protected dungeon map {} difficulty {}",
            dungeon->map, uint32(difficulty));
        return LfgBindCleanupResult::Protected;
    }

    InstanceGroupBind* bind = group->GetBoundInstance(difficulty, dungeon->map);
    if (!bind || !bind->save)
        return LfgBindCleanupResult::NoBind;

    // Only clear the exact copy the player left - a faster member may already have rebound the group to a fresh one.
    if (oldInstanceId && bind->save->GetInstanceId() != oldInstanceId)
        return LfgBindCleanupResult::NoBind;

    // Permanent binds are authoritative locks and must not be bypassed by LFG requeue.
    if (bind->perm)
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing permanent group bind group={} map={} difficulty={} instance={}",
            group->GetGUID().ToString(), dungeon->map, uint32(difficulty), bind->save->GetInstanceId());
        return LfgBindCleanupResult::Protected;
    }

    // Non-resettable saves are protected by the instance system.
    if (!bind->save->CanReset())
    {
        TC_LOG_DEBUG("lfg.teleport", "LFG requeue: not clearing non-resettable group bind group={} map={} difficulty={} instance={}",
            group->GetGUID().ToString(), dungeon->map, uint32(difficulty), bind->save->GetInstanceId());
        return LfgBindCleanupResult::Protected;
    }

    uint32 const instanceId = bind->save->GetInstanceId();

    if (Map* boundMap = sMapMgr->FindMap(dungeon->map, instanceId))
        if (boundMap->IsDungeon())
            static_cast<InstanceMap*>(boundMap)->Reset(INSTANCE_RESET_RESPAWN_DELAY);

    group->UnbindInstance(dungeon->map, uint8(difficulty), false);

    TC_LOG_DEBUG("lfg.teleport", "LFG requeue: cleared resettable non-permanent group bind group={} map={} difficulty={} instance={}",
        group->GetGUID().ToString(), dungeon->map, uint32(difficulty), instanceId);

    return LfgBindCleanupResult::Cleared;
}

void lfg::LFGMgr::ProcessPendingTeleportIns(time_t currTime)
{
    for (std::map<ObjectGuid, PendingTeleportInData>::iterator itr = PendingTeleportInStore.begin(); itr != PendingTeleportInStore.end();)
    {
        ObjectGuid guid = itr->first;
        time_t expireTime = itr->second.ExpireTime;
        uint32 previousDungeonId = itr->second.PreviousDungeonId;
        uint32 oldInstanceId = itr->second.OldInstanceId;

        Player* player = ObjectAccessor::FindConnectedPlayer(guid);
        if (!player)
        {
            if (currTime >= expireTime)
                itr = PendingTeleportInStore.erase(itr);
            else
                ++itr;

            continue;
        }

        if (player->IsBeingTeleported())
        {
            if (currTime >= expireTime)
                itr = PendingTeleportInStore.erase(itr);
            else
                ++itr;

            continue;
        }

        Group* group = player->GetGroup();
        if (!group || !group->isLFGGroup())
        {
            itr = PendingTeleportInStore.erase(itr);
            continue;
        }

        LFGDungeonData const* dungeon = GetLFGDungeon(GetDungeon(group->GetGUID()));
        if (!dungeon)
        {
            itr = PendingTeleportInStore.erase(itr);
            continue;
        }

        if (player->GetMapId() == uint32(dungeon->map) && player->GetMap()->IsDungeon())
        {
            if (currTime >= expireTime)
            {
                TC_LOG_DEBUG("lfg.teleport", "Deferred LFG teleport for player {} expired while still inside map {}", player->GetName(), dungeon->map);
                itr = PendingTeleportInStore.erase(itr);
            }
            else
                ++itr;

            continue;
        }

        if (previousDungeonId)
            TryClearOldLfgGroupBind(group, previousDungeonId, oldInstanceId);

        itr = PendingTeleportInStore.erase(itr);
        TeleportPlayer(player, false, false, true, previousDungeonId);
    }
}

EG::SetRaceMasqueradeSetting::SetRaceMasqueradeSetting(Player* owner, Races selectedRace) : _owner(owner), _selectedRace(selectedRace)
{
}

bool EG::SetRaceMasqueradeSetting::Execute(uint64, uint32)
{
    if (!_owner->IsInWorld())
        return false;

    _owner->SetMasqueradeRace(_selectedRace);
    return true;
}

EG::CleanRaceMasquerade::CleanRaceMasquerade(Player* owner) : _owner(owner)
{
}

bool EG::CleanRaceMasquerade::Execute(uint64, uint32)
{
    if (!_owner->CleanMasqueradeRaceValue())
        return false;

    return true;
}
