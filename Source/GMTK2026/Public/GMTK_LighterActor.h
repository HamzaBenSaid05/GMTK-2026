#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GMTK_LighterActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UParticleSystemComponent; 
class USphereComponent;
class AGMTK_BurnableNote;

UCLASS(Blueprintable)
class AGMTK_LighterActor : public AActor
{
	GENERATED_BODY()

public:
	AGMTK_LighterActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighter")
	TObjectPtr<USceneComponent> Root_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighter")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	// Flame start where the lighter body is, but the flame visual will lag behind it.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighter")
	TObjectPtr<USceneComponent> FlameSocket;

	// Niagara that attached to the FlameSocket, but it will be interpolated manually in Tick()
	// towards the world position of the FlameSocket
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighter")
	TObjectPtr<USceneComponent> FlameVisual;

	// Sphere detector on the tip of the flame for overlap with the notes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lighter")
	TObjectPtr<USphereComponent> FlameDetector;

	// Flame velocity lag speed. Lower = more elastic/moving.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighter")
	float FlameLagSpeed = 8.0f;

	// If trye the player is dragging the lighter 
	UPROPERTY(BlueprintReadOnly, Category = "Lighter")
	bool bIsDragging = false;

	UFUNCTION(BlueprintCallable, Category = "Lighter")
	void MoveTo(const FVector& NewWorldLocation);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnFlameBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnFlameEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// Notes under the flame
	UPROPERTY()
	TSet<TObjectPtr<AGMTK_BurnableNote>> OverlappingNotes;
};
