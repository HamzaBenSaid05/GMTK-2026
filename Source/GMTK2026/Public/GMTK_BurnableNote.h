#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GMTK_BurnableNote.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoteBurnResult, bool, bWasCorrect);

UCLASS(Blueprintable)
class AGMTK_BurnableNote : public AActor
{
	GENERATED_BODY()

public:
	AGMTK_BurnableNote();

	// Scene desire id
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note")
	FGameplayTag WishID = FGameplayTag();

	// Set in the GameFlowManager at runtime, based on the completed scenes and the WishID of this note.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note")
	bool bIsCorrectWish = true;

	// Seconds required to burn the note
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note")
	float RequiredBurnTime = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	float CurrentBurnTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	bool bIsBurned = false;

	UPROPERTY(BlueprintAssignable, Category = "Note")
	FOnNoteBurnResult OnNoteBurnResult;

	// If bOverlapping is true, DeltaTime is the time since last frame (to accumulate burn time).
	UFUNCTION(BlueprintCallable, Category = "Note")
	void SetFlameOverlapping(bool bOverlapping, float DeltaTime);

	// Reset Note to "not burned" state for retry.
	UFUNCTION(BlueprintCallable, Category = "Note")
	void ResetNote();

protected:
	// BP Event for visual feedback when the fire does not catch.
	UFUNCTION(BlueprintImplementableEvent, Category = "Note")
	void OnBurnRejected();

	// BP Event for visual feedback when the note is successfully burned.
	UFUNCTION(BlueprintImplementableEvent, Category = "Note")
	void OnBurnSuccessVisual();
};
