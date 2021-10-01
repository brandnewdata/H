// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "..\Source\H1\H1.h"
#include "GameFramework/GameModeBase.h"
#include "H1GameMode.generated.h"

UCLASS(minimalapi)
class AH1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AH1GameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
};



