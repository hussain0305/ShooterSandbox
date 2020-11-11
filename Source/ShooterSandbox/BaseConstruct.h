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

//=#=#=#=#= EDITABLE IN BLUEPRINTS =#=#=#=#=

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int maxHealth;

	UPROPERTY(Replicated)
	int health;

	UPROPERTY(Replicated)
	class AShooterSandboxController* constructedBy;

public:	

//=#=#=#=#= EDITABLE IN BLUEPRINTS =#=#=#=#=

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isGridAligned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canBeParented;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString constructName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int constructionCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials)
	TArray<class UMaterialInstance*> appearanceOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool shouldForm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destruction")
	TSubclassOf<class AEConstructDestruction> destructionBP;


/**************************
*       FUNCTIONS         *
**************************/

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;

	void SetConstructedBy(class AShooterSandboxController* owner);

	class AShooterSandboxController* GetConstructedBy();

	virtual void DestroyConstruct(int lowMidHigh);

	UFUNCTION(BlueprintCallable, Category = "Materials")
	void RefreshAppearance();

	UFUNCTION(Category = "Materials")
	void FormationScaling(FVector current, FVector fullScale, float alpha);

//=#=#=#=#= SERVER FUNCTIONS =#=#=#=#=

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DestroyConstruct(int lowMidHigh);
};
