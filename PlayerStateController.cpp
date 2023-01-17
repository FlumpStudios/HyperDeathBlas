#include "PlayerStateController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/InputComponent.h"
#include <DeadHunt\BasicAnchor.h>
#include <DeadHunt\CollisionDamageComponent.h>
#include "Engine/World.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/DecalActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/WorldPSCPool.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"
#include <DeadHunt\ExplodeComponent.h>
#include <DeadHunt\Logger.h>
#include <DeadHunt\Configuration.h>
#include "DrawDebugHelpers.h"
#include "FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include <DeadHunt\Projectile.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "EnemyBase.h"

const int  NO_BULLET_WEAPON = 0;

const int STARTING_LEVEL_SECTION = 1;

int UPlayerStateController::KeysCollected = 0;

int UPlayerStateController::LevelSection = STARTING_LEVEL_SECTION;

int UPlayerStateController::EnemiesKilled = 0;

int UPlayerStateController::RemainingEnemiesInSection = 0;


UPlayerStateController::UPlayerStateController()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	// Reset statics on player spawn
	UPlayerStateController::KeysCollected = 0;
	UPlayerStateController::EnemiesKilled = 0;
	UPlayerStateController::LevelSection = STARTING_LEVEL_SECTION;
	UPlayerStateController::RemainingEnemiesInSection = 0;
}

void UPlayerStateController::BeginPlay()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	Super::BeginPlay();
	GetOwner()->Tags.Add(FName(Tags::PLAYER));
	SetupComponents();
}

void UPlayerStateController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (GameInstance->MotionBlurAmount > 0)
	{	
		UpdateMotionBlur();
	}

	if (GameInstance->FovOnMoveAmount > 0)
	{
		UpdateFov(DeltaTime);
	}

	auto World = GetWorld();

	if (bIsInGroundSmashState)
	{	
		if (World->GetTimeSeconds() < LastGroundSmash + GroundSmashDelayBeforeSmash)
		{			
			Fpc->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
		}
		else
		{
			Fpc->GetCharacterMovement()->Velocity = FVector(0, 0, -GroundSmashSpeed);
		}
	}

	CastHeightRay();	
	
	if (GetIsAirborne())
	{
		GroundTarget->SetVisibility(true);
	}
	else
	{
		if (bIsInGroundSmashState)
		{
			// Ground slams should decrease health/game speed, but not if time dialation is under 1
			if (GetCurrentTimeDialation() >= 1)
			{
				IncreaseTimeDialation(-GameOptions::TIME_DECREASE_ON_GROUND_SLAM);
			};

			SpawnGroundAttack();
			bIsInGroundSmashState = false;
		}

		JumpCount = 0;
		GroundTarget->SetVisibility(false);		
		if (!bIsPlayerDead)
		{		
			GetOwner()->EnableInput(GetWorld()->GetFirstPlayerController());
		}
	}

	float Dialation = GetCurrentTimeDialation();
		
	GameInstance->LevelTime += (DeltaTime * 1000) / Dialation;

	DisplayHealth = FMath::FInterpConstantTo(DisplayHealth, Health, DeltaTime, HEALTH_BAR_INTERPOLATE_SPEED);		
	
	DisplayDialation = (Dialation - GameOptions::MIN_TIME_DIALATION_BEFORE_DEATH) / (GameOptions::MAX_TIME_DIALATION - GameOptions::MIN_TIME_DIALATION_BEFORE_DEATH);

	RunSaturationFades(DeltaTime);

	if (HitEffectIntensity > 0)
	{
		if (bIsPlayerDead)
		{
			SetHitEffect(0, HitEffectFadeSpeed * 3, DeltaTime);
		}
		else
		{
			SetHitEffect(0, HitEffectFadeSpeed, DeltaTime);
		}
	}
	

	if (bIsInCoolDownState && World->GetTimeSeconds() > LastAttacked + HitCoolDown)
	{
		bIsInCoolDownState = false;
	}

	if (bIsPlayerDead)
	{
		RunPlayerDeath(DeltaTime);
	}

	// For single frame actions on player death
	if (!bIsPlayerDead && Dialation < GameOptions::MIN_TIME_DIALATION_BEFORE_DEATH)
	{
		SetPlayerToDead();
	}	

	// This is here to give the gun longer to riase, so it doesn't lower the weapon on each weapon change
	if (World->GetTimeSeconds() > TimeOfLastWeaponSwitch + (WeaponSwitchDelay * 2))
	{
		RaiseWeapon(WeaponSwitchSpeed, DeltaTime);		
	}


	// TODO : Extract this out into a function so it can be called on level start
	if (bIsInWeaponChangeState)
	{
		if (World->GetTimeSeconds() > TimeOfLastWeaponSwitch + WeaponReactivateDelay)
		{
			bIsInWeaponChangeState = false;
			bHasWeaponConfigUpdated = false;
			return;
		}

		if (World->GetTimeSeconds() > TimeOfLastWeaponSwitch + WeaponSwitchDelay)
		{	
			if (!bHasWeaponConfigUpdated)
			{
				auto P = GameInstance->CurrentLoadOut;
				
				if (IsPrimaryWeaponEquiped())
				{
					UpdateWeapon(P.SecondaryWeaponIndex);
				}
				else
				{
					UpdateWeapon(P.PrimaryWeaponIndex);
				}
				
				bHasWeaponConfigUpdated = true;
			}
		}
		else
		{
			LowerWeapon(WeaponSwitchSpeed, DeltaTime);
		}
	}

	bool bHasBullets = DoesCurrentWeaponHaveBullets();
	
	// Weapon ammo count check is performed on the onFire callback function
	if (bCanFire && bIsFiring && !bIsInWeaponChangeState && bHasBullets)
	{				
		DepleteBullet();
		ActionFireWeapon();
	}
	// play a sounds when the weapon runs out of ammo, need this for auto weapons
	if (bCanFire && bIsFiring && !bHasBullets && !bAudioIsSetToOutOfAmmo)
	{
		if (AlternateWeaponAtZeroAmmo())
		{
			ActionOutOfAmmo();
		}
		else
		{
			// Change name of function
			OnWeaponChange();
		}
	}
	

	if (bCanMelee && bMeleeing)
	{
		ActionMeleeAttack();
	}

	if (World->GetTimeSeconds() > LastFireTime + WeaponConfig->GetCurrentWeaponConfiguration().MuzzleFlashTime)
	{
		SetMuzzleOn(false);
	}	
}

