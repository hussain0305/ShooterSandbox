// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConstructibleWall.generated.h"

UCLASS()
class SHOOTERSANDBOX_API AConstructibleWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AConstructibleWall();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction Grid")
	float gridSizeInUnits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction Grid")
	float gridBuildPadding;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
