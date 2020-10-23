// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterProjectile.h"
#include "Grabbable.generated.h"


UCLASS()
class SHOOTERSANDBOX_API AGrabbable : public AShooterProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabbable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	class ALevelGrabbablesManager* grabManager;
	/*UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* body;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float lifetime = 10.0f;*/

	//FTimerHandle deathCountdown;

	/*void TurnEthereal();

	void SelfDestruction();*/
};
