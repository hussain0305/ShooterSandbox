// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseConstruct.h"
#include "Tower.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API ATower : public ABaseConstruct
{
	GENERATED_BODY()

public:
	
	ATower();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	class USceneComponent* rootComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* towerBase;
	
};
