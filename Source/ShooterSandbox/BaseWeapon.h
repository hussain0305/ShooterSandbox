// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class SHOOTERSANDBOX_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
/**************************
*       CONSTANTS         *
**************************/
public:	
	const float PLAYER_WALK_SPEED = 600;

/**************************
*       VARIABLES         *
**************************/

//=#=#=#=#= EDITABLE IN BLUEPRINTS =#=#=#=#=

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Stationary;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Walking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Running;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float recoil_Jumping;

//=#=#=#=#= OTHER VARIABLES =#=#=#=#=

	int currentClipSize;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* gunBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* referenceCam;

	class AShooterSandboxController* weilderController;
	class AShooterSandboxCharacter* weilderCharacter;

	FVector gunRecoilOffset;

/**************************
*       FUNCTIONS         *
**************************/

//=#=#=#=#= INHERENT FUNCTIONS (1/2) =#=#=#=#=

	ABaseWeapon();

//=#=#=#=#= USING WEAPON =#=#=#=#=

	void PerformGunRecoil();

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

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_SpawnProjectile(FVector shootDirection, FVector shootPosition, FRotator directionRotation);

//=#=#=#=#= WEAPON OWNERSHIP =#=#=#=#=

	//UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "Weapon Controls")
	void WasPickedUpBy(class AShooterSandboxCharacter* pickerCharacter);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_WasPickedUpBy(class AShooterSandboxController* pickerController);

protected:

//=#=#=#=#= INHERENT FUNCTIONS (2/2) =#=#=#=#=

	// Called when the game starts or when spawnedTSubclassOf<class UCameraShake> shotShake;
	virtual void BeginPlay() override;

};
