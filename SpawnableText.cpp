#include "SpawnableText.h"
#include <DeadHunt\Tags.h>

ASpawnableText::ASpawnableText()
{ 	
	PrimaryActorTick.bCanEverTick = true;
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("The text renderer compoent"));
	FacePlayer = CreateDefaultSubobject<UFacePlayer>(TEXT("Face player component"));
	MovementProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("projectile component to control enemy movements"));
	ToggleEnabledAfterTime = CreateDefaultSubobject<UToggleEnabledAfterTime>(TEXT("To enable after the actor and components after a delay"));
	ScaleOverTime = CreateDefaultSubobject<UScaleOverTime>(TEXT("Scale the object after a time"));
	SetRootComponent(TextRenderComponent);			

	SetupScaleOverTime();	
	SetupProjectileComponent();
	SetupToggleEnabledAfterTime();

	this->Tags.Add(FName(Tags::WORLD_TEXT));
}

void ASpawnableText::SetupTextRenderComponent()
{
	if (!AssertDefaultSubsExists<UTextRenderComponent>()) { return; }
}

void ASpawnableText::SetupToggleEnabledAfterTime()
{	
	if (!AssertDefaultSubsExists<UToggleEnabledAfterTime>()) { return; }
	ToggleEnabledAfterTime->SetDelay(.0f);
	ToggleEnabledAfterTime->SetIncludeComponents(true);
	ToggleEnabledAfterTime->SetForceToggleOnLoad(false);
	ToggleEnabledAfterTime->SetShouldDisable(false);
}

void ASpawnableText::SetupScaleOverTime()
{
	if (!AssertDefaultSubsExists<UScaleOverTime>()) { return; }
	ScaleOverTime->SetFinalScale(FVector(1.0f, 1.0f, 0.1f));
	ScaleOverTime->SetDelay(2.0f);
	ScaleOverTime->SetDestroyParentOnComplete(true);
	ScaleOverTime->SetSpeed(1.0f);
	ScaleOverTime->SetActive(false);
}

void ASpawnableText::SetupProjectileComponent()
{
	if (!AssertDefaultSubsExists<UMovementComponent>()) { return; }
	MovementProjectileComponent->InitialSpeed = 75.0f;
	MovementProjectileComponent->ProjectileGravityScale = .0f;
	MovementProjectileComponent->Velocity = FVector(0, 0, 1);
	MovementProjectileComponent->bAutoActivate = false;
}

void ASpawnableText::SetEnabledDelay(float NewDelay)
{
	ToggleEnabledAfterTime->SetDelay(NewDelay);
}

void ASpawnableText::BeginPlay()
{
	Super::BeginPlay();		
}

void ASpawnableText::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawnableText::UpdateText(const FString& NewText)
{
	if (TextRenderComponent)
	{
		FText TextVariable = FText::AsCultureInvariant(NewText);
		TextRenderComponent->SetText(TextVariable);
	}	
}

