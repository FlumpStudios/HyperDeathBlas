#include "EnemyBase.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "FacePlayer.h"
#include "PaperFlipbook.h"
#include "UObject/UObjectGlobals.h"
#include <DeadHunt\ToggleEnabledAfterTime.h>
#include "DrawDebugHelpers.h"
#include "PaperFlipbookComponent.h"
#include <DeadHunt\GameInstanceMain.h>
#include <DeadHunt\Logger.h>
#include "Kismet/GameplayStatics.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	FacePlayer = CreateDefaultSubobject<UFacePlayer>(TEXT("Face player component"));
	MovementProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("projectile component to control enemy movements"));
	BaseFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Root Flipbook component"));
	SetRootComponent(BaseFlipBook);
	this->Tags.Add(FName(Tags::ENEMY));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	MovementProjectileComponent->bIsHomingProjectile = true;
	
	MovementProjectileComponent->HomingAccelerationMagnitude = 5000;	

	if (LevelSection < 1)
	{
		Logger::LogError("Enemy has not been assigned a level section", this->GetName());
	}

	CriticalHitPointComponent = FindComponentByClass<UCriticalHitPointComponent>();

	if (RandomSpeedModifier > 0)
	{
		float Rnd = FMath::RandRange(-RandomSpeedModifier, RandomSpeedModifier);
		MovementProjectileComponent->MaxSpeed += Rnd;
		MovementProjectileComponent->InitialSpeed +=  Rnd;
	}

	
	PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (PlayerActor)
	{
		MovementProjectileComponent->HomingTargetComponent = PlayerActor->GetRootComponent();
		PlayerStateController = PlayerActor->FindComponentByClass<UPlayerStateController>();
		if (!PlayerStateController)
		{		
			Logger::LogWarn("Couldn't find player state controller", this->GetName());
		}
	}
	else
	{
		Logger::LogWarn("Couldn't find player actor", this->GetName());
	}
	
	// save the original flipbook so we can return to it later, BaseFlipBook will be switching flipbooks all over the shop.
	WalkingAnimation = BaseFlipBook->GetFlipbook();
	int FrameCount = BaseFlipBook->GetFlipbookLengthInFrames();	
	BaseFlipBook->SetPlaybackPositionInFrames(FMath::RandRange(1, FrameCount), false);

	if (bRandomiseFrequency)
	{
		RandomiseRangeAttackFrequency();
	}

	if (FacePlayer)
	{
		FacePlayer->SetFacingCoords(false, false, true);
	}
	else
	{
		Logger::LogWarn("faceplayer component not found on enemy", this->GetName());
	}

	GameInstance = Cast<UGameInstanceMain>(GetWorld()->GetGameInstance());
	if (!GameInstance) 
	{
		Logger::LogWarn("Unable to cast the found game instance to UGameInstanceMain", this->GetName());
	}

	auto Components = this->GetComponents();

	const FString VISIBILITY_HELPER_NAME = TEXT("LocationChevron");
		 
	// HACK: We look for the chevton flipbook by name as the creating default sub was not making the flipbook a child of the base flipbook correctly
	for (const auto& Component : Components)
	{
		if (Component->GetName() == VISIBILITY_HELPER_NAME)
		{			
			ChevronFlipBook = Cast<UPaperFlipbookComponent>(Component);
			return;
		}	
	}

	if (ChevronFlipBook)
	{
		SetHelperChevronVisibility(false);
	}
	else
	{
		Logger::LogError("Could not find visibilty helper flipbook on enemy. Flipbook component must be called " + VISIBILITY_HELPER_NAME, this->GetName());
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	if (bIsDead) { return; }
	
	Super::Tick(DeltaTime);	


	auto sec = GetWorld()->GetTimeSeconds();

	UpdateLineOfSightActor();

	if (LevelSection == UPlayerStateController::LevelSection &&
		UPlayerStateController::RemainingEnemiesInSection <= GameOptions::NUMBER_OF_REMAINING_ENEMIES_TO_SHOW_HELP_CHEVRON &&
		!GetHasLineOfSightOfPlayer())
	{
		SetHelperChevronVisibility(true);
	}
	else
	{
		SetHelperChevronVisibility(false);
	}

	if (bRunningHitAnimation && bToggleRunForceBack)
	{
		if (!bForceBackImpulseHasBeenApplied)
		{
			RunForceBack(DeltaTime);
		}
	}

	bool bCanseePlayer = GetHasLineOfSightOfPlayer();
	
	if (bCanseePlayer)
	{	
		if (!bHasSeenPlayer)
		{
			bHasSeenPlayer = true;
		}	
	}
	
	// If the enemy hasn't seen the player yet, don't do anything.
	if (!bHasSeenPlayer) { return; }

	if (bRunningHitAnimation
		|| bIsInMeleeState
		|| bIsInRangedWarningState
		|| bIsInRangedAttackState)
	{
		MovementProjectileComponent->SetActive(false);
	}
	else
	{
		MovementProjectileComponent->SetActive(true);
		if (bCanseePlayer)
		{
			MovementProjectileComponent->bIsHomingProjectile = true;
		}		
		else if (GetHasLineOfSightOfAnotherEnemy())
		{
			LastStraffTime = GetWorld()->GetTimeSeconds();
			MovementProjectileComponent->bIsHomingProjectile = false;
		}
		else
		{
			RandomPatrol();
			MovementProjectileComponent->bIsHomingProjectile = false;
		}
	}
	
	
	if (sec < LastStraffTime + strafeLengthIfTargetingEnemy)
	{	
		RunStraff();
	}
	else
	{
		bIsInStraffState = false;
	}
		
	// Here we Add some warning frames before firing to let player know enemy is about to shoot
	if (bIsInRangedAttackState)
	{
		if (sec <= LastRangedAttackStart + RangedWarningBeforeFireDelay)
		{
			BaseFlipBook->Stop();
			BaseFlipBook->SetPlaybackPositionInFrames(1, false);
		}
		else
		{
			bIsInRangedWarningState = false;			
			BaseFlipBook->Play();
		}
	}

	if(sec > LastRangedAttackStart + RangedAttackFrequency)
    {		
		if (GetHasLineOfSightOfPlayer())
		{						
			RunRangeAttack();			
		}
    }

	if (sec > HitAnimationTime + CurrentHitDelay)
	{
		if (bRunningHitAnimation)
		{
			bRunningHitAnimation = false;
			bForceBackImpulseHasBeenApplied = false;
			bIsInStunState = false;
		}
	}

	if (sec > LastMeleeStart + MeleeCooldown)
	{
		bIsInMeleeState = false;
	}

	if (sec > LastRangedAttackStart + RangedAttackLength)
	{
		// range attack finished randomise the next attack...if randomise attack is an true
		if (bIsInRangedAttackState)
		{
			if (bRandomiseFrequency)
			{				
				RandomiseRangeAttackFrequency();
			}
		}

		bIsInRangedAttackState = false;
	}

	if (!bRunningHitAnimation)
	{
		if (GetPlayerEnemyIsTouching() && MeleeAnimation)
		{
			RunMelee(GetPlayerEnemyIsTouching());
		}
		else
		{
			if (bIsInRangedAttackState)
			{
				SetFlipbookToRangedAttackAnimation();				
			}
			else if (!bIsInMeleeState)
			{
				SetFlipbookToOriginalAnimation();
			}
		}
	}
}

