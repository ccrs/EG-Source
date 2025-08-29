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

#ifndef TRINITY_BATTLEFIELD_H_
#define TRINITY_BATTLEFIELD_H_

#include "Common.h"
#include "SharedDefines.h"
#include "Timer.h"
#include "ZoneScript.h"
#include <memory>
#include <unordered_map>

enum BattlefieldSpells
{
    SPELL_BATTLEFIELD_WAITING_FOR_RESURRECT = 2584,  // Waiting to Resurrect
    SPELL_BATTLEFIELD_SPIRIT_HEAL_CHANNEL   = 22011, // Spirit Heal Channel
    SPELL_BATTLEFIELD_SPIRIT_HEAL           = 22012, // Spirit Heal
    SPELL_BATTLEFIELD_RESURRECTION_VISUAL   = 24171, // Resurrection Impact Visual
    SPELL_BATTLEFIELD_SPIRIT_HEAL_MANA      = 44535, // Spirit Heal
    SPELL_BATTLEFIELD_AURA_PLAYER_INACTIVE  = 43681  // Inactive
};

namespace WorldPackets
{
    namespace WorldState
    {
        class InitWorldStates;
    }
}

class BattlefieldGraveyard;
class GameObject;
class ObjectGuid;
class Player;
class Unit;
struct WorldSafeLocsEntry;

class TC_GAME_API Battlefield : public ZoneScript
{
public:
    typedef std::unique_ptr<BattlefieldGraveyard> BattlefieldGraveyardPointer;
    typedef std::unordered_map<uint8 /*id*/, BattlefieldGraveyardPointer> BattlefieldGraveyardContainer;

    explicit Battlefield(BattlefieldBattleId battleId, BattlefieldZoneId zoneId);
    Battlefield(Battlefield const&) = delete;
    Battlefield(Battlefield&&) = delete;

    Battlefield& operator=(Battlefield const&) = delete;
    Battlefield& operator=(Battlefield&&) = delete;

    virtual ~Battlefield();

    // Called on battlefield creation
    virtual bool SetupBattlefield();
    virtual void Update(uint32 diff);
    // Called when a player enters the battlefield zone
    virtual void HandlePlayerEnterZone(Player* player);
    // Called when a player leaves the battlefield zone
    virtual void HandlePlayerLeaveZone(Player* player);
    // Called when a player inside the battlefield zone kills a unit
    virtual void HandleKill(Player* /*killer*/, Unit* /*victim*/) { }
    // Called when a player queries a gossip from a spirit healer
    virtual void HandleAreaSpiritHealerQueryOpcode(Player* player, ObjectGuid source);
    // Called when a player moves into a resurrection queue
    virtual void HandleAddPlayerToResurrectionQueue(Player* player, ObjectGuid source);
    // Called when a player moves out of a resurrection queue
    virtual void HandleRemovePlayerFromResurrectionQueue(Player* player);
    virtual void FillInitialWorldStates(WorldPackets::WorldState::InitWorldStates& /*packet*/) { }
    virtual void SendGlobalWorldStates(Player const* /*player*/) const { }
    // Can players inside the battlefield zone use ground mounts?
    virtual bool IsMountAllowed() const { return true; }
    // Can players inside the battlefield zone use flying mounts?
    virtual bool IsFlyingMountAllowed() const { return true; }
    // Is the referenced SpellArea spellId allowed for the referenced player and newArea?
    virtual bool IsSpellAreaAllowed(uint32 /*spellId*/, Player const* /*player*/, uint32 /*newArea*/) const { return false; }

    void EmplaceGraveyard(uint8 id, BattlefieldGraveyardPointer&& pointer);
    BattlefieldGraveyardPointer& GetGraveyard(uint8 graveyardId);

    void SetMapId(uint32 mapId) { _mapId = mapId; }

    BattlefieldBattleId GetId() const { return _battleId; }
    uint32 GetZoneId() const { return _zoneId; }
    // enum PvPTeamId
    PvPTeamId GetControllingTeam() const { return _controllingTeam; }
    // enum PvPTeamId
    PvPTeamId GetAttackingTeam() const;
    // enum TeamId
    TeamId GetControllingTeamId() const;
    // enum TeamId
    TeamId GetAttackingTeamId() const;
    // Battle timer
    uint32 GetTimer() const { return _timer.GetTimer(); }
    // Closest available graveyard WorldSafeLocsEntry for Player's TeamId
    WorldSafeLocsEntry const* GetClosestGraveyardLocation(Player* player) const;
    Creature* GetCreature(ObjectGuid guid);
    GameObject* GetGameObject(ObjectGuid guid);

    bool IsEnabled() const { return _enabled; }
    bool IsWarTime() const { return _active; }

protected:
    uint32 _mapId;
    bool _enabled;
    uint32 _resurrectionBaseTimer;

private:
    // constant information
    BattlefieldBattleId _battleId;
    uint32 _zoneId;

    bool _active;
    PvPTeamId _controllingTeam;
    CountdownTimer _timer;
    BattlefieldGraveyardContainer _graveyards;
    CountdownTimer _resurrectionTimer;
};

#endif
