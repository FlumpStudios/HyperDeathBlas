#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include <DeadHunt\Configuration.h>
#include "GameInstanceMain.generated.h"

USTRUCT()
struct FLoadOut
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Load out")
	int meleeIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Load out")
	uint8 PrimaryWeaponIndex = 1;

	UPROPERTY(EditAnywhere, Category = "Load out")
	uint8 SecondaryWeaponIndex = 5;
};

UCLASS()
class DEADHUNT_API UGameInstanceMain : public UGameInstance
{
GENERATED_BODY()
public:
	UGameInstanceMain();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	int CurrentWorld = -1; // World 0 = main menu; World -1 = intro 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	int CurrentLevel = 1;

	// Default amount is 300
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graphics")
	int PixelationLevel = 310;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	bool bPixelateEffectOn = true;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	float MotionBlurAmount = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	float FovOnMoveAmount = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	float FovOnFocus = 14.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	float BaseFov = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	int SfxVolune = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	int MusicVolune = 10;

	UPROPERTY(EditAnywhere, Category = "Load out")
	FLoadOut CurrentLoadOut;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Weapons")
	int MaxGranades = 3;

	// 0.04 is about medium fog level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	float FogDensity = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	bool FogEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	bool bUseLegacyPostProcessing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameStats")
	float LevelTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameStats")
	float TotalPlayTime = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameStats")
	int CurrentScore = 0;

	TMap<EEnemyDeathTypes, int> SpecialSkillScoreTable;

	UFUNCTION(BlueprintCallable)
	void ToggleMotionBlur();
	
	UFUNCTION(BlueprintCallable)
	void IncrementMusicVolume();

	UFUNCTION(BlueprintCallable)
	void DecrementMusicVolume();

	UFUNCTION(BlueprintCallable)
	void IncrementSfxVolume();

	UFUNCTION(BlueprintCallable)
	int GetPixelationDisplayValue() const;

	UFUNCTION(BlueprintCallable)
	void DecrementSfxVolume();

	UFUNCTION(BlueprintCallable)
	void IncrementPixelationLevel();

	UFUNCTION(BlueprintCallable)
	void DecrementPixelationLevel();

	UFUNCTION(BlueprintCallable)
	void IncrementFogLevel();

	UFUNCTION(BlueprintCallable)
	void DecrementFogLevel();
	
	UFUNCTION(BlueprintCallable)
	float GetFogDisplayValue() const;

	UFUNCTION()
	void ResetSpecialKillScoreTable();

	UFUNCTION()
	void DecrementSpecialSkillScoreValue(EEnemyDeathTypes DeathType);
	
	UFUNCTION()
	int GetSpecialKillScoreValue(EEnemyDeathTypes DeathType);

	UFUNCTION()
	FString GetDeathMessage(EEnemyDeathTypes MessageType) const;
private:	
	const int MAX_PIXEL_LEVEL = 450;
	const int MIN_PIXEL_LEVEL = 250;	
	const int PIXEL_LEVEL_INC_AMOUNT = 20;

	const float MAX_FOG_LEVEL = 0.099f; //0.035f
	const float MIN_FOG_LEVEL = 0.001f;
	const float FOG_LEVEL_INC_AMOUNT = 0.01f;

	const float MOTION_BLUR_BASE_INCREMENT_VALUE = 0.5f;
};