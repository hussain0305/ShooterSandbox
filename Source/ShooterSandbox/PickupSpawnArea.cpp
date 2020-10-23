// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupSpawnArea.h"
#include "BaseWeaponPickup.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickupSpawnArea::APickupSpawnArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APickupSpawnArea::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupSpawnArea::SpawnNewPickup(TSubclassOf<class ABaseWeaponPickup> PickupToSpawn)
{
	if (!HasAuthority())
	{
		return;
	}

	if (currentSpawnedPickup)
	{
		UKismetSystemLibrary::PrintString(this, (TEXT("Not spawning new since current spawn already present")));
		return;
		//currentSpawnedPickup->DestroyThyself();
	}

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	currentSpawnedPickup = GetWorld()->SpawnActor<ABaseWeaponPickup>(PickupToSpawn,
		GetActorLocation(),
		FRotator::ZeroRotator, spawnParams);
	currentSpawnedPickup->associatedSpawnArea = this;
}

void APickupSpawnArea::PickupWasPickedUp()
{
	currentSpawnedPickup = nullptr;

	//Multicast here for pickup spawn effects
}