void UPlayerStateController::SetupComponents()
{
	AActor* Owner = GetOwner();	

	Camera = Owner->FindComponentByClass<UCameraComponent>();
	DeathAnimationSprite = Owner->FindComponentByClass<UPaperSpriteComponent>();

	TArray<UPaperSpriteComponent*> Sprites;
	GetOwner()->GetComponents<UPaperSpriteComponent>(Sprites);

	for (UPaperSpriteComponent* Sprite : Sprites)
	{
		if (Sprite->GetName() == TEXT("LandingLocation"))
		{
			GroundTarget = Sprite;
		}
	}

	TArray<UStaticMeshComponent*> Meshes;
	GetOwner()->GetComponents<UStaticMeshComponent>(Meshes);

	const FString MeleePositionName = TEXT("MeleePosition");
	const FString WeaponPositionName = TEXT("WeaponPosition");
	for (UStaticMeshComponent* Mesh : Meshes)
	{
		if (Mesh->GetName() == MeleePositionName)
		{
			MeleeMarker = Mesh;
		}
		if (Mesh->GetName() == WeaponPositionName)
		{
			WeaponMarker = Mesh;
		}
	};

	if (!MeleeMarker)
	{
		Logger::LogWarn("Could not find Melee marker mesh on player", GetOwner()->GetName());
	}

	UWeaponConfigComponent* C = Owner->FindComponentByClass<UWeaponConfigComponent>();

	if (C)
	{
		WeaponConfig = C;
	}
	else
	{
		Logger::LogWarn("Could not find weapon config component", GetOwner()->GetName());
	}

	TArray<UPaperFlipbookComponent*> flipbooks;
	Owner->GetComponents<UPaperFlipbookComponent>(flipbooks);

	const FString GunFbName = TEXT("Gun_FB");
	const FString KickFbName = TEXT("Kick_FB");
	if (flipbooks.Num() > 0)
	{
		for (UPaperFlipbookComponent* flipbook : flipbooks)
		{
			if (flipbook->GetName() == GunFbName)
			{
				GunFlip = flipbook;
			}
			if (flipbook->GetName() == KickFbName)
			{
				MeleeFlip = flipbook;
			}
		}
	}
	else
	{
		Logger::LogWarn("Could not find any required flipbook components on player", GetOwner()->GetName());
	}

	SetupFlipBookInitialState(GunFlip);

	// UFUNCTION doesn't allow pointers to functions, so can't add this to the flipbook set up method
	GunFlip->OnFinishedPlaying.AddDynamic(this, &UPlayerStateController::FinshPlaying);

	SetupFlipBookInitialState(MeleeFlip);
	MeleeFlip->OnFinishedPlaying.AddDynamic(this, &UPlayerStateController::FinshPlayingMeleeAnimation);

	TArray<AActor*> Lights;
	Owner->GetAllChildActors(Lights);

	if (Lights.Num() > 0)
	{
		for (const auto &Light : Lights)
		{		
			if (Light->GetName() == TEXT("BP_MuzzleFlash_C_0"))
			{
				GunMuzzle = Light;
			}
		}
	}
	else
	{
		Logger::LogWarn("Could not find any required lights components on player", GetOwner()->GetName());
	}

	if (GunMuzzle)
	{
		SetMuzzleOn(false);
	}

	Audio = Owner->FindComponentByClass<UAudioComponent>();
	Audio->SetAutoActivate(false);

	GameInstance = Cast<UGameInstanceMain>(GetWorld()->GetGameInstance());

	if (GameInstance)
	{	
		UpdateWeapon(GameInstance->CurrentLoadOut.PrimaryWeaponIndex);
		
		// Reset the special kill score table when the player spawns		
		GameInstance->ResetSpecialKillScoreTable();
	}
	else
	{
		Logger::LogWarn("Could not cast game instance", GetOwner()->GetName());
	}

	Fpc = Cast<ACharacter>(GetOwner());
}

