// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterSandboxGameMode.generated.h"

UCLASS(minimalapi)
class AShooterSandboxGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	const int PULSE_ENERGY_AMOUNT = 15;
	const int MAX_ENERGY_AMOUNT = 500;

	AShooterSandboxGameMode();

	FTimerHandle energyPulse;
	FTimerHandle weaponSpawnRoutine;

	void BeginPlay() override;

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	void PostLogin(APlayerController* NewPlayer) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class AShooterSandboxCharacter> playerCharacterToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class AEnergyPack> playerEnergyPack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	TArray<TSubclassOf<class ABaseWeaponPickup>> allWeaponPickups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	float pickupInterval = 5;

	void SpawnNewPickup();

	bool Server_RespawnPlayer(APlayerController* playerController, AShooterSandboxCharacter*& playerCharacter);
	
	void Server_GiveEnergyPackToPlayer(AShooterSandboxCharacter*& playerCharacter);

	void Server_GiveEnergyToPlayers();

	void Server_SpawnConstruct(TSubclassOf<class ABaseConstruct> construct, class AConstructibleSurface* surfaceToSpawnOn, class AShooterSandboxController* playerController, FVector spawnPosition, FRotator spawnRotation);

	void Temp_PrintLog();
};