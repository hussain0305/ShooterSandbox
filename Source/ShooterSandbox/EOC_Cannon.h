// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOffensiveConstruct.h"
#include "EOC_Cannon.generated.h"

UCLASS()
class SHOOTERSANDBOX_API AEOC_Cannon : public ABaseOffensiveConstruct
{
	GENERATED_BODY()
	
protected:

	virtual void CheckBeforeLeaving() override;

public:
	const int NUM_BURST_SHOTS = 3;

	int burstShotsCompleted = 0;

	FTimerHandle burstShotTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<class UCameraShake> shotShake;

	virtual void StartShooting() override;

	virtual void StopShooting() override;

	void BurstFire();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Offensive Construct Controls")
	void SpawnProjectile();

	UFUNCTION(Client, Unreliable, WithValidation)
	void PerformRecoil();

	void RecoilRoutine();

};