void UPlayerStateController::SetPlayerToDead()
{
	DeathTimeStamp = GetWorld()->GetTimeSeconds();
	bIsPlayerDead = true;
}

int UPlayerStateController::GetGrenandeCount() const
{
	return GrenadeCount;
}

bool UPlayerStateController::GetIsPlayerInFocusState() const
{
	return bIsInFocusState;
}


bool UPlayerStateController::GetIsAirborne()
{
	return CurrentDistanceFromGround > playerGroundedValue + 20;
}

void UPlayerStateController::RunPlayerDeath(float Dt)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	
	GetOwner()->DisableInput(PlayerController);
	MeleeFlip->SetVisibility(false);

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2);	
	FadeInDeathSprite(DeathSpriteSpeed, Dt);
	LowerWeapon(LowerWeaponOnDeathSpeed, Dt);

	if (GetWorld()->GetTimeSeconds() > DeathTimeStamp + LevelRestartAfterDeathDelay)
	{
		RestartLevel();
	}
}

void UPlayerStateController::UpdateWeapon(uint8 configIndex)
{
	WeaponConfig->SetCurrentWeaponConfiguration(configIndex);
	auto Config = WeaponConfig->GetCurrentWeaponConfiguration();
	GunFlip->SetFlipbook(Config.WeaponAnimation);
	bAudioIsSetToOutOfAmmo = false;
	if (DoesCurrentWeaponHaveBullets())
	{
		Audio->SetSound(Config.SoundEffect);
	}
}

void UPlayerStateController::SetupFlipBookInitialState(UPaperFlipbookComponent* Fb)
{
	Fb->Stop();
	Fb->SetLooping(false);
	Fb->SetPlaybackPositionInFrames(1, false);
}

void UPlayerStateController::SetMuzzleOn(bool active)
{
	if (GunMuzzle)
	{
		if (active)
		{
			GunMuzzle->SetActorRelativeLocation(FVector(0, 0, 0));
		}
		else
		{
			GunMuzzle->SetActorRelativeLocation(FVector(1000, -1000, 1000));
		}
		// auto f = GunMuzzle->setlocaio
	}
}

// Call back for when gun animation has finished
void UPlayerStateController::FinshPlaying()
{
	if (GunFlip)
	{
		GunFlip->SetPlaybackPositionInFrames(1, false);
		bCanFire = true;
	}
}

void UPlayerStateController::OnFireGranade()
{	
	if (GrenadeCount > 0)
	{
		GrenadeCount--;
		SpawnGrenade();		
	}
}

void UPlayerStateController::FinshPlayingMeleeAnimation()
{
	if (MeleeFlip)
	{
		MeleeFlip->SetPlaybackPositionInFrames(1, false);
		bCanMelee = true;
	}
}

void UPlayerStateController::ActionFireWeapon()
{
	if (!AssertWeaponConfig()) { return; }
	FRotator Rotation;
	FVector Locale;
	auto world = GetWorld();
	auto character = world->GetFirstPlayerController();
	character->GetPlayerViewPoint(OUT Locale, OUT Rotation);
	auto CurrentWeaponConfig = WeaponConfig->GetCurrentWeaponConfiguration();

	CastAttackRay(CurrentWeaponConfig, Locale, Rotation);

	LastFireTime = GetWorld()->GetTimeSeconds();
	
	if (CurrentWeaponConfig.MuzzleFlashTime > 0.01f)
	{	
		SetMuzzleOn(true);
	}
	GunFlip->Play();
	bCanFire = false;

	if (!Audio->IsActive())
	{
		Audio->SetActive(true);
	}
	Audio->Play();
}

void UPlayerStateController::ActionOutOfAmmo()
{
	if (!bAudioIsSetToOutOfAmmo)
	{
		bAudioIsSetToOutOfAmmo = true;
		Audio->SetSound(WeaponConfig->GetCurrentWeaponConfiguration().NoAmmoSoundEffect);
	}
	Audio->Play();
}

void UPlayerStateController::ActionMeleeAttack()
{
	if (!AssertWeaponConfig()) { return; }
	int currentFrame = MeleeFlip->GetPlaybackPositionInFrames();
	for (const uint16& ImpactFrame : MeleeImpactFrames)
	{
		if (ImpactFrame == currentFrame)
		{
			auto L = MeleeMarker->GetComponentLocation();
			auto R = MeleeMarker->GetComponentRotation();

			CastAttackRay(WeaponConfig->GetWeaponConfiguration(KickConfigName), L, R);

			// If we hit the enemy, we don't want to hit them again in the same animation
			bCanMelee = false;
		}
	}
}

