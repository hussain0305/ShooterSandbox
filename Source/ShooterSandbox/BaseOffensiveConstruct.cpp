// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseOffensiveConstruct.h"
#include "Camera/CameraComponent.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxCharacter.h"
#include "EConstructDestruction.h"
#include "AShooterSandboxHUD.h"
#include "ShooterProjectile.h"
#include "ShooterSandboxGlobal.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABaseOffensiveConstruct::ABaseOffensiveConstruct()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;//true
	bUseControllerRotationRoll = false;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("ConstructRoot"));
	SetRootComponent(rootComp);

	baseOrientation = CreateDefaultSubobject<USceneComponent>(TEXT("BaseOrientation"));
	baseOrientation->SetupAttachment(GetRootComponent());

	constructBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Construct Base"));
	constructBase->SetupAttachment(baseOrientation);

	rotatingConstructPart = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotating Construct Part"));
	rotatingConstructPart->SetMobility(EComponentMobility::Movable);
	rotatingConstructPart->SetupAttachment(baseOrientation);
	
	barrelFulcrum = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelFulcrum"));//barrelFulcrum
	barrelFulcrum->SetMobility(EComponentMobility::Movable);
	barrelFulcrum->SetupAttachment(rotatingConstructPart);
	
	barrel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel"));//barrelFulcrum
	barrel->SetMobility(EComponentMobility::Movable);
	barrel->SetupAttachment(barrelFulcrum);

	TurretCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Turret Camera"));
	TurretCamera->SetupAttachment(barrel, FName("CameraHolder"));

	vicinity = CreateDefaultSubobject<USphereComponent>(TEXT("Vicinity"));
	vicinity->SetupAttachment(GetRootComponent());
	vicinity->SetGenerateOverlapEvents(true);
	vicinity->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	vicinity->OnComponentBeginOverlap.AddDynamic(this, &ABaseOffensiveConstruct::OnOverlapBegan);
	vicinity->OnComponentEndOverlap.AddDynamic(this, &ABaseOffensiveConstruct::OnOverlapEnded);
}

void ABaseOffensiveConstruct::BeginPlay()
{
	Super::BeginPlay();

	isBeingUsed = false;
	currentMode = ETurretFireMode::Primary;

	rotatingPartDefaultRotation = rotatingConstructPart->GetRelativeRotation();
	barrelPartDefaultRotation = barrelFulcrum->GetRelativeRotation();

	rotatingPartLowerRotationLimit = rotatingPartDefaultRotation.Pitch - 45;
	rotatingPartUpperRotationLimit = rotatingPartDefaultRotation.Pitch + 45;
	barrelLowerRotationLimit = barrelPartDefaultRotation.Yaw - 45;
	barrelUpperRotationLimit = barrelPartDefaultRotation.Yaw + 45;
}

void ABaseOffensiveConstruct::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ABaseOffensiveConstruct::Server_LeaveOffensive);
	PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &ABaseOffensiveConstruct::StartShooting);
	PlayerInputComponent->BindAction("MouseLeft", IE_Released, this, &ABaseOffensiveConstruct::StopShooting);
	PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &ABaseOffensiveConstruct::SwitchMode);

	PlayerInputComponent->BindAxis("Turn", this, &ABaseOffensiveConstruct::AddControllerYawInputTo);
	PlayerInputComponent->BindAxis("LookUp", this, &ABaseOffensiveConstruct::AddControllerPitchInputTo);
}

#pragma region Turret Orientation

void ABaseOffensiveConstruct::AddControllerPitchInputTo(float Val)
{
	barrelFulcrum->AddLocalRotation(FRotator(0, 0, Val));
	Server_AddControllerPitchInputTo(Val);
}

bool ABaseOffensiveConstruct::Server_AddControllerPitchInputTo_Validate(float Val)
{
	return true;
}

void ABaseOffensiveConstruct::Server_AddControllerPitchInputTo_Implementation(float Val)
{
	barrelFulcrum->AddLocalRotation(FRotator(0, 0, Val));
}

void ABaseOffensiveConstruct::AddControllerYawInputTo(float Val)
{
	rotatingConstructPart->AddLocalRotation(FRotator(0, Val, 0));
	Server_AddControllerYawInputTo(Val);
}

bool ABaseOffensiveConstruct::Server_AddControllerYawInputTo_Validate(float Val)
{
	return true;
}

void ABaseOffensiveConstruct::Server_AddControllerYawInputTo_Implementation(float Val)
{
	rotatingConstructPart->AddLocalRotation(FRotator(0, Val, 0));
}

#pragma endregion

void ABaseOffensiveConstruct::OnOverlapBegan(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (isBeingUsed || !HasAuthority())
	{
		return;
	}

	if (Cast<AShooterSandboxCharacter>(OtherActor) && !isBeingUsed)
	{
		Cast<AShooterSandboxCharacter>(OtherActor)->SetOffensiveConstructInVicinity(this);
	}
}

void ABaseOffensiveConstruct::OnOverlapEnded(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (isBeingUsed || !HasAuthority()) {// TEMP TEST -- REMOVE isBeingUsed || 
		return;
	}

	if (Cast<AShooterSandboxCharacter>(OtherActor)) 
	{
		Cast<AShooterSandboxCharacter>(OtherActor)->SetOffensiveConstructInVicinity(nullptr);
	}
}

