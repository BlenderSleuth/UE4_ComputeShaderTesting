// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShaderTestingGameMode.generated.h"

UCLASS(minimalapi)
class AShaderTestingGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShaderTestingGameMode();

	virtual void BeginPlay() override;
};



