#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/GMTK_AudioRow.h"
#include "GMTK_SoundComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GMTK2026_API UGMTK_SoundComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* Table;
	UFUNCTION(BluePrintCallable)
	USoundBase* FindSound(FGameplayTag Tag);
	FGMTK_AudioRow* FindGamplayTag(FGameplayTag Tag) const;
	UFUNCTION(BluePrintCallable)
	void PlaySound(FGameplayTag Tag, FVector SourceLocation, UAudioComponent*& AudioComp, FGMTK_AudioParameter Parameter = FGMTK_AudioParameter(), USceneComponent* SourceActor = nullptr);
	void SetAudioComponent(UAudioComponent*& AudioComponent, FGameplayTag Tag);
	
};
