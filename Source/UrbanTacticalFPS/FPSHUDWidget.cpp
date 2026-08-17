#include "FPSHUDWidget.h"
#include "Components/TextBlock.h"

void UFPSHUDWidget::SetAmmo(int32 CurrentAmmo, int32 ReserveAmmo)
{
    if (AmmoText)
    {
        AmmoText->SetText(
            FText::FromString(
                FString::Printf(TEXT("%d / %d"), CurrentAmmo, ReserveAmmo)
            )
        );
    }
}

void UFPSHUDWidget::HideCrosshair()
{
    if (CrosshairImage)
    {
        CrosshairImage->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UFPSHUDWidget::ShowCrosshair()
{
    if (CrosshairImage)
    {
        CrosshairImage->SetVisibility(ESlateVisibility::Visible);
    }
}

void UFPSHUDWidget::ShowGameOver()
{
    if (GameOverText)
    {
        GameOverText->SetVisibility(ESlateVisibility::Visible);
    }

    if (RestartButton)
    {
        RestartButton->SetVisibility(ESlateVisibility::Visible);
    }
}

void UFPSHUDWidget::HideGameOver()
{
    if (GameOverText)
    {
        GameOverText->SetVisibility(ESlateVisibility::Hidden);
    }

    if (RestartButton)
    {
        RestartButton->SetVisibility(ESlateVisibility::Hidden);
    }
}


