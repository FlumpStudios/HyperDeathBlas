// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "FadeCameraAfterTime.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UFadeCameraAfterTime : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFadeCameraAfterTime();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

private:
	UCameraComponent* CurrentCamera = nullptr;
	
	float CurrentValue = 0;

	UPROPERTY(EditAnywhere)
	float FadeSpeed = 0.1f;
	
	UPROPERTY(EditAnywhere)
	float StartValue = 0.0f;

	UPROPERTY(EditAnywhere)
	float EndValue = 1.f;

	UPROPERTY(EditAnywhere)
	float Delay = 0;
};
