#include "Bag.h"
#include "Config.h"
#include "Creature.h"
#include "DatabaseEnv.h"
#include "DBCStructure.h"
#include "GameEventMgr.h"
#include "GossipDef.h"
#include "Item.h"
#include "ItemTemplate.h"
#include "Language.h"
#include "Log.h"
#include "Player.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "RBAC.h"
#include "QueryResult.h"
#include "ScriptedGossip.h"
#include "ScriptedCreature.h"
#include "ScriptMgr.h"
#include "SharedDefines.h"
#include "Transaction.h"
#include "WorldSession.h"
#include "Transmogrification.h"

class EG_transmogrification_creature : public CreatureScript
{
public:
    EG_transmogrification_creature() : CreatureScript("EG_transmogrification_creature") { }

    struct EG_transmogrification_creatureAI : public ScriptedAI
    {
        EG_transmogrification_creatureAI(Creature* creature) : ScriptedAI(creature) { }

        bool OnGossipHello(Player* player) override
        {
            if (sTransmogrification->IsTransmogrificationInfoEnabled())
                AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Book_11:30:30:-18:0|tHow transmogrification works", EQUIPMENT_SLOT_END + 9, 0);
            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
            {
                if (char const* slotName = sTransmogrification->GetSlotName(slot))
                {
                    Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                    uint32 entry = newItem ? sTransmogrification->GetFakeEntry(newItem) : 0;
                    std::string icon = entry ? sTransmogrification->GetItemIcon(entry, 30, 30, -18, 0) : sTransmogrification->GetSlotIcon(slot, 30, 30, -18, 0);
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, icon + std::string(slotName), EQUIPMENT_SLOT_END, slot);
                }
            }

            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_Disenchant:30:30:-18:0|tRemove all transmogrifications", EQUIPMENT_SLOT_END + 2, 0, "Remove transmogrifications from all equipped items?", 0, false);
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|tUpdate menu", EQUIPMENT_SLOT_END + 1, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
        {
            WorldSession* session = player->GetSession();
            uint32 const sender = player->PlayerTalkClass->GetGossipOptionSender(gossipListId);
            uint32 const action = player->PlayerTalkClass->GetGossipOptionAction(gossipListId);
            ClearGossipMenuFor(player);
            switch (sender)
            {
                case EQUIPMENT_SLOT_END: // Show items you can use
                    ShowTransmogItems(player, action);
                    break;
                case EQUIPMENT_SLOT_END + 1: // Main menu
                    OnGossipHello(player);
                    break;
                case EQUIPMENT_SLOT_END + 2: // Remove Transmogrifications
                {
                    bool removed = false;
                    for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                    {
                        if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                        {
                            if (!sTransmogrification->GetFakeEntry(newItem))
                                continue;
                            sTransmogrification->DeleteFakeEntry(player, newItem);
                            removed = true;
                        }
                    }
                    if (removed)
                        session->SendAreaTriggerMessage("%s", session->GetTrinityString(LANG_ERR_UNTRANSMOG_OK));
                    else
                        session->SendNotification(LANG_ERR_UNTRANSMOG_NO_TRANSMOGS);
                    OnGossipHello(player);
                    break;
                }
                case EQUIPMENT_SLOT_END + 3: // Remove Transmogrification from single item
                {
                    if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, action))
                    {
                        if (sTransmogrification->GetFakeEntry(newItem))
                        {
                            sTransmogrification->DeleteFakeEntry(player, newItem);
                            session->SendAreaTriggerMessage("%s", session->GetTrinityString(LANG_ERR_UNTRANSMOG_OK));
                        }
                        else
                            session->SendNotification(LANG_ERR_UNTRANSMOG_NO_TRANSMOGS);
                    }
                    OnGossipSelect(player, EQUIPMENT_SLOT_END, action);
                    break;
                }
                case EQUIPMENT_SLOT_END + 9: // Transmog info
                {
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", EQUIPMENT_SLOT_END + 1, 0);
                    SendGossipMenuFor(player, sTransmogrification->GetTransmogrificationInfoTextId(), me->GetGUID());
                    break;
                }
                default: // Transmogrify
                {
                    if (!sender && !action)
                    {
                        OnGossipHello(player);
                        return true;
                    }
                    // sender = slot, action = guid
                    TrinityStrings res = sTransmogrification->Transmogrify(player, ObjectGuid(HighGuid::Item, 0, action), sender);
                    if (res == LANG_ERR_TRANSMOG_OK)
                        session->SendAreaTriggerMessage("%s", session->GetTrinityString(LANG_ERR_TRANSMOG_OK));
                    else
                        session->SendNotification(res);
                    OnGossipSelect(player, EQUIPMENT_SLOT_END, sender);
                    break;
                }
            }

