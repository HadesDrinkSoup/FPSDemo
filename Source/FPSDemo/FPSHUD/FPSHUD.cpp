// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSHUD/FPSHUD.h"
#include "Engine/Canvas.h"

// ----------------------------------------------------------
// DrawHUD
// 每帧由引擎调用，负责在屏幕上绘制 HUD 元素。
// 当前功能：在屏幕正中央渲染准星贴图。
// ----------------------------------------------------------
void AFPSHUD::DrawHUD()
{
    Super::DrawHUD();

    // 确保准星贴图资源有效
    if (CrosshairTexture)
    {
        /* 计算屏幕中心坐标（以像素为单位） */
        FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

        /* 计算贴图左上角绘制位置，使准星居中 */
        FVector2D CrossHairDrawPosition(
            Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f),   // 水平居左偏移
            Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f)    // 垂直居上偏移
        );

        /* 创建绘制项：指定位置、贴图资源及颜色（白色保持原色） */
        FCanvasTileItem TileItem(CrossHairDrawPosition, CrosshairTexture->GetResource(), FLinearColor::White);

        /* 设置混合模式为半透明，以支持带透明通道的准星贴图 */
        TileItem.BlendMode = SE_BLEND_Translucent;

        /* 将绘制项提交到 Canvas，完成渲染 */
        Canvas->DrawItem(TileItem);
    }
}