// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include <DeadHunt\PlayerStateController.h>
#include "PlayerControlsComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UPlayerControlsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerControlsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private: 
	UInputComponent* Input = nullptr;
	
	UPlayerStateController* PlayerState = nullptr;
	
	UFUNCTION(Category="CallBack")
	void HandleFireGun();
	
	UFUNCTION(Category = "CallBack")
	void HandleStoppedFiring();

	UFUNCTION(Category = "CallBack")
	void HandleMeleeAttack();

	UFUNCTION(Category = "CallBack")
	void GroundSlam();

	UFUNCTION(Category = "CallBack")
	void HandleJump();

	UFUNCTION(Category = "CallBack")
	void HandleFocus();

	UFUNCTION(Category = "CallBack")
	void HandleStopFocus();

	UFUNCTION(Category = "CallBack")
	void HandleWeaponChange();

	UFUNCTION(Category = "CallBack")
	void HandleFireGranade();


	// NOTE: Pause is handled by a blueprints, coz I was lazy

	bool AssertPlayerState();
};
