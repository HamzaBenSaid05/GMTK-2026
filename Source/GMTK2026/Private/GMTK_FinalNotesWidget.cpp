#include "GMTK_FinalNotesWidget.h"

#include "GMTK_BurnableNoteWidget.h"
#include "GMTK_LighterWidget.h"
#include "GMTK_GameFlowManager.h"
#include "GMTK_SceneControllerBase.h"
#include "GMTK_SceneDefinitionAsset.h"
#include "Blueprint/WidgetTree.h"

void UGMTK_FinalNotesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Find all UGMTK_BurnableNoteWidget placed inside this Widget Blueprint,
	// except PlayerNote which is managed separately
	if (WidgetTree)
	{
		WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if (UGMTK_BurnableNoteWidget* Note = Cast<UGMTK_BurnableNoteWidget>(Widget))
			{
				if (Note != PlayerNote)
				{
					SceneNotes.AddUnique(Note);
				}
			}
		});
	}

	UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr;

	for (UGMTK_BurnableNoteWidget* Note : SceneNotes)
	{
		if (!Note) continue;

		bool bCompleted = false;
		if (Flow)
		{
			for (const FSceneProgress& Scene : Flow->ScenesData)
			{
				if (Scene.SceneData && Scene.SceneData->WishID == Note->WishID)
				{
					bCompleted = Scene.bIsCompleted;
					break;
				}
			}
		}
		Note->bIsCorrectWish = bCompleted;
		Note->OnNoteBurnResult.AddDynamic(this, &UGMTK_FinalNotesWidget::HandleSceneNoteBurnResult);

		if (LighterWidget)
		{
			LighterWidget->RegisterNote(Note);
		}
	}

	// PlayerNote burn always but is disabled until all other notes are correct.
	if (PlayerNote)
	{
		PlayerNote->bIsCorrectWish = true;
		PlayerNote->SetNoteEnabled(false);

		if (LighterWidget)
		{
			LighterWidget->RegisterNote(PlayerNote);
		}
	}
}

bool UGMTK_FinalNotesWidget::AreAllSceneNotesBurned() const
{
	for (const UGMTK_BurnableNoteWidget* Note : SceneNotes)
	{
		if (!Note || !Note->bIsBurned)
		{
			return false;
		}
	}
	return SceneNotes.Num() > 0;
}

void UGMTK_FinalNotesWidget::HandleSceneNoteBurnResult(bool bWasCorrect)
{
	// On NoteBurnResult is triggered only on success (see BurnableNoteWidget), so
	// we just need to check if with this one all notes are burned.
	if (!bWasCorrect)
	{
		return;
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

void UGMTK_FinalNotesWidget::RequestRetryCheck()
{
	if (AreAllSceneNotesBurned())
	{
		OnAllSceneNotesCorrect();
	}
	else
	{
		OnSomeSceneNotesWrong();
	}
}