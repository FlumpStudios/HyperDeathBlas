// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ToggleEnabledAfterTime.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UToggleEnabledAfterTime : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UToggleEnabledAfterTime();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetCurrentDelay() const;
	void SetDelay(float NewDelay);

	void SetIncludeComponents(bool bShouldSetIncludeComponents);
	void SetShouldDisable(bool bShouldDisable);
	void SetForceToggleOnLoad(bool bShouldFoceToggleOnload);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	float Delay = 1;
	
	// On actor load, should the actor be set to the oppersite of the Disable field on load
	UPROPERTY(EditAnywhere)
	bool bForceToggleOnLoad = false;

	// Should the actor be disabled rather than enabled after the delay	
	UPROPERTY(EditAnywhere)
	bool bDisable = false;

	UPROPERTY(EditAnywhere)
	bool bIncludeComponents = false;

	void ToggleEnabled(bool ShouldDisable) const;
	
	float TimeCreated = 0;
};
