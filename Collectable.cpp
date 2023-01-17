#include "Collectable.h"
#include "Engine/World.h"
#include <DeadHunt\Configuration.h>
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACollectable::ACollectable()
{
 	PrimaryActorTick.bCanEverTick = true;	
	
	Mesh = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere mesh component"));	
	SetRootComponent(Mesh);
				
	MovementProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile component"));		
}

void ACollectable::BeginPlay()
{
	Super::BeginPlay();

	if (bUsePhysics)
	{	
		Mesh->SetSimulatePhysics(true);
	}

	SpawnTime = GetWorld()->GetTimeSeconds();

	this->Tags.Add(FName(Tags::PICKUP));
	MainSprite = this->FindComponentByClass<UPaperSpriteComponent>();

	PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{	
		PlayerActor = PlayerController->GetPawn();
		if (!PlayerActor) { return; }
		
		PlayerState = PlayerActor->FindComponentByClass<UPlayerStateController>();
		
	}

	if (MovementProjectileComponent)
	{
		MovementProjectileComponent->bIsHomingProjectile = true;
		MovementProjectileComponent->HomingAccelerationMagnitude = 10000;
		MovementProjectileComponent->HomingTargetComponent = PlayerActor->GetRootComponent();
		MovementProjectileComponent->SetActive(false);
		MovementProjectileComponent->ProjectileGravityScale = 0;
	}

	if (Mesh && bUsePhysics)
	{	
		if (!PlayerController) { return; }
		OUT FVector PlayerLocale;
		OUT FRotator Rotation;

		PlayerController->GetPlayerViewPoint(OUT PlayerLocale, OUT Rotation);		
		auto Impulse = Rotation.Vector() * 21000;
		Impulse.Z = FMath::RandRange(6000, 8000);
		Impulse.Y += FMath::RandRange(-21000, 21000);
		Impulse.X += FMath::RandRange(-21000, 21000);
		Mesh->AddImpulse(Impulse);
	}
}

void ACollectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	
	// If a sprite is found, make it face the player
	if (MainSprite)
	{	
		SetSpriteLookAtPlayer();
	}

	int D = GetDistanceFromPlayer();
	
	if (D < ActionPickUpDistance && bIsReadyToCollect)
	{	
		PlayerState->ActionPickup(PickUpType);
		this->Destroy();
	}

	if (D > 0 && D < AttachDistance)
	{
		if (GetWorld()->GetTimeSeconds() > SpawnTime + DelayBeforeRunAttach)
		{		
			bIsReadyToCollect = true;
			
			if (Mesh->IsSimulatingPhysics())
			{
				Mesh->SetSimulatePhysics(false);
			}
		
			if (!MovementProjectileComponent->IsActive())
			{		
			
				MovementProjectileComponent->SetActive(true);
				Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}
}

int ACollectable::GetDistanceFromPlayer() const
{
	auto world = GetWorld();
	auto Locale = this->GetActorLocation();
	auto LineTraceEnd = PlayerActor->GetActorLocation();

	if (bForceShowDebugLine || DebugOptions::MODE == EDebugMode::Full)
	{
		DrawDebugLine(
			world,
			Locale,
			LineTraceEnd,
			FColor(0, 0, 255),
			false,
			0.f,
			0,
			5.f);
	}

	FHitResult hit;

	FCollisionQueryParams r(
		FName(TEXT("")),
		false,
		GetOwner());

	world->LineTraceSingleByObjectType(
		hit,
		Locale,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		r);

	AActor* hitActor = hit.GetActor();
	if (hitActor)
	{
		if (hitActor->ActorHasTag(Tags::PLAYER))
		{
			return hit.Distance;
		}
	}
	return 0;
}

void ACollectable::SetSpriteLookAtPlayer() const
{	
	OUT FVector PlayerLocale;
	OUT FRotator Rotation;	
	if (!PlayerController) { return; }
	
	PlayerController->GetPlayerViewPoint(OUT PlayerLocale, OUT Rotation);
	
	FRotator n = UKismetMathLibrary::FindLookAtRotation(PlayerLocale, this->GetActorLocation());
	FRotator o(
		n.Pitch,
		n.Yaw + 90,
		n.Roll);
	MainSprite->SetWorldRotation(o);
}

