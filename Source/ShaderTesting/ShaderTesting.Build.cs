// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShaderTesting : ModuleRules
{
	public ShaderTesting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(new string[] {"ComputeShaders"});
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RenderCore", "RHI", "ComputeShaders"});
	}
}

