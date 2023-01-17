
#include "LevelComplete.h"
#include <Deadhunt\Logger.h>
#include <DeadHunt\Tags.h>

ULevelComplete::ULevelComplete()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULevelComplete::BeginPlay()
{
	Super::BeginPlay();
	auto Owner = GetOwner();
	if (Owner)
	{
		LevelEndCollider = GetOwner()->FindComponentByClass<UBoxComponent>();
	}
	else
	{
		Logger::LogWarn("Could not find owner", this->GetName());
	}

	if (LevelEndCollider)
	{
		LevelEndCollider->OnComponentBeginOverlap.AddDynamic(this, &ULevelComplete::OnOverLap);
	}
	else
	{
		Logger::LogError("Level complete script is added to an actor that does not have a box collider component!", this->GetName());
	}
}
void ULevelComplete::OnOverLap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor->ActorHasTag(Tags::PLAYER))
	{
		if (LevelToLoad == "")
		{
			Logger::LogError("No level has been selected for the level end blueprint", this->GetName());		
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), LevelToLoad);
		}
	}
}