void ABaseOffensiveConstruct::DestroyConstruct()
{
	if (Role < ROLE_Authority)
	{
		return;
	}

	Server_LeaveOffensive();

	if (destructionBP)
	{
		Multicast_DestroyConstruct();
	}

	Destroy(this);
}

bool ABaseOffensiveConstruct::Multicast_ControlOffensive_Validate(AShooterSandboxController * occupantController)
{
	return true;
}

void ABaseOffensiveConstruct::Multicast_ControlOffensive_Implementation(AShooterSandboxController * occupantController)
{
	isBeingUsed = true;
	
	if (occupantController == nullptr)
	{
		return;
	}

	SetOwner(occupantController);

	userController = occupantController;
	userCharacter = Cast<AShooterSandboxCharacter>(occupantController->GetCharacter());

	userController->PossessThis(this);
}

bool ABaseOffensiveConstruct::Server_LeaveOffensive_Validate()
{
	return true;
}

void ABaseOffensiveConstruct::Server_LeaveOffensive_Implementation()
{
	if (userController == nullptr) {
		return;
	}

	CheckBeforeLeaving();

	Multicast_LeaveOffensive();
}

bool ABaseOffensiveConstruct::Multicast_LeaveOffensive_Validate()
{
	return true;
}

void ABaseOffensiveConstruct::Multicast_LeaveOffensive_Implementation()
{
	isBeingUsed = false;
	rotatingConstructPart->SetRelativeRotation(rotatingPartDefaultRotation);
	barrelFulcrum->SetRelativeRotation(barrelPartDefaultRotation);

	if (userController == nullptr) {
		return;
	}

	userController->PossessThis(userCharacter);
	if (Cast<AAShooterSandboxHUD>(userController->GetHUD()))
	{
		Cast<AAShooterSandboxHUD>(userController->GetHUD())->RemoveTurretModeSwitching();
	}

	userCharacter = nullptr;
	userController = nullptr;

	totalRecoil = FVector2D::ZeroVector;
}

void ABaseOffensiveConstruct::StartShooting()
{
	keepFiring = true;
}

void ABaseOffensiveConstruct::StopShooting()
{
	keepFiring = false;
}

void ABaseOffensiveConstruct::SwitchMode()
{
	currentMode = (currentMode == ETurretFireMode::Primary) ? ETurretFireMode::Alternate : ETurretFireMode::Primary;
	
	if(userController && userController->GetHUD())
	{
		Cast<AAShooterSandboxHUD>(userController->GetHUD())->SetTurretMode((currentMode == ETurretFireMode::Primary) ? 0 : 1);
	}
}

void ABaseOffensiveConstruct::CheckBeforeLeaving()
{

}

void ABaseOffensiveConstruct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseOffensiveConstruct, userController);
	DOREPLIFETIME(ABaseOffensiveConstruct, userCharacter);
	DOREPLIFETIME(ABaseOffensiveConstruct, isBeingUsed);
}


//=#=#=#=#= CONSIDER IF THIS CAN BE INTEGRATED BACK IN THE BASE CLASS =#=#=#=#=

//bool ABaseOffensiveConstruct::PerformRecoil_Validate()
//{
//	return true;
//}
//
//void ABaseOffensiveConstruct::PerformRecoil_Implementation()
//{
//	if (currentMode == ETurretFireMode::Primary)
//	{
//		totalRecoil.X += FMath::RandRange(verticalRecoilRange.X, verticalRecoilRange.Y);
//		totalRecoil.Y += FMath::RandRange(-sidewaysRecoilRange, sidewaysRecoilRange);
//		totalRecoil.X = totalRecoil.X > 5 ? 5 : totalRecoil.X;
//		totalRecoil.Y = totalRecoil.Y > 5 ? 5 : totalRecoil.Y;
//	}
//	else
//	{
//		totalRecoil.X += FMath::RandRange(verticalRecoilRange.X / 3, verticalRecoilRange.Y / 3);
//		totalRecoil.Y += FMath::RandRange(-sidewaysRecoilRange / 3, sidewaysRecoilRange / 3);
//	}
//	recoilCount = 0;
//
//	if (recoilProcess.IsValid())
//	{
//		GetWorldTimerManager().ClearTimer(recoilProcess);
//		recoilProcess.Invalidate();
//	}
//	GetWorld()->GetTimerManager().SetTimer(recoilProcess, this, &ABaseOffensiveConstruct::RecoilRoutine, 0.016f, true);
//}

//void ABaseOffensiveConstruct::RecoilRoutine()
//{
//	if (recoilCount < shootRecoilFrames)
//	{
//		AddControllerPitchInputTo(totalRecoil.X / shootRecoilFrames);
//		AddControllerYawInputTo(totalRecoil.Y / shootRecoilFrames);
//	}
//
//	else if (recoilCount < (shootRecoilFrames + recoilRecoveryFrames))
//	{
//		AddControllerPitchInputTo(-totalRecoil.X / recoilRecoveryFrames);
//		AddControllerYawInputTo(-totalRecoil.Y / recoilRecoveryFrames);
//	}
//
//	else
//	{
//		GetWorldTimerManager().ClearTimer(recoilProcess);
//		recoilProcess.Invalidate();
//		totalRecoil = FVector2D::ZeroVector;
//	}
//
//	recoilCount++;
//}
