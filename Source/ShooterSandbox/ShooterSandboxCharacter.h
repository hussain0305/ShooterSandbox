// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraShake.h"
#include "ShooterSandboxCharacter.generated.h"

enum class EMovementState : uint8;

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

	const float BUILD_DISTANCE = 1500;

//=#=#=#=#= VARIABLES =#=#=#=#=

	FTimerHandle movementStateMonitoring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<UCameraShake> runCamShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<UCameraShake> endRunCamShake;

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	virtual void BeginPlay() override;

	//************ Movement State Functions ************

	void MonitorMovementState();

	void SetCurrentEMovementState(EMovementState newState);

	EMovementState GetCurrentEMovementState();

	//************ Construction Related Functions ************

	void MouseWheelDown();
	void MouseWheelUp();

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleConstructionMenu();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void TryConstruct(TSubclassOf<class ABaseConstruct> construct);

	UFUNCTION(BlueprintImplementableEvent, Category = "Construction")
	void TryQuickConstruct();
	
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Construction")
	void ServerConstruct(TSubclassOf<class ABaseConstruct> construct, class AShooterSandboxController* constructorController, FVector spawnLocation, FRotator spawnRotation);

	//************ Other Functions ************

	UFUNCTION(BlueprintCallable, Category = "Construction")
	bool GetSpawnLocation(FVector &spawnLocation);


	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Gameplay")
	void SetOffensiveConstructInVicinity(class ABaseOffensiveConstruct* construct);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class ABaseOffensiveConstruct* GetOffensiveConstructInVicinity();

	//************ RPC Functions ************

	void AttemptControlOffensiveConstruct();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Gameplay")
	void Server_AttemptControlOffensiveConstruct(class ABaseOffensiveConstruct* constructToControl, class AShooterSandboxController* controllerController);

	UFUNCTION(BlueprintCallable)
	void PrintTestData();


protected:

//=#=#=#=#= VARIABLES =#=#=#=#=
	bool bIsRunning;
	EMovementState currentMovementState;

	float walkSpeed;
	const float RUN_MULTIPLIER = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AShooterSandboxController* myController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class ABaseOffensiveConstruct* currentConstructInVicinity;

//=#=#=#=#= FUNCTIONS =#=#=#=#=
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//************ Movement Functions ************

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void ToggleCrouch();
	void ToggleRunOn();
	void ToggleRunOff();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ToggleRun(float newSpeed);

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
