// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterSandboxCharacter.generated.h"

enum class EMovementState : uint8;
enum class EConstructionMode : uint8;

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
	const float JETPACK_THRUST_COST = 10;

	class ABaseWeapon* weaponCurrentlyHeld;

//=#=#=#=#= VARIABLES =#=#=#=#=

	EConstructionMode currentConstructionMode;

	FTimerHandle movementStateMonitoring;
	FTimerHandle jetpack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bHasWeapon = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool jetpackActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float jetpackThrust = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<class UCameraShake> runCamShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<class UCameraShake> endRunCamShake;

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	virtual void BeginPlay() override;

	void OnRep_PlayerState() override;

	void FetchPersistentComponents();

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
	bool GetSpawnLocationAndRotation(FVector &spawnLocation, FRotator &spawnRotation);

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Gameplay")
	void SetOffensiveConstructInVicinity(class ABaseOffensiveConstruct* construct);

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	class ABaseOffensiveConstruct* GetOffensiveConstructInVicinity();

	UFUNCTION(BlueprintCallable, Category = "Construction")
	void SwitchConstructionMode();

	//************ Energy and Energy Pack Functions ************
	
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void Server_AddEnergy(int amount, int maxEnergy);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void Server_SpendEnergy(int amount, int maxEnergy);

	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Energy")
	void Client_UpdateEnergyOnHUD();

	UFUNCTION(Client, Unreliable)
	void PlayerOutOfEnergy();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void PickupEnergyPack(class AEnergyPack* thePack);

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void PickupEnergyPackOnOwningClient(class AEnergyPack* thePack);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void DropEnergyPack();

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void DropEnergyPackOnOwningClient();

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void Multicast_PickupOrDropWeapon(class ABaseWeapon* theWeapon);

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void Client_PickupOrDropWeapon(bool hasPickedUp);

	UFUNCTION(Client, Reliable, WithValidation, BlueprintCallable, Category = "Energy")
	void Client_UpdateWeaponAmmo(int max, int current);

	void StartWeaponFire();
	void StopWeaponFire();
	void WeaponAltMode();

	//UFUNCTION(Category = "Energy")
	//void OnRep_EnergyChanged();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AShooterSandboxPlayerState* myPlayerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AAShooterSandboxHUD* myHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	class ABaseOffensiveConstruct* currentConstructInVicinity;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	class AEnergyPack* myEnergyPack;

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
	void ToggleRunCamShake(bool startShake);

	void ToggleJetpackOn();
	void ToggleJetpackOff();
	UFUNCTION(Server, Reliable, WithValidation)
	void Jetpack();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ToggleRun(float newSpeed);

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class AShooterSandboxController* GetMyController() const { return myController; }
};