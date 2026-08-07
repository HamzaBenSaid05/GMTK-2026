#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GMTK_BurnableNoteWidget.generated.h"

class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnNoteBurnResult, bool, bWasCorrect, FGameplayTag, WishID, bool, bIsPlayerWish);

UCLASS(Abstract, Blueprintable)
class GMTK2026_API UGMTK_BurnableNoteWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Scene desire id
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note")
	FGameplayTag WishID;

	// If true, this note displays the wish text written by the player (instead of a scene wish).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note")
	bool bIsPlayerWishNote = false;

	// Set at runtime (dal FinalNotesWidget) in base alle scene completate.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note")
	bool bIsCorrectWish = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Note")
	float RequiredBurnTime = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	float CurrentBurnTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	bool bIsBurned = false;
	
	// Bind to show the wish text
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WishTextBlock;

	UPROPERTY(BlueprintAssignable, Category = "Note")
	FOnNoteBurnResult OnNoteBurnResult;

	// Called every frame by LighterWidget based on geometric check 
	UFUNCTION(BlueprintCallable, Category = "Note")
	void SetFlameOverlapping(bool bOverlapping, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Note")
	void ResetNote();

	UFUNCTION(BlueprintCallable, Category = "Note")
	void SetSceneMaterial(UMaterialInstance* Material);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Note")
	void SetNoteSceneBrush(UMaterialInstance* Material);
	
	// Toggle note enabled/disabled 
	UFUNCTION(BlueprintCallable, Category = "Note")
	void SetNoteEnabled(bool bEnabled);

	UPROPERTY(BlueprintReadOnly, Category = "Note")
	TObjectPtr<UMaterialInstance> SceneNoteMaterial;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Note")
	void OnBurnRejected();

	UFUNCTION(BlueprintImplementableEvent, Category = "Note")
	void OnBurnSuccessVisual();

	UFUNCTION(BlueprintImplementableEvent, Category = "Note")
	void OnWishTextSet(const FString& WishText);
};
