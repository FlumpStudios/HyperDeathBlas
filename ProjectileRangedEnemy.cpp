// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileRangedEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"

AProjectileRangedEnemy::AProjectileRangedEnemy()
{    
}

void AProjectileRangedEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileRangedEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (GetIsInRangedAttackState())
     {     
         if (!GetIsInRangedWarningState())
         {                
            if(bAutomaticProjectile)
            {
                // If the player is hit and weapon automatic this resets the LastSpawn time to give a delay between enemy being hit and being able to attack again
                if (GetIsInHitState())
                {
                    LastSpawn = GetWorld()->GetTimeSeconds();
                };
                    
                if (GetWorld()->GetTimeSeconds() > LastSpawn + SpawnFrequency)
                {                     
                    LastSpawn = GetWorld()->GetTimeSeconds();
                    SpawnProjectile();
                }
            }
            else
            {
                if (!bHasProjectileBeenFired)
                {                
                    SpawnProjectile();
                    bHasProjectileBeenFired = true;
                }
            }             
         }
     }
     else
     {
         bHasProjectileBeenFired = false;
     }    
}

void AProjectileRangedEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AProjectileRangedEnemy::SpawnProjectile() const
{
    if (!GetIsInHitState() && PlayerActor && Projectile)
    {    
        const int Z_OFFSET = -20;
        FVector Start = GetAttackSpawnPointWithOffset(100.f);
        FRotator Rotation = GetActorRotation();                
        FVector V = UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), PlayerActor->GetActorLocation()) * ProjectileSpeed;
        auto SpawnedProjectile = GetWorld()->SpawnActor(Projectile, &Start, &Rotation);
        if (SpawnedProjectile)
        {        
            auto PrimativeComponent = SpawnedProjectile->FindComponentByClass<UProjectileMovementComponent>();
            if (PrimativeComponent)
            {
                // Need to add a offset as the projectile is aiming a little too high.
                PrimativeComponent->Velocity =  FVector(V.X, V.Y, V.Z + Z_OFFSET);
            }
        }
    }
}
