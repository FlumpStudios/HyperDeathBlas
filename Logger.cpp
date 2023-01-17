#include "Logger.h"
#include "Engine/World.h"
#include <DeadHunt\Configuration.h>

#define LOG_MESSAGE(LEVEL) \
	if ((uint8)DebugOptions::MODE > 0)\
	{\
		UE_LOG(LogTemp, LEVEL, TEXT("Message: %s From: %s"), *Message, *Name)\
	}

void Logger::LogInfo(const FString &Message, const FString &Name)
{
	LOG_MESSAGE(Display);	
}

void Logger::LogWarn(const FString& Message, const FString &Name)
{
	LOG_MESSAGE(Warning);
}

void Logger::LogError(const FString& Message, const FString &Name)
{
	LOG_MESSAGE(Error);
}


	