FVector AEnemyBase::GetLocaleWithZoffset(float Offset /* = 100*/) const
{	
	FVector Location = this->GetActorLocation();
	return FVector(Location.X, Location.Y, Location.Z + Offset);
}

FVector AEnemyBase::GetAttackSpawnPointWithOffset(float ForwardOffset /* = 20 */) const
{	
	return GetLocaleWithZoffset(RangedAttackHeight) + GetActorRotationWithYawAdjustment().Vector() * ForwardOffset;
}

FVector AEnemyBase::GetLineOfSightPointWithOffset(float ForwardOffset /* = 20 */) const
{
	return GetLocaleWithZoffset(lineOfSightHeight) + GetActorRotationWithYawAdjustment().Vector() * ForwardOffset;
}

FRotator AEnemyBase::GetActorRotationWithYawAdjustment() const
{
	FRotator ro = GetActorRotation();
	return FRotator(ro.Pitch, ro.Yaw + 90, ro.Roll);
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyBase::Kill(bool bCritical)
{
	if (!bIsDead)
	{			
		RunDeathCleanup(bCritical);
	} 
}

void AEnemyBase::RunForceBack(float Dt)
{
	// if the enemy has physics, use that!
	if (BaseFlipBook && BaseFlipBook->IsSimulatingPhysics())
	{
		FVector Impulse = PlayerActor->GetActorRotation().Vector() * 50000;
		BaseFlipBook->AddImpulse(Impulse);
		bForceBackImpulseHasBeenApplied = true;
	}
	else
	{
		const float ForceBackDistance = 10;
		const float InterpSpeed = 70;

		auto CurrentRotation = this->GetActorRotation();
		bIsInForceBackState = true;
		EnemyForceBackEndPosition = this->GetActorLocation() + FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + 90, CurrentRotation.Roll).Vector() * (ForceBackDistance * -1);

		auto P = this->GetActorLocation();


		FVector V = FVector(
			FMath::FInterpTo(P.X, EnemyForceBackEndPosition.X, Dt, InterpSpeed),
			FMath::FInterpTo(P.Y, EnemyForceBackEndPosition.Y, Dt, InterpSpeed),

			FMath::FInterpTo(P.Z, EnemyForceBackEndPosition.Z, Dt, InterpSpeed));

		FHitResult* Hit = nullptr;
		this->SetActorLocation(FVector(V), true, Hit);
		auto o = GetOwner();
		if (o)
		{		
			SetActorLocation(FVector(V), true, Hit);
		}
	}
}

