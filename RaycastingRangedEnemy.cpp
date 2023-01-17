#include "RaycastingRangedEnemy.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include <DeadHunt\Configuration.h>
#include <DeadHunt\EnemyBase.h>
#include "Engine/World.h"

ARaycastingRangedEnemy::ARaycastingRangedEnemy()
{
}

void ARaycastingRangedEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void ARaycastingRangedEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    bool IsInWarningState = GetIsInRangedWarningState();

    if (IsInWarningState)
    {
        // We need to capture the ray so it points to the position the player was in when the warning frame started...so we don't just follow the player around and never miss
        if (!bHasRayBeenCaptured)
        {
            CapturedRayEndpoint = PlayerActor->GetActorLocation();
            bHasRayBeenCaptured = true;
        }
    }
    else if (GetIsInRangedAttackState())
    {        
        if (!bRayHasHitSomething && !GetIsInHitState())
        {   
           RayCast(CapturedRayEndpoint); 
        }
    }
    else
    {
        bRayHasHitSomething = false;
        bHasRayBeenCaptured = false;
    }    

    // If the enemy is touching the player then fire away!
    if (bRunRangedAttackOnTouch && GetPlayerEnemyIsTouching())
    {
        RunRangeAttack();
    }
}

void ARaycastingRangedEnemy::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector ARaycastingRangedEnemy::GetLineTraceEnd()
{
    FRotator ro = GetActorRotation();
    FRotator Rotation = FRotator(ro.Pitch, ro.Yaw + 90, ro.Roll);

    return GetLocaleWithZoffset() + Rotation.Vector() * WeaponRange;
}

void ARaycastingRangedEnemy::RayCast(FVector &LineTraceEnd)
{
    auto world = GetWorld();
    auto owner = GetOwner();
    
    // Need to move slightly in front of enemy so the their ray tracing doesn't hit them first.
    auto Locale = GetAttackSpawnPointWithOffset();

    if (bForceShowRangedWeaponDebugLine || DebugOptions::MODE == EDebugMode::Full)
    {
        DrawDebugLine(
            world,
            Locale,
            LineTraceEnd,
            FColor(0, 255, 0),
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
        FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECollisionChannel::ECC_Pawn) | ECC_TO_BITFIELD(ECollisionChannel::ECC_WorldStatic)),
        r);

    AActor *hitActor = hit.GetActor();
    if (hitActor)
    {
        if (hitActor->ActorHasTag(Tags::PLAYER))
        {        
            RunAttackOnPlayer(hitActor, WeaponPower);
        }

        // Failsafe to make sure the enemy doesn't accidentally shoot themselves
        if (hitActor == this) { return; }

        if (hitActor->ActorHasTag(Tags::ENEMY))
        {
            auto E = Cast<AEnemyBase>(hitActor);
            if (E)
            {
                E->AttackEnemy(
                    WeaponPower, 
                    false, 
                    EDamageType::EnemyBullet,
                    false);
            }
        }
        bRayHasHitSomething = true;
    }
}
