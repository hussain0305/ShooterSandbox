// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

ATower::ATower()
{
	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("ConstructRoot"));
	SetRootComponent(rootComp);

	towerBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tower Base"));
	towerBase->SetupAttachment(GetRootComponent());
}
