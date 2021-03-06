// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "H1ItemTableDef.generated.h"

/**
 * 
 */
UCLASS()
class UH1ItemTableDef : public UObject
{
	GENERATED_BODY()
	
};

UENUM(BlueprintType)
enum class EItemEquipSlot : uint8
{
	Unknown,
	WeaponBack,
	WeaponLeftHand,
	WeaponRightHand,
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Equip,
	Consume,
};

UENUM(BlueprintType)
enum class EMeshType : uint8
{
	StaticMesh,
	SkeletalMesh,
};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FItemTableRow() {}

	static FName GetTableAssetPath() { return TEXT("DataTable'/Game/H1/Item/H1ItemTable.H1ItemTable'"); }
	// 	static ETableType GetTableType() { return ETableType::TablePathDef; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	int ItemClassID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	EItemEquipSlot ItemEquipSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	EMeshType MeshType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	float ItemFloat1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	FSoftObjectPath ItemModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	FSoftObjectPath ItemThumb;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	FSoftObjectPath TestItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemTable)
	FString ItemDesc;
};