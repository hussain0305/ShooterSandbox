// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterSandboxPlayerState.h"
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

int AShooterSandboxPlayerState::TakeHealthDamage(int damageAmount)
{
	health -= damageAmount;

	return health;
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
}

int AShooterSandboxPlayerState::GetEnergy()
{
	return energy;
}

#pragma endregion

void AShooterSandboxPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterSandboxPlayerState, health);
	DOREPLIFETIME(AShooterSandboxPlayerState, playerScore);
	DOREPLIFETIME(AShooterSandboxPlayerState, playerNumber);
	DOREPLIFETIME(AShooterSandboxPlayerState, energy);
	DOREPLIFETIME(AShooterSandboxPlayerState, numConstructsConstructed);
	DOREPLIFETIME(AShooterSandboxPlayerState, numConstructsBroken);
	DOREPLIFETIME(AShooterSandboxPlayerState, kills);
}
