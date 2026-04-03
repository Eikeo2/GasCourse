// Copyright eike


#include "AbilitySystem/Abilities/Player/CC_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"


void UCC_Primary::SendHitReactEventToActors(const TArray<AActor*>& ActorsHit)
{
	for (AActor* HitActor : ActorsHit)
	{
		//发送给被重叠检测中的
		FGameplayEventData Playload;
		Playload.Instigator = GetAvatarActorFromActorInfo();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor,
		                                                         FGameplayTag::RequestGameplayTag(
			                                                         FName("CCTags.Events.Enemy.HitReact")), Playload);
	}
}

