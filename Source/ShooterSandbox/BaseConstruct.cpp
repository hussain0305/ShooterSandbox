// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseConstruct.h"
#include "ShooterSandboxController.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseConstruct::ABaseConstruct()
{
	SetReplicates(true);
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseConstruct::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseConstruct::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseConstruct::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (DamageCauser == nullptr || EventInstigator == nullptr) {
		return health;
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	health -= DamageAmount;

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

void ABaseConstruct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{ 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);  
	
	DOREPLIFETIME(ABaseConstruct, health);
	DOREPLIFETIME(ABaseConstruct, constructedBy);
}