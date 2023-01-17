// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileRangedEnemy.generated.h"

UCLASS()
class DEADHUNT_API AProjectileRangedEnemy : public AEnemyBase
{
	GENERATED_BODY()
	public:
		virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
		virtual void Tick(float DeltaTime) override;
		AProjectileRangedEnemy();

	protected:
		virtual void BeginPlay() override;	

	private:
		UPROPERTY(Category = "Projectile", EditAnywhere)
		uint32 bAutomaticProjectile : 1;

		// TODO: Finish the projectile.cpp class and replace the UClass here with that.
		UPROPERTY(EditAnywhere,Category="Projectile", meta = (EditCondition= "bHasrangedAttack"))
		UClass* Projectile = nullptr;					

		UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "bAutomaticProjectile"))
		float SpawnFrequency = 0.125f;

		UPROPERTY(EditAnywhere, Category = "Projectile", meta = (EditCondition = "bAutomaticProjectile"))
		int ProjectileSpeed = 400;
		
		float LastSpawn = 0;

		bool bHasProjectileBeenFired = false;

		void SpawnProjectile() const;
};
