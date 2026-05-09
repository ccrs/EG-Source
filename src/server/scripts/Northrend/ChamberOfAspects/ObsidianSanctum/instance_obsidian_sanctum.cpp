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

#include "obsidian_sanctum.h"
#include "AreaBoundary.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "InstanceScript.h"
#include "Map.h"
#include "Player.h"
#include "ScriptMgr.h"

/* Obsidian Sanctum encounters:
0 - Sartharion
*/

BossBoundaryData const boundaries =
{
    { DATA_SARTHARION, new RectangleBoundary(3218.86f, 3275.69f, 484.68f, 572.4f) }
};

class instance_obsidian_sanctum : public InstanceMapScript
{
public:
    instance_obsidian_sanctum() : InstanceMapScript(OSScriptName, 615) { }

    struct instance_obsidian_sanctum_InstanceMapScript : public InstanceScript
    {
        instance_obsidian_sanctum_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadBossBoundaries(boundaries);
            _isPortalOpen = false;
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_SARTHARION:
                    sartharionGUID = creature->GetGUID();
                    break;
                // Three dragons below set to active state once created.
                // We must expect bigger raid to encounter main boss, and then three dragons must be active due to grid differences
                case NPC_TENEBRON:
                    tenebronGUID = creature->GetGUID();
                    creature->setActive(true);
                    creature->SetFarVisible(true);
                    break;
                case NPC_SHADRON:
                    shadronGUID = creature->GetGUID();
                    creature->setActive(true);
                    creature->SetFarVisible(true);
                    break;
                case NPC_VESPERON:
                    vesperonGUID = creature->GetGUID();
                    creature->setActive(true);
                    creature->SetFarVisible(true);
                    break;
                case NPC_LAVA_BLAZE:
                case NPC_SATH_TWILIGHT_WHELP:
                    if (Creature* sartharion = instance->GetCreature(GetGuidData(DATA_SARTHARION)))
                        sartharion->AI()->SetGUID(creature->GetGUID());
                    break;
                default:
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                 return false;

            switch (type)
            {
                case DATA_SARTHARION:
                    if (state == IN_PROGRESS)
                        _playerGUIDs.clear();
                    break;
                case DATA_TENEBRON:
                case DATA_SHADRON:
                case DATA_VESPERON:
                    break;
                default:
                    break;
            }
            return true;
        }

        ObjectGuid GetGuidData(uint32 Data) const override
        {
            switch (Data)
            {
                case DATA_SARTHARION:
                    return sartharionGUID;
                case DATA_TENEBRON:
                    return tenebronGUID;
                case DATA_SHADRON:
                    return shadronGUID;
                case DATA_VESPERON:
                    return vesperonGUID;
            }
            return ObjectGuid::Empty;
        }

        void SetData(uint32 id, uint32 value) override
        {
            if (id == DATA_PORTAL_OPEN)
                _isPortalOpen = (value != 0);
        }

        uint32 GetData(uint32 id) const override
        {
            if (id == DATA_PORTAL_OPEN)
                return _isPortalOpen ? 1 : 0;
            return 0;
        }

        void SetGuidData(uint32 id, ObjectGuid guid) override
        {
            if (id == DATA_GONNA_GO_WHEN_THE_VOLCANO_BLOWS)
                _playerGUIDs.insert(guid);
        }

        bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* player, Unit const* /* = nullptr */, uint32 /* = 0 */) override
        {
            switch (criteriaId)
            {
                case CRITERIA_VOLCANO_BLOWS_10:
                case CRITERIA_VOLCANO_BLOWS_25:
                    return _playerGUIDs.find(player->GetGUID()) == _playerGUIDs.end();
            }

            return false;
        }

    protected:
        ObjectGuid sartharionGUID;
        ObjectGuid tenebronGUID;
        ObjectGuid shadronGUID;
        ObjectGuid vesperonGUID;
        GuidUnorderedSet _playerGUIDs;
        bool _isPortalOpen;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_obsidian_sanctum_InstanceMapScript(map);
    }
};

void AddSC_instance_obsidian_sanctum()
{
    new instance_obsidian_sanctum();
}
