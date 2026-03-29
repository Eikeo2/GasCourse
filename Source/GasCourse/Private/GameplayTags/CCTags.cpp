#include "GameplayTags/CCTags.h"

namespace CCTags
{
	namespace SetByCaller
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Projectile, "CCTags.SetByCaller.Projectile",
		                               "Tag for the Tertiary Tag for Set by Caller Magnitude for Projectiles");
	}

	namespace CCAbilities
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ActivateOnGiven, "CCTags.CCAbilities.ActivateOnGiven",
		                               "Tag for Abilities that should be immediately once given");

		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Primary, "CCTags.CCAbilities.Primary", "Tag for the Primary Ability");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Secondary, "CCTags.CCAbilities.Secondary", "Tag for the Secondary Ability");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(Tertiary, "CCTags.CCAbilities.Tertiary", "Tag for the Tertiary Ability");

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attack, "CCTags.CCAbilities.Enemy.Attack", "Enemy Attack Tag")
		}
	}

	namespace Events
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(KillScored, "CCTags.Events.KillScored", "Tag for the KillScored Event");

		namespace Enemy
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(HitReact, "CCTags.Events.Enemy.HitReact", "Tag for the Enemy React Event");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(EndAttack, "CCTags.Events.Enemy.EndAttack",
			                               "Tag for the Enemy Ending an Attack")
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(MeleeTraceHit, "CCTags.Events.Enemy.MeleeTraceHit",
			                               "Tag for the Enemy Melee Trace Hit")
		}
	}
}
