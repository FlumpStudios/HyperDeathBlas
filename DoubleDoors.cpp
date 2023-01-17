#include "DoubleDoors.h"
#include "Engine/World.h"
#include <DeadHunt\Tags.h>
#include <DeadHunt\Logger.h>

ADoubleDoors::ADoubleDoors()
{
	PrimaryActorTick.bCanEverTick = true;
	SecondDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Second door mesh"));
	SecondDoorStopPositionMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Second door stop position marker"));
}

void ADoubleDoors::BeginPlay()
{
	Super::BeginPlay();	
	if (SecondDoorStopPositionMarker)
	{
		SecondDoorStopPositionMarker->SetVisibility(false);
		SecondDoorStopPosition = SecondDoorStopPositionMarker->GetComponentLocation();
	}
	else
	{	
		Logger::LogWarn("Couldn't find a stop position mesh for second door", this->GetName());
	}

	if (SecondDoorMesh)
	{
		SecondDoorStartPosition = SecondDoorMesh->GetComponentLocation();
	}
	else
	{
		Logger::LogWarn("Couldn't find a mesh for the second door", this->GetName());
	}
}

void ADoubleDoors::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADoubleDoors::RunDoorAction(DoorAction Action, float Dt) const
{
	FVector Destination1;
	FVector Destination2;

	if (Action == DoorAction::Open)
	{
		Destination1 = DoorStopPosition;
		Destination2 = SecondDoorStopPosition;
	}
	else if (Action == DoorAction::Close)
	{
		Destination1 = DoorStartPosition;
		Destination2 = SecondDoorStartPosition;
	}

	FVector Door1Locale = DoorMesh->GetComponentLocation();
	FVector Door2Locale = SecondDoorMesh->GetComponentLocation();
	
	DoorMesh->SetWorldLocation(FMath::VInterpConstantTo(Door1Locale, Destination1, Dt, OpenSpeed));
	SecondDoorMesh->SetWorldLocation(FMath::VInterpConstantTo(Door2Locale, Destination2, Dt, OpenSpeed));
}