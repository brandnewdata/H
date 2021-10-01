// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Source\H1\H1.h"
#include "UObject/NoExportTypes.h"
#include "H1InventoryItem.generated.h"


enum class EItemEquipSlot : uint8; // ������ �������� ����ü.
struct FItemTableRow; // ������ DT�� ����ü

/**
 * 
 */
UCLASS()
class UH1InventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	// �� �κ��丮 �������� ����µ� ����� DT ���� ������ ���� ��ȯ�Ѵ�.
	const FItemTableRow* GetItemTableRow();
	EItemEquipSlot GetItemEquipSlot();

	// �⺻������ �������� �ּ� 1���� �־�� �����ϴϱ�.
	void SetItemInfo(int NewItemClassID, int NewStackCount = 1);
	void CleanUp();

public:
	UPROPERTY()
	int ItemClassID = 0; // �������� ID
	UPROPERTY()
	bool Empty = true; // �����?
	UPROPERTY()
	bool Equipped = false; // ���������� �������ΰ�?
	UPROPERTY()
	int32 StackCount = 1; // �������� � ¥���ΰ�?
private:
	// �� �κ��丮 �������� ����µ� ����� DT ���� ������ ��.
	const FItemTableRow* ItemTableRow = nullptr;
};
