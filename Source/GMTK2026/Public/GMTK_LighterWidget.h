#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GMTK_LighterWidget.generated.h"

class UImage;
class UCanvasPanel;
class UGMTK_BurnableNoteWidget;
class UWidgetAnimation;

UCLASS(Abstract, Blueprintable)
class GMTK2026_API UGMTK_LighterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Local Offset from the lighter body to the flame socket (in pixels, inside this widget).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighter")
	FVector2D FlameSocketOffset = FVector2D(0.0f, -40.0f);

	// Follow speed of the flame to the lighter body. Lower = more elastic/moving.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighter")
	float FlameLagSpeed = 8.0f;

	// True once the ignition animation has finished and the flame can actually burn notes.
	UPROPERTY(BlueprintReadOnly, Category = "Lighter")
	bool bIsLit = false;

	// True while the ignition animation/sprite is playing (input is ignored during this window).
	UPROPERTY(BlueprintReadOnly, Category = "Lighter")
	bool bIsIgniting = false;

	UFUNCTION(BlueprintCallable, Category = "Lighter")
	void RegisterNote(UGMTK_BurnableNoteWidget* Note);

	// Called by the controller on left click. Only does something while the lighter is off and not already igniting.
	UFUNCTION(BlueprintCallable, Category = "Lighter")
	void TryIgnite();

	// Called from Blueprint (Anim Notify / Event Track / Timeline end) once the ignition sprite/anim has finished.
	UFUNCTION(BlueprintCallable, Category = "Lighter")
	void OnIgnitionFinished();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvas;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> FlameImage;

	// BP event: play here the ignition sprite-sheet/flipbook material or a UWidgetAnimation.
	// Call OnIgnitionFinished() at the end (via Anim Notify, Timeline, or a Delay node).
	UFUNCTION(BlueprintImplementableEvent, Category = "Lighter")
	void PlayIgnitionVisual();

private:
	UPROPERTY()
	TArray<TObjectPtr<UGMTK_BurnableNoteWidget>> WatchedNotes;

	FVector2D CurrentFlameLocalPosition = FVector2D::ZeroVector;
};