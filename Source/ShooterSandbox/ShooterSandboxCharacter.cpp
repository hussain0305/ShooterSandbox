#include "ShooterSandboxCharacter.h"
#include "AShooterSandboxHUD.h"
#include "ShooterSandboxPlayerState.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxGameMode.h"
#include "ShooterSandboxGlobal.h"
#include "ConstructibleSurface.h"
#include "BaseConstruct.h"
#include "BaseOffensiveConstruct.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

AShooterSandboxCharacter::AShooterSandboxCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	SetReplicates(true);
	SetReplicatingMovement(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
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
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterSandboxCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShooterSandboxCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AShooterSandboxCharacter::ToggleRunOn);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AShooterSandboxCharacter::ToggleRunOff);

	PlayerInputComponent->BindAction("ConstructionMenu", IE_Pressed, this, &AShooterSandboxCharacter::ToggleConstructionMenu);
	PlayerInputComponent->BindAction("QuickConstruct", IE_Pressed, this, &AShooterSandboxCharacter::TryQuickConstruct);//TryQuickConstruct

	PlayerInputComponent->BindAction("MouseWheelDown", IE_Pressed, this, &AShooterSandboxCharacter::MouseWheelDown);
	PlayerInputComponent->BindAction("MouseWheelUp", IE_Pressed, this, &AShooterSandboxCharacter::MouseWheelUp);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AShooterSandboxCharacter::AttemptControlOffensiveConstruct);
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

	myController = Cast<AShooterSandboxController>(GetController());//UGameplayStatics::GetPlayerController(this, 0));//

	currentMovementState = EMovementState::Stationary;
	GetWorld()->GetTimerManager().SetTimer(movementStateMonitoring, this, &AShooterSandboxCharacter::MonitorMovementState, 0.25f, true);
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
		if (currentMovementState != EMovementState::Jumping)
		{
			SetCurrentEMovementState(EMovementState::Jumping);
		}
	}
	else if (GetCharacterMovement()->Velocity.Size() < 10.0f)
	{
		//Stationary
		if (currentMovementState != EMovementState::Stationary)
		{
			if (currentMovementState == EMovementState::Running)
			{
				ToggleRunOff();
			}
			SetCurrentEMovementState(EMovementState::Stationary);
		}
	}

	else if (GetCharacterMovement()->Velocity.Size() < (walkSpeed + 10.0f))
	{
		//Walking
		if (currentMovementState != EMovementState::Walking)
		{
			SetCurrentEMovementState(EMovementState::Walking);
			if (currentMovementState == EMovementState::Running)
			{
				ToggleRunOff();
			}
		}

	}

	else {
		if (currentMovementState != EMovementState::Running)
		{
			SetCurrentEMovementState(EMovementState::Running);
		}
	}
}

