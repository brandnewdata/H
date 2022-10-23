// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Source\H1\H1.h"
#include "UObject/NoExportTypes.h"
#include "H1InventoryItem.generated.h"


enum class EItemEquipSlot : uint8; // 아이템 장착슬롯 열거체.
struct FItemTableRow; // 아이템 DT행 구조체

/**
 * 
 */
UCLASS()
class UH1InventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	// 이 인벤토리 아이템을 만드는데 사용한 DT 행의 포인터 값을 반환한다.
	const FItemTableRow* GetItemTableRow();
	EItemEquipSlot GetItemEquipSlot();

	// 기본적으로 아이템은 최소 1개가 있어야 존재하니까.
	void SetItemInfo(int NewItemClassID, int NewStackCount = 1);
	void CleanUp();

public:
	UPROPERTY()
	int ItemClassID = 0; // 아이템의 ID
	UPROPERTY()
	bool Empty = true; // 비었나?
	UPROPERTY()
	bool Equipped = false; // 장착가능한 아이템인가?
	UPROPERTY()
	int32 StackCount = 1; // 아이템이 몇개 짜리인가?
private:
	// 이 인벤토리 아이템을 만드는데 사용한 DT 행의 포인터 값.
	const FItemTableRow* ItemTableRow = nullptr;
};
