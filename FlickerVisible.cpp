#include "FlickerVisible.h"
#include "Math/UnrealMathUtility.h"
#include <DeadHunt\Logger.h>
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


UFlickerVisible::UFlickerVisible()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFlickerVisible::BeginPlay()
{
	LastOnPosition = GetOwner()->GetActorLocation();
	Super::BeginPlay();
}

void UFlickerVisible::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	auto Sec = GetWorld()->GetTimeSeconds();
		
	if (Sec > LastFlickerStartTime + CurrentFlickerTime)
	{
		ToggleVisible();
		UpdateFlickerLifeTime();
		LastFlickerStartTime = Sec;
	}
}

void UFlickerVisible::ToggleVisible()
{	
	auto Owner = GetOwner();
	FVector SafeHiddenLocation = FVector(-100000,-100000,-100000);
	
	bIsVisible = !bIsVisible;	

	if (bIsVisible)
	{
		Owner->SetActorLocation(LastOnPosition);
	}
	else
	{
		LastOnPosition = Owner->GetActorLocation();
		Owner->SetActorLocation(SafeHiddenLocation);
	}
}

void UFlickerVisible::UpdateFlickerLifeTime()
{
	if (UseRandom)
	{
		CurrentFlickerTime = FMath::FRandRange(RandomRange.Min, RandomRange.Max);
	}
	else
	{
		TimeArrayIndex++;

		if (TimeArrayIndex > 9)
		{
			TimeArrayIndex = 0;
		};

		CurrentFlickerTime = FlickerLifetimes[TimeArrayIndex];
	}
}