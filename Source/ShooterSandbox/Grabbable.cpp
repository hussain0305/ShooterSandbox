// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabbable.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGrabbable::AGrabbable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	//body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	//body->SetMobility(EComponentMobility::Movable);

	ProjectileMovementComponent->InitialSpeed = 0;
	ProjectileMovementComponent->MaxSpeed = 30000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AGrabbable::BeginPlay()
{
	Super::BeginPlay();

	/*TurnEthereal();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(deathCountdown, this, &AGrabbable::SelfDestruction, lifetime, true);
	}*/

}

//void AGrabbable::TurnEthereal()
//{
//	body->CanCharacterStepUpOn = ECB_No;
//	body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//}
//
//void AGrabbable::SelfDestruction()
//{
//	if (HasAuthority()) 
//	{
//		Destroy(this);
//	}
//}

