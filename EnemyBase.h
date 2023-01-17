// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "PlayerStateController.h"
#include "GameFramework/Actor.h"
#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "PaperFlipbook.h"
#include <DeadHunt/CriticalHitPointComponent.h>
#include "PaperFlipbookComponent.h"
#include <DeadHunt\Configuration.h>
#include <DeadHunt\SpawnableText.h>
#include "FacePlayer.h"
#include "Components/TextRenderComponent.h"
#include <DeadHunt\tags.h>
#include "EnemyBase.generated.h"

USTRUCT(BlueprintType)
struct FMinMax
{
		GENERATED_BODY()
		UPROPERTY(EditAnywhere)
		int Min;

		UPROPERTY(EditAnywhere)
		int Max;
};

UCLASS(Abstract)
class DEADHUNT_API AEnemyBase : public APawn
{
	GENERATED_BODY()

public:
	AEnemyBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
	virtual void SetActive(bool Active);
	void AttackEnemy(int Attackstrength, bool bIsStunAttack, EDamageType DamageType, bool bRunForceBack = true, float DistanceFromPlayer = -1, bool bCriticalHit = false);
	void AddEnemyDeathSpawnMessage(EEnemyDeathTypes MessageType);

	// Getters
	bool GetIsDead() const;		
	bool GetIsInHitState() const;
	bool GetIsinMeleeState() const;
	bool GetIsInRangedWarningState() const;
	bool GetIsInRangedAttackState() const;
	bool GetHasLineOfSightOfPlayer() const;
	bool GetHasLineOfSightOfAnotherEnemy() const;
	int GetHealth() const;
	FRotator GetActorRotationWithYawAdjustment() const;
	AActor* GetPlayerEnemyIsTouching() const;
	bool GetShouldSpawnBlood() const;

protected:
	virtual void BeginPlay() override;	
	virtual FVector GetAttackSpawnPointWithOffset(float ForwardOffset = 20.f) const;
	virtual FVector GetLineOfSightPointWithOffset(float ForwardOffset = 20.f) const;

	void RunMelee(AActor* actor);
	void RunRangeAttack();		
	void SetLastRangedAttackStartTime();
	void RunAttackOnPlayer(const AActor* HitActor, int AttackStrength);
	void RandomPatrol();
	
	UPROPERTY()
	AActor* PlayerActor = nullptr;

	UPROPERTY()
	UPlayerStateController* PlayerStateController = nullptr;
	
	float LastRangedAttackStart = 0;
	FVector GetLocaleWithZoffset(float Offset = 100) const;

	UPROPERTY(Category = "Ranged Attack", EditAnywhere)
	uint32 bHasrangedAttack : 1;

	UPROPERTY(Category = "Melee Attack", EditAnywhere)
	uint32 bHasMeleeAttack : 1;		
	
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementProjectileComponent = nullptr;
	
	UFUNCTION()
	bool GetIsInStunState() const;

private:	
	// Player Health
	UPROPERTY(Category = "Health", EditAnywhere)
	int Health = 1;
	
	// How much should the attack be multiplied by if a critical hit detected
	UPROPERTY(Category = "Health", EditAnywhere, meta = (EditCondition = "bHasCriticalHitArea"))
	int CriticalHitModifier = 1;
	
	// How often the enemy will perform a ranged attack
	UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
	float RangedAttackFrequency = 8;		

	// Should Ranged attack be based on random timings
	UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
	bool bRandomiseFrequency = false;
	
	UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
	FMinMax RandomisedAttackFrequencyRange;	

	// How long the enemy will perform a ranged attack for
	UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
	float RangedAttackLength = 1;

	// How long the enemy will perform a ranged attack for
	UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
	float RangedAttackHeight = 100;

	// How long the warning flipbook frame should display before performing a ranged attack
	UPROPERTY(Category = "Ranged Attack", EditAnywhere, meta = (EditCondition = "bHasrangedAttack"))
	float RangedWarningBeforeFireDelay = 0.5f;
	
