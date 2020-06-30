#include "ShooterSandboxCharacter.h"
#include "AShooterSandboxHUD.h"
#include "ShooterSandboxPlayerState.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxGlobal.h"
#include "ConstructibleSurface.h"
#include "ConstructibleWall.h"
#include "BaseConstruct.h"
#include "BaseOffensiveConstruct.h"
#include "BaseWeapon.h"
#include "EnergyPack.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

AShooterSandboxCharacter::AShooterSandboxCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	SetReplicates(true);
	//SetReplicatingMovement(true);
	SetReplicateMovement(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	//GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 150.0f;
	CameraBoom->SocketOffset = FVector(0, 50, 75);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bIsRunning = false;
	walkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void AShooterSandboxCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterSandboxCharacter::CombinedJumpJetpackOn);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShooterSandboxCharacter::CombinedJumpJetpackOff);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterSandboxCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShooterSandboxCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AShooterSandboxCharacter::ToggleRunOn);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AShooterSandboxCharacter::ToggleRunOff);

	PlayerInputComponent->BindAction("ConstructionMenu", IE_Pressed, this, &AShooterSandboxCharacter::ToggleConstructionMenu);
	PlayerInputComponent->BindAction("QuickConstruct", IE_Pressed, this, &AShooterSandboxCharacter::TryQuickConstruct);//TryQuickConstruct

	PlayerInputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &AShooterSandboxCharacter::MouseWheelDown);
	PlayerInputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &AShooterSandboxCharacter::MouseWheelUp);

	PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &AShooterSandboxCharacter::StartWeaponFire);
	PlayerInputComponent->BindAction("MouseLeft", IE_Released, this, &AShooterSandboxCharacter::StopWeaponFire);
	PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &AShooterSandboxCharacter::WeaponAltMode);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AShooterSandboxCharacter::AttemptControlOffensiveConstruct);
	PlayerInputComponent->BindAction("SwitchConstructionMode", IE_Pressed, this, &AShooterSandboxCharacter::SwitchConstructionMode);

	PlayerInputComponent->BindAction("TestData", IE_Pressed, this, &AShooterSandboxCharacter::PrintTestData);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterSandboxCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterSandboxCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterSandboxCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterSandboxCharacter::LookUpAtRate);
}

void AShooterSandboxCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	currentConstructionMode = EConstructionMode::Surface;

	currentMovementState = EMovementState::Stationary;
	GetWorld()->GetTimerManager().SetTimer(movementStateMonitoring, this, &AShooterSandboxCharacter::MonitorMovementState, 0.25f, true);

	FTimerHandle fetchComponents;
	GetWorld()->GetTimerManager().SetTimer(fetchComponents, this, &AShooterSandboxCharacter::FetchPersistentComponents, 0.5f, false);
}

void AShooterSandboxCharacter::OnRep_PlayerState()
{
	myController = Cast<AShooterSandboxController>(GetController());
	myPlayerState = Cast<AShooterSandboxPlayerState>(GetPlayerState());
	if (myController)
	{
		myHUD = Cast<AAShooterSandboxHUD>(myController->GetHUD());
	}
}

//The need for this function can be eliminated if there's a way to trigger OnRep_PlayerState on server
void AShooterSandboxCharacter::FetchPersistentComponents()
{
	myController = Cast<AShooterSandboxController>(GetController());
	myPlayerState = Cast<AShooterSandboxPlayerState>(GetPlayerState());
	if (myController)
	{
		myHUD = Cast<AAShooterSandboxHUD>(myController->GetHUD());
	}
	if (myHUD)
	{
		myHUD->SetConstructionMode(currentConstructionMode);
	}
}

#pragma region Movement Related

void AShooterSandboxCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterSandboxCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AShooterSandboxCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AShooterSandboxCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AShooterSandboxCharacter::MonitorMovementState()
{
	if (GetCharacterMovement()->IsFalling())
	{
		SetCurrentEMovementState(EMovementState::Jumping);
	}
	else if (GetCharacterMovement()->Velocity.Size() < 10.0f)
	{
		SetCurrentEMovementState(EMovementState::Stationary);
	}
	else if (GetCharacterMovement()->Velocity.Size() < (walkSpeed + 10.0f))
	{
		SetCurrentEMovementState(EMovementState::Walking);
	}
	else
	{
		SetCurrentEMovementState(EMovementState::Running);
	}
}

void AShooterSandboxCharacter::SetCurrentEMovementState(EMovementState newState)
{
	if (currentMovementState == EMovementState::Running && newState != EMovementState::Running)
	{
		ToggleRunCamShake(false);
	}
	else if (currentMovementState != EMovementState::Running && newState == EMovementState::Running)
	{
		ToggleRunCamShake(true);
	}
	currentMovementState = newState;
}

EMovementState AShooterSandboxCharacter::GetCurrentEMovementState()
{
	return currentMovementState;
}

void AShooterSandboxCharacter::ToggleCrouch()
{
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Crouch();
	}
}

void AShooterSandboxCharacter::ToggleRunOn()
{
	//Only continue if player isn't already running and doesn't have ADS
	if (bIsRunning) {
		return;
	}

	bIsRunning = true;

	GetCharacterMovement()->MaxWalkSpeed = RUN_MULTIPLIER * walkSpeed;
	Server_ToggleRun(RUN_MULTIPLIER * walkSpeed);
}

void AShooterSandboxCharacter::ToggleRunOff()
{
	//Only continue if player is already running
	if (!bIsRunning) {
		return;
	}

	bIsRunning = false;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
	Server_ToggleRun(walkSpeed);
}

