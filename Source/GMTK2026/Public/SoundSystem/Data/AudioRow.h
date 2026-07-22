#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AudioSpatialization.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "AudioRow.generated.h"

USTRUCT(BlueprintType)
struct FTPP_AudioParameter
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Parameter")
	int32 Int = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FName Name = NAME_None;
};
UENUM(BlueprintType)
enum class ButtonType : uint8 
{
	Button,
	Slider,
	CheckBox
};

USTRUCT(BlueprintType)
struct FAudioRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories="Audio"))
	FGameplayTag ItemTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AudioSpatialization SpatializationType = AudioSpatialization::TwoD;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TPP_AudioTargetFollowMode FollowModeType = TPP_AudioTargetFollowMode::StartLocation;
	
	UPROPERTY(EditAnywhere)
	bool bIsUISound = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	FTPP_AudioParameter Parameter;
};
