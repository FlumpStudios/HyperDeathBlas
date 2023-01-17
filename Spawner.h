// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Spawner.generated.h"

UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEADHUNT_API USpawner : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpawner();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

protected:
	virtual void BeginPlay() override;
	void SpawnObject() const;
	
	// The object to spawn
	UPROPERTY(EditAnywhere)
	UClass* ObjectToSpawn = nullptr;

	// Should the spawner be destroy the parent actor after spawning the object
	UPROPERTY(EditAnywhere)
	bool bDestroyOnSpawn = false;
};


