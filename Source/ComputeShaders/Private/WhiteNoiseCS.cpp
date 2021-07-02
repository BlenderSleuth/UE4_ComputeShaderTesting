// Copyright Ben Sutherland 2021. All rights reserved.

#include "WhiteNoiseCS.h"

#include "GlobalShader.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "ShaderHelpers.h"
#include "ShaderParameterStruct.h"


class FWhiteNoiseCS : public FGlobalShader
{
public:
	// Check out ShaderParametersStruct: L20
	DECLARE_GLOBAL_SHADER(FWhiteNoiseCS);
	SHADER_USE_PARAMETER_STRUCT(FWhiteNoiseCS, FGlobalShader);

	// Shader I/O
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, OutputTexture)
		SHADER_PARAMETER(FIntPoint, Dimensions)
		SHADER_PARAMETER(uint32, TimeStamp)
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

//                      Shader Class            Shader Virtual Path       HLSL main function name    Type
IMPLEMENT_GLOBAL_SHADER(FWhiteNoiseCS, "/ComputeShaders/WhiteNoiseCS.usf",		"MainCS",			SF_Compute)


void FWhiteNoiseCSManager::BeginRendering()
{
	bEnableRendering = true;
}

void FWhiteNoiseCSManager::EndRendering()
{
	bEnableRendering = false;
}

void FWhiteNoiseCSManager::UpdateParameters(FWhiteNoiseCSParameters& DrawParameters)
{
	CachedParams = DrawParameters;
	bCachedParamsAreValid = true;
}

void FWhiteNoiseCSManager::Tick_RenderThread(FRHICommandListImmediate& RHICmdList) const
{
	// Make sure we're running in the render thread
	check(IsInRenderingThread());
	
	// Early out if invalid
	if (!bEnableRendering || !bCachedParamsAreValid || !CachedParams.RenderTarget)
	{
		return;
	}

	FRDGBuilder GraphBuilder(RHICmdList);

	// Create a RDG Texture
	const FRDGTextureDesc RenderTargetDesc = FRDGTextureDesc::Create2D(
		FIntPoint(1024,1024),
		PF_R32_FLOAT,
		FClearValueBinding::Black,
		TexCreate_RenderTargetable| TexCreate_ShaderResource | TexCreate_UAV
	);
	FRDGTexture* RenderTargetTex = GraphBuilder.CreateTexture(RenderTargetDesc, TEXT("ComputeRenderTarget"));

	// Create a UAV
	FRDGTextureUAV* RenderTargetUAV = GraphBuilder.CreateUAV(RenderTargetTex);

	// Set shader parameters
	FWhiteNoiseCS::FParameters* ShaderParameters = GraphBuilder.AllocParameters<FWhiteNoiseCS::FParameters>();
	ShaderParameters->OutputTexture = RenderTargetUAV;
	ShaderParameters->Dimensions = CachedParams.CachedRenderTargetSize;
	ShaderParameters->TimeStamp = CachedParams.TimeStamp;

	const TShaderMapRef<FWhiteNoiseCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	const FIntVector GroupCount = CachedParams.GetGroupCount();

	// Utility to actually run ("Dispatch") the compute shader
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("WhiteNoise Compute Shader"),
		ComputeShader,
		ShaderParameters,
		GroupCount
	);

	AddReadbackTexturePass(GraphBuilder, TEXT("RenderTarget"), RenderTargetTex, CachedParams.RenderTarget->GetRenderTargetResource()->TextureRHI);

	GraphBuilder.Execute();
}
