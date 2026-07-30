#include "GMTK_GameFlowManager.h"

#include "GMTK_SceneControllerBase.h"

void UGMTK_GameFlowManager::ResetGameProgress()
{
	CurrentLevelIndex = 0;
	ScenesData.Empty();

	OnNewLevelToLoad.Broadcast(CurrentLevelIndex);
}

void UGMTK_GameFlowManager::AdvanceToNextLevel()
{
	CurrentLevelIndex++;
	OnNewLevelToLoad.Broadcast(CurrentLevelIndex);
}

void UGMTK_GameFlowManager::SetPlayerWish(const FString& WishText)
{
	PlayerWishText = WishText;
}

void UGMTK_GameFlowManager::RegisterScene(const FSceneProgress SceneProgress)
{
	ScenesData.Add(SceneProgress); 
}

void UGMTK_GameFlowManager::SetSceneCompleted(FGameplayTag WishID, bool bWasCorrect)
{
	// Find the scene in the array and update its completion status
	for (FSceneProgress& Scene : ScenesData)
	{
		if (Scene.SceneData->WishID == WishID)
		{
			Scene.bIsCompleted = true;
			break;
		}
	}

	// Check if all scenes are completed
	if (AreAllScenesCompleted())
	{
		OnAllScenesFinished.Broadcast(bWasCorrect);
	}
}

bool UGMTK_GameFlowManager::AreAllScenesCompleted() const
{
	for (const FSceneProgress Scene : ScenesData)
	{
		if (!Scene.bIsCompleted) { return false; }
	}
	return ScenesData.Num() > 0;
}

void UGMTK_GameFlowManager::RetryFailedScenes()
{
	for (FSceneProgress Scene : ScenesData)
	{
		if (!Scene.bIsCompleted)
		{
			Scene.ResetScene();
		}
	}
}
