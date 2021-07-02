// Copyright Ben Sutherland 2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ComputeShaders.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"


class COMPUTESHADERS_API FRayTracingCS : public FGlobalShader
{
public:
	// Check out ShaderParametersStruct: L20
	DECLARE_GLOBAL_SHADER(FRayTracingCS);
	SHADER_USE_PARAMETER_STRUCT(FRayTracingCS, FGlobalShader);

	// Shader I/O
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
		SHADER_PARAMETER_TEXTURE(Texture2D, SkyboxTexture)
		SHADER_PARAMETER_SAMPLER(SamplerState, SkyboxTextureSampler)
		SHADER_PARAMETER(FIntPoint, Dimensions)
		SHADER_PARAMETER(FMatrix, CameraToWorld)
		SHADER_PARAMETER(FMatrix, CameraInverseProjection)
		SHADER_PARAMETER(FVector4, Colour)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, SphereBuffer)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float2>, RandomBuffer)
	END_SHADER_PARAMETER_STRUCT()

	//Called by the engine to determine which permutations to compile for this shader
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	//Modifies the compilations environment of the shader
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		//We're using it here to add some preprocessor defines. That way we don't have to change both C++ and HLSL code when we change the value for NUM_THREADS_PER_GROUP_DIMENSION
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
		OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
	}
};
