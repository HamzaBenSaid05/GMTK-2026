// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/GMTK_AudioRow.h"
#include "Data/GMTK_SoundLevel.h"
#include "Subsystems/WorldSubsystem.h"
#include "GMTK_SoundSystem.generated.h"

UCLASS()
class GMTK2026_API UGMTK_SoundSystem : public UWorldSubsystem
{
	GENERATED_BODY()
	public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Deinitialize() override;
	UFUNCTION()
	void PlayBlueprintSound(struct FGMTK_AudioRow& Row, class AActor* SourceActor, class UAudioComponent* AudioComp, FVector Location);
	UFUNCTION()
	void PlaySound(FGMTK_AudioRow& Row, class UAudioComponent*& AudioComp, FVector& Location, FGMTK_AudioParameter& Parameter, class USceneComponent* SourceActor);

	FORCEINLINE const class UGMTK_SoundDeleveloperSettings* GetSettings() const { return SoundSettings; }

	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetMasterVolume() const { return Levels.master; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMasterVolume(float NewMasterVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetAmbienceVolume() const { return Levels.ambience; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetAmbienceVolume(float NewAmbienceVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetBGMVolume() const { return Levels.music; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetBGMVolume(float NewBgmVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetSFXVolume() const { return Levels.effects; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetSFXVolume(float NewSFXVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetGUIVolume() const { return Levels.userInterface; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetGUIVolume(float NewGUIVolume);
	UFUNCTION(BlueprintPure, Category = "Audio")
	FORCEINLINE float GetVoiceVolume() const { return Levels.voice; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetVoiceVolume(float NewVoiceVolume);

	const FGMTK_SoundLevel& GetLevels() const { return Levels; }
	UFUNCTION(BlueprintCallable, Category = "Audio", DisplayName = "Get Levels")
	FGMTK_SoundLevel GetLevelsCopy() const { return Levels; }

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetLevels(const FGMTK_SoundLevel& NewLevels);

private:
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetLevel(const TSoftObjectPtr<class USoundSubmix>& Submix, float& Level, float NewLevel);
	UFUNCTION()
	void HandleWorldLoaded(UWorld* NewWorld);
	UPROPERTY()
	TObjectPtr<UWorld> CurrentWorld;
	UPROPERTY()
	TObjectPtr<const class UGMTK_SoundDeleveloperSettings> SoundSettings;
	UPROPERTY()
	FGMTK_SoundLevel Levels;
};
