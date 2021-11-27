// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxCharacter.h"
#include "BaseConstruct.h"
#include "PickupSpawnArea.h"
#include "EnergyPack.h"
#include "ShooterSandboxPlayerState.h"
#include "ConstructibleSurface.h"
#include "ConstructibleWall.h"
#include "AShooterSandboxHUD.h"
#include "ShooterSandboxGameState.h"
#include "ShooterSandboxController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "UObject/ConstructorHelpers.h"

AShooterSandboxGameMode::AShooterSandboxGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Characters/EBaseCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerControllerClass = AShooterSandboxController::StaticClass();
	HUDClass = AAShooterSandboxHUD::StaticClass();
	PlayerStateClass = AShooterSandboxPlayerState::StaticClass();
}

void AShooterSandboxGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(energyPulse, this, &AShooterSandboxGameMode::Server_GiveEnergyToPlayers, 4.0f, true);
	GetWorld()->GetTimerManager().SetTimer(weaponSpawnRoutine, this, &AShooterSandboxGameMode::SpawnNewPickup, pickupInterval, true);
}

void AShooterSandboxGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (NewPlayer == nullptr)
	{
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	Super::PostLogin(NewPlayer);

	AShooterSandboxCharacter* currentPlayer;

	Cast<AShooterSandboxPlayerState>(NewPlayer->GetPlayerState<AShooterSandboxPlayerState>())->playerNumber = GetGameState<AShooterSandboxGameState>()->PlayerArray.Num();
	Cast<AShooterSandboxController>(NewPlayer)->ClientPostLogin();
	if (Server_RespawnPlayer(NewPlayer, currentPlayer) && currentPlayer)
	{
		Server_GiveEnergyPackToPlayer(currentPlayer);
	}
}

void AShooterSandboxGameMode::SpawnNewPickup()
{
	TArray<AActor*> pickupSpots;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APickupSpawnArea::StaticClass(), pickupSpots);

	if(pickupSpots.Num() > 0 && Cast<APickupSpawnArea>(pickupSpots[0]))
	{
		Cast<APickupSpawnArea>(pickupSpots[0])->SpawnNewPickup(allWeaponPickups[0]);//FMath::RandRange(0, allWeaponPickups.Num() - 1)
	}

}

bool AShooterSandboxGameMode::Server_RespawnPlayer(APlayerController* playerController, AShooterSandboxCharacter*& playerCharacter)
{
	if (playerController == nullptr) {
		return false;
	}

	if (playerController->GetPawn()) {
		playerController->GetPawn()->Destroy();
	}

	TArray<AActor*> startPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), startPoints);

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APawn* spawnedActor = GetWorld()->SpawnActor<APawn>(playerCharacterToSpawn,
		startPoints.Num() == 0 ? FVector::ZeroVector : startPoints[FMath::RandRange(0, (startPoints.Num() - 1))]->GetActorLocation(),
		FRotator::ZeroRotator, spawnParams);
	playerController->Possess(spawnedActor);

	if (Cast<AShooterSandboxCharacter>(spawnedActor))
	{
		playerCharacter = Cast<AShooterSandboxCharacter>(spawnedActor);
		return true;
	}
	return false;
}

