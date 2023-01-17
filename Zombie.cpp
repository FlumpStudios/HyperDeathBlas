#include "Zombie.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Character.h"

AZombie::AZombie()
{
}

void AZombie::BeginPlay()
{
    Super::BeginPlay();    
}

void AZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AZombie::SetActive(bool Active)
{
    Super::SetActive(Active);
}
