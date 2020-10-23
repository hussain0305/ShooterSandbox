// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGrabbablesManager.generated.h"

UCLASS()
class SHOOTERSANDBOX_API ALevelGrabbablesManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGrabbablesManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	FVector2D xBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	FVector2D yBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	FVector2D zBounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	int spawnFrequency = 15;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	int numObjectsToSpawnPer = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
	TArray<TSubclassOf<class AGrabbable>> grabbableObjectBlueprints;

	FTimerHandle spawnGrabs;

	int currentGrabbableBlueprint = 0;

	void SpawnSome();

	UFUNCTION(Server, Reliable, WithValidation)
	void PlayerWantsGrabbable(class AShooterSandboxCharacter* grabber, class AGrabbable* grabee);

};
