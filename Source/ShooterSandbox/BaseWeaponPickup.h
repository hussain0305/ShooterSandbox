// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeaponPickup.generated.h"

UCLASS()
class SHOOTERSANDBOX_API ABaseWeaponPickup : public AActor
{
	GENERATED_BODY()
	
/**************************
*       VARIABLES         *
**************************/

public:	

//=#=#=#=#= MISC VARIABLES =#=#=#=#=

	FTimerHandle activationCountdown;

	class APickupSpawnArea* associatedSpawnArea;
	class ABaseWeapon* spawnedWeapon;

	bool isActivated = false;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* vicinity;

//=#=#=#=#= EDITABLE IN BLUEPRINTS =#=#=#=#=

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class ABaseWeapon> theWeapon;

/**************************
*       FUNCTIONS         *
**************************/

//=#=#=#=#= WEAPON PICKUP FUNCTIONS =#=#=#=#=

	void ActivatePickup();

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegan(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapEnded(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void DestroyThyself();

//=#=#=#=#= INHERENT FUNCTIONS =#=#=#=#=
	ABaseWeaponPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
