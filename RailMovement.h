// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <DeadHunt/Logger.h>
#include "Components/ActorComponent.h"
#include "Containers/Array.h"
#include <DeadHunt\Configuration.h>
#include "Components/SplineComponent.h"
#include "RailMovement.generated.h"

USTRUCT()
struct FTransformFrame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Speed = 0;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 0;

	UPROPERTY(EditAnywhere)
	FVector Location = FVector(0, 0, 0);

	UPROPERTY(EditAnywhere)
	FRotator Roatation = FRotator(0,0,0);
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEADHUNT_API URailMovement : public UActorComponent
{
	GENERATED_BODY()		

public:
	// Sets default values for this component's properties
	URailMovement();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnFinished();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// The position data for each frame
	UPROPERTY(EditAnywhere, Category = "Position Data")
	TArray<FTransformFrame> TransformData;	

	// Should the animation repeat when finished
	UPROPERTY(EditAnywhere, Category = "Controls")
	bool bRepeat = false;

	UPROPERTY(EditAnywhere, Category = "Advanced")
	float LerpEpsilon = 10;

	// The base speed for the whole animation. Each frame speed is added to this
	UPROPERTY(EditAnywhere, Category = "Controls")
	float GlobalSpeed = 1000;

	// The maximum speed the animation can reach. Set to 0 for no limit.
	UPROPERTY(EditAnywhere, Category = "Controls")
	float MaxMoveSpeed = 10000;

	// The maximum rotation speed the animation can reach. Set to 0 for no limit.
	UPROPERTY(EditAnywhere, Category = "Controls")
	float MaxRotationSpeed = 0;

	// How quickly the animation should accelerate
	UPROPERTY(EditAnywhere, Category = "Controls")
	float AccelerationModifier = 0;

	// IF true the acceleration will be reset on next play
	UPROPERTY(EditAnywhere, Category = "Controls")
	bool bShouldResetAccelerationOnComplete = false;

	// Should the animation play on spawn
	UPROPERTY(EditAnywhere, Category = "Controls")
	bool bPlayOnStart = false;
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void ResetAnimation();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void StartAnimation();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void StopAnimation();	

	UPROPERTY(EditAnywhere, Category = "Controls")
	float GlobalRotationSpeed = 0;

	UPROPERTY(EditAnywhere, Category = "Controls")
	bool DestoryOnComplete = false;

	UPROPERTY(EditAnywhere, Category = "Controls", meta = (EditCondition = "bPlayOnStart"))
	float DelayBeforeInitalPlay = 0.f;

	UFUNCTION(CallInEditor, BlueprintCallable)
	void PopulateTransformWithSplineData();

	UFUNCTION(CallInEditor, BlueprintCallable)
	void PopulateSplineWithTransformData();

	bool AssertAndAttemptToAssignSplineComponent();
	
	bool IsLerpComplete(FVector& Locale) const;		
		
	int CurrentIndex = 0;

	bool bFinished = false;

	int Length = 0;

	bool bPlay = false;

	float AccumulatedAcceleration = 1;

	float TimeSpawned = 0;

	bool InitalPlayHasBeenSet = false;

	FVector OriginalLocation = FVector(0, 0, 0);

	FRotator OriginalRotation = FRotator(0, 0, 0);

	USplineComponent* SplineComponent = nullptr;
};

