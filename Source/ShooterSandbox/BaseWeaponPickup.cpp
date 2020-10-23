// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeaponPickup.h"
#include "BaseWeapon.h"
#include "PickupSpawnArea.h"
#include "ShooterSandboxCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseWeaponPickup::ABaseWeaponPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("PickupRoot")));

	vicinity = CreateDefaultSubobject<USphereComponent>(TEXT("Vicinity"));
	vicinity->SetupAttachment(GetRootComponent());
	vicinity->SetGenerateOverlapEvents(true);
	vicinity->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	vicinity->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeaponPickup::OnOverlapBegan);
	vicinity->OnComponentEndOverlap.AddDynamic(this, &ABaseWeaponPickup::OnOverlapEnded);

}

// Called when the game starts or when spawned
void ABaseWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(theWeapon,
		FVector(0, 0, -30000),
		FRotator::ZeroRotator, spawnParams);

	GetWorld()->GetTimerManager().SetTimer(activationCountdown, this, &ABaseWeaponPickup::ActivatePickup, 1.0f, false);
}

void ABaseWeaponPickup::ActivatePickup()
{
	isActivated = true;
}

void ABaseWeaponPickup::OnOverlapBegan(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!HasAuthority() || !isActivated)
	{
		return;
	}

	if (Cast<AShooterSandboxCharacter>(OtherActor))
	{
		if (Cast<AShooterSandboxCharacter>(OtherActor)->bHasWeapon)
		{
			Cast<AShooterSandboxCharacter>(OtherActor)->Client_SendAlertMessage("You already have a weapon, hoarder");
			return;
		}
		spawnedWeapon->WasPickedUpBy(Cast<AShooterSandboxCharacter>(OtherActor));
		Cast<AShooterSandboxCharacter>(OtherActor)->Multicast_PickupOrDropWeapon(spawnedWeapon);
		Cast<AShooterSandboxCharacter>(OtherActor)->Client_PickupOrDropWeapon(true, spawnedWeapon->weaponName);

		//FTimerHandle destroyingRoutine;
		//GetWorld()->GetTimerManager().SetTimer(destroyingRoutine, this, &ABaseWeaponPickup::DestroyThyself, 0.1f, true);
		DestroyThyself();
	}
}

void ABaseWeaponPickup::OnOverlapEnded(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority()) {
		return;
	}

}

bool ABaseWeaponPickup::DestroyThyself_Validate()
{
	return true;
}

void ABaseWeaponPickup::DestroyThyself_Implementation()
{
	if (associatedSpawnArea)
	{
		associatedSpawnArea->PickupWasPickedUp();
	}
	Destroy();
}
