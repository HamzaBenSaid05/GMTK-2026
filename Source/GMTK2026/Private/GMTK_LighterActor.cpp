#include "GMTK_LighterActor.h"
#include "GMTK_BurnableNote.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

AGMTK_LighterActor::AGMTK_LighterActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root_ = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root_);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(Root_);

	// The lighter uses custom 2D drag movement, so the body does not require physics simulation.
	BodyMesh->SetCollisionProfileName(TEXT("NoCollision"));

	FlameSocket = CreateDefaultSubobject<USceneComponent>(TEXT("FlameSocket"));
	FlameSocket->SetupAttachment(BodyMesh);

	// Socket position representing the lighter's flame origin. Adjust this value in the editor if needed.
	FlameSocket->SetRelativeLocation(FVector(0.f, 0.f, 15.f));


	// FlameVisual is intentionally not attached to FlameSocket.
	// It is updated manually in Tick() to create a delayed, elastic flame movement effect.
	// This allows the flame to slightly lag behind when the lighter moves quickly.
	FlameVisual = CreateDefaultSubobject<USceneComponent>(TEXT("FlameVisual"));
	FlameVisual->SetupAttachment(Root_);

	// The actual Niagara flame component can be attached to FlameVisual
	// either in Blueprint or during component initialization.


	FlameDetector = CreateDefaultSubobject<USphereComponent>(TEXT("FlameDetector"));
	FlameDetector->SetupAttachment(FlameVisual);

	FlameDetector->SetSphereRadius(8.0f);

	// Detects objects entering and leaving the flame area.
	FlameDetector->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}


void AGMTK_LighterActor::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the flame position to match the lighter tip.
	FlameVisual->SetWorldLocation(FlameSocket->GetComponentLocation());

	FlameDetector->OnComponentBeginOverlap.AddDynamic(this, &AGMTK_LighterActor::OnFlameBeginOverlap);
	FlameDetector->OnComponentEndOverlap.AddDynamic(this, &AGMTK_LighterActor::OnFlameEndOverlap);
}


void AGMTK_LighterActor::MoveTo(const FVector& NewWorldLocation)
{
	// The lighter body follows the player's input instantly.
	// The flame movement is handled separately to preserve the lag effect.
	Root_->SetWorldLocation(NewWorldLocation);
}


void AGMTK_LighterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// Smoothly interpolate the flame position toward the lighter tip,
	// creating a delayed "elastic" movement effect.
	const FVector Target = FlameSocket->GetComponentLocation();
	const FVector Current = FlameVisual->GetComponentLocation();

	const FVector NewLoc = FMath::VInterpTo(Current, Target, DeltaTime, FlameLagSpeed);

	FlameVisual->SetWorldLocation(NewLoc);


	// Update all notes currently inside the flame detection area.
	// Each note accumulates burning time while exposed to the flame.
	for (AGMTK_BurnableNote* Note : OverlappingNotes)
	{
		if (IsValid(Note))
		{
			Note->SetFlameOverlapping(true, DeltaTime);
		}
	}
}


void AGMTK_LighterActor::OnFlameBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (AGMTK_BurnableNote* Note = Cast<AGMTK_BurnableNote>(OtherActor))
	{
		// Store notes currently affected by the flame.
		OverlappingNotes.Add(Note);
	}
}


void AGMTK_LighterActor::OnFlameEndOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (AGMTK_BurnableNote* Note = Cast<AGMTK_BurnableNote>(OtherActor))
	{
		// Remove the note from the active burning list
		// and reset its accumulated burning progress.
		OverlappingNotes.Remove(Note);

		Note->SetFlameOverlapping(false, 0.0f);
	}
}