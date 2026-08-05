#include "SoundSystem/GMTK_SoundComponent.h"
#include "GameplayTagContainer.h"
#include "Components/AudioComponent.h"
#include "EventProxies/AudioProxySubsystem.h"

USoundBase* UGMTK_SoundComponent::FindSound(FGameplayTag Tag)
{
	FGMTK_AudioRow* Row = FindGamplayTag(Tag);
	if (Row == nullptr || Row->Sound == nullptr) return nullptr;
	return Row->Sound;
}

FGMTK_AudioRow* UGMTK_SoundComponent::FindGamplayTag(FGameplayTag Tag) const
{
	static const FString ContextString(TEXT("Tag Search Context"));

	TArray<FGMTK_AudioRow*> AllRows;
	Table->GetAllRows<FGMTK_AudioRow>(ContextString, AllRows);
	for (FGMTK_AudioRow* Row : AllRows)
	{
		if (Row && Row->ItemTag == Tag)
		{
			return Row;
		}
	}
	return nullptr;
}

void UGMTK_SoundComponent::PlaySound(FGameplayTag Tag, FVector SourceLocation, UAudioComponent*& AudioComp, FGMTK_AudioParameter Parameter, USceneComponent* SourceActor)
{
	if (Tag == FGameplayTag::EmptyTag || Table == nullptr) { return; }

	FGMTK_AudioRow* Row = FindGamplayTag(Tag);
	
	if (Row == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Sound doesn't exist in table"));
		return;
	}
	Parameter.Name = Row->Parameter.Name;
	GetWorld()->GetSubsystem<UAudioProxySubsystem>()->NotifyAudioTableEvent(*Row, AudioComp, SourceLocation,Parameter,SourceActor);
}

void UGMTK_SoundComponent::SetAudioComponent(UAudioComponent*& AudioComponent, FGameplayTag Tag)
{
	if (!AudioComponent)
	{
		AudioComponent = NewObject<UAudioComponent>(this);

		AudioComponent->RegisterComponent();

		AudioComponent->bAutoActivate = false;
		FGMTK_AudioRow* Row = FindGamplayTag(Tag);
	}
}