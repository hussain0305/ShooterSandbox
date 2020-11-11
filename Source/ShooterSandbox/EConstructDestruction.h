// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EConstructDestruction.generated.h"

UCLASS()
class SHOOTERSANDBOX_API AEConstructDestruction : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEConstructDestruction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void DestroyConstruct(class UMaterialInstance* materialToApply, int lowMidHigh);

	UFUNCTION(BlueprintImplementableEvent)
	void ConstructDestruction(class UMaterialInstance* materialToApply, int lowMidHigh);

	void DestroyVFX();
};
