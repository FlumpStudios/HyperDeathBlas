#include "ToggleEnabledAfterTime.h"

UToggleEnabledAfterTime::UToggleEnabledAfterTime()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UToggleEnabledAfterTime::BeginPlay()
{
	Super::BeginPlay();
	TimeCreated = GetWorld()->GetTimeSeconds();
	if (bForceToggleOnLoad)
	{
		ToggleEnabled(!bDisable);
	}
}

void UToggleEnabledAfterTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	float Sec = GetWorld()->GetTimeSeconds();
	
	if (Sec > TimeCreated + Delay)
	{
		ToggleEnabled(bDisable);
	}
}

void UToggleEnabledAfterTime::ToggleEnabled(bool ShouldDisable) const
{
	AActor* Owner = GetOwner();
	if (Owner)
	{
		Owner->SetActorEnableCollision(!ShouldDisable);
		Owner->SetActorHiddenInGame(ShouldDisable);
		Owner->SetActorTickEnabled(!ShouldDisable);
	
		if (bIncludeComponents)
		{		
			TSet<UActorComponent*> Components = Owner->GetComponents();
			for (auto& Component : Components)
			{
				if (Component != this)
				{
					Component->SetComponentTickEnabled(!ShouldDisable);
					Component->SetActive(!ShouldDisable);
				}	
			}	
		}
	}
}

void UToggleEnabledAfterTime::SetDelay(float NewDelay)
{
	Delay = NewDelay;
}

float UToggleEnabledAfterTime::GetCurrentDelay() const
{
	return Delay;
}


void UToggleEnabledAfterTime::SetIncludeComponents(bool bShouldSetIncludeComponents)
{
	bIncludeComponents = bShouldSetIncludeComponents;
}

void UToggleEnabledAfterTime::SetShouldDisable(bool bShouldDisable)
{
	bDisable = bShouldDisable;
}

void UToggleEnabledAfterTime::SetForceToggleOnLoad(bool bShouldFoceToggleOnload)
{
	bForceToggleOnLoad = bShouldFoceToggleOnload;
}
