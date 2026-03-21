// Copyright eike


#include "Characters/CC_EnemyCharacter.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"

ACC_EnemyCharacter::ACC_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UCC_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	//UE_LOG(LogTemp,Warning,TEXT("Enemy Character Constructed"))
}

UAbilitySystemComponent* ACC_EnemyCharacter::GetAbilitySystemComponent() const
{
	//UE_LOG(LogTemp,Warning,TEXT("Enemy Character GetAbilitySystemComponent"));
	return AbilitySystemComponent;
}

void ACC_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetAbilitySystemComponent()))return;

	//enemy both self
	GetAbilitySystemComponent()->InitAbilityActorInfo(this, this);

	if (!HasAuthority())return;

	GiveStatupAbilities();
	//UE_LOG(LogTemp,Warning,TEXT("Enemy Character GiveStatupAbilities"));
}
