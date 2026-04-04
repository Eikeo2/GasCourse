// Copyright eike


#include "Utils/CC_BlueprintLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/CC_AttributeSet.h"
#include "Characters/CC_BaseCharacter.h"
#include "Engine/OverlapResult.h"
#include "GameplayTags/CCTags.h"
#include "Kismet/GameplayStatics.h"


// 根据目标的前向向量和到发起者的方向确定击中方向。
// 此函数计算点积和叉积，将击中分类为前、后、左或右。
// @param TargetForward 目标Actor的前向向量。
// @param ToInstigator 从目标到发起者的标准化向量。
// @return 击中方向作为EHitDirection枚举值。
EHitDirection UCC_BlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	const float Dot = FVector::DotProduct(TargetForward, ToInstigator);
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}
	if (Dot < 0.5f)
	{
		// Either Left or Right
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		if (Cross.Z < 0.f)
		{
			return EHitDirection::Left;
		}
		return EHitDirection::Right;
	}
	return EHitDirection::Forward;
}


// 将EHitDirection枚举值转换为相应的FName，用于动画或其他系统。
// @param HitDirection 击中方向枚举值。
// @return 击中方向的名称作为FName（例如，“Left”、“Right”等）。
FName UCC_BlueprintLibrary::GetHitDirectionName(const EHitDirection& HitDirection)
{
	switch (HitDirection)
	{
	case EHitDirection::Left: return FName("Left");
	case EHitDirection::Right: return FName("Right");
	case EHitDirection::Forward: return FName("Forward");
	case EHitDirection::Back: return FName("Back");
	default: return FName("None");
	}
}


// 查找距离给定原点最近的具有特定标签的存活Actor。
// 此函数搜索所有具有给定标签的Actor，筛选有效的存活ACC_BaseCharacter实例，并返回最近的一个。
// @param WorldContextObject 用于访问游戏世界的世界上下文对象。
// @param Origin 用于测量距离的位置。
// @param Tag 在Actor上搜索的标签。
// @return 包含最近Actor及其距离的结构体。
FClosestActorWithTagResult UCC_BlueprintLibrary::FindClosestActorWithTag(const UObject* WorldContextObject,
                                                                         const FVector& Origin, const FName& Tag)
{
	//although one player ,still world
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(WorldContextObject, Tag, ActorsWithTag);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;

	for (AActor* Actor : ActorsWithTag)
	{
		if (!IsValid(Actor)) continue;
		ACC_BaseCharacter* BaseCharacter = Cast<ACC_BaseCharacter>(Actor);
		if (!IsValid(BaseCharacter) || !BaseCharacter->IsAlive()) continue;

		const float Distance = FVector::Dist(Origin, Actor->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = Actor;
		}
	}

	FClosestActorWithTagResult Result;
	Result.Actor = ClosestActor;
	Result.Distance = ClosestDistance;

	return Result;
}

// 向玩家角色发送伤害事件，应用游戏效果并触发击中反应或死亡事件。
// 此函数检查目标是否为有效的存活玩家，计算伤害是否致命，发送适当的游戏事件，
// 并将伤害效果应用到目标的能力系统组件。
// @param Target 接收伤害的Actor（预期为ACC_BaseCharacter）。
// @param DamageEffect 要应用的UGameplayEffect子类。
// @param Payload 与事件一起发送的附加游戏事件数据。
// @param DataTag 在效果规范中分配伤害大小的游戏标签。
// @param Damage 要应用的伤害量。
void UCC_BlueprintLibrary::SendDamageEventToPlayer(AActor* Target, const TSubclassOf<UGameplayEffect>& DamageEffect,
	UPARAM(ref) FGameplayEventData& Payload, const FGameplayTag& DataTag, float Damage,UObject* OptionalParticleSystem)
{
	ACC_BaseCharacter* PlayerCharacter = Cast<ACC_BaseCharacter>(Target);
	if (!IsValid(PlayerCharacter)) return;
	if (!PlayerCharacter->IsAlive()) return;

	UCC_AttributeSet* AttributeSet = Cast<UCC_AttributeSet>(PlayerCharacter->GetAttributeSet());
	if (!IsValid(AttributeSet)) return;

	const bool bLethal = AttributeSet->GetHealth() - Damage <= 0.f;
	const FGameplayTag EventTag = bLethal ? CCTags::Events::Player::Death : CCTags::Events::Player::HitReact;

	Payload.OptionalObject = OptionalParticleSystem;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter, EventTag, Payload);

	UAbilitySystemComponent* TargetASC = PlayerCharacter->GetAbilitySystemComponent();
	if (!IsValid(TargetASC)) return;

	FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
	FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffect, 1.f, ContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DataTag, -Damage);

	TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

TArray<AActor*> UCC_BlueprintLibrary::HitBoxOverlapTest(AActor* AvatarActor, float HitBoxRadius, float HitBoxForwardOffset, float HitBoxElevationOffset, bool bDrawDebugs)
{
	if (!IsValid(AvatarActor))return TArray<AActor*>();
	//初始化忽略列表：确保施法者自己不会被自己的攻击打中
	// Ensure that the overlap test ignores the Avatar Actor 
	FCollisionQueryParams QueryParams; //设置碰撞查询参数
	QueryParams.AddIgnoredActor(AvatarActor);

	//碰撞响应参数
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore); //默认忽略所有碰撞通道
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block); //仅对 Pawn 通道（通常是角色/怪物）产生阻挡/重叠响应

	//定义检测形状与结果容器
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	//计算 HitBox 的空间位置
	const FVector Forward = AvatarActor->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = AvatarActor->GetActorLocation() + Forward + FVector(
		0.f, 0.f, HitBoxElevationOffset);

	//执行物理场景重叠检测 使用可见性通道进行初步扫描
	UWorld* World=GEngine->GetWorldFromContextObject(AvatarActor,EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return TArray<AActor*>();
	World->OverlapMultiByChannel(OverlapResults, HitBoxLocation, FQuat::Identity, ECC_Visibility, Sphere,
									  QueryParams, ResponseParams);

	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		ACC_BaseCharacter* BaseCharacter = Cast<ACC_BaseCharacter>(Result.GetActor());
		if (!IsValid(BaseCharacter)) continue;
		if (!BaseCharacter->IsAlive()) continue;
		ActorsHit.AddUnique(Result.GetActor());
	}


	if (bDrawDebugs)
	{
		DrawHixBoxOverlapDebugs(AvatarActor,OverlapResults, HitBoxLocation,HitBoxRadius);
	}

	return ActorsHit;
}

void UCC_BlueprintLibrary::DrawHixBoxOverlapDebugs(const UObject* WorldContextObject,const TArray<FOverlapResult>& OverlapResults,
	const FVector& HitBoxLocation,float HitBoxRadius)
{
	UWorld* World=GEngine->GetWorldFromContextObject(WorldContextObject,EGetWorldErrorMode::LogAndReturnNull);
	if (!IsValid(World)) return ;
	//绘制红色的攻击判定球体
	DrawDebugSphere(World, HitBoxLocation, HitBoxRadius, 16, FColor::Red, false, 3.f);

	//遍历被击中的目标，在它们头顶（Z轴+100）绘制绿色小球以确认命中
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f;
			DrawDebugSphere(World, DebugLocation, 30.f, 10, FColor::Green, false, 3.f);
		}
	}
}

TArray<AActor*> UCC_BlueprintLibrary::ApplyKnockback(AActor* AvatarActor, const TArray<AActor*>& HitActors,
	float InnerRadius, float OuterRadius, float LaunchForceMagnitude, float RotationAngle, bool bDrawDebugs)
{
	for (AActor* HitActor : HitActors)
	{
		ACharacter* HitCharacter = Cast<ACharacter>(HitActor);
		if (!IsValid(HitCharacter) || !IsValid(AvatarActor)) return TArray<AActor*>();

		const FVector HitCharacterLocation = HitCharacter->GetActorLocation();
		const FVector AvatarLocation = AvatarActor->GetActorLocation();

		const FVector ToHitActor = HitCharacterLocation - AvatarLocation;
		const float Distance = FVector::Dist(AvatarLocation, HitCharacterLocation);

		float LaunchForce = 0.f;
		if (Distance > OuterRadius) continue;
		if (Distance <= InnerRadius)
		{
			LaunchForce = LaunchForceMagnitude;
		}
		else
		{
			const FVector2D FalloffRange(InnerRadius, OuterRadius); // input range
			const FVector2D LaunchForceRange(LaunchForceMagnitude, 0.f); // output range
			LaunchForce = FMath::GetMappedRangeValueClamped(FalloffRange, LaunchForceRange, Distance);
		}
		if (bDrawDebugs) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("LaunchForce: %f"), LaunchForce));

		FVector KnockbackForce = ToHitActor.GetSafeNormal();
		KnockbackForce.Z = 0.f;
		
		// 计算一个垂直于KnockbackForce的右向量，并将KnockbackForce绕这个轴旋转指定的角度，以实现更自然的击退效果。
		const FVector Right = KnockbackForce.RotateAngleAxis(90.f, FVector::UpVector);
		//逆时针旋转回来 
		KnockbackForce = KnockbackForce.RotateAngleAxis(-RotationAngle, Right) * LaunchForce;

		if (bDrawDebugs)
		{
			UWorld* World = GEngine->GetWorldFromContextObject(AvatarActor, EGetWorldErrorMode::LogAndReturnNull);
			DrawDebugDirectionalArrow(World, HitCharacterLocation, HitCharacterLocation + KnockbackForce, 100.f, FColor::Green, false, 3.f);
		}

		HitCharacter->LaunchCharacter(KnockbackForce, true, true);
	}
	return HitActors;
}
