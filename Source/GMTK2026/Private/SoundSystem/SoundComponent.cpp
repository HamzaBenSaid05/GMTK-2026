#include "GameplayTagContainer.h"
#include "Components/AudioComponent.h"
#include "EventProxies/AudioProxySubsystem.h"
#include "SoundSystem/SoundComponent.h"

USoundBase* USoundComponent::FindSound(FGameplayTag Tag)
{
	FAudioRow* Row = FindGamplayTag(Tag);
	if (Row == nullptr || Row->Sound == nullptr) return nullptr;
	return Row->Sound;
}

FAudioRow* USoundComponent::FindGamplayTag(FGameplayTag Tag) const
{
	static const FString ContextString(TEXT("Tag Search Context"));

	TArray<FAudioRow*> AllRows;
	Table->GetAllRows<FAudioRow>(ContextString, AllRows);
	for (FAudioRow* Row : AllRows)
	{
		if (Row && Row->ItemTag == Tag)
		{
			return Row;
		}
	}
	return nullptr;
}

void USoundComponent::PlaySound(FGameplayTag Tag, FVector SourceLocation, UAudioComponent*& AudioComp, FTPP_AudioParameter Parameter, USceneComponent* SourceActor)
{
	if (Tag == FGameplayTag::EmptyTag || Table == nullptr) { return; }

	FAudioRow* Row = FindGamplayTag(Tag);
	
	if (Row == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Sound doesn't exist in table"));
		return;
	}
	Parameter.Name = Row->Parameter.Name;
	GetWorld()->GetSubsystem<UAudioProxySubsystem>()->NotifyAudioTableEvent(*Row, AudioComp, SourceLocation,Parameter,SourceActor);
}

void USoundComponent::SetAudioComponent(UAudioComponent*& AudioComponent, FGameplayTag Tag)
{
	if (!AudioComponent)
	{
		AudioComponent = NewObject<UAudioComponent>(this);

		AudioComponent->RegisterComponent();

		AudioComponent->bAutoActivate = false;
		FAudioRow* Row = FindGamplayTag(Tag);
	}
}