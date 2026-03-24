// Copyright eike


#include "UI/CC_AttributeWidget.h"

void UCC_AttributeWidget::OnAttributeChange(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair,
                                            UCC_AttributeSet* AttributeSet)
{
	//它会接收两个属性（通过 Pair 传递），分别是当前值和最大值
	//它通过 GetNumericValue 方法从 AttributeSet 中获取当前属性的数值（如血量、最大血量等）
	const float AttributeValue = Pair.Key.GetNumericValue(AttributeSet);
	const float MaxAttributeValue = Pair.Value.GetNumericValue(AttributeSet);
	//然后它调用一个蓝图事件 BP_OnAttributeChange，并将这些值传递给蓝图，通常用于 UI 更新（例如显示当前血量和最大血量）
	BP_OnAttributeChange(AttributeValue, MaxAttributeValue);
}

bool UCC_AttributeWidget::MatchesAttributes(const TTuple<FGameplayAttribute, FGameplayAttribute>& Pair) const
{
	return Attribute == Pair.Key && MaxAttribute == Pair.Value;
}