void AShooterSandboxGameMode::Server_GiveEnergyPackToPlayer(AShooterSandboxCharacter*& playerCharacter)
{
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AEnergyPack* spawnedPack = GetWorld()->SpawnActor<AEnergyPack>(playerEnergyPack, FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
	
	spawnedPack->SetOwner(playerCharacter);
	
	playerCharacter->Server_PickupEnergyPack(spawnedPack);

	AShooterSandboxPlayerState* thisGuysPlayerState = Cast<AShooterSandboxPlayerState>(playerCharacter->GetPlayerState());
	thisGuysPlayerState->SetMaxEnergy(MAX_ENERGY_AMOUNT);
	thisGuysPlayerState->SetHealthAndBalance(DEFAULT_PLAYER_HEALTH, DEFAULT_PLAYER_BALANCE);
}

void AShooterSandboxGameMode::Server_GiveEnergyToPlayers()
{
	APlayerController* currentPlayerController;
	AShooterSandboxCharacter* currentPlayerCharacter;
	for (FConstPlayerControllerIterator iterator = GetWorld()->GetPlayerControllerIterator(); iterator; ++iterator)
	{
		currentPlayerController = Cast<APlayerController>(*iterator);
		currentPlayerCharacter = Cast<AShooterSandboxCharacter>(currentPlayerController->GetCharacter());
		if (currentPlayerCharacter)
		{
			currentPlayerCharacter->Server_AddEnergy(PULSE_ENERGY_AMOUNT);
		}
	}
}

bool AShooterSandboxGameMode::GetSpawnLocationAndRotation(FVector &spawnLocation, FRotator &spawnRotation, AConstructibleSurface* &surfaceToSpawnOn, bool canBeParented, FString& constructName, AShooterSandboxCharacter* spawnerPlayer)
{
	if (!GetWorld()) {
		return false;
	}

	surfaceToSpawnOn = nullptr;

	if (spawnerPlayer->currentConstructionMode == EConstructionMode::Surface)
	{
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(spawnerPlayer);

		FHitResult hit;

		if (GetWorld()->LineTraceSingleByObjectType(hit, spawnerPlayer->FollowCamera->GetComponentLocation(),
			(spawnerPlayer->FollowCamera->GetComponentLocation() + (spawnerPlayer->FollowCamera->GetForwardVector() * BUILD_DISTANCE)),
			ECC_GameTraceChannel1, traceParams)) {

			if (Cast<AConstructibleSurface>(hit.Actor))
			{
				if (Cast<AConstructibleSurface>(hit.Actor)->requiresParenting && !canBeParented)
				{
					spawnerPlayer->Client_SendAlertMessage_Implementation("Can't construct " + constructName + " on " + Cast<AConstructibleSurface>(hit.Actor)->constructName);
					return false;
				}
				spawnLocation = hit.ImpactPoint;

				float gridSize = Cast<AConstructibleSurface>(hit.Actor)->gridSizeInUnits;
				int gridNoX = spawnLocation.X / gridSize;
				int gridNoY = spawnLocation.Y / gridSize;

				float gridAlignedX = (gridNoX * gridSize) + (spawnLocation.X < 0 ? -gridSize / 2 : gridSize / 2);
				float gridAlignedY = (gridNoY * gridSize) + (spawnLocation.Y < 0 ? -gridSize / 2 : gridSize / 2);

				spawnLocation = FVector(gridAlignedX, gridAlignedY, spawnLocation.Z);
				spawnRotation = spawnerPlayer->GetActorRotation();
				surfaceToSpawnOn = Cast<AConstructibleSurface>(hit.Actor);

				return true;
			}
			else
			{
				spawnerPlayer->Client_SendAlertMessage_Implementation("Aim at a constructible floor to build");
			}
		}

		else
		{
			spawnerPlayer->Client_SendAlertMessage_Implementation("Aim at a constructible floor nearby to build");
		}

	}

	else //if construction mode == wall
	{
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(spawnerPlayer);

		FHitResult hit;

		if (GetWorld()->LineTraceSingleByObjectType(hit, spawnerPlayer->FollowCamera->GetComponentLocation(),
			(spawnerPlayer->FollowCamera->GetComponentLocation() + (spawnerPlayer->FollowCamera->GetForwardVector() * BUILD_DISTANCE)),
			ECC_GameTraceChannel7, traceParams)) {

			if (Cast<AConstructibleWall>(hit.Actor))
			{
				spawnLocation = hit.ImpactPoint;

				float gridSize = Cast<AConstructibleWall>(hit.Actor)->gridSizeInUnits;
				float padding = Cast<AConstructibleWall>(hit.Actor)->gridBuildPadding;

				//if HitNormal has X=1, positioning will be along X
				//if HitNormal has Y=1, positioning will be along Y

				int gridNoZ = spawnLocation.Z / gridSize;
				float gridAlignedZ = (gridNoZ * gridSize) + (spawnLocation.Z < 0 ? -gridSize / 2 : gridSize / 2);

				int gridNoXY;
				float gridAlignedXY;

				if (hit.ImpactNormal.Y == 1 || hit.ImpactNormal.Y == -1)
				{
					gridNoXY = spawnLocation.X / gridSize;
					gridAlignedXY = (gridNoXY * gridSize) + (spawnLocation.X < 0 ? -gridSize / 2 : gridSize / 2);
					spawnLocation = FVector(gridAlignedXY, spawnLocation.Y - (hit.ImpactNormal.Y * padding), gridAlignedZ);
				}
				else
				{
					gridNoXY = spawnLocation.Y / gridSize;
					gridAlignedXY = (gridNoXY * gridSize) + (spawnLocation.Y < 0 ? -gridSize / 2 : gridSize / 2);
					spawnLocation = FVector(spawnLocation.X - (hit.ImpactNormal.X * padding), gridAlignedXY, gridAlignedZ);
				}

				spawnRotation = hit.ImpactNormal.Rotation();

				return true;
			}
			else
			{
				spawnerPlayer->Client_SendAlertMessage_Implementation("Aim at a constructible wall to build");
			}
		}

		else
		{
			spawnerPlayer->Client_SendAlertMessage_Implementation("Aim at a constructible wall nearby to build");
		}
	}

	spawnLocation = FVector::ZeroVector;
	spawnRotation = FRotator::ZeroRotator;

	return false;
}

void AShooterSandboxGameMode::PlaceNewConstructRequest(AShooterSandboxCharacter * spawnerPlayer, AShooterSandboxController * playerController, FName constructRowName, int playerEnergyBalance)
{
	FConstructsDatabase* databaseRow;
	GetConstructDetails(constructRowName, databaseRow);

	FVector spawnLocation;
	FRotator spawnRotation;
	AConstructibleSurface* surfaceToSpawnOn;

	if (databaseRow->constructionCost > playerEnergyBalance)
	{
		spawnerPlayer->Client_PlayerOutOfEnergy();
		return;
	}

	if (GetSpawnLocationAndRotation(spawnLocation, spawnRotation, surfaceToSpawnOn, databaseRow->constructBP.GetDefaultObject()->canBeParented, databaseRow->constructName, spawnerPlayer))
	{
		Server_SpawnConstruct(constructRowName, surfaceToSpawnOn, playerController, spawnLocation, spawnRotation);
	}

}

void AShooterSandboxGameMode::Server_SpawnConstruct(FName constructRowName, AConstructibleSurface* surfaceToSpawnOn, AShooterSandboxController* playerController, FVector spawnPosition, FRotator spawnRotation)
{
	UWorld* world = GetWorld();
	FConstructsDatabase* fetchedConstruct;
	GetConstructDetails(constructRowName, fetchedConstruct);

	if (world == nullptr || fetchedConstruct->constructBP == nullptr || playerController == nullptr)
	{
		return;
	}

	//Raise the construct up slightly so it doesn't collide with the building floor
	spawnPosition += FVector::UpVector * 1;
	spawnRotation = fetchedConstruct->constructBP.GetDefaultObject()->isGridAligned ? GetAlignedRotation(spawnRotation) : spawnRotation;

	FActorSpawnParameters spawnParams; 
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	spawnParams.Owner = playerController->GetCharacter(); 
	spawnParams.Instigator = playerController->GetPawn();
	
	ABaseConstruct* spawnedConstruct = world->SpawnActor<ABaseConstruct>(fetchedConstruct->constructBP, spawnPosition, spawnRotation, spawnParams);

	if (spawnedConstruct)
	{
		if (surfaceToSpawnOn && surfaceToSpawnOn->requiresParenting)
		{
			spawnedConstruct->AttachToActor(surfaceToSpawnOn, FAttachmentTransformRules::KeepWorldTransform);
		}

		spawnedConstruct->SetConstructedBy(playerController);
		playerController->PostConstructionUpdate(spawnedConstruct->constructName, spawnedConstruct->constructionCost);

		Cast<AShooterSandboxCharacter>(playerController->GetCharacter())->Server_SpendEnergy(fetchedConstruct->constructionCost, MAX_ENERGY_AMOUNT);
		Cast<AShooterSandboxPlayerState>(playerController->PlayerState)->HasConstructed(fetchedConstruct->constructionCost);
	}
	else
	{
		playerController->ProxyForHUD_AlertMessage("Build failed. Cell is occupied", 2);
	}
}

//REMOVE THIS FUNCTION WHEN TESTING IS DONE
void AShooterSandboxGameMode::Temp_PrintLog()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AShooterSandboxController* playerControl = Cast<AShooterSandboxController>(*Iterator);
		if (playerControl)
		{
			AShooterSandboxPlayerState* pState = Cast<AShooterSandboxPlayerState>(playerControl->PlayerState);
			if (pState) {
				FString msg = "" + playerControl->GetName() + " has built " + FString::FromInt(pState->GetNumConstructsBuilt());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, msg);
			}
		}
	}
}

