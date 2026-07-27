#pragma once

#include "CoreMinimal.h"
#include "GMTK_BurnableNote.h"
#include "GameFramework/Actor.h"
#include "GMTK_FinalNotesController.generated.h"

UCLASS(Blueprintable)
class GMTK2026_API AGMTK_FinalNotesController : public AActor
{
	GENERATED_BODY()

public:
	AGMTK_FinalNotesController();

	// Scene Ticktes  
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "FinalNotes")
	TArray<TObjectPtr<AGMTK_BurnableNote>> SceneNotes;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "FinalNotes")
	TObjectPtr<AGMTK_BurnableNote> PlayerNote;

	UFUNCTION(BlueprintCallable, Category = "FinalNotes")
	void RequestRetryCheck();

protected:
	virtual void BeginPlay() override;

	// All scene are burned 
	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnAllSceneNotesCorrect();

	// Not all scene notes are burned, or at least one of them is wrong 
	UFUNCTION(BlueprintImplementableEvent, Category = "FinalNotes")
	void OnSomeSceneNotesWrong();

private:
	UFUNCTION()
	void HandleSceneNoteBurnResult(bool bWasCorrect);

	bool AreAllSceneNotesBurned() const;
};