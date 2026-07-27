#include "GMTK_BurnableNote.h"
#include "GMTK_GameFlowManager.h"
#include "Kismet/GameplayStatics.h"

AGMTK_BurnableNote::AGMTK_BurnableNote()
{
	PrimaryActorTick.bCanEverTick = false;

	WishTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("WishTextComponent"));
	WishTextComponent->SetupAttachment(RootComponent);
	WishTextComponent->SetHorizontalAlignment(EHTA_Center);
	WishTextComponent->SetVerticalAlignment(EVRTA_TextCenter);
	WishTextComponent->SetVisibility(false); // hidden by default, shown only if bIsPlayerWishNote
}

void AGMTK_BurnableNote::BeginPlay()
{
	Super::BeginPlay();

	if (WishTextComponent && RootComponent && WishTextComponent->GetAttachParent() != RootComponent)
	{
		WishTextComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
	
	if (bIsPlayerWishNote)
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
		{
			if (UGMTK_GameFlowManager* Flow = GI->GetSubsystem<UGMTK_GameFlowManager>())
			{
				const FString& WishText = Flow->PlayerWishText;

				if (WishTextComponent)
				{
					WishTextComponent->SetVisibility(true);
					WishTextComponent->SetText(FText::FromString(WishText));
				}

				OnWishTextSet(WishText);
			}
		}
	}
}

void AGMTK_BurnableNote::ResetNote()
{
	bIsBurned = false;
	CurrentBurnTime = 0.0f;
}

void AGMTK_BurnableNote::SetFlameOverlapping(bool bOverlapping, float DeltaTime)
{
	if (bIsBurned)
	{
		return;
	}

	if (!bOverlapping)
	{
		CurrentBurnTime = 0.0f;
		return;
	}

	if (!bIsCorrectWish)
	{
		OnBurnRejected();
		return;
	}

	CurrentBurnTime += DeltaTime;
	if (CurrentBurnTime >= RequiredBurnTime)
	{
		bIsBurned = true;
		OnBurnSuccessVisual();
		OnNoteBurnResult.Broadcast(true);
	}
}