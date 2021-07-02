// Copyright Ben Sutherland 2021. All rights reserved.


#pragma once

// Helper function to copy a texture back from the GPU
void AddReadbackTexturePass(FRDGBuilder& GraphBuilder, const TCHAR* Name, const FRDGTextureRef SrcTexture, FTextureRHIRef DestTextureRHI, const FRHICopyTextureInfo& CopyInfo = FRHICopyTextureInfo());

// Helper function to copy a StructuredBuffer back from the GPU
void AddReadbackStructuredBufferPass(FRDGBuilder& GraphBuilder, const FRDGBufferRef SrcBuffer, void* DestBufferPtr, const uint32 BufferSize);

 	
DECLARE_DELEGATE_OneParam(FRenderTickDelegate, FRHICommandListImmediate&)

// Helper class to allow ticking on the render thread
class COMPUTESHADERS_API FRenderTickHelper final : public FTickableObjectRenderThread
{
public:
	explicit FRenderTickHelper(const bool bInHighFrequency):
		FTickableObjectRenderThread(false, bInHighFrequency)
	{}

	// Register from Game Thread
	void GameThread_Register();
	// Unregister from Game Thread
	void GameThread_Unregister();

	virtual void Tick(const float DeltaTime) override
	{
		// In render thread
		if (TickImplementation.IsBound())
		{
			TickImplementation.Execute(GetImmediateCommandList_ForRenderCommand());
		} else
		{
			Unregister();
		}
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FRenderTickHelper, STATGROUP_Tickables);
	}

	virtual bool IsTickable() const override { return true; }
	
	FRenderTickDelegate TickImplementation;
};