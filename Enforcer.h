// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "Enforcer.generated.h"

/**
 * 
 */
UCLASS()
class DEADHUNT_API AEnforcer : public AEnemyBase
{
	GENERATED_BODY()

	protected:
	virtual void BeginPlay() override;
	
	public:	
		virtual void Tick(float DeltaTime) override;
		AEnforcer();	
};
