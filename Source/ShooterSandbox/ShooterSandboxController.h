// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterSandboxController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API AShooterSandboxController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UFUNCTION(Client, reliable, WithValidation)
	void ClientPostLogin();

	UFUNCTION(Client, reliable, WithValidation, BlueprintCallable, Category = "Player Stats")
	void PostConstructionUpdate(TSubclassOf<ABaseConstruct> construct);
	
	UFUNCTION(Client, reliable, WithValidation, BlueprintCallable, Category = "Player HUD")
	void ProxyForHUD_AlertMessage(const FString& message);

	void PossessThis(class ABaseOffensiveConstruct* constructToControl);
	void PossessThis(class AShooterSandboxCharacter* characterToControl);
};
