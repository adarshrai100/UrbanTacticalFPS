// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanTacticalFPSGameMode.h"
#include "PlayerOperator.h"


AUrbanTacticalFPSGameMode::AUrbanTacticalFPSGameMode()
{
	DefaultPawnClass = APlayerOperator::StaticClass();
}
