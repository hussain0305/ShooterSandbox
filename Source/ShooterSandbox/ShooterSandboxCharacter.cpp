#include "ShooterSandboxCharacter.h"
#include "AShooterSandboxHUD.h"
#include "ShooterSandboxController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

AShooterSandboxCharacter::AShooterSandboxCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bIsRunning = false;
	walkSpeed = GetCharacterMovement()->MaxWalkSpeed;

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
}

void AShooterSandboxCharacter::BeginPlay()
{
	Super::BeginPlay();

	myController = UGameplayStatics::GetPlayerController(this, 0);
}

void AShooterSandboxCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterSandboxCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShooterSandboxCharacter::ToggleCrouch);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AShooterSandboxCharacter::ToggleRun);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AShooterSandboxCharacter::ToggleRun);

	PlayerInputComponent->BindAction("ConstructionMenu", IE_Pressed, this, &AShooterSandboxCharacter::ToggleConstructionMenu);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterSandboxCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterSandboxCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterSandboxCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterSandboxCharacter::LookUpAtRate);
}

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

void AShooterSandboxCharacter::ToggleCrouch()
{
	if (bIsCrouched) {
		UnCrouch();
	}
	else {
		Crouch();
	}
}

void AShooterSandboxCharacter::ToggleRun()
{
	bIsRunning = !bIsRunning;
	if (bIsRunning) {
		GetCharacterMovement()->MaxWalkSpeed = RUN_MULTIPLIER * walkSpeed;
		myController->ClientPlayCameraShake(runCamShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
		myController->ClientStopCameraShake(runCamShake);
		myController->ClientPlayCameraShake(endRunCamShake, 1, ECameraAnimPlaySpace::CameraLocal, FRotator(0, 0, 0));
	}
}

void AShooterSandboxCharacter::ToggleConstructionMenu()
{
	Cast<AAShooterSandboxHUD>(Cast<AShooterSandboxController>(GetController())->GetHUD())->ToggleConstructionMenu();
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
	if ( (Controller != NULL) && (Value != 0.0f) )
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
