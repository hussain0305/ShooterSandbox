// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabbable.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "LevelGrabbablesManager.h"
#include "ShooterSandboxCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


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
	BP_ApplyProjectileCollisionSettings();
}


void AGrabbable::MoveTowardsSocket_Implementation(AShooterSandboxCharacter* grabber)
{
	if (!grabber)
	{
		return;
	}

	grabberCharacter = grabber;
	grabLerpAlpha = 0.0f;

	if (grabberCharacter)//HasAuthority() && 
	{
		GetWorld()->GetTimerManager().SetTimer(grabMotion, this, &AGrabbable::GrabMotion, MOTION_PULSE, true);
	}

}

void AGrabbable::GrabMotion()
{
	if (isThrowable) 
	{
		if (grabMotion.IsValid())
		{
			GetWorldTimerManager().ClearTimer(grabMotion);
			grabMotion.Invalidate();
		}

		return;
	}

	SetActorLocation(FMath::Lerp(GetActorLocation(), grabberCharacter->GetMesh()->GetSocketLocation("GrabSocket"), grabLerpAlpha));

	if (grabLerpAlpha >= 1.0f)
	{
		AttachToComponent(grabberCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GrabSocket"));

		isThrowable = true;

		return;
	}

	grabLerpAlpha += MOTION_PULSE;
}

void AGrabbable::CancelGrabMotion()
{
	if (grabMotion.IsValid())
	{
		GetWorldTimerManager().ClearTimer(grabMotion);
		grabMotion.Invalidate();
	}

	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AGrabbable::SetGrabManager_Implementation(ALevelGrabbablesManager* grabMan)
{
	grabManager = grabMan;
}

void AGrabbable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGrabbable, isThrowable);
}

