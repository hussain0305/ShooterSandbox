// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseConstruct.h"
#include "BaseOffensiveConstruct.generated.h"

enum class ETurretFireMode : uint8;

UCLASS()
class SHOOTERSANDBOX_API ABaseOffensiveConstruct : public ABaseConstruct
{
	GENERATED_BODY()

protected:

/**************************
*       VARIABLES         *
**************************/

//=#=#=#=#= REPLICATED AND STATE VARIABLES =#=#=#=#=

	UPROPERTY(Transient, Replicated, BlueprintReadOnly)
	class AShooterSandboxController* userController;

	UPROPERTY(Transient, Replicated)
	class AShooterSandboxCharacter* userCharacter;

	UPROPERTY(Transient, Replicated)
	bool isBeingUsed;

	UPROPERTY(BlueprintReadWrite)
	bool keepFiring = false;

public:

//=#=#=#=#= EDITABLE IN BLUEPRINTS =#=#=#=#=

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	class USceneComponent* rootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	int shootRecoilFrames = 45;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	int recoilRecoveryFrames = 60;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float maxRecoilPerFrame = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AShooterProjectile> projectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ToolTip = "The function of this value depends on the construct's specific recoil implementation. Name might not be self-explanatory."))
	FVector2D verticalRecoilRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ToolTip = "The function of this value depends on the construct's specific recoil implementation. Name might not be self-explanatory."))
	float sidewaysRecoilRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UTurretModeSwitchScreen> switchScreen;

//=#=#=#=#= RECOIL =#=#=#=#=
	
	int recoilCount;

	FTimerHandle recoilProcess;

//=#=#=#=#= ROTATION AND SETUP RELATED VARIABLES =#=#=#=#=

	UPROPERTY(BlueprintReadWrite)
	FVector2D totalRecoil;

	FRotator rotatingPartDefaultRotation;
	FRotator barrelPartDefaultRotation;

	ETurretFireMode currentMode;

	float rotatingPartLowerRotationLimit;
	float rotatingPartUpperRotationLimit;
	float barrelLowerRotationLimit;
	float barrelUpperRotationLimit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TurretCamera;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* baseOrientation;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* constructBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* rotatingConstructPart;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* barrel;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* barrelFulcrum;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* vicinity;


/**************************
*       FUNCTIONS         *
**************************/

//=#=#=#=#= INHERENT FUNCTIONS =#=#=#=#=

	ABaseOffensiveConstruct();

	void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
//=#=#=#=#= TURRET ROTATION FUNCTIONS =#=#=#=#=

	void AddControllerPitchInputTo(float Val);
	void AddControllerYawInputTo(float Val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddControllerPitchInputTo(float Val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AddControllerYawInputTo(float Val);

//=#=#=#=#= CONTROL AND USAGE FUNCTION =#=#=#=#=

	UFUNCTION(BlueprintCallable, Category = "Offensive Construct Controls")//Server, Reliable, WithValidation, 
	virtual void StartShooting();

	UFUNCTION(BlueprintCallable, Category = "Offensive Construct Controls")
	virtual void StopShooting();

	UFUNCTION(BlueprintCallable, Category = "Offensive Construct Controls")
	virtual void SwitchMode();

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "Offensive Construct Controls")
	void Multicast_ControlOffensive(class AShooterSandboxController* occupantController);

	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Offensive Construct Controls")
	void Server_LeaveOffensive();

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable, Category = "Offensive Construct Controls")
	void Multicast_LeaveOffensive();

//=#=#=#=#= FUNCTIONS =#=#=#=#=

	virtual void CheckBeforeLeaving();

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegan(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapEnded(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DestroyConstruct(int lowMidHigh) override;

	FORCEINLINE bool GetIsBeingUsed() const { return isBeingUsed; }
};


//=#=#=#=#= CONSIDER IF THIS CAN BE INTEGRATED BACK IN THE BASE CLASS =#=#=#=#=

	//UFUNCTION(Client, Unreliable, WithValidation)
	//void PerformRecoil();

	//void RecoilRoutine();
