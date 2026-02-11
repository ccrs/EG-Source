#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PetAI.h"

enum HandOfDrakuru
{
    SPELL_CHARM_DRAKURU_SERVANT = 52390
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

    CreatureAI* GetAIForCharm(Creature* /*who*/) override
    {
        return new PetAI(me);
    }
};

void AddSC_EG_pet_scripts()
{
    RegisterCreatureAI(EG_npc_pet_hand_of_drakuru);
}
