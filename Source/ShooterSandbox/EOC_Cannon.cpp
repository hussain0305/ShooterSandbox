// Fill out your copyright notice in the Description page of Project Settings.


#include "EOC_Cannon.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxController.h"
#include "ShooterProjectile.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShake.h"

void AEOC_Cannon::StartShooting()
{	
	Super::StartShooting();

	SpawnProjectile();
}

void AEOC_Cannon::StopShooting()
{
	Super::StopShooting();

}

void AEOC_Cannon::SwitchMode()
{
	Super::SwitchMode();
}

bool AEOC_Cannon::SpawnProjectile_Validate()
{
	return true;
}

void AEOC_Cannon::SpawnProjectile_Implementation()
{
	if (Role == ROLE_Authority && GetWorld() && projectile)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		spawnParams.Owner = userCharacter;
		spawnParams.Instigator = userCharacter;

		AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, barrel->GetSocketLocation(FName("Muzzle")), barrel->GetSocketRotation(FName("Muzzle")), spawnParams);

		if (spawnedProjectile)
		{
			spawnedProjectile->SetShooterController(userController);
			spawnedProjectile->FireInDirection(barrel->GetSocketRotation(FName("Muzzle")).Vector());

			AddControllerPitchInputTo(FMath::RandRange(pitchRecoilRange.X, pitchRecoilRange.Y));
			AddControllerYawInputTo(FMath::RandRange(yawRecoilRange.X, yawRecoilRange.Y));

			userController->ClientPlayCameraShake(shotShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
		}

	}
}