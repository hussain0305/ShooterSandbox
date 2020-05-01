// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxCharacter.h"
#include "AShooterSandboxHUD.h"
#include "ShooterSandboxController.h"
#include "UObject/ConstructorHelpers.h"

AShooterSandboxGameMode::AShooterSandboxGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/EBaseCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AShooterSandboxController::StaticClass();
	HUDClass = AAShooterSandboxHUD::StaticClass();
}

