// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterSandboxController.h"
#include "ShooterSandboxGameMode.h"
#include "Engine/World.h"
#include "ShooterSandboxGameMode.h"
#include "Kismet/GameplayStatics.h"

bool AShooterSandboxController::ClientPostLogin_Validate() {
	return true;
}

void AShooterSandboxController::ClientPostLogin_Implementation() 
{
	//Setup HUD here
	if (GetWorld()->GetAuthGameMode<AShooterSandboxGameMode>()) {
		GetWorld()->GetAuthGameMode<AShooterSandboxGameMode>()->Server_RespawnPlayer(this);
	}
}