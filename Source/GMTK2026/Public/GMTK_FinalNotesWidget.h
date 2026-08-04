#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "GMTK_FinalNotesWidget.generated.h"

class UGMTK_BurnableNoteWidget;
class UGMTK_LighterWidget;
class UPanelWidget;

UCLASS(Abstract, Blueprintable)
class GMTK2026_API UGMTK_FinalNotesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Note widgets currently spawned for this visit to the tickets screen
	UPROPERTY(BlueprintReadOnly, Category = "FinalNotes")
	TArray<TObjectPtr<UGMTK_BurnableNoteWidget>> SceneNotes;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UGMTK_BurnableNoteWidget> PlayerNote;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UGMTK_LighterWidget> LighterWidget;

	// Class used to spawn a scene note widget dynamically, one per unburned scene.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FinalNotes")
	TSubclassOf<UGMTK_BurnableNoteWidget> SceneNoteWidgetClass;

	// Panel where dynamically created note widgets are added 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FinalNotes")
	TMap<FGameplayTag, FName> NoteSlotNames;

	UFUNCTION(BlueprintCallable, Category = "FinalNotes")
	void StartRetryOfFailedScenes();

	UFUNCTION(BlueprintCallable, Category = "FinalNotes")
	void RestartAllScenes();
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnAllSceneNotesCorrect();

	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnSomeSceneNotesWrong();

	// Called right after a note widget is spawned, in case you want to set its position/slot
	// from Blueprint (e.g. a predefined layout per WishID) instead of relying only on the panel's auto-layout.
	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnSceneNoteSpawned();

	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnPlayerNoteBurned();

private:
	UFUNCTION()
	void HandleSceneNoteBurnResult(bool bWasCorrect, FGameplayTag WishID, bool bIsPlayerNote = false);

	bool AreAllSceneNotesBurned() const;

	void SpawnUnburnedSceneNotes();

	UPROPERTY()
	TObjectPtr<UMaterialInstance> PlayerNoteMaterial;
};