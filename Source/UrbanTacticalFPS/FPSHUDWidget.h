// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "FPSHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class URBANTACTICALFPS_API UFPSHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetAmmo(int32 CurrentAmmo, int32 ReserveAmmo);
	void ShowCrosshair();
	void HideCrosshair();
	void ShowGameOver();
	void HideGameOver();
	void SetObjectiveText(const FText& Text);
	void ShowMissionComplete();

protected:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CrosshairImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GameOverText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RestartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectiveText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MissionCompleteText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;
	
};
