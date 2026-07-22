#pragma once
#include "CoreMinimal.h"
#include "SoundLevel.generated.h"

USTRUCT(BlueprintType)
struct FSoundLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Levels", Meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float master = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Levels", Meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float ambience = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Levels", Meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float music = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Levels", Meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float effects = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Levels", Meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float userInterface = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound Levels", Meta = (UIMin = 0.0f, UIMax = 1.0f, ClampMin = 0.0f, ClampMax = 1.0f))
	float voice = 1.0f;
};
