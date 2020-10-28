// Fill out your copyright notice in the Description page of Project Settings.
#include "EnergyPack.h"
#include "ShooterSandboxCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "ShooterSandboxCharacter.h"

// Sets default values
AEnergyPack::AEnergyPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);
	SetReplicateMovement(true);

	boxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	boxCollider->SetupAttachment(GetRootComponent());
	boxCollider->SetSimulatePhysics(false);

	energyPackBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnergyPackBody"));
	energyPackBody->SetupAttachment(boxCollider);
	energyPackBody->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	energyPackBody->SetSimulatePhysics(false);

	vicinity = CreateDefaultSubobject<USphereComponent>(TEXT("Vicinity"));
	vicinity->SetupAttachment(boxCollider);
	vicinity->SetGenerateOverlapEvents(true);
	vicinity->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

// Called when the game starts or when spawned
void AEnergyPack::BeginPlay()
{
	Super::BeginPlay();

	SetCantBePicked();

	vicinity->OnComponentBeginOverlap.AddDynamic(this, &AEnergyPack::OnOverlapBegan);
	vicinity->OnComponentEndOverlap.AddDynamic(this, &AEnergyPack::OnOverlapEnded);
}

void AEnergyPack::SetCanBePicked()
{
	canBePicked = true;
}

void AEnergyPack::SetCantBePicked()
{
	canBePicked = false;
}


void AEnergyPack::ScheduleReactivation()
{
	GetWorld()->GetTimerManager().SetTimer(packReactivation, this, &AEnergyPack::SetCanBePicked, 4.0f, false);
}

void AEnergyPack::OnOverlapBegan(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!HasAuthority() || !Cast<AShooterSandboxCharacter>(OtherActor))
	{
		return;
	}

	if (!canBePicked)
	{
		return;
	}

	if (Cast<AShooterSandboxCharacter>(OtherActor) != Cast<AShooterSandboxCharacter>(GetOwner())) {
		return;
	}

	SetCantBePicked();
	Cast<AShooterSandboxCharacter>(GetOwner())->Server_PickupEnergyPack(this);
}

void AEnergyPack::OnOverlapEnded(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{

}



void AEnergyPack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnergyPack, canBePicked);
}