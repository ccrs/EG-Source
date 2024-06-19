#include "Bag.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "DBCStores.h"
#include "GameEventMgr.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Language.h"
#include "Log.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "QueryResult.h"
#include "RBAC.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "Transaction.h"
#include "WorldSession.h"
#include "Transmogrification.h"

Transmogrification* Transmogrification::instance()
{
    static Transmogrification instance;
    return &instance;
}

const char* Transmogrification::GetSlotName(uint8 slot) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::GetSlotName with value: slot (%u)", slot);

    switch (slot)
    {
        case EQUIPMENT_SLOT_HEAD:
            return  "Head";
        case EQUIPMENT_SLOT_SHOULDERS:
            return  "Shoulders";
        case EQUIPMENT_SLOT_BODY:
            return  "Shirt";
        case EQUIPMENT_SLOT_CHEST:
            return  "Chest";
        case EQUIPMENT_SLOT_WAIST:
            return  "Waist";
        case EQUIPMENT_SLOT_LEGS:
            return  "Legs";
        case EQUIPMENT_SLOT_FEET:
            return  "Feet";
        case EQUIPMENT_SLOT_WRISTS:
            return  "Wrists";
        case EQUIPMENT_SLOT_HANDS:
            return  "Hands";
        case EQUIPMENT_SLOT_BACK:
            return  "Back";
        case EQUIPMENT_SLOT_MAINHAND:
            return  "Main hand";
        case EQUIPMENT_SLOT_OFFHAND:
            return  "Off hand";
        case EQUIPMENT_SLOT_RANGED:
            return  "Ranged";
        case EQUIPMENT_SLOT_TABARD:
            return  "Tabard";
        default:
            return nullptr;
    }
}

std::string Transmogrification::GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::GetItemIcon with values: entry (%u), width (%u), height (%u), x (%d), y (%i)", entry, width, height, x, y);

    std::string value = "|TInterface";
    ItemTemplate const* temp = sObjectMgr->GetItemTemplate(entry);
    ItemDisplayInfoEntry const* dispInfo = nullptr;
    if (temp)
    {
        dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
        if (dispInfo)
            value += "/ICONS/" + std::string(dispInfo->InventoryIcon[0]);
    }
    if (!dispInfo)
        value += "/InventoryItems/WoWUnknownItem01";
    value += ":" + std::to_string(width) + ":" + std::to_string(height) + ":" + std::to_string(x) + ":" + std::to_string(y) + "|t";
    return value;
}

std::string Transmogrification::GetSlotIcon(uint8 slot, uint32 width, uint32 height, int x, int y) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::GetSlotIcon with values: slot (%u), width (%u), height (%u), x (%d), y (%i)", slot, width, height, x, y);

    std::string value = "|TInterface/PaperDoll/";
    switch (slot)
    {
        case EQUIPMENT_SLOT_HEAD:
            value += "UI-PaperDoll-Slot-Head";
            break;
        case EQUIPMENT_SLOT_SHOULDERS:
            value += "UI-PaperDoll-Slot-Shoulder";
            break;
        case EQUIPMENT_SLOT_BODY:
            value += "UI-PaperDoll-Slot-Shirt";
            break;
        case EQUIPMENT_SLOT_CHEST:
            value += "UI-PaperDoll-Slot-Chest";
            break;
        case EQUIPMENT_SLOT_WAIST:
            value += "UI-PaperDoll-Slot-Waist";
            break;
        case EQUIPMENT_SLOT_LEGS:
            value += "UI-PaperDoll-Slot-Legs";
            break;
        case EQUIPMENT_SLOT_FEET:
            value += "UI-PaperDoll-Slot-Feet";
            break;
        case EQUIPMENT_SLOT_WRISTS:
            value += "UI-PaperDoll-Slot-Wrists";
            break;
        case EQUIPMENT_SLOT_HANDS:
            value += "UI-PaperDoll-Slot-Hands";
            break;
        case EQUIPMENT_SLOT_BACK:
            value += "UI-PaperDoll-Slot-Chest";
            break;
        case EQUIPMENT_SLOT_MAINHAND:
            value += "UI-PaperDoll-Slot-MainHand";
            break;
        case EQUIPMENT_SLOT_OFFHAND:
            value += "UI-PaperDoll-Slot-SecondaryHand";
            break;
        case EQUIPMENT_SLOT_RANGED:
            value += "UI-PaperDoll-Slot-Ranged";
            break;
        case EQUIPMENT_SLOT_TABARD:
            value += "UI-PaperDoll-Slot-Tabard";
            break;
        default:
            value += "UI-Backpack-EmptySlot";
    }
    value += ":" + std::to_string(width) + ":" + std::to_string(height) + ":" + std::to_string(x) + ":" + std::to_string(y) + "|t";
    return value;
}

