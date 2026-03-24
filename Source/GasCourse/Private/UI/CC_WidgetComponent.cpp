// Copyright eike


#include "UI/CC_WidgetComponent.h"
#include "AbilitySystem/CC_AbilitySystemComponent.h"
#include "Blueprint/WidgetTree.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Characters/CC_BaseCharacter.h"
#include "UI/CC_AttributeWidget.h"

void UCC_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	//能力组件在多人游戏可能需要多个帧才能初始化

	InitAbilitySystemData();

	//确认初始化属性集并且拥有第一个游戏效果
	if (!IsASCInitialized())
	{
		//if not Initialized如果 ASC 还没初始化，就先注册一个委托，添加监听，等 ASC 初始化完成的时候，请自动在this（当前类实例指针）上调用我这个类里的OnASCInitialized函数，即UCC_AttributeSet::PostGameplayEffectExecute中OnAttributesInitialized.Broadcast()广播时，OnASCInitialized函数会被调用，从而完成对 ASC 初始化完成事件的响应。
		CrashCharacter->OnASCInitialized.AddDynamic(this, &ThisClass::OnASCInitialized);
		return;
	}
	InitializeAttributeDelegate();
}

//初始化能力系统相关数据：获取角色、属性集和能力系统组件的指针
void UCC_WidgetComponent::InitAbilitySystemData()
{
	CrashCharacter = Cast<ACC_BaseCharacter>(GetOwner());
	AttributeSet = Cast<UCC_AttributeSet>(CrashCharacter->GetAttributeSet());
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(CrashCharacter->GetAbilitySystemComponent());
}

bool UCC_WidgetComponent::IsASCInitialized() const
{
	return AbilitySystemComponent.IsValid() && AttributeSet.IsValid();
}

void UCC_WidgetComponent::InitializeAttributeDelegate()
{
	if (!AttributeSet->bAttributesInitialized)
	{
		AttributeSet->OnAttributesInitialized.AddDynamic(this, &ThisClass::BindToAttributeChanges);
	}
	else
	{
		BindToAttributeChanges();
	}
}

//这个函数的作用是将一个 UWidget 对象绑定到特定的属性变化上，以便在属性值发生变化时更新 UI
void UCC_WidgetComponent::BindWidgetToAttributeChanges(UWidget* WidgetObject,
                                                       const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	UCC_AttributeWidget* AttributeWidget = Cast<UCC_AttributeWidget>(WidgetObject);
	if (!IsValid(AttributeWidget)) return; // We only care about CC Attribute Widgets
	if (!AttributeWidget->MatchesAttributes(Pair)) return; // Only subscribe for matching Attributes

	AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get()); // for initial values.

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Key).AddLambda(
		[this, AttributeWidget, &Pair](const FOnAttributeChangeData& AttributeChangeData)
		{
			AttributeWidget->OnAttributeChange(Pair, AttributeSet.Get()); // For changes during the game.
		});
}

void UCC_WidgetComponent::OnASCInitialized(UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	AbilitySystemComponent = Cast<UCC_AbilitySystemComponent>(ASC);
	AttributeSet = Cast<UCC_AttributeSet>(AS);

	if (!IsASCInitialized()) return;
	InitializeAttributeDelegate();
}

//
void UCC_WidgetComponent::BindToAttributeChanges()
{
	// TODO: Listen for changes to Gameplay Attributes and update our widgets accordingly.
	for (const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair : AttributeMap)
	{
		BindWidgetToAttributeChanges(GetUserWidgetObject(), Pair); // for checking the owned widget object.

		GetUserWidgetObject()->WidgetTree->ForEachWidget([this, &Pair](UWidget* ChildWidget)
		{
			BindWidgetToAttributeChanges(ChildWidget, Pair);
		});
	}
}