void AEnemyBase::AttackEnemy(
	int Attackstrength, 
	bool bIsStunAttack,
	EDamageType DamageType, 
	bool bRunForceBack /* = true */,
	float DistanceFromPlayer /* = -1 */,
	bool bCriticalHit /* = false */)
{	
	bToggleRunForceBack = bRunForceBack;

	if (bCriticalHit)
	{
		Attackstrength = Attackstrength * CriticalHitModifier;
	}

	Health -= Attackstrength;	
	
	if (bIsStunAttack) 
	{
		CurrentHitDelay = StunDelay;
		bIsInStunState = true;
		if (!bHasSpawnedBullets)
		{
			SpawnBulletPickups();
			bHasSpawnedBullets = true;
		}
	}
	else
	{
		CurrentHitDelay = HitAnimationDelay;
	}

	if (Health < 1)
	{
		AddDeathMessages(DistanceFromPlayer, DamageType, bCriticalHit);
		Kill(bCriticalHit);
	}
	else
	{
		if (HitAnimation)
		{
			BaseFlipBook->SetFlipbook(HitAnimation);
			bRunningHitAnimation = true;
			HitAnimationTime = GetWorld()->GetTimeSeconds();
		}
	}
}

void AEnemyBase::AddDeathMessages(float DistanceFromPlayer, EDamageType DamageType, bool bCriticalHit)
{
	// NOTE: If the distance from player is -1, this means it's come from a non-player actor, such as a barrel explosion.

	// In this block we only want to add messages if the damage is from a playerbullet
	if (DamageType == EDamageType::PlayerBullet)
	{
		float Multiplier = PlayerStateController->GetCurrentTimeDialation();
		float DistanceFromGround = PlayerStateController->GetCurrentDistanceFromGround();

		if (bCriticalHit)
		{
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::Critical);
		}

		if (DistanceFromGround > GameOptions::DEATH_FROM_ABOVE_REQUIRED_HEIGHT)
		{
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::DeathFromAbove);
		}

		if (FMath::IsNearlyEqual(Multiplier, GameOptions::MAX_TIME_DIALATION))
		{
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::MaxSpeed);
		}

		if (Multiplier < GameOptions::TIME_DIALATION_DANGER_ZONE)
		{
			if (PlayerStateController->GetIsPlayerInFocusState())
			{
				AddEnemyDeathSpawnMessage(EEnemyDeathTypes::Focused);
			}
			else
			{
				AddEnemyDeathSpawnMessage(EEnemyDeathTypes::DangerZone);
			}
		}

		if (DistanceFromPlayer < GameOptions::POINT_BLANK_DISTANCE)
		{
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::PointBlank);
		}

		if (DistanceFromPlayer > GameOptions::LONG_SHOT_DISTANCE)
		{
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::LongDistance);
		}

		if (bIsInStunState)
		{
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::StunnedKill);
		}

		if (!PlayerStateController->DoesCurrentWeaponHaveBullets())
		{
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::LastBullet);
		}
	}

	if (Health < GameOptions::OVERKILL_HEALTH_THRESHOLD)
	{
		AddEnemyDeathSpawnMessage(EEnemyDeathTypes::OverKill);
	}

	switch (DamageType)
	{
		case EDamageType::PlayerMelee:
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::Melee);
			break;
		case EDamageType::PlayerExplosive:
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::Grenade);
			break;
		case EDamageType::PlayerReboundAttack:
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::ReboundProjectile);
			break;
		case EDamageType::PhysicsHit:
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::PhysicsHit);
			break;
		case EDamageType::EnemyProjectile:
		case EDamageType::EnemyBullet:
		case EDamageType::EnemyMelee:
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::FriendlyFire);
			break;
		case EDamageType::EnvironmentalExplosion:
			AddEnemyDeathSpawnMessage(EEnemyDeathTypes::BarrelExplosion);
			break;
	}
}

