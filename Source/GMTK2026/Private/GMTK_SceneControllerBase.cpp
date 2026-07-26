#include "GMTK_SceneControllerBase.h"

#include "GMTK_GameFlowManager.h"
#include "GMTK_SceneDefinitionAsset.h"

AGMTK_SceneControllerBase::AGMTK_SceneControllerBase() { PrimaryActorTick.bCanEverTick = false; }

void AGMTK_SceneControllerBase::BeginPlay()
{
	Super::BeginPlay();
	ResetScene();
	UGMTK_GameFlowManager* GameFlowManager =
		GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>();
	if (GameFlowManager)
	{
		GameFlowManager->RegisterScene(this);
	}
}

void AGMTK_SceneControllerBase::ResetScene()
{
	bIsCompleted = false;
	CurrentState = SceneData ? SceneData->StartState : FGameplayTag();
}

void AGMTK_SceneControllerBase::HandleInteraction(FGameplayTag ActionID)
{
	UE_LOG(LogTemp, Log, TEXT("SceneControllerBase::HandleInteraction: ActionID = %s"), *ActionID.ToString());
	
	if (!SceneData || bIsCompleted) { return; }

	FSceneTransitionRow Row;
	if (!SceneData->FindTransition(CurrentState, ActionID, Row))
	{
		// No transition defined for this state+action
		return;
	}

	CurrentState = Row.ToState;

	// Notify linked event 
	OnStateChanged.Broadcast(Row.EventTag);

	if (Row.bIsFailure) { OnSceneFailed.Broadcast(Row.EventTag); }

	if (Row.bIsSuccess || CurrentState == SceneData->SuccessState)
	{
		bIsCompleted = true;
		OnSceneCompleted.Broadcast();
	}
}
