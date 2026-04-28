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

#include "old_hillsbrad.h"
#include "Creature.h"
#include "GameObject.h"
#include "InstanceScript.h"
#include "Log.h"
#include "Map.h"
#include "Player.h"
#include "ScriptMgr.h"

enum OHInstanceMisc
{
    NPC_THRALL = 17876,
    NPC_TARETHA = 18887,
    NPC_EPOCH_HUNTER = 18096,
    NPC_DRAKE = 17848,
    LODGE_QUEST_TRIGGER = 20155,

    GO_ROARING_FLAME = 182592,

    EVENT_BARREL_FIRE = 11111
};

// Maps each barrel's DB spawn GUID to its paired Roaring Flame DB spawn GUID.
static std::unordered_map<uint32, uint32> const BarrelFlameGuids =
{
    { 30242, 30289 },
    { 30243, 30297 },
    { 30244, 30299 },
    { 30263, 30290 },
    { 30286, 30292 },
    { 33565, 30305 },
    { 33567, 30300 },
    { 34868, 30296 }
};

class instance_old_hillsbrad : public InstanceMapScript
{
public:
    instance_old_hillsbrad() : InstanceMapScript(OHScriptName, 560) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_old_hillsbrad_InstanceMapScript(map);
    }

    struct instance_old_hillsbrad_InstanceMapScript : public InstanceScript
    {
        instance_old_hillsbrad_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(OldHillsbradFoothillsBossCount);

            ThrallEscortState = OH_ESCORT_PRISON_TO_SKARLOC;
            BarrelCount = 0;
        }

        OHThrallEscortStates ThrallEscortState;
        uint32 BarrelCount;

        ObjectGuid ThrallGUID;
        ObjectGuid TarethaGUID;
        ObjectGuid EpochGUID;

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_THRALL:
                    ThrallGUID = creature->GetGUID();
                    break;
                case NPC_TARETHA:
                    TarethaGUID = creature->GetGUID();
                    break;
                case NPC_EPOCH_HUNTER:
                    EpochGUID = creature->GetGUID();
                    break;
            }
        }

        void ProcessEvent(WorldObject* obj, uint32 eventId) override
        {
            if (eventId != EVENT_BARREL_FIRE || !obj || !obj->IsGameObject())
                return;

            auto it = BarrelFlameGuids.find(obj->ToGameObject()->GetSpawnId());
            if (it == BarrelFlameGuids.end())
                return;

            if (GameObject* flame = instance->GetGameObjectBySpawnId(it->second))
                flame->SetRespawnTime(6 * DAY);

            SetData(TYPE_BARREL_DIVERSION, IN_PROGRESS);
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case TYPE_BARREL_DIVERSION:
                {
                    if (data != IN_PROGRESS || BarrelCount >= 5)
                        return;

                    ++BarrelCount;
                    DoUpdateWorldState(WORLD_STATE_OH, BarrelCount);

                    TC_LOG_DEBUG("scripts", "Instance Old Hillsbrad: barrel count {}", BarrelCount);

                    if (BarrelCount == 5)
                    {
                        // Give all players quest credit for the diversion
                        Map::PlayerList const& players = instance->GetPlayers();
                        for (auto const& ref : players)
                            if (Player* player = ref.GetSource())
                                player->KilledMonsterCredit(LODGE_QUEST_TRIGGER);

                        instance->SummonCreature(NPC_DRAKE, { 2128.43f, 71.01f, 64.42f, 1.74f }, nullptr, 0);
                    }
                    break;
                }
                case TYPE_THRALL_EVENT:
                {
                    if (data == OH_ESCORT_DEATH_EVENT)
                    {
                        // Always reset to beginning; the escort recovers naturally via boss state checks
                        ThrallEscortState = OH_ESCORT_PRISON_TO_SKARLOC;
                        TC_LOG_DEBUG("scripts", "Instance Old Hillsbrad: Thrall escort reset after wipe.");
                        return;
                    }

                    ThrallEscortState = OHThrallEscortStates(data);

                    // Only set Thrall's gossip flag for states where the player must interact with him
                    if (data == OH_ESCORT_HORSE_RIDE || data == OH_ESCORT_BARN_TO_TARETHA)
                    {
                        if (Creature* thrall = instance->GetCreature(ThrallGUID))
                            thrall->SetNpcFlag(UNIT_NPC_FLAG_GOSSIP);
                    }

                    TC_LOG_DEBUG("scripts", "Instance Old Hillsbrad: Thrall escort state set to {}.", data);
                    break;
                }
            }
        }

        uint32 GetData(uint32 data) const override
        {
            switch (data)
            {
                case TYPE_BARREL_DIVERSION:
                    if (BarrelCount == 0)
                        return NOT_STARTED;
                    return BarrelCount >= 5 ? DONE : IN_PROGRESS;
                case TYPE_THRALL_EVENT:
                    return ThrallEscortState;
            }
            return 0;
        }

        ObjectGuid GetGuidData(uint32 data) const override
        {
            switch (data)
            {
                case DATA_THRALL:
                    return ThrallGUID;
                case DATA_TARETHA:
                    return TarethaGUID;
                case DATA_EPOCH_HUNTER:
                    return EpochGUID;
            }
            return ObjectGuid::Empty;
        }

        void WriteSaveDataMore(std::ostringstream& data) override
        {
            data << ThrallEscortState << ' ' << BarrelCount;
        }

        void ReadSaveDataMore(std::istringstream& data) override
        {
            uint32 escortState = 0;
            uint32 barrelCount = 0;
            data >> escortState >> barrelCount;

            ThrallEscortState = OHThrallEscortStates(std::min(escortState, uint32(OH_ESCORT_FINISHED)));
            BarrelCount = std::min(barrelCount, 5u);

            // Temp summons don't persist across reloads. Re-summon Drake if barrels
            // were completed but he was never killed, otherwise players are softlocked
            // (all barrels locked via GetData == DONE, Drake gone, Thrall unreachable).
            if (BarrelCount >= 5 && GetBossState(DATA_LIEUTENANT_DRAKE) != DONE)
                instance->SummonCreature(NPC_DRAKE, { 2128.43f, 71.01f, 64.42f, 1.74f }, nullptr, 0);
        }
    };
};

void AddSC_instance_old_hillsbrad()
{
    new instance_old_hillsbrad();
}
