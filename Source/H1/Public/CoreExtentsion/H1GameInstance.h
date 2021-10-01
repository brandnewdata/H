// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Item/H1ItemTableDef.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "H1GameInstance.generated.h"

/**
 * 
 */
class UH1TableManager;

UCLASS()
class H1_API UH1GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	static UH1GameInstance* Get(class AActor* Actor);
	static UH1TableManager* GetTableManager();

private:
	static UH1TableManager* TableManager;
};
