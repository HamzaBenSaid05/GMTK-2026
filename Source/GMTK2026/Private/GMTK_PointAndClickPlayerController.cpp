#include "GMTK_PointAndClickPlayerController.h"
#include "GMTK_InteractableActor.h"
#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GMTK_GameMode.h"
#include "InputAction.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"

AGMTK_PointAndClickPlayerController::AGMTK_PointAndClickPlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGMTK_PointAndClickPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PC = UGameplayStatics::GetPlayerController(this, 0);
	// Set Up Input Mode
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	
	UE_LOG(LogTemp, Warning, TEXT("Mapping: %s"), *DefaultMappingContext.GetName());
	UE_LOG(LogTemp, Warning, TEXT("Action: %s"), *GetNameSafe(ClickAction));

	// Activate mapping context
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, MappingContextPriority);
			}
		}
	}

	// Create the hover widget and add it to the viewport, but keep it hidden initially
	if(HoverWidgetClass)
	{
		HoverWidget = CreateWidget<UUserWidget>(this, HoverWidgetClass);

		if(HoverWidget)
		{
			HoverWidget->AddToViewport();
			HoverWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	// Bind to game mode timer event to toggle input
	//if (AGMTK_GameMode* GameMode = Cast<AGMTK_GameMode>(UGameplayStatics::GetGameMode(this)))
	//{
	//	// Disable Input
	//	GameMode->OnDelayStart.AddDynamic(this, &AGMTK_PointAndClickPlayerController::OnDelayStart);
	//	// Enable Input
	//	GameMode->OnTimerStart.AddDynamic(this, &AGMTK_PointAndClickPlayerController::OnTimerStart);
	//	// Disable Input
	//	GameMode->OnTimerFinish.AddDynamic(this, &AGMTK_PointAndClickPlayerController::OnTimerFinish);
	//}
}

void AGMTK_PointAndClickPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ClickAction)
		{
			// Bind the left mouse click action to the HandleLeftClick function
			// Trigger the action when the button is pressed 
			EnhancedInput->BindAction(ClickAction, ETriggerEvent::Started, this, &AGMTK_PointAndClickPlayerController::HandleLeftClick);
		}
	}
}

AGMTK_InteractableActor* AGMTK_PointAndClickPlayerController::TraceUnderCursor() const
{
	FHitResult Hit;

	// GetHitResultUnderCursor make the deprojection of the mouse position and the line trace in the world
	const bool bHit = GetHitResultUnderCursor(TraceChannel, false, Hit);

	// Debug drawing of the trace line and hit point
	if (bDrawDebugTrace)
	{
		FVector RayStart, RayDirection;
		DeprojectMousePositionToWorld(RayStart, RayDirection);

		const FVector RayEnd = bHit ? Hit.ImpactPoint : (RayStart + RayDirection * 10000.0f);
		const FColor LineColor = bHit ? FColor::Green : FColor::Red;

		DrawDebugLine(GetWorld(), RayStart, RayEnd, LineColor, false, DebugDrawDuration, 0, 1.5f);

		if (bHit)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 8.0f, 12, FColor::Yellow, false, DebugDrawDuration);
			DrawDebugLine(GetWorld(), Hit.ImpactPoint, Hit.ImpactPoint + Hit.ImpactNormal * 30.0f, FColor::Blue, false, DebugDrawDuration, 0, 1.5f);

			DrawDebugString(GetWorld(), Hit.ImpactPoint, GetNameSafe(Hit.GetActor()), nullptr, FColor::White, DebugDrawDuration);
		}
	}

	if (bHit && Hit.GetActor())
	{
		return Cast<AGMTK_InteractableActor>(Hit.GetActor());
	}
	return nullptr;
}

void AGMTK_PointAndClickPlayerController::TogglePlayerControllerInput(bool bIsEnabled) 
{
	if (PC)
	{
		bInputEnabled = bIsEnabled;
		if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (DefaultMappingContext)
				{
					if (bIsEnabled)
					{
						Subsystem->AddMappingContext(DefaultMappingContext, MappingContextPriority);
					}
					else
					{
						Subsystem->RemoveMappingContext(DefaultMappingContext);
					}
				}
			}
			PC->bShowMouseCursor = bIsEnabled;
		}
	}
}

void AGMTK_PointAndClickPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	// If input is disabled, skip the hover detection and interaction logic
	if (!bInputEnabled)
	{
		return;
	}
	// Update over every frame
	AGMTK_InteractableActor* CurrentHover = TraceUnderCursor();
	if (CurrentHover != HoveredInteractable)
	{
		AGMTK_InteractableActor* Old = HoveredInteractable;
		HoveredInteractable = CurrentHover;
		OnHoveredInteractableChanged(CurrentHover, Old);
		if(HoverWidget)
		{
			if(CurrentHover)
			{
				FVector WorldLocation = CurrentHover->HoverPoint->GetComponentLocation();

				FVector2D ScreenPosition;

				ProjectWorldLocationToScreen(WorldLocation, ScreenPosition);

				HoverWidget->SetPositionInViewport(ScreenPosition, false);
				
				DrawDebugSphere(
				                GetWorld(),
				                CurrentHover->HoverPoint->GetComponentLocation(),
				                10,
				                12,
				                FColor::Purple,
				                false,
				                0.1f
				               );
				
				HoverWidget->SetVisibility(
					ESlateVisibility::Visible
				);
			}
			else
			{
				HoverWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void AGMTK_PointAndClickPlayerController::HandleLeftClick()
{
	UE_LOG(LogTemp, Log, TEXT("Left mouse button clicked"));
	if (AGMTK_InteractableActor* Target = TraceUnderCursor())
	{
		Target->Interact();
	}
}

void AGMTK_PointAndClickPlayerController::ShowHoverImage()
{
	if(!HoverWidget && HoverWidgetClass)
	{
		HoverWidget = CreateWidget<UUserWidget>(
			this,
			HoverWidgetClass
		);

		HoverWidget->AddToViewport();
	}
}

void AGMTK_PointAndClickPlayerController::HideHoverImage()
{
	if(HoverWidget)
	{
		HoverWidget->RemoveFromParent();
		HoverWidget = nullptr;
	}
}
