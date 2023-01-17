#include "PlayerControlsComponent.h"
#include <DeadHunt\\Logger.h>
#include "Components/ActorComponent.h"

UPlayerControlsComponent::UPlayerControlsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerControlsComponent::HandleFireGun() 
{
	PlayerState->OnFire();
}

void UPlayerControlsComponent::HandleMeleeAttack()
{
	
	PlayerState->OnMelee();	
}

void UPlayerControlsComponent::HandleStoppedFiring()
{
	
	PlayerState->OnStopFire();	
}

void UPlayerControlsComponent::HandleWeaponChange()
{
	
	PlayerState->OnWeaponChange();
}

void UPlayerControlsComponent::GroundSlam()
{
	
	PlayerState->OnGroundSlam();	
}

void UPlayerControlsComponent::HandleFocus()
{
	PlayerState->OnFocus();
}

void UPlayerControlsComponent::HandleStopFocus()
{
	PlayerState->OnStopFocus();
}

void UPlayerControlsComponent::HandleJump()
{
	PlayerState->OnJump();
}

void UPlayerControlsComponent::HandleFireGranade()
{
	PlayerState->OnFireGranade();
}

void UPlayerControlsComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();	
	
	PlayerState = Owner->FindComponentByClass<UPlayerStateController>();
	
	AssertPlayerState();

	Input = Owner->FindComponentByClass<UInputComponent>();

	if (Input)
	{
		Input->BindAction("Shoot", IE_Pressed, this, &UPlayerControlsComponent::HandleFireGun);
		Input->BindAction("Shoot", IE_Released, this, &UPlayerControlsComponent::HandleStoppedFiring);
		Input->BindAction("Melee", IE_Released, this, &UPlayerControlsComponent::HandleMeleeAttack);
		Input->BindAction("WeaponChange", IE_Pressed, this, &UPlayerControlsComponent::HandleWeaponChange);
		Input->BindAction("Crouch", IE_Pressed, this, &UPlayerControlsComponent::GroundSlam);
		Input->BindAction("Jump", IE_Pressed, this, &UPlayerControlsComponent::HandleJump);
		Input->BindAction("Grenade", IE_Pressed, this, &UPlayerControlsComponent::HandleFireGranade);
		Input->BindAction("Focus", IE_Pressed, this, &UPlayerControlsComponent::HandleFocus);
		Input->BindAction("Focus", IE_Released, this, &UPlayerControlsComponent::HandleStopFocus);
	}
	else
	{
		Logger::LogWarn("Could not find input component on player", GetOwner()->GetName());
	}
}

void UPlayerControlsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPlayerControlsComponent::AssertPlayerState()
{
	if (!PlayerState)
	{
		Logger::LogError("Could not find playerstate component on player", GetOwner()->GetName());
		return false;
	}
	return true;
}