	// How far the enemy can reach for a melee atatck
	UPROPERTY(Category = "Melee Attack", EditAnywhere, meta = (EditCondition = "bHasMeleeAttack"))
	int MeleeReach = 75;

	// How powerful is the enemy mell attack
	UPROPERTY(Category = "Melee Attack", EditAnywhere, meta = (EditCondition = "bHasMeleeAttack"))
	int MeleeStrength = 50;
	
	// How long after a melee attack can the enemy perform the next melee
	UPROPERTY(Category = "Melee Attack", EditAnywhere, meta = (EditCondition = "bHasMeleeAttack"))
	float MeleeCooldown = 1;

	// Which frames of the flipbook animation wil register as an attack
	UPROPERTY(Category = "Melee Attack", EditAnywhere, meta = (EditCondition = "bHasMeleeAttack"))
	TArray<uint16> AttackFrames;

	// Should the melee debug ray be displayed even if debug isn't set to full
	UPROPERTY(Category = "Debug", EditAnywhere)
	bool bForceShowMeleeDebugLine = false;
	
	// Should the enemy object be destoryed on death rather than play an animation
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	bool DestroyOnKilled = false;

	// Animation to play when enemy is dying
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	UPaperFlipbook *DeathAnimation = nullptr;

	// Alternative death animation to play when and enemy is dying
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	UPaperFlipbook* CriticalDeathAnimation = nullptr;

	
	// Animation to play when enemy has been hit
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	UPaperFlipbook *HitAnimation = nullptr;

	// Animation to play when enemy is performing a melee attacl
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	UPaperFlipbook *MeleeAnimation = nullptr;	

	// Animation to play when enemy is performing a ranged attacl
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	UPaperFlipbook *RangeAttackAnimation = nullptr;
	
	// How long after the enemy has taken a hit can he return to his normal state
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	float HitAnimationDelay = 0.25f;	

	// How long after the enemy has taken a stun hit can he return to his normal state
	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	float StunDelay = 1;

	UPROPERTY(Category = "Flipbook Animations", EditAnywhere)
	class UPaperFlipbookComponent* BaseFlipBook = nullptr;

	UPROPERTY()
	class UPaperFlipbookComponent* ChevronFlipBook = nullptr;

	// Should line of sight debug line show even if debug mode isn't set to full
	UPROPERTY(Category = "Debug", EditAnywhere)
	bool bForceLineOfSightDebugLine = false;

	// What section of the level does the enemy belong to
	UPROPERTY(Category = "Behavior", EditAnywhere)
	int LevelSection = 1;

	// How far can the enemy see
	UPROPERTY(Category = "Behavior", EditAnywhere)
	float lineOfSightLength = 10000;

	UPROPERTY(Category = "Behavior", EditAnywhere)
	float lineOfSightHeight = 100;

	// The length of time the enemy will strafe if it has an enemy in its line of site
	UPROPERTY(Category = "Behavior", EditAnywhere)
	float strafeLengthIfTargetingEnemy = 0.5f;

	// The speed at which the enemy will try and side step an enemy if another enemy is in line of sight.
	UPROPERTY(Category = "Behavior", EditAnywhere)
	int strafeSpeedIfTargetingEnemy = 50;

	// How fast the enemy should patrol if enemy not in line of sight
	UPROPERTY(Category = "Behavior", EditAnywhere)
	int PatrolSpeed = 50;

	// The amount the game will speed up after killing an enemy
	UPROPERTY(Category = "Behavior", EditAnywhere)
	float TimeDialationIncreaseAmont = 0.1f;

	UPROPERTY(Category = "Pickups", EditAnywhere)
	UClass* TimePickUpItem = nullptr;

	UPROPERTY(Category = "Pickups", EditAnywhere)
	int TimePickUpCount = 3;

	UPROPERTY(Category = "Pickups", EditAnywhere)
	UClass* BulletPickUpItem = nullptr;

