// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseConstruct.generated.h"

UCLASS()
class SHOOTERSANDBOX_API ABaseConstruct : public AActor
{
	GENERATED_BODY()
	
public:	

	ABaseConstruct();

protected:

//=#=#=#=#= VARIABLES =#=#=#=#=

	UPROPERTY(Replicated)
	int health;

	UPROPERTY(Replicated)
	class AShooterSandboxController* ownerController;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isGridAligned;

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	void SetConstructOwner(class AShooterSandboxController* owner);

	class AShooterSandboxController* GetConstructOwner();
};
