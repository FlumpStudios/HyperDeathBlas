#include "ExplodeComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UExplodeComponent::UExplodeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UExplodeComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	Owner->Tags.Add(FName(Tags::EXPLODABLE));

	if (bExplodeOnCollision)
	{
		UPrimitiveComponent* PrimativeComponent = Owner->FindComponentByClass<UPrimitiveComponent>();

		if (PrimativeComponent)
		{	
			PrimativeComponent->OnComponentHit.AddDynamic(this, &UExplodeComponent::OnCollision);
		}
	}

}

void UExplodeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bExplodeAfterTime)
	{
		float T = GetWorld()->GetTimeSeconds();
		if (DetonateStartTime < 1)
		{
			DetonateStartTime = T;
		}

		if (T > DetonateStartTime + TimeToDetonate)
		{
			Destroy();
		}
	}
}

void UExplodeComponent::Hit(int Power)
{
	if (bCanBeDamaged) 
	{
		Health -= Power;
		if (Health < 0)
		{
			Destroy();
		}
	}
}

void UExplodeComponent::Destroy()
{
	if (!bHasSpawned)
	{
		// Ensure that spawn and destory is only ran once
		bHasSpawned = true;
		AActor* Owner = GetOwner();
		auto rot = Owner->GetActorRotation();
		auto loc = Owner->GetActorLocation();

		GetWorld()->SpawnActor(SpawnObject, &loc, &rot);
		GetOwner()->Destroy();
	}
}

int UExplodeComponent::GetHealth() const
{
	return Health;
}

float UExplodeComponent::GetTimeToDetonate() const
{
	return TimeToDetonate;
}

bool UExplodeComponent::GetExplodeOnCollision() const
{
	return bExplodeOnCollision;
}

void UExplodeComponent::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	// If no actor tags set, don't check the tags.
	if (CollisionTags.Num() == 0)
	{
		Destroy();
		return;
	}

	for (FName& CollisionTag : CollisionTags)
	{
		if (OtherActor->ActorHasTag(CollisionTag))
		{
			Destroy();
		}
	}
}
