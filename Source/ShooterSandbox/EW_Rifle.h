// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "EW_Rifle.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API AEW_Rifle : public ABaseWeapon
{
	GENERATED_BODY()
	
public:

	FTimerHandle sprayTimer;

	virtual void StartShooting() override;

	virtual void StopShooting() override;

	virtual void AlternateFire() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnProjectile();

};
