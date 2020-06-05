// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseConstruct.generated.h"

UCLASS()
class SHOOTERSANDBOX_API ABaseConstruct : public APawn
{
	GENERATED_BODY()
	
public:	

	ABaseConstruct();

protected:

//=#=#=#=#= VARIABLES =#=#=#=#=

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int maxHealth;

	UPROPERTY(Replicated)
	int health;

	UPROPERTY(Replicated)
	class AShooterSandboxController* constructedBy;

public:	

//=#=#=#=#= VARIABLES =#=#=#=#=

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isGridAligned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString constructName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int constructionCost;

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	void SetConstructedBy(class AShooterSandboxController* owner);

	class AShooterSandboxController* GetConstructedBy();
};
