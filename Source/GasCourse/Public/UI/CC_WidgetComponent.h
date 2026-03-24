// Copyright eike

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AttributeSet.h"

#include "CC_WidgetComponent.generated.h"

class UAbilitySystemComponent;
class UCC_AbilitySystemComponent;
class ACC_BaseCharacter;
class UCC_AttributeSet;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GASCOURSE_API UCC_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayAttribute, FGameplayAttribute> AttributeMap;

private:
	//weak ptr 弱引用意味着TWeakObjectPtr不会阻止其引用的UObject被GC销毁，因此在使用之前需要检查它是否仍然有效。这对于避免悬挂指针和内存泄漏非常有用，特别是在复杂的对象关系中。
	TWeakObjectPtr<ACC_BaseCharacter> CrashCharacter;
	TWeakObjectPtr<UCC_AbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UCC_AttributeSet> AttributeSet;

	void InitAbilitySystemData();
	bool IsASCInitialized() const;
	void InitializeAttributeDelegate();
	void BindWidgetToAttributeChanges(UWidget* WidgetObject,
	                                  const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const;

	UFUNCTION()
	void OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS);

	UFUNCTION()
	void BindToAttributeChanges();
};