void UPlayerStateController::OnFire()
{
	if (DoesCurrentWeaponHaveBullets())
	{
		if (bCanFire)
		{
			bIsFiring = true;
		}
	}
	else
	{
		ActionOutOfAmmo();
	}
}

void UPlayerStateController::OnMelee()
{
	if (bCanMelee)
	{
		MeleeFlip->Play();
		bMeleeing = true;
	}
}

void UPlayerStateController::OnStopFire()
{
	bIsFiring = false;
}

void UPlayerStateController::OnGroundSlam()
{	
	if (GetIsAirborne())
	{
		if (!bIsInGroundSmashState)
		{	
			LastGroundSmash = GetWorld()->GetTimeSeconds();
			bIsInGroundSmashState = true;
		}
	}
}

void UPlayerStateController::OnStopFocus()
{
	if (bIsInFocusState)
	{	
		bIsInFocusState = false;
		SetCurrentTimeDialation(TimeDialationBeforeFocusStart);
	}
}

void UPlayerStateController::OnFocus()
{
	TimeDialationBeforeFocusStart = GetCurrentTimeDialation();
	SetCurrentTimeDialation(GameOptions::MIN_TIME_DIALATION_BEFORE_DEATH);
	bIsInFocusState = true;
}


void UPlayerStateController::OnJump()
{
	if (GetIsAirborne())
	{
		if (!bIsInGroundSmashState && JumpCount < 1)
		{
			FVector Impulse = FVector(0, 0, DoubleJumpHeight * GetCurrentTimeDialation());
			Fpc->LaunchCharacter(Impulse, false, false);
			JumpCount ++;
		}
	}
}

void UPlayerStateController::ActionPickup(EPickUps Pickuptype)
{		
	switch (Pickuptype)
	{
		case Speed:						
			IncreaseTimeDialation(GameOptions::TIME_DIALATION_INCREASE);			
			break;
		case Bullet:
			AddBulletsToAlternateWeapon();
			AddBulletsToEquipedWeapon();
			break;
		case Key:
			HandleCollectKey();
			break;
	}
}

void UPlayerStateController::HandleCollectKey() const
{
	UPlayerStateController::LevelSection++;
	UPlayerStateController::KeysCollected++;
}

float UPlayerStateController::GetCurrentTimeDialation() const
{
	return UGameplayStatics::GetGlobalTimeDilation(GetWorld());
}

void UPlayerStateController::SetCurrentTimeDialation(float NewDialation)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewDialation);
}

void UPlayerStateController::IncreaseTimeDialation(float Amount)
{
	float currentGameSpeed = GetCurrentTimeDialation();

	if (bIsInFocusState && currentGameSpeed < GameOptions::MAX_TIME_DIALATION)
	{	
		TimeDialationBeforeFocusStart += Amount;
	}

	if (currentGameSpeed < GameOptions::MAX_TIME_DIALATION)
	{
		float IncAmount = currentGameSpeed + Amount;
		if (IncAmount > GameOptions::MAX_TIME_DIALATION)
		{
			IncAmount = GameOptions::MAX_TIME_DIALATION;
		}
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), IncAmount);
	};
}

