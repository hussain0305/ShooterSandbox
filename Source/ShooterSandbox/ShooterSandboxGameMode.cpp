// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxCharacter.h"
#include "AShooterSandboxHUD.h"
#include "BaseConstruct.h"
#include "ShooterSandboxPlayerState.h"
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

void AShooterSandboxGameMode::PostLogin(APlayerController * NewPlayer)
{
	if (NewPlayer == nullptr) {
		return;
	}

	if (!HasAuthority()) {
		return;
	}

	Super::PostLogin(NewPlayer);

	Cast<AShooterSandboxPlayerState>(NewPlayer->GetPlayerState<AShooterSandboxPlayerState>())->playerNumber = GetGameState<AShooterSandboxGameState>()->PlayerArray.Num();
	Cast<AShooterSandboxController>(NewPlayer)->ClientPostLogin();
}

void AShooterSandboxGameMode::Server_RespawnPlayer(APlayerController * playerController)//_Implementation
{
	if (playerController == nullptr) {
		return;
	}

	if (playerController->GetPawn()) {
		playerController->GetPawn()->Destroy();
	}

	TArray<AActor*> startPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), startPoints);

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APawn* spawnedActor = GetWorld()->SpawnActor<APawn>(playerCharacterToSpawn, FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
	playerController->Possess(spawnedActor);
}

void AShooterSandboxGameMode::Server_SpawnConstruct(TSubclassOf<ABaseConstruct> construct, AShooterSandboxController* playerController, FVector spawnPosition, FRotator spawnRotation)//_Implementation
{
	FActorSpawnParameters spawnParams; 
	spawnParams.Owner = playerController->GetCharacter(); 
	spawnParams.Instigator = playerController->GetPawn();
	UWorld* world = GetWorld();
	
	if(world){
		ABaseConstruct* spawnedConstruct = world->SpawnActor<ABaseConstruct>(construct, spawnPosition, spawnRotation, spawnParams);

		if (spawnedConstruct) {
			spawnedConstruct->SetConstructOwner(playerController);
			Cast<AShooterSandboxPlayerState>(playerController->PlayerState)->HasConstructed();
			Temp_PrintLog();
		}
	}
}

void AShooterSandboxGameMode::Temp_PrintLog() {
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Cast<APlayerController>(*Iterator);
		if (PlayerController)
		{
			AShooterSandboxPlayerState* pState = Cast<AShooterSandboxPlayerState>(PlayerController->PlayerState);
			if (pState) {
				FString msg = "" + PlayerController->GetName() + " has built " + FString::FromInt(pState->GetNumConstructsBuilt());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, msg);
			}
		}
	}
}


//bool AShooterSandboxGameMode::Server_SpawnConstruct_Validate(TSubclassOf<class ABaseConstruct> construct, AShooterSandboxController * playerController, FVector spawnPosition, FRotator spawnRotation)
//{
//	return true;
//}


//bool AShooterSandboxGameMode::Server_RespawnPlayer_Validate(APlayerController * playerController)
//{
//	return true;
//}
