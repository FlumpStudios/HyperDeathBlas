// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"	
#include "Engine/World.h"
#include "Components/ActorComponent.h"
#include "PlaySoundAfterDelay.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UPlaySoundAfterDelay : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlaySoundAfterDelay();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere)
	float Delay = 1.0f;

	UPROPERTY(EditAnywhere)
	USoundBase* SoundToPlay = nullptr;

	UPROPERTY(EditAnywhere)
	float VolumeMultiplier = 1.f;

	UPROPERTY(EditAnywhere)
	float PitchMultiplier = 1.f;

	bool HasPlayed = false;

		
};
