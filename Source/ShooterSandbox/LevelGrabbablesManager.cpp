// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelGrabbablesManager.h"
#include "Grabbable.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALevelGrabbablesManager::ALevelGrabbablesManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALevelGrabbablesManager::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	//ONLY DO THE STUFF BELOW IF YOU'RE THE SERVER

	GetWorld()->GetTimerManager().SetTimer(spawnGrabs, this, &ALevelGrabbablesManager::SpawnSome, spawnFrequency, true);

}

void ALevelGrabbablesManager::SpawnSome()
{
	FVector loc;
	AGrabbable* spawnedGrabbable;
	for (int loop = 0; loop < numObjectsToSpawnPer; loop++)
	{
		loc = FVector(1000 * FMath::RandRange((int)xBounds.X, (int)xBounds.Y), 1000 * FMath::RandRange((int)yBounds.X, (int)yBounds.Y), 
			1000 * FMath::RandRange((int)zBounds.X, (int)zBounds.Y));
		
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

		spawnedGrabbable = GetWorld()->SpawnActor<AGrabbable>(grabbableObjectBlueprints[currentGrabbableBlueprint],
			loc, FRotator::ZeroRotator, spawnParams);

		spawnedGrabbable->SetGrabManager(this);
		//spawnedGrabbable->grabManager = this;
	}
}

bool ALevelGrabbablesManager::PlayerWantsGrabbable_Validate(AShooterSandboxCharacter* grabber, AGrabbable * grab)
{
	return true;
}

void ALevelGrabbablesManager::PlayerWantsGrabbable_Implementation(AShooterSandboxCharacter* grabber, AGrabbable * grab)
{
	if (!grabber || !grab)
	{
		return;
	}

	//Cancel this grabbale's auto-death
	grab->InvalidateDeathTimer();

	//Sort out owner properties
	if (grabber->GetGrabbableInHand())
	{
		grabber->GetGrabbableInHand()->DestroyProjectile();
	}
	grab->Instigator = grabber;
	grab->SetOwner(grabber);
	grab->SetShooterController(Cast<AShooterSandboxController>(grabber->GetController()));
	grabber->Server_SetGrabbableInHand(grab);

	//Move Grabbable towards player
	grab->MoveTowardsSocket(grabber);
	grab->ApplyVisualChangesUponGrab();
}