void UPlayerStateController::CastHeightRay()
{
	auto World = GetWorld();

	FVector Locale = GetOwner()->GetActorLocation();
	auto Rotation = FRotator(270, 0, 0);

	FVector LineTraceEnd = Locale + Rotation.Vector() * 1000;

	if (bForceShowDistanceFromFloorDebugLine || DebugOptions::MODE == EDebugMode::Full)
	{
		DrawDebugLine(
			World,
			Locale,
			LineTraceEnd,
			FColor(255, 255, 255),
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

	World->LineTraceSingleByObjectType(
		hit,
		Locale,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic),
		r);

	AActor* hitActor = hit.GetActor();	
	
	FVector I = hit.ImpactPoint;

	// Raise slightly above the floor to avoid Z fighting
	GroundTarget->SetWorldLocation(FVector(I.X, I.Y, I.Z + 5));		
	
	float s = hit.Distance / 650;	
	CurrentDistanceFromGround = hit.Distance;
	
	if (s > 0.1 && s < 100)
	{	
		GroundTarget->SetWorldScale3D(FVector(s, s, s));
	}
}

void UPlayerStateController::CastAttackRay(FWeaponConfiguration Config, const FVector& Locale, const  FRotator& Rotation) const
{
	// TODO: This should probably go somwewhere else.
	const int DEFLECT_SPEED = 5000;

	FVector LineTraceEnd = Locale + Rotation.Vector() * Config.Distance;
	auto World = GetWorld();

	for (int i = 0; i < Config.Raycount; i++)
	{
		// Creates offsets for line trace end so gun isn't 100% accurate. :)
		FVector OffSetRay;

		int A = Config.AccuracyOffset;
		if (A)
		{
			OffSetRay = FVector(
				LineTraceEnd.X + FMath::RandRange(-A, A),
				LineTraceEnd.Y + FMath::RandRange(-A, A),
				LineTraceEnd.Z + FMath::RandRange(-A, A));
		}
		else
		{
			OffSetRay = LineTraceEnd;
		}		

		FHitResult hit;

		FCollisionQueryParams r(
			FName(TEXT("")),
			false,
			GetOwner());

		World->LineTraceSingleByChannel(	
			hit,
			Locale,
			OffSetRay,
			EnemyTrace,
			r);
		auto rot = UKismetMathLibrary::MakeRotFromX(hit.Normal);
		AActor *hitActor = hit.GetActor();

		if (hitActor)
		{			
			if (hitActor->ActorHasTag(Tags::ENEMY))
			{
				bool bCriticalHit = false;
				UPrimitiveComponent* HitComponent = hit.GetComponent();
				if (HitComponent && HitComponent->ComponentHasTag(Tags::CRITICAL_HIT))
				{
					bCriticalHit = true;					
				}

				AEnemyBase *enemy = Cast<AEnemyBase>(hitActor);

				enemy->AttackEnemy(
					Config.Power, 
					Config.bIsStunAttack, 
					Config.Name == KickConfigName ? EDamageType::PlayerMelee : EDamageType::PlayerBullet,
					true, 
					hit.Distance, 
					bCriticalHit);
				
				if (enemy->GetShouldSpawnBlood())
				{
					SpawnBlood(hit.ImpactPoint, rot);
				}

				SpawnBulletWound(hit.ImpactPoint, rot);
			}
			else if (hitActor->ActorHasTag(Tags::PHYSICS_PROJECTILE))
			{			
				auto PrimitiveComponent = hitActor->FindComponentByClass<UPrimitiveComponent>();
				
				if (!PrimitiveComponent) { return; }
				FVector Impulse = GetOwner()->GetActorRotation().Vector() * Config.PhysicsImpulse;

				// Push the object into the air for a nicer effect
				Impulse.Z += 10000;

				PrimitiveComponent->AddImpulse(Impulse);

				SpawnImpact(hit.ImpactPoint, rot);
			}
			else if (Config.bCanDeflectEnemyProjectile && hitActor->ActorHasTag(Tags::ENEMY_PROJECTILE))
			{
				UProjectileMovementComponent* Projectile = hitActor->FindComponentByClass<UProjectileMovementComponent>();
				if (Projectile)
				{
						FVector V = GetOwner()->GetActorRotation().Vector() * DEFLECT_SPEED;
						Projectile->Velocity = V;
				}
				else
				{
					Logger::LogWarn("Could not find projectile component when trying to run a rebound attacl", this->GetName());
				}
				
				UCollisionDamageComponent* CollisionComponent = hitActor->FindComponentByClass<UCollisionDamageComponent>();
				if (CollisionComponent)
				{
					CollisionComponent->SetCollisionType(EDamageType::PlayerReboundAttack);					
					int currentDamage = CollisionComponent->GetDamage();
					CollisionComponent->SetDamage(currentDamage * GameOptions::REBOUND_PROJECTILE_MODIFIER);
				}
				else
				{
					Logger::LogWarn("ENEMY_PROJECTILE found on collision actor but could not find UCollisionDamageComponent", this->GetName());
				}
			}
			else
			{
				SpawnImpact(hit.ImpactPoint, rot);
				SpawnBulletHole(hit.ImpactPoint, rot);
			}

			// Don't want kicking someting to make it explode
			if (Config.Name != KickConfigName && hitActor->ActorHasTag(Tags::EXPLODABLE))
			{
				hitActor->FindComponentByClass<UExplodeComponent>()->Hit(Config.Power);
			}
		}
	}	
}

void UPlayerStateController::RestartLevel() const
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void UPlayerStateController::RunSaturationFades(float Dt)
{
	auto Dialation = GetCurrentTimeDialation();

	if (!bIsPlayerDead)
	{
		if (Dialation < GameOptions::TIME_DIALATION_DANGER_ZONE)
		{
			if (!FMath::IsNearlyZero(CurrentCameraSaturation))
			{
				MonoChromeFade(0.15f, 2 / Dialation, Dt);
			}
		}
		else
		{
			if (CurrentCameraSaturation < 1)
			{
				MonoChromeFade(1, 2 / Dialation, Dt);
			}
		}
	}
	else
	{
		MonoChromeFade(0, 2 / Dialation, Dt);
	}	
}

void UPlayerStateController::SetHitEffect(float Target, float FadeSpeed, float Dt)
{
	if(AssertHasCamera())
	{ 	
		Camera->PostProcessSettings.BloomDirtMaskIntensity = HitEffectIntensity;
		HitEffectIntensity = FMath::FInterpConstantTo(HitEffectIntensity, Target, Dt, FadeSpeed);
	}
}

void UPlayerStateController::MonoChromeFade(float Target, float FadeSpeed, float Dt)
{	
	if (AssertHasCamera())
	{
		CurrentCameraSaturation = FMath::FInterpConstantTo(CurrentCameraSaturation, Target, Dt, FadeSpeed);
		Camera->PostProcessSettings.ColorSaturation = FVector4(CurrentCameraSaturation, CurrentCameraSaturation, CurrentCameraSaturation);
	}
}

void UPlayerStateController::FadeInDeathSprite(float FadeSpeed, float Dt)
{
	if (AssertHasDeathSprite())
	{		
		DeathAnimationSprite->SetSpriteColor(FLinearColor(CurrentDeathSpriteAlpha, CurrentDeathSpriteAlpha, CurrentDeathSpriteAlpha, CurrentDeathSpriteAlpha));
		CurrentDeathSpriteAlpha = FMath::FInterpTo(CurrentDeathSpriteAlpha, 1, Dt, FadeSpeed);
	}
}

void UPlayerStateController::LowerWeapon(float Speed, float Dt)
{
	if (AssertHasWeaponFlipbook())
	{
		auto WeaponDropTargetLocation = GetOwner()->FindComponentByClass<UBasicAnchor>()->GetComponentTransform().GetLocation();
		auto PlayerActorLocation = GunFlip->GetComponentTransform().GetLocation();
		
		PlayerActorLocation.X = FMath::FInterpConstantTo(PlayerActorLocation.X, WeaponDropTargetLocation.X, Dt, Speed);
		PlayerActorLocation.Y = FMath::FInterpConstantTo(PlayerActorLocation.Y, WeaponDropTargetLocation.Y, Dt, Speed);
		PlayerActorLocation.Z = FMath::FInterpConstantTo(PlayerActorLocation.Z, WeaponDropTargetLocation.Z, Dt, Speed);

		GunFlip->SetWorldLocation(FVector(PlayerActorLocation.X, PlayerActorLocation.Y, PlayerActorLocation.Z));
	}
}

void UPlayerStateController::RaiseWeapon(float Speed, float Dt)
{
	if (AssertHasWeaponFlipbook())
	{
		auto WeaponTargetLocation = WeaponMarker->GetComponentTransform().GetLocation();
		auto PlayerActorLocation = GunFlip->GetComponentTransform().GetLocation();

		PlayerActorLocation.X = FMath::FInterpConstantTo(PlayerActorLocation.X, WeaponTargetLocation.X, Dt, Speed);
		PlayerActorLocation.Y = FMath::FInterpConstantTo(PlayerActorLocation.Y, WeaponTargetLocation.Y, Dt, Speed);
		PlayerActorLocation.Z = FMath::FInterpConstantTo(PlayerActorLocation.Z, WeaponTargetLocation.Z, Dt, Speed);

		GunFlip->SetWorldLocation(FVector(PlayerActorLocation.X, PlayerActorLocation.Y, PlayerActorLocation.Z));
	}
}

void UPlayerStateController::Attack(int Damage)
{
	if(!bIsInCoolDownState && !bIsPlayerDead)
	{
		if (bIsInFocusState)
		{		
			SetPlayerToDead();
		}
		else
		{
			HitEffectIntensity += Damage;
			Health -= Damage;
			LastAttacked = GetWorld()->GetTimeSeconds();
			bIsInCoolDownState = true;
			SlowDownTimeDialation();
		}
	}
}

void UPlayerStateController::SlowDownTimeDialation() const
{
	auto currentGameSpeed = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	auto NewSpeed = currentGameSpeed - 0.2f;

	if (currentGameSpeed > GameOptions::MIN_TIME_DIALATION_BEFORE_DEATH)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NewSpeed);
	}

	if (currentGameSpeed < GameOptions::MIN_TIME_DIALATION_BEFORE_DEATH)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), GameOptions::MIN_TIME_DIALATION_BEFORE_DEATH);
	}
}

