// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseOffensiveConstruct.h"
#include "Camera/CameraComponent.h"
#include "ShooterSandboxController.h"
#include "ShooterSandboxCharacter.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ABaseOffensiveConstruct::ABaseOffensiveConstruct()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("ConstructRoot")));

	constructBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Construct Base"));
	constructBase->SetupAttachment(GetRootComponent());

	rotatingConstructPart = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rotating Construct Part"));
	rotatingConstructPart->SetupAttachment(GetRootComponent());
	
	muzzle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Muzzle"));
	muzzle->SetupAttachment(rotatingConstructPart);

	vicinity = CreateDefaultSubobject<USphereComponent>(TEXT("Vicinity"));
	vicinity->SetupAttachment(GetRootComponent());
	vicinity->SetGenerateOverlapEvents(true);
	vicinity->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	vicinity->OnComponentBeginOverlap.AddDynamic(this, &ABaseOffensiveConstruct::OnOverlapBegin);
	vicinity->OnComponentEndOverlap.AddDynamic(this, &ABaseOffensiveConstruct::OnOverlapEnd);
}

void ABaseOffensiveConstruct::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &ABaseOffensiveConstruct::LeaveOffensive);
}

void ABaseOffensiveConstruct::StartShooting()
{

}

void ABaseOffensiveConstruct::StopShooting()
{
}

bool ABaseOffensiveConstruct::OnOverlapBegin_Validate(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	return true;
}

void ABaseOffensiveConstruct::OnOverlapBegin_Implementation(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Cast<AShooterSandboxCharacter>(OtherActor)) {
		Cast<AShooterSandboxCharacter>(OtherActor)->SetOffensiveConstructInVicinity(this);
	}
}

bool ABaseOffensiveConstruct::OnOverlapEnd_Validate(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	return true;
}

void ABaseOffensiveConstruct::OnOverlapEnd_Implementation(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AShooterSandboxCharacter>(OtherActor)) {
		Cast<AShooterSandboxCharacter>(OtherActor)->SetOffensiveConstructInVicinity(nullptr);
	}
}

bool ABaseOffensiveConstruct::ControlOffensive_Validate(AShooterSandboxController * occupantController) {
	return true;
}

void ABaseOffensiveConstruct::ControlOffensive_Implementation(AShooterSandboxController * occupantController)
{
	if (!HasAuthority()|| occupantController == nullptr) {
		return;
	}

	userController = occupantController;
	userCharacter = Cast<AShooterSandboxCharacter>(occupantController->GetCharacter());

	userCharacter->GetFollowCamera()->SetupAttachment(muzzle, FName("CameraHolder"));
	userController->Possess(this);
}

bool ABaseOffensiveConstruct::LeaveOffensive_Validate() {
	return true;
}

void ABaseOffensiveConstruct::LeaveOffensive_Implementation()
{
	if (!HasAuthority() || userController == nullptr) {
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Leaving Offensive"));
	userController->Possess(userCharacter);
	userCharacter->ReattachCamera(userCharacter->GetFollowCamera());

	userCharacter = nullptr;
	userController = nullptr;

}


void ABaseOffensiveConstruct::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseOffensiveConstruct, userController);
}