void AEnemyBase::AddEnemyDeathSpawnMessage(EEnemyDeathTypes MessageType)
{
	MessageSpawnList.Add(MessageType);
}

bool AEnemyBase::GetIsDead() const
{
	return bIsDead;
}

int AEnemyBase::GetHealth() const
{
	return Health;
}

bool AEnemyBase::GetIsInHitState() const
{
	return bRunningHitAnimation;
}

AActor* AEnemyBase::GetPlayerEnemyIsTouching() const
{
	UWorld* World = GetWorld();
	
	FVector lo = this->GetActorLocation();
	FVector Locale = GetAttackSpawnPointWithOffset();

	FRotator ro = GetActorRotation();
	FRotator Rotation = FRotator(ro.Pitch, ro.Yaw + 90, ro.Roll);

	FVector LineTraceEnd = Locale + Rotation.Vector() * MeleeReach;

	if (bForceShowMeleeDebugLine || DebugOptions::MODE == EDebugMode::Full)
	{
		DrawDebugLine(
			World,
			Locale,
			LineTraceEnd,
			FColor(255,0, 0),
			false,
			0.f,
			0,
			5.f);
	}

	FHitResult hit;

	FCollisionQueryParams r(
		FName(TEXT("")),
		false,
		this);

	World->LineTraceSingleByObjectType(
		hit,
		Locale,
		LineTraceEnd,
		FCollisionObjectQueryParams(),
		r);

	AActor *hitActor = hit.GetActor();
	if (hitActor)
	{
		if (hitActor->ActorHasTag(Tags::PLAYER))
		{			
			return hitActor;
		}
	}
	return nullptr;
}

void AEnemyBase::RunMelee(AActor* actor)
{
	if (MeleeAnimation && bHasMeleeAttack)
	{
		if (!GetIsInHitState())
		{
			BaseFlipBook->SetFlipbook(MeleeAnimation);
			LastMeleeStart = GetWorld()->GetTimeSeconds();
			bIsInMeleeState = true;

			if (AttackFrames.Num() <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("No attack frame has been set on enemy %s"), *this->GetName());
			}
			else
			{				
				uint16 CurrentFrame = BaseFlipBook->GetPlaybackPositionInFrames();
				for (const auto& AttackFrame : AttackFrames)
				{
					if (AttackFrame == CurrentFrame)
					{
						RunAttackOnPlayer(actor, MeleeStrength);
					};
				};
			}
		}
	}
}

void AEnemyBase::RunRangeAttack()
{	
	if (RangeAttackAnimation && bHasrangedAttack)
	{
		if (!bRunningHitAnimation && !bIsInMeleeState)
		{
			if (!bIsInRangedAttackState)
			{
				SetLastRangedAttackStartTime();
				BaseFlipBook->SetFlipbook(RangeAttackAnimation);
			}
			bIsInRangedAttackState = true;
			bIsInRangedWarningState = true;
		}
	}
}

void AEnemyBase::SetFlipbookToOriginalAnimation()
{
	if (WalkingAnimation)
	{
		BaseFlipBook->SetFlipbook(WalkingAnimation);
	}
}

void AEnemyBase::SetFlipbookToRangedAttackAnimation()
{
	if (RangeAttackAnimation)
	{
		BaseFlipBook->SetFlipbook(RangeAttackAnimation);
	}
}

void AEnemyBase::SetLastRangedAttackStartTime()
{
	if (bHasSeenPlayer)
	{		
		LastRangedAttackStart = GetWorld()->GetTimeSeconds();
	}
}

void AEnemyBase::RunAttackOnPlayer(const AActor* HitActor, int AttackStrength)
{
	if (PlayerStateController)
	{
		PlayerStateController->Attack(AttackStrength);
	}
	else
	{
		Logger::LogWarn("Could not find UPlayerStateController when running attack from EnemyBase", this->GetName());
	}
}

bool AEnemyBase::GetIsinMeleeState() const
{
	return bIsInMeleeState;
}

bool AEnemyBase::GetIsInRangedWarningState() const
{
	return bIsInRangedWarningState;
}