void UPlayerStateController::OnWeaponChange()
{	
	if (!bIsInWeaponChangeState)
	{	
		bIsInWeaponChangeState = true;
		TimeOfLastWeaponSwitch = GetWorld()->GetTimeSeconds();			
	}
}

bool UPlayerStateController::GetIsInCoolDownState() const
{
	return bIsInCoolDownState;
}

int UPlayerStateController::GetHealth() const
{
	return Health;
}

void UPlayerStateController::IncreaseScore(int Score)
{
	GameInstance->CurrentScore += Score;
}

void UPlayerStateController::SpawnImpact(const FVector& L, const FRotator& R) const
{
	if (ImpactObject)
	{
		GetWorld()->SpawnActor(ImpactObject, &L, &R);
	}
}

void UPlayerStateController::SpawnGroundAttack() const
{
	if (GroundSmackSpawnObject)
	{
		AActor* O = GetOwner();
		FVector Pl = O->GetActorLocation();
		FVector L = FVector(Pl.X, Pl.Y, (Pl.Z - CurrentDistanceFromGround));
		FRotator R = FRotator(0,0,0);
		GetWorld()->SpawnActor(GroundSmackSpawnObject, &L, &R);
	}
}

void UPlayerStateController::SpawnGrenade() const
{
	if (GranadeObject)
	{
		const int FORWARD_SPAWN_OFFSET = 350;
		const float UP_FORCE_OFFSET = 0.30;

		UCameraComponent* CamComponent = GetOwner()->FindComponentByClass<UCameraComponent>();

		if (!CamComponent) { return; }
		FVector Pl = CamComponent->GetComponentLocation();
		FRotator Pr = CamComponent->GetComponentRotation();
		FVector Sl = Pl + Pr.Vector() * FORWARD_SPAWN_OFFSET;
		FVector RotationUnitVector = Pr.Vector();

		// For some reason the launch speed is effected by the time game speed
		auto LaunchSpeed = GrenandeLaunchSpeed / GetCurrentTimeDialation();

		FVector ForceVec = Pl + (FVector(RotationUnitVector.X, RotationUnitVector.Y, RotationUnitVector.Z + UP_FORCE_OFFSET)) * LaunchSpeed;

		AActor* SpawnedActor = GetWorld()->SpawnActor(GranadeObject, &Sl, &Pr);

		auto LiveGrenande = SpawnedActor->FindComponentByClass<UPrimitiveComponent>();

		if (LiveGrenande)
		{
			LiveGrenande->AddForce(ForceVec,NAME_None,true);
		}
		else
		{
			Logger::LogWarn("Can't find the primative component on the spawned grenade", GetOwner()->GetName());
		}
	}
	else
	{	
		Logger::LogError("No grenade object found on player blueprint", GetOwner()->GetName());
	}
}

