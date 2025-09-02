// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter/FPSCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "FPSProjetile/ProjetileActor.h" // 注意：文件名可能有拼写错误 (Projectile)

// 设置默认值
AFPSCharacter::AFPSCharacter()
{
	// 设置此角色每帧调用Tick()。如果不需要，可以关闭以提高性能。
	PrimaryActorTick.bCanEverTick = true;

	// 创建并初始化第一人称相机组件
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr); // 确保相机创建成功
	// 将相机附加到角色的胶囊体组件上
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	// 设置相机相对于胶囊体的位置（略微高于基础眼睛高度）
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, BaseEyeHeight + 20.0f));
	// 允许相机随控制器旋转
	FPSCameraComponent->bUsePawnControlRotation = true;

	// 创建并初始化第一人称网格（手臂/武器）
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);
	// 仅对角色所有者可见（第一人称视角）
	FPSMesh->SetOnlyOwnerSee(true);
	// 将网格附加到相机上，使其随相机移动
	FPSMesh->SetupAttachment(FPSCameraComponent);
	// 禁用动态阴影投射以提高性能
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	// 设置第三人称网格对所有者不可见（避免穿模）
	GetMesh()->SetOwnerNoSee(true);

	// 设置最大跳跃次数（2表示允许二段跳）
	JumpMaxCount = 2;
	// 设置射击速率（每秒2发）
	FireRate = 2.0f;
	// 初始化上次射击时间为0
	LastFireTime = 0.0f;
}

// 游戏开始或角色生成时调用
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 在屏幕上显示调试消息5秒
	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hello Unreal, this message come FPSCharacter"));
}

// 每帧调用
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// 绑定功能到输入
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 获取玩家控制器并添加强制输入映射上下文
	if (APlayerController* PC = CastChecked<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// 将输入动作绑定到对应的处理函数
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 移动：持续触发
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);
		// 视角：持续触发
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);
		// 跳跃：按下开始，释放结束
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AFPSCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopJump);
		// 射击：持续触发（按住可连续射击）
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Shoot);
	}
}

// 处理移动输入
void AFPSCharacter::Move(const FInputActionValue& Value)
{
	// 获取二维移动向量（X: 左右, Y: 前后）
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 获取控制器的偏航旋转（忽略俯仰和滚动）
		const FRotator YawRotation(0, Controller->GetControlRotation().Yaw, 0);

		// 计算前进方向（X轴）
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// 应用前后移动
		AddMovementInput(ForwardDirection, MovementVector.Y);

		// 计算右方向（Y轴）
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// 应用左右移动
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

// 处理视角转动输入
void AFPSCharacter::Look(const FInputActionValue& Value)
{
	// 获取二维视角转动向量（X: 水平, Y: 垂直）
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 添加偏航（左右看）输入
		AddControllerYawInput(LookAxisVector.X);
		// 添加俯仰（上下看）输入（取反因为屏幕坐标与游戏世界坐标方向相反）
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

// 处理开始跳跃输入
void AFPSCharacter::StartJump(const FInputActionValue& Value)
{
	// 调用父类的Jump方法
	Jump();
}

// 处理停止跳跃输入
void AFPSCharacter::StopJump(const FInputActionValue& Value)
{
	// 调用父类的StopJumping方法
	StopJumping();
}

// 处理射击输入
void AFPSCharacter::Shoot(const FInputActionValue& Value)
{
	// 检查开火冷却时间
	const float CurrentTime = GetWorld()->GetTimeSeconds();
	//每次开火至少需要等待的秒数
	float TimeBetweenShots = 1.0f / FireRate;   // FireRate = 2 ⇒ 0.5 s
	// 下一次允许开火的时间
	float NextFireTime = LastFireTime + TimeBetweenShots;
	if (CurrentTime < NextFireTime)
	{
		return; // 如果未达到开火间隔，直接返回
	}

	// 更新上次开火时间
	LastFireTime = CurrentTime;

	// 检查子弹类是否有效
	if (ProjectileClass)
	{
		// 获取摄像机位置和旋转
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// 设置枪口偏移量（在摄像机前方100单位）
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		// 将偏移量从本地空间转换到世界空间
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// 使用摄像机旋转作为子弹初始旋转
		FRotator MuzzleRotation = CameraRotation;
		// 可以在此处添加额外的俯仰调整（已注释掉）
		// MuzzleRotation.Pitch += 10.0f;

		// 获取世界上下文
		UWorld* World = GetWorld();
		if (World)
		{
			// 设置生成参数
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;        // 设置子弹所有者
			SpawnParams.Instigator = GetInstigator(); // 设置发起者

			// 在枪口位置生成子弹
			AProjetileActor* Projectile = World->SpawnActor<AProjetileActor>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

			if (Projectile)
			{
				// 设置子弹的发射方向
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->ShootInDirection(LaunchDirection);
			}
		}
	}
}