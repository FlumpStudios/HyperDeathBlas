// Fill out your copyright notice in the Description page of Project Settings.

#include "FlashComponents.h"
#include <DeadHunt/Logger.h>
UFlashComponents::UFlashComponents()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UFlashComponents::BeginPlay()
{
	Super::BeginPlay();
	TSet<UActorComponent*> Components = GetOwner()->GetComponents();
	for (const auto& Component: Components)
	{
		if (Component->GetName() == ComponentName)
		{
			ActorComponent = Cast<USceneComponent>(Component);
			if (!ActorComponent)
			{
				Logger::LogWarn("Actor found with name of " + ComponentName + " but could not cast to a scene component", this->GetName());
			}
			else
			{
				Logger::LogInfo("Scene component found with name of " + ComponentName, this->GetName());
			}
			continue;
		}
	}

	if (!ActorComponent)
	{
		Logger::LogWarn("No ActorComponent found with name of " + ComponentName, this->GetName());
	}
}



// Called every frame
void UFlashComponents::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	float Sec = GetWorld()->GetTimeSeconds();

	if (bIsOn)
	{
		if (Sec > LastSwitchTime + TimeOn)
		{
			bIsOn = false;
			ActorComponent->SetHiddenInGame(true, true);
			LastSwitchTime = Sec;
			return;
		}
	}
	else
	{
		if (Sec > LastSwitchTime + TimeOff)
		{
			bIsOn = true;
			ActorComponent->SetHiddenInGame(false, true);
			LastSwitchTime = Sec;
			return;
		}
	}
}

