// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructibleWall.h"

// Sets default values
AConstructibleWall::AConstructibleWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AConstructibleWall::BeginPlay()
{
	Super::BeginPlay();
	
}
