#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GMTK_GameSettings.generated.h"

UCLASS(Config = GameSettings, DefaultConfig)
class GMTK2026_API UGMTK_GameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGMTK_GameSettings();
	
	// Game Levels 
	UPROPERTY(Config, EditAnywhere, Category = "Levels")
	TArray<TSoftObjectPtr<UWorld>> GameLevels;

	// Final Level to load after all game levels are completed!
	UPROPERTY(Config, EditAnywhere, Category = "Levels")
	TSoftObjectPtr<UWorld> FinalLevel;
};
