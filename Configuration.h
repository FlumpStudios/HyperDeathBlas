#pragma once

#include "CoreMinimal.h"


#define EnemyTrace ECollisionChannel::ECC_GameTraceChannel1


// This is where all the static game config stuff will live.
enum class EDebugMode : uint8
{
	Off,
	MessagesOnly,
	Full
};

namespace DebugOptions
{
	const EDebugMode MODE = EDebugMode::MessagesOnly;
}

namespace CollisionPresets
{
	const FName EnemyCollisionPreset = TEXT("Enemy");
}


namespace GameOptions
{
	const float MIN_TIME_DIALATION_BEFORE_DEATH = 0.8f;
	const float MAX_TIME_DIALATION = 3.f;
	const float TIME_DIALATION_INCREASE = 0.04f;
	const float TIME_DECREASE_ON_GROUND_SLAM = 0.05f;
	const float TIME_DIALATION_DANGER_ZONE = 0.9f;
	const float DEATH_FROM_ABOVE_REQUIRED_HEIGHT = 350.f;
	const float POINT_BLANK_DISTANCE = 200;
	const float LONG_SHOT_DISTANCE = 2500;	
	const int OVERKILL_HEALTH_THRESHOLD = -100;
	const int AMMO_INCREASE_PERCENTAGE = 5;
	const int BASE_SPECIAL_KILL_BONUS = 512;
	const int NUMBER_OF_REMAINING_ENEMIES_TO_SHOW_HELP_CHEVRON = 2;
	const float SPEED_THRESHOLD_FOR_FOV_SPEED_EFFECTS = 250.f;
	const float DIALATION_THRESHOLD_FOR_FOV_SPEED_EFFECTS = 1.5f;
	const float FOV_SPEED_EFFECT_LERP_SPEED = 20;
	
	const float FOV_FOCUS_EFFECT_LERP_SPEED = 50;


	// How much to multiply a projectile after the payer has rebounded it
	const int REBOUND_PROJECTILE_MODIFIER = 3;
	
	// Below this amount then kill bonuses will stop counting
	const int MIN_SPECIAL_KILL_BONUS = 64;
	
	// This should be between 0 - 1 as it's a decrement value
	const float SPECIAL_KILL_DECREMENT_MULTIPLIER = 0.5f;

}

UENUM()
enum class EEnemyDeathTypes : uint8
{
	ReboundProjectile,
	PointBlank,
	LongDistance,
	Melee,
	Grenade,
	BarrelExplosion,
	PhysicsHit,
	StunnedKill,
	FriendlyFire,
	DeathFromAbove,
	MaxSpeed,
	DangerZone,
	LastBullet,
	OverKill,
	Focused,
	Critical,
	
	// for cheeky looping, don't actually use
	Last
};

UENUM()
enum class EDamageType : uint8
{
	PlayerMelee,
	PlayerExplosive,
	PlayerBullet,
	PlayerReboundAttack,
	EnemyProjectile,
	EnemyBullet,
	EnemyMelee,
	EnvironmentalExplosion,
	EnvironmentalLaser,
	PhysicsHit
};