bool AEnemyBase::GetIsInRangedAttackState() const
{
	return bIsInRangedAttackState;
}

bool AEnemyBase::GetHasLineOfSightOfPlayer() const
{		
	return CheckLineOfSight(Tags::PLAYER);
}

bool AEnemyBase::GetHasLineOfSightOfAnotherEnemy() const
{
	return CheckLineOfSight(Tags::ENEMY);
}

bool AEnemyBase::GetShouldSpawnBlood() const
{
	return bShouldSpawnBlood;
}

bool AEnemyBase::CheckLineOfSight(const FName& Tag) const
{
	if (CurrentLineOfSightActor)
	{
		if (CurrentLineOfSightActor->ActorHasTag(Tag))
		{
			return true;
		}
	}
	return false;
}

void AEnemyBase::UpdateLineOfSightActor()
{
	auto world = GetWorld();
	auto owner = GetOwner();
	auto Locale = GetLineOfSightPointWithOffset();
	
	FVector LineTraceEnd = PlayerActor->GetActorLocation();

	if (bForceLineOfSightDebugLine || DebugOptions::MODE == EDebugMode::Full)
	{	
		DrawDebugLine(
			world,
			Locale,
			LineTraceEnd,
			FColor(0, 0, 255),
			false,
			0.f,
			0,
			5.f);
	}

	FHitResult hit;

	FCollisionQueryParams r(
		FName(TEXT("")),
		false,
		owner);

	world->LineTraceSingleByObjectType(
		hit,
		Locale,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_Pawn)),
		r);	

	if (hit.Distance < lineOfSightLength)
	{	
		CurrentLineOfSightActor = hit.GetActor();
	}
}

void AEnemyBase::RunDeathCleanup(bool bCritical)
{
	float Delay = 0;
	int ScoreToAdd = Worth;
	float Multiplier = PlayerStateController->GetCurrentTimeDialation();	

	SetHelperChevronVisibility(false);
	for (const EEnemyDeathTypes &DeathType : MessageSpawnList)
	{	
			int SpecialKillScoreValue = GameInstance->GetSpecialKillScoreValue(DeathType);
			
			if (SpecialKillScoreValue >= GameOptions::MIN_SPECIAL_KILL_BONUS)
			{
				ScoreToAdd += SpecialKillScoreValue;
				Delay += DelayBetweenDeathMessages;		
				SpawnDeathText(GameInstance->GetDeathMessage(DeathType), Delay, SpecialKillScoreValue);
				GameInstance->DecrementSpecialSkillScoreValue(DeathType);
			}
	}
	
	if (PlayerActor && PlayerStateController)
	{		
		PlayerStateController->IncreaseScore(ScoreToAdd * Multiplier);
		UPlayerStateController::EnemiesKilled ++;
	}

	SpawnTimeItems();

	if (Health < GibsThreshold)
	{
		if (Gibs)
		{
			FVector Start = GetLocaleWithZoffset(GibsSpawnHeight);
			FRotator Rotation = FRotator(0,0,0);
			GetWorld()->SpawnActor(Gibs, &Start, &Rotation);

			// Have to destroy if spawning gibs, otherwise there's a dead body after it's seemingly exploded
			this->Destroy();
			return;
		}
	}

	if (DestroyOnKilled)
	{
		this->Destroy();
	}
	else
	{

		if (bCritical)
		{
			FVector SpawnPosition = CriticalHitPointComponent->GetComponentLocation();
			FRotator Rotation = FRotator(0, 0, 0);
			GetWorld()->SpawnActor(CriticalDeathSpawnBlueprint, &SpawnPosition, &Rotation);
		}

		if (DeathAnimation)
		{
			BaseFlipBook->SetFlipbook(bCritical && CriticalDeathAnimation ? CriticalDeathAnimation : DeathAnimation);
			BaseFlipBook->SetLooping(false);
			BaseFlipBook->Play();		
		}
		else
		{
			Logger::LogWarn("Enemy DestroyOnKilled is set to false, but no death animation has been set.", this->GetName());
			return;

		}
		bIsDead = true;
		SetActive(false);
	}
}

void AEnemyBase::SpawnTimeItems() const
{
	for (int i = 0; i < TimePickUpCount; i++)
	{
		FVector Start = GetLocaleWithZoffset();
		FRotator Rotation = GetActorRotation();
		GetWorld()->SpawnActor(TimePickUpItem, &Start, &Rotation);
	}
}

