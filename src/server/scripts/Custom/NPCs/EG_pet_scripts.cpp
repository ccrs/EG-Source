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
#include "Map.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
#include "PetAI.h"

enum HandOfDrakuru
{
    SPELL_CHARM_DRAKURU_SERVANT = 52390,
    AREA_RELIQUARY_OF_PAIN = 4315
};

struct EG_npc_pet_hand_of_drakuru_petAI : public PetAI
{
    EG_npc_pet_hand_of_drakuru_petAI(Creature* creature) : PetAI(creature) { }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetMap() && me->GetMap()->GetAreaId(me->GetPhaseMask(), *me) != AREA_RELIQUARY_OF_PAIN)
            me->DespawnOrUnsummon();
        PetAI::UpdateAI(diff);
    }
};

struct EG_npc_pet_hand_of_drakuru : public ScriptedAI
{
    EG_npc_pet_hand_of_drakuru(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summonerWO) override
    {
        Unit* summoner = summonerWO->ToUnit();
        if (!summoner)
            return;

        me->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_ACTIVE);
        summoner->CastSpell(me, SPELL_CHARM_DRAKURU_SERVANT, true);
    }

    void OnCharmed(bool isNew) override
    {
        if (!me->IsCharmed())
            me->DespawnOrUnsummon();
        else
            ScriptedAI::OnCharmed(isNew);
    }

    CreatureAI* GetAIForCharm(Unit* /*who*/) override
    {
        return new EG_npc_pet_hand_of_drakuru_petAI(me);
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetMap() && me->GetMap()->GetAreaId(me->GetPhaseMask(), *me) != AREA_RELIQUARY_OF_PAIN)
            me->DespawnOrUnsummon();
        ScriptedAI::UpdateAI(diff);
    }
};

enum BlightbloodTroll
{
    SPELL_SCOURGE_SPOTLIGHT = 53104,
    NPC_TOTALLY_GENERIC_BUNNY_x80__JSB = 29100,
    SPELL_DRAKARU_DESPAWN_BLIGHTBLOOD = 61492,
    AREA_VOLTARUS = 4314
};

struct EG_npc_pet_blightblood_troll_petAI : public PetAI
{
    EG_npc_pet_blightblood_troll_petAI(Creature* creature) : PetAI(creature) { }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_DRAKARU_DESPAWN_BLIGHTBLOOD)
            me->DespawnOrUnsummon();
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetMap() && me->GetMap()->GetAreaId(me->GetPhaseMask(), *me) != AREA_VOLTARUS)
            me->DespawnOrUnsummon();
        PetAI::UpdateAI(diff);
    }
};

struct EG_npc_pet_blightblood_troll : public ScriptedAI
{
    EG_npc_pet_blightblood_troll(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* /*summoner*/) override
    {
        me->SetReactState(REACT_PASSIVE);
    }

    void OnCharmed(bool isNew) override
    {
        if (me->IsCharmed())
        {
            std::list<Creature*> triggerList;
            GetCreatureListWithOptionsInGrid(triggerList, me, 50.f, FindCreatureOptions{ .CreatureId = NPC_TOTALLY_GENERIC_BUNNY_x80__JSB, .AuraSpellId = SPELL_SCOURGE_SPOTLIGHT });
            for (Creature* trigger : triggerList)
                trigger->RemoveAurasDueToSpell(SPELL_SCOURGE_SPOTLIGHT);

            me->SetImmuneToNPC(false);
            me->SetImmuneToPC(false);
            me->SetReactState(REACT_AGGRESSIVE);
        }
        ScriptedAI::OnCharmed(isNew);
    }

    CreatureAI* GetAIForCharm(Unit* /*who*/) override
    {
        return new EG_npc_pet_blightblood_troll_petAI(me);
    }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_DRAKARU_DESPAWN_BLIGHTBLOOD)
            me->DespawnOrUnsummon();
    }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 1 && data == 1)
        {
            me->SetImmuneToNPC(false);
            me->SetImmuneToPC(false);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetMap() && me->GetMap()->GetAreaId(me->GetPhaseMask(), *me) != AREA_VOLTARUS)
            me->DespawnOrUnsummon();
        ScriptedAI::UpdateAI(diff);
    }
};

void AddSC_EG_pet_scripts()
{
    RegisterCreatureAI(EG_npc_pet_hand_of_drakuru);
    RegisterCreatureAI(EG_npc_pet_blightblood_troll);
}