std::string Transmogrification::GetItemLink(Item* item, WorldSession* session) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::GetItemLink called with value: item (%u)", item->GetEntry());

    LocaleConstant loc_idx = session->GetSessionDbLocaleIndex();
    ItemTemplate const* temp = item->GetTemplate();
    std::string name = temp->Name1;
    if (ItemLocale const* il = sObjectMgr->GetItemLocale(temp->ItemId))
        ObjectMgr::GetLocaleString(il->Name, loc_idx, name);

    if (int32 itemRandPropId = item->GetItemRandomPropertyId())
    {
        std::array<char const*, 16> const* suffix = nullptr;
        if (itemRandPropId < 0)
        {
            const ItemRandomSuffixEntry* itemRandEntry = sItemRandomSuffixStore.LookupEntry(-item->GetItemRandomPropertyId());
            if (itemRandEntry)
                suffix = &itemRandEntry->Name;
        }
        else
        {
            const ItemRandomPropertiesEntry* itemRandEntry = sItemRandomPropertiesStore.LookupEntry(item->GetItemRandomPropertyId());
            if (itemRandEntry)
                suffix = &itemRandEntry->Name;
        }
        if (suffix)
        {
            std::string_view test((*suffix)[(name != temp->Name1) ? loc_idx : DEFAULT_LOCALE]);
            if (!test.empty())
            {
                name += ' ';
                name += test;
            }
        }
    }

    std::ostringstream oss;
    oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec << "|Hitem:" << temp->ItemId << ":" <<
        item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT) << ":" << item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT) << ":" <<
        item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2) << ":" << item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3) << ":" <<
        item->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT) << ":" << item->GetItemRandomPropertyId() << ":" <<
        item->GetItemSuffixFactor() << ":" << item->GetOwner()->GetLevel() << "|h[" << name << "]|h|r";

    return oss.str();
}

std::string Transmogrification::GetItemLink(uint32 entry, WorldSession* session) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::GetItemLink called with value: entry (%u)", entry);

    ItemTemplate const* temp = sObjectMgr->GetItemTemplate(entry);
    LocaleConstant loc_idx = session->GetSessionDbLocaleIndex();
    std::string name = temp->Name1;
    if (ItemLocale const* il = sObjectMgr->GetItemLocale(entry))
        ObjectMgr::GetLocaleString(il->Name, loc_idx, name);

    std::ostringstream oss;
    oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec << "|Hitem:" << entry << ":0:0:0:0:0:0:0:0:0|h[" << name << "]|h|r";

    return oss.str();
}

uint32 Transmogrification::GetFakeEntry(Item const* item) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::GetFakeEntry called with value: item (%u)", item->GetEntry());

    Player* owner = item->GetOwner();

    if (!owner)
        return 0;

    return owner->GetTransmogrificationEntry(item->GetGUID());
}

void Transmogrification::UpdateItem(Player* player, Item* item) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::UpdateItem called with values: player (%s, %u), item (%u)", player->GetName().c_str(), player->GetGUID().GetCounter(), item->GetEntry());

    if (item->IsEquipped())
    {
        player->SetVisibleItemSlot(item->GetSlot(), item);
        if (player->IsInWorld())
            item->SendUpdateToPlayer(player);
    }
}