            return true;
        }

    private:
        void ShowTransmogItems(Player* player, uint8 slot) // Only checks bags while can use an item from anywhere in inventory
        {
            WorldSession* session = player->GetSession();
            Item* oldItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
            if (oldItem)
            {
                uint32 limit = 0;
                uint32 price = sTransmogrification->GetSpecialPrice(oldItem->GetTemplate());
                price *= sTransmogrification->GetCostModifier();
                price += sTransmogrification->GetCopperCost();

                if (player->GetSession()->HasPermission(rbac::RBAC_PERM_FREE_TRANSMOGRIFICATION))
                    price = 0;

                std::ostringstream ss;
                ss << std::endl;
                if (sTransmogrification->IsTokenRequired())
                    ss << std::endl << std::endl << sTransmogrification->GetTokenAmount() << " x " << sTransmogrification->GetItemLink(sTransmogrification->GetTokenEntry(), session);

                for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
                {
                    if (limit >= MAX_OPTIONS)
                        break;
                    Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
                    if (!newItem)
                        continue;
                    if (!sTransmogrification->CanTransmogrifyItemWithItem(player, oldItem->GetTemplate(), newItem->GetTemplate()))
                        continue;
                    if (sTransmogrification->GetFakeEntry(oldItem) == newItem->GetEntry())
                        continue;
                    ++limit;
                    AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, sTransmogrification->GetItemIcon(newItem->GetEntry(), 30, 30, -18, 0) + sTransmogrification->GetItemLink(newItem, session), slot, newItem->GetGUID().GetCounter(), "Using this item for transmogrify will bind it to you and make it non-refundable and non-tradeable.\nDo you wish to continue?\n\n" + sTransmogrification->GetItemIcon(newItem->GetEntry(), 40, 40, -15, -10) + sTransmogrification->GetItemLink(newItem, session) + ss.str(), price, false);
                }

                for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
                {
                    Bag* bag = player->GetBagByPos(i);
                    if (!bag)
                        continue;
                    for (uint32 j = 0; j < bag->GetBagSize(); ++j)
                    {
                        if (limit >= MAX_OPTIONS)
                            break;
                        Item* newItem = player->GetItemByPos(i, j);
                        if (!newItem)
                            continue;
                        if (!sTransmogrification->CanTransmogrifyItemWithItem(player, oldItem->GetTemplate(), newItem->GetTemplate()))
                            continue;
                        if (sTransmogrification->GetFakeEntry(oldItem) == newItem->GetEntry())
                            continue;
                        ++limit;
                        AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, sTransmogrification->GetItemIcon(newItem->GetEntry(), 30, 30, -18, 0) + sTransmogrification->GetItemLink(newItem, session), slot, newItem->GetGUID().GetCounter(), "Using this item for transmogrify will bind it to you and make it non-refundable and non-tradeable.\nDo you wish to continue?\n\n" + sTransmogrification->GetItemIcon(newItem->GetEntry(), 40, 40, -15, -10) + sTransmogrification->GetItemLink(newItem, session) + ss.str(), price, false);
                    }
                }
            }

            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_Disenchant:30:30:-18:0|tRemove transmogrification", EQUIPMENT_SLOT_END + 3, slot, "Remove transmogrification from the slot?", 0, false);
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|tUpdate menu", EQUIPMENT_SLOT_END, slot);
            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|tBack..", EQUIPMENT_SLOT_END + 1, 0);
            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new EG_transmogrification_creatureAI(creature);
    }
};

class EG_transmogrification_playerscript : public PlayerScript
{
public:
    EG_transmogrification_playerscript() : PlayerScript("EG_transmogrification_playerscript") { }

    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        if (player->HasTransmogrifications())
        {
            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
            {
                if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                {
                    player->SetVisibleItemSlot(slot, item);
                    if (player->IsInWorld())
                        item->SendUpdateToPlayer(player);
                }
            }
        }
    }
};

class EG_transmogrification_worldscript : public WorldScript
{
public:
    EG_transmogrification_worldscript() : WorldScript("EG_transmogrification_worldscript") { }

    void OnConfigLoad(bool reload) override
    {
        if (reload)
        {
            TC_LOG_INFO("misc", "Reloading Transmogrification config...");
            sTransmogrification->LoadConfig(reload);
        }
    }

    void OnStartup() override
    {
        TC_LOG_INFO("misc", "Deleting non-existing transmogrification entries...");
        CharacterDatabase.DirectExecute("DELETE FROM character_transmogrification WHERE NOT EXISTS (SELECT 1 FROM item_instance WHERE item_instance.guid = character_transmogrification.GUID)");
        sTransmogrification->LoadConfig(false);
        TC_LOG_INFO("misc", "Loading Transmogrification config...");
    }
};

void AddSC_EG_transmogrification()
{
    new EG_transmogrification_creature();
    new EG_transmogrification_playerscript();
    new EG_transmogrification_worldscript();
}
