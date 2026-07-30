#include "GMTK_GameMode.h"
#include "GMTK_PointAndClickPlayerController.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "GMTK_GameFlowManager.h"
#include "GMTK_SceneControllerBase.h"

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
	ActivateGameCamera();
	
	// The list is copied into the subsystem ONLY if it is still empty
	UGMTK_GameFlowManager* Flow = GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>();
	Flow->OnNewLevelToLoad.AddDynamic(this, &AGMTK_GameMode::LoadLevelByIndex);

	// Bind on scene completed events for all scene controllers in the level
	//if (Flow->Scenes.IsValidIndex(Flow->CurrentLevelIndex))
	//{
	//	Flow->Scenes[Flow->CurrentLevelIndex]->OnSceneCompleted.AddDynamic(
	//		this,
	//		&AGMTK_GameMode::HandleCurrentSceneCompleted);
	//}
	//else
	//{
		UE_LOG(LogTemp, Error,
			TEXT("Invalid Scene Index %d (Num=%d)"),
			Flow->CurrentLevelIndex,
			Flow->ScenesData.Num());
	//}

	
	// Start the delay timer
	OnDelayStart.Broadcast();

	GetWorldTimerManager().SetTimer(
		DelayHandle,
		this,
		&AGMTK_GameMode::StartMainTimer,
		DelayTimer,
		false
	);
}

void AGMTK_GameMode::ActivateGameCamera() const
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

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
	if (!TargetCamera && bFallbackToFirstCameraInLevel && AllCameras.Num() > 0)
	{
		TargetCamera = AllCameras[0];
	}

	if (TargetCamera)
	{
		// Instant blend 
		PC->SetViewTargetWithBlend(TargetCamera, 0.0f);
	}
}

void AGMTK_GameMode::StartMainTimer()
{
	OnDelayFinish.Broadcast();

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

	if (RemainingTime > 0)
	{
		OnTimerUpdate.Broadcast(RemainingTime);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);

		OnTimerUpdate.Broadcast(0);
		OnTimerFinish.Broadcast();

		FTimerHandle DelayFinishHandle;
		GetWorldTimerManager().SetTimer(
			DelayFinishHandle,
			[this]()
			{
				if (UGMTK_GameFlowManager* Flow = GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>())
				{
					Flow->AdvanceToNextLevel();
				}
			},
			DelayFinish, 
			false
		);
	}
}

void AGMTK_GameMode::HandleCurrentSceneCompleted()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);
	GetWorldTimerManager().ClearTimer(DelayHandle);

	if (UGMTK_GameFlowManager* Flow = GetGameInstance()->GetSubsystem<UGMTK_GameFlowManager>())
	{
		Flow->AdvanceToNextLevel();
	}
}

void AGMTK_GameMode::LoadLevelByIndex(int32 LevelIndex)
{
	if (LevelIndex >= LevelSequence.Num())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), FinalLevel);
		return;
	}
	UGameplayStatics::OpenLevelBySoftObjectPtr(
								   GetWorld(),
								   LevelSequence[LevelIndex]
								  );
}
