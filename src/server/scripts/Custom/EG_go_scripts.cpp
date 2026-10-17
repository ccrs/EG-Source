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

#include "ScriptMgr.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "InstanceScript.h"

enum NaxxramasOrbMisc
{
    DATA_ALL_QUARTERS_DONE = 9 // NAXData in naxxramas.h
};

// 202278 - Orb of Naxxramas
struct EG_go_naxxramas_orb_frostwyrm_lair : public GameObjectAI
{
    EG_go_naxxramas_orb_frostwyrm_lair(GameObject* go) : GameObjectAI(go), _instance(go->GetInstanceScript()) { }

    bool OnGossipHello(Player* /*player*/) override
    {
        return _instance && !_instance->GetData(DATA_ALL_QUARTERS_DONE);
    }

private:
    InstanceScript* _instance;
};

void AddSC_EG_go_scripts()
{
    RegisterGameObjectAI(EG_go_naxxramas_orb_frostwyrm_lair);
}
