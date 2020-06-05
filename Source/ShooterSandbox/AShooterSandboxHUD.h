// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AShooterSandboxHUD.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERSANDBOX_API AAShooterSandboxHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AAShooterSandboxHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ToggleConstructionMenu();

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Messages")
	void ShowAlertMessage(const FString& message);

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
	void BP_SwitchConstructionMode(bool isOnQuickConstruction);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Energy")
	void SetHasEnergyPack(bool hasPack);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Energy")
	void UpdateEnergyLevel(int newEnergyLevel, int maxEnergyLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void ShowScorecard(bool shouldOpen);

protected:

	UPROPERTY(BlueprintReadWrite)
	bool constructMenuOpen;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditAnywhere, Category = "Health")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Health")
	TSubclassOf<class UUserWidget> constructionMenuWidget;

	UPROPERTY(EditAnywhere, Category = "Health")
	class UUserWidget* CurrentWidget;

	UPROPERTY(EditAnywhere, Category = "Health")
	class UUserWidget* constructionMenu;
};
