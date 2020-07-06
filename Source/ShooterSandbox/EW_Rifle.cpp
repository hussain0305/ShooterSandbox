// Fill out your copyright notice in the Description page of Project Settings.


#include "EW_Rifle.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxController.h"
#include "ShooterProjectile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

void AEW_Rifle::StartShooting()
{
	GetWorld()->GetTimerManager().SetTimer(sprayTimer, this, &AEW_Rifle::SpawnProjectile, 0.33f, true);
}

void AEW_Rifle::StopShooting()
{
	if (sprayTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(sprayTimer);
		sprayTimer.Invalidate();
	}
}

void AEW_Rifle::AlternateFire()
{
}

bool AEW_Rifle::SpawnProjectile_Validate()
{
	return true;
}

//static UParticleSystemComponent * SpawnEmitterAtLocation
//(
//	UWorld * World,
//	UParticleSystem * EmitterTemplate,
//	const FTransform & SpawnTransform,
//	bool bAutoDestroy,
//	EPSCPoolMethod PoolingMethod,
//	bool bAutoActivate
//)

void AEW_Rifle::SpawnProjectile_Implementation()
{
	if (Role == ROLE_Authority && GetWorld() && projectile && referenceCam)
	{
		if (SpendAmmo()) {
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			spawnParams.Owner = GetOwner();
			spawnParams.Instigator = Cast<AShooterSandboxCharacter>(GetOwner())->GetMyController()->GetPawn();

			AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, gunBody->GetSocketLocation(FName("Muzzle")), GetActorRotation(), spawnParams);

			if (spawnedProjectile)
			{
				spawnedProjectile->SetShooterController(Cast<AShooterSandboxCharacter>(GetOwner())->GetMyController());
				
				spawnedProjectile->FireInDirection(referenceCam->GetForwardVector() + (GetOwner()->GetVelocity().GetSafeNormal() * mobilityMultiplier));
				
				Cast<AShooterSandboxCharacter>(GetOwner())->Client_UpdateWeaponAmmo(currentClipSize, clipSize);

				//UGameplayStatics::SpawnEmitterAtLocation(this, bulletTrail, gunBody->GetSocketLocation(FName("Muzzle")), GetActorRotation());
				//userController->ClientPlayCameraShake(shotShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
			}

			if (currentClipSize == 0)
			{
				Cast<AShooterSandboxCharacter>(GetOwner())->Multicast_PickupOrDropWeapon(nullptr);
				Cast<AShooterSandboxCharacter>(GetOwner())->Client_PickupOrDropWeapon(false);
				DestroyWeapon();
			}
		}
	}
}
