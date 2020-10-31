// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterProjectile.h"
#include "Grabbable.generated.h"


UCLASS()
class SHOOTERSANDBOX_API AGrabbable : public AShooterProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabbable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	const float MOTION_PULSE = 0.016f;
	const float STANDARD_GRABBABLE_LIFETIME = 4.0f;

	float grabLerpAlpha;
	class AShooterSandboxCharacter* grabberCharacter;
	FTimerHandle grabMotion;

	UPROPERTY(Replicated)
	bool isThrowable;

	class ALevelGrabbablesManager* grabManager;
	
	UFUNCTION(NetMulticast, Reliable)
	void SetGrabManager(class ALevelGrabbablesManager* grabMan);

	void ApplyProjectileCollisionSettings();

	UFUNCTION(NetMulticast, Reliable)
	void MoveTowardsSocket(class AShooterSandboxCharacter* grabber);

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyVisualChangesUponGrab();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyVisualChangesUponFire();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_ApplyProjectileCollisionSettings();

	void GrabMotion();

	void CancelGrabMotion();
};
