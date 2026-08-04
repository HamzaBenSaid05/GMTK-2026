#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GMTK_AudioSpatialization.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "GMTK_AudioRow.generated.h"

USTRUCT(BlueprintType)
struct FGMTK_AudioParameter
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Parameter")
	int32 Int = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FName Name = NAME_None;
};
UENUM(BlueprintType)
enum class EGMTK_ButtonType : uint8 
{
	Button,
	Slider,
	CheckBox
};

USTRUCT(BlueprintType)
struct FGMTK_AudioRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Audio"))
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	GMTK_AudioSpatialization SpatializationType = GMTK_AudioSpatialization::TwoD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TPP_AudioTargetFollowMode FollowModeType = TPP_AudioTargetFollowMode::StartLocation;
	
	UPROPERTY(EditAnywhere)
	bool bIsUISound = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FGMTK_AudioParameter Parameter;
};
