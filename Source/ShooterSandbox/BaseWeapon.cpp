// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Engine/SkeletalMesh.h"
#include "ShooterProjectile.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterSandboxController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
	bAlwaysRelevant = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot")));

	gunBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunBody"));
	gunBody->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	currentClipSize = clipSize;
}

void ABaseWeapon::PerformGunRecoil()
{
	switch (weilderCharacter->GetCurrentEMovementState())
	{
		case EMovementState::Stationary:
			gunRecoilOffset = FVector(FMath::RandRange(-recoil_Stationary, recoil_Stationary),
				FMath::RandRange(-recoil_Stationary, recoil_Stationary),
				FMath::RandRange(-recoil_Stationary, recoil_Stationary));
			break;

		case EMovementState::Walking:
			gunRecoilOffset = FVector(FMath::RandRange(-recoil_Walking, recoil_Walking),
				FMath::RandRange(-recoil_Walking, recoil_Walking),
				FMath::RandRange(-recoil_Walking, recoil_Walking));
			break;

		case EMovementState::Running:
			gunRecoilOffset = FVector(FMath::RandRange(-recoil_Running, recoil_Running),
				FMath::RandRange(-recoil_Running, recoil_Running),
				FMath::RandRange(-recoil_Running, recoil_Running));
			break;

		case EMovementState::Jumping:
			gunRecoilOffset = FVector(FMath::RandRange(-recoil_Jumping, recoil_Jumping),
				FMath::RandRange(-recoil_Jumping, recoil_Jumping),
				FMath::RandRange(-recoil_Jumping, recoil_Jumping));
			break;

	}

}

void ABaseWeapon::StartShooting()
{
}

void ABaseWeapon::StopShooting()
{
}

void ABaseWeapon::AlternateFire()
{
}

bool ABaseWeapon::SpendAmmo()
{
	currentClipSize--;

	if (currentClipSize >= 0)
	{
		return true;
	}
	return false;
}


//bool ABaseWeapon::WasPickedUpBy_Validate(AShooterSandboxCharacter * pickerCharacter)
//{
//	return true;
//}

void ABaseWeapon::WasPickedUpBy(AShooterSandboxCharacter * pickerCharacter)//_Implementation
{
	//Probably needs to be multicast if damage and kills dont register on all clients
	SetOwner(pickerCharacter);

	weilderCharacter = pickerCharacter;
	referenceCam = weilderCharacter->GetFollowCamera();

	weilderController = Cast<AShooterSandboxController>(pickerCharacter->GetController());
	Client_WasPickedUpBy(weilderController);
}

bool ABaseWeapon::Multicast_SpawnProjectile_Validate(FVector shootDirection, FVector shootPosition, FRotator directionRotation)
{
	return true;
}

void ABaseWeapon::Multicast_SpawnProjectile_Implementation(FVector shootDirection, FVector shootPosition, FRotator directionRotation)
{
	if (Role == ROLE_Authority)
	{
		//Already Spawned on server, don't again
		return;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, shootPosition, directionRotation, spawnParams);
	spawnedProjectile->FireInDirection(shootDirection);
}

bool ABaseWeapon::Client_WasPickedUpBy_Validate(AShooterSandboxController * pickerController)
{
	return true;
}

void ABaseWeapon::Client_WasPickedUpBy_Implementation(AShooterSandboxController * pickerController)
{
	weilderController = pickerController;
}

bool ABaseWeapon::DestroyWeapon_Validate()
{
	return true;
}

void ABaseWeapon::DestroyWeapon_Implementation()
{
	if (weilderController)
	{
		weilderController->ClientStopCameraShake(shotShake, true);
	}

	Destroy();
}


//void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ABaseWeapon, currentEnergy);
//}
