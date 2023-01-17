#include "FadeCameraAfterTime.h"
#include <DeadHunt\Logger.h>

UFadeCameraAfterTime::UFadeCameraAfterTime()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFadeCameraAfterTime::BeginPlay()
{
	Super::BeginPlay();
	auto Owner = GetOwner();
	CurrentCamera = Owner->FindComponentByClass<UCameraComponent>();

	if(CurrentCamera)
	{
		Logger::LogError("Could not find a camera component on an actor where a UFadeCameraAfterTime actor component is present", Owner->GetName());
	};
}

void UFadeCameraAfterTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto Sec = GetWorld()->GetTimeSeconds();
	
	if (Sec > Delay)
	{	
		StartValue = FMath::FInterpConstantTo(StartValue, EndValue, DeltaTime, FadeSpeed);
		CurrentCamera->PostProcessSettings.ColorGain = FVector4(StartValue, StartValue, StartValue, StartValue);

		if (FMath::IsNearlyEqual(StartValue, EndValue))
		{
			SetComponentTickEnabled(false);
		}
	};
}

