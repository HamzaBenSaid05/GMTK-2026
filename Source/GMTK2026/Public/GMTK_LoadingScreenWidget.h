#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GMTK_LoadingScreenWidget.generated.h"

class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFadeComplete);

UENUM(BlueprintType)
enum class EPhraseTransitionMode : uint8
{
	Fade UMETA(DisplayName = "Fade Opacity"),
	Typewriter UMETA(DisplayName = "Typewriter (one letter at a time)")
};

UCLASS(Abstract, Blueprintable)
class GMTK2026_API UGMTK_LoadingScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Time to go from transparent to fully black.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Timing")
	float FadeInDuration = 0.5f;

	// Minimum time the screen stays fully black before it's allowed to fade out
	// (even if the new level is already fully loaded/ready).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Timing")
	float HoldDuration = 1.0f;

	// Time to go from fully black back to transparent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Timing")
	float FadeOutDuration = 0.5f;
	
	// Pool of loading tip/flavor phrases. One is picked at random each time the screen goes fully black.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Tips")
	TArray<FText> LoadingPhrases;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Tips")
	bool bCyclePhrases = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Tips", meta = (EditCondition = "bCyclePhrases"))
	float CyclePhraseInterval = 2.5f;

	// How the phrase text appears/disappears when it changes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Tips")
	EPhraseTransitionMode PhraseTransitionMode = EPhraseTransitionMode::Fade;

	// Duration of the fade-in/fade-out of a single phrase (Fade mode only).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Tips", meta = (EditCondition = "PhraseTransitionMode == EPhraseTransitionMode::Fade"))
	float PhraseFadeDuration = 0.35f;

	// Seconds between each revealed letter (Typewriter mode only).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading|Tips", meta = (EditCondition = "PhraseTransitionMode == EPhraseTransitionMode::Typewriter"))
	float TypewriterLetterInterval = 0.03f;

	UPROPERTY(BlueprintAssignable, Category = "Loading")
	FOnFadeComplete OnFadeInComplete;

	UPROPERTY(BlueprintAssignable, Category = "Loading")
	FOnFadeComplete OnFadeOutComplete;

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void FadeIn();

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void FadeOut();

	UFUNCTION(BlueprintCallable, Category = "Loading")
	void SetFullyOpaque();

	// Picks and shows a new random phrase, using the configured transition mode.
	UFUNCTION(BlueprintCallable, Category = "Loading|Tips")
	void ShowRandomPhrase();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> BlackImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PhraseText;

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading")
	void OnFadeInStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading")
	void OnFadeOutStarted();

private:
	enum class EFadeState : uint8 { None, FadingIn, FadingOut };
	enum class EPhraseState : uint8 { Idle, PhraseFadingIn, PhraseFadingOut, Typewriting };

	EFadeState CurrentState = EFadeState::None;
	float CurrentOpacity = 0.0f;
	float FadeElapsed = 0.0f;
	float PhraseCycleElapsed = 0.0f;
	float HoldElapsed = 0.0f;

	EPhraseState PhraseState = EPhraseState::Idle;
	float PhraseTransitionElapsed = 0.0f;
	FString PendingPhraseString;
	FString CurrentPhraseString;
	int32 TypewriterCharIndex = 0;
	float TypewriterElapsed = 0.0f;
	FString LastShownPhrase;

	void SetOpacity(float Opacity);
	void SetPhraseOpacity(float Opacity) const;

	// Begins showing a new phrase with the configured transition (called once the old one is gone/faded out).
	void BeginNewPhrase(const FString& NewPhrase);

	// Clears the phrase text entirely (used at the end of FadeOut).
	void ClearPhrase();
	
	// Starts a gradual opacity of the currently shown phrase
	void HidePhrase();
};