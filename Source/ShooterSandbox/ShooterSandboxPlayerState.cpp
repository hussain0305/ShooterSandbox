// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterSandboxPlayerState.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AShooterSandboxPlayerState::BeginPlay()
{
	Super::BeginPlay();

	playerScore = 0;
	health = 100;
	energy = 0;
	numConstructsConstructed = 0;
	numConstructsBroken = 0;
	kills = 0;
	deaths = 0;
}

int AShooterSandboxPlayerState::HealthChangedBy(int amount)
{
	health -= amount;
	health = health < 0 ? 0 : health;
	health = health > currentMaxHealth ? currentMaxHealth : health;

	return health;
}

int AShooterSandboxPlayerState::BalanceChangedBy(int amount)
{
	balance -= amount;
	balance = balance < 0 ? 0 : balance;
	balance = balance > currentMaxBalance ? currentMaxBalance : balance;

	return balance;
}

#pragma region Getters And Setters

void AShooterSandboxPlayerState::HasDied()
{
	deaths++;
}

int AShooterSandboxPlayerState::GetDeaths()
{
	return deaths;
}

void AShooterSandboxPlayerState::HasKilled()
{
	kills++;
}

int AShooterSandboxPlayerState::GetKills()
{
	return kills;
}

void AShooterSandboxPlayerState::HasConstructed(int cost)
{
	numConstructsConstructed++;
	playerScore += cost;
}

int AShooterSandboxPlayerState::GetNumConstructsBuilt()
{
	return numConstructsConstructed;
}

int AShooterSandboxPlayerState::GetScore()
{
	return playerScore;
}

void AShooterSandboxPlayerState::HasBrokenConstruct(int healthOfConstruct)
{
	numConstructsBroken++;
	playerScore += healthOfConstruct;
}

int AShooterSandboxPlayerState::GetNumConstructsBroken()
{
	return numConstructsBroken;
}

void AShooterSandboxPlayerState::IncrementOrDecrementEnergyBy(int amount)
{
	energy += amount;

	if (Role == ROLE_Authority)
	{
		OnRep_EnergyChanged();
	}
}

int AShooterSandboxPlayerState::GetEnergy()
{
	return energy;
}

bool AShooterSandboxPlayerState::SetMaxEnergy_Validate(int max)
{
	return true;
}

void AShooterSandboxPlayerState::SetMaxEnergy_Implementation(int max)
{
	currentMaxEnergy = max;
}

bool AShooterSandboxPlayerState::SetHealthAndBalance_Validate(int healthVal, int balanceVal)
{
	return true;
}

void AShooterSandboxPlayerState::SetHealthAndBalance_Implementation(int healthVal, int balanceVal)
{
	health = healthVal;
	currentMaxHealth = healthVal;
	balance = balanceVal;
	currentMaxBalance = balanceVal;
}

int AShooterSandboxPlayerState::GetMaxEnergy()
{
	return currentMaxEnergy;
}

void AShooterSandboxPlayerState::OnRep_EnergyChanged()
{
	if (GetOwner())
	{
		if(Cast<AShooterSandboxCharacter>(Cast<AShooterSandboxController>(GetOwner())->GetPawn()))
		{
			Cast<AShooterSandboxCharacter>(Cast<AShooterSandboxController>(GetOwner())->GetPawn())->Client_UpdateEnergyOnHUD();
		}
	}
}

#pragma endregion

void AShooterSandboxPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterSandboxPlayerState, energy, COND_OwnerOnly);

	DOREPLIFETIME(AShooterSandboxPlayerState, health);
	DOREPLIFETIME(AShooterSandboxPlayerState, balance);
	DOREPLIFETIME(AShooterSandboxPlayerState, playerScore);
	DOREPLIFETIME(AShooterSandboxPlayerState, playerNumber);
	DOREPLIFETIME(AShooterSandboxPlayerState, currentMaxEnergy);
	DOREPLIFETIME(AShooterSandboxPlayerState, numConstructsConstructed);
	DOREPLIFETIME(AShooterSandboxPlayerState, numConstructsBroken);
	DOREPLIFETIME(AShooterSandboxPlayerState, kills);
	DOREPLIFETIME(AShooterSandboxPlayerState, currentMaxHealth);
	DOREPLIFETIME(AShooterSandboxPlayerState, currentMaxBalance);
}
