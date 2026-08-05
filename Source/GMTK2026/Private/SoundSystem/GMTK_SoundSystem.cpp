#include "Public/SoundSystem/GMTK_SoundSystem.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Public/EventProxies/AudioProxySubsystem.h"
#include "Public/SoundSystem/Data/GMTK_AudioRow.h"
#include "Public/SoundSystem/Data/GMTK_AudioSpatialization.h"
#include "Public/SoundSystem/DeveloperSetting/GMTK_SoundDeleveloperSettings.h"
#include "Sound/SoundSubmix.h"

void UGMTK_SoundSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UAudioProxySubsystem* Proxy = GetWorld()->GetSubsystem<UAudioProxySubsystem>())
	{
		Proxy->OnAudioTableEvent.AddDynamic(this, &UGMTK_SoundSystem::PlaySound);
	}

	SoundSettings = GetDefault<UGMTK_SoundDeleveloperSettings>();

	if (GEngine)
	{
		for (const FWorldContext& context : GEngine->GetWorldContexts())
		{
			UWorld* world = context.World();
			if (
				world &&
				world->IsGameWorld()
			)
			{
				HandleWorldLoaded(world);
				break;
			}
		}
	}
}

void UGMTK_SoundSystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	//if (UTPP_GameInstance* GI = InWorld.GetGameInstance<UTPP_GameInstance>())
	//{
	//	if (UTPP_SaveGame* SaveGame = GI->GetSaveGameObject())
	//	{
	//		SetMasterVolume(SaveGame->VolumeMaster);
	//		SetBGMVolume(SaveGame->VolumeMusic);
	//		SetSFXVolume(SaveGame->VolumeSFX);
	//		SetAmbienceVolume(SaveGame->VolumeEnv);
	//	}
	//}
}

void UGMTK_SoundSystem::Deinitialize()
{
	SoundSettings = nullptr;
	CurrentWorld = nullptr;

	Super::Deinitialize();
}

void UGMTK_SoundSystem::PlayBlueprintSound(FGMTK_AudioRow & Row, class AActor * SourceActor, class UAudioComponent*  AudioComp, FVector Location)
{
	USceneComponent* AttachComponent = SourceActor->GetRootComponent();

	if (Row.SpatializationType == GMTK_AudioSpatialization::ThreeDs)
	{
		if (Row.FollowModeType == TPP_AudioTargetFollowMode::TargetLocation)
		{
			AudioComp = UGameplayStatics::SpawnSoundAttached(Row.Sound, AttachComponent);
		}
		else { AudioComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Row.Sound, Location); }
	}
	else { AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), Row.Sound); }
}

void UGMTK_SoundSystem::PlaySound(struct FGMTK_AudioRow& Row, class UAudioComponent*& AudioComp, FVector& Location, struct FGMTK_AudioParameter& Parameter,
	class USceneComponent* SourceActor)
{
	if (Row.SpatializationType == GMTK_AudioSpatialization::ThreeDs)
	{
		if (Row.FollowModeType == TPP_AudioTargetFollowMode::TargetLocation)
		{
			AudioComp = UGameplayStatics::SpawnSoundAttached(Row.Sound, SourceActor);
		}
		else { AudioComp = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Row.Sound, Location); }
	}
	else { AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), Row.Sound); }
	if (AudioComp == nullptr) return;
	AudioComp->SetIntParameter(Parameter.Name, Parameter.Int);
	AudioComp->SetUISound(Row.bIsUISound);
}

void UGMTK_SoundSystem::SetMasterVolume(float NewMasterVolume)
{
	SetLevel(
		SoundSettings->MasterMixer,
		Levels.master,
		NewMasterVolume
	);
}

void UGMTK_SoundSystem::SetAmbienceVolume(float NewAmbienceVolume)
{
	SetLevel(
		SoundSettings->ambienceMixer,
		Levels.ambience,
		NewAmbienceVolume
	);
}

void UGMTK_SoundSystem::SetBGMVolume(float NewBgmVolume)
{
	SetLevel(
		SoundSettings->musicMixer,
		Levels.music,
		NewBgmVolume
	);
}

void UGMTK_SoundSystem::SetSFXVolume(float NewSFXVolume)
{
	SetLevel(
		SoundSettings->effectsMixer,
		Levels.effects,
		NewSFXVolume
	);
}

void UGMTK_SoundSystem::SetGUIVolume(float NewGUIVolume)
{
	SetLevel(
		SoundSettings->userInterfaceMixer,
		Levels.userInterface,
		NewGUIVolume
	);
}

void UGMTK_SoundSystem::SetVoiceVolume(float NewVoiceVolume)
{
	SetLevel(
		SoundSettings->voiceMixer,
		Levels.voice,
		NewVoiceVolume
	);
}

void UGMTK_SoundSystem::SetLevels(const FGMTK_SoundLevel& NewLevels)
{
	SetMasterVolume(NewLevels.master);
	SetAmbienceVolume(NewLevels.ambience);
	SetBGMVolume(NewLevels.music);
	SetSFXVolume(NewLevels.effects);
	SetGUIVolume(NewLevels.userInterface);
	SetVoiceVolume(NewLevels.voice);
}

void UGMTK_SoundSystem::SetLevel(const TSoftObjectPtr<class USoundSubmix>& Submix, float& Level, float NewLevel)
{
	Level = FMath::Clamp(NewLevel, 0.0f, 1.0f);

	if (!CurrentWorld)
		return;

	USoundSubmix* LoadedSubmix = Submix.LoadSynchronous();

	if (!LoadedSubmix)
		return;

	float SafeValue = FMath::Max(Level, 0.0001f);
	LoadedSubmix->SetSubmixOutputVolume(CurrentWorld,SafeValue);
		GEngine->AddOnScreenDebugMessage(
			5,
			0.3f,
			FColor::Cyan,
			FString::SanitizeFloat(SafeValue)
		);
}

void UGMTK_SoundSystem::HandleWorldLoaded(UWorld* NewWorld)
{
	if (
		!NewWorld ||
		!NewWorld->IsGameWorld()
	)
		return;

	CurrentWorld = NewWorld;

	SetLevels(Levels);
}
