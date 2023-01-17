#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlashComponents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UFlashComponents : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFlashComponents();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere)
	float TimeOff = 1.f;

	UPROPERTY(EditAnywhere)
	float TimeOn = 1.f;

	// The name of the component you wish to flash
	UPROPERTY(EditAnywhere)
	FString ComponentName = TEXT("StaticMeshComponent");	

	// These scene component to flicker
	UPROPERTY()
	USceneComponent* ActorComponent = nullptr;

	bool bIsOn = false;

	float LastSwitchTime = 0;
};
