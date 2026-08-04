#pragma once

#include "CoreMinimal.h"
#include "GMTK_LoadingScreenWidget.h"
#include "GMTK_SceneControllerBase.h"
#include "GameFramework/GameMode.h"
#include "GMTK_GameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDelayStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerStart);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerUpdate, int, TimerLength);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerFinish);

UCLASS()
class GMTK2026_API AGMTK_GameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AGMTK_GameMode();

	// Camera Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game|Camera")
	FName GameCameraTag = FName(TEXT("GameCamera"));

	// If true use the first camera found in the level if no camera has the tag above. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Camera")
	bool bFallbackToFirstCameraInLevel = true;

	// Delay and Timer delegates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Timer")
	float DelayTimer = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Timer")
	float TimerLength = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game|Timer")
	float DelayFinish = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game|Loading")
	TSubclassOf<UGMTK_LoadingScreenWidget> LoadingScreenWidgetClass;

	// Delegates for Blueprint binding
	UPROPERTY(BlueprintAssignable, Category = "PointAndClick")
	FOnDelayStart OnDelayStart;
	UPROPERTY(BlueprintAssignable, Category = "PointAndClick")
	FOnTimerStart OnTimerStart;
	UPROPERTY(BlueprintAssignable, Category = "PointAndClick")
	FOnTimerUpdate OnTimerUpdate;
	UPROPERTY(BlueprintAssignable, Category = "PointAndClick")
	FOnTimerFinish OnTimerFinish;
protected:
	virtual void BeginPlay() override;

private:
	// Find the correct camera by tag, then fallback
	void ActivateGameCamera() const;

	// Delay and Timer functions
	void StartMainTimer();
	void UpdateMainTimer();

	FTimerHandle DelayHandle;
	FTimerHandle TimerHandle;
	FTimerHandle DelayFinishHandle;
	
	int32 RemainingTime;

	UPROPERTY()
	TObjectPtr<UGMTK_LoadingScreenWidget> LoadingScreenWidget;

	FTimerHandle FakeLoadHandle;

	// True once the scene has actually been "started"
	bool bSceneStarted = false;
	
	UPROPERTY()
	TObjectPtr<AGMTK_SceneControllerBase> CurrentSceneController;

	TSoftObjectPtr<UWorld> PendingLevelAsset;

	bool bSceneResolved = false;

	void ResolveAndAdvance(bool bSucceeded);
	
	UFUNCTION()
	void HandleFadeInComplete();
	
	UFUNCTION()
	void HandleFadeOutComplete();

	void OpenPendingLevel();
	void RevealLevel();
	void StartScene();
	
	UFUNCTION()
	void HandleCurrentSceneCompleted();

	UFUNCTION()
	void LoadLevelByIndex(int32 LevelIndex);
};
