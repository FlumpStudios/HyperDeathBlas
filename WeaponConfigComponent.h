// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "PaperFlipbook.h" 
#include "Components/ActorComponent.h"
#include "Sound/SoundBase.h"
#include "WeaponConfigComponent.generated.h"

USTRUCT(BlueprintType)
struct FWeaponConfiguration
{
	GENERATED_BODY()

	// Name of the weapon
	UPROPERTY(EditAnywhere)
	FName Name;

	// Is the weapon automatic
	UPROPERTY(EditAnywhere)
	bool bIsAuto;

	// How powerful the weapon is
	UPROPERTY(EditAnywhere)
	int Power = 0;

	// The distance the shot ray will cast
	UPROPERTY(EditAnywhere)
	float Distance = 0;

	// The number of rays that are cast
	UPROPERTY(EditAnywhere)
	int Raycount = 0;

	// How accurate the weapon is to the crosshair. 0 = location of the crosshair
	UPROPERTY(EditAnywhere)
	int AccuracyOffset = 0;

	// If is a stun attack, the hit animation on an enemy will last longer
	UPROPERTY(EditAnywhere)
	bool bIsStunAttack = false;

	// The flip book to play when firing the weapon
	UPROPERTY(EditAnywhere)
	UPaperFlipbook* WeaponAnimation = nullptr;	

	// Is the weapon selectable via the weapon cycle
	UPROPERTY(EditAnywhere)
	bool  bIsSelectableWeapon = true;
	
	// The sound effect to play when firing the weapon
	UPROPERTY(EditAnywhere)
	USoundBase* SoundEffect = nullptr;

	UPROPERTY(EditAnywhere)
	USoundBase* NoAmmoSoundEffect = nullptr;

	// How long the muzzle light effect lives
	UPROPERTY(EditAnywhere)
	float MuzzleFlashTime = 0.15f;

	// How much should collision affect a physics objecy
	UPROPERTY(EditAnywhere)
	int PhysicsImpulse = 10000;

	// Current number of bullets in weapon
	UPROPERTY(EditAnywhere)
	int BulletCount = 0;
	
	// Max number of bullets weapon can be loaded with
	UPROPERTY(EditAnywhere)
	int MaxBulletCount = 0;

	UPROPERTY(EditAnywhere)
	bool bCanDeflectEnemyProjectile = false;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UWeaponConfigComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponConfigComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	uint8 GetCurrentWeaponIndex() const;
	
	FWeaponConfiguration GetCurrentWeaponConfiguration() const;
	FWeaponConfiguration GetWeaponConfiguration(uint8 Index) const;
	FWeaponConfiguration GetWeaponConfiguration(FName Weaponname) const;	
	void SetCurrentWeaponConfiguration(uint8 Index);
	void NextWeapon();
	void PreviousWeapon();
	
	UFUNCTION(BlueprintCallable)
	int GetBulletsRemaining() const;

	UFUNCTION(BlueprintCallable)
	int GetMaxBulletsRemaining() const;

	UFUNCTION()
	void AddBullets(int Count, int WeaponIndex, bool bMaxBulletCheck = false);
	
	UFUNCTION()
	void AddBulletsToCurrentWeapon(int Count, bool bMaxBulletCheck = false);

	UFUNCTION()
	void RemoveBullets(int Count, int WeaponIndex);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	uint8 CurrentWeapon = 0;

	// What weapon should the player start the game with
	UPROPERTY(EditAnywhere)
	uint8 StartingWeaponIndex = 0;

	UPROPERTY(EditAnywhere)
	TArray<FWeaponConfiguration> WeaponConfigurations;
};
