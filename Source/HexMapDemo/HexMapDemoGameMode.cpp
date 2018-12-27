// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HexMapDemoGameMode.h"
#include "HexMapDemoPlayerController.h"
#include "HexMapDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHexMapDemoGameMode::AHexMapDemoGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AHexMapDemoPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}