// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class SHOOTERSANDBOX_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* gunBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AShooterProjectile> projectile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int energyPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int clipSize;

	int currentClipSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* referenceCam;

	UFUNCTION(BlueprintCallable, Category = "Weapon Controls")//Server, Reliable, WithValidation, 
	virtual void StartShooting();

	UFUNCTION(BlueprintCallable, Category = "Weapon Controls")
	virtual void StopShooting();

	UFUNCTION(BlueprintCallable, Category = "Weapon Controls")
	virtual void AlternateFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon Controls")
	bool SpendAmmo();

	UFUNCTION(Server, Reliable, WithValidation)
	void DestroyWeapon();

	//UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "Weapon Controls")
	void WasPickedUpBy(class AShooterSandboxCharacter* pickerCharacter);

};
