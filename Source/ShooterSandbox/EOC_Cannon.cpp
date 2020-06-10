// Fill out your copyright notice in the Description page of Project Settings.


#include "EOC_Cannon.h"
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

void AEOC_Cannon::StartShooting()
{	
	Super::StartShooting();

	//askPerformRecoil = true;
	burstShotsCompleted = 0;

	if (currentMode == ETurretFireMode::Primary)
	{
		SpawnProjectile();
	}

	else
	{
		if(burstShotTimer.IsValid())
		{
			GetWorldTimerManager().ClearTimer(burstShotTimer);
			burstShotTimer.Invalidate();
		}
		SpawnProjectile();
		GetWorld()->GetTimerManager().SetTimer(burstShotTimer, this, &AEOC_Cannon::BurstFire, 0.15f, true);
	}
}

void AEOC_Cannon::StopShooting()
{
	Super::StopShooting();

}

//void AEOC_Cannon::SwitchMode()
//{
//	Super::SwitchMode();
//}

void AEOC_Cannon::BurstFire()
{
	burstShotsCompleted++;
	if (burstShotsCompleted >= NUM_BURST_SHOTS)
	{
		GetWorldTimerManager().ClearTimer(burstShotTimer);
		burstShotTimer.Invalidate();
	}
	else
	{
		SpawnProjectile();
	}
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
		UKismetSystemLibrary::PrintString(this, (TEXT("SpawnProjectile")));
		AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, barrel->GetSocketLocation(FName("Muzzle")), TurretCamera->GetComponentRotation(), spawnParams);

		if (spawnedProjectile)
		{
			spawnedProjectile->SetShooterController(userController);
			//spawnedProjectile->FireInDirection(barrel->GetSocketRotation(FName("Muzzle")).Vector()); TurretCamera->GetComponentRotation()
			spawnedProjectile->FireInDirection(TurretCamera->GetForwardVector());//>GetComponentRotation().Vector());
			
			PerformRecoil();
			userController->ClientPlayCameraShake(shotShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
		}
	}
}

bool AEOC_Cannon::PerformRecoil_Validate()
{
	return true;
}

void AEOC_Cannon::PerformRecoil_Implementation()
{
	if (currentMode == ETurretFireMode::Primary)
	{
		totalRecoil.X += FMath::RandRange(verticalRecoilRange.X, verticalRecoilRange.Y);
		totalRecoil.Y += FMath::RandRange(-sidewaysRecoilRange, sidewaysRecoilRange);
	}
	else
	{
		totalRecoil.X += FMath::RandRange(verticalRecoilRange.X / 3, verticalRecoilRange.Y / 3);
		totalRecoil.Y += FMath::RandRange(-sidewaysRecoilRange / 3, sidewaysRecoilRange / 3);
	}
	recoilCount = 0;

	if (recoilProcess.IsValid())
	{
		GetWorldTimerManager().ClearTimer(recoilProcess);
		recoilProcess.Invalidate();
	}
	GetWorld()->GetTimerManager().SetTimer(recoilProcess, this, &AEOC_Cannon::RecoilRoutine, 0.016f, true);
}

void AEOC_Cannon::RecoilRoutine()
{
	if (recoilCount < shootRecoilFrames)
	{
		AddControllerPitchInputTo(totalRecoil.X / shootRecoilFrames);
		AddControllerYawInputTo(totalRecoil.Y / shootRecoilFrames);
	}

	else if (recoilCount < (shootRecoilFrames + recoilRecoveryFrames))
	{
		AddControllerPitchInputTo(-totalRecoil.X / recoilRecoveryFrames);
		AddControllerYawInputTo(-totalRecoil.Y / recoilRecoveryFrames);
	}

	else
	{
		GetWorldTimerManager().ClearTimer(recoilProcess);
		recoilProcess.Invalidate();
		totalRecoil = FVector2D::ZeroVector;
	}

	recoilCount++;
}

void AEOC_Cannon::CheckBeforeLeaving()
{
	Super::CheckBeforeLeaving();

	if (burstShotTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(burstShotTimer);
		burstShotTimer.Invalidate();
	}
}