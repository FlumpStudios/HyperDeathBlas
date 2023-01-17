// Fill out your copyright notice in the Description page of Project Settings.
#include "Spawner.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include <DeadHunt\Logger.h>
#include "GameFramework/Actor.h"

USpawner::USpawner()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USpawner::SpawnObject() const
{
	if(ObjectToSpawn)
	{
		auto owner = GetOwner();
		FVector l = owner->GetActorLocation();
		FRotator r = owner->GetActorRotation();		
		GetWorld()->SpawnActor(ObjectToSpawn, &l, &r);

		if (bDestroyOnSpawn)
		{	
			owner->Destroy();
		}
	}
	else
	{
		Logger::LogWarn("Spawner is trying to spawn and object, but no actor to spawn has been set", GetOwner()->GetName());		
	}
}

void USpawner::BeginPlay()
{
	Super::BeginPlay();
}

void USpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}