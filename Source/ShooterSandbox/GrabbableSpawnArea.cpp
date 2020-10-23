// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbableSpawnArea.h"
#include "Grabbable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGrabbableSpawnArea::AGrabbableSpawnArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
//void AGrabbableSpawnArea::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

//void AGrabbableSpawnArea::SpawnNewGrabbable(TSubclassOf<class AGrabbable> GrabbableToSpawn)
//{
//	if (!HasAuthority())
//	{
//		return;
//	}
//
//	if (currentSpawnedGrabbable)
//	{
//		UKismetSystemLibrary::PrintString(this, (TEXT("Not spawning new since current spawn already present")));
//		return;
//		//currentSpawnedPickup->DestroyThyself();
//	}
//
//	FActorSpawnParameters spawnParams;
//	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	currentSpawnedGrabbable = GetWorld()->SpawnActor<AGrabbable>(GrabbableToSpawn,
//		GetActorLocation(),
//		FRotator::ZeroRotator, spawnParams);
//	//currentSpawnedGrabbable->miMaestro = this;
//}

//void AGrabbableSpawnArea::GrabbableWasGrabbed()
//{
//	currentSpawnedGrabbable = nullptr;
//
//	//Multicast here for pickup spawn effects
//}