void Transmogrification::DeleteFakeEntry(Player* player, Item* item)
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::DeleteFakeEntry called with values: player (%s, %u), item (%u)", player->GetName().c_str(), player->GetGUID().GetCounter(), item->GetEntry());

    if (player->EraseTransmogrificationEntry(item->GetGUID()))
        UpdateItem(player, item);
}

void Transmogrification::SetFakeEntry(Player* player, Item* item, uint32 entry)
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::SetFakeEntry called with values: player (%s, %u), item (%u)", player->GetName().c_str(), player->GetGUID().GetCounter(), item->GetEntry());

    player->InsertTransmogrificationEntry(item->GetGUID(), entry);
    UpdateItem(player, item);
}

TrinityStrings Transmogrification::Transmogrify(Player* player, ObjectGuid itemGUID, uint8 slot, bool noCost)
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::Transmogrify called with values: player (%s, %u), itemGUID (%u), slot (%u), cost (%s)", player->GetName().c_str(), player->GetGUID().GetCounter(), itemGUID.GetCounter(), slot, noCost ? "true" : "false");

    // slot of the transmogrified item
    if (slot >= EQUIPMENT_SLOT_END)
    {
        TC_LOG_DEBUG("transmogrification", "Transmogrification::Transmogrify - %s (%s) tried to transmogrify an %s with a wrong slot (%u) when transmogrifying items.", player->GetName().c_str(), player->GetGUID().ToString().c_str(), itemGUID.ToString().c_str(), slot);
        return LANG_ERR_TRANSMOG_INVALID_SLOT;
    }

    Item* itemTransmogrifier = nullptr;
    // guid of the transmogrifier item, if it's not 0
    if (!itemGUID.IsEmpty())
    {
        itemTransmogrifier = player->GetItemByGuid(itemGUID);
        if (!itemTransmogrifier)
        {
            TC_LOG_DEBUG("transmogrification", "Transmogrification::Transmogrify - %s (%s) tried to transmogrify with an invalid %s.", player->GetName().c_str(), player->GetGUID().ToString().c_str(), itemGUID.ToString().c_str());
            return LANG_ERR_TRANSMOG_MISSING_SRC_ITEM;
        }
    }

    // transmogrified item
    Item* itemTransmogrified = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
    if (!itemTransmogrified)
    {
        TC_LOG_DEBUG("transmogrification", "Transmogrification::Transmogrify - %s (%s) tried to transmogrify an invalid item in a valid slot (slot: %u).", player->GetName().c_str(), player->GetGUID().ToString().c_str(), slot);
        return LANG_ERR_TRANSMOG_MISSING_DEST_ITEM;
    }

    if (!itemTransmogrifier) // reset look newEntry
    {
        DeleteFakeEntry(player, itemTransmogrified);
    }
    else
    {
        if (!CanTransmogrifyItemWithItem(player, itemTransmogrified->GetTemplate(), itemTransmogrifier->GetTemplate()))
        {
            TC_LOG_DEBUG("transmogrification", "Transmogrification::Transmogrify - %s (%s) failed CanTransmogrifyItemWithItem (%u with %u).", player->GetName().c_str(), player->GetGUID().ToString().c_str(), itemTransmogrified->GetEntry(), itemTransmogrifier->GetEntry());
            return LANG_ERR_TRANSMOG_INVALID_ITEMS;
        }

        if (!noCost)
        {
            if (RequireToken)
            {
                if (player->HasItemCount(TokenEntry, TokenAmount))
                    player->DestroyItemCount(TokenEntry, TokenAmount, true);
                else
                    return LANG_ERR_TRANSMOG_NOT_ENOUGH_TOKENS;
            }

            int32 cost = 0;
            cost = GetSpecialPrice(itemTransmogrified->GetTemplate());
            cost *= ScaledCostModifier;
            cost += CopperCost;

            if (player->GetSession()->HasPermission(rbac::RBAC_PERM_FREE_TRANSMOGRIFICATION))
                cost = 0;

            if (cost) // 0 cost if reverting look
            {
                if (cost < 0)
                    TC_LOG_DEBUG("transmogrification", "Transmogrification::Transmogrify - %s (%s) transmogrification invalid cost (non negative, amount %i). Transmogrified %u with %u", player->GetName().c_str(), player->GetGUID().ToString().c_str(), -cost, itemTransmogrified->GetEntry(), itemTransmogrifier->GetEntry());
                else
                {
                    if (!player->HasEnoughMoney(cost))
                        return LANG_ERR_TRANSMOG_NOT_ENOUGH_MONEY;
                    player->ModifyMoney(-cost, false);
                }
            }
        }

        SetFakeEntry(player, itemTransmogrified, itemTransmogrifier->GetEntry());

        itemTransmogrified->UpdatePlayedTime(player);

        itemTransmogrified->SetOwnerGUID(player->GetGUID());
        itemTransmogrified->SetNotRefundable(player);
        itemTransmogrified->ClearSoulboundTradeable(player);

        if (itemTransmogrifier->GetTemplate()->Bonding == BIND_WHEN_EQUIPED || itemTransmogrifier->GetTemplate()->Bonding == BIND_WHEN_USE)
            itemTransmogrifier->SetBinding(true);

        itemTransmogrifier->SetOwnerGUID(player->GetGUID());
        itemTransmogrifier->SetNotRefundable(player);
        itemTransmogrifier->ClearSoulboundTradeable(player);
    }

    return LANG_ERR_TRANSMOG_OK;
}

