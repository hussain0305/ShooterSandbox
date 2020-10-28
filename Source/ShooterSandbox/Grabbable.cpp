// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabbable.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LevelGrabbablesManager.h"
#include "ShooterSandboxCharacter.h"
#include "Kismet/GameplayStatics.h"


AGrabbable::AGrabbable()
{
	ProjectileMovementComponent->InitialSpeed = 0;
	ProjectileMovementComponent->MaxSpeed = 30000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = false;
	ProjectileMovementComponent->bShouldBounce = true;

	isThrowable = false;
}

void AGrabbable::BeginPlay()
{
	Super::BeginPlay();
}

void AGrabbable::ApplyProjectileCollisionSettings()
{
}


void AGrabbable::MoveTowardsSocket_Implementation(AShooterSandboxCharacter* grabber)
{
	grabberCharacter = grabber;
	grabLerpAlpha = 0.0f;

	if (HasAuthority() && grabberCharacter)
	{
		GetWorld()->GetTimerManager().SetTimer(grabMotion, this, &AGrabbable::GrabMotion, MOTION_PULSE, true);
	}

}

void AGrabbable::GrabMotion()
{
	if (isThrowable) 
	{
		if (FVector::Dist(GetActorLocation(), grabberCharacter->GetMesh()->GetSocketLocation("GrabSocket")) > 100) 
		{
			isThrowable = false;
			grabLerpAlpha = 0.766f;
		}
		else
		{
			return;
		}

	}
	SetActorLocation(FMath::Lerp(GetActorLocation(), grabberCharacter->GetMesh()->GetSocketLocation("GrabSocket"), grabLerpAlpha));
	grabLerpAlpha += MOTION_PULSE;

	if (grabLerpAlpha >= 1.0f)
	{
		isThrowable = true;
	}
}

void AGrabbable::CancelGrabMotion()
{
	if (grabMotion.IsValid())
	{
		GetWorldTimerManager().ClearTimer(grabMotion);
		grabMotion.Invalidate();
	}
}

void AGrabbable::SetGrabManager_Implementation(ALevelGrabbablesManager* grabMan)
{
	grabManager = grabMan;
}


