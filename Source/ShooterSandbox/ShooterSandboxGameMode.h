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

/**************************
*       CONSTANTS         *
**************************/
	
	const int PULSE_ENERGY_AMOUNT = 15;
	const int MAX_ENERGY_AMOUNT = 500;
	const float DEFAULT_PLAYER_BALANCE = 100;
	const float DEFAULT_PLAYER_HEALTH = 25;
	const float BUILD_DISTANCE = 1500;

/**************************
*       VARIABLES         *
**************************/

	FTimerHandle energyPulse;
	FTimerHandle weaponSpawnRoutine;

//=#=#=#=#= EDITABLE IN BLUEPRINTS =#=#=#=#=

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class AShooterSandboxCharacter> playerCharacterToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class AEnergyPack> playerEnergyPack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	TArray<TSubclassOf<class ABaseWeaponPickup>> allWeaponPickups;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	float pickupInterval = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Constructs Database")
	class UDataTable* constructsDataTable;

/**************************
*       FUNCTIONS         *
**************************/

//=#=#=#=#= INHERENT FUNCTIONS =#=#=#=#=

	AShooterSandboxGameMode();

	void BeginPlay() override;

	void PostLogin(APlayerController* NewPlayer) override;

//=#=#=#=#= GAMEPLAY FUNCTIONS =#=#=#=#=

	void SpawnNewPickup();

	bool Server_RespawnPlayer(APlayerController* playerController, AShooterSandboxCharacter*& playerCharacter);
	
	void Server_GiveEnergyPackToPlayer(AShooterSandboxCharacter*& playerCharacter);

	void Server_GiveEnergyToPlayers();

	bool GetSpawnLocationAndRotation(FVector &spawnLocation, FRotator &spawnRotation, class AConstructibleSurface* &surfaceToSpawnOn, bool canBeParented, FString& constructName, class AShooterSandboxCharacter* spawnerPlayer);

	void PlaceNewConstructRequest(class AShooterSandboxCharacter* spawnerPlayer, class AShooterSandboxController* playerController, FName constructRowName, int playerEnergyBalance);

	void Server_SpawnConstruct(FName constructRowName, class AConstructibleSurface* surfaceToSpawnOn, class AShooterSandboxController* playerController, FVector spawnPosition, FRotator spawnRotation);

	void Temp_PrintLog();

	FRotator GetAlignedRotation(FRotator rawRotation);

//=#=#=#=#=	FETCHING FROM DATABASE =#=#=#=#=

	bool GetConstructDetails(FName rowName, struct FConstructsDatabase*& databaseRow);
	
	int GetConstructCost(FName rowName);

	FString& GetConstructName(FName rowName);

};