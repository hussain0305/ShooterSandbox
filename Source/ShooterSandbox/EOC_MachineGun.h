// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseOffensiveConstruct.h"
#include "EOC_MachineGun.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API AEOC_MachineGun : public ABaseOffensiveConstruct
{
	GENERATED_BODY()

protected:

	virtual void CheckBeforeLeaving() override;

public:

	FVector recoilOffset;

	FTimerHandle machineGunShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<class UCameraShake> shotShake;

	virtual void StartShooting() override;

	virtual void StopShooting() override;

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Offensive Construct Controls")
	void SpawnProjectile();

	UFUNCTION(Client, Unreliable, WithValidation)
	void PerformRecoil();


};
