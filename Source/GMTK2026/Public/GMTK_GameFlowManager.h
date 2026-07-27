#pragma once

#include "CoreMinimal.h"
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
	// All the scenes
	UPROPERTY(BlueprintReadWrite, Category = "Flow")
	TArray<TObjectPtr<AGMTK_SceneControllerBase>> Scenes;

	// Player start desire
	UPROPERTY(BlueprintReadWrite, Category = "Flow")
	FString PlayerWishText;

	UPROPERTY(BlueprintAssignable, Category = "Flow")
	FOnAllScenesFinished OnAllScenesFinished;

	UPROPERTY(BlueprintAssignable, Category = "Flow")
	FOnNewLevelToLoad OnNewLevelToLoad;

	UPROPERTY(BlueprintReadOnly, Category = "Flow|Levels")
	int32 CurrentLevelIndex = 0;

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
	void RegisterScene(AGMTK_SceneControllerBase* Scene);
};