bool Transmogrification::CanTransmogrifyItemWithItem(Player* player, ItemTemplate const* target, ItemTemplate const* source) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::CanTransmogrifyItemWithItem called with values: player (%s, %u), target (%u), source (%u)", player->GetName().c_str(), player->GetGUID().GetCounter(), target->ItemId, source->ItemId);

    if (!target || !source)
        return false;

    if (source->ItemId == target->ItemId)
        return false;

    if (source->DisplayInfoID == target->DisplayInfoID)
        return false;

    if (source->Class != target->Class)
        return false;

    if (source->InventoryType == INVTYPE_BAG || source->InventoryType == INVTYPE_RELIC || source->InventoryType == INVTYPE_FINGER ||
        source->InventoryType == INVTYPE_TRINKET || source->InventoryType == INVTYPE_AMMO || source->InventoryType == INVTYPE_QUIVER)
        return false;

    if (target->InventoryType == INVTYPE_BAG || target->InventoryType == INVTYPE_RELIC || target->InventoryType == INVTYPE_FINGER ||
        target->InventoryType == INVTYPE_TRINKET || target->InventoryType == INVTYPE_AMMO || target->InventoryType == INVTYPE_QUIVER)
        return false;

    if (!SuitableForTransmogrification(player, target) || !SuitableForTransmogrification(player, source))
        return false;

    if (IsRangedWeapon(source->Class, source->SubClass) != IsRangedWeapon(target->Class, target->SubClass))
        return false;

    if (source->SubClass != target->SubClass && !IsRangedWeapon(target->Class, target->SubClass))
    {
        if (source->Class == ITEM_CLASS_ARMOR && !AllowMixedArmorTypes)
            return false;
        if (source->Class == ITEM_CLASS_WEAPON && !AllowMixedWeaponTypes)
            return false;
    }

    if (source->InventoryType != target->InventoryType || (AllowMixedWeaponTypes && source->Class == ITEM_CLASS_WEAPON))
    {
        if (source->Class == ITEM_CLASS_WEAPON)
        {
            if (!((target->Class == source->Class && target->SubClass == source->SubClass) ||
                (IsRangedWeapon(target->Class, target->SubClass) && IsRangedWeapon(source->Class, source->SubClass)) ||
                (IsTwoHandedWeapon(target->Class, target->SubClass) && IsTwoHandedWeapon(source->Class, source->SubClass)) ||
                (IsOneHandedWeapon(target->Class, target->SubClass) && IsOneHandedWeapon(source->Class, source->SubClass)) ||
                (IsMainHandDagger(target) && IsOneHandedWeapon(source->Class, source->SubClass))))
                return false;
            if (source->InventoryType == INVTYPE_WEAPONMAINHAND && target->InventoryType != INVTYPE_WEAPONMAINHAND)
                return false;
            if (source->InventoryType == INVTYPE_WEAPONOFFHAND && target->InventoryType != INVTYPE_WEAPONOFFHAND)
                return false;
        }
        if (source->Class == ITEM_CLASS_ARMOR)
        {
            if (source->InventoryType != INVTYPE_CHEST || target->InventoryType != INVTYPE_ROBE)
                if (source->InventoryType != INVTYPE_ROBE || target->InventoryType != INVTYPE_CHEST)
                    return false;
        }
    }

    return true;
}

