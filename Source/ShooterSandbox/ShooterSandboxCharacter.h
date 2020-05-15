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

	const float BUILD_DISTANCE = 1500;

//=#=#=#=#= VARIABLES =#=#=#=#=

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

	//*** Other Functions ***
	UFUNCTION(BlueprintCallable, Category = "Camera")
	void ReattachCamera(UCameraComponent* cam);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ToggleConstructionMenu();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	bool GetSpawnLocation(FVector &spawnLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Construction")
	void TryQuickConstruct();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetOffensiveConstructInVicinity(class ABaseOffensiveConstruct* construct);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class ABaseOffensiveConstruct* GetOffensiveConstructInVicinity();

	//*** RPC Functions ***

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ToggleRun(float newSpeed);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Construction")
	void TryConstruct(TSubclassOf<class ABaseConstruct> construct);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Gameplay")
	void AttemptControlOffensiveConstruct();


protected:

//=#=#=#=#= VARIABLES =#=#=#=#=

	bool bIsRunning;

	float walkSpeed;
	const float RUN_MULTIPLIER = 3;

	class AShooterSandboxController* myController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class ABaseOffensiveConstruct* currentConstructInVicinity;

//=#=#=#=#= FUNCTIONS =#=#=#=#=
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//*** Movement Functions ***

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void ToggleCrouch();
	void ToggleRun();

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

