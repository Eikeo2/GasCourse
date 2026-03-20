// Copyright eike


#include "GasCourse/Public/Characters/CC_PlayCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/CC_PlayerState.h"


ACC_PlayCharacter::ACC_PlayCharacter()
{
	//禁用角色的Tick函数，提高性能（假设不需要每帧更新）
	PrimaryActorTick.bCanEverTick = false;

	// 初始化胶囊体碰撞组件的大小：半径42.f，高度96.f
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// 禁用控制器旋转对俯仰、偏航和滚动的控制
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 启用角色朝向移动方向旋转
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 设置旋转速率：每秒偏航轴旋转540度（用于平滑转向）
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	// 设置跳跃Z轴速度（跳跃高度）
	GetCharacterMovement()->JumpZVelocity = 500.f;
	// 设置空中控制百分比（0.35表示35%的空中转向控制）
	GetCharacterMovement()->AirControl = 0.35f;
	// 设置最大步行速度
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	// 设置最小模拟步行速度（用于手柄输入）
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	// 设置步行时的制动减速度
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	// 设置下落时的制动减速度
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

	// 创建默认子对象：弹簧臂组件，用于相机跟随
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	// 将弹簧臂附加到根组件
	CameraBoom->SetupAttachment(GetRootComponent());
	// 设置弹簧臂目标长度（相机距离）
	CameraBoom->TargetArmLength = 600.f;
	// 启用弹簧臂使用Pawn控制旋转
	CameraBoom->bUsePawnControlRotation = true;

	// 创建默认子对象：跟随相机组件
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	// 将相机附加到弹簧臂的SocketName（通常是末端）
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 禁用相机使用Pawn控制旋转（由弹簧臂处理）
	FollowCamera->bUsePawnControlRotation = false;
}

UAbilitySystemComponent* ACC_PlayCharacter::GetAbilitySystemComponent() const
{
	ACC_PlayerState* CCPlayerState = Cast<ACC_PlayerState>(GetPlayerState());
	if (!IsValid(CCPlayerState))
	{
		return nullptr;
	}

	return CCPlayerState->GetAbilitySystemComponent();
}

// Called when the character is possessed by a controller.
// Initializes the ability system component with the actor and player state information.
void ACC_PlayCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//接下来代码只会在服务器执行
	if (!IsValid(GetAbilitySystemComponent()) || !HasAuthority())return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, GetPlayerState<APlayerState>());
	GiveStatupAbilities();
}

// Replication callback called when the PlayerState is replicated.
// Ensures the ability system component is properly initialized with the new player state.
void ACC_PlayCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (!IsValid(GetAbilitySystemComponent()))return;

	GetAbilitySystemComponent()->InitAbilityActorInfo(this, GetPlayerState<APlayerState>());
}
