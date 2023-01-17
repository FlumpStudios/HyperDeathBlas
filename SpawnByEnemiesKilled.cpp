#include "SpawnByEnemiesKilled.h"
#include <DeadHunt/Logger.h>

USpawnByEnemiesKilled::USpawnByEnemiesKilled()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	if (SectionNumber == 0)
	{
		Logger::LogError("It looks like the level section hasn't been set on the spawn by enemies component", this->GetName());
	}
}

void USpawnByEnemiesKilled::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// TODO: This is a bit shit, as it listens on each tick. Could be handled better by acting on an enemy dying.
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (UPlayerStateController::LevelSection == SectionNumber)
	{	
		UPlayerStateController::RemainingEnemiesInSection = EnemiesRequiredForSpawn - UPlayerStateController::EnemiesKilled;
	}
	else
	{
		// If it's not the current section then get the hell of of this function
		return;
	}

	if (UPlayerStateController::EnemiesKilled >= EnemiesRequiredForSpawn)
	{
		SpawnObject();
	}
}