// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include <DeadHunt\tags.h>
#include "PhysicsProjectile.generated.h"


UCLASS()
class DEADHUNT_API APhysicsProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhysicsProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private: 
	// Sets the amount of health to take from enemy on hit
	UPROPERTY(EditAnyWhere)
	int Power = 25;

	// Sets the amount of health to take from enemy on hit
	UPROPERTY(EditAnyWhere)
	bool bShouldStunEnemy = true;
	
	// Sets the velocity at which point the projectile damages an enemy
	UPROPERTY(EditAnyWhere)
	int VelocityDamageThreshold = 500;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent = nullptr;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