FRotator AShooterSandboxGameMode::GetAlignedRotation(FRotator rawRotation)
{
	if (rawRotation.Yaw < -45 && rawRotation.Yaw > -135)
	{
		return FRotator(rawRotation.Pitch, -90, rawRotation.Roll);
	}
	else if (rawRotation.Yaw > 45 && rawRotation.Yaw < 135)
	{
		return FRotator(rawRotation.Pitch, 90, rawRotation.Roll);
	}

	else if (rawRotation.Yaw > -45 && rawRotation.Yaw < 45)
	{
		return FRotator(rawRotation.Pitch, 0, rawRotation.Roll);
	}

	return FRotator(rawRotation.Pitch, 179.5f, rawRotation.Roll);
}

bool AShooterSandboxGameMode::GetConstructDetails(FName rowName, FConstructsDatabase*& databaseRow)
{
	FString contextString;
	databaseRow = constructsDataTable->FindRow<FConstructsDatabase>(rowName, contextString);
	if (databaseRow)
	{
		return true;
	}
	return false;
}

int AShooterSandboxGameMode::GetConstructCost(FName rowName)
{
	FString contextString;
	FConstructsDatabase* databaseRow;

	databaseRow = constructsDataTable->FindRow<FConstructsDatabase>(rowName, contextString);
	if (databaseRow)
	{
		return databaseRow->constructionCost;
	}
	return -1;
}

FString& AShooterSandboxGameMode::GetConstructName(FName rowName)
{
	FString contextString;
	FConstructsDatabase* databaseRow;

	databaseRow = constructsDataTable->FindRow<FConstructsDatabase>(rowName, contextString);
	if (databaseRow)
	{
		return databaseRow->constructName;
	}
	return contextString;
}

