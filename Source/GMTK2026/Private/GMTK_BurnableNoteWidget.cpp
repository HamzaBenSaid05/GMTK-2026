#include "GMTK_BurnableNoteWidget.h"

#include "GMTK_GameFlowManager.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UGMTK_BurnableNoteWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (bIsPlayerWishNote)
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
		{
			if (const UGMTK_GameFlowManager* Flow = GI->GetSubsystem<UGMTK_GameFlowManager>())
			{
				const FString& WishText = Flow->PlayerWishText;

				if (WishTextBlock)
				{
					WishTextBlock->SetText(FText::FromString(WishText));
					if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(
						                                 -1,
						                                 15.0f,
						                                 FColor::Yellow,
						                                 FString::Printf(
						                                                 TEXT("The player wish is:%s"),
						                                                 *Flow->PlayerWishText
						                                                )
						                                );
					}
				}
				OnWishTextSet(WishText);
			}
		}
	}
}

void UGMTK_BurnableNoteWidget::ResetNote()
{
	bIsBurned = false;
	CurrentBurnTime = 0.0f;
}

void UGMTK_BurnableNoteWidget::SetNoteEnabled(bool bEnabled)
{
	// Set to SelfHitTestInvisible to allow the flame to overlap and trigger the burn logic, but not block clicks.
	SetVisibility(bEnabled ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UGMTK_BurnableNoteWidget::SetFlameOverlapping(bool bOverlapping, float DeltaTime)
{
	if (bOverlapping)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
			                                 -1,
			                                 15.0f,
			                                 FColor::Yellow,
			                                 FString::Printf(
			                                                 TEXT("%s is overlapping"),
			                                                 *GetName()
			                                                )
			                                );
		}
	}

	if (bIsBurned)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
			                                 -1,
			                                 1.f,
			                                 FColor::Yellow,
			                                 FString::Printf(
			                                                 TEXT("%s is burned: "),
			                                                 *GetName()
			                                                )
			                                );
		}
		return;
	}

	if (!bOverlapping)
	{
		CurrentBurnTime = 0.0f;
		return;
	}

	if (!bIsCorrectWish)
	{
		OnNoteBurnResult.Broadcast(false,WishID, bIsPlayerWishNote);
		OnBurnRejected();
		return;
	}

	CurrentBurnTime += DeltaTime;
	if (CurrentBurnTime >= RequiredBurnTime)
	{
		bIsBurned = true;
		OnNoteBurnResult.Broadcast(true,WishID, bIsPlayerWishNote);
		OnBurnSuccessVisual();
	}
}
