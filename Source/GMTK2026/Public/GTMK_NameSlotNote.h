#pragma once

#include "CoreMinimal.h"
#include "Components/NamedSlot.h"
#include "GTMK_NameSlotNote.generated.h"

UCLASS()
class GMTK2026_API UGTMK_NameSlotNote : public UNamedSlot
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Note")
	TObjectPtr<UMaterialInstance> SceneNoteMaterial;
};
