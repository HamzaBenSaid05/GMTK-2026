#include "GMTK_FinalNotesController.h"
#include "GMTK_GameFlowManager.h"
#include "GMTK_SceneControllerBase.h"
#include "GMTK_SceneDefinitionAsset.h"

AGMTK_FinalNotesController::AGMTK_FinalNotesController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGMTK_FinalNotesController::BeginPlay()
{
	Super::BeginPlay();

	UGMTK_GameFlowManager* Flow = GetGameInstance() ? GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>() : nullptr;

	// Set the bIsCorrectWish of each SCENE ticket based on whether
	// that scene was actually completed during the game, matching
	for (AGMTK_BurnableNote* Note : SceneNotes)
	{
		if (!Note) continue;

		bool bCompleted = false;
		if (Flow)
		{
			for (const AGMTK_SceneControllerBase* Scene : Flow->Scenes)
			{
				if (Scene && Scene->SceneData && Scene->SceneData->WishID == Note->WishID)
				{
					bCompleted = Scene->bIsCompleted;
					break;
				}
			}
		}
		Note->bIsCorrectWish = bCompleted;
		Note->OnNoteBurnResult.AddDynamic(this, &AGMTK_FinalNotesController::HandleSceneNoteBurnResult);
	}

	// Player Note is always correct, so we set bIsCorrectWish to true.
	if (PlayerNote)
	{
		PlayerNote->bIsCorrectWish = true;
		PlayerNote->SetNoteEnabled(false);
	}
}

bool AGMTK_FinalNotesController::AreAllSceneNotesBurned() const
{
	for (const AGMTK_BurnableNote* Note : SceneNotes)
	{
		if (!Note || !Note->bIsBurned)
		{
			return false;
		}
	}
	return SceneNotes.Num() > 0;
}

void AGMTK_FinalNotesController::HandleSceneNoteBurnResult(bool bWasCorrect)
{
	// OnNoteBurnResult start only if the note is burned successfully, so we only check for success here.
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

void AGMTK_FinalNotesController::RequestRetryCheck()
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