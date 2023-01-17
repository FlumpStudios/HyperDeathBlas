// Fill out your copyright notice in the Description page of Project Settings.

#include "PlaySoundAfterDelay.h"
#include <DeadHunt\Logger.h>

// Sets default values for this component's properties
UPlaySoundAfterDelay::UPlaySoundAfterDelay()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlaySoundAfterDelay::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UPlaySoundAfterDelay::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (HasPlayed)
	{
		return;
	}
	auto sec = GetWorld()->GetTimeSeconds();
	AActor* Owner = GetOwner();
	if (!Owner) { return; }

	if (sec > Delay)
	{
		if (SoundToPlay)
		{

			FVector Locale = GetOwner()->GetActorLocation();

			UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, Locale, VolumeMultiplier, PitchMultiplier);		
			HasPlayed = true;
		}			
		else
		{
			Logger::LogWarn(("No SoundBase component found"), *Owner->GetName());
		}
	}

}

