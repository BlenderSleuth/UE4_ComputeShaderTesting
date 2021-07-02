// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShaderTestingGameMode.h"
#include "ShaderTestingCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShaderTestingGameMode::AShaderTestingGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AShaderTestingGameMode::BeginPlay()
{
}
