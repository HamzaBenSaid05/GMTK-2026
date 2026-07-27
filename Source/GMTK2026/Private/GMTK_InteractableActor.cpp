#include "GMTK_InteractableActor.h"
#include "GMTK_SceneControllerBase.h"
#include "Kismet/GameplayStatics.h"

AGMTK_InteractableActor::AGMTK_InteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	HoverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HoverMesh"));
	SetRootComponent(HoverMesh);
	HoverPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HoverPoint"));
	HoverPoint->SetupAttachment(RootComponent);
	HoverPoint->SetRelativeLocation(FVector(0,0,100));
}

void AGMTK_InteractableActor::BeginPlay()
{
	Super::BeginPlay();
	OwningSceneController = Cast<AGMTK_SceneControllerBase>(
	                                                   UGameplayStatics::GetActorOfClass(GetWorld(), AGMTK_SceneControllerBase::StaticClass()));
}

void AGMTK_InteractableActor::Interact()
{
	if (OwningSceneController && ActionID.IsValid())
	{
		OwningSceneController->HandleInteraction(ActionID);
	}
}
