#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Public/SoundSystem/Data/SoundLevel.h"
#include "SoundDeleveloperSettings.generated.h"

UCLASS(Config = GameSound, DefaultConfig)
class GMTK2026_API USoundDeleveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	USoundDeleveloperSettings();
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> MasterMixer;
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> ambienceMixer;
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> musicMixer;
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> effectsMixer;
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> userInterfaceMixer;
	UPROPERTY(Config, EditAnywhere, Category = "Sound Classes")
	TSoftObjectPtr<class USoundSubmix> voiceMixer;

	UPROPERTY(Config, EditAnywhere, Category = "Levels")
	FSoundLevel defaultLevels;
};
