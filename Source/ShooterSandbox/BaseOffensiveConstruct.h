// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseConstruct.h"
#include "BaseOffensiveConstruct.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API ABaseOffensiveConstruct : public ABaseConstruct
{
	GENERATED_BODY()

protected:

//=#=#=#=#= VARIABLES =#=#=#=#=

	UPROPERTY(Transient, Replicated)
	class AShooterSandboxController* userController;

	UPROPERTY(Transient, Replicated)
	class AShooterSandboxCharacter* userCharacter;

public:
	ABaseOffensiveConstruct();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//=#=#=#=#= VARIABLES =#=#=#=#=

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* constructBase;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* rotatingConstructPart;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* muzzle;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* vicinity;

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	UFUNCTION(BlueprintCallable, Category = "Offensive Construct Controls")
	virtual void StartShooting();

	UFUNCTION(BlueprintCallable, Category = "Offensive Construct Controls")
	virtual void StopShooting();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Offensive Construct Controls")
	void ControlOffensive(class AShooterSandboxController* occupantController);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Offensive Construct Controls")
	void LeaveOffensive();

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
