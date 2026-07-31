#include "GMTK_SceneControllerBase.h"

#include "GMTK_GameFlowManager.h"
#include "GMTK_GameMode.h"
#include "GMTK_GameSettings.h"
#include "GMTK_SceneDefinitionAsset.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"

AGMTK_SceneControllerBase::AGMTK_SceneControllerBase() { PrimaryActorTick.bCanEverTick = false; }

void AGMTK_SceneControllerBase::BeginPlay()
{
	Super::BeginPlay();
	SceneProgress.ResetScene();

	UGMTK_GameFlowManager* GameFlowManager =
		GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>();
	if (GameFlowManager) { GameFlowManager->RegisterScene(SceneProgress); }
	AGMTK_GameMode* GameMode = Cast<AGMTK_GameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode) { GameMode->OnTimerFinish.AddDynamic(this, &AGMTK_SceneControllerBase::UpdateSceneGameConfig); }
}

void AGMTK_SceneControllerBase::HandleInteraction(FGameplayTag ActionID)
{
	UE_LOG(LogTemp, Log, TEXT("SceneControllerBase::HandleInteraction: ActionID = %s"), *ActionID.ToString());

	if (!SceneProgress.SceneData || SceneProgress.bIsCompleted || ActionTags.HasTagExact(ActionID)) { return; }

	FSceneTransitionRow Row;
	if (!SceneProgress.SceneData->FindTransition(SceneProgress.CurrentState, ActionID, Row))
	{
		// No transition defined for this state+action
		return;
	}

	SceneProgress.CurrentState = Row.ToState;

	// Notify linked event
	OnStateChanged.Broadcast(Row.EventTag);

	// Play sequence if defined for this event
	PlaySequenceForEvent(Row.EventTag);

	// Add tag to prevent repeated actions
	ActionTags.AddTag(ActionID);

	if (Row.bIsFailure) { OnSceneFailed.Broadcast(Row.EventTag); }

	if (Row.bEndsScene) { SceneProgress.bSceneLocked = true; }

	if (Row.bIsSuccess || SceneProgress.CurrentState == SceneProgress.SceneData->SuccessState)
	{
		SceneProgress.bIsCompleted = true;
		UGMTK_GameFlowManager* GameFlowManager =
			GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>();
		if (GameFlowManager) { GameFlowManager->SetSceneCompleted(SceneProgress.SceneData->WishID, SceneProgress.bIsCompleted); }
		OnSceneCompleted.Broadcast();
		UE_LOG(LogTemp, Log, TEXT("Scene Completed"));
	}
}

void AGMTK_SceneControllerBase::PlaySequenceForEvent(FGameplayTag EventTag)
{
	if (!SceneProgress.SceneData || !EventTag.IsValid()) { return; }

	const TSoftObjectPtr<ULevelSequence>* SequencePtr = SceneProgress.SceneData->EventSequences.Find(EventTag);
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

void AGMTK_SceneControllerBase::UpdateSceneGameConfig()
{
	if (!SceneProgress.bIsCompleted)
	{
		return;
	}

	UGMTK_GameSettings* Settings = GetMutableDefault<UGMTK_GameSettings>();

	if (!Settings)
	{
		return;
	}

	UWorld* CurrentWorld = GetWorld();

	if (!CurrentWorld)
	{
		return;
	}


	FSoftObjectPath CurrentPath(
		CurrentWorld->GetOutermost()->GetName()
	);


	for (int32 i = Settings->Levels.Num() - 1; i >= 0; --i)
	{
		if (Settings->Levels[i].ToSoftObjectPath() == CurrentPath)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Removing completed level: %s"),
				*CurrentPath.ToString()
			);


			Settings->Levels.RemoveAt(i);
			break;
		}
	}

	Settings->SaveConfig();

	if (UGMTK_GameFlowManager* Flow =
		GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>())
	{
		Flow->UnRegisterScene(SceneProgress.SceneData->WishID);
	}
}