#include "PhysicsProjectile.h"
#include <DeadHunt\EnemyBase.h>
#include "GameFramework/Actor.h"
#include <DeadHunt\Tags.h>
#include <DeadHunt\Configuration.h>
#include <DeadHunt\ExplodeComponent.h>

APhysicsProjectile::APhysicsProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("static mesh component"));
	SetRootComponent(StaticMeshComponent);		
	this->Tags.Add(FName(Tags::PHYSICS_PROJECTILE));
}

void APhysicsProjectile::BeginPlay()
{
	Super::BeginPlay();	
	
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetSimulatePhysics(true);
		StaticMeshComponent->OnComponentHit.AddDynamic(this, &APhysicsProjectile::OnCollision);
	}
}

void APhysicsProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APhysicsProjectile::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto g = this->GetVelocity().GetAbs();
	if ((g.X + g.Y + g.Z) > VelocityDamageThreshold)
	{
		if (this->ActorHasTag(Tags::EXPLODABLE))
		{
			auto C = this->FindComponentByClass<UExplodeComponent>();
			if (C && C->GetExplodeOnCollision())

			{
				C->Hit(Power);
			}
		}

		if (OtherActor && OtherActor->ActorHasTag(Tags::ENEMY))
		{	
			auto E = Cast<AEnemyBase>(OtherActor);
			if (E)
			{
				E->AttackEnemy(Power, bShouldStunEnemy, EDamageType::PhysicsHit, false);
			}
		}
	}
}

