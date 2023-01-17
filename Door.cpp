#include "Door.h"
#include "Engine/World.h"
#include <Deadhunt\Logger.h>
#include <DeadHunt\Tags.h>

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;
	DoorWay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Doorway mesh"));
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door mesh"));
	DoorStopPositionMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door stop position marker"));
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Open door trigger"));
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SetRootComponent(DoorWay);

	if (TriggerBox)
	{
		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnOverLap);
		TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnOverlapEnd);
	}
	else
	{
		Logger::LogWarn("Couldn't find trigger box", this->GetName());
	}
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();
	if (DoorStopPositionMarker)
	{
		DoorStopPositionMarker->SetVisibility(false);
		DoorStopPosition = DoorStopPositionMarker->GetComponentLocation();
	}
	else
	{
		Logger::LogWarn("Couldn't find a stop position mesh for door", this->GetName());
	}
	

	if (DoorMesh)
	{
		DoorStartPosition = DoorMesh->GetComponentLocation();
	}
	else
	{
		Logger::LogWarn("Couldn't find a mesh for door", this->GetName());
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto Sec = GetWorld()->GetTimeSeconds();	

	if (bShouldRunOpen)
	{
		RunDoorAction(DoorAction::Open, DeltaTime);
	}

	if (bTriggerBoxRequired)
	{
		if (Sec > TimePlayerLastCrossedTrigger + OpenTime)
		{
			if (!bIsInTriggerBox)
			{
				bShouldRunOpen = false;
				RunDoorAction(DoorAction::Close, DeltaTime);
			}
		}
	}
	else
	{
		// Door should open automatically when enough keys are collected
		if (UPlayerStateController::KeysCollected >= RequiredKeysToUnlock)
		{
			bShouldRunOpen = true;
		}
	}
}

void ADoor::RunDoorAction(DoorAction Action, float Dt) const	
{	
	// Psudo Abstract method...coz Unreal is weird about abstract classes apparently
}

void ADoor::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bTriggerBoxRequired)
	{
		return;
	}

	if (OtherActor->ActorHasTag(Tags::PLAYER))
	{
		UPlayerStateController* PlayerState = OtherActor->FindComponentByClass<UPlayerStateController>();

		if (!PlayerState)
		{		
			Logger::LogWarn("Could not find player state controller on object with a player component tag", this->GetName());		
		}
		else
		{
			if (UPlayerStateController::KeysCollected >= RequiredKeysToUnlock)
			{
				bIsInTriggerBox = true;
				bShouldRunOpen = true;
			}
		}
	}
}

void ADoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bTriggerBoxRequired)
	{
		return;
	}

	if (OtherActor->ActorHasTag(Tags::PLAYER))
	{
		bIsInTriggerBox = false;
		TimePlayerLastCrossedTrigger = GetWorld()->GetTimeSeconds();
	}
}

