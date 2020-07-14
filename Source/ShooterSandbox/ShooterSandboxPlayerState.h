// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterSandboxPlayerState.generated.h"


UCLASS()
class SHOOTERSANDBOX_API AShooterSandboxPlayerState : public APlayerState
{
	GENERATED_BODY()

/***********************************
*       INHERENT FUNCTIONS         *
***********************************/

public:
	void BeginPlay() override;

/**##**##**##**##**##**##**##*
*       PLAYER STATS         *
**##**##**##**##**##**##**##*/

//=#=#=#=#= REPLICATED VARIABLES =#=#=#=#=

	UPROPERTY(Replicated)
	int playerNumber;

protected:

	UPROPERTY(Replicated)
	int playerScore;

	UPROPERTY(Replicated)
	int health;

	UPROPERTY(Replicated)
	int currentMaxEnergy;

	UPROPERTY(ReplicatedUsing = OnRep_EnergyChanged)
	int energy;

	UPROPERTY(Replicated)
	int numConstructsConstructed;

	UPROPERTY(Replicated)
	int numConstructsBroken;

	UPROPERTY(Replicated)
	int kills;

	UPROPERTY(Replicated)
	int deaths;

public:


/**************************
*       FUNCTIONS         *
**************************/

	UFUNCTION(BlueprintCallable)
	int TakeHealthDamage(int damageAmount);

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	void HasDied();

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetDeaths();
	
	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	void HasKilled();
	
	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetKills();

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	void HasConstructed(int cost);
	
	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetNumConstructsBuilt();

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetScore();

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	void HasBrokenConstruct(int healthOfConstruct);

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetNumConstructsBroken();

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	void IncrementOrDecrementEnergyBy(int amount);

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetEnergy();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Round Stats")
	void SetMaxEnergy(int max);

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetMaxEnergy();

	UFUNCTION(BlueprintCallable, Category = "RepNotify")
	void OnRep_EnergyChanged();

};
