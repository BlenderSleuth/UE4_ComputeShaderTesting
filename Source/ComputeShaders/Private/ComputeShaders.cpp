// Copyright Ben Sutherland 2021. All rights reserved.

#include "ComputeShaders.h"
#include "Modules/ModuleManager.h"
#include "ShaderCore.h"

DEFINE_LOG_CATEGORY(LogComputeShaders);

void FComputeShadersModule::StartupModule()
{
	//const FString ShaderDir = FPaths::Combine(FPaths::GameSourceDir(), TEXT("ComputeShaders/Shaders"));
	//AddShaderSourceDirectoryMapping("/ComputeShaders", ShaderDir);

	const FString ShaderDir = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders/Private"));
	AddShaderSourceDirectoryMapping("/ComputeShaders", ShaderDir);
}

void FComputeShadersModule::ShutdownModule()
{
}

IMPLEMENT_GAME_MODULE(FComputeShadersModule, ComputeShaders);