bool Transmogrification::SuitableForTransmogrification(Player* player, ItemTemplate const* proto) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::SuitableForTransmogrification called with values: player (%s, %u), proto (%u)", player->GetName().c_str(), player->GetGUID().GetCounter(), proto->ItemId);

    // ItemTemplate const* proto = item->GetTemplate();
    if (!player || !proto)
        return false;

    if (proto->Class != ITEM_CLASS_ARMOR && proto->Class != ITEM_CLASS_WEAPON)
        return false;

    // Skip all checks for allowed items
    if (IsAllowed(proto->ItemId))
        return true;

    if (IsNotAllowed(proto->ItemId))
        return false;

    if (!AllowFishingPoles && proto->Class == ITEM_CLASS_WEAPON && proto->SubClass == ITEM_SUBCLASS_WEAPON_FISHING_POLE)
        return false;

    if (!IsAllowedQuality(proto->Quality))
        return false;

    if ((proto->Flags2 & ITEM_FLAG2_FACTION_HORDE) && player->GetTeam() != HORDE)
        return false;

    if ((proto->Flags2 & ITEM_FLAG2_FACTION_ALLIANCE) && player->GetTeam() != ALLIANCE)
        return false;

    if (!IgnoreReqClass && (proto->AllowableClass & player->GetClassMask()) == 0)
        return false;

    if (!IgnoreReqRace && (proto->AllowableRace & player->GetRaceMask()) == 0)
        return false;

    if (!IgnoreReqSkill && proto->RequiredSkill != 0)
    {
        if (player->GetSkillValue(proto->RequiredSkill) == 0)
            return false;
        else if (player->GetSkillValue(proto->RequiredSkill) < proto->RequiredSkillRank)
            return false;
    }

    if (!IgnoreReqSpell && proto->RequiredSpell != 0 && !player->HasSpell(proto->RequiredSpell))
        return false;

    if (!IgnoreReqLevel && player->GetLevel() < proto->RequiredLevel)
        return false;

    // If World Event is not active, prevent using event dependant items
    if (!IgnoreReqEvent && proto->HolidayId && !IsHolidayActive((HolidayIds)proto->HolidayId))
        return false;

    if (!IgnoreReqStats)
    {
        if (!proto->RandomProperty && !proto->RandomSuffix)
        {
            bool found = false;
            for (uint8 i = 0; i < proto->StatsCount; ++i)
            {
                if (proto->ItemStat[i].ItemStatValue != 0)
                {
                    found = true;
                    break;
                }
            }
            
            if (proto->Spells[0].SpellId != 0)
                found = true;
            
            switch (proto->ItemId)
            {
                case 50741: // Vile Fumigator's Mask
                    found = false;
                    break;
                case 2565: // Rod of Molten Fire
                case 5248: // Flash Wand
                case 7682: // Torturing Poker
                case 13004: // Torch of Austen
                case 13137: // Ironweaver
                case 14100: // Brightcloth Robe
                case 16039: // Ta'kierthan Songblade
                case 19087: // Frostsaber Gloves
                case 28301: // Syrannis Mystic Sheen
                    found = true;
                    break;
                default:
                    break;
            }


            if (!found)
                return false;
        }
    }

    return true;
}

