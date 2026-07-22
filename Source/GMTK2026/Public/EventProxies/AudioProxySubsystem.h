#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Public/SoundSystem/Data/AudioRow.h"
#include "Sound/SoundSubmix.h"
#include "Subsystems/WorldSubsystem.h"
#include "AudioProxySubsystem.generated.h"

struct AudioRow;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(
	FAudioTableEventDelegate,
	FAudioRow&, Row,
	class UAudioComponent*&, AudioComp,
	FVector&, Location,
	FTPP_AudioParameter&, Parameter,
	USceneComponent*, TargetSource
);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FAudioTableEventDelegateUI,
	FGameplayTag, Tag,
	FTPP_AudioParameter, Parameter,
	USoundSubmix*,Submix,
	ButtonType, Button
);

UCLASS(BlueprintType, Blueprintable)
class GMTK2026_API UAudioProxySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FAudioTableEventDelegateUI OnAudioTableEventUI;
	FAudioTableEventDelegate OnAudioTableEvent;
	
	UFUNCTION(BlueprintCallable)
	void NotifyAudioTableEventUI(FGameplayTag Tag,struct FTPP_AudioParameter Parameter,USoundSubmix* Submix,ButtonType Button) const;
	void NotifyAudioTableEvent(FAudioRow& Row, class UAudioComponent*& AudioComp, FVector& Location, FTPP_AudioParameter& Parameter, class USceneComponent* SourceActor) const;

};
