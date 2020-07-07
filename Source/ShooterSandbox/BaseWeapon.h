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

	const float PLAYER_WALK_SPEED = 600;
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawnedTSubclassOf<class UCameraShake> shotShake;
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* gunBody;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AShooterProjectile> projectile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* bulletTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UCameraShake> shotShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int energyPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int clipSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mobilityMultiplier;

	int currentClipSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Stationary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Walking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Running;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Jumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* referenceCam;

	class AShooterSandboxController* weilderController;
	class AShooterSandboxCharacter* weilderCharacter;

	FVector gunRecoilOffset;

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

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_WasPickedUpBy(class AShooterSandboxController* pickerController);

};