void UPlayerStateController::SpawnBulletWound(const FVector& L, const FRotator& R) const
{
	if (BulletWound)
	{
		auto G =  GetVectorWithSpawnOffset(L, R);
		GetWorld()->SpawnActor(BulletWound, &G, &R);
	}
}

void UPlayerStateController::SpawnBulletHole(const FVector& L, const FRotator& R) const
{
	ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(L, FRotator(R.Pitch + 90, R.Yaw, R.Roll));
	if (decal)
	{
		decal->SetDecalMaterial(DecalMat);
		decal->SetLifeSpan(5.0f);
		decal->GetDecal()->DecalSize = FVector(5.0f, 5.0f, 5.0f);
	}
	else
	{
		Logger::LogWarn("No decal actor found when attempting to spawn a bullect impact", GetOwner()->GetName());
	}
}

void UPlayerStateController::SpawnBlood(const FVector& L, const FRotator& R) const
{	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticles, GetVectorWithSpawnOffset(L, R), R, true, EPSCPoolMethod::AutoRelease, true);
}

void UPlayerStateController::SetPlayerGravity(float Gravity)
{
	Fpc->GetCharacterMovement()->GravityScale = Gravity;
}

FVector UPlayerStateController::GetVectorWithSpawnOffset(const FVector& L, const FRotator& R) const
{
	// This is how far a hit object will spawn in front of an ememy sprite
	const float RenderOffsetOnEnemyHit = 1.75f;
	return L + R.Vector() * RenderOffsetOnEnemyHit;
}

void UPlayerStateController::DepleteBullet() const
{
	if (WeaponConfig->GetBulletsRemaining() > 0)
	{
		int Index = WeaponConfig->GetCurrentWeaponIndex();
		WeaponConfig->RemoveBullets(1, Index);
	}
}

bool UPlayerStateController::DoesCurrentWeaponHaveBullets() const
{
	auto Config = WeaponConfig->GetCurrentWeaponConfiguration();
	
	return Config.MaxBulletCount == NO_BULLET_WEAPON || Config.BulletCount > 0;
}

