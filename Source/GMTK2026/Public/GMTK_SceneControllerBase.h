#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GMTK_SceneData.h"
#include "GMTK_SceneControllerBase.generated.h"

class UGMTK_SceneDefinitionAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneStateChanged, FGameplayTag, EventTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneFailed, FGameplayTag, FailEventTag);

UCLASS(Blueprintable)
class GMTK2026_API AGMTK_SceneControllerBase : public AActor
{
	GENERATED_BODY()

public:
	AGMTK_SceneControllerBase();

	// Define the states and transitions for this scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	TObjectPtr<UGMTK_SceneDefinitionAsset> SceneData;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	FGameplayTag CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	bool bIsCompleted = false;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	TArray<TSoftObjectPtr<ULevel>> LevelsToLoad;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	bool bSceneLocked = false;
	
	UPROPERTY(BlueprintAssignable, Category = "Scene")
	FOnSceneStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Scene")
	FOnSceneCompleted OnSceneCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Scene")
	FOnSceneFailed OnSceneFailed;

	// Called when the player interacts with an object
	UFUNCTION(BlueprintCallable, Category = "Scene")
	void HandleInteraction(FGameplayTag ActionID);

	// Reset Scenes when the player fails and wants to retry.
	UFUNCTION(BlueprintCallable, Category = "Scene")
	void ResetScene();

protected:
	virtual void BeginPlay() override;

	void PlaySequenceForEvent(FGameplayTag EventTag);
	
	UPROPERTY()
	TObjectPtr<class ULevelSequencePlayer> ActiveSequencePlayer;

private:
	FGameplayTagContainer ActionTags;
	
};
