// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeaponPickup.generated.h"

UCLASS()
class SHOOTERSANDBOX_API ABaseWeaponPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeaponPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	FTimerHandle activationCountdown;

	bool isActivated = false;

	void ActivatePickup();
	//Just a little Dev easter egg
	class APickupSpawnArea* miMaestro;
	class ABaseWeapon* spawnedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class ABaseWeapon> theWeapon;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* vicinity;

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegan(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapEnded(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void DestroyThyself();
};
