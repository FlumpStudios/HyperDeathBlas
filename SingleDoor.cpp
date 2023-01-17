#include "SingleDoor.h"

ASingleDoor::ASingleDoor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASingleDoor::BeginPlay()
{
	Super::BeginPlay();
}

void ASingleDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASingleDoor::RunDoorAction(DoorAction Action, float Dt) const
{
	FVector Destination;

	if (Action == DoorAction::Open)
	{
		Destination = DoorStopPosition;	
	}
	else if (Action == DoorAction::Close)
	{
		Destination = DoorStartPosition;		
	}

	FVector Door1Locale = DoorMesh->GetComponentLocation();
	
	DoorMesh->SetWorldLocation(FMath::VInterpConstantTo(Door1Locale, Destination, Dt, OpenSpeed));	
}