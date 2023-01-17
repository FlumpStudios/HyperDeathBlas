#pragma once

#include "CoreMinimal.h"
#include "Spawner.h"
#include  <DeadHunt\PlayerStateController.h>
#include "SpawnByEnemiesKilled.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DEADHUNT_API USpawnByEnemiesKilled : public USpawner
{
public:
	GENERATED_BODY()
	USpawnByEnemiesKilled();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Number of enemies the player needs to kill to spawn the object
	UPROPERTY(EditAnywhere)
	int EnemiesRequiredForSpawn = 0;

	// Number of enemies the player needs to kill to spawn the object
	UPROPERTY(EditAnywhere)
	int SectionNumber = 0;
};
