// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class DEADHUNT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	void SetVelocity(FVector& Speed) const;
	void Deflect(const FVector& Velocity);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnyWhere)
	int Power = 25;
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollision = nullptr;
	
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	bool HasDeflected = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
