#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScaleOverTime.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UScaleOverTime : public UActorComponent
{
	GENERATED_BODY()

public:	
	UScaleOverTime();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool GetHasFinishedScaling() const;
	void OnComplete();
	
	void SetDestroyParentOnComplete(bool ShouldDestroyOnComplete);
	void SetFinalScale(const FVector& NewFinalScale);
	void SetSpeed(float NewSpeed);
	void SetDelay(float NewDelay);
	
protected:
	virtual void BeginPlay() override;
	
private:		
	// The Target scale Vector
	UPROPERTY(EditAnywhere)
	FVector FinalScale = FVector(0,0,0);

	// Delay before scaling starts
	UPROPERTY(EditAnywhere)
	float Delay = 0;

	// Should the parent object be destroyed once scaling has finished
	UPROPERTY(EditAnywhere)
	bool DestroyParentOnComplete = false;

	// The speed in which the scaling happens
	UPROPERTY(EditAnywhere)
	float Speed = 1;

	UPROPERTY()
	UPrimitiveComponent* Primative = nullptr;

	bool bHasFinishedScaling = false;

	float StartTime = 0;
};
