#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GMTK_GameSettings.generated.h"

UCLASS(Config=GameLevels, DefaultConfig)
class GMTK2026_API UGMTK_GameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UGMTK_GameSettings();
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly,
		Category="Levels|Game",
		meta=(ToolTip="List of levels in the game, in order of progression."))
	TArray<TSoftObjectPtr<UWorld>> Levels;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly,
		Category="Levels|Final",
		meta=(ToolTip="The final level to load after all other levels are completed."))
	TSoftObjectPtr<UWorld> FinalLevel;
};
