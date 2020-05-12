// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterSandboxPlayerState.h"
#include "Net/UnrealNetwork.h"

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

void AShooterSandboxPlayerState::HasConstructed()
{
	numConstructsConstructed++;
}

int AShooterSandboxPlayerState::GetNumConstructsBuilt()
{
	return numConstructsConstructed;
}

void AShooterSandboxPlayerState::HasBrokenConstruct()
{
	numConstructsBroken++;
}

int AShooterSandboxPlayerState::GetNumConstructsBroken()
{
	return numConstructsBroken;
}

#pragma endregion

void AShooterSandboxPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterSandboxPlayerState, health);
	//DOREPLIFETIME(AShooterSandboxPlayerState, playerScore);
	//DOREPLIFETIME(AShooterSandboxPlayerState, playerNumber);
	//DOREPLIFETIME(AShooterSandboxPlayerState, energy);
	//DOREPLIFETIME(AShooterSandboxPlayerState, numConstructsConstructed);
	//DOREPLIFETIME(AShooterSandboxPlayerState, numConstructsBroken);
	//DOREPLIFETIME(AShooterSandboxPlayerState, kills);
}