uint32 Transmogrification::GetSpecialPrice(ItemTemplate const* proto) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::GetSpecialPrice");

    uint32 cost = proto->SellPrice < 10000 ? 10000 : proto->SellPrice;

    if (DisableCost)
        cost = 0;

    return cost;
}

bool Transmogrification::IsOneHandedWeapon(uint32 Class, uint32 SubClass) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::IsOneHandedWeapon");

    return Class == ITEM_CLASS_WEAPON && (
        SubClass == ITEM_SUBCLASS_WEAPON_AXE ||
        SubClass == ITEM_SUBCLASS_WEAPON_MACE ||
        SubClass == ITEM_SUBCLASS_WEAPON_SWORD);
}

bool Transmogrification::IsTwoHandedWeapon(uint32 Class, uint32 SubClass) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::IsTwoHandedWeapon");

    return Class == ITEM_CLASS_WEAPON && (
        SubClass == ITEM_SUBCLASS_WEAPON_AXE2 ||
        SubClass == ITEM_SUBCLASS_WEAPON_MACE2 ||
        SubClass == ITEM_SUBCLASS_WEAPON_SWORD2 ||
        SubClass == ITEM_SUBCLASS_WEAPON_STAFF ||
        SubClass == ITEM_SUBCLASS_WEAPON_POLEARM);
}

bool Transmogrification::IsMainHandDagger(ItemTemplate const* item) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::IsMainHandDagger");

    return item->Class == ITEM_CLASS_WEAPON && item->InventoryType == INVTYPE_WEAPONMAINHAND && item->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER;
}

bool Transmogrification::IsRangedWeapon(uint32 Class, uint32 SubClass) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::IsRangedWeapon");

    return Class == ITEM_CLASS_WEAPON && (
        SubClass == ITEM_SUBCLASS_WEAPON_BOW ||
        SubClass == ITEM_SUBCLASS_WEAPON_GUN ||
        SubClass == ITEM_SUBCLASS_WEAPON_CROSSBOW);
}

bool Transmogrification::IsAllowed(uint32 entry) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::IsAllowed");

    return Allowed.find(entry) != Allowed.end();
}

bool Transmogrification::IsNotAllowed(uint32 entry) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::IsNotAllowed");

    return NotAllowed.find(entry) != NotAllowed.end();
}

bool Transmogrification::IsAllowedQuality(uint32 quality) const
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::IsAllowedQuality");

    switch (quality)
    {
        case ITEM_QUALITY_POOR:
            return AllowPoor;
        case ITEM_QUALITY_NORMAL:
            return AllowCommon;
        case ITEM_QUALITY_UNCOMMON:
            return AllowUncommon;
        case ITEM_QUALITY_RARE:
            return AllowRare;
        case ITEM_QUALITY_EPIC:
            return AllowEpic;
        case ITEM_QUALITY_LEGENDARY:
            return AllowLegendary;
        case ITEM_QUALITY_ARTIFACT:
            return AllowArtifact;
        case ITEM_QUALITY_HEIRLOOM:
            return AllowHeirloom;
        default:
            return false;
    }
}

