// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseActor.h"

#include "WhiteNoiseCS.h"

// Sets default values
ANoiseActor::ANoiseActor():
	WhiteNoiseManager(new FWhiteNoiseCSManager())
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
	
	TimeStamp = 0;
}

// Called when the game starts or when spawned
void ANoiseActor::BeginPlay()
{
	Super::BeginPlay();

	WhiteNoiseManager->BeginRendering();
	
	UMaterialInstanceDynamic* MID = StaticMesh->CreateAndSetMaterialInstanceDynamic(0);
	MID->SetTextureParameterValue("InputTexture", RenderTarget);
}

void ANoiseActor::BeginDestroy()
{
	WhiteNoiseManager->EndRendering();
	
	Super::BeginDestroy();
}

// Called every frame
void ANoiseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update parameters
	FWhiteNoiseCSParameters Parameters(RenderTarget);
	Parameters.TimeStamp = TimeStamp;
	WhiteNoiseManager->UpdateParameters(Parameters);
	
	TimeStamp++;
	if (TimeStamp > 100000)
	{
		TimeStamp = 0;
	}
}

