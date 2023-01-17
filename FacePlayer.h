// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "FacePlayer.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UFacePlayer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFacePlayer();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
	void SetFacingCoords(bool X, bool Y, bool Z);
	void SetYawOffset(int NewYawOffset);

protected:
	virtual void BeginPlay() override;

private:
	
	UPROPERTY(EditAnywhere)	
	bool bX;

	UPROPERTY(EditAnywhere)	
	bool bY;
	
	UPROPERTY(EditAnywhere)
	bool bZ;

	UPROPERTY(EditAnywhere)
	int YawOffet = 90;
};
