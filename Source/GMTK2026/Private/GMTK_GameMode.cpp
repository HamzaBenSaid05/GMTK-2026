#include "GMTK_GameMode.h"

#include "GMTK_PointAndClickPlayerController.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "GMTK_GameFlowManager.h"
#include "GMTK_GameSettings.h"
#include "GMTK_LoadingScreenWidget.h"
#include "Blueprint/UserWidget.h"

AGMTK_GameMode::AGMTK_GameMode()
{
	// No pawn to controll
	DefaultPawnClass = nullptr;

	// Point and Click controller
	PlayerControllerClass = AGMTK_PointAndClickPlayerController::StaticClass();
}

void AGMTK_GameMode::BeginPlay()
{
	Super::BeginPlay();

	UGMTK_GameFlowManager* Flow = GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>();
	Flow->OnNewLevelToLoad.AddDynamic(this, &AGMTK_GameMode::LoadLevelByIndex);
	Flow->CurrentLevelAsset = TSoftObjectPtr<UWorld>(
													 FSoftObjectPath(GetWorld()->GetOutermost()->GetName()));
	UE_LOG(LogTemp, Log,
		   TEXT("BeginPlay: LevelIndex %d (ScenesData Num=%d)"),
		   Flow->CurrentLevelIndex,
		   Flow->ScenesData.Num());

	UE_LOG(LogTemp, Warning, TEXT("=== BeginPlay on level, LoadingScreenWidgetClass=%s ==="),
		   LoadingScreenWidgetClass ? *LoadingScreenWidgetClass->GetName() : TEXT("NULL"));

	// Find the scene controller in this level (if any) and bind to its completion.
	if (AGMTK_SceneControllerBase* Scene = Cast<AGMTK_SceneControllerBase>(
																		   UGameplayStatics::GetActorOfClass(GetWorld(),
																			   AGMTK_SceneControllerBase::StaticClass())))
	{
		CurrentSceneController = Scene;
	}

	if (!CurrentSceneController)
		return;

	// Activate the camera for the game, either by tag or fallback to the first camera in the level.
	ActivateGameCamera();

	// Spawn the loading screen already fully black BEFORE anything else,
	// so the player never sees a flash of the fresh level underneath.
	if (LoadingScreenWidgetClass)
	{
		LoadingScreenWidget = CreateWidget<UGMTK_LoadingScreenWidget>(GetWorld(), LoadingScreenWidgetClass);
		if (LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport(1000);
			LoadingScreenWidget->SetFullyOpaque();
			LoadingScreenWidget->OnFadeOutComplete.AddUniqueDynamic(this, &AGMTK_GameMode::HandleFadeOutComplete);
		}
	}

	if (LoadingScreenWidget)
	{
		// Wait HoldDuration then fade out; the scene itself only starts once fade-out
		// is fully complete (see HandleFadeOutComplete -> StartScene).
		GetWorldTimerManager().SetTimer(
										FakeLoadHandle,
										this,
										&AGMTK_GameMode::RevealLevel,
										LoadingScreenWidget->HoldDuration,
										false
									   );
	}
	else
	{
		// No loading screen configured: start the scene immediately.
		StartScene();
	}
}

void AGMTK_GameMode::ActivateGameCamera() const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) { return; }

	// Take all cameras in the level 
	TArray<AActor*> AllCameras;
	UGameplayStatics::GetAllActorsOfClass(this, ACameraActor::StaticClass(), AllCameras);

	AActor* TargetCamera = nullptr;

	// Find the first camera with the specified tag
	for (AActor* Camera : AllCameras)
	{
		if (Camera && Camera->ActorHasTag(GameCameraTag))
		{
			TargetCamera = Camera;
			break;
		}
	}

	// Fallback: no camera with the specified tag found, use the first available one.
	if (!TargetCamera && bFallbackToFirstCameraInLevel && AllCameras.Num() > 0) { TargetCamera = AllCameras[0]; }

	if (TargetCamera)
	{
		// Instant blend 
		PC->SetViewTargetWithBlend(TargetCamera, 0.0f);
	}
}

