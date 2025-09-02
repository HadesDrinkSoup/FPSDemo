// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSHUD.generated.h"

/**
 * AFPSHUD
 * 项目的主 HUD（Head-Up Display）类。
 * 用于在屏幕上绘制第一人称射击游戏所需的基本 UI，例如准星/十字准线。
 */
UCLASS()
class FPSDEMO_API AFPSHUD : public AHUD
{
    GENERATED_BODY()

protected:
    /* 准星贴图资源。
     * 仅在蓝图中（EditDefaultsOnly）可编辑，方便策划/美术替换不同样式。 */
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    UTexture2D* CrosshairTexture;

public:
    /* 重写 AHUD::DrawHUD()
     * 每帧由引擎调用，负责在屏幕上绘制所有 HUD 元素。
     * 当前实现：在画面中央绘制 CrosshairTexture。
     */
    virtual void DrawHUD() override;
};