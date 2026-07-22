#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/AudioRow.h"
#include "SoundComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GMTK2026_API USoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* Table;
	UFUNCTION(BluePrintCallable)
	USoundBase* FindSound(FGameplayTag Tag);
	FAudioRow* FindGamplayTag(FGameplayTag Tag) const;
	UFUNCTION(BluePrintCallable)
	void PlaySound(FGameplayTag Tag, FVector SourceLocation, UAudioComponent*& AudioComp, FTPP_AudioParameter Parameter = FTPP_AudioParameter(), USceneComponent* SourceActor = nullptr);
	void SetAudioComponent(UAudioComponent*& AudioComponent, FGameplayTag Tag);
	
};
