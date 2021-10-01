// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/H1InventoryItem.h"
#include "Item/H1ItemTableDef.h"
#include "CoreExtentsion/H1GameInstance.h"
#include "CoreExtentsion/H1TableManager.h"

const FItemTableRow* UH1InventoryItem::GetItemTableRow()
{
	// ������ SetItemInfo�ʿ��� �� ó���մϴ�.
	// 
	// 	UTableManager* TableManager = Upractice_umg_GameInstance::GetTableManager();
	// 	if (!IsValid(TableManager) || 0 == ItemClassID )
	// 	{
	// 		return nullptr;
	// 	}
	// 
	// 	const FItemTableRow* ItemTableRow = TableManager->GetTable<FItemTableRow>(ItemClassID);
	return ItemTableRow;
}

EItemEquipSlot UH1InventoryItem::GetItemEquipSlot()
{
	UH1TableManager* TableManager = UH1GameInstance::GetTableManager();
	if (!IsValid(TableManager) || 0 == ItemClassID)
		return EItemEquipSlot::Unknown;

	const FItemTableRow* ItemTable = TableManager->GetTable<FItemTableRow>(ItemClassID);

	if (nullptr == ItemTable)
		return EItemEquipSlot::Unknown;

	return ItemTable->ItemEquipSlot;
}

void UH1InventoryItem::SetItemInfo(int NewItemClassID, int NewStackCount/* = 1*/)
{
	ItemClassID = NewItemClassID;
	StackCount = NewStackCount;

	UH1TableManager* TableManager = UH1GameInstance::GetTableManager();
	if (IsValid(TableManager) && 0 != ItemClassID)
	{
		ItemTableRow = TableManager->GetTable<FItemTableRow>(ItemClassID);
	}

	// ���� ��ĭ�� �ƴ�.
	Empty = false;
	// ������ ����!
	Equipped = false;
}

void UH1InventoryItem::CleanUp()
{
	// ���� ��ĭ�̴�.
	ItemClassID = 0;
	StackCount = 0;
	ItemTableRow = nullptr;
	Empty = true;
	Equipped = false;
}