void AShooterSandboxCharacter::SetCurrentEMovementState(EMovementState newState)
{
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
	//Only continue if player isn't already running
	if (bIsRunning) {
		return;
	}

	bIsRunning = true;

	GetCharacterMovement()->MaxWalkSpeed = RUN_MULTIPLIER * walkSpeed;
	Server_ToggleRun(RUN_MULTIPLIER * walkSpeed);
	myController->ClientPlayCameraShake(runCamShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
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
	myController->ClientStopCameraShake(runCamShake);
	myController->ClientPlayCameraShake(endRunCamShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
}

bool AShooterSandboxCharacter::Server_ToggleRun_Validate(float newSpeed)
{
	return true;
}

void AShooterSandboxCharacter::Server_ToggleRun_Implementation(float newSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

#pragma endregion


void AShooterSandboxCharacter::MouseWheelDown()
{
	if (!myController)
	{
		return;
	}

	Cast<AAShooterSandboxHUD>(myController->GetHUD())->ScrollDownList();
}

void AShooterSandboxCharacter::MouseWheelUp()
{
	if (!myController)
	{
		return;
	}

	Cast<AAShooterSandboxHUD>(myController->GetHUD())->ScrollUpList();
}

bool AShooterSandboxCharacter::GetSpawnLocation(FVector &spawnLocation)
{
	if (!GetWorld())
	{
		return false;
	}

	FCollisionQueryParams traceParams;
	traceParams.AddIgnoredActor(this);

	FHitResult hit;
	
	if (GetWorld()->LineTraceSingleByObjectType(hit, FollowCamera->GetComponentLocation(),
		(FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * BUILD_DISTANCE)),
		ECC_GameTraceChannel1, traceParams)){

		if (Cast<AConstructibleSurface>(hit.Actor))
		{
			spawnLocation = hit.ImpactPoint;

			float gridSize = Cast<AConstructibleSurface>(hit.Actor)->gridSizeInUnits;
			int gridNoX = spawnLocation.X / gridSize;
			int gridNoY = spawnLocation.Y / gridSize;

			float gridAlignedX = (gridNoX * gridSize) + (spawnLocation.X < 0 ? -gridSize / 2 : gridSize / 2);
			float gridAlignedY = (gridNoY * gridSize) + (spawnLocation.Y < 0 ? -gridSize / 2 : gridSize / 2);

			spawnLocation = FVector(gridAlignedX, gridAlignedY, spawnLocation.Z);

			return true;
		}
	}

	spawnLocation = FVector(1000, 1000, 1000);
	return false;
}

void AShooterSandboxCharacter::TryConstruct(TSubclassOf<class ABaseConstruct> construct)
{
	if (construct == nullptr)
	{
		return;
	}

	FVector spawnLocation;
	if (GetSpawnLocation(spawnLocation)) {
		ServerConstruct(construct, myController, spawnLocation, GetActorRotation());
	}
}

bool AShooterSandboxCharacter::SetOffensiveConstructInVicinity_Validate(ABaseOffensiveConstruct* construct) {
	return true;
}

void AShooterSandboxCharacter::SetOffensiveConstructInVicinity_Implementation(ABaseOffensiveConstruct* construct)//
{
	currentConstructInVicinity = construct;
	if (construct)
	{
		Cast<AAShooterSandboxHUD>(myController->GetHUD())->ShowActionPromptMessage("E", ("Control " + construct->constructName));
	}
	else
	{
		Cast<AAShooterSandboxHUD>(myController->GetHUD())->RemoveActionPromptMessage();
	}
}

ABaseOffensiveConstruct* AShooterSandboxCharacter::GetOffensiveConstructInVicinity()
{
	return currentConstructInVicinity;
}

void AShooterSandboxCharacter::ToggleConstructionMenu()
{
	Cast<AAShooterSandboxHUD>(Cast<AShooterSandboxController>(GetController())->GetHUD())->ToggleConstructionMenu();
}

bool AShooterSandboxCharacter::ServerConstruct_Validate(TSubclassOf<ABaseConstruct> construct, AShooterSandboxController* constructorController, FVector spawnLocation, FRotator spawnRotation)
{
	return true;
}

void AShooterSandboxCharacter::ServerConstruct_Implementation(TSubclassOf<ABaseConstruct> construct, AShooterSandboxController* constructorController, FVector spawnLocation, FRotator spawnRotation)
{
	if (construct == nullptr || constructorController == nullptr)
	{
		return;
	}

	UWorld* world = GetWorld();
	if (world && world->GetAuthGameMode<AShooterSandboxGameMode>())
	{
		world->GetAuthGameMode<AShooterSandboxGameMode>()->Server_SpawnConstruct(construct, constructorController, spawnLocation, spawnRotation);
	}
}

void AShooterSandboxCharacter::AttemptControlOffensiveConstruct()
{
	if (currentConstructInVicinity && myController)
	{
		if (currentConstructInVicinity->GetIsBeingUsed())
		{
			Cast<AAShooterSandboxHUD>(myController->GetHUD())->RemoveActionPromptMessage();
			currentConstructInVicinity = nullptr;
			Cast<AAShooterSandboxHUD>(myController->GetHUD())->ShowNotificationMessage("Construct already being controlled");
			return;
		}
		Server_AttemptControlOffensiveConstruct(currentConstructInVicinity, myController);
		Cast<AAShooterSandboxHUD>(myController->GetHUD())->RemoveActionPromptMessage();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Controller/vicinity construct inside AttemptControlOffensiveConstruct_Implementation"));
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
	FString msg = "Number of constructs I've built - " + FString::FromInt(Cast<AShooterSandboxPlayerState>(myController->PlayerState)->GetNumConstructsBuilt());
	if (Cast<AShooterSandboxPlayerState>(myController->PlayerState)) {
		msg += " true";
	}
	else {
		msg += " true";
	}
	UKismetSystemLibrary::PrintString(this, (TEXT("%s"), *msg));
}