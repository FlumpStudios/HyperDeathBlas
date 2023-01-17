#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include <DeadHunt\PlayerStateController.h>
#include "Door.generated.h"

UCLASS(Abstract)
class DEADHUNT_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	ADoor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	
	enum DoorAction : uint8
	{
		Open,
		Close
	};

	UFUNCTION(Category = "Callback")
	virtual void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(Category = "Callback")
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Properties")
	float OpenSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category = "Properties")
	float OpenTime = 1.75f;

	bool bShouldRunOpen = false;
	FVector DoorStartPosition;
	FVector DoorStopPosition;
	float TimePlayerLastCrossedTrigger = 0.f;
	virtual void RunDoorAction(DoorAction Action, float Dt) const;
	UPROPERTY(EditAnywhere, Category = "Meshes")
	UStaticMeshComponent* DoorMesh = nullptr;


private:
	// The doorway mesh
	UPROPERTY(EditAnywhere, Category = "Meshes")
	UStaticMeshComponent* DoorWay = nullptr;
	
	// Marker mesh to set where the door should stop
	UPROPERTY(EditAnywhere, Category = "Meshes")
	UStaticMeshComponent* DoorStopPositionMarker = nullptr;

	// Collision box to trigger the door opening event
	UPROPERTY(EditAnywhere, Category = "Trigger")
	UBoxComponent* TriggerBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	int RequiredKeysToUnlock = 0;

	// Does the door need a player to be in the trigger box to activate. If not door will open as soon as enough keys are collected
	UPROPERTY(EditAnywhere, Category = "Trigger")
	bool bTriggerBoxRequired = false;

	bool bIsInTriggerBox = false;
};