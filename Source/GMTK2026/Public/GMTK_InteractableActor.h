#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "GMTK_InteractableActor.generated.h"

class AGMTK_SceneControllerBase;

UCLASS(Blueprintable)
class GMTK2026_API AGMTK_InteractableActor : public AActor
{
	GENERATED_BODY()

public:
	AGMTK_InteractableActor();

	// Unique ID used in the SceneDefinitionAsset
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable")
	FGameplayTag ActionID;

	UPROPERTY(BlueprintReadOnly, Category = "Interactable")
	TObjectPtr<AGMTK_SceneControllerBase> OwningSceneController;

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void Interact();

	UPROPERTY(EditAnywhere)
	USceneComponent* HoverPoint;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HoverMesh;

protected:
	virtual void BeginPlay() override;
};