void AEnemyBase::SpawnDeathText(const FString& Text, float SpawnDelay, int SpecialKillScore) const
{
	if (!DeathTextRenderer)
	{
		Logger::LogWarn("No Text renderer has been set", this->GetName());
		return;
	}
			
	FVector Location = GetLocaleWithZoffset();
	ASpawnableText* SpawnedText = Cast<ASpawnableText>(GetWorld()->SpawnActor(DeathTextRenderer, &Location));
	if (SpawnedText)
	{			
		SpawnedText->UpdateText(Text);
		SpawnedText->SetEnabledDelay(SpawnDelay);
	}
	else
	{
		Logger::LogWarn("Could not cast the provided DeathTextRenderer Blueprint to an ASpawnableText", this->GetName());
	}

	ASpawnableText* SpawnedScoreText = Cast<ASpawnableText>(GetWorld()->SpawnActor(DeathTextScoreRenderer, &Location));
	if (SpawnedScoreText)
	{
		SpawnedScoreText->UpdateText(TEXT(" +") + FString::FromInt(SpecialKillScore));
		SpawnedScoreText->SetEnabledDelay(SpawnDelay);
	}
	else
	{
		Logger::LogWarn("Could not cast the provided SpawnedScoreText Blueprint to an ASpawnableText", this->GetName());
	}
}

bool AEnemyBase::GetIsInStunState() const
{
	return bIsInStunState;
}

void AEnemyBase::SpawnBulletPickups() const
{
	if (PlayerStateController)
	{	
		// Don't spawn if both weapons are at max ammo
		if (PlayerStateController->AlternateWeaponAtMaxAmmo() && PlayerStateController->CurrentWeaponAtMaxAmmo())
		{
			return;
		}
	}
	
	for (int i = 0; i < BulletPickUpCount; i++)
	{
		FVector Start = GetLocaleWithZoffset();
		FRotator Rotation = GetActorRotation();
		GetWorld()->SpawnActor(BulletPickUpItem, &Start, &Rotation);
	}
}

void AEnemyBase::RandomiseRangeAttackFrequency()
{
	RangedAttackFrequency = FMath::RandRange(RandomisedAttackFrequencyRange.Min, RandomisedAttackFrequencyRange.Max);
}

void AEnemyBase::SetActive(bool Active)
{
	this->MovementProjectileComponent->SetActive(Active);
	this->SetActorEnableCollision(Active);
	this->SetActorTickEnabled(Active);
}

void AEnemyBase::RunStraff()
{	
	if (!bIsInStraffState)
	{		
		isStraffLeft = FMath::RandRange(0, 10) > 5;
		bIsInStraffState = true;
	}
	else
	{
		MovementProjectileComponent->SetActive(true);		
		FVector Cv = this->GetVelocity();
		if (isStraffLeft)
		{
			MovementProjectileComponent->SetVelocityInLocalSpace(FVector(-strafeSpeedIfTargetingEnemy, Cv.Y, Cv.Z));
		}
		else
		{			
			MovementProjectileComponent->SetVelocityInLocalSpace(FVector(strafeSpeedIfTargetingEnemy, Cv.Y, Cv.Z));
		}
	}
}

void AEnemyBase::SetHelperChevronVisibility(bool bIsVisible) const
{
	if (ChevronFlipBook)
	{
		ChevronFlipBook->SetHiddenInGame(!bIsVisible, false);
	}
}

void AEnemyBase::RandomPatrol()
{
	const int LENGTH_OF_TIME_BETWEEN_CHECKPOINTS = 3;	

	float sec = GetWorld()->GetTimeSeconds();

	if (!bIsInRandomPatrolState)
	{
		MovementProjectileComponent->SetVelocityInLocalSpace(FVector(FMath::RandRange(-PatrolSpeed, -PatrolSpeed), FMath::RandRange(-PatrolSpeed, -PatrolSpeed), 0));
		bIsInRandomPatrolState = true;
		TimeOfLastRandomPatrolCheckPoint = sec;
	}

	if (sec > TimeOfLastRandomPatrolCheckPoint + LENGTH_OF_TIME_BETWEEN_CHECKPOINTS)
	{	
		MovementProjectileComponent->SetVelocityInLocalSpace(FVector(FMath::RandRange(-PatrolSpeed, -PatrolSpeed), FMath::RandRange(-PatrolSpeed, -PatrolSpeed), 0));
		TimeOfLastRandomPatrolCheckPoint = sec;
	}
}