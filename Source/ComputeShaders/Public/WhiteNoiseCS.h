// Copyright Ben Sutherland 2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComputeShaders.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ShaderHelpers.h"

struct FWhiteNoiseCSParameters
{
	UTextureRenderTarget2D* RenderTarget;
	FIntPoint CachedRenderTargetSize;
	uint32 TimeStamp;

	FWhiteNoiseCSParameters() { }

	explicit FWhiteNoiseCSParameters(UTextureRenderTarget2D* InRenderTarget):
		RenderTarget(InRenderTarget),
		CachedRenderTargetSize(
			RenderTarget ? FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY) : FIntPoint::ZeroValue
		),
		TimeStamp(0)
	{}

	FIntVector GetGroupCount() const
	{
		return FIntVector(
			FMath::DivideAndRoundUp(CachedRenderTargetSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
			FMath::DivideAndRoundUp(CachedRenderTargetSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
			1
		);
	}
};

class COMPUTESHADERS_API FWhiteNoiseCSManager
{
public:
	FWhiteNoiseCSManager():
		TickHelper(new FRenderTickHelper(false)),
		bCachedParamsAreValid(false)
	{
		TickHelper->TickImplementation.BindRaw(this, &FWhiteNoiseCSManager::Tick_RenderThread);
		TickHelper->GameThread_Register();
	}
	~FWhiteNoiseCSManager()
	{
		TickHelper->TickImplementation.Unbind();
		TickHelper->GameThread_Unregister();
	}
	
	// Call this when you want to start executing the compute shader. The shader will be dispatched once per frame.
	void BeginRendering();

	// Stops compute shader execution
	void EndRendering();

	// Call this whenever you have new parameters, on any thread
	void UpdateParameters(FWhiteNoiseCSParameters& DrawParameters);

	// Called on the render thread to dispatch compute shader
	void Tick_RenderThread(FRHICommandListImmediate& RHICmdList) const;
	
private:
	TUniquePtr<FRenderTickHelper> TickHelper;

	// Cached Shader Manager Parameters
	FWhiteNoiseCSParameters CachedParams;

	// Whether we have cached parameters to pass to the shader or not
	FThreadSafeBool bCachedParamsAreValid;

	// Whether the shader should execute each frame
	FThreadSafeBool bEnableRendering;
};