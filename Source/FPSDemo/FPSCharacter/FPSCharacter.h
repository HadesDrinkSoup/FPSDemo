// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputLibrary.h" // 引入增强输入系统功能
#include "Camera/CameraComponent.h" // 引入相机组件
#include "FPSCharacter.generated.h" // 包含由UHT生成的代码

// 前向声明（在TSubclassOf中使用）
class AProjetileActor;

/**
 * 第一人称视角（FPS）游戏角色类。
 * 该类继承自ACharacter，实现了基本的移动、视角控制、跳跃和射击功能。
 * 使用Enhanced Input系统进行输入处理。
 */
UCLASS()
class FPSDEMO_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// 构造函数，设置默认属性值
	AFPSCharacter();

	// 第一人称相机组件，仅玩家自己可见
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* FPSCameraComponent;

	// 第一人称视角下可见的骨骼网格体（例如持枪的手臂）
	UPROPERTY(VisibleAnywhere, Category = "FPSMesh")
	USkeletalMeshComponent* FPSMesh;

private:
	// 射击速率（每秒发射的子弹数）
	UPROPERTY(EditAnywhere, Category = "Shoot")
	float FireRate;

	// 记录上一次射击的时间，用于控制射击间隔
	UPROPERTY(EditAnywhere, Category = "Shoot")
	float LastFireTime;

	// 从相机/角色位置发射子弹的偏移量，用于调整生成位置
	UPROPERTY(EditAnywhere, Category = "Projectile")
	FVector MuzzleOffset;

	// 要生成的子弹类（使用TSubclassOf确保类型安全）
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<class AProjetileActor> ProjectileClass;

protected:
	// 游戏开始或角色生成时调用
	virtual void BeginPlay() override;

	// 默认输入映射上下文，用于将输入动作绑定到具体的按键/操作
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	// 移动输入动作（通常绑定到WASD或摇杆）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;

	// 视角转动输入动作（通常绑定到鼠标移动或右摇杆）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	// 跳跃输入动作
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;

	// 射击输入动作（通常绑定到鼠标左键或手柄扳机键）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ShootAction;

public:
	// 每帧调用
	virtual void Tick(float DeltaTime) override;

	// 设置玩家输入组件，绑定输入动作到对应的处理函数
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 处理移动输入的回调函数
	void Move(const FInputActionValue& Value);

	// 处理视角转动输入的回调函数
	void Look(const FInputActionValue& Value);

	// 处理开始跳跃输入的回调函数
	void StartJump(const FInputActionValue& Value);

	// 处理停止跳跃输入的回调函数
	void StopJump(const FInputActionValue& Value);

	// 处理射击输入的回调函数
	void Shoot(const FInputActionValue& Value);
};