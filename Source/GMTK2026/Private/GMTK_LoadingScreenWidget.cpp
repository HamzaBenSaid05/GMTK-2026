#include "GMTK_LoadingScreenWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UGMTK_LoadingScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetOpacity(0.0f);
	SetVisibility(ESlateVisibility::HitTestInvisible);
	ClearPhrase();
}

void UGMTK_LoadingScreenWidget::SetOpacity(float Opacity)
{
	CurrentOpacity = FMath::Clamp(Opacity, 0.0f, 1.0f);
	if (BlackImage) { BlackImage->SetRenderOpacity(CurrentOpacity); }
}

void UGMTK_LoadingScreenWidget::SetPhraseOpacity(float Opacity) const { if (PhraseText) { PhraseText->SetRenderOpacity(FMath::Clamp(Opacity, 0.0f, 1.0f)); } }

void UGMTK_LoadingScreenWidget::ClearPhrase()
{
	PhraseState = EPhraseState::Idle;
	CurrentPhraseString.Empty();
	PendingPhraseString.Empty();
	TypewriterCharIndex = 0;
	TypewriterElapsed = 0.0f;
	PhraseTransitionElapsed = 0.0f;

	if (PhraseText)
	{
		PhraseText->SetText(FText::GetEmpty());
		SetPhraseOpacity(0.0f);
	}
}

void UGMTK_LoadingScreenWidget::HidePhrase()
{
	// Nothing visible to hide.
	if (CurrentPhraseString.IsEmpty() && PhraseState == EPhraseState::Idle) { return; }

	PendingPhraseString.Empty(); // empty pending = "fade out and stop", not "fade out then show next"
	PhraseTransitionElapsed = 0.0f;
	PhraseState = EPhraseState::PhraseFadingOut;
}

void UGMTK_LoadingScreenWidget::ShowRandomPhrase()
{
	if (!PhraseText || LoadingPhrases.Num() == 0) { return; }

	FString NewPhrase;

	if (LoadingPhrases.Num() == 1)
	{
		// Only one phrase available: nothing to exclude, just use it.
		NewPhrase = LoadingPhrases[0].ToString();
	}
	else
	{
		// Pick a random phrase different from the one currently/previously shown.
		int32 Index;
		FString Candidate;
		do
		{
			Index = FMath::RandRange(0, LoadingPhrases.Num() - 1);
			Candidate = LoadingPhrases[Index].ToString();
		}
		while (Candidate == LastShownPhrase);

		NewPhrase = Candidate;
	}

	LastShownPhrase = NewPhrase;

	if (!CurrentPhraseString.IsEmpty() && PhraseTransitionMode == EPhraseTransitionMode::Fade)
	{
		PendingPhraseString = NewPhrase;
		PhraseState = EPhraseState::PhraseFadingOut;
		PhraseTransitionElapsed = 0.0f;
	}
	else { BeginNewPhrase(NewPhrase); }
}

void UGMTK_LoadingScreenWidget::BeginNewPhrase(const FString& NewPhrase)
{
	CurrentPhraseString = NewPhrase;
	PhraseTransitionElapsed = 0.0f;

	if (PhraseTransitionMode == EPhraseTransitionMode::Fade)
	{
		if (PhraseText) { PhraseText->SetText(FText::FromString(CurrentPhraseString)); }
		SetPhraseOpacity(0.0f);
		PhraseState = EPhraseState::PhraseFadingIn;
	}
	else // Typewriter
	{
		TypewriterCharIndex = 0;
		TypewriterElapsed = 0.0f;
		SetPhraseOpacity(1.0f);
		if (PhraseText) { PhraseText->SetText(FText::GetEmpty()); }
		PhraseState = EPhraseState::Typewriting;
	}
}

void UGMTK_LoadingScreenWidget::SetFullyOpaque()
{
	CurrentState = EFadeState::None;
	SetOpacity(1.0f);
	LastShownPhrase.Empty();
	ShowRandomPhrase();
	PhraseCycleElapsed = 0.0f;
	HoldElapsed = 0.0f;
}

void UGMTK_LoadingScreenWidget::FadeIn()
{
	CurrentState = EFadeState::FadingIn;
	FadeElapsed = 0.0f;
	OnFadeInStarted();
}

void UGMTK_LoadingScreenWidget::FadeOut()
{
	CurrentState = EFadeState::FadingOut;
	FadeElapsed = 0.0f;
	HidePhrase();
	OnFadeOutStarted();
}

