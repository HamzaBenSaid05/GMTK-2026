#pragma once

#include "CoreMinimal.h"
#include "GMTK_GameplayTags.h"
#include "Engine/DataAsset.h"
#include "GMTK_SceneData.h"
#include "GMTK_SceneDefinitionAsset.generated.h"

UCLASS(BlueprintType)
class GMTK2026_API UGMTK_SceneDefinitionAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Initial state of the scene 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	FGameplayTag StartState;

	// State that represents the successful completion of the scene
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	FGameplayTag SuccessState;

	// Transition list
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	TArray<FSceneTransitionRow> Transitions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene")
	TMap<FGameplayTag, TSoftObjectPtr<class ULevelSequence>> EventSequences;

	// Name of the Note/wish associated with this scene 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scene|Wish")
	FGameplayTag WishID;

	// Find the first valid transition for the current state + received action.
	bool FindTransition(FGameplayTag CurrentState, FGameplayTag ActionID, FSceneTransitionRow& OutRow) const
	{
		for (const FSceneTransitionRow& Row : Transitions)
		{
			if (Row.FromState == CurrentState && Row.ActionID == ActionID)
			{
				OutRow = Row;
				return true;
			}
		}
		for (const FSceneTransitionRow& Row : Transitions)
		{
			if (Row.FromState == GMTK_GameplayTags::Scene_State_Any && Row.ActionID == ActionID)
			{
				OutRow = Row;
				return true;
			}
		}
		return false;
	}
};
