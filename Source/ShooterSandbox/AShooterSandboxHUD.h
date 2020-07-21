// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterSandboxGlobal.h"
#include "AShooterSandboxHUD.generated.h"

//SORT OUT GLOBAL FORWARD DECLARATION HERE

//enum class EConstructionMode : uint8;
//enum class EMovementState : uint8;

UCLASS()
class SHOOTERSANDBOX_API AAShooterSandboxHUD : public AHUD
{
	GENERATED_BODY()
	
public:

/**************************
*   INHERENT FUNCTIONS    *
**************************/

	AAShooterSandboxHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	void SetupHUD();

/**************************
*       VARIABLES         *
**************************/

	EConstructionMode currentConstructionMode;

//=#=#=#=#= EDITABLE IN BLUEPRINTS =#=#=#=#=

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UUserWidget* mainHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UUserWidget* constructionMenuSurface;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UUserWidget* constructionMenuWall;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UUserWidget* currentConstructionMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UTurretModeSwitchScreen* turretModeSwitchingScreen;

/**************************
*       FUNCTIONS         *
**************************/

	UFUNCTION(BlueprintCallable)
	void ToggleConstructionMenu();

	void SetConstructionMode(EConstructionMode currentMode);

	UFUNCTION(BlueprintCallable)
	void SetTurretMode(int modeNum);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Messages")
	void ShowAlertMessage(const FString& message, int greenRedNeutral);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Messages")
	void ShowNotificationMessage(const FString& message);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Messages")
	void ShowActionPromptMessage(const FString& keyVal, const FString& msg);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Messages")
	void RemoveActionPromptMessage();

	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Construction")
	void ScrollDownList();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Quick Construction")
	void ScrollUpList();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Construction")
	void BP_SetConstructionScreen(bool isOnQuickConstruction);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Construction")
	void BP_SetConstructionMode(int modeNum);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Energy")
	void SetHasEnergyPack(bool hasPack);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Energy")
	void UpdateEnergyLevel(int newEnergyLevel, int maxEnergyLevel);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Energy")
	void OutOfEnergy();

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void ShowScorecard(bool shouldOpen);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void WeaponAmmoScreen(bool show, const FString& weaponName);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void UpdateWeaponAmmo(int currentEnergy, int maxEnergy);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Turret Gun Controls")
	void ShowTurretModeSwitching(TSubclassOf<class UTurretModeSwitchScreen> switchScreen, int mode);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Turret Gun Controls")
	void RemoveTurretModeSwitching();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Crosshair Controls")
	void MovementModeCrosshairChange(EMovementState newState);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Crosshair Controls")
	void ExecuteHUDActionByCommandCode(EHUDCommandType commandType);

protected:

	UPROPERTY(BlueprintReadWrite)
	bool constructMenuOpen;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditAnywhere, Category = "PlayerWidgets")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "ConstructionMenu")
	TSubclassOf<class UUserWidget> constructionMenuSurfaceWidget;

	UPROPERTY(EditAnywhere, Category = "ConstructionMenu")
	TSubclassOf<class UUserWidget> constructionMenuWallWidget;
};
