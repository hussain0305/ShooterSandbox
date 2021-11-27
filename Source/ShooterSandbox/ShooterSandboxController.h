// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterSandboxGlobal.h"
#include "ShooterSandboxController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API AShooterSandboxController : public APlayerController
{
	GENERATED_BODY()
	
protected:

/**************************
*       FUNCTIONS         *
**************************/

//=#=#=#=#= INHERENT FUNCTIONS =#=#=#=#=

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent();//class UInputComponent* PlayerInputComponent

public:

//=#=#=#=#= CONNECTION FUNCTIONS =#=#=#=#=

	UFUNCTION(Client, reliable, WithValidation)
	void ClientPostLogin();

	UFUNCTION(Client, reliable, WithValidation, BlueprintCallable, Category = "Player Stats")
	void PostConstructionUpdate(const FString& constructName, int constructCost);
	
//=#=#=#=#= HUD FUNCTIONS =#=#=#=#=

	UFUNCTION(Client, reliable, BlueprintCallable, Category = "Player HUD")
	void ProxyForHUD_AlertMessage(const FString& message, int greenRedNeut);

	UFUNCTION(Client, reliable, BlueprintCallable, Category = "Player HUD")
	void ProxyForHUD_ByCommandCode(EHUDCommandType commandType);

//=#=#=#=#= GAMEPLAY FUNCTIONS =#=#=#=#=

	void OpenScorecard();
	void CloseScorecard();

	void PossessThis(class ABaseOffensiveConstruct* constructToControl);
	void PossessThis(class AShooterSandboxCharacter* characterToControl);
};
