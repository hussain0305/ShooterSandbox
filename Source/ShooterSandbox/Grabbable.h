// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grabbable.generated.h"

UCLASS()
class SHOOTERSANDBOX_API AGrabbable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabbable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