bool UPlayerStateController::IsPrimaryWeaponEquiped() const
{
	auto P = GameInstance->CurrentLoadOut;
	auto C = WeaponConfig->GetCurrentWeaponIndex();

	if (P.PrimaryWeaponIndex == C)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UPlayerStateController::AddBulletsToAlternateWeapon()
{
	
	auto P = GameInstance->CurrentLoadOut;
	FWeaponConfiguration Config;
	int Index = 0;
	if (IsPrimaryWeaponEquiped())
	{	
		Index = P.SecondaryWeaponIndex;
		Config = WeaponConfig->GetWeaponConfiguration(Index);
	}
	else
	{
		Index = P.PrimaryWeaponIndex;
		Config = WeaponConfig->GetWeaponConfiguration(Index);
	}

	int addCount = Config.MaxBulletCount / GameOptions::AMMO_INCREASE_PERCENTAGE;
	WeaponConfig->AddBullets(addCount, Index, true);
}

void UPlayerStateController::AddBulletsToEquipedWeapon()
{
	FWeaponConfiguration Config = WeaponConfig->GetCurrentWeaponConfiguration();
	int addCount = Config.MaxBulletCount / GameOptions::AMMO_INCREASE_PERCENTAGE;
	WeaponConfig->AddBulletsToCurrentWeapon(addCount, true);
}


bool UPlayerStateController::CurrentWeaponAtMaxAmmo() const
{
	auto Config = GetEquippedWeaponConfig();
	if (Config.BulletCount >= Config.MaxBulletCount)
	{
		return true;
	}

	return false;
}

bool UPlayerStateController::AlternateWeaponAtMaxAmmo() const
{
	auto Config = GetAlternateWeaponConfig();
	if (Config.BulletCount >= Config.MaxBulletCount)
	{
		return true;
	}
	
	return false;
}

bool UPlayerStateController::AlternateWeaponAtZeroAmmo() const
{
	auto Config = GetAlternateWeaponConfig();
	if (Config.BulletCount <= 0)
	{
		return true;
	}

	return false;
}

FWeaponConfiguration UPlayerStateController::GetEquippedWeaponConfig() const
{
	auto P = GameInstance->CurrentLoadOut;
	FWeaponConfiguration Config;
	if (IsPrimaryWeaponEquiped())
	{
		Config = WeaponConfig->GetWeaponConfiguration(P.PrimaryWeaponIndex);
	}
	else
	{
		Config = WeaponConfig->GetWeaponConfiguration(P.SecondaryWeaponIndex);
	}

	return Config;
}

FWeaponConfiguration UPlayerStateController::GetAlternateWeaponConfig() const
{
	auto P = GameInstance->CurrentLoadOut;
	FWeaponConfiguration Config;
	if (IsPrimaryWeaponEquiped())
	{
		Config = WeaponConfig->GetWeaponConfiguration(P.SecondaryWeaponIndex);
	}
	else
	{
		Config = WeaponConfig->GetWeaponConfiguration(P.PrimaryWeaponIndex);
	}

	return Config;
}

void UPlayerStateController::IncreaseGrenadeCount(int Count /* 1 */)
{
	GrenadeCount += Count;
}

float UPlayerStateController::GetCurrentDistanceFromGround() const
{
	return CurrentDistanceFromGround;
}

int UPlayerStateController::GetEnemiesRemainingInSectionForDisplay() const
{
	return RemainingEnemiesInSection;
}

void UPlayerStateController::UpdateMotionBlur() const
{
	float MotionBlurAmount = GameInstance->MotionBlurAmount;	
	Camera->PostProcessSettings.MotionBlurAmount = (GetCurrentTimeDialation() - 1) * MotionBlurAmount;
}

void UPlayerStateController::UpdateFov(float Dt) const
{
	UMovementComponent* Motion = GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
	if (Motion)
	{
		FVector V = Motion->Velocity;
		float Dialation = GetCurrentTimeDialation();
		float AbsoluteSpeed = abs(V.X) + abs(V.Y);

		if (!bIsInFocusState 
			&& AbsoluteSpeed > GameOptions::SPEED_THRESHOLD_FOR_FOV_SPEED_EFFECTS 
			&& Dialation > GameOptions::DIALATION_THRESHOLD_FOR_FOV_SPEED_EFFECTS)
		{
			float Target = GameInstance->BaseFov - (Dialation * GameInstance->FovOnMoveAmount);
			float NewFov = FMath::FInterpConstantTo(Camera->FieldOfView, Target, Dt, GameOptions::FOV_SPEED_EFFECT_LERP_SPEED);
			Camera->SetFieldOfView(NewFov);
		}
		else if(bIsInFocusState)
		{
			float Target = GameInstance->BaseFov - (Dialation * GameInstance->FovOnFocus);
			float NewFov = FMath::FInterpConstantTo(Camera->FieldOfView, Target, Dt, GameOptions::FOV_FOCUS_EFFECT_LERP_SPEED);
			Camera->SetFieldOfView(NewFov);
		}
		else
		{
			Camera->SetFieldOfView(GameInstance->BaseFov);
		}
	}
}

bool UPlayerStateController::AssertWeaponConfig() const
{
	if (!WeaponConfig)
	{
		Logger::LogWarn("No weapon configuration has been found", GetOwner()->GetName());
		return false;
	}

	return true;
}

bool UPlayerStateController::AssertHasCamera() const
{
	if (!Camera)
	{
		Logger::LogWarn("No camera attached", GetOwner()->GetName());
		return false;
	}
	return true;
}

bool UPlayerStateController::AssertHasDeathSprite() const
{
	if (!DeathAnimationSprite)
	{
		Logger::LogWarn("No player death sprite attached", GetOwner()->GetName());
		return false;
	}
	return true;
}

bool UPlayerStateController::AssertHasWeaponFlipbook() const
{
	if (!GunFlip)
	{
		Logger::LogWarn("No weapon flipbook attached", GetOwner()->GetName());
		return false;
	}
	return true;
}

