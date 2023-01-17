#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include <DeadHunt\Tags.h>
#include "ExplodeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UExplodeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UExplodeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Hit(int Power);
	int GetHealth() const;
	bool GetExplodeOnCollision() const;
	float GetTimeToDetonate() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UClass* SpawnObject = nullptr;
	
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bCanBeDamaged"))
	int Health = 0;

	UPROPERTY(EditAnywhere)
	bool bExplodeOnCollision = false;

	UPROPERTY(EditAnywhere)
	bool bCanBeDamaged = true;

	UPROPERTY(EditAnywhere)
	bool bExplodeAfterTime = false;

	UPROPERTY(EditAnywhere, meta = (EditCondition = "bExplodeAfterTime"))
	float TimeToDetonate = 1.f;

	float DetonateStartTime = 0;

	void Destroy();

	bool bHasSpawned = false;

	UFUNCTION()
	void OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// If explode on collision is set, what tags must be set 
	UPROPERTY(EditAnywhere,meta = (EditCondition = "bExplodeOnCollision"))
	TSet<FName> CollisionTags = { Tags::ENEMY };
};


