// Fill out your copyright notice in the Description page of Project Settings.


/*******************************
*                              *
*       DEPRECATED CLASS       *
*                              *
*       DEPRECATED CLASS       *
*                              *
*       DEPRECATED CLASS       *
*                              *
********************************/


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrabbableSpawnArea.generated.h"

UCLASS()
class SHOOTERSANDBOX_API AGrabbableSpawnArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabbableSpawnArea();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	

	//class AGrabbable* currentSpawnedGrabbable;

	//void SpawnNewGrabbable(TSubclassOf<class AGrabbable> GrabbableToSpawn);

	//void GrabbableWasGrabbed();


};
