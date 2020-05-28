// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterSandboxController.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxPlayerState.h"
#include "AShooterSandboxHUD.h"
#include "BaseConstruct.h"
#include "BaseOffensiveConstruct.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

bool AShooterSandboxController::ClientPostLogin_Validate() {
	return true;
}

void AShooterSandboxController::ClientPostLogin_Implementation() 
{
	//Setup HUD here
	if (GetWorld()->GetAuthGameMode<AShooterSandboxGameMode>()) {
		//GetWorld()->GetAuthGameMode<AShooterSandboxGameMode>()->Server_RespawnPlayer(this);
	}
}

bool AShooterSandboxController::PostConstructionUpdate_Validate(TSubclassOf<ABaseConstruct> construct)
{
	return true;
}

void AShooterSandboxController::PostConstructionUpdate_Implementation(TSubclassOf<ABaseConstruct> construct)
{
	Cast<AAShooterSandboxHUD>(GetHUD())->ShowAlertMessage("" + construct.GetDefaultObject()->constructName + " constructed");
}

bool AShooterSandboxController::ProxyForHUD_AlertMessage_Validate(const FString& message)
{
	return true;
}

void AShooterSandboxController::ProxyForHUD_AlertMessage_Implementation(const FString& message)
{
	Cast<AAShooterSandboxHUD>(GetHUD())->ShowAlertMessage(message);
}

void AShooterSandboxController::PossessThis(ABaseOffensiveConstruct* constructToControl)
{
	if (Role == ROLE_Authority)
	{
		Possess(constructToControl);
		constructToControl->SetOwner(this);
	}
}

void AShooterSandboxController::PossessThis(AShooterSandboxCharacter* characterToControl)
{
	if (Role == ROLE_Authority)
	{
		Possess(characterToControl);
		characterToControl->SetOwner(this);
	}
}
