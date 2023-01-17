#include "Projectile.h"
#include "Components/SphereComponent.h"
#include <DeadHunt\ProjectileRangedEnemy.h>
#include <DeadHunt\PlayerStateController.h>
#include <DeadHunt\Logger.h>
#include <DeadHunt\EnemyBase.h>
#include <DeadHunt\Configuration.h>
#include <DeadHunt\Tags.h>
#include "GameFramework/Actor.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;	
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile component"));
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere collision component"));
	SetRootComponent(SphereCollision);
	this->Tags.Add(Tags::ENEMY_PROJECTILE);
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();	

	if (SphereCollision)
	{	
		SphereCollision->OnComponentHit.AddDynamic(this, &AProjectile::OnCollision);
	}
	else
	{
		Logger::LogWarn("No sphere collision component found on projectile", GetOwner()->GetName());		
	}
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{	
	if (OtherActor->IsA(AProjectile::StaticClass()))
	{
		return;
	}
	
	if (OtherActor->ActorHasTag(Tags::PLAYER))
	{
		UPlayerStateController* C = OtherActor->FindComponentByClass<UPlayerStateController>();
		if (C)
		{
			C->Attack(Power);
		}
	}

	if (OtherActor->ActorHasTag(Tags::ENEMY))
	{
		auto E = Cast<AEnemyBase>(OtherActor);
		if (E)
		{
			E->AttackEnemy(
				Power, 
				false,
				HasDeflected ?  EDamageType::PlayerReboundAttack : EDamageType::EnemyProjectile,
				HasDeflected ? true : false);
		}
	}

	
	// We don't wanna destroy 
	auto Primative = OtherActor->FindComponentByClass<UPrimitiveComponent>();
	if (Primative)
	{
		ECollisionChannel CollisionObjectType = Primative->GetCollisionObjectType();
		if (CollisionObjectType == !ECollisionChannel::ECC_WorldDynamic)
		{
			this->Destroy();
		}
	}
}

void AProjectile::Deflect(const FVector& Velocity)
{
	ProjectileComponent->Velocity = Velocity;
	HasDeflected = true;
}


void AProjectile::SetVelocity(FVector& Speed) const
{
	ProjectileComponent->SetVelocityInLocalSpace(Speed);
}