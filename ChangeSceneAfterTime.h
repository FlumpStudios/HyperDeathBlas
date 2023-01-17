#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include <DeadHunt\GameInstanceMain.h>
#include "ChangeSceneAfterTime.generated.h"

UCLASS()
class DEADHUNT_API AChangeSceneAfterTime : public AActor
{
	GENERATED_BODY()
	
public:	
	AChangeSceneAfterTime();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	float TimeToSceneChange = 0.f;

	UPROPERTY(EditAnywhere)
	FName LevelToLoad = "MainMenu";

	UPROPERTY()
	UGameInstanceMain* GameInstance = nullptr;

	int GameWorldToLoad = 0;
	int GameLevelToLoad = 0;
};


