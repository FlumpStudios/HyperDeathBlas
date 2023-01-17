#include "ChangeSceneAfterTime.h"

AChangeSceneAfterTime::AChangeSceneAfterTime()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AChangeSceneAfterTime::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UGameInstanceMain>(GetWorld()->GetGameInstance());
	
}

void AChangeSceneAfterTime::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto Sec = GetWorld()->GetTimeSeconds();

	if (Sec > TimeToSceneChange)
	{
		GameInstance->CurrentWorld = GameWorldToLoad;
		GameInstance->CurrentLevel = GameLevelToLoad;

		UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
	}
}

