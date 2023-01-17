#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "PlaySoundOnCollision.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UPlaySoundOnCollision : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlaySoundOnCollision();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	// The sound to play
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* SoundToPlay = nullptr;

	UPROPERTY(EditAnywhere, Category = "Audio")
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	float VolumeMultiplier = 1.0f;

	// The number of times the sound is allowed to play. Set to zero for unlimited play count
	UPROPERTY(EditAnywhere, Category = "Audio")
	int MaxPlayCount = 0;

	int PlayCount = 0;

	// After playing a sound make lower or raise the volume for the next colision
	UPROPERTY(EditAnywhere, Category = "Audio")
	float VolumeDropOff = .0f;


	// How soon after playing a sound can another sound be played
	UPROPERTY(EditAnywhere, Category = "Audio")	
	float DelayBeforeNextPlay = .0f;


	float TimeSinceLastPlay = 0;

	// Limit the colision to objects in this list. If empty the sound will play on all collisions
	UPROPERTY(EditAnywhere, Category = "Collision")
	TArray<FName> CollisionTags;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY()
	UPrimitiveComponent* CollisionComponent = nullptr;
	
	UFUNCTION()
	bool ActorHasTagFromList(const AActor* OtherActor) const;


};
