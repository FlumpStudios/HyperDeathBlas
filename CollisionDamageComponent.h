#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SphereComponent.h"
#include <DeadHunt\Configuration.h>
#include <DeadHunt\tags.h>
#include "CollisionDamageComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEADHUNT_API UCollisionDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UCollisionDamageComponent();

	UFUNCTION()
	void SetCollisionType(EDamageType NewDamageType);

	UFUNCTION()
	void SetDamage(int Damage);

	UFUNCTION()
	int GetDamage() const;

protected:
	virtual void BeginPlay() override;

private:
	UPrimitiveComponent* CollisionComponent = nullptr;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverLapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// How powerful is the collision damage
	UPROPERTY(EditAnyWhere)
	int Power = 100;

	// Should the component register damage when overlapping actor
	UPROPERTY(EditAnyWhere, Category = "Overlaps")
	bool RegisterOverlapResults = true;

	// Should the component continue to cause damage whist over lapping. If set to false, damage is only inflicted when the overlap beings.
	UPROPERTY(EditAnyWhere, Category = "Overlaps",  meta = (EditCondition = "RegisterOverlapResults"))
	bool bRunContinuousDamageOnOverlap = false;

	// Should the component register damage when a hit result is registered
	UPROPERTY(EditAnyWhere, Category = "Hits")
	bool RegisterHitResults = true;

	// Should the parent actor be destroyed when inflicting damage
	UPROPERTY(EditAnyWhere, Category = "Hits")
	bool DestoryOnHit = false;

	// Should the damage stun any stunnable actor when inflicting damage
	UPROPERTY(EditAnyWhere, Category = "Hits")
	bool bShouldStunEnemy = false;


	// Should the damage force the other actor back when inflicting damage
	UPROPERTY(EditAnyWhere, Category = "Hits")
	bool bShouldForceEnemyBack = false;

	// What type of damage is being inflicted
	UPROPERTY(EditAnyWhere)
	EDamageType DamageType = EDamageType::PlayerMelee;

	// If collision is on a physics object, how much force to apply
	UPROPERTY(EditAnyWhere, Category = "Hits")
	int PhysicsForceToApply = 10000;

	// Object to spawn if destroying on collision
	UPROPERTY(EditAnyWhere, Category = "Hits", meta = (EditCondition = "DestoryOnHit"))
	UClass* ObjectToSpawnOnDestroy = nullptr;

	// Do you need to specify what collision component to use, if false the first UPrimativeComponent will be used
	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bUseSpecificCollisionComponent = false;

	// Specify the name of the collision component you want to use, in case there are multiply on collision components on the actor
	UPROPERTY(EditAnywhere, Category = "Advanced", meta = (EditCondition = "bUseSpecificCollisionComponent"))
	FString CollisionComponentName;
		
	UPROPERTY()
	TMap<FString, AActor*> OverlappedActorList;

	void DamageEntity(AActor* Actor);

	void RunOnCollisionDetected(AActor* Actor);
};