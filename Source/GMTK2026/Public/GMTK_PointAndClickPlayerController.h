#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GMTK_PointAndClickPlayerController.generated.h"

class AGMTK_InteractableActor;
class UInputAction;
class UInputMappingContext;

UCLASS(Blueprintable)
class GMTK2026_API AGMTK_PointAndClickPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGMTK_PointAndClickPlayerController();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointAndClick|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointAndClick|Input")
	TObjectPtr<UInputAction> ClickAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointAndClick|Input")
	int32 MappingContextPriority = 0;

	// Collision Channel for mouse trace
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointAndClick")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointAndClick")
		bool bDrawDebugTrace = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointAndClick",
		meta = (EditCondition = "bDrawDebugTrace"))
	float DebugDrawDuration = 2.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PointAndClick")
	TSubclassOf<UUserWidget> HoverWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "PointAndClick")
	TObjectPtr<AGMTK_InteractableActor> HoveredInteractable;
	

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	// Called when the left mouse button is pressed
	UFUNCTION(BlueprintCallable, Category = "PointAndClick")
	void HandleLeftClick();

	// Called when the hovered interactable actor changes
	UFUNCTION(BlueprintImplementableEvent, Category = "PointAndClick")
	void OnHoveredInteractableChanged(AGMTK_InteractableActor* NewHovered, AGMTK_InteractableActor* OldHovered);

	void ShowHoverImage();
	void HideHoverImage();
	
private:
	AGMTK_InteractableActor* TraceUnderCursor() const;
	
	UPROPERTY()
	UUserWidget* HoverWidget;

	UFUNCTION()
	void TogglePlayerControllerInput(bool bIsEnabled);

	bool bInputEnabled = true;
	
	UFUNCTION()
	void OnDelayStart()
	{
		TogglePlayerControllerInput(false);
	}

	UFUNCTION()
	void OnTimerStart()
	{
		TogglePlayerControllerInput(true);
	}

	UFUNCTION()
	void OnTimerFinish()
	{
		TogglePlayerControllerInput(false);
	}
};
