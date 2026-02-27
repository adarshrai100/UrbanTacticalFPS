// Copyright Epic Games, Inc. All Rights Reserved.

#include "UrbanTacticalFPSGameMode.h"
#include "UrbanTacticalFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUrbanTacticalFPSGameMode::AUrbanTacticalFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
