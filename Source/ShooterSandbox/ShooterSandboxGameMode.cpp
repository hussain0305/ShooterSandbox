// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxCharacter.h"
#include "BaseConstruct.h"
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
	Server_RespawnPlayer(NewPlayer);
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
	UWorld* world = GetWorld();

	if (world == nullptr || construct == nullptr || playerController == nullptr) {
		return;
	}

	//Raise the construct up slightly so it doesn't collide with the building floor
	spawnPosition += construct.GetDefaultObject()->GetActorUpVector() * 1;

	FActorSpawnParameters spawnParams; 
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	spawnParams.Owner = playerController->GetCharacter(); 
	spawnParams.Instigator = playerController->GetPawn();
	
	ABaseConstruct* spawnedConstruct = world->SpawnActor<ABaseConstruct>(construct, spawnPosition, spawnRotation, spawnParams);

	if (spawnedConstruct) {
		spawnedConstruct->SetConstructedBy(playerController);
		playerController->PostConstructionUpdate(construct);

		Cast<AShooterSandboxPlayerState>(playerController->PlayerState)->HasConstructed(construct.GetDefaultObject()->constructionCost);

		//Temp_PrintLog();
	}
	else {
		playerController->ProxyForHUD_AlertMessage("Build failed. Another construct already exists there");
	}
}

//void AShooterSandboxGameMode::Server_ControlOffensiveConstruct(class ABaseConstruct* construct, AShooterSandboxController * playerController)
//{
//	if (construct == nullptr)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("construct is null"));
//		if (playerController == nullptr)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("playerController is null"));
//			return;
//		}
//		return;
//	}
//	FString msg = construct->GetName() + "'s former Owner was " + construct->GetOwner()->GetName();
//	UE_LOG(LogTemp, Warning, TEXT("%s"), *msg);
//	construct->SetOwner(playerController);
//	msg = construct->GetName() + "'s current Owner is " + construct->GetOwner()->GetName();
//	UE_LOG(LogTemp, Warning, TEXT("%s"), *msg);
//}

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