#include "Playerbots.h"
#include "OldKingdomActions.h"
#include "OldKingdomStrategy.h"


bool AttackNadoxGuardianAction::Execute(Event event)
{
    Unit* target = AI_VALUE2(Unit*, "find target", "ahn'kahar guardian");
    if (!target || AI_VALUE(Unit*, "current target") == target)
    {
        return false;
    }
    
    return Attack(target);
}

bool AttackJedogaVolunteerAction::Execute(Event event)
{
    Unit* target = nullptr;
    // Target is not findable from threat table using AI_VALUE2(),
    // therefore need to search manually for the unit name
    GuidVector targets = AI_VALUE(GuidVector, "possible targets no los");

    for (auto i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = botAI->GetUnit(*i);
        if (unit && unit->GetEntry() == NPC_TWILIGHT_VOLUNTEER)
        {
            target = unit;
            break;
        }
    }

    if (!target || AI_VALUE(Unit*, "current target") == target)
    {
        return false;
    }
    return Attack(target);
}

bool AvoidShadowCrashAction::Execute(Event event)
{
    // Could check all enemy units in range as it's possible to pull multiple of these mobs.
    // They should really be killed 1 by 1, multipulls are messy so we just handle singles for now
    Unit* npc = AI_VALUE2(Unit*, "find target", "forgotten one");
    Unit* victim = nullptr;
    float radius = 10.0f;
    float targetDist = radius + 2.0f;
    if (!npc) { return false; }

    // Actively move if targeted by a shadow crash.
    // Spell check not needed, they don't have any other non-instant casts
    if (npc->HasUnitState(UNIT_STATE_CASTING)) // && npc->FindCurrentSpellBySpellId(SPELL_SHADOW_CRASH))
    {
        // This doesn't seem to avoid casts very well, perhaps because this isn't checked while allies are casting.
        // TODO: Revisit if this is an issue in heroics, otherwise ignore shadow crashes for the most part.
        victim = botAI->GetUnit(npc->GetTarget());
        if (victim && bot->GetExactDist2d(victim) < radius)
        {
            return MoveAway(victim, targetDist - bot->GetExactDist2d(victim));
        }
    }

    // Otherwise ranged members passively spread, to avoid AoE overlap
    if (botAI->IsMelee(bot)) { return false; }

    GuidVector members = AI_VALUE(GuidVector, "group members");
    for (auto& member : members)
    {
        if (bot->GetGUID() == member)
        {
            continue;
        }
        float currentDist = bot->GetExactDist2d(botAI->GetUnit(member));
        if (currentDist < radius)
        {
            return MoveAway(botAI->GetUnit(member), targetDist - currentDist);
        }
    }
    return false;
}