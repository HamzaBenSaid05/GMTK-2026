#include "GMTK_GameFlowManager.h"

#include "GMTK_SceneControllerBase.h"

void UGMTK_GameFlowManager::ResolveCurrentLevel(bool bSucceeded)
{
	if (CurrentLevelAsset.IsNull())
	{
		return;
	}

	if (bSucceeded)
	{
		FailedLevels.Remove(CurrentLevelAsset);
	}
	else
	{
		FailedLevels.AddUnique(CurrentLevelAsset);
	}
}

void UGMTK_GameFlowManager::RequestRetry()
{
	if (FailedLevels.Num() == 0)
	{
		return;
	}
	RetryQueue = FailedLevels;
	bIsRetryPass = true;	

	OnNewLevelToLoad.Broadcast(CurrentLevelIndex); // param unused now, kept for delegate compatibility
}

TSoftObjectPtr<UWorld> UGMTK_GameFlowManager::GetNextLevelToLoad(const TArray<TSoftObjectPtr<UWorld>>& AllGameLevels)
{
	if (bIsRetryPass)
	{
		if (FailedLevels.Num() > 0)
		{
			if (RetryQueue.Num() > 0)
			{
				TSoftObjectPtr<UWorld> Next = RetryQueue[0];
				RetryQueue.RemoveAt(0);
				return Next;
			}
		}
		bIsRetryPass = false;

		return nullptr; // retry pass finished -> back to tickets screen
	}

	if (CurrentLevelIndex < AllGameLevels.Num())
	{
		return AllGameLevels[CurrentLevelIndex];
	}

	return nullptr; // first pass finished -> tickets screen (regardless of failures)
}

void UGMTK_GameFlowManager::ResetGameProgress()
{
	CurrentLevelIndex = 0;
	ScenesData.Empty();

	OnNewLevelToLoad.Broadcast(CurrentLevelIndex);
}

void UGMTK_GameFlowManager::AdvanceToNextLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("AdvanceToNextLevel: CurrentLevelIndex BEFORE=%d, bIsRetryPass=%d"), CurrentLevelIndex, bIsRetryPass);

	if (!bIsRetryPass)
	{
		CurrentLevelIndex++;
	}
	UE_LOG(LogTemp, Warning, TEXT("AdvanceToNextLevel: CurrentLevelIndex AFTER=%d"), CurrentLevelIndex);

	OnNewLevelToLoad.Broadcast(CurrentLevelIndex);
}

void UGMTK_GameFlowManager::SetPlayerWish(const FString& WishText)
{
	PlayerWishText = WishText;
}

void UGMTK_GameFlowManager::RegisterScene(const FSceneProgress SceneProgress)
{
	for (FSceneProgress& Existing : ScenesData)
	{
		if (Existing.SceneData && SceneProgress.SceneData &&
			Existing.SceneData->WishID == SceneProgress.SceneData->WishID)
		{
			const bool bPreservedNoteBurned = Existing.bNoteBurned;
			Existing = SceneProgress;
			Existing.bNoteBurned = bPreservedNoteBurned;
			return;
		}
	}
	ScenesData.Add(SceneProgress);
}

void UGMTK_GameFlowManager::UnRegisterScene(FGameplayTag WishID)
{
	ScenesData.RemoveAll(
		[WishID](const FSceneProgress& Scene)
		{
			return Scene.SceneData &&
				   Scene.SceneData->WishID == WishID;
		}
	);
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

void UGMTK_GameFlowManager::MarkNoteBurned(FGameplayTag WishID)
{
	for (FSceneProgress& Scene : ScenesData)
	{
		if (Scene.SceneData && Scene.SceneData->WishID == WishID)
		{
			Scene.bNoteBurned = true;
			break;
		}
	}
}

bool UGMTK_GameFlowManager::HasUnburnedNotes() const
{
	for (const FSceneProgress& Scene : ScenesData)
	{
		if (!Scene.bNoteBurned)
		{
			return true;
		}
	}
	return ScenesData.Num() > 0 ? false : true; 
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
	for (FSceneProgress& Scene : ScenesData)
	{
		if (!Scene.bIsCompleted)
		{
			Scene.ResetScene();
		}
	}
}
