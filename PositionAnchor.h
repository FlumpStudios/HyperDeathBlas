#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PositionAnchor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UPositionAnchor : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPositionAnchor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
