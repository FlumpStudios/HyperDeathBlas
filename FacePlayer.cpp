#include "FacePlayer.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "DeadHunt\Logger.h"
#include "GameFramework/Actor.h"

UFacePlayer::UFacePlayer()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFacePlayer::BeginPlay()
{
	Super::BeginPlay();
}

void UFacePlayer::SetFacingCoords(bool X, bool Y, bool Z)
{			
	bX = X;
	bY = Y;
	bZ = Z;
}

void UFacePlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	auto owner = GetOwner();
	auto world = GetWorld();
	auto player = world->GetFirstPlayerController();

	if (!player)
	{
		Logger::LogWarn("Could not find a player controller when trying to face player", this->GetName());
		return;
	}
	
	OUT FVector Locale;	
	OUT FRotator Rotation;

	world->GetFirstPlayerController()->GetPlayerViewPoint(OUT Locale, OUT Rotation);

	FRotator n = UKismetMathLibrary::FindLookAtRotation(Locale, owner->GetActorLocation());
	FRotator o(
		bX ? n.Pitch : 0, 
		bZ ? n.Yaw + YawOffet : 0,
		bY ? n.Roll : 0);	
	owner->SetActorRotation(o);
}

void UFacePlayer::SetYawOffset(int NewYawOffset)
{
	YawOffet = NewYawOffset;
}
