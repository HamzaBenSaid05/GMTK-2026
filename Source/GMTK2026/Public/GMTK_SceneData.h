#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GMTK_SceneData.generated.h"

USTRUCT(BlueprintType)
struct FSceneTransitionRow
{
	GENERATED_BODY()

	// Start State 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	FGameplayTag FromState;

	// Action ID that triggers the transition 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	FGameplayTag ActionID;

	// Arrival State
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	FGameplayTag ToState;

	// Event name to trigger when this transition occurs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	FGameplayTag EventTag;

	// If true this transition represents a failure of the scene
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	bool bIsFailure = false;

	// If true this transition completes the scene successfully
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	bool bIsSuccess = false;

	// If true this transition ends the scene and moves to the next level
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Transition")
	bool bEndsScene = false;
};
