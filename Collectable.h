// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FacePlayer.h"
#include "DrawDebugHelpers.h"
#include "PaperSpriteComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include <DeadHunt\PickupTypes.h>
#include <DeadHunt\tags.h>
#include <DeadHunt\PlayerStateController.h>
#include "Collectable.generated.h"

UCLASS()
class DEADHUNT_API ACollectable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACollectable();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;


private:	
	UPaperSpriteComponent* MainSprite = nullptr;

	UPROPERTY(EditAnywhere)
	USphereComponent* Mesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementProjectileComponent = nullptr;
	
	// Display the debug line even if debug mode is not set to false
	UPROPERTY(EditAnywhere, category = "Debug")
	bool bForceShowDebugLine = false;

	// The distance the pickup is from player when it attaches and gets sucked it.
	UPROPERTY(EditAnywhere, category = "Attach")
	int AttachDistance = 800;

	// The amount of time between spawning and attaching to player
	UPROPERTY(EditAnywhere, category = "Attach")
	float DelayBeforeRunAttach = 0.75f;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPickUps> PickUpType;

	UPROPERTY(EditAnywhere)
	bool bUsePhysics = true;

	// The distance in which the collectable is considered 'picked up'
	UPROPERTY(EditAnywhere)
	int ActionPickUpDistance = 250;
	
	UPROPERTY()
	APlayerController* PlayerController = nullptr;

	UPROPERTY()
	UPlayerStateController* PlayerState = nullptr;

	UPROPERTY()
	AActor* PlayerActor = nullptr;	

	bool bIsReadyToCollect = false;

	void SetSpriteLookAtPlayer() const;

	int GetDistanceFromPlayer() const;

	float SpawnTime = 0;
};
