// Fill out your copyright notice in the Description page of Project Settings.


#include "EConstructDestruction.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEConstructDestruction::AEConstructDestruction()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEConstructDestruction::BeginPlay()
{
	Super::BeginPlay();
}

void AEConstructDestruction::DestroyConstruct(FVector scale, UMaterialInstance* materialToApply, float forceVal, float scaleFactor)
{
	ConstructDestruction(scale, materialToApply, forceVal, scaleFactor);
}

