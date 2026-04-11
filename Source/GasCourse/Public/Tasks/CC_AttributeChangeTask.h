// Copyright eike

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Kismet/BlueprintAsyncActionBase.h"// 蓝图异步节点基类
#include "CC_AttributeChangeTask.generated.h"

struct FOnAttributeChangeData;
class UAbilitySystemComponent;

// 蓝图可绑定的动态多播委托：当某个属性变化时广播
// 参数：变化的属性 / 新值 / 旧值
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, FGameplayAttribute, Attribute, float, NewValue,
                                               float, OldValue);
// 异步蓝图代理对象：用于监听 ASC 指定属性变化，并把变化事件抛给蓝图
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class GASCOURSE_API UCC_AttributeChangeTask : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// 蓝图可绑定事件：属性每次变化都会触发
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnAttributeChanged;
	
	// 蓝图异步节点入口（工厂函数）
	// AbilitySystemComponent: 要监听的 ASC
	// Attribute: 要监听的具体属性（例如 Health/Mana）
	// 返回值：创建出的异步任务对象（无效输入时可返回 nullptr）
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UCC_AttributeChangeTask* ListenForAttributeChange(UAbilitySystemComponent* AbilitySystemComponent,
	                                                         FGameplayAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	
	// 主动结束任务：通常用于解绑委托、停止继续监听，需自己调用
	void EndTask();

	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	FGameplayAttribute AttributeToListenFor;

	// ASC 属性变化时的内部回调：接收数据后通常转发到 OnAttributeChanged
	void AttributeChanged(const FOnAttributeChangeData& Data);
};
