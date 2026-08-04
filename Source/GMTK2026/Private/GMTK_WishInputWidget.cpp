#include "GMTK_WishInputWidget.h"
#include "GMTK_GameFlowManager.h"
#include "Kismet/GameplayStatics.h"

void UGMTK_WishInputWidget::UpdateCurrentText(const FString& NewText)
{
	if (NewText.Len() > MaxCharacters)
	{
		CurrentText = NewText.Left(MaxCharacters);
		OnTextTruncated(CurrentText);
	}
	else
	{
		CurrentText = NewText;
	}
}

void UGMTK_WishInputWidget::SubmitWish()
{
	if (CurrentText.IsEmpty())
	{
		return; 
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UGMTK_GameFlowManager* Flow = GI->GetSubsystem<UGMTK_GameFlowManager>())
		{
			Flow->SetPlayerWish(CurrentText);
			//Flow->AdvanceToNextLevel();
		}
	}

	OnWishSubmitted.Broadcast(CurrentText);
	RemoveFromParent();
}
