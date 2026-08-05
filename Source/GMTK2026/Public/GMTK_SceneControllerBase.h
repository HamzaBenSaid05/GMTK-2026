#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GMTK_SceneDefinitionAsset.h"
#include "GMTK_SceneControllerBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneStateChanged, FGameplayTag, EventTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSceneFailed, FGameplayTag, FailEventTag);

USTRUCT(BlueprintType)
struct FSceneProgress
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	TObjectPtr<UGMTK_SceneDefinitionAsset> SceneData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	FName WishText;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	FGameplayTag CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	bool bIsCompleted = false;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	bool bSceneLocked = false;

	// True once this scene's ticket has been successfully burned in the tickets screen.
	UPROPERTY()
	bool bNoteBurned = false;
	
	// Reset Scenes when the player fails and wants to retry.
	void ResetScene()
	{
		bIsCompleted = false;
		bSceneLocked = false;
		CurrentState = SceneData ? SceneData->StartState : FGameplayTag();
	}
};

UCLASS(Blueprintable)
class GMTK2026_API AGMTK_SceneControllerBase : public AActor
{
	GENERATED_BODY()

public:
	AGMTK_SceneControllerBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	FSceneProgress SceneProgress;

	UPROPERTY(BlueprintReadOnly, Category = "Scene")
	TArray<TSoftObjectPtr<ULevel>> LevelsToLoad;

	UPROPERTY(BlueprintAssignable, Category = "Scene")
	FOnSceneStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Scene")
	FOnSceneCompleted OnSceneCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Scene")
	FOnSceneFailed OnSceneFailed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene")
	TObjectPtr<class ULevelSequencePlayer> ActiveSequencePlayer;

	// Called when the player interacts with an object
	UFUNCTION(BlueprintCallable, Category = "Scene")
	void HandleInteraction(FGameplayTag ActionID);

protected:
	virtual void BeginPlay() override;

	void PlaySequenceForEvent(FGameplayTag EventTag);
	
private:
	FGameplayTagContainer ActionTags;
};
