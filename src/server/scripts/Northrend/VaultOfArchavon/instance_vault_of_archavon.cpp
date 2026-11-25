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

#include "vault_of_archavon.h"
#include "Battlefield/BattlefieldWG.h"
#include "BattlefieldMgr.h"
#include "GameTime.h"
#include "InstanceScript.h"
#include "Player.h"
#include "ScriptMgr.h"
#include <algorithm>
#include <array>

ObjectData const creatureData[] =
{
    { NPC_ARCHAVON, DATA_ARCHAVON },
    { NPC_EMALON, DATA_EMALON },
    { NPC_KORALON, DATA_KORALON },
    { NPC_TORAVON, DATA_TORAVON },
    { 0, 0 }
};

class instance_vault_of_archavon : public InstanceMapScript
{
public:
    instance_vault_of_archavon() : InstanceMapScript(VaultOfArchavonScriptName, 624) { }

    struct instance_vault_of_archavon_InstanceMapScript : public InstanceScript
    {
        instance_vault_of_archavon_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadObjectData(creatureData, nullptr);
        }

        void OnPlayerEnter(Player* player) override
        {
            if (player->IsGameMaster())
                return;

            BattlefieldWintergrasp* wintergrasp = dynamic_cast<BattlefieldWintergrasp*>(sBattlefieldMgr->GetBattlefield(BATTLEFIELD_BATTLEID_WINTERGRASP));
            if (wintergrasp && wintergrasp->IsEnabled() && (wintergrasp->GetPhase() != BATTLEFIELD_PHASE_IDLE || player->GetTeamId() != wintergrasp->GetControllingTeamId()))
                wintergrasp->TeleportPlayerToBattle(player);
        }

        bool SetBossState(uint32 type, EncounterState state) override
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            if (state == DONE && type != DATA_TORAVON)
            {
                _watcherDeathTimes[type] = GameTime::Now();
                DoCastSpellOnPlayers(SPELL_EARTH_WIND_FIRE_ACHIEVEMENT_CHECK);
            }

            return true;
        }

        bool CheckAchievementCriteriaMeet(uint32 criteriaId, Player const* source, Unit const* target, uint32 miscValue1) override
        {
            switch (criteriaId)
            {
                case CRITERIA_EARTH_WIND_FIRE_10:
                case CRITERIA_EARTH_WIND_FIRE_25:
                {
                    if (std::ranges::find(_watcherDeathTimes, TimePoint()) != _watcherDeathTimes.end())
                        return false;

                    auto const [firstDeath, lastDeath] = std::ranges::minmax(_watcherDeathTimes);
                    return lastDeath - firstDeath <= 1min;
                }
                default:
                    return InstanceScript::CheckAchievementCriteriaMeet(criteriaId, source, target, miscValue1);
            }
        }

    private:
        std::array<TimePoint, 3> _watcherDeathTimes = { };
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_vault_of_archavon_InstanceMapScript(map);
    }
};

void AddSC_instance_vault_of_archavon()
{
    new instance_vault_of_archavon();
}
