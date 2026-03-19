// Copyright eike

#pragma once

#include "CoreMinimal.h"
#include "CC_BaseCharacter.h"
#include "CC_PlayCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;;

UCLASS()
class GASCOURSE_API ACC_PlayCharacter : public ACC_BaseCharacter
{
	GENERATED_BODY()

public:
	ACC_PlayCharacter();

private:
	
	UPROPERTY(VisibleAnywhere,Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;//镜头臂
	
	UPROPERTY(VisibleAnywhere,Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;//跟随摄像机
};		
