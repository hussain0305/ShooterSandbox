#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "ShooterSandboxGlobal.generated.h"

UENUM(BlueprintType)
enum class EConstructType : uint8{ 
	OffensiveConstruct	UMETA(DisplayName = "Offensive Construct"),
	DefensiveConstruct	UMETA(DisplayName = "Defensive Construct"),
	UtilityConstruct	UMETA(DisplayName = "Utility Construct"),
	EnergyWeapon		UMETA(DisplayName = "Energy Weapon"),
	HandledWeapon		UMETA(DisplayName = "Handled Weapon"),
	Environmental		UMETA(DisplayName = "Environmental")
};


USTRUCT(BlueprintType)
struct FConstructsDatabase : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EConstructType> type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString constructName;
};