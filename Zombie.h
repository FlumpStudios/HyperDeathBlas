#pragma once
#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Zombie.generated.h"

UCLASS()
class DEADHUNT_API AZombie : public AEnemyBase
{
	GENERATED_BODY()
	
	protected:
		virtual void BeginPlay() override;
		virtual void SetActive(bool Active) override;
	
	public:	
		virtual void Tick(float DeltaTime) override;
		AZombie();
};