void AShooterSandboxCharacter::ToggleRunCamShake(bool startShake)
{
	if(!myController)
	{
		return;
	}

	if (startShake)
	{
		myController->ClientPlayCameraShake(runCamShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
	}
	else
	{
		myController->ClientStopCameraShake(runCamShake);
	}
}

void AShooterSandboxCharacter::CombinedJumpJetpackOn()
{
	if (!GetCharacterMovement()->IsFalling()) {
		Jump();
		return;
	}

	if (GetWorld())
	{
		jetpackActive = true;
		Jetpack();
		GetWorld()->GetTimerManager().SetTimer(jetpack, this, &AShooterSandboxCharacter::Jetpack, 1.f, true);
	}
}

void AShooterSandboxCharacter::CombinedJumpJetpackOff()
{
	StopJumping();

	jetpackActive = false;
	if (jetpack.IsValid())
	{
		GetWorldTimerManager().ClearTimer(jetpack);
		jetpack.Invalidate();
	}
}

bool AShooterSandboxCharacter::Server_ToggleRun_Validate(float newSpeed)
{
	return true;
}

void AShooterSandboxCharacter::Server_ToggleRun_Implementation(float newSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

bool AShooterSandboxCharacter::Jetpack_Validate()
{
	return true;
}

void AShooterSandboxCharacter::Jetpack_Implementation()
{
	if (myPlayerState->GetEnergy() >= JETPACK_THRUST_COST)
	{
		GetCharacterMovement()->Velocity += FVector(0, 0, jetpackThrust);
		Server_SpendEnergy(JETPACK_THRUST_COST, GetWorld()->GetAuthGameMode<AShooterSandboxGameMode>()->MAX_ENERGY_AMOUNT);
		Client_EnergyNotification("Jetpack Thrust", JETPACK_THRUST_COST, 1);
	}
	else
	{
		PlayerOutOfEnergy();
	}
}

#pragma endregion


void AShooterSandboxCharacter::MouseWheelDown()
{
	if (!myController)
	{
		return;
	}

	myHUD->ScrollDownList();
}

void AShooterSandboxCharacter::MouseWheelUp()
{
	if (!myController)
	{
		return;
	}

	myHUD->ScrollUpList();
}

bool AShooterSandboxCharacter::GetSpawnLocationAndRotation(FVector &spawnLocation, FRotator &spawnRotation, class AConstructibleSurface* &surfaceToSpawnOn, TSubclassOf<class ABaseConstruct> construct)
{
	if (!GetWorld())
	{
		return false;
	}

	surfaceToSpawnOn = nullptr;

	if (currentConstructionMode == EConstructionMode::Surface)
	{
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(this);

		FHitResult hit;

		if (GetWorld()->LineTraceSingleByObjectType(hit, FollowCamera->GetComponentLocation(),
			(FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * BUILD_DISTANCE)),
			ECC_GameTraceChannel1, traceParams)) {

			if (Cast<AConstructibleSurface>(hit.Actor))
			{
				if (Cast<AConstructibleSurface>(hit.Actor)->requiresParenting && !construct.GetDefaultObject()->canBeParented)
				{
					myHUD->ShowNotificationMessage("Can't construct " + construct.GetDefaultObject()->constructName + " on " + Cast<AConstructibleSurface>(hit.Actor)->constructName);
					return false;
				}
				spawnLocation = hit.ImpactPoint;

				float gridSize = Cast<AConstructibleSurface>(hit.Actor)->gridSizeInUnits;
				int gridNoX = spawnLocation.X / gridSize;
				int gridNoY = spawnLocation.Y / gridSize;

				float gridAlignedX = (gridNoX * gridSize) + (spawnLocation.X < 0 ? -gridSize / 2 : gridSize / 2);
				float gridAlignedY = (gridNoY * gridSize) + (spawnLocation.Y < 0 ? -gridSize / 2 : gridSize / 2);

				spawnLocation = FVector(gridAlignedX, gridAlignedY, spawnLocation.Z);
				spawnRotation = GetActorRotation();
				surfaceToSpawnOn = Cast<AConstructibleSurface>(hit.Actor);

				return true;
			}
			else
			{
				myHUD->ShowNotificationMessage("Aim at a constructible floor to build");
			}
		}

		else
		{
			myHUD->ShowNotificationMessage("Aim at a constructible floor nearby to build");
		}

	}

	else //if construction mode == wall
	{
		FCollisionQueryParams traceParams;
		traceParams.AddIgnoredActor(this);

		FHitResult hit;

		if (GetWorld()->LineTraceSingleByObjectType(hit, FollowCamera->GetComponentLocation(),
			(FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * BUILD_DISTANCE)),
			ECC_GameTraceChannel7, traceParams)) {

			if (Cast<AConstructibleWall>(hit.Actor))
			{
				spawnLocation = hit.ImpactPoint;

				float gridSize = Cast<AConstructibleWall>(hit.Actor)->gridSizeInUnits;
				float padding = Cast<AConstructibleWall>(hit.Actor)->gridBuildPadding;

				//if HitNormal has X=1, positioning will be along X
				//if HitNormal has Y=1, positioning will be along Y

				int gridNoZ = spawnLocation.Z / gridSize;
				float gridAlignedZ = (gridNoZ * gridSize) + (spawnLocation.Z < 0 ? -gridSize / 2 : gridSize / 2);

				int gridNoXY;
				float gridAlignedXY;

				if (hit.ImpactNormal.Y == 1 || hit.ImpactNormal.Y == -1)
				{
					gridNoXY = spawnLocation.X / gridSize;
					gridAlignedXY = (gridNoXY * gridSize) + (spawnLocation.X < 0 ? -gridSize / 2 : gridSize / 2);
					spawnLocation = FVector(gridAlignedXY, spawnLocation.Y - (hit.ImpactNormal.Y * padding), gridAlignedZ);
				}
				else
				{
					gridNoXY = spawnLocation.Y / gridSize;
					gridAlignedXY = (gridNoXY * gridSize) + (spawnLocation.Y < 0 ? -gridSize / 2 : gridSize / 2);
					spawnLocation = FVector(spawnLocation.X - (hit.ImpactNormal.X * padding), gridAlignedXY, gridAlignedZ);
				}

				spawnRotation = hit.ImpactNormal.Rotation();

				return true;
			}
			else
			{
				myHUD->ShowNotificationMessage("Aim at a constructible wall to build");
			}
		}

		else
		{
			myHUD->ShowNotificationMessage("Aim at a constructible wall nearby to build");
		}

	}

	spawnLocation = FVector::ZeroVector;
	spawnRotation = FRotator::ZeroRotator;

	return false;
}

