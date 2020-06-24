// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructibleSurface.h"

// Sets default values
AConstructibleSurface::AConstructibleSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

float AConstructibleSurface::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Na uh we dont take damage here"));
	return 0;
}

// Called when the game starts or when spawned
void AConstructibleSurface::BeginPlay()
{
	Super::BeginPlay();
	
}
