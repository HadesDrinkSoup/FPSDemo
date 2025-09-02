// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjetileActor.generated.h"

/**
 * AProjetileActor
 * 负责表现一枚可发射的子弹（Projectile）。
 * 包含碰撞、外观网格以及飞行逻辑，并提供射击与碰撞回调接口。
 */
UCLASS()
class FPSDEMO_API AProjetileActor : public AActor
{
    GENERATED_BODY()

public:
    /* 构造函数：初始化默认值，如组件、移动速度等。 */
    AProjetileActor();

    /* 球形碰撞体，用于触发命中事件以及阻挡/重叠检测。 */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class USphereComponent* CollisionComponent;

    /* 投射物移动组件，驱动子弹沿给定方向持续前进。 */
    UPROPERTY(VisibleAnywhere, Category = "Components")
    class UProjectileMovementComponent* ProjectileMovementComponent;

    /* 静态网格组件，负责子弹的外观表现。可在蓝图内替换为任意模型。 */
    UPROPERTY(EditAnywhere, Category = "Components")
    class UStaticMeshComponent* ProjectileMeshComponent;

protected:
    /* 生命周期函数：当Actor生成或关卡开始时调用。 */
    virtual void BeginPlay() override;

public:
    /* 每帧调用，用于持续逻辑（如轨迹特效、计时器等）。 */
    virtual void Tick(float DeltaTime) override;

public:
    /**
     * 向指定方向发射子弹。
     * @param ShootDirection 归一化的发射方向向量。
     */
    UFUNCTION()
    void ShootInDirection(const FVector& ShootDirection);

    /**
     * 碰撞回调：当子弹击中世界中的某个对象时触发。
     * @param HitComp      子弹自身的碰撞组件。
     * @param OtherActor   被击中的Actor。
     * @param OtherComp    被击中的组件。
     * @param NormalImpulse 碰撞冲量（用于物理模拟）。
     * @param Hit          详细的命中结果，包含位置、法线等。
     */
    UFUNCTION()
    void OnHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};