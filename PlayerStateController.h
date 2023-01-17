// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Materials/Material.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "WeaponConfigComponent.h"
#include "Particles/ParticleSystem.h"
#include <DeadHunt\tags.h>
#include <DeadHunt\GameInstanceMain.h>
#include <DeadHunt\PickupTypes.h>
#include "PlayerStateController.generated.h"

#define KickConfigName "Kick"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEADHUNT_API UPlayerStateController : public UActorComponent
{
	GENERATED_BODY()

public:		
	
	// Sets default values for this component's properties
	UPlayerStateController();

	const float playerGroundedValue = 90.f;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Attack(int Damage);
	
	int GetHealth() const;

	
	// TODO: Move statics to gameinstance and create a reset statics static method
	static int KeysCollected;

	static int EnemiesKilled;

	static int LevelSection;

	static int RemainingEnemiesInSection;

	UFUNCTION()
	float GetCurrentDistanceFromGround() const;

	UFUNCTION()
	float GetCurrentTimeDialation() const;

	UFUNCTION()
	void  SetCurrentTimeDialation(float NewDialation);

	UFUNCTION(BlueprintCallable, Category = "Player ranged attack")
	int GetGrenandeCount() const;
	
	UFUNCTION(BlueprintCallable, Category = "Score")
	void IncreaseScore(int Score);

	bool GetIsInCoolDownState() const;
	void OnWeaponChange();

	UFUNCTION(BlueprintCallable)
	void OnFire();

	UFUNCTION(BlueprintCallable)
	void OnStopFire();

	UFUNCTION(BlueprintCallable)
	void OnMelee();

	UFUNCTION(BlueprintCallable)
	void OnGroundSlam();

	UFUNCTION(BlueprintCallable)
	void OnFocus();

	UFUNCTION(BlueprintCallable)
	void OnStopFocus();

	UFUNCTION(BlueprintCallable)
	void OnJump();

	UFUNCTION(BlueprintCallable)
	void OnFireGranade();

	UFUNCTION(BlueprintCallable)
	void IncreaseGrenadeCount(int Count = 1);

	UFUNCTION(BlueprintCallable)
	bool AlternateWeaponAtMaxAmmo() const;

	UFUNCTION(BlueprintCallable)
	bool CurrentWeaponAtMaxAmmo() const;
	
	UFUNCTION(BlueprintCallable)
	bool AlternateWeaponAtZeroAmmo() const;

	UFUNCTION(BlueprintCallable)
	FWeaponConfiguration GetEquippedWeaponConfig() const;

	UFUNCTION(BlueprintCallable)
	FWeaponConfiguration GetAlternateWeaponConfig() const;

	void ActionPickup(EPickUps Pickuptype);

	// This is the health that is used to display on the hud. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	int DisplayHealth = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DisplayDialation = 100;
	
	UFUNCTION()
	bool GetIsAirborne();

	UFUNCTION()
	bool DoesCurrentWeaponHaveBullets() const;

	UFUNCTION(BlueprintCallable)
	int GetEnemiesRemainingInSectionForDisplay() const;

	UFUNCTION()
	bool GetIsPlayerInFocusState() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// The speed at which the health bar depletes on a hit
	const float HEALTH_BAR_INTERPOLATE_SPEED = 5.f;

	// Should distance from floor debug line show even if debug mode isn't set to full
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bForceShowDistanceFromFloorDebugLine = false;

	// Player health
	UPROPERTY(EditAnywhere, Category = "Health")
	int Health = 100;		
		
	// How long after the player has recieved damage can he be hurt again
	UPROPERTY(EditAnywhere, Category = "On Attacked")
	float HitCoolDown= 0.2f;

	// How fast the hit effect overlay fades after an attack
	UPROPERTY(EditAnywhere, Category = "On Attacked")
	float HitEffectFadeSpeed = 15.f;
	
	// How quickly does the death sprite fade in on death
	UPROPERTY(EditAnywhere, Category = "Death")
	float DeathSpriteSpeed = 200.f;

	// How fast does the players weapon drop down on death
	UPROPERTY(EditAnywhere, Category = "Death")
	float LowerWeaponOnDeathSpeed = 0.5f;

	// How long after the player has died does the level restart
	UPROPERTY(EditAnywhere, Category = "Death")
	float LevelRestartAfterDeathDelay = 1;

	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	UClass* ImpactObject = nullptr;
	
	// The decal to spawn at the end of the ray
	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	UMaterial* DecalMat = nullptr;

	// The blood particles  to spaw
	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	UParticleSystem* BloodParticles = nullptr;

	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	UClass* BulletWound = nullptr;

	// How long after the weapon has been lowered should the weapon be switched at rasied back up
	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	float WeaponSwitchDelay = 0.75f;

	// How long after weapon change can the weapon fire again
	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	float WeaponReactivateDelay = 1.5f;

	// How fast is the weapon switch animation
	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	float WeaponSwitchSpeed = 35.f;
	
	UPROPERTY(EditAnywhere, Category = "Airborne")
	int GrenandeLaunchSpeed = 150000;

	UPROPERTY(EditAnywhere, Category = "Player ranged attack")
	UClass* GranadeObject = nullptr;

	UFUNCTION(Category = "Player ranged attack")
	void SpawnGrenade() const;
	
	// The frames in the melee flipbook that should register and impact
	UPROPERTY(EditAnywhere, Category = "Player melee attack")
	TArray<uint16> MeleeImpactFrames;

	// The gravity to apply when doing a ground smash
	UPROPERTY(EditAnywhere, Category = "Airborne")
	float GroundSmashSpeed = 2500;

	UPROPERTY(EditAnywhere, Category = "Airborne")
	UClass* GroundSmackSpawnObject = nullptr;

	// How long is the small pause before smashing to the ground
	UPROPERTY(EditAnywhere, Category = "Airborne")
	float GroundSmashDelayBeforeSmash = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Airborne")
	float DoubleJumpHeight = 500;

	UFUNCTION(Category = "CallBack")
	void FinshPlaying();

	UFUNCTION(Category = "CallBack")
	void FinshPlayingMeleeAnimation();

	void IncreaseTimeDialation(float amount);

	void SetPlayerToDead();
	

	float LastGroundSmash = 0;

	bool bIsInGroundSmashState = false;
	
	// Utility Functions
	void CastAttackRay(FWeaponConfiguration Config, const FVector& Locale, const  FRotator& Rotation) const;
	void SetPlayerGravity(float Gravity);
	void CastHeightRay();
	void SetupComponents();
	void ActionFireWeapon();
	void ActionMeleeAttack();
	void RunPlayerDeath(float Dt);
	void SetupFlipBookInitialState(UPaperFlipbookComponent* Fb);
	void SetMuzzleOn(bool active);
	void SpawnImpact(const FVector& L, const FRotator& R) const;
	void SpawnBulletHole(const FVector& L, const FRotator& R) const;
	void SpawnBlood(const FVector& L, const FRotator& R) const;
	void SpawnBulletWound(const FVector& L, const FRotator& R) const;
	void SpawnGroundAttack() const;
	void MonoChromeFade(float Target, float FadeSpeed, float Dt);
	void SetHitEffect(float Target, float FadeSpeed, float Dt);
	void FadeInDeathSprite(float FadeSpeed, float Dt);
	void RaiseWeapon(float Speed, float Dt);
	void LowerWeapon(float Speed, float Dt);
	void RestartLevel() const;
	void SlowDownTimeDialation() const;
	void RunSaturationFades(float dt);
	bool AssertWeaponConfig() const;
	bool AssertHasCamera() const;
	bool AssertHasDeathSprite() const;
	bool AssertHasWeaponFlipbook() const;
	void DepleteBullet() const;
	void ActionOutOfAmmo();
	bool bAudioIsSetToOutOfAmmo = false;
	void UpdateWeapon(uint8 configIndex);
	bool IsPrimaryWeaponEquiped() const;
	void AddBulletsToEquipedWeapon();
	void AddBulletsToAlternateWeapon();

	void HandleCollectKey() const;


	// Value Fields
	bool bIsFiring = false;
	bool bMeleeing = false;
	bool bCanFire = true;
	bool bCanMelee = true;
	bool bIsInCoolDownState = false;		
	bool bIsPlayerDead = false;
	float LastAttacked = 0;
	float LastFireTime = 0;
	float HitEffectIntensity = 0;
	float CurrentCameraSaturation = 1;
	float CurrentDeathSpriteAlpha = 0;
	float DeathTimeStamp = 0;	
	float TimeOfLastWeaponSwitch = 0;
	bool bIsInWeaponChangeState = false;
	bool bHasWeaponConfigUpdated = false;
	float CurrentDistanceFromGround = 0;
	int JumpCount = 0;
	int GrenadeCount = 3;
	FVector GetVectorWithSpawnOffset(const FVector& L, const FRotator& R) const;
	void UpdateMotionBlur() const;
	void UpdateFov(float Dt) const;
	bool bIsInFocusState = false;
	
	// The time dialation amount to return to when coming out of focus mode
	float TimeDialationBeforeFocusStart = 0;
	
	// Pointer fields
	UPROPERTY()
	AActor* GunMuzzle = nullptr;

	UPROPERTY()
	UAudioComponent* Audio = nullptr;

	UPROPERTY()
	UPaperFlipbookComponent* GunFlip = nullptr;

	UPROPERTY()
	UPaperFlipbookComponent* MeleeFlip = nullptr;

	UPROPERTY()
	UStaticMeshComponent* MeleeMarker = nullptr;

	UPROPERTY()
	UStaticMeshComponent* WeaponMarker = nullptr;

	UPROPERTY()
	UWeaponConfigComponent* WeaponConfig = nullptr;

	UPROPERTY()
	UCameraComponent* Camera = nullptr;	

	UPROPERTY()
	UPaperSpriteComponent* DeathAnimationSprite = nullptr;	

	UPROPERTY()
	UPaperSpriteComponent* GroundTarget = nullptr;

	UPROPERTY()
	UGameInstanceMain* GameInstance = nullptr;
	
	UPROPERTY()
	ACharacter* Fpc = nullptr;
};