// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnArea.generated.h"

UCLASS()
class SHOOTERSANDBOX_API APickupSpawnArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupSpawnArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	class ABaseWeaponPickup* currentSpawnedPickup;

	void SpawnNewPickup(TSubclassOf<class ABaseWeaponPickup> PickupToSpawn);

	void PickupWasPickedUp();
};