	// A blue print to spawn if the death is caused by a critical hit. This will spawn at the point of the Critical hit component.	
	UPROPERTY(Category = "On Enemy Death", EditAnywhere)
	UClass* DeathTextRenderer = nullptr;

	UPROPERTY(Category = "On Enemy Death", EditAnywhere)
	UClass* DeathTextScoreRenderer = nullptr;

	UPROPERTY(Category = "On Enemy Death", EditAnywhere)
	bool bShouldSpawnBlood = true;

	UPROPERTY(Category = "On Enemy Death", EditAnywhere)
	UClass* CriticalDeathSpawnBlueprint = nullptr;

	UPROPERTY(Category = "On Enemy Death", EditAnywhere)
	float GibsSpawnHeight = 100;

	// Gib effect to spawn if the enemy is blown up. Leave empty if enemy shouldn't ever spawn gib effects
	UPROPERTY(Category = "OnEnemyDeath", EditAnywhere)
	UClass* Gibs = nullptr;	

	// Gib effect to spawn if the enemy is blown up. Leave empty if enemy shouldn't ever spawn gib effects
	UPROPERTY(Category = "OnEnemyDeath", EditAnywhere)
	int GibsThreshold = -50;

	// The delay between the spawning of death messages if more than one message is to be spawned
	UPROPERTY(Category = "OnEnemyDeath", EditAnywhere)
	float DelayBetweenDeathMessages = 0.65f;

	UPROPERTY(Category = "Pickups", EditAnywhere)
	int BulletPickUpCount = 3;
	
	// The player score
	UPROPERTY(Category = "Score", EditAnywhere)
	int Worth = 1000;

	UPROPERTY(Category = "Projectile", EditAnywhere)
	float RandomSpeedModifier = 0;

	UPROPERTY()
	UPaperFlipbook *WalkingAnimation = nullptr;
	
	UPROPERTY()
	AActor* CurrentLineOfSightActor = nullptr;
	
	UPROPERTY(EditAnywhere)
	UFacePlayer* FacePlayer;

	UPROPERTY()
	UGameInstanceMain* GameInstance = nullptr;
		
	UPROPERTY()
	UCriticalHitPointComponent* CriticalHitPointComponent = nullptr;

	float CurrentHitDelay = 0;

	float LastMeleeStart = 0;

	bool bIsInRangedAttackState = false;
	
	bool bIsInRangedWarningState = false;

	bool bIsInMeleeState = false;

	bool bIsInForceBackState = false;

	bool bRunningHitAnimation = false;

	bool bToggleRunForceBack = false;
	
	float HitAnimationTime = 0;

	float LastStraffTime = 0;
	
	bool bIsInStraffState = false;

	bool bHasSeenPlayer = false;

	bool bForceBackImpulseHasBeenApplied = false;

	bool bIsInStunState = false;

	FVector EnemyForceBackEndPosition;
	
	bool isStraffLeft = false;

	bool bIsDead = false;

	void SetFlipbookToOriginalAnimation();	

	void SetFlipbookToRangedAttackAnimation();

	void Kill(bool bCritical);

	bool CheckLineOfSight(const FName &Tag) const;

	void RunForceBack(float Dt);

	void RunStraff();

	void RandomiseRangeAttackFrequency();

	void RunDeathCleanup(bool bCritical);

	void UpdateLineOfSightActor();	

	void SpawnTimeItems() const;

	void SpawnBulletPickups() const;
	
	void SpawnDeathText(const FString& Text, float SpawnDelay, int SpecialKillScore) const;
	
	float TimeOfLastRandomPatrolCheckPoint = 0;
	
	bool bIsInRandomPatrolState = false;

	bool bHasSpawnedBullets = false;

	void AddDeathMessages(float DistanceFromPlayer, EDamageType DamageType, bool bCriticalHit);
	
	void SetHelperChevronVisibility(bool bIsVisible) const;
	
	TSet<EEnemyDeathTypes> MessageSpawnList = {};
};