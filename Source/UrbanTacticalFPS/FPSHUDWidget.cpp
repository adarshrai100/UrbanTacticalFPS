#include "FPSHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "TimerManager.h"

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

void UFPSHUDWidget::SetObjectiveText(const FText& Text)
{
    if (ObjectiveText)
    {
        ObjectiveText->SetText(Text);
        ObjectiveText->SetVisibility(ESlateVisibility::Visible);
    }
}


void UFPSHUDWidget::ShowMissionComplete()
{
    if (ObjectiveText)
    {
        ObjectiveText->SetVisibility(ESlateVisibility::Hidden);
    }

    if (MissionCompleteText)
    {
        MissionCompleteText->SetVisibility(ESlateVisibility::Visible);
    }

    if (ContinueButton)
    {
        ContinueButton->SetVisibility(ESlateVisibility::Visible);
    }
}

void UFPSHUDWidget::OnContinueClicked()
{
    UGameplayStatics::OpenLevel(
        this,
        FName(TEXT("L_TacticalMission"))
    );
}

void UFPSHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (DamageOverlay)
    {
        DamageOverlay->SetRenderOpacity(0.0f);
    }

    if (ContinueButton)
    {
        ContinueButton->OnClicked.AddDynamic(
            this,
            &UFPSHUDWidget::OnContinueClicked
        );
    }
}

void UFPSHUDWidget::ShowDamageEffect()
{
    UE_LOG(
        LogTemp,
        Warning,
        TEXT("SHOW DAMAGE EFFECT CALLED | Overlay Valid: %s"),
        DamageOverlay ? TEXT("YES") : TEXT("NO")
    );

    if (!DamageOverlay)
    {
        return;
    }

    DamageOverlay->SetRenderOpacity(0.35f);

    GetWorld()->GetTimerManager().SetTimer(
        DamageEffectTimer,
        this,
        &UFPSHUDWidget::HideDamageEffect,
        0.1f,
        false
    );
}

void UFPSHUDWidget::HideDamageEffect()
{
    if (DamageOverlay)
    {
        DamageOverlay->SetRenderOpacity(0.0f);
    }
}



