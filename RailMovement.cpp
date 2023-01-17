#include "RailMovement.h"
#include <DeadHunt/Logger.h>
#include "DrawDebugHelpers.h"

URailMovement::URailMovement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URailMovement::BeginPlay()
{
	Super::BeginPlay();	
	auto Owner = GetOwner();
	OriginalLocation = Owner->GetActorLocation();
	OriginalRotation = Owner->GetActorRotation();
	TimeSpawned = GetWorld()->GetTimeSeconds();	
	SplineComponent = Owner->FindComponentByClass<USplineComponent>();

}

void URailMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);	
	Length = TransformData.Num() - 1;

	if (!InitalPlayHasBeenSet && bPlayOnStart)
	{
		float Sec = GetWorld()->GetTimeSeconds();
		if (Sec > TimeSpawned + DelayBeforeInitalPlay)
		{
			StartAnimation();
			InitalPlayHasBeenSet = true;
		}
	}

	if (bFinished || Length < 1)
	{
		return;
	}	

	if (DebugOptions::MODE == EDebugMode::Full)
	{	
		for (int i = 0; i < Length; i++)
		{
			DrawDebugLine(
				GetWorld(),
				TransformData[i].Location,
				TransformData[i + 1].Location,
				FColor(255, 255, 255),
				false,
				0.f,
				0,
				5.f);
		}
	}

	FVector Locale = GetOwner()->GetActorLocation();
	FRotator Rote = GetOwner()->GetActorRotation();	

	if (bPlay)
	{
		if (IsLerpComplete(Locale))
		{		
			if (CurrentIndex < Length - 1)
			{					
				CurrentIndex++;
			}
			else
			{						
				if (bRepeat)
				{
					ResetAnimation();					
				}						
				else
				{						
					OnFinished();					
				}
				return;
			}
		}

		AccumulatedAcceleration += AccelerationModifier;
		float MoveSpeed = (TransformData[CurrentIndex].Speed + AccumulatedAcceleration + GlobalSpeed) * AccumulatedAcceleration;

		if (MoveSpeed > MaxMoveSpeed && MaxMoveSpeed > 0)
		{
			MoveSpeed = MaxMoveSpeed;
		}

		float NewX = FMath::FInterpConstantTo(Locale.X, TransformData[CurrentIndex + 1].Location.X, DeltaTime, MoveSpeed);
		float NewY = FMath::FInterpConstantTo(Locale.Y, TransformData[CurrentIndex + 1].Location.Y, DeltaTime, MoveSpeed);
		float NewZ = FMath::FInterpConstantTo(Locale.Z, TransformData[CurrentIndex + 1].Location.Z, DeltaTime, MoveSpeed);
		GetOwner()->SetActorLocation(FVector(NewX, NewY, NewZ));

		float RoteSpeed = TransformData[CurrentIndex].RotationSpeed + GlobalRotationSpeed;
		if (RoteSpeed > MaxRotationSpeed && MaxRotationSpeed > 0)
		{
			RoteSpeed = MaxRotationSpeed;
		}
		
		float NewRotePitch = FMath::FInterpConstantTo(Rote.Pitch, TransformData[CurrentIndex + 1].Roatation.Pitch, DeltaTime, RoteSpeed);
		float NewRoteRoll = FMath::FInterpConstantTo(Rote.Roll, TransformData[CurrentIndex + 1].Roatation.Roll, DeltaTime, RoteSpeed);
		float NewRoteYaw = FMath::FInterpConstantTo(Rote.Yaw, TransformData[CurrentIndex + 1].Roatation.Yaw, DeltaTime, RoteSpeed);

		GetOwner()->SetActorRotation(FRotator(NewRotePitch, NewRoteYaw, NewRoteRoll));
	}
}

void URailMovement::StartAnimation()
{
	bPlay = true;
}

void URailMovement::StopAnimation()
{
	bPlay = false;
}
void URailMovement::ResetAnimation()
{
	bFinished = false;
	CurrentIndex = 0;
	auto Owner = GetOwner();
	Owner->SetActorLocation(OriginalLocation);
	Owner->SetActorRotation(OriginalRotation);
	if (bShouldResetAccelerationOnComplete)
	{
		AccumulatedAcceleration = 1;
	}

}

bool URailMovement::IsLerpComplete(FVector& Locale) const
{
	if (Locale.X > TransformData[CurrentIndex + 1].Location.X - LerpEpsilon && Locale.X < TransformData[CurrentIndex + 1].Location.X + LerpEpsilon)
	{
		if (Locale.Y > TransformData[CurrentIndex + 1].Location.Y - LerpEpsilon && Locale.Y < TransformData[CurrentIndex + 1].Location.Y + LerpEpsilon)
		{
			if (Locale.Z > TransformData[CurrentIndex + 1].Location.Z - LerpEpsilon && Locale.Z < TransformData[CurrentIndex + 1].Location.Z + LerpEpsilon)
			{
				return true;
			}
		}
	}
	return false;
}

void URailMovement::OnFinished()
{	
	if (DestoryOnComplete)
	{
		GetOwner()->Destroy();
	};
	bFinished = true;
}

void URailMovement::PopulateSplineWithTransformData()
{
	if (!AssertAndAttemptToAssignSplineComponent()) { return; }
	
	SplineComponent->ClearSplinePoints();

	for (int i = 0; i < TransformData.Num(); i++)
	{
		SplineComponent->AddSplinePoint(TransformData[i].Location, ESplineCoordinateSpace::World);
		SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);		
	}
}

void URailMovement::PopulateTransformWithSplineData()
{
	if (!AssertAndAttemptToAssignSplineComponent()) { return; }

	TransformData.Empty();

	TArray<FInterpCurvePoint<FVector>> Points = SplineComponent->GetSplinePointsPosition().Points;	
	TArray<FInterpCurvePoint<FQuat>> RotePoints = SplineComponent->GetSplinePointsRotation().Points;
	
	for (int i = 0; i < Points.Num(); i++)
	{
		SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
		FTransformFrame Tf = FTransformFrame();
		Tf.Location = SplineComponent->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::World);
		Tf.Roatation = SplineComponent->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
		TransformData.Add(Tf);
	};
}

bool URailMovement::AssertAndAttemptToAssignSplineComponent()
{
	if (!SplineComponent)
	{
		SplineComponent = GetOwner()->FindComponentByClass<USplineComponent>();
		if (!SplineComponent)
		{
			Logger::LogError("Could not find a spline component on parent actor", this->GetName());
			return false;
		}
	}
	return true;
}