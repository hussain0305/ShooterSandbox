
#include "AShooterSandboxHUD.h"
//#include "Engine/Canvas.h"
//#include "Engine/Texture2D.h"
//#include "TextureResource.h"
//#include "CanvasItem.h"
//#include "UObject/ConstructorHelpers.h"
#include "ShooterSandboxGlobal.h"
#include "Blueprint/UserWidget.h"
#include "TurretModeSwitchScreen.h"


AAShooterSandboxHUD::AAShooterSandboxHUD()
{
	//// Set the crosshair texture
	//static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	//CrosshairTex = CrosshairTexObj.Object;

	//static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/FirstPerson/UI/Health_UI"));
	//HUDWidgetClass = HealthBarObj.Class;
}


void AAShooterSandboxHUD::DrawHUD()
{
	Super::DrawHUD();

	//// Draw very simple crosshair

	//// find center of the Canvas
	//const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	//// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	//const FVector2D CrosshairDrawPosition((Center.X),
	//	(Center.Y + 20.0f));

	//// draw the crosshair
	//FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	//TileItem.BlendMode = SE_BLEND_Translucent;
	//Canvas->DrawItem(TileItem);
}

void AAShooterSandboxHUD::BeginPlay()
{
	Super::BeginPlay();

	constructMenuOpen = false;

	SetupHUD();
}

void AAShooterSandboxHUD::SetupHUD()
{
	if (HUDWidgetClass != nullptr)
	{
		mainHUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (mainHUDWidget)
		{
			mainHUDWidget->AddToViewport();
		}
	}

	if (constructionMenuSurfaceWidget != nullptr)
	{
		constructionMenuSurface = CreateWidget<UUserWidget>(GetWorld(), constructionMenuSurfaceWidget);
	}

	if (constructionMenuWallWidget != nullptr)
	{
		constructionMenuWall = CreateWidget<UUserWidget>(GetWorld(), constructionMenuWallWidget);
	}
}

void AAShooterSandboxHUD::ToggleConstructionMenu()
{
	if (!currentConstructionMenu)
	{
		return;
	}

	constructMenuOpen = !constructMenuOpen;

	if (constructMenuOpen) {

		if (currentConstructionMenu)
		{
			currentConstructionMenu->AddToViewport();
			BP_SetConstructionScreen(false);
			APlayerController* playerController = GetOwningPlayerController();
			if (playerController) {
				playerController->SetInputMode(FInputModeGameAndUI());
				playerController->bShowMouseCursor = true;
				currentConstructionMenu->bIsFocusable = true;
			}
		}
	}

	else {
		if (currentConstructionMenu)
		{
			currentConstructionMenu->RemoveFromParent();
			BP_SetConstructionScreen(true);
			APlayerController* playerController = GetOwningPlayerController();
			if (playerController) {
				playerController->SetInputMode(FInputModeGameOnly());
				playerController->bShowMouseCursor = false;
				currentConstructionMenu->bIsFocusable = false;
			}
		}
	}
}

void AAShooterSandboxHUD::SetConstructionMode(EConstructionMode currentMode)
{
	bool reopenMenuAfter = false;
	if (constructMenuOpen)
	{
		ToggleConstructionMenu();
		reopenMenuAfter = true;
	}

	currentConstructionMode = currentMode;
	currentConstructionMenu = currentMode == EConstructionMode::Surface ? constructionMenuSurface : constructionMenuWall;

	BP_SetConstructionMode(currentMode == EConstructionMode::Surface ? 0 : 1);

	if (reopenMenuAfter)
	{
		ToggleConstructionMenu();
	}
}

void AAShooterSandboxHUD::SetTurretMode(int modeNum)
{
	if (turretModeSwitchingScreen) {
		turretModeSwitchingScreen->SetFireMode(modeNum);
	}
}



