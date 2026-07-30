#include "GMTK_LighterWidget.h"
#include "GMTK_BurnableNoteWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelWidget.h"
#include "Framework/Application/SlateApplication.h"


void UGMTK_LighterWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (UCanvasPanelSlot* FlameSlot = Cast<UCanvasPanelSlot>(FlameImage->Slot))
	{
		FlameSlot->SetPosition(FlameOffset);
	}
}


void UGMTK_LighterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);

	// Start off: the flame image shouldn't be visible until ignited.
	if (FlameImage)
	{
		FlameImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGMTK_LighterWidget::RegisterNote(UGMTK_BurnableNoteWidget* Note)
{
	if (Note)
	{
		WatchedNotes.AddUnique(Note);
	}
}

void UGMTK_LighterWidget::TryIgnite()
{
	if (bIsLit || bIsIgniting)
	{
		return;
	}

	bIsIgniting = true;
	PlayIgnitionVisual();
}

void UGMTK_LighterWidget::OnIgnitionFinished()
{
	bIsIgniting = false;
	bIsLit = true;

	if (FlameImage)
	{
		FlameImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UGMTK_LighterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (UCanvasPanelSlot* MySlot = Cast<UCanvasPanelSlot>(Slot))
	{
		if (UPanelWidget* ParentPanel = GetParent())
		{
			const FGeometry& ParentGeometry = ParentPanel->GetCachedGeometry();
			const FVector2D AbsoluteMousePos = FSlateApplication::Get().GetCursorPos();
			FVector2D LocalPosInParent = ParentGeometry.AbsoluteToLocal(AbsoluteMousePos);

			const FVector2D ParentSize = ParentGeometry.GetLocalSize();
			LocalPosInParent.X = FMath::Clamp(LocalPosInParent.X, 0.0f, ParentSize.X);
			LocalPosInParent.Y = FMath::Clamp(LocalPosInParent.Y, 0.0f, ParentSize.Y);

			MySlot->SetPosition(LocalPosInParent);
		}
	}

	// While off or still igniting, the flame doesn't burn anything: force every note to "not overlapping".
	if (!bIsLit)
	{
		for (UGMTK_BurnableNoteWidget* Note : WatchedNotes)
		{
			if (IsValid(Note))
			{
				Note->SetFlameOverlapping(false, 0.0f);
			}
		}
		return;
	}

	const FVector2D FlameAbsolutePosition = MyGeometry.LocalToAbsolute(CurrentFlameLocalPosition);

	for (UGMTK_BurnableNoteWidget* Note : WatchedNotes)
	{
		if (!IsValid(Note))
		{
			continue;
		}

		const bool bOverlapping = Note->GetCachedGeometry().IsUnderLocation(FlameAbsolutePosition);
		Note->SetFlameOverlapping(bOverlapping, InDeltaTime);
	}
}