bool AShooterSandboxCharacter::Server_AddEnergy_Validate(int amount, int maxEnergy)
{
	return true;
}

void AShooterSandboxCharacter::Server_AddEnergy_Implementation(int amount, int maxEnergy)
{
	if (!myEnergyPack || !myPlayerState || !myController)
	{
		return;
	}

	myPlayerState->SetMaxEnergy(maxEnergy);
	int currentEnergy = myPlayerState->GetEnergy();

	if (currentEnergy >= maxEnergy)
	{
		return;
	}

	myPlayerState->IncrementOrDecrementEnergyBy((currentEnergy + amount > maxEnergy) ? (maxEnergy - currentEnergy) : amount);
}

bool AShooterSandboxCharacter::Server_SpendEnergy_Validate(int amount, int maxEnergy)
{
	return true;
}

void AShooterSandboxCharacter::Server_SpendEnergy_Implementation(int amount, int maxEnergy)
{
	if (!myEnergyPack || myPlayerState->GetEnergy() < amount)
	{
		return;
	}

	int currentEnergy = myPlayerState->GetEnergy();

	if (currentEnergy <= 0 || currentEnergy - amount < 0)
	{
		return;
	}

	myPlayerState->IncrementOrDecrementEnergyBy(-amount);
}

void AShooterSandboxCharacter::Client_UpdateEnergyOnHUD_Implementation()
{
	if (!myHUD || !myPlayerState)
	{
		return;
	}

	myHUD->UpdateEnergyLevel(myPlayerState->GetEnergy(), myPlayerState->GetMaxEnergy());
}

void AShooterSandboxCharacter::PlayerOutOfEnergy_Implementation()
{
	myHUD->ShowNotificationMessage("Not enough Energy");
	myHUD->OutOfEnergy();
}

void AShooterSandboxCharacter::TryConstruct(TSubclassOf<class ABaseConstruct> construct)
{
	if (construct == nullptr)
	{
		return;
	}

	if (!myEnergyPack)
	{
		myHUD->ShowNotificationMessage("Cannot construct without Energy Pack");
		return;
	}

	FVector spawnLocation;
	FRotator spawnRotation;
	AConstructibleSurface* surfaceToSpawnOn;
	if (GetSpawnLocationAndRotation(spawnLocation, spawnRotation, surfaceToSpawnOn, construct)) {
		if (construct.GetDefaultObject()->constructionCost > myPlayerState->GetEnergy())
		{
			PlayerOutOfEnergy();
			return;
		}
		ServerConstruct(construct, surfaceToSpawnOn, myController, spawnLocation, spawnRotation);
	}
}

bool AShooterSandboxCharacter::SetOffensiveConstructInVicinity_Validate(ABaseOffensiveConstruct* construct) {
	return true;
}

void AShooterSandboxCharacter::SetOffensiveConstructInVicinity_Implementation(ABaseOffensiveConstruct* construct)
{
	currentConstructInVicinity = construct;
	if (construct)
	{
		myHUD->ShowActionPromptMessage("E", ("Control " + construct->constructName));
	}
	else
	{
		myHUD->RemoveActionPromptMessage();
	}
}

bool AShooterSandboxCharacter::PickupEnergyPack_Validate(AEnergyPack* thePack)
{
	return true;
}

void AShooterSandboxCharacter::PickupEnergyPack_Implementation(AEnergyPack* thePack)
{
	myEnergyPack = thePack;
	PickupEnergyPackOnOwningClient(thePack);
	
	myEnergyPack->energyPackBody->SetSimulatePhysics(false);
	myEnergyPack->boxCollider->SetSimulatePhysics(false);

	myEnergyPack->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("EnergyPackHolder"));
}

