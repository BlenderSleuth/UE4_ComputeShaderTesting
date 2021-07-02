// Copyright Ben Sutherland 2021. All rights reserved.

#pragma once

#include "CoreMinimal.h"

#include "ComputeShaders.h"
#include "GameFramework/Actor.h"
#include "RayTracingManager.generated.h"

class UTextureRenderTarget2D;
class UCameraComponent;

struct FRayTracingParams
{
	FMatrix CameraToWorldMat;
	FMatrix CameraInverseProjection;
	FIntPoint TexSize;
	EPixelFormat PixelFormat;
	TArray<FVector4> SphereBuffer;
	
	FIntVector GetGroupCount() const
	{
		return FIntVector(
			FMath::DivideAndRoundUp(TexSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
			FMath::DivideAndRoundUp(TexSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
			1
		);
	}
};

UCLASS(ClassGroup=(RayTracing))
class COMPUTESHADERS_API ARayTracingManager : public AActor
{
	GENERATED_BODY()
	
public:
	ARayTracingManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RayTracing)
	FLinearColor Colour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RayTracing)
	int32 NumAASamples;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RayTracing)
	UTexture2D* SkyboxTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RayTracing)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RayTracing)
	UCameraComponent* Camera;

	virtual void BeginPlay() override;
	
private:
	void Render();
	
	FRayTracingParams Params;

	// Render run from RenderThread
	void Execute_RenderThread(FRHICommandListImmediate& RHICmdList);
};
