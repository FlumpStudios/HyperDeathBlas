#pragma once
#include "CoreMinimal.h"
namespace Logger
{
	void LogInfo(const FString& Message, const FString& Name);
	void LogWarn(const FString& Message, const FString& Name);
	void LogError(const FString& Message, const FString& Name);
}

