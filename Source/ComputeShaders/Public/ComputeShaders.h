// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#define NUM_THREADS_PER_GROUP_DIMENSION 32

DECLARE_LOG_CATEGORY_EXTERN(LogComputeShaders, Log, All);

class COMPUTESHADERS_API FComputeShadersModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

// Logging macros
#if !UE_BUILD_SHIPPING
#include "Engine/Engine.h"
#define print(text, ...) UE_LOG(LogComputeShaders, Log, TEXT(text), ##__VA_ARGS__)
#define printw(text, ...) UE_LOG(LogComputeShaders, Warning, TEXT(text), ##__VA_ARGS__)
#define printe(text, ...) UE_LOG(LogComputeShaders, Error, TEXT(text), ##__VA_ARGS__)
#define printsc(key, text, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(key, 6, FColor::Green, FString::Printf(TEXT(text), ##__VA_ARGS__));
#define printscw(key, text, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(key, 6, FColor::Yellow, FString::Printf(TEXT(text), ##__VA_ARGS__));
#define printsce(key, text, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(key, 6, FColor::Red, FString::Printf(TEXT(text), ##__VA_ARGS__).ToUpper(), true, FVector2D(3.f,3.f));
#else
#define print(text, ...)
#define printw(text, ...)
#define printe(text, ...)
#define printsc(key, text, ...)
#define printscw(key, text, ...)
#define printsce(key, text, ...)
#endif // !UE_BUILD_SHIPPING