bool AShooterSandboxCharacter::PickupEnergyPackOnOwningClient_Validate(AEnergyPack* thePack)
{
	return true;
}

void AShooterSandboxCharacter::PickupEnergyPackOnOwningClient_Implementation(AEnergyPack* thePack)
{
	if (!myController)
	{
		return;
	}

	//myEnergyPack = thePack;

	myHUD->SetHasEnergyPack(true);
}

bool AShooterSandboxCharacter::DropEnergyPack_Validate()
{
	return true;
}

void AShooterSandboxCharacter::DropEnergyPack_Implementation()
{
	if(!myEnergyPack)
	{
		return;
	}

	myEnergyPack->energyPackBody->SetSimulatePhysics(true);
	myEnergyPack->boxCollider->SetSimulatePhysics(true);

	myEnergyPack->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	myEnergyPack->ScheduleReactivation();

	myEnergyPack = nullptr;

	DropEnergyPackOnOwningClient();
}

bool AShooterSandboxCharacter::DropEnergyPackOnOwningClient_Validate()
{
	return true;
}

void AShooterSandboxCharacter::DropEnergyPackOnOwningClient_Implementation()
{
	if (!myController) {
		return;
	}
	//myEnergyPack = nullptr;

	myHUD->SetHasEnergyPack(false);
}

ABaseOffensiveConstruct* AShooterSandboxCharacter::GetOffensiveConstructInVicinity()
{
	return currentConstructInVicinity;
}

void AShooterSandboxCharacter::SwitchConstructionMode()
{
	currentConstructionMode = currentConstructionMode == EConstructionMode::Surface ? EConstructionMode::Wall : EConstructionMode::Surface;
	myHUD->SetConstructionMode(currentConstructionMode);
}

void AShooterSandboxCharacter::ToggleConstructionMenu()
{
	Cast<AAShooterSandboxHUD>(Cast<AShooterSandboxController>(GetController())->GetHUD())->ToggleConstructionMenu();
}

bool AShooterSandboxCharacter::ServerConstruct_Validate(TSubclassOf<ABaseConstruct> construct, AConstructibleSurface* surfaceToSpawnOn, AShooterSandboxController* constructorController, FVector spawnLocation, FRotator spawnRotation)
{
	return true;
}

void AShooterSandboxCharacter::ServerConstruct_Implementation(TSubclassOf<ABaseConstruct> construct, AConstructibleSurface* surfaceToSpawnOn, AShooterSandboxController* constructorController, FVector spawnLocation, FRotator spawnRotation)
{
	if (construct == nullptr || constructorController == nullptr)
	{
		return;
	}

	UWorld* world = GetWorld();
	if (world && world->GetAuthGameMode<AShooterSandboxGameMode>())
	{
		world->GetAuthGameMode<AShooterSandboxGameMode>()->Server_SpawnConstruct(construct, surfaceToSpawnOn, constructorController, spawnLocation, spawnRotation);
	}
}

bool AShooterSandboxCharacter::Multicast_PickupOrDropWeapon_Validate(ABaseWeapon* theWeapon)
{
	return true;
}

void AShooterSandboxCharacter::Multicast_PickupOrDropWeapon_Implementation(ABaseWeapon* theWeapon)
{

	if (theWeapon) {
		weaponCurrentlyHeld = theWeapon;
		weaponCurrentlyHeld->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("WeaponHolder"));
		bHasWeapon = true;	
	}

	else
	{
		//when will a player drop weapons?
		//1. they can consume its remaining energy
		//2. or they can run out of energy/ammo in the gun
		//no "dropping" weapons
		//in both cases, weapon will be destroyed on server, do NOT destroy here

		weaponCurrentlyHeld->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//weaponCurrentlyHeld->DestroyWeapon();
		weaponCurrentlyHeld = nullptr;
		bHasWeapon = false;
	}
}

