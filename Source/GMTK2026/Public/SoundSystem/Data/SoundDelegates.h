#pragma once
#include "CoreMinimal.h"
#include "SoundDelegates.generated.h"

UDELEGATE()	//	Required when declaring delegates in a dedicated header file
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSoundLevelDelegate, TSoftObjectPtr<class USoundSubmix>, submix, float, level);
