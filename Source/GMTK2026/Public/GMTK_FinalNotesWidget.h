#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GMTK_FinalNotesWidget.generated.h"

class UGMTK_BurnableNoteWidget;
class UGMTK_LighterWidget;

UCLASS(Abstract, Blueprintable)
class GMTK2026_API UGMTK_FinalNotesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Populated in Native Construct by scanning all UGMTK_BurnableNoteWidget in this Widget Blueprint 
	UPROPERTY(BlueprintReadOnly, Category = "FinalNotes")
	TArray<TObjectPtr<UGMTK_BurnableNoteWidget>> SceneNotes;

	// Note with Player desire 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UGMTK_BurnableNoteWidget> PlayerNote;

	// Lighter
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UGMTK_LighterWidget> LighterWidget;

	// Called from a "Retry" button in UI: if some notes are still missing, OnSomeSceneNotesWrong is triggered.
	UFUNCTION(BlueprintCallable, Category = "FinalNotes")
	void RequestRetryCheck();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnAllSceneNotesCorrect();

	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnSomeSceneNotesWrong();

private:
	UFUNCTION()
	void HandleSceneNoteBurnResult(bool bWasCorrect);

	bool AreAllSceneNotesBurned() const;
};
