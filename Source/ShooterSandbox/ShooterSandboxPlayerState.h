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

	UPROPERTY(Replicated, BlueprintReadOnly)
	int playerNumber;

protected:

	UPROPERTY(Replicated, BlueprintReadOnly)
	int playerScore;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int currentMaxHealth;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int health;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int currentMaxBalance;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int balance;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int currentMaxEnergy;

	UPROPERTY(ReplicatedUsing = OnRep_EnergyChanged, BlueprintReadOnly)
	int energy;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int numConstructsConstructed;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int numConstructsBroken;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int kills;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int deaths;

	friend class AShooterSandboxCharacter;

public:


/**************************
*       FUNCTIONS         *
**************************/

	UFUNCTION(BlueprintCallable)
	int HealthChangedBy(int amount);

	UFUNCTION(BlueprintCallable)
	int BalanceChangedBy(int amount);

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

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Round Stats")
	void SetHealthAndBalance(int healthVal, int balanceVal);

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetMaxEnergy();

	UFUNCTION(BlueprintCallable, Category = "RepNotify")
	void OnRep_EnergyChanged();

};
