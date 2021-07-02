// Copyright Ben Sutherland 2021. All rights reserved.

#include "RayTracingManager.h"

#include "EngineUtils.h"
#include "RayTracingCS.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ShaderHelpers.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/TextureCube.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"


ARayTracingManager::ARayTracingManager()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	RootComponent = Camera;
}

void ARayTracingManager::BeginPlay()
{
	Super::BeginPlay();
	Render();
}

void ARayTracingManager::Render()
{
	if (Camera == nullptr || RenderTarget == nullptr || SkyboxTexture == nullptr)
	{
		printw("NULL Camera, RenderTarget or SkyboxTexture")
		return;
	}

	// Save params
	Params.TexSize = FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY);
	const float AspectRatio = static_cast<float>(Params.TexSize.X) / static_cast<float>(Params.TexSize.Y);
	
	// Get Camera Settings
	FMinimalViewInfo ViewInfo; 
	Camera->GetCameraView(0.f, ViewInfo);

	FMatrix ProjectionMatrix = FReversedZPerspectiveMatrix(
		FMath::Max(0.001f, ViewInfo.FOV) * PI / 360.0f,
		AspectRatio,
		1.0f,
		1
	);

	FMatrix ViewMatrix, ViewProjectionMatrix;
	UGameplayStatics::CalculateViewProjectionMatricesFromMinimalView(
		ViewInfo,
		ProjectionMatrix,
		ViewMatrix,
		ProjectionMatrix,
		ViewProjectionMatrix
	);
	
	Params.CameraToWorldMat = ViewMatrix.Inverse();
	Params.CameraInverseProjection = ProjectionMatrix;

	// Copy over all the spheres in the scene by name
	for (TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It)
	{
		if (It->GetName().Contains(TEXT("Sphere")))
		{
			Params.SphereBuffer.Emplace(It->GetActorLocation(), It->GetActorScale().Z * 50.f);
		}
	}

	// Make sure we have at least one sphere
	if (Params.SphereBuffer.Num() == 0)
	{
		Params.SphereBuffer.Emplace(0.f, 0.f, 50.f, 50.f);
	}
	
	Params.PixelFormat = GetPixelFormatFromRenderTargetFormat(RenderTarget->RenderTargetFormat);
	
	ENQUEUE_RENDER_COMMAND(RunComputeShader)([this](FRHICommandListImmediate& RHICmdList)
	{
		this->Execute_RenderThread(RHICmdList);
	});
}


void ARayTracingManager::Execute_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	// Only execute from render thread
	check(IsInRenderingThread());

	FRDGBuilder GraphBuilder(RHICmdList);

	// Create the Sphere Buffer and copy over elements
	const uint32 SphereBufferSize = Params.SphereBuffer.Num() * Params.SphereBuffer.GetTypeSize();
	
	const FRDGBufferRef SphereBuffer = CreateStructuredBuffer(
		GraphBuilder,
		TEXT("SphereBuffer"),
		Params.SphereBuffer.GetTypeSize(),
		Params.SphereBuffer.Num(),
		Params.SphereBuffer.GetData(),
		SphereBufferSize,
		ERDGInitialDataFlags::None // Possible to skip the copy if you can guarantee the lifetime of the data
	);

	const FRDGBufferSRVRef SphereBufferSRV = GraphBuilder.CreateSRV(SphereBuffer);
	
	// Create the random buffer (for antialiasing)
	TArray<FVector2D> RandomBufferData;
	const int32 NumSamples = FMath::Max(1, NumAASamples);
	RandomBufferData.Reserve(NumSamples);
	for (int32 Sample = 0; Sample < NumSamples; Sample++)
	{
		RandomBufferData.Emplace(FMath::FRand(), FMath::FRand());
	}
	const uint32 RandBufferSize = RandomBufferData.Num() * RandomBufferData.GetTypeSize();
	
	const FRDGBufferRef RandomBuffer = CreateStructuredBuffer(
		GraphBuilder,
		TEXT("RandomBuffer"),
		RandomBufferData.GetTypeSize(),
		NumSamples,
		RandomBufferData.GetData(),
		RandBufferSize,
		ERDGInitialDataFlags::None
	);

	const FRDGBufferUAVRef RandomBufferUAV = GraphBuilder.CreateUAV(RandomBuffer);
	
	// Create the RenderTarget Texture
	const FRDGTextureDesc RenderTargetDesc = FRDGTextureDesc::Create2D(
		Params.TexSize,
		Params.PixelFormat,
		FClearValueBinding::Black,
		TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV
	);
	const FRDGTextureRef RenderTargetTex = GraphBuilder.CreateTexture(RenderTargetDesc, TEXT("RayTracingRenderTarget"));

	// Create a Render Target UAV
	FRDGTextureUAV* RenderTargetUAV = GraphBuilder.CreateUAV(RenderTargetTex);
	
	// Set shader parameters
	FRayTracingCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FRayTracingCS::FParameters>();
	PassParameters->OutputTexture = RenderTargetUAV;
	PassParameters->SkyboxTexture = SkyboxTexture->Resource->TextureRHI;
	PassParameters->SkyboxTextureSampler = TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap>::CreateRHI();
	PassParameters->Dimensions = Params.TexSize;
	PassParameters->CameraToWorld = Params.CameraToWorldMat;
	PassParameters->CameraInverseProjection = Params.CameraInverseProjection;
	PassParameters->Colour = Colour;
	PassParameters->SphereBuffer = SphereBufferSRV;
	PassParameters->RandomBuffer = RandomBufferUAV;

	const TShaderMapRef<FRayTracingCS> RayTracingShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	
	// Utility to actually run ("Dispatch") the compute shader
	FComputeShaderUtils::AddPass(
		GraphBuilder,
		RDG_EVENT_NAME("RayTracing Compute Shader"),
		RayTracingShader,
		PassParameters,
		Params.GetGroupCount()
	);

	// Get resulting texture out of the GPU
	AddReadbackTexturePass(GraphBuilder, TEXT("RenderTarget"), RenderTargetTex, RenderTarget->GetRenderTargetResource()->TextureRHI);
	
	// Get buffer data back out of the GPU
	TArray<FVector2D> RandomBufferOut;
	RandomBufferOut.SetNumUninitialized(NumSamples);
	AddReadbackStructuredBufferPass(GraphBuilder, RandomBuffer, RandomBufferOut.GetData(), RandBufferSize);

	GraphBuilder.Execute();
}
