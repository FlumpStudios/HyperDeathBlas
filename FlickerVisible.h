
#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/ActorComponent.h"
#include "FlickerVisible.generated.h"

USTRUCT()
struct FRandomFlickerRange
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category = "Configuration")
	float Min = 0;

	UPROPERTY(EditAnywhere, Category = "Configuration")
	float Max = 1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UFlickerVisible : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFlickerVisible();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Configuration")
	bool UseRandom = false;

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "!UseRandom"))
	TArray<float> FlickerLifetimes = { 0.25f, 0.5f,0.35f,1,3,1.5f,0.2f,0.5f,2,1 };

	UPROPERTY(EditAnywhere, Category = "Configuration", meta = (EditCondition = "UseRandom"))
	FRandomFlickerRange RandomRange;

	void ToggleVisible();

	void UpdateFlickerLifeTime();

	bool bIsVisible = false;

	int	TimeArrayIndex = 0;

	float LastFlickerStartTime = 0.f;

	float CurrentFlickerTime = 1;

	FVector LastOnPosition; 
};
