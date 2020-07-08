// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyPack.generated.h"

UCLASS()
class SHOOTERSANDBOX_API AEnergyPack : public AActor
{
	GENERATED_BODY()
	
/***********************************
*       INHERENT FUNCTIONS         *
***********************************/

public:	
	// Sets default values for this actor's properties
	AEnergyPack();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

/**************************
*       VARIABLES         *
**************************/

//=#=#=#=#= STATE RELATED =#=#=#=#=

	FTimerHandle packReactivation;

	UPROPERTY(Replicated)
	bool canBePicked;

//=#=#=#=#= PHYSICAL SETUP =#=#=#=#=

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* energyPackBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* boxCollider;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* vicinity;


/**************************
*       FUNCTIONS         *
**************************/
	
	UFUNCTION(BlueprintCallable)
	void SetCanBePicked();

	UFUNCTION(BlueprintCallable)
	void SetCantBePicked();

	UFUNCTION(BlueprintCallable)
	void ScheduleReactivation();

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegan(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapEnded(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
