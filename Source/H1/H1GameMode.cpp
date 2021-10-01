// Copyright Epic Games, Inc. All Rights Reserved.

#include "H1GameMode.h"
#include "H1PlayerController.h"
#include "H1Character.h"
#include "H1Pawn.h"
#include "UObject/ConstructorHelpers.h"

AH1GameMode::AH1GameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	//if (PlayerPawnBPClass.Class != nullptr)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}

	DefaultPawnClass = AH1Character::StaticClass();
	
	// use our custom PlayerController class
	PlayerControllerClass = AH1PlayerController::StaticClass();
}

void AH1GameMode::PostLogin(APlayerController* NewPlayer)
{
	H1LOG(Warning, TEXT("PostLogin Begin"));
	Super::PostLogin(NewPlayer);
	H1LOG(Warning, TEXT("PostLogin End"));
	
}

