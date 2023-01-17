// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <DeadHunt\Logger.h>
#include "GameFramework/ProjectileMovementComponent.h"
#include <DeadHunt\ScaleOverTime.h>
#include <DeadHunt\FacePlayer.h>
#include <DeadHunt\ToggleEnabledAfterTime.h>
#include <DeadHunt\Logger.h>
#include "Components/TextRenderComponent.h"
#include "Components/ArrowComponent.h"
#include "SpawnableText.generated.h"

UCLASS()
class DEADHUNT_API ASpawnableText : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnableText();
	virtual void Tick(float DeltaTime) override;
	
	void UpdateText(const FString& NewText);
	void SetEnabledDelay(float NewDelay);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* TextRenderComponent = nullptr;	
	
	UPROPERTY(EditAnywhere)
	UFacePlayer* FacePlayer = nullptr;
	
	UPROPERTY()
	UProjectileMovementComponent* MovementProjectileComponent = nullptr;
	
	UPROPERTY()
	UToggleEnabledAfterTime* ToggleEnabledAfterTime = nullptr;	

	UPROPERTY()
	UScaleOverTime* ScaleOverTime = nullptr;	
	
	UPROPERTY(EditAnywhere)
	float DelayBeforeEnabled = .0f;

	void SetupTextRenderComponent();
	void SetupScaleOverTime();	
	void SetupProjectileComponent();
	void SetupToggleEnabledAfterTime();

	template<class T>
	bool AssertDefaultSubsExists()
	{
		T* Exists = this->FindComponentByClass<T>();
		if (Exists)	
		{
			return true;
		}
		else
		{
			Logger::LogError("A critical component appears to be missing!", this->GetName());
			return false;
		}
	}
};
