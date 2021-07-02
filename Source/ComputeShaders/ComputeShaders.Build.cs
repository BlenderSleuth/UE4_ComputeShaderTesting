// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ComputeShaders : ModuleRules
{
	public ComputeShaders(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine" });
		PrivateDependencyModuleNames.AddRange(new[] { "RenderCore", "RHI" });
	}
}
