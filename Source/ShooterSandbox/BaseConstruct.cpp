// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseConstruct.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "EConstructDestruction.h"
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
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (destructionBP)
	{
		Multicast_DestroyConstruct();
	}

	Destroy(this);
}

void ABaseConstruct::Multicast_DestroyConstruct_Implementation()
{
	TArray<UStaticMeshComponent*> allStaticMeshes;
	Cast<AActor>(this)->GetComponents<UStaticMeshComponent>(allStaticMeshes);

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEConstructDestruction* spawnedDestruction = GetWorld()->SpawnActor<AEConstructDestruction>(destructionBP,
		GetActorLocation() + allStaticMeshes[0]->GetRelativeLocation(),
		GetActorRotation(), spawnParams);

	spawnedDestruction->DestroyConstruct(allStaticMeshes[0]->GetRelativeScale3D(), Cast<UMaterialInstance>(allStaticMeshes[0]->GetMaterial(0)), destructionExplosionStrength, scaleFactor);

}

void ABaseConstruct::RefreshAppearance()
{
	if (shouldForm && Role == ROLE_Authority)
	{
		FormationScaling(FVector(0, 0, 0), Cast<AActor>(this)->GetActorScale3D(), 0.0f);
	}

	TArray<UActorComponent*> mainMesh;
	mainMesh = Cast<AActor>(this)->GetComponentsByTag(UStaticMeshComponent::StaticClass(), FName("MainMesh"));

	if (mainMesh.Num() != 0)
	{
		if (Cast<UStaticMeshComponent>(mainMesh[0]))
		{
			Cast<UStaticMeshComponent>(mainMesh[0])->SetMaterial(0, appearanceOptions[FMath::RandRange(0, appearanceOptions.Num() - 1)]);
		}

		return;
	}

	TArray<UStaticMeshComponent*> allStaticMeshes;
	Cast<AActor>(this)->GetComponents<UStaticMeshComponent>(allStaticMeshes);
	
	if (allStaticMeshes.Num() == 0 || appearanceOptions.Num() == 0)
	{
		return;
	}

	allStaticMeshes[0]->SetMaterial(0, appearanceOptions[FMath::RandRange(0, appearanceOptions.Num() - 1)]);

}

void ABaseConstruct::FormationScaling(FVector current, FVector fullScale, float alpha)
{
	Cast<AActor>(this)->SetActorScale3D(FMath::Lerp(current, fullScale, alpha));

	if (alpha < 1)
	{
		FTimerHandle scalingUp;
		FTimerDelegate scalingDelegate;

		scalingDelegate.BindUFunction(this, FName("FormationScaling"), Cast<AActor>(this)->GetActorScale3D(), fullScale, alpha + 0.05f);
		GetWorld()->GetTimerManager().SetTimer(scalingUp, scalingDelegate, 0.025f, false);
	}
}

void ABaseConstruct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{ 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);  
	
	DOREPLIFETIME(ABaseConstruct, health);
	DOREPLIFETIME(ABaseConstruct, constructedBy);
}