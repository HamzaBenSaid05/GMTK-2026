#include "GMTK_GameFlowManager.h"
#include "GMTK_SceneControllerBase.h"
void UGMTK_GameFlowManager::ResetGameProgress()
{
	CurrentLevelIndex = 0;
	Scenes.Empty();

	OnNewLevelToLoad.Broadcast(CurrentLevelIndex);
}

void UGMTK_GameFlowManager::AdvanceToNextLevel()
{
	CurrentLevelIndex++;
	OnNewLevelToLoad.Broadcast(CurrentLevelIndex);
}

void UGMTK_GameFlowManager::SetPlayerWish(const FString& WishText) { PlayerWishText = WishText; }

void UGMTK_GameFlowManager::RegisterScene(AGMTK_SceneControllerBase* Scene) { if (Scene) { Scenes.AddUnique(Scene); } }

bool UGMTK_GameFlowManager::AreAllScenesCompleted() const
{
	for (const AGMTK_SceneControllerBase* Scene : Scenes) { if (!Scene || !Scene->bIsCompleted) { return false; } }
	return Scenes.Num() > 0;
}

void UGMTK_GameFlowManager::RetryFailedScenes()
{
	for (AGMTK_SceneControllerBase* Scene : Scenes) { if (Scene && !Scene->bIsCompleted) { Scene->ResetScene(); } }
}
