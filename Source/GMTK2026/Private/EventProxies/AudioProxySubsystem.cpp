#include "EventProxies/AudioProxySubsystem.h"

void UAudioProxySubsystem::NotifyAudioTableEvent(struct FAudioRow& Row, class UAudioComponent *& AudioComp, FVector & Location,struct FTPP_AudioParameter& Parameter,class USceneComponent * SourceActor) const
{OnAudioTableEvent.Broadcast(Row,AudioComp,Location,Parameter,SourceActor);
}

void UAudioProxySubsystem::NotifyAudioTableEventUI(FGameplayTag Tag, struct FTPP_AudioParameter Parameter,USoundSubmix* Submix, ButtonType Button) const {OnAudioTableEventUI.Broadcast(Tag,Parameter,Submix,Button);}
