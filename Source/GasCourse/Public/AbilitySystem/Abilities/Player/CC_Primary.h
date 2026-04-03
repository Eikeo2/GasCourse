// Copyright eike

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/CC_GameplayAbility.h"
#include "CC_Primary.generated.h"

/**
 * 
 */
UCLASS()
class GASCOURSE_API UCC_Primary : public UCC_GameplayAbility
{
	GENERATED_BODY()

public:
	//向数组actor发送HitReaction事件
	UFUNCTION(BlueprintCallable, Category = "Crash|Abilities")
	void SendHitReactEventToActors(const TArray<AActor*>& ActorsHit);

protected:
	//碰撞半径
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Crash|Abilities")
	float HitBoxRadius = 100.0f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Crash|Abilities")
	float HitBoxForwardOffset = 200.0f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Crash|Abilities")
	float HitBoxElevationOffset = 20.0f;
	 
};
