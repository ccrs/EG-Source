#ifndef DEF_TRANSMOGRIFICATION_H
#define DEF_TRANSMOGRIFICATION_H

#include "Common.h"
#include "Language.h"

#define MAX_OPTIONS 25 // do not alter

class ObjectGuid;
class Item;
class Player;
class WorldSession;
struct ItemTemplate;

class TC_GAME_API Transmogrification
{
    public:
        Transmogrification() { };
        ~Transmogrification() { };

        static Transmogrification* instance();

        bool IsAllowed(uint32 entry) const;
        bool IsNotAllowed(uint32 entry) const;
        bool IsAllowedQuality(uint32 quality) const;
        bool IsRangedWeapon(uint32 Class, uint32 SubClass) const;

        bool IsMainHandDagger(ItemTemplate const* item) const;
        bool IsTwoHandedWeapon(uint32 Class, uint32 SubClass) const;
        bool IsOneHandedWeapon(uint32 Class, uint32 SubClass) const;

        void LoadConfig(bool reload); // thread unsafe

        std::string GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y) const;
        std::string GetSlotIcon(uint8 slot, uint32 width, uint32 height, int x, int y) const;
        const char* GetSlotName(uint8 slot) const;
        std::string GetItemLink(Item* item, WorldSession* session) const;
        std::string GetItemLink(uint32 entry, WorldSession* session) const;
        uint32 GetFakeEntry(Item const* item) const;
        void UpdateItem(Player* player, Item* item) const;
        void DeleteFakeEntry(Player* player, Item* item);
        void SetFakeEntry(Player* player, Item* item, uint32 entry);

        TrinityStrings Transmogrify(Player* player, ObjectGuid itemGUID, uint8 slot, bool noCost = false);
        bool CanTransmogrifyItemWithItem(Player* player, ItemTemplate const* destination, ItemTemplate const* source) const;
        bool SuitableForTransmogrification(Player* player, ItemTemplate const* proto) const;

        uint32 GetSpecialPrice(ItemTemplate const* proto) const;
        std::vector<ObjectGuid> GetItemList(Player const* player) const;

        bool IsTransmogrificationInfoEnabled() const { return EnableTransmogInfo; }
        uint32 GetTransmogrificationInfoTextId() const { if (EnableTransmogInfo) return TransmogNpcText; return 0; }

        uint32 GetCostModifier() const { return ScaledCostModifier; }
        int32 GetCopperCost() const { return CopperCost; }

        bool IsTokenRequired() const { return RequireToken; }
        uint32 GetTokenEntry() const { return TokenEntry; }
        uint32 GetTokenAmount() const { return TokenAmount; }

    private:
        std::unordered_set<uint32> Allowed;
        std::unordered_set<uint32> NotAllowed;
        float ScaledCostModifier;
        int32 CopperCost;
        uint32 TokenEntry;
        uint32 TokenAmount;
        uint32 TransmogNpcText;
        bool EnableTransmogInfo;
        bool RequireToken;
        bool AllowPoor;
        bool AllowCommon;
        bool AllowUncommon;
        bool AllowRare;
        bool AllowEpic;
        bool AllowLegendary;
        bool AllowArtifact;
        bool AllowHeirloom;
        bool AllowMixedArmorTypes;
        bool AllowMixedWeaponTypes;
        bool AllowFishingPoles;
        bool IgnoreReqRace;
        bool IgnoreReqClass;
        bool IgnoreReqSkill;
        bool IgnoreReqSpell;
        bool IgnoreReqLevel;
        bool IgnoreReqEvent;
        bool IgnoreReqStats;
        bool DisableCost;
};

#define sTransmogrification Transmogrification::instance()

#endif // DEF_TRANSMOGRIFICATION_H
