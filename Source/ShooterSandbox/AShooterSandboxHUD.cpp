
#include "AShooterSandboxHUD.h"
//#include "Engine/Canvas.h"
//#include "Engine/Texture2D.h"
//#include "TextureResource.h"
//#include "CanvasItem.h"
//#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"


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

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}

	if (constructionMenuWidget != nullptr) 
	{
		constructionMenu = CreateWidget<UUserWidget>(GetWorld(), constructionMenuWidget);
	}
}

void AAShooterSandboxHUD::ToggleConstructionMenu()
{
	constructMenuOpen = !constructMenuOpen;

	if (constructMenuOpen) {

		if (constructionMenu)
		{
			constructionMenu->AddToViewport();
			APlayerController* playerController = GetOwningPlayerController();
			if (playerController) {
				playerController->SetInputMode(FInputModeGameAndUI());
				playerController->bShowMouseCursor = true;
				constructionMenu->bIsFocusable = true;
			}
		}
	}

	else {
		if (constructionMenu)
		{
			constructionMenu->RemoveFromParent();
			APlayerController* playerController = GetOwningPlayerController();
			if (playerController) {
				playerController->SetInputMode(FInputModeGameOnly());
				playerController->bShowMouseCursor = false;
				constructionMenu->bIsFocusable = false;
			}
		}
	}
}



