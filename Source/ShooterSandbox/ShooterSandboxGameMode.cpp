// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxCharacter.h"
#include "BaseConstruct.h"
#include "PickupSpawnArea.h"
#include "EnergyPack.h"
#include "ShooterSandboxPlayerState.h"
#include "AShooterSandboxHUD.h"
#include "ShooterSandboxGameState.h"
#include "ShooterSandboxController.h"
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
	
	playerCharacter->PickupEnergyPack(spawnedPack);
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
			currentPlayerCharacter->Server_AddEnergy(PULSE_ENERGY_AMOUNT, MAX_ENERGY_AMOUNT);
		}
	}
}

void AShooterSandboxGameMode::Server_SpawnConstruct(TSubclassOf<ABaseConstruct> construct, AShooterSandboxController* playerController, FVector spawnPosition, FRotator spawnRotation)
{
	UWorld* world = GetWorld();

	if (world == nullptr || construct == nullptr || playerController == nullptr)
	{
		return;
	}

	//Raise the construct up slightly so it doesn't collide with the building floor
	spawnPosition += construct.GetDefaultObject()->GetActorUpVector() * 1;
	spawnRotation = construct.GetDefaultObject()->isGridAligned ? FRotator::ZeroRotator : spawnRotation;

	FActorSpawnParameters spawnParams; 
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	spawnParams.Owner = playerController->GetCharacter(); 
	spawnParams.Instigator = playerController->GetPawn();
	
	ABaseConstruct* spawnedConstruct = world->SpawnActor<ABaseConstruct>(construct, spawnPosition, spawnRotation, spawnParams);

	if (spawnedConstruct)
	{
		spawnedConstruct->SetConstructedBy(playerController);
		playerController->PostConstructionUpdate(construct);

		Cast<AShooterSandboxCharacter>(playerController->GetCharacter())->Server_SpendEnergy(construct.GetDefaultObject()->constructionCost, MAX_ENERGY_AMOUNT);
		Cast<AShooterSandboxPlayerState>(playerController->PlayerState)->HasConstructed(construct.GetDefaultObject()->constructionCost);
		//Temp_PrintLog();
	}
	else
	{
		playerController->ProxyForHUD_AlertMessage("Build failed. Cell is occupied");
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