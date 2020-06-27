// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseConstruct.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseConstruct::ABaseConstruct()
{
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABaseConstruct::BeginPlay()
{
	Super::BeginPlay();
	health = maxHealth;

	RefreshAppearance();
}

// Called every frame
void ABaseConstruct::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseConstruct::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (!HasAuthority() || DamageCauser == nullptr || EventInstigator == nullptr)
	{
		return health;
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	health -= DamageAmount;

	if (health < 0)
	{
		Cast<AShooterSandboxPlayerState>(EventInstigator->PlayerState)->HasBrokenConstruct(maxHealth);
		DestroyConstruct();
	}

	return health;
}

void ABaseConstruct::SetConstructedBy(AShooterSandboxController* owner)
{
	constructedBy = owner;
}

AShooterSandboxController* ABaseConstruct::GetConstructedBy()
{
	return constructedBy;
}

void ABaseConstruct::DestroyConstruct()
{
	if (Role < ROLE_Authority) {
		return;
	}

	Destroy(this);
}

void ABaseConstruct::RefreshAppearance()
{
	TArray<UStaticMeshComponent*> allStaticMeshes;
	Cast<AActor>(this)->GetComponents<UStaticMeshComponent>(allStaticMeshes);
	
	if (allStaticMeshes.Num() == 0 || appearanceOptions.Num() == 0)
	{
		return;
	}

	allStaticMeshes[0]->SetMaterial(0, appearanceOptions[FMath::RandRange(0, appearanceOptions.Num() - 1)]);
}

void ABaseConstruct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{ 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);  
	
	DOREPLIFETIME(ABaseConstruct, health);
	DOREPLIFETIME(ABaseConstruct, constructedBy);
}