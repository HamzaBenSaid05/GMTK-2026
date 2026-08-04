#pragma once

#include "CoreMinimal.h"
#include "GMTK_SceneControllerBase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GMTK_GameFlowManager.generated.h"

class AGMTK_SceneControllerBase;
class AGMTK_BurnableNote;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllScenesFinished, bool, bAllWishesCorrect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewLevelToLoad, int, LevelIndex);

UCLASS()
class GMTK2026_API UGMTK_GameFlowManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// All the scenes data
	UPROPERTY(BlueprintReadOnly, Category = "Flow")
	TArray<FSceneProgress> ScenesData;

	// Player start desire
	UPROPERTY(BlueprintReadWrite, Category = "Flow")
	FString PlayerWishText;

	UPROPERTY(BlueprintAssignable, Category = "Flow")
	FOnAllScenesFinished OnAllScenesFinished;

	UPROPERTY(BlueprintAssignable, Category = "Flow")
	FOnNewLevelToLoad OnNewLevelToLoad;

	UPROPERTY(BlueprintReadOnly, Category = "Flow|Levels")
	int32 CurrentLevelIndex = 0;

	// Levels (soft refs) whose scene was NOT completed correctly. Populated by GameMode
	// when a scene ends without reaching SuccessState.
	UPROPERTY(BlueprintReadOnly, Category = "Flow|Levels")
	TArray<TSoftObjectPtr<UWorld>> FailedLevels;

	// True while replaying only FailedLevels (as opposed to the initial full sequence).
	UPROPERTY(BlueprintReadOnly, Category = "Flow|Levels")
	bool bIsRetryPass = false;

	UPROPERTY()
	bool bRetryCompleted = false;
	
	// The level asset currently loaded/being played. Set by GameMode right before travel.
	UPROPERTY(BlueprintReadWrite, Category = "Flow|Levels")
	TSoftObjectPtr<UWorld> CurrentLevelAsset;

	UFUNCTION(BlueprintPure, Category = "Flow|Levels")
	bool HasFailedLevels() const { return FailedLevels.Num() > 0; }

	// Called by GameMode when the currently loaded scene ends (either success or timeout).
	// Updates FailedLevels accordingly.
	UFUNCTION(BlueprintCallable, Category = "Flow|Levels")
	void ResolveCurrentLevel(bool bSucceeded);

	// Called from the tickets screen "Retry" button. Starts replaying only FailedLevels.
	UFUNCTION(BlueprintCallable, Category = "Flow|Levels")
	void RequestRetry();

	// Decides which level to load next, given the game's full ordered level list.
	// Returns null when there's nothing left to play (go to FinalLevel / tickets screen).
	UFUNCTION(BlueprintCallable, Category = "Flow|Levels")
	TSoftObjectPtr<UWorld> GetNextLevelToLoad(const TArray<TSoftObjectPtr<UWorld>>& AllGameLevels);
	
	// Reset the game progress: clears the player wish and resets all scenes.
	UFUNCTION(BlueprintCallable, Category = "Flow|Levels")
	void ResetGameProgress();
	
	// Advance to the next level in the sequence.
	UFUNCTION(BlueprintCallable, Category = "Flow|Levels")
	void AdvanceToNextLevel();
	
	// Register the text written by the player on the initial Note.
	UFUNCTION(BlueprintCallable, Category = "Flow")
	void SetPlayerWish(const FString& WishText);

	// Called when all Notes have received a burn attempt: checks if they are ALL correct.
	UFUNCTION(BlueprintCallable, Category = "Flow")
	bool AreAllScenesCompleted() const;

	// Reset all the failed scenes so they can be retried.
	UFUNCTION(BlueprintCallable, Category = "Flow")
	void RetryFailedScenes();

	// Register scene
	UFUNCTION(BlueprintCallable, Category = "Flow")
	void RegisterScene(const FSceneProgress SceneProgress);
	UFUNCTION(BlueprintCallable, Category = "Flow")
	void UnRegisterScene(FGameplayTag WishID);

	UFUNCTION(BlueprintCallable, Category = "Flow")
	void SetSceneCompleted(FGameplayTag WishID, bool bWasCorrect);

	// Marks the ticket for this WishID as successfully burned (called from the tickets screen).
	UFUNCTION(BlueprintCallable, Category = "Flow")
	void MarkNoteBurned(FGameplayTag WishID);

	// True if at least one scene's ticket still needs to be burned.
	UFUNCTION(BlueprintPure, Category = "Flow")
	bool HasUnburnedNotes() const;

	UPROPERTY()
	TArray<TSoftObjectPtr<UWorld>> RetryQueue;
};
