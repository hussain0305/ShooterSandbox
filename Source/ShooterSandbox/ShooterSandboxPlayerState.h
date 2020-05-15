// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterSandboxPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API AShooterSandboxPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	//UPROPERTY(Transient, Replicated)
	int playerNumber;

protected:

	//UPROPERTY(Transient, Replicated)
	int playerScore;

	UPROPERTY(Transient, Replicated)
	int health;

	//UPROPERTY(Transient, Replicated)
	int energy;

	//UPROPERTY(Transient, Replicated)
	int numConstructsConstructed;

	//UPROPERTY(Transient, Replicated)
	int numConstructsBroken;

	//UPROPERTY(Transient, Replicated)
	int kills;

	//UPROPERTY(Transient, Replicated)
	int deaths;


public:

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
	void HasBrokenConstruct();

	UFUNCTION(BlueprintCallable, Category = "Round Stats")
	int GetNumConstructsBroken();

};
