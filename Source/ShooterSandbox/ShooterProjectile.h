// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterProjectile.generated.h"

UCLASS()
class SHOOTERSANDBOX_API AShooterProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShooterProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class AShooterSandboxController* shooterController;

public:

	FTimerHandle countdownLife;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	class USphereComponent* CollisionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	bool destroyOnImpact;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float lifetime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	int damage;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	void FireInDirection(FVector shootDirection);

	void SetShooterController(class AShooterSandboxController* shooter);

	class AShooterSandboxController* GetShooterController();

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void Server_EnableProjectileGravity();

	UFUNCTION(Server, Reliable, WithValidation)
	void DestroyProjectile();

	UFUNCTION(Server, Reliable)
	void DestroyProjectileInSeconds(float inSeconds);

	UFUNCTION(Server, Reliable)
	void InvalidateDeathTimer();

	void Local_DestroyProjectile();
};