void AGMTK_GameMode::StartMainTimer()
{
	OnTimerStart.Broadcast();

	RemainingTime = FMath::CeilToInt(TimerLength);

	// First update immediately
	OnTimerUpdate.Broadcast(RemainingTime);

	GetWorldTimerManager().SetTimer(
	                                TimerHandle,
	                                this,
	                                &AGMTK_GameMode::UpdateMainTimer,
	                                1.0f,
	                                true
	                               );
}

void AGMTK_GameMode::UpdateMainTimer()
{
	RemainingTime--;

	if (RemainingTime > 0) { OnTimerUpdate.Broadcast(RemainingTime); }
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);

		OnTimerUpdate.Broadcast(0);
		OnTimerFinish.Broadcast();

		GetWorldTimerManager().SetTimer(
		                                DelayFinishHandle,
		                                [this]()
		                                {
			                                if (!IsValid(CurrentSceneController))
			                                {
				                                UE_LOG(LogTemp, Warning, TEXT("UpdateMainTimer timeout: CurrentSceneController invalid, skipping."));
				                                HandleCurrentSceneCompleted();
				                                return;
			                                }
			                                HandleCurrentSceneCompleted();
		                                },
		                                DelayFinish,
		                                false
		                               );
	}
}

void AGMTK_GameMode::ResolveAndAdvance(bool bSucceeded)
{
	UE_LOG(LogTemp, Warning, TEXT("ResolveAndAdvance called, bSceneResolved=%d, bSucceeded=%d"), bSceneResolved, bSucceeded);

	if (bSceneResolved)
	{
		UE_LOG(LogTemp, Warning, TEXT("  -> BLOCKED by guard (would have double-advanced!)"));
		return;
	}
	bSceneResolved = true;

	if (UGMTK_GameFlowManager* Flow = GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>())
	{
		Flow->ResolveCurrentLevel(bSucceeded);
		Flow->AdvanceToNextLevel();
	}
}

void AGMTK_GameMode::HandleFadeInComplete()
{
	if (LoadingScreenWidget) { LoadingScreenWidget->OnFadeInComplete.RemoveDynamic(this, &AGMTK_GameMode::HandleFadeInComplete); }
	OpenPendingLevel();
}

void AGMTK_GameMode::HandleFadeOutComplete()
{
	if (LoadingScreenWidget) { LoadingScreenWidget->OnFadeOutComplete.RemoveDynamic(this, &AGMTK_GameMode::HandleFadeOutComplete); }
	StartScene();
}

void AGMTK_GameMode::OpenPendingLevel()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), PendingLevelAsset);
}

void AGMTK_GameMode::RevealLevel() { if (LoadingScreenWidget) { LoadingScreenWidget->FadeOut(); } }

void AGMTK_GameMode::StartScene()
{
	if (!CurrentSceneController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Scene Controller NULL"));
		return;
	}

	if (bSceneStarted) { return; }
	bSceneStarted = true;

	OnDelayStart.Broadcast();

	GetWorldTimerManager().SetTimer(
	                                DelayHandle,
	                                this,
	                                &AGMTK_GameMode::StartMainTimer,
	                                DelayTimer,
	                                false
	                               );
}

void AGMTK_GameMode::HandleCurrentSceneCompleted()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().ClearTimer(DelayHandle);
	GetWorldTimerManager().ClearTimer(DelayFinishHandle);

	ResolveAndAdvance(CurrentSceneController->SceneProgress.bIsCompleted);
}

void AGMTK_GameMode::LoadLevelByIndex(int32 LevelIndex)
{
	UGMTK_GameFlowManager* Flow = GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>();
	if (!Flow) { return; }

	const UGMTK_GameSettings* GameSettings = GetDefault<UGMTK_GameSettings>();
	PendingLevelAsset = Flow->GetNextLevelToLoad(GameSettings->GameLevels);
	if (PendingLevelAsset.IsNull()) { PendingLevelAsset = GameSettings->FinalLevel; }

	if (!LoadingScreenWidget)
	{
		OpenPendingLevel();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("LoadLevelByIndex: LevelIndex %d, PendingLevelAsset: %s"), LevelIndex, *PendingLevelAsset.ToString());

	LoadingScreenWidget->OnFadeInComplete.AddUniqueDynamic(this, &AGMTK_GameMode::HandleFadeInComplete);
	LoadingScreenWidget->FadeIn();
}
