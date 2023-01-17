// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Door.h"
#include "DoubleDoors.generated.h"

UCLASS()
class DEADHUNT_API ADoubleDoors : public ADoor
{
	GENERATED_BODY()
	
public:	
	ADoubleDoors();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Category = "Meshes")
	UStaticMeshComponent* SecondDoorMesh = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Meshes")
	UStaticMeshComponent* SecondDoorStopPositionMarker = nullptr;		
	
	FVector SecondDoorStartPosition;	
	FVector SecondDoorStopPosition;
	
	void RunDoorAction(DoorAction Action, float Dt) const override;
};
