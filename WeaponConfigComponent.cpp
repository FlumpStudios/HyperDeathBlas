#include "WeaponConfigComponent.h"
#include <DeadHunt\Logger.h>

// Sets default values for this component's properties
UWeaponConfigComponent::UWeaponConfigComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UWeaponConfigComponent::BeginPlay()
{
	Super::BeginPlay();	
	CurrentWeapon = StartingWeaponIndex;
}

void UWeaponConfigComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// NOTE: Avoid using tick
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FWeaponConfiguration UWeaponConfigComponent::GetCurrentWeaponConfiguration() const
{
	return WeaponConfigurations[CurrentWeapon];
}

FWeaponConfiguration UWeaponConfigComponent::GetWeaponConfiguration(uint8 WeaponIndex) const
{
	if (WeaponConfigurations.Num() > WeaponIndex)
	{	
		return WeaponConfigurations[WeaponIndex];
	}

	Logger::LogWarn("Could not find the supplied weapon config index", GetOwner()->GetName());

	return FWeaponConfiguration();
}

FWeaponConfiguration UWeaponConfigComponent::GetWeaponConfiguration(FName Weaponname) const
{
	for (const auto& Config : WeaponConfigurations)
	{
		if (Config.Name == Weaponname)
		{
			return Config;
		};
	}
	
	Logger::LogWarn("Could not find the supplied weapon config name", GetOwner()->GetName());
	return FWeaponConfiguration();
}

uint8 UWeaponConfigComponent::GetCurrentWeaponIndex() const
{
	return CurrentWeapon;
}

void UWeaponConfigComponent::NextWeapon()
{	
	CurrentWeapon++;

	int Count = WeaponConfigurations.Num();

	if (CurrentWeapon >= Count)
	{
		CurrentWeapon = 0;
	}	
	
	if (!WeaponConfigurations[CurrentWeapon].bIsSelectableWeapon)
	{
		NextWeapon();
	}
}

void UWeaponConfigComponent::PreviousWeapon()
{
	CurrentWeapon--;

	int Count = WeaponConfigurations.Num();

	if (CurrentWeapon < 0)
	{
		CurrentWeapon = Count - 1;
	}

	if (!WeaponConfigurations[CurrentWeapon].bIsSelectableWeapon)
	{
		PreviousWeapon();
	}
}

void UWeaponConfigComponent::SetCurrentWeaponConfiguration(uint8 Index)
{
	CurrentWeapon = Index;
}

int UWeaponConfigComponent::GetBulletsRemaining() const
{
	return GetCurrentWeaponConfiguration().BulletCount;
}

int UWeaponConfigComponent::GetMaxBulletsRemaining() const
{
	return GetCurrentWeaponConfiguration().MaxBulletCount;;
}

void UWeaponConfigComponent::AddBullets(int Count, int WeaponIndex, bool bMaxBulletCheck /* =False*/)
{	
	WeaponConfigurations[WeaponIndex].BulletCount += Count;
	
	if (bMaxBulletCheck)
	{
		FWeaponConfiguration C = GetWeaponConfiguration(WeaponIndex);

		if (C.BulletCount > C.MaxBulletCount)
		{
			WeaponConfigurations[WeaponIndex].BulletCount = C.MaxBulletCount;
		}
	}
}

void UWeaponConfigComponent::AddBulletsToCurrentWeapon(int Count, bool bMaxBulletCheck /* =False*/)
{
	int currentWeaponIndex = GetCurrentWeaponIndex();
	AddBullets(Count, currentWeaponIndex, bMaxBulletCheck);
}

void UWeaponConfigComponent::RemoveBullets(int Count, int WeaponIndex)
{
	AddBullets(-WeaponIndex, WeaponIndex);
}
