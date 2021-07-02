// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "WhiteNoiseCS.h"
#include "GameFramework/Actor.h"
#include "NoiseActor.generated.h"

UCLASS()
class SHADERTESTING_API ANoiseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoiseActor();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShaderDemo)
	class UTextureRenderTarget2D* RenderTarget;

	TUniquePtr<FWhiteNoiseCSManager> WhiteNoiseManager;
	
	uint32 TimeStamp;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
