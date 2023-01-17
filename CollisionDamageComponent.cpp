#include "CollisionDamageComponent.h"
#include "GameFramework/Actor.h"
#include <DeadHunt\PlayerStateController.h>	
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include <DeadHunt\Logger.h>
#include <DeadHunt\EnemyBase.h>
#include "GameFramework/ProjectileMovementComponent.h"
#include <DeadHunt\ExplodeComponent.h>

UCollisionDamageComponent::UCollisionDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}	

void UCollisionDamageComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* Owner = GetOwner();
	if (bUseSpecificCollisionComponent)
	{
		TSet<UActorComponent*> AllComponents = Owner->GetComponents();
		for (const auto& Comp : AllComponents)
		{
			if (Comp->GetName() == CollisionComponentName)
			{
				CollisionComponent = Cast<UPrimitiveComponent>(Comp);
				if (!CollisionComponent)
				{
					Logger::LogWarn("CollisionComponentName was specified but was unable to cast to a Primative component", this->GetName());
				}
				continue;
			}
		}
	}
	else
	{
		CollisionComponent = Owner->FindComponentByClass<UPrimitiveComponent>();
	}

	if (CollisionComponent)
	{	
		if (RegisterOverlapResults)
		{
			CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UCollisionDamageComponent::OnOverLap);
			CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &UCollisionDamageComponent::OnOverLapEnd);
			CollisionComponent->SetGenerateOverlapEvents(true);
		}

		if (RegisterHitResults)
		{			
			CollisionComponent->OnComponentHit.AddDynamic(this, &UCollisionDamageComponent::OnCollision);
		}
	}
	else
	{
		Logger::LogWarn("Collision damage component could not find a primative component", *Owner->GetName());
	}
}

void UCollisionDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bRunContinuousDamageOnOverlap)
	{
		for (const auto& OverlappedActor: OverlappedActorList)
		{      
			if (OverlappedActor.Value)
			{			
				DamageEntity(OverlappedActor.Value);
			}
			else
			{
				Logger::LogWarn("OverlappedActorList contains a pointer to a null AActor", this->GetName());
			}
		}
	}
}

void UCollisionDamageComponent::OnCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	RunOnCollisionDetected(OtherActor);
}

void UCollisionDamageComponent::OnOverLapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	OverlappedActorList.Remove(OtherActor->GetName());
}

void UCollisionDamageComponent::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bRunContinuousDamageOnOverlap)
	{
		OverlappedActorList.Add(OtherActor->GetName(),OtherActor);
	}
	else
	{
		RunOnCollisionDetected(OtherActor);
	}
}

void UCollisionDamageComponent::DamageEntity(AActor* Actor)
{	
	if (!Actor)
	{
		return;
	}

	if (Actor == GetOwner())
	{
		return;
	}

	if (Actor->ActorHasTag(Tags::PLAYER))
	{
		auto C = Actor->FindComponentByClass<UPlayerStateController>();
		if (C)
		{
			C->Attack(Power);
		}
	}

	if (Actor->ActorHasTag(Tags::ENEMY))
	{	
		auto E = Cast<AEnemyBase>(Actor);
		if (E)
		{
			E->AttackEnemy(
				Power,
				bShouldStunEnemy ? true : false,
				DamageType,
				bShouldForceEnemyBack ? true : false);
		}
	};

	if (Actor->ActorHasTag(Tags::PHYSICS_PROJECTILE))
	{	
		UPrimitiveComponent* f = Actor->FindComponentByClass<UPrimitiveComponent>();

		if (f) 
		{ 			
			FVector Impulse = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), Actor->GetActorLocation()) * PhysicsForceToApply;			
			Impulse.Z += 50000;

			f->AddImpulse(Impulse);
		}
	};

	if (Actor->ActorHasTag(Tags::EXPLODABLE))
	{		 
		auto C = Actor->FindComponentByClass<UExplodeComponent>();
		if (C)
		{
			C->Hit(Power);
		}
		else
		{
			Logger::LogWarn("Actor hit by CollisionDamageComponent, but no explode component was found on", Actor->GetName());
		}
	};

	// TODO: Doesn't seem to deflect enemy projectile...need to fix
	if (Actor->ActorHasTag(Tags::ENEMY_PROJECTILE))
	{
		UProjectileMovementComponent* Projectile = Actor->FindComponentByClass<UProjectileMovementComponent>();

		if (Projectile)
		{
			const int DEFLECT_SPEED = 5000;
			FVector Impulse = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), Actor->GetActorLocation()) * DEFLECT_SPEED;
			Projectile->Velocity = Impulse;
		}
		else
		{
			Logger::LogWarn("Actor hit by CollisionDamageComponent, but no explode component was found on", Actor->GetName());
		}
	};
}

void UCollisionDamageComponent::SetCollisionType(EDamageType NewDamageType)
{
	DamageType = NewDamageType;
}

void UCollisionDamageComponent::RunOnCollisionDetected(AActor* Actor)
{
	DamageEntity(Actor);

	if (DestoryOnHit)
	{
		if (ObjectToSpawnOnDestroy)
		{		
			FVector Start = GetOwner()->GetActorLocation();
			FRotator Rot = GetOwner()->GetActorRotation();
			GetWorld()->SpawnActor(ObjectToSpawnOnDestroy, &Start, &Rot);
		}
		
		if(Actor)
		{ 
			UPrimitiveComponent* PrimativeComponent = Actor->FindComponentByClass<UPrimitiveComponent>();
			if (PrimativeComponent)
			{		
				ECollisionChannel CollisionObjectType = PrimativeComponent->GetCollisionObjectType();
				
				// World dyanmic should be resereved for things like trigger boxes, so we don't wanna destroy stuff in world dyanmic collision objects
				if (CollisionObjectType != ECollisionChannel::ECC_WorldDynamic)
				{			
					GetOwner()->Destroy();
				}
			}
		}
	}
}

void UCollisionDamageComponent::SetDamage(int Damage)
{
	Power = Damage;
}

int UCollisionDamageComponent::GetDamage() const
{
	return Power;;
}
