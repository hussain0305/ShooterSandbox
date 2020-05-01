// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraShake.h"
#include "ShooterSandboxCharacter.generated.h"

UCLASS(config=Game)
class AShooterSandboxCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	AShooterSandboxCharacter();

	//*** VARIABLES ***

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<UCameraShake> runCamShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<UCameraShake> endRunCamShake;


	//*** FUNCTIONS ***

	virtual void BeginPlay();

protected:

	//*** VARIABLES ***

	bool bIsRunning;

	float walkSpeed;
	const float RUN_MULTIPLIER = 3;

	APlayerController* myController;

	//*** FUNCTIONS ***
	
	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	void ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ToggleRun();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleConstructionMenu();


protected:

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

