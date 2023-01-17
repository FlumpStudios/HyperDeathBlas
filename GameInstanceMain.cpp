#include "GameInstanceMain.h"
#include <DeadHunt/Logger.h>

UGameInstanceMain::UGameInstanceMain()
{
	ResetSpecialKillScoreTable();
}

void UGameInstanceMain::ResetSpecialKillScoreTable()
{
	SpecialSkillScoreTable.Reset();
	for (uint8 i = 0; i < static_cast<uint8>(EEnemyDeathTypes::Last); i++)
	{
		SpecialSkillScoreTable.Add(static_cast<EEnemyDeathTypes>(i), GameOptions::BASE_SPECIAL_KILL_BONUS);
	}
}


void UGameInstanceMain::ToggleMotionBlur()
{
	if (MotionBlurAmount > 0)
	{
		MotionBlurAmount = 0;
	}
	else
	{
		MotionBlurAmount = MOTION_BLUR_BASE_INCREMENT_VALUE;
	}
}

void UGameInstanceMain::DecrementSpecialSkillScoreValue(EEnemyDeathTypes DeathType)
{
	int* CurrentValuePtr = SpecialSkillScoreTable.Find(DeathType);
	if (CurrentValuePtr)
	{
		int NewValue = *CurrentValuePtr - (*CurrentValuePtr * GameOptions::SPECIAL_KILL_DECREMENT_MULTIPLIER);
		if (NewValue >= GameOptions::MIN_SPECIAL_KILL_BONUS)
		{
			*CurrentValuePtr = NewValue;
		}
	}
	else
	{
		Logger::LogWarn("Could not find the provided death type in the SpecialSkillScoreTable Map when attempting to decrement", TEXT("Gameinstance"));
	}
}

void UGameInstanceMain::IncrementMusicVolume()
{
	if (MusicVolune < 10)
	{
		MusicVolune++;
	}
}

void UGameInstanceMain::DecrementMusicVolume()
{
	if (MusicVolune > 0)
	{
		MusicVolune--;
	}
}

void UGameInstanceMain::IncrementSfxVolume()
{
	if (SfxVolune < 10)
	{
		SfxVolune++;
	}
}

void UGameInstanceMain::DecrementSfxVolume()
{
	if (SfxVolune > 0)
	{
		SfxVolune--;
	}
}

void UGameInstanceMain::IncrementPixelationLevel()
{
	if (PixelationLevel < MAX_PIXEL_LEVEL)
	{
		PixelationLevel += PIXEL_LEVEL_INC_AMOUNT;
	}	
	if (PixelationLevel == MAX_PIXEL_LEVEL)
	{
		bPixelateEffectOn = false;
	}
}

void UGameInstanceMain::DecrementPixelationLevel()
{
	if (PixelationLevel > MIN_PIXEL_LEVEL)
	{
		PixelationLevel -= PIXEL_LEVEL_INC_AMOUNT;
	}
	if (PixelationLevel < MAX_PIXEL_LEVEL)
	{
		bPixelateEffectOn = true;
	}
}

void UGameInstanceMain::IncrementFogLevel()
{
	if (FogDensity < MAX_FOG_LEVEL)
	{
		FogDensity += FOG_LEVEL_INC_AMOUNT;
	}
	if (FogDensity > MIN_FOG_LEVEL)
	{
		FogEnabled = true;
	}
}

void UGameInstanceMain::DecrementFogLevel()
{
	if (FogDensity > MIN_FOG_LEVEL)
	{
		FogDensity -= FOG_LEVEL_INC_AMOUNT;
	}
	if (FogDensity <= MIN_FOG_LEVEL)
	{
		FogEnabled = false;
	}
}

int UGameInstanceMain::GetPixelationDisplayValue() const
{
	int p = (PixelationLevel - MIN_PIXEL_LEVEL) / 20;
	return (p - 10) * -1;
}

float UGameInstanceMain::GetFogDisplayValue() const
{	
	return FogDensity * 100;
}

int UGameInstanceMain::GetSpecialKillScoreValue(EEnemyDeathTypes DeathType)
{
	int* CurrentValuePtr = SpecialSkillScoreTable.Find(DeathType);
	if (CurrentValuePtr)
	{
		return *CurrentValuePtr;
	}
	else
	{
		Logger::LogWarn("Could not find the provided death type in the SpecialSkillScoreTable Map", TEXT("Gameinstance"));
		return 0;
	}
}

FString UGameInstanceMain::GetDeathMessage(EEnemyDeathTypes MessageType) const
{
	switch (MessageType)
	{
	case EEnemyDeathTypes::PointBlank:
		return TEXT("Point Blank");
	case EEnemyDeathTypes::LongDistance:
		return TEXT("Long Shot");
	case EEnemyDeathTypes::Melee:
		return TEXT("Booted");
	case EEnemyDeathTypes::Grenade:
		return TEXT("BOOM");
	case EEnemyDeathTypes::BarrelExplosion:
		return TEXT("Enviromental");
	case EEnemyDeathTypes::PhysicsHit:
		return TEXT("In the Face");
	case EEnemyDeathTypes::StunnedKill:
		return TEXT("Stunner");
	case EEnemyDeathTypes::FriendlyFire:
		return TEXT("Friendly Fire");
	case EEnemyDeathTypes::ReboundProjectile:
		return TEXT("On the Rebound");
	case EEnemyDeathTypes::DeathFromAbove:
		return TEXT("Death From Above");
	case EEnemyDeathTypes::OverKill:
		return TEXT("Overkill");
	case EEnemyDeathTypes::MaxSpeed:
		return TEXT("Speed Freak");
	case EEnemyDeathTypes::DangerZone:
		return TEXT("Survivor");
	case EEnemyDeathTypes::LastBullet:
		return TEXT("Last bullet");
	case EEnemyDeathTypes::Focused:
		return TEXT("Focused");
	case EEnemyDeathTypes::Critical:
		return TEXT("Criti-kill");
	default:
		Logger::LogWarn("Could not find an enemy death message for provided enemy death type", TEXT("Game Instance"));
		return TEXT("");
	}
};
