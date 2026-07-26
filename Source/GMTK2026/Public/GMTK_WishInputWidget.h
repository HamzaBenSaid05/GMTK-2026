// WishInputWidget.h
// Classe base C++ per il widget UMG del "biglietto bianco vuoto" iniziale.
// In editor crei un Widget Blueprint che eredita da questa classe, ci metti
// un EditableTextBox chiamato "WishTextBox" (o binda OnTextChanged manualmente)
// e un bottone "Conferma" collegato a SubmitWish.

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

	// Notify the GameFlowManager and close the widget
	UFUNCTION(BlueprintCallable, Category = "Wish")
	void SubmitWish();

	// Remaining chars
	UFUNCTION(BlueprintPure, Category = "Wish")
	int32 GetRemainingCharacters() const { return MaxCharacters - CurrentText.Len(); }
};
