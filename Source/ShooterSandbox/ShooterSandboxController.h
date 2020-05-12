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
};
