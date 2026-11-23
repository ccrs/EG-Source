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

#ifndef DEF_ARCHAVON_H
#define DEF_ARCHAVON_H

#include "CreatureAIImpl.h"

inline constexpr char const* VaultOfArchavonScriptName = "instance_vault_of_archavon";
inline constexpr char const* DataHeader = "VA";
inline constexpr uint32 EncounterCount = 4;

enum VaultOfArchavonMisc
{
    DATA_ARCHAVON = 0,
    DATA_EMALON,
    DATA_KORALON,
    DATA_TORAVON,

    NPC_ARCHAVON = 31125,
    NPC_EMALON = 33993,
    NPC_KORALON = 35013,
    NPC_TORAVON = 38433,

    CRITERIA_EARTH_WIND_FIRE_10 = 12018,
    CRITERIA_EARTH_WIND_FIRE_25 = 12019,

    SPELL_EARTH_WIND_FIRE_ACHIEVEMENT_CHECK = 68308
};

template <class AI, class T>
inline AI* GetVaultOfArchavonAI(T* obj)
{
    return GetInstanceAI<AI>(obj, VaultOfArchavonScriptName);
}

#define RegisterVaultOfArchavonCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetVaultOfArchavonAI)

#endif
