// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterSandboxController.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxPlayerState.h"
#include "AShooterSandboxHUD.h"
#include "BaseConstruct.h"
#include "BaseOffensiveConstruct.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void AShooterSandboxController::BeginPlay()
{
	SetupPlayerInputComponent();
}

void AShooterSandboxController::SetupPlayerInputComponent()//class UInputComponent* PlayerInputComponent
{
	Super::SetupInputComponent();

	//FInputActionBinding ActionBindingOpenScorecard;
	//ActionBindingOpenScorecard.KeyEvent = IE_Pressed;
	//FInputActionHandlerSignature OpenScorecardHandler;
	//OpenScorecardHandler.BindUFunction(this, FName("OpenScorecard"), true);
	//ActionBindingOpenScorecard.ActionDelegate = OpenScorecardHandler;

	//FInputActionBinding ActionBindingCloseScorecard;
	//ActionBindingCloseScorecard.KeyEvent = IE_Released;
	//FInputActionHandlerSignature CloseScorecardHandler;
	//CloseScorecardHandler.BindUFunction(this, FName("OpenScorecard"), false);
	//ActionBindingCloseScorecard.ActionDelegate = CloseScorecardHandler;
	
	//InputComponent->AddActionBinding(ActionBindingOpenScorecard);
	//InputComponent->AddActionBinding(ActionBindingCloseScorecard);

	InputComponent->BindAction("ShowScorecard", IE_Pressed, this, &AShooterSandboxController::OpenScorecard);
	InputComponent->BindAction("ShowScorecard", IE_Released, this, &AShooterSandboxController::CloseScorecard);
}

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

void AShooterSandboxController::OpenScorecard()
{
	Cast<AAShooterSandboxHUD>(GetHUD())->ShowScorecard(true);
}

void AShooterSandboxController::CloseScorecard()
{
	Cast<AAShooterSandboxHUD>(GetHUD())->ShowScorecard(false);
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
