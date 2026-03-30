// Copyright eike


#include "GameObjects/CC_Projectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/CC_PlayCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayTags/CCTags.h"
#include "Utils/CC_BlueprintLibrary.h"

ACC_Projectile::ACC_Projectile()
{
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	bReplicates = true;
}

void ACC_Projectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACC_PlayCharacter* PlayerCharacter = Cast<ACC_PlayCharacter>(OtherActor);
	if (!IsValid(PlayerCharacter)) return;
	if (!PlayerCharacter->IsAlive()) return;
	UAbilitySystemComponent* AbilitySystemComponent = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(AbilitySystemComponent) || !HasAuthority()) return;

	// FGameplayEffectContextHandle ContextHandle =  AbilitySystemComponent->MakeEffectContext();
	// FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);
	//
	// //Use the Damage variable for the amount of damage to cause.
	// UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, CCTags::SetByCaller::Projectile, Damage);
	//
	// AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	FGameplayEventData Payload;
	Payload.Instigator = GetOwner();
	Payload.Target = PlayerCharacter;
	
	UCC_BlueprintLibrary::SendDamageEventToPlayer(PlayerCharacter, DamageEffect, Payload, CCTags::SetByCaller::Projectile, Damage);
	
	SpawnImpactEffects();
	Destroy();
}