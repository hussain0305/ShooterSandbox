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
	AShooterSandboxGameMode();

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	void PostLogin(APlayerController* NewPlayer) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<class AShooterSandboxCharacter> playerCharacterToSpawn;

	void Server_RespawnPlayer(APlayerController* playerController);

	void Server_SpawnConstruct(TSubclassOf<class ABaseConstruct> construct, class AShooterSandboxController* playerController, FVector spawnPosition, FRotator spawnRotation);

	void Temp_PrintLog();
};