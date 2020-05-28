// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseOffensiveConstruct.h"
#include "Camera/CameraComponent.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxCharacter.h"
#include "ShooterProjectile.h"
#include "Components/SphereComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABaseOffensiveConstruct::ABaseOffensiveConstruct()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;//true
	bUseControllerRotationRoll = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("ConstructRoot")));

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

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ABaseOffensiveConstruct::LeaveOffensive);
	PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &ABaseOffensiveConstruct::StartShooting);
	PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &ABaseOffensiveConstruct::StartShooting);

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

bool ABaseOffensiveConstruct::StartShooting_Validate()
{
	return true;
}

void ABaseOffensiveConstruct::StartShooting_Implementation()
{
	if (Role == ROLE_Authority && GetWorld() && projectile)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
		spawnParams.Owner = userCharacter;
		spawnParams.Instigator = userCharacter;

		AShooterProjectile* spawnedProjectile = GetWorld()->SpawnActor<AShooterProjectile>(projectile, barrel->GetSocketLocation(FName("Muzzle")),  barrel->GetSocketRotation(FName("Muzzle")), spawnParams);

		if (spawnedProjectile)
		{
			spawnedProjectile->FireInDirection(barrel->GetSocketRotation(FName("Muzzle")).Vector());
		}

	}
}

void ABaseOffensiveConstruct::StopShooting()
{

}

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
	if (!HasAuthority()) {
		return;
	}

	if (Cast<AShooterSandboxCharacter>(OtherActor)) 
	{
		Cast<AShooterSandboxCharacter>(OtherActor)->SetOffensiveConstructInVicinity(nullptr);
	}
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
	//userCharacter->GetFollowCamera()->AttachToComponent(muzzle, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("CameraHolder"));
}

bool ABaseOffensiveConstruct::LeaveOffensive_Validate()
{
	return true;
}

void ABaseOffensiveConstruct::LeaveOffensive_Implementation()
{
	if (userController == nullptr) {
		return;
	}

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
	//userCharacter->ReattachCamera(userCharacter->GetFollowCamera());

	userCharacter = nullptr;
	userController = nullptr;

}

void ABaseOffensiveConstruct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseOffensiveConstruct, userController);
	DOREPLIFETIME(ABaseOffensiveConstruct, userCharacter);
	DOREPLIFETIME(ABaseOffensiveConstruct, isBeingUsed);
}
