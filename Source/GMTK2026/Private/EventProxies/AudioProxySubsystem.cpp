#include "EventProxies/AudioProxySubsystem.h"

void UAudioProxySubsystem::NotifyAudioTableEvent(struct FGMTK_AudioRow& Row, class UAudioComponent *& AudioComp, FVector & Location,struct FGMTK_AudioParameter& Parameter,class USceneComponent * SourceActor) const
{OnAudioTableEvent.Broadcast(Row,AudioComp,Location,Parameter,SourceActor);
}

void UAudioProxySubsystem::NotifyAudioTableEventUI(FGameplayTag Tag, struct FGMTK_AudioParameter Parameter,USoundSubmix* Submix, EGMTK_ButtonType Button) const {OnAudioTableEventUI.Broadcast(Tag,Parameter,Submix,Button);}