bool AShooterSandboxCharacter::Client_PickupOrDropWeapon_Validate(bool hasPickedUp)
{
	return true;
}

void AShooterSandboxCharacter::Client_PickupOrDropWeapon_Implementation(bool hasPickedUp)
{

	if (hasPickedUp)
	{
		if (weaponCurrentlyHeld) {
			myHUD->WeaponAmmoScreen(true);
			myHUD->UpdateWeaponAmmo(weaponCurrentlyHeld->currentClipSize, weaponCurrentlyHeld->clipSize);
		}
	}

	else
	{
		myHUD->WeaponAmmoScreen(false);
	}
}

bool AShooterSandboxCharacter::Client_UpdateWeaponAmmo_Validate(int max, int current)
{
	return true;
}

void AShooterSandboxCharacter::Client_UpdateWeaponAmmo_Implementation(int max, int current)
{
	myHUD->UpdateWeaponAmmo(current, max);
}

void AShooterSandboxCharacter::Client_EnergyNotification_Implementation(const FString & reason, int amount, int greenRedNeut)
{
	FString msg;
	if (greenRedNeut == 1)
	{
		msg = reason + " ( - " + FString::FromInt(amount) + " )";
	}
	if (greenRedNeut == 0)
	{
		msg = reason + " ( + " + FString::FromInt(amount) + " )";
	}
	if (myHUD)
	{
		myHUD->ShowAlertMessage(msg, greenRedNeut);
	}
}

void AShooterSandboxCharacter::StartWeaponFire()
{
	if (!weaponCurrentlyHeld)
	{
		return;
	}

	weaponCurrentlyHeld->StartShooting();
}

void AShooterSandboxCharacter::StopWeaponFire()
{
	if (!weaponCurrentlyHeld)
	{
		return;
	}

	weaponCurrentlyHeld->StopShooting();
}

void AShooterSandboxCharacter::WeaponAltMode()
{
	if (!weaponCurrentlyHeld)
	{
		return;
	}

	weaponCurrentlyHeld->AlternateFire();
}

void AShooterSandboxCharacter::AttemptControlOffensiveConstruct()
{
	if (currentConstructInVicinity && myController)
	{
		if (currentConstructInVicinity->GetIsBeingUsed())
		{
			myHUD->RemoveActionPromptMessage();
			currentConstructInVicinity = nullptr;
			myHUD->ShowNotificationMessage("Construct already being controlled");
			return;
		}
		Server_AttemptControlOffensiveConstruct(currentConstructInVicinity, myController);
		myHUD->RemoveActionPromptMessage();
		myHUD->ShowTurretModeSwitching(currentConstructInVicinity->switchScreen,
			currentConstructInVicinity->currentMode == ETurretFireMode::Primary ? 0 : 1);
	}
}

bool AShooterSandboxCharacter::Server_AttemptControlOffensiveConstruct_Validate(ABaseOffensiveConstruct * constructToControl, AShooterSandboxController * controllerController)
{
	return true;
}

void AShooterSandboxCharacter::Server_AttemptControlOffensiveConstruct_Implementation(ABaseOffensiveConstruct * constructToControl, AShooterSandboxController * controllerController)
{
	//Construct->Set Owner on client
	constructToControl->Multicast_ControlOffensive(controllerController);
}

void AShooterSandboxCharacter::PrintTestData()
{
	//FString msg = "Number of constructs I've built - " + FString::FromInt(Cast<AShooterSandboxPlayerState>(myController->PlayerState)->GetNumConstructsBuilt());
	//if (Cast<AShooterSandboxPlayerState>(myController->PlayerState)) {
	//	msg += " true";
	//}
	//else {
	//	msg += " true";
	//}
	//UKismetSystemLibrary::PrintString(this, (TEXT("%s"), *msg));
	UKismetSystemLibrary::PrintString(this, (TEXT("Energy was changed")));

}

void AShooterSandboxCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterSandboxCharacter, myEnergyPack);
}
