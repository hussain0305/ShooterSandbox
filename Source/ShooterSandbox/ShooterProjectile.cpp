// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterProjectile.h"
#include "ShooterSandboxController.h"
#include "BaseConstruct.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShooterProjectile::AShooterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	RootComponent = CollisionComponent;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 30000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
}

// Called when the game starts or when spawned
void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CollisionComponent->OnComponentHit.AddDynamic(this, &AShooterProjectile::OnProjectileHit);

	FTimerHandle countdownLife;
	GetWorld()->GetTimerManager().SetTimer(countdownLife, this, &AShooterProjectile::DestroyProjectile, lifetime, false);
}

void AShooterProjectile::FireInDirection(FVector shootDirection)
{
	ProjectileMovementComponent->Velocity = shootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AShooterProjectile::SetShooterController(AShooterSandboxController * shooter)
{
	shooterController = shooter;
}

AShooterSandboxController * AShooterProjectile::GetShooterController()
{
	return shooterController;
}

void AShooterProjectile::OnProjectileHit(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, FVector NormalImpulse, const FHitResult & Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	if (Cast<ABaseConstruct>(OtherActor))
	{
		Cast<ABaseConstruct>(OtherActor)->TakeDamage((float)damage, FDamageEvent(), GetShooterController(), GetInstigator());
	}

	if(destroyOnImpact)
	{
		DestroyProjectile();
	}
}

bool AShooterProjectile::DestroyProjectile_Validate()
{
	return true;
}

void AShooterProjectile::DestroyProjectile_Implementation()
{
	Destroy(this);
}
