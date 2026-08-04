#include "GMTK_FinalNotesWidget.h"

#include "GMTK_BurnableNoteWidget.h"
#include "GMTK_LighterWidget.h"
#include "GMTK_GameFlowManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/NamedSlot.h"
#include "Components/TextBlock.h"

void UGMTK_FinalNotesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SceneNotes.Reset();
	SpawnUnburnedSceneNotes();

	const UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr;

	// PlayerNote is only relevant once EVERY scene ticket has been burned.
	if (PlayerNote)
	{
		PlayerNote->bIsCorrectWish = true;
		PlayerNote->SetNoteEnabled(Flow ? !Flow->HasUnburnedNotes() : false);
		PlayerNote->OnNoteBurnResult.AddDynamic(this, &UGMTK_FinalNotesWidget::HandleSceneNoteBurnResult);

		if (LighterWidget)
		{
			LighterWidget->RegisterNote(PlayerNote);
		}
	}
}

void UGMTK_FinalNotesWidget::SpawnUnburnedSceneNotes()
{
	if (!WidgetTree || !SceneNoteWidgetClass)
	{
		return;
	}

	UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr;
	if (!Flow)
	{
		return;
	}

	for (const FSceneProgress& Scene : Flow->ScenesData)
	{
		if (Scene.bNoteBurned || !Scene.SceneData)
		{
			continue;
		}

		const FGameplayTag WishID = Scene.SceneData->WishID;
		const FName* SlotName = NoteSlotNames.Find(WishID);
		if (!SlotName)
		{
			UE_LOG(LogTemp, Warning, TEXT("No slot configured for WishID %s, skipping note spawn."), *WishID.ToString());
			continue;
		}

		UNamedSlot* TargetSlot = Cast<UNamedSlot>(WidgetTree->FindWidget(*SlotName));
		if (!TargetSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("Named Slot '%s' not found in this Widget Blueprint."), *SlotName->ToString());
			continue;
		}

		UGMTK_BurnableNoteWidget* Note = CreateWidget<UGMTK_BurnableNoteWidget>(this, SceneNoteWidgetClass);
		if (!Note)
		{
			continue;
		}	
		Note->WishTextBlock->SetText(FText::FromName(Scene.WishText));
		Note->WishID = WishID;
		Note->bIsCorrectWish = Scene.bIsCompleted;
		Note->OnNoteBurnResult.AddDynamic(this, &UGMTK_FinalNotesWidget::HandleSceneNoteBurnResult);

		TargetSlot->SetContent(Note);

		SceneNotes.AddUnique(Note);

		if (LighterWidget)
		{
			LighterWidget->RegisterNote(Note);
		}
	}
	OnSceneNoteSpawned();
}

void UGMTK_FinalNotesWidget::HandleSceneNoteBurnResult(bool bWasCorrect, FGameplayTag WishID, bool bIsPlayerNote)
{
	if (!bWasCorrect)
	{
		return;
	}

	if (bIsPlayerNote)
	{
		OnPlayerNoteBurned();
		return;
	}
	
	// Find which note was just burned to mark it as burned persistently
	if (UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr)
	{
		Flow->MarkNoteBurned(WishID);
	}

	if (AreAllSceneNotesBurned())
	{
		if (PlayerNote)
		{
			PlayerNote->SetNoteEnabled(true);
		}
		OnAllSceneNotesCorrect();
	}
}

bool UGMTK_FinalNotesWidget::AreAllSceneNotesBurned() const
{
	// All burned means: no scene left with bNoteBurned false,
	// looking at the persistent state in Flow, not just the widgets
	// currently spawned in this visit.
	const UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr;
	return Flow ? !Flow->HasUnburnedNotes() : false;
}

void UGMTK_FinalNotesWidget::StartRetryOfFailedScenes()
{
	if (UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr)
	{
		Flow->RequestRetry();
	}
}

void UGMTK_FinalNotesWidget::RestartAllScenes()
{
	if (UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr)
	{
		Flow->ResetGameProgress();
	}
}
