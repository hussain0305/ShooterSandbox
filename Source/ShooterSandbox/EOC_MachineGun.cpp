// Fill out your copyright notice in the Description page of Project Settings.


#include "EOC_MachineGun.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxController.h"
#include "ShooterProjectile.h"
#include "ShooterSandboxGlobal.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShake.h"
#include "Camera/CameraComponent.h"


void AEOC_MachineGun::StartShooting()
{
	if (machineGunShot.IsValid())
	{
		GetWorldTimerManager().ClearTimer(machineGunShot);
		machineGunShot.Invalidate();
	}

	if (recoilProcess.IsValid())
	{
		GetWorldTimerManager().ClearTimer(recoilProcess);
		recoilProcess.Invalidate();
	}

	GetWorld()->GetTimerManager().SetTimer(machineGunShot, this, &AEOC_MachineGun::SpawnProjectile, 0.2f, true);
	GetWorld()->GetTimerManager().SetTimer(recoilProcess, this, &AEOC_MachineGun::PerformRecoil, 0.016f, true);
}

void AEOC_MachineGun::StopShooting()
{
	if (machineGunShot.IsValid())
	{
		GetWorldTimerManager().ClearTimer(machineGunShot);
		machineGunShot.Invalidate();
	}

	if (recoilProcess.IsValid())
	{
		GetWorldTimerManager().ClearTimer(recoilProcess);
		recoilProcess.Invalidate();
	}

}

bool AEOC_MachineGun::SpawnProjectile_Validate()
{
	return true;
}
void AEOC_MachineGun::SpawnProjectile_Implementation()
{
	if (Role == ROLE_Authority && GetWorld() && projectile)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		spawnParams.Owner = userCharacter;
		spawnParams.Instigator = userCharacter;

		AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, barrel->GetSocketLocation(FName("Muzzle")), TurretCamera->GetComponentRotation(), spawnParams);

		if (spawnedProjectile)
		{
			spawnedProjectile->SetShooterController(userController);
			spawnedProjectile->FireInDirection(TurretCamera->GetForwardVector() + recoilOffset);

			userController->ClientPlayCameraShake(shotShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
		}
	}
}

bool AEOC_MachineGun::PerformRecoil_Validate()
{
	return true;
}

void AEOC_MachineGun::PerformRecoil_Implementation()
{
	AddControllerPitchInputTo(verticalRecoilRange.X);
	recoilOffset = FVector(FMath::RandRange(-sidewaysRecoilRange, sidewaysRecoilRange), FMath::RandRange(-sidewaysRecoilRange, sidewaysRecoilRange), FMath::RandRange(-sidewaysRecoilRange, sidewaysRecoilRange));
}

void AEOC_MachineGun::CheckBeforeLeaving()
{
	Super::CheckBeforeLeaving();

	StopShooting();
}