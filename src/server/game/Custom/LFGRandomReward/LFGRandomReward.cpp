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

#include "LFGRandomReward.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Group.h"
#include "GroupReference.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Mail.h"
#include "ObjectMgr.h"
#include "Optional.h"
#include "Player.h"
#include "Random.h"
#include "WorldSession.h"
#include <iterator>
#include <span>
#include <string_view>
#include <vector>

namespace
{
    struct PoolItem
    {
        uint32 ItemId;
        uint32 MinQty;
        uint32 MaxQty;
    };

    struct LootPool
    {
        char const* Name;
        float DropChancePercent;
        bool GuaranteedFirst;
        std::span<PoolItem const> Items;
    };

    // Epic gems
    constexpr PoolItem EPIC_GEMS_ITEMS[] =
    {
        { 36919, 1, 2 }, // Cardinal Ruby
        { 36922, 1, 2 }, // King's Amber
        { 36925, 1, 2 }, // Majestic Zircon
        { 36928, 1, 2 }, // Dreadstone
        { 36931, 1, 2 }, // Ametrine
        { 36934, 1, 2 }, // Eye of Zul
    };

    // Craft materials
    constexpr PoolItem CRAFT_COMMON_ITEMS[] =
    {
        { 42253, 1, 10 }, // Iceweb Spider Silk
        { 36860, 1, 10 }, // Eternal Fire
        { 35624, 1, 10 }, // Eternal Earth
        { 35622, 1, 10 }, // Eternal Water
        { 35623, 1, 10 }, // Eternal Air
        { 35625, 1, 10 }, // Eternal Life
        { 35627, 1, 10 }, // Eternal Shadow
        { 38558, 1, 10 }, // Nerubian Chitin
        { 36903, 2, 15 }, // Adder's Tongue
        { 34055, 1, 5 }, // Greater Cosmic Essence
        { 34052, 1, 5 }, // Dream Shard
    };

    // Craft materials - uncommon
    constexpr PoolItem CRAFT_UNCOMMON_ITEMS[] =
    {
        { 38425, 1, 5 }, // Heavy Borean Leather
        { 41511, 1, 5 }, // Bolt of Imbued Frostweave
        { 43007, 1, 5 }, // Northern Spices
        { 34057, 1, 1 }, // Abyss Crystal
        { 43127, 1, 5 }, // Snowfall Ink
    };

    // Craft materials - rare
    constexpr PoolItem CRAFT_RARE_ITEMS[] =
    {
        { 43102, 1, 1 }, // Frozen Orb
        { 43297, 1, 1 }, // Damaged Necklage
    };

    // Relics - rare
    constexpr PoolItem RELIC_RARE_ITEMS[] =
    {
        { 50255, 1, 1 }, // Dread Pirate Ring
    };

    // Fortune
    constexpr PoolItem FORTUNE_ITEMS[] =
    {
        { 32498, 1, 1 }, // Fortune Coin
    };

    // ---- Pool registry ----
    constexpr LootPool POOLS[] =
    {
        { "Epic Gems", 8.25f, false, EPIC_GEMS_ITEMS },
        { "Craft Materials", 20.0f, true, CRAFT_COMMON_ITEMS },
        { "Craft Materials (Uncommon)", 20.0f, false, CRAFT_UNCOMMON_ITEMS },
        { "Craft Materials (Rare)", 5.0f, false, CRAFT_RARE_ITEMS },
        { "Relics (Rare)", 1.0f, false, RELIC_RARE_ITEMS },
        { "Fortune", 0.0825f, false, FORTUNE_ITEMS },
    };

    constexpr bool ValidatePools()
    {
        for (LootPool const& p : POOLS)
        {
            if (p.Items.empty())
                return false;
            if (p.DropChancePercent < 0.0f || p.DropChancePercent > 100.0f)
                return false;
            for (PoolItem const& item : p.Items)
            {
                if (item.MinQty < 1 || item.MaxQty < item.MinQty)
                    return false;
            }
        }
        return true;
    }
    static_assert(std::size(POOLS) > 0, "LFGRandomReward: at least one pool must be registered");
    static_assert(ValidatePools(), "LFGRandomReward: pool config invalid (empty items / bad chance / MaxQty < MinQty)");

    constexpr uint32 SENDER_NPC_ENTRY = 34337;
    constexpr std::string_view MAIL_SUBJECT = "Random Group Reward";
    constexpr std::string_view MAIL_BODY = "Bonus reward for completing a heroic dungeon with a fully random group.";

    uint32 RollGeometricQty(uint32 min, uint32 max)
    {
        uint32 qty = min;
        while (qty < max)
        {
            if (roll_chance_i(40))
                break;
            ++qty;
        }
        return qty;
    }

    void BroadcastHit(Group* group, std::string const& playerName, char const* poolName, std::string const& itemName, uint32 quantity)
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
            if (Player* member = itr->GetSource())
                if (WorldSession* session = member->GetSession())
                    ChatHandler(session).PSendSysMessage("[Random Group Reward] %s received %s x%u from pool '%s'.", playerName.c_str(), itemName.c_str(), quantity, poolName);
    }
}

void LFGRandomReward::TryReward(Player* player, Group* group)
{
    if (!player || !group)
        return;

    std::string const& playerName = player->GetName();
    CharacterDatabaseTransaction trans;
    Optional<MailDraft> draft;

    struct PendingDrop
    {
        char const* PoolName;
        uint32 ItemId;
        uint32 Quantity;
    };
    std::vector<PendingDrop> drops;

    auto grant = [&](LootPool const& pool, PoolItem const& item)
    {
        uint32 const quantity = RollGeometricQty(item.MinQty, item.MaxQty);
        Item* created = Item::CreateItem(item.ItemId, quantity, player);
        if (!created)
            return;
        if (!trans)
        {
            trans = CharacterDatabase.BeginTransaction();
            draft.emplace(std::string{MAIL_SUBJECT}, std::string{MAIL_BODY});
        }
        created->SaveToDB(trans);
        draft->AddItem(created);
        drops.push_back({ pool.Name, item.ItemId, quantity });
    };

    for (LootPool const& pool : POOLS)
    {
        if (pool.GuaranteedFirst)
        {
            uint32 const itemCount = uint32(pool.Items.size());
            uint32 const firstIdx = urand(0, itemCount - 1);
            grant(pool, pool.Items[firstIdx]);

            if (itemCount >= 2 && roll_chance_f(pool.DropChancePercent))
            {
                uint32 secondIdx = urand(0, itemCount - 2);
                if (secondIdx >= firstIdx)
                    ++secondIdx;
                grant(pool, pool.Items[secondIdx]);
            }
        }
        else if (roll_chance_f(pool.DropChancePercent))
            grant(pool, pool.Items[urand(0, uint32(pool.Items.size()) - 1)]);
    }

    if (!draft)
        return;

    // Persist first, announce second
    draft->SendMailTo(trans, player, MailSender(MAIL_CREATURE, SENDER_NPC_ENTRY));
    CharacterDatabase.CommitTransaction(trans);

    for (PendingDrop const& drop : drops)
    {
        ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(drop.ItemId);
        std::string const itemName = itemTemplate ? itemTemplate->Name1 : std::to_string(drop.ItemId);
        BroadcastHit(group, playerName, drop.PoolName, itemName, drop.Quantity);
    }
}
