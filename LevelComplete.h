#pragma once
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelComplete.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API ULevelComplete : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelComplete();	
	
protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere)
	FName LevelToLoad = "LevelEnd";
	
	UPROPERTY()
	UBoxComponent* LevelEndCollider = nullptr;
	
	UFUNCTION()
	void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
