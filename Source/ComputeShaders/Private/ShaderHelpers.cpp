// Copyright Ben Sutherland 2021. All rights reserved.

#include "ShaderHelpers.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"


void AddReadbackTexturePass(FRDGBuilder& GraphBuilder, const TCHAR* Name, const FRDGTextureRef SrcTexture, FTextureRHIRef DestTextureRHI, const FRHICopyTextureInfo& CopyInfo)
{
	AddReadbackTexturePass(
		GraphBuilder,
		RDG_EVENT_NAME("ReadbackTexturePass(%s)", Name),
		SrcTexture,
		[SrcTexture, DestTextureRHI, CopyInfo](FRHICommandList& RHICmdList)
	{
		if (DestTextureRHI)
		{
			RHICmdList.CopyTexture(
				SrcTexture->GetRHI(),
				DestTextureRHI,
				CopyInfo
			);
		}
	});
}

BEGIN_SHADER_PARAMETER_STRUCT(FReadbackStructuredBufferParameters, )
	RDG_BUFFER_ACCESS(Buffer, ERHIAccess::CopySrc)
END_SHADER_PARAMETER_STRUCT()

void AddReadbackStructuredBufferPass(FRDGBuilder& GraphBuilder, const FRDGBufferRef SrcBuffer, void* DestBufferPtr, const uint32 BufferSize)
{
	FReadbackStructuredBufferParameters* PassParameters = GraphBuilder.AllocParameters<FReadbackStructuredBufferParameters>();
	PassParameters->Buffer = SrcBuffer;
	
	GraphBuilder.AddPass(
		RDG_EVENT_NAME("StructuredBufferReadback(%s)", SrcBuffer->Name),
		PassParameters,
		ERDGPassFlags::Readback,
	[SrcBuffer, DestBufferPtr, BufferSize](FRHICommandListImmediate& RHICmdList)
	{
		FRHIStructuredBuffer* StructuredBuffer = SrcBuffer->GetRHIStructuredBuffer();
		void* SrcPtr = RHICmdList.LockStructuredBuffer(StructuredBuffer, 0, BufferSize, RLM_ReadOnly);
		FMemory::Memcpy(DestBufferPtr, SrcPtr, BufferSize);
		RHICmdList.UnlockStructuredBuffer(StructuredBuffer);
	});
}

void FRenderTickHelper::GameThread_Register()
{
	// Register on the render thread
	ENQUEUE_RENDER_COMMAND(CreateRenderTickHelper)([this](FRHICommandList& RHICmdList)
	{
		this->Register();
	});
	
	// Make sure it gets done before we return
	FlushRenderingCommands();
}

void FRenderTickHelper::GameThread_Unregister()
{
	// Register on the render thread
	ENQUEUE_RENDER_COMMAND(CreateRenderTickHelper)([this](FRHICommandList& RHICmdList)
	{
		this->Unregister();
	});
	
	// Make sure it gets done before we return
	FlushRenderingCommands();
}
