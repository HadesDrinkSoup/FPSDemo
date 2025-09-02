// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSProjetile/ProjetileActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// ------------------------------------------------------------------
// 构造函数：初始化默认组件与属性
// ------------------------------------------------------------------
AProjetileActor::AProjetileActor()
{
    // 允许 Tick 函数每帧调用；若不需要可关闭以提升性能
    PrimaryActorTick.bCanEverTick = true;

    /* ---------------- 碰撞组件 ---------------- */
    if (!CollisionComponent)
    {
        // 创建球形碰撞体，命名为 "SphereComponent"
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

        // 使用 "Projectile" 碰撞预设（通常对可见物开启碰撞，对摄像机忽略）
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

        // 绑定命中事件，击中物体时触发 OnHit 函数
        CollisionComponent->OnComponentHit.AddDynamic(this, &AProjetileActor::OnHit);

        // 设置球体半径
        CollisionComponent->InitSphereRadius(15.0f);

        // 将球体设为根组件，确保所有其他组件跟随其变换
        RootComponent = CollisionComponent;
    }

    /* ---------------- 移动组件 ---------------- */
    if (!ProjectileMovementComponent)
    {
        // 创建并命名投射物移动组件
        ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

        // 指定移动组件所更新的根组件（即碰撞体）
        ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);

        // 初始速度/最大速度：3000 cm/s（约 30 m/s）
        ProjectileMovementComponent->InitialSpeed = 3000.0f;
        ProjectileMovementComponent->MaxSpeed = 3000.0f;

        // 使子弹朝向跟随速度方向（视觉上更自然）
        ProjectileMovementComponent->bRotationFollowsVelocity = true;

        // 允许子弹反弹
        ProjectileMovementComponent->bShouldBounce = true;
        ProjectileMovementComponent->Bounciness = 0.3f;

        // 关闭重力（0 表示不受重力影响）
        ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    }

    /* ---------------- 网格组件 ---------------- */
    ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));

    // 网格默认缩放：略小于单位大小（9%）
    ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));

    // 将网格附加到根组件（碰撞球体）
    ProjectileMeshComponent->SetupAttachment(RootComponent);

    // 生命周期：子弹生成 3 秒后自动销毁，防止残留
    InitialLifeSpan = 3.0f;
}

// ------------------------------------------------------------------
// 生命周期：关卡开始或生成时调用
// ------------------------------------------------------------------
void AProjetileActor::BeginPlay()
{
    Super::BeginPlay();
}

// ------------------------------------------------------------------
// 每帧调用，可在此实现持续逻辑（尾迹、计时器等）
// ------------------------------------------------------------------
void AProjetileActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// ------------------------------------------------------------------
// 发射接口：根据给定方向设置速度
// @param ShootDirection 归一化的方向向量
// ------------------------------------------------------------------
void AProjetileActor::ShootInDirection(const FVector& ShootDirection)
{
    ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

// ------------------------------------------------------------------
// 碰撞回调：处理命中逻辑
// @param HitComp      子弹自身的碰撞组件
// @param OtherActor   被击中的 Actor
// @param OtherComp    被击中的组件
// @param NormalImpulse 碰撞冲量（物理模拟用）
// @param Hit          命中结果（位置、法线、表面材质等）
// ------------------------------------------------------------------
void AProjetileActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    // 避免自身碰撞；若被击中组件具有物理模拟，则施加冲击力
    if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
    {
        // 使用当前速度向量作为冲量方向，乘以 100 作为力度
        OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
    }

    // 命中后无论是否造成伤害，均立即销毁子弹
    Destroy();
}