// Fill out your copyright notice in the Description page of Project Settings.

#include "DestoryAfterTime.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"


// Sets default values for this component's properties
UDestoryAfterTime::UDestoryAfterTime()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDestoryAfterTime::BeginPlay()
{
	Super::BeginPlay();
	TimeOfSpawn = GetWorld()->GetTimeSeconds();
;
	// ...
	
}


// Called every frame
void UDestoryAfterTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(GetWorld()->GetTimeSeconds() > TimeOfSpawn + LifeTime)
	{
		GetOwner()->Destroy();
	}
}

