#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GMTK_WishInputWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWishSubmitted, const FString&, WishText);

UCLASS(Abstract, Blueprintable)
class GMTK2026_API UGMTK_WishInputWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Character Limit, to bind to the MaxLength of the EditableTextBox in BP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wish")
	int32 MaxCharacters = 80;

	// Current text
	UPROPERTY(BlueprintReadOnly, Category = "Wish")
	FString CurrentText;

	UPROPERTY(BlueprintAssignable, Category = "Wish")
	FOnWishSubmitted OnWishSubmitted;

	// Sinchronized with current text to apply truncation if needed
	UFUNCTION(BlueprintCallable, Category = "Wish")
	void UpdateCurrentText(const FString& NewText);
	
	// Fired after UpdateCurrentText, only when truncation actually happened 
	UFUNCTION(BlueprintImplementableEvent, Category = "Wish")
	void OnTextTruncated(const FString& TruncatedText);

	// Notify the GameFlowManager and close the widget
	UFUNCTION(BlueprintCallable, Category = "Wish")
	void SubmitWish();

	// Remaining chars
	UFUNCTION(BlueprintPure, Category = "Wish")
	int32 GetRemainingCharacters() const { return MaxCharacters - CurrentText.Len(); }
};
