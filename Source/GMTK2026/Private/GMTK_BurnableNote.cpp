#include "GMTK_BurnableNote.h"

AGMTK_BurnableNote::AGMTK_BurnableNote()
{
	PrimaryActorTick.bCanEverTick = false; 
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
		// The flame has moved away before the time: reset the accumulation.
		CurrentBurnTime = 0.0f;
		return;
	}

	if (!bIsCorrectWish)
	{
		// Note wrong and burning attempt rejected. We can add some visual feedback here.
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
