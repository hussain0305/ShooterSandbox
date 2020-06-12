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

void AEW_Rifle::SpawnProjectile_Implementation()
{
	if (Role == ROLE_Authority && GetWorld() && projectile && referenceCam)
	{
		energyAmount--;

		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParams.Owner = GetOwner();
		spawnParams.Instigator = Cast<AShooterSandboxCharacter>(GetOwner())->GetMyController()->GetPawn();

		AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, gunBody->GetSocketLocation(FName("Muzzle")), FRotator::ZeroRotator, spawnParams);

		if (spawnedProjectile)
		{
			spawnedProjectile->SetShooterController(Cast<AShooterSandboxCharacter>(GetOwner())->GetMyController());
			spawnedProjectile->FireInDirection(referenceCam->GetForwardVector());

			//userController->ClientPlayCameraShake(shotShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
		}
	}
}
