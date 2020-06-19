// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "Engine/SkeletalMesh.h"
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
	referenceCam = Cast<AShooterSandboxCharacter>(pickerCharacter)->GetFollowCamera();
}

bool ABaseWeapon::DestroyWeapon_Validate()
{
	return true;
}

void ABaseWeapon::DestroyWeapon_Implementation()
{
	Destroy();
}


//void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ABaseWeapon, currentEnergy);
//}