void UGMTK_LoadingScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// --- Phrase text transition state machine ---
	switch (PhraseState)
	{
	case EPhraseState::PhraseFadingOut:
		{
			PhraseTransitionElapsed += InDeltaTime;
			const float Alpha = PhraseFadeDuration > 0.0f ? FMath::Clamp(PhraseTransitionElapsed / PhraseFadeDuration, 0.0f, 1.0f) : 1.0f;
			SetPhraseOpacity(1.0f - Alpha);
			if (Alpha >= 1.0f)
			{
				if (PendingPhraseString.IsEmpty())
				{
					// No new phrase queued: this was a "hide" (screen about to fade out), so just clear.
					ClearPhrase();
				}
				else
				{
					// Mid-hold phrase cycling: swap in the next one.
					BeginNewPhrase(PendingPhraseString);
				}
			}
			break;
		}
	case EPhraseState::PhraseFadingIn:
		{
			PhraseTransitionElapsed += InDeltaTime;
			const float Alpha = PhraseFadeDuration > 0.0f ? FMath::Clamp(PhraseTransitionElapsed / PhraseFadeDuration, 0.0f, 1.0f) : 1.0f;
			SetPhraseOpacity(Alpha);
			if (Alpha >= 1.0f) { PhraseState = EPhraseState::Idle; }
			break;
		}
	case EPhraseState::Typewriting:
		{
			TypewriterElapsed += InDeltaTime;
			if (TypewriterElapsed >= TypewriterLetterInterval && TypewriterCharIndex < CurrentPhraseString.Len())
			{
				TypewriterElapsed = 0.0f;
				TypewriterCharIndex++;
				if (PhraseText) { PhraseText->SetText(FText::FromString(CurrentPhraseString.Left(TypewriterCharIndex))); }
				if (TypewriterCharIndex >= CurrentPhraseString.Len()) { PhraseState = EPhraseState::Idle; }
			}
			break;
		}
	case EPhraseState::Idle:
	default:
		break;
	}

	// --- Phrase cycling while fully black ---
	if (bCyclePhrases && CurrentState == EFadeState::None && CurrentOpacity >= 1.0f && PhraseState == EPhraseState::Idle)
	{
		PhraseCycleElapsed += InDeltaTime;
		if (PhraseCycleElapsed >= CyclePhraseInterval)
		{
			PhraseCycleElapsed = 0.0f;
			ShowRandomPhrase();
		}
	}

	// --- Screen fade ---
	if (CurrentState == EFadeState::None) { return; }

	FadeElapsed += InDeltaTime;

	if (CurrentState == EFadeState::FadingIn)
	{
		const float Alpha = FadeInDuration > 0.0f ? FMath::Clamp(FadeElapsed / FadeInDuration, 0.0f, 1.0f) : 1.0f;
		SetOpacity(Alpha);

		// Debug
		if (GEngine)
		{
			UE_LOG(LogTemp, Warning,
			       TEXT("Elapsed: %.2f  Duration: %.2f  Alpha: %.2f"),
			       FadeElapsed,
			       FadeInDuration,
			       FadeElapsed / FadeInDuration);
		}

		if (Alpha >= 1.0f)
		{
			CurrentState = EFadeState::None;
			ShowRandomPhrase();
			PhraseCycleElapsed = 0.0f;
			HoldElapsed = 0.0f;
			OnFadeInComplete.Broadcast();
			FadeElapsed = 0.0f;
		}
	}
	else if (CurrentState == EFadeState::FadingOut)
	{
		const float Alpha = FadeOutDuration > 0.0f ? FMath::Clamp(FadeElapsed / FadeOutDuration, 0.0f, 1.0f) : 1.0f;
		SetOpacity(1.0f - Alpha);

		// Debug
		if (GEngine)
		{
			UE_LOG(LogTemp, Warning,
				   TEXT("Elapsed: %.2f  Duration: %.2f  Alpha: %.2f"),
				   FadeElapsed,
				   FadeOutDuration,
				   FadeElapsed / FadeOutDuration);
		}
		
		if (Alpha >= 1.0f)
		{
			CurrentState = EFadeState::None;
			FadeElapsed = 0.0f;
			ClearPhrase();
			OnFadeOutComplete.Broadcast();
		}
	}
}