void Transmogrification::LoadConfig(bool /*reload*/)
{
    TC_LOG_DEBUG("transmogrification", "Transmogrification::LoadConfig");

    EnableTransmogInfo = sConfigMgr->GetBoolDefault("Transmogrification.EnableTransmogInfo", true);
    TransmogNpcText = uint32(sConfigMgr->GetIntDefault("Transmogrification.TransmogNpcText", 50000));

    std::istringstream issAllowed(sConfigMgr->GetStringDefault("Transmogrification.Allowed", ""));
    std::istringstream issNotAllowed(sConfigMgr->GetStringDefault("Transmogrification.NotAllowed", ""));
    Allowed.clear();
    while (issAllowed.good())
    {
        uint32 entry;
        issAllowed >> entry;
        if (issAllowed.fail())
            break;
        Allowed.insert(entry);
    }
    NotAllowed.clear();
    while (issNotAllowed.good())
    {
        uint32 entry;
        issNotAllowed >> entry;
        if (issNotAllowed.fail())
            break;
        NotAllowed.insert(entry);
    }

    ScaledCostModifier = sConfigMgr->GetFloatDefault("Transmogrification.ScaledCostModifier", 1.0f);
    CopperCost = sConfigMgr->GetIntDefault("Transmogrification.CopperCost", 0);

    RequireToken = sConfigMgr->GetBoolDefault("Transmogrification.RequireToken", false);
    TokenEntry = uint32(sConfigMgr->GetIntDefault("Transmogrification.TokenEntry", 49426));
    TokenAmount = uint32(sConfigMgr->GetIntDefault("Transmogrification.TokenAmount", 1));

    AllowPoor = sConfigMgr->GetBoolDefault("Transmogrification.AllowPoor", false);
    AllowCommon = sConfigMgr->GetBoolDefault("Transmogrification.AllowCommon", false);
    AllowUncommon = sConfigMgr->GetBoolDefault("Transmogrification.AllowUncommon", true);
    AllowRare = sConfigMgr->GetBoolDefault("Transmogrification.AllowRare", true);
    AllowEpic = sConfigMgr->GetBoolDefault("Transmogrification.AllowEpic", true);
    AllowLegendary = sConfigMgr->GetBoolDefault("Transmogrification.AllowLegendary", false);
    AllowArtifact = sConfigMgr->GetBoolDefault("Transmogrification.AllowArtifact", false);
    AllowHeirloom = sConfigMgr->GetBoolDefault("Transmogrification.AllowHeirloom", true);

    AllowMixedArmorTypes = sConfigMgr->GetBoolDefault("Transmogrification.AllowMixedArmorTypes", false);
    AllowMixedWeaponTypes = sConfigMgr->GetBoolDefault("Transmogrification.AllowMixedWeaponTypes", false);
    AllowFishingPoles = sConfigMgr->GetBoolDefault("Transmogrification.AllowFishingPoles", false);

    IgnoreReqRace = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqRace", false);
    IgnoreReqClass = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqClass", false);
    IgnoreReqSkill = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqSkill", false);
    IgnoreReqSpell = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqSpell", false);
    IgnoreReqLevel = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqLevel", false);
    IgnoreReqEvent = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqEvent", false);
    IgnoreReqStats = sConfigMgr->GetBoolDefault("Transmogrification.IgnoreReqStats", false);

    if (!sObjectMgr->GetItemTemplate(TokenEntry))
    {
        TC_LOG_INFO("transmogrification", "Transmogrification.TokenEntry (%u) does not exist. Using default (%u).", TokenEntry, 49426);
        TokenEntry = 49426;
    }
}

std::vector<ObjectGuid> Transmogrification::GetItemList(Player const* player) const
{
    std::vector<ObjectGuid> itemlist;

    // Copy paste from Player::GetItemByGuid(guid)

    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemlist.push_back(pItem->GetGUID());

    for (uint8 i = KEYRING_SLOT_START; i < CURRENCYTOKEN_SLOT_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemlist.push_back(pItem->GetGUID());

    for (int i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            itemlist.push_back(pItem->GetGUID());

    for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
        if (Bag* pBag = player->GetBagByPos(i))
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                if (Item* pItem = pBag->GetItemByPos(j))
                    itemlist.push_back(pItem->GetGUID());

    for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
        if (Bag* pBag = player->GetBagByPos(i))
            for (uint32 j = 0; j < pBag->GetBagSize(); ++j)
                if (Item* pItem = pBag->GetItemByPos(j))
                    itemlist.push_back(pItem->GetGUID());

    return itemlist;
}
