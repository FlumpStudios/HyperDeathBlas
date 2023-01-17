// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Door.h"
#include "SingleDoor.generated.h"

UCLASS()
class DEADHUNT_API ASingleDoor : public ADoor
{

GENERATED_BODY()
public:
	ASingleDoor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	void RunDoorAction(DoorAction Action, float Dt) const override;
};
