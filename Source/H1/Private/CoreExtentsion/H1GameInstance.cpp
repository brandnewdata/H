// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreExtentsion/H1GameInstance.h"
#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "CoreExtentsion/H1TableManager.h"

UH1TableManager* UH1GameInstance::TableManager = nullptr;

UH1TableManager* UH1GameInstance::GetTableManager()
{
	if (!IsValid(TableManager))
	{
		TableManager = NewObject<UH1TableManager>(GetTransientPackage(), UH1TableManager::StaticClass());
		TableManager->AddToRoot();
	}

	return TableManager;
}

UH1GameInstance* UH1GameInstance::Get(AActor* Actor)
{
	UWorld* World = GEngine->GetWorldFromContextObject(Actor->GetWorld(), EGetWorldErrorMode::LogAndReturnNull);
	return World ? Cast<UH1GameInstance>(World->GetGameInstance()) : nullptr;
}