#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"
#include "BaseConstruct.h"
#include "ShooterSandboxGlobal.generated.h"

UENUM(BlueprintType)
enum class EConstructType : uint8{ 
	OffensiveConstruct	UMETA(DisplayName = "Offensive Construct"),
	DefensiveConstruct	UMETA(DisplayName = "Defensive Construct"),
	UtilityConstruct	UMETA(DisplayName = "Utility Construct"),
	EnergyWeapon		UMETA(DisplayName = "Energy Weapon"),
	HandledWeapon		UMETA(DisplayName = "Handled Weapon"),
	Environmental		UMETA(DisplayName = "Environmental"),
	WallConstruct		UMETA(DisplayName = "Wall Construct")
};

UENUM(BlueprintType)
enum class EMovementState : uint8 {
	Stationary	UMETA(DisplayName = "Stationary"),
	Walking		UMETA(DisplayName = "Walking"),
	Running		UMETA(DisplayName = "Running"),
	Jumping		UMETA(DisplayName = "Jumping")
};

UENUM(BlueprintType)
enum class ETurretFireMode : uint8 {
	Primary			UMETA(DisplayName = "Primary"),
	Alternate		UMETA(DisplayName = "Alternate")
};

UENUM(BlueprintType)
enum class EConstructionMode : uint8 {
	Surface			UMETA(DisplayName = "Surface"),
	Wall			UMETA(DisplayName = "Wall")
};

UENUM(BlueprintType)
enum class EHUDCommandType : uint8 {
	PlayHitAnimation			UMETA(DisplayName = "PlayHitAnimation"),
	PlayKillAnimation			UMETA(DisplayName = "PlayKillAnimation"),
	SuccessfulHit				UMETA(DisplayName = "SuccessfulHit")
};

USTRUCT(BlueprintType)
struct FConstructsDatabase : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EConstructType> type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString constructName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString displayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int constructionCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseConstruct> constructBP;
};