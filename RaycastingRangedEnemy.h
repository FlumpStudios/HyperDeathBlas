#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "RaycastingRangedEnemy.generated.h"

UCLASS()
class DEADHUNT_API ARaycastingRangedEnemy : public AEnemyBase
{
	GENERATED_BODY()

	public:
		ARaycastingRangedEnemy();
		virtual void Tick(float DeltaTime) override;
		virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;				
		virtual void BeginPlay() override;

	private:
		UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
		int WeaponPower = 15;
		
		UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
		float WeaponRange = 15000;

		UPROPERTY(Category = "Behavior", EditAnywhere)
		bool bRunRangedAttackOnTouch = true;

		// Should ranged weapon debug line show even if debug mode isn't set to full
		UPROPERTY(Category = "Debug", EditAnywhere)
		bool bForceShowRangedWeaponDebugLine = false;

		FVector CapturedRayEndpoint;				
		FVector GetLineTraceEnd();
		bool bHasRayBeenCaptured = false;
		bool bRayHasHitSomething;
		void RayCast(FVector& LineTraceEnd);
};