#include "GMTK_SceneControllerBase.h"

#include "GMTK_GameFlowManager.h"
#include "GMTK_SceneDefinitionAsset.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"

AGMTK_SceneControllerBase::AGMTK_SceneControllerBase() { PrimaryActorTick.bCanEverTick = false; }

void AGMTK_SceneControllerBase::BeginPlay()
{
	Super::BeginPlay();
	ResetScene();
	UGMTK_GameFlowManager* GameFlowManager =
		GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>();
	if (GameFlowManager) { GameFlowManager->RegisterScene(this); }
}

void AGMTK_SceneControllerBase::ResetScene()
{
	bIsCompleted = false;
	bSceneLocked = false;
	CurrentState = SceneData ? SceneData->StartState : FGameplayTag();
}

void AGMTK_SceneControllerBase::HandleInteraction(FGameplayTag ActionID)
{
	UE_LOG(LogTemp, Log, TEXT("SceneControllerBase::HandleInteraction: ActionID = %s"), *ActionID.ToString());

	if (!SceneData || bIsCompleted || ActionTags.HasTagExact(ActionID)) { return; }

	FSceneTransitionRow Row;
	if (!SceneData->FindTransition(CurrentState, ActionID, Row))
	{
		// No transition defined for this state+action
		return;
	}

	CurrentState = Row.ToState;

	// Notify linked event
	OnStateChanged.Broadcast(Row.EventTag);

	// Play sequence if defined for this event
	PlaySequenceForEvent(Row.EventTag);
	
	// Add tag to prevent repeated actions
	ActionTags.AddTag(ActionID);

	if (Row.bIsFailure) { OnSceneFailed.Broadcast(Row.EventTag); }

	if (Row.bEndsScene) { bSceneLocked = true; }

	if (Row.bIsSuccess || CurrentState == SceneData->SuccessState)
	{
		bIsCompleted = true;
		OnSceneCompleted.Broadcast();
	}
}

void AGMTK_SceneControllerBase::PlaySequenceForEvent(FGameplayTag EventTag)
{
	if (!SceneData || !EventTag.IsValid()) { return; }

	const TSoftObjectPtr<ULevelSequence>* SequencePtr = SceneData->EventSequences.Find(EventTag);
	if (!SequencePtr) { return; }

	ULevelSequence* Sequence = SequencePtr->LoadSynchronous();
	if (!Sequence) { return; }

	FMovieSceneSequencePlaybackSettings PlaybackSettings;
	ALevelSequenceActor* OutActor = nullptr;
	ActiveSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this, Sequence, PlaybackSettings, OutActor);

	if (ActiveSequencePlayer)
	{
		// Print to screen
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.0f,
				FColor::Yellow,
				FString::Printf(
					TEXT("SceneControllerBase::PlaySequenceForEvent %s: Playing sequence %s"),
					*EventTag.ToString(),
					*Sequence->GetName()
				)
			);
		}
		ActiveSequencePlayer->Play();
	}
}
