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

#ifndef OBSIDIAN_SANCTUM_H_
#define OBSIDIAN_SANCTUM_H_

#include "CreatureAIImpl.h"

#define OSScriptName "instance_obsidian_sanctum"
#define DataHeader "OS"

uint32 const EncounterCount = 4;

enum OSDataTypes
{
    DATA_SARTHARION             = 0,
    DATA_TENEBRON               = 1,
    DATA_SHADRON                = 2,
    DATA_VESPERON               = 3,
    DATA_PORTAL_OPEN            = 4,
    DATA_TWILIGHT_ACHIEVEMENTS  = 5,
    DATA_GONNA_GO_WHEN_THE_VOLCANO_BLOWS = 6
};

enum OSCreaturesIds
{
    NPC_SARTHARION              = 28860,
    NPC_TENEBRON                = 30452,
    NPC_SHADRON                 = 30451,
    NPC_VESPERON                = 30449,
    NPC_LAVA_BLAZE              = 30643,
    NPC_SATH_TWILIGHT_WHELP     = 31214
};

enum OSGameObjectIds
{
    GO_TWILIGHT_PORTAL          = 193988
};

enum OSCriteriaIds
{
    CRITERIA_VOLCANO_BLOWS_10   = 7326,
    CRITERIA_VOLCANO_BLOWS_25   = 7327
};

enum OSActions
{
    ACTION_CANCEL_FREE_MOVEMENT = 1,
    ACTION_EGG_DIED
};

template <class AI, class T>
inline AI* GetObsidianSanctumAI(T* obj)
{
    return GetInstanceAI<AI>(obj, OSScriptName);
}

#define RegisterObsidianSanctumCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetObsidianSanctumAI)

#endif // OBSIDIAN_SANCTUM_H_
