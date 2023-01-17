#include "PlaySoundOnCollision.h"
#include <DeadHunt\Logger.h>

UPlaySoundOnCollision::UPlaySoundOnCollision()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlaySoundOnCollision::BeginPlay()
{
	Super::BeginPlay();
	auto owner = GetOwner();
	CollisionComponent = owner->FindComponentByClass<UPrimitiveComponent>();
	if (CollisionComponent)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &UPlaySoundOnCollision::OnCollision);
	}	
}

void UPlaySoundOnCollision::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPlaySoundOnCollision::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ActorHasTagFromList(OtherActor))
	{	
		auto CurrentTime = GetWorld()->GetTimeSeconds();
		if ((PlayCount < MaxPlayCount || MaxPlayCount == 0) && 
			(CurrentTime > TimeSinceLastPlay + DelayBeforeNextPlay || PlayCount == 0))
		{
			TimeSinceLastPlay = GetWorld()->GetTimeSeconds();

			AActor* Owner = GetOwner();
			if (!Owner) { return; }

			FVector Locale = GetOwner()->GetActorLocation();
			auto World = GetWorld();
			if (SoundToPlay)
			{
				UGameplayStatics::PlaySoundAtLocation(World, SoundToPlay, Locale, VolumeMultiplier, PitchMultiplier);
			}
			else
			{
				Logger::LogWarn(("No SoundBase component found"), *Owner->GetName());
			}

			VolumeMultiplier += VolumeDropOff;
			PlayCount++;
		}
	}
}

bool UPlaySoundOnCollision::ActorHasTagFromList(const AActor* OtherActor) const
{
	// If no tags present in list, then assume we want to play sound on all collisions
	if (CollisionTags.Num() == 0)
	{
		return true;
	}

	for (const FName &CollisionTag : CollisionTags)
	{
		if (OtherActor->ActorHasTag(CollisionTag))
		{		
			return true;
		}
	}
	return false;
}
