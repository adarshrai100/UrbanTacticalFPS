// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanTacticalFPSGameMode.h"
#include "PlayerOperator.h"
#include "UObject/ConstructorHelpers.h"


AUrbanTacticalFPSGameMode::AUrbanTacticalFPSGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
        TEXT("/Game/Blueprints/BP_PlayerOperator"));

    if (PlayerPawnBPClass.Class)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}
