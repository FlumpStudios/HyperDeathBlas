#include "ScaleOverTime.h"

UScaleOverTime::UScaleOverTime()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UScaleOverTime::BeginPlay()
{
	Super::BeginPlay();	
	AActor* Owner = GetOwner();
	Primative =  Owner->FindComponentByClass<UPrimitiveComponent>();	
	StartTime = GetWorld()->GetTimeSeconds();
}

void UScaleOverTime::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Primative || bHasFinishedScaling) { return; }
	
	if ((GetWorld()->GetTimeSeconds() - StartTime) >= Delay)
	{
		auto CurVec = Primative->GetComponentScale();

		FVector NewVec = FVector(
			FMath::FInterpConstantTo(CurVec.X, FinalScale.X, DeltaTime, Speed),
			FMath::FInterpConstantTo(CurVec.Y, FinalScale.Y, DeltaTime, Speed),
			FMath::FInterpConstantTo(CurVec.Z, FinalScale.Z, DeltaTime, Speed));

		Primative->SetWorldScale3D(NewVec);

		if (CurVec.Equals(FinalScale, 0.1))
		{
			OnComplete();
		}
	}
}

void UScaleOverTime::OnComplete()
{
	bHasFinishedScaling = true;
	if (DestroyParentOnComplete)
	{
		GetOwner()->Destroy();
	}
}

bool UScaleOverTime::GetHasFinishedScaling() const
{
	return bHasFinishedScaling;
}

void UScaleOverTime::SetDestroyParentOnComplete(bool ShouldDestroyOnComplete)
{
	DestroyParentOnComplete = ShouldDestroyOnComplete;
}

void UScaleOverTime::SetFinalScale(const FVector& NewFinalScale)
{
	FinalScale = NewFinalScale;
}

void UScaleOverTime::SetSpeed(float NewSpeed)
{
	Speed = NewSpeed;
}

void UScaleOverTime::SetDelay(float NewDelay)
{
	Delay = NewDelay;
}



