// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurretModeSwitchScreen.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API UTurretModeSwitchScreen : public UUserWidget
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Turret Gun Controls")
	void SetFireMode(int modeNum);
};
