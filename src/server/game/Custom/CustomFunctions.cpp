#include "CustomFunctions.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "Map.h"
#include "MotionMaster.h"
#include "Log.h"
#include "Object.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Optional.h"
#include "Player.h"
#include "PlayerTaxi.h"
#include "SharedDefines.h"
#include "SmartAI.h"
#include "SpellMgr.h"
#include "SpellInfo.h"
#include "StringConvert.h"
#include "Transmogrification.h"
#include "Unit.h"
#include "Util.h"
#include "World.h"
#include <unordered_map>
#include <vector>


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
        for (std::pair<uint32, uint32> currentValue : spellIdsByTeam)
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
                        if (GetLevel() >= 77)
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
        if (HasCustomFlag(CustomFlagsIndex::CUSTOM_ACCOUNT_TAXI, CustomFlags::CUSTOM_FLAG_ACCOUNT_TAXI_ACTIVE))
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
        for (ObjectGuid itemGUID : items)
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

    for (LootReference currentLoot : StoredLoot)
    {
        if (currentLoot.ContainerEntityGUID == lootGUID)
            return currentLoot.RelatedLoot;
    }
    return nullptr;
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
        && unit->IsInCombat()
        && _source->IsValidAssistTarget(unit)
        && _source->IsWithinDistInMap(unit, _range)
        && unit->GetHealthPct() < _hp
    )
    {
        _hp = unit->GetHealthPct();
        return true;
    }

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
