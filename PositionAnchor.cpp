#include "PositionAnchor.h"
UPositionAnchor::UPositionAnchor()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPositionAnchor::BeginPlay()
{
	Super::BeginPlay();
}

void UPositionAnchor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

