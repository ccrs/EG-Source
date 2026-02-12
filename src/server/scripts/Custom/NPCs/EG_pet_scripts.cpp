#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellInfo.h"
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

    CreatureAI* GetAIForCharm(Unit* /*who*/) override
    {
        return new PetAI(me);
    }
};

enum BlightbloodTroll
{
    SPELL_SCOURGE_SPOTLIGHT = 53104,
    NPC_TOTALLY_GENERIC_BUNNY_x80__JSB = 29100,
    SPELL_DRAKARU_DESPAWN_BLIGHTBLOOD = 61492
};

struct EG_npc_pet_blightblood_troll_petAI : public PetAI
{
    EG_npc_pet_blightblood_troll_petAI(Creature* creature) : PetAI(creature) { }

    void SpellHit(WorldObject* /*caster*/, SpellInfo const* spellInfo) override
    {
        if (spellInfo->Id == SPELL_DRAKARU_DESPAWN_BLIGHTBLOOD)
            me->DespawnOrUnsummon();
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
};

void AddSC_EG_pet_scripts()
{
    RegisterCreatureAI(EG_npc_pet_hand_of_drakuru);
    RegisterCreatureAI(EG_npc_pet_blightblood_troll);
}
