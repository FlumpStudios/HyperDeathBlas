// Fill out your copyright notice in the Description page of Project Settings.


#include "CriticalHitPointComponent.h"
#include <DeadHunt/Configuration.h>
#include <DeadHunt/Tags.h>

UCriticalHitPointComponent::UCriticalHitPointComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	this->ComponentTags.Add(Tags::CRITICAL_HIT);
	this->SetCollisionProfileName(CollisionPresets::EnemyCollisionPreset);
}


void UCriticalHitPointComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}
