// Fill out your copyright notice in the Description page of Project Settings.


#include "EW_Rifle.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxGlobal.h"
#include "ShooterProjectile.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

void AEW_Rifle::StartShooting()
{
	if (weilderController)
	{
		weilderController->ClientPlayCameraShake(shotShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
	}

	GetWorld()->GetTimerManager().SetTimer(sprayTimer, this, &AEW_Rifle::SpawnProjectile, 0.1f, true);
}

void AEW_Rifle::StopShooting()
{
	if (weilderController)
	{
		weilderController->ClientStopCameraShake(shotShake, true);
	}

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
		if (SpendAmmo()) {
			FActorSpawnParameters spawnParams;
			spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			spawnParams.Owner = GetOwner();
			spawnParams.Instigator = Cast<AShooterSandboxCharacter>(GetOwner())->GetMyController()->GetPawn();

			FVector muzzlePos = gunBody->GetSocketLocation(FName("Muzzle"));
			FVector shootDirec = referenceCam->GetForwardVector() + gunRecoilOffset;
			PerformGunRecoil();

			AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, muzzlePos, GetActorRotation(), spawnParams);

			if (spawnedProjectile)
			{
				spawnedProjectile->SetShooterController(weilderController);
				spawnedProjectile->FireInDirection(shootDirec);
				
				Multicast_SpawnProjectile(shootDirec, muzzlePos, GetActorRotation());

				weilderCharacter->Client_UpdateWeaponAmmo(currentClipSize, clipSize);
				
				//UGameplayStatics::SpawnEmitterAtLocation(this, bulletTrail, gunBody->GetSocketLocation(FName("Muzzle")), GetActorRotation());
			}

			if (currentClipSize == 0)
			{
				weilderCharacter->Multicast_PickupOrDropWeapon(nullptr);
				weilderCharacter->Client_PickupOrDropWeapon(false, FString(" "));
				DestroyWeapon();
			}
		}
	}
}
