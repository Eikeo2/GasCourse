// Copyright eike


#include "AbilitySystem/Abilities/Player/CC_Primary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/OverlapResult.h"


TArray<AActor*> UCC_Primary::HitBoxOverlapTest()
{
	//初始化忽略列表：确保施法者自己不会被自己的攻击打中
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	// Ensure that the overlap test ignores the Avatar Actor 
	FCollisionQueryParams QueryParams; //设置碰撞查询参数
	QueryParams.AddIgnoredActors(ActorsToIgnore);

	//碰撞响应参数
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore); //默认忽略所有碰撞通道
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block); //仅对 Pawn 通道（通常是角色/怪物）产生阻挡/重叠响应

	//定义检测形状与结果容器
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(HitBoxRadius);

	//计算 HitBox 的空间位置
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitBoxForwardOffset;
	const FVector HitBoxLocation = GetAvatarActorFromActorInfo()->GetActorLocation() + Forward + FVector(
		0.f, 0.f, HitBoxElevationOffset);

	//执行物理场景重叠检测 使用可见性通道进行初步扫描
	GetWorld()->OverlapMultiByChannel(OverlapResults, HitBoxLocation, FQuat::Identity, ECC_Visibility, Sphere,
	                                  QueryParams, ResponseParams);

	TArray<AActor*> ActorsHit;
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (!IsValid(Result.GetActor())) continue;
		ActorsHit.AddUnique(Result.GetActor());
	}


	if (bDrawDebugs)
	{
		DrawHixBoxOverlapDebugs(OverlapResults, HitBoxLocation);
	}

	return ActorsHit;
}

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

void UCC_Primary::DrawHixBoxOverlapDebugs(const TArray<FOverlapResult>& OverlapResults,
                                          const FVector& HitBoxLocation) const
{
	//绘制红色的攻击判定球体
	DrawDebugSphere(GetWorld(), HitBoxLocation, HitBoxRadius, 16, FColor::Red, false, 3.f);

	//遍历被击中的目标，在它们头顶（Z轴+100）绘制绿色小球以确认命中
	for (const FOverlapResult& Result : OverlapResults)
	{
		if (IsValid(Result.GetActor()))
		{
			FVector DebugLocation = Result.GetActor()->GetActorLocation();
			DebugLocation.Z += 100.f;
			DrawDebugSphere(GetWorld(), DebugLocation, 30.f, 10, FColor::Green, false, 3.f);
		}
	}
}
