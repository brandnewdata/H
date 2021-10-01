// Fill out your copyright notice in the Description page of Project Settings.

#include "UMG/Inventory_WS.h"
#include "UMG/InventorySlot_WS.h"



UClass* UInventory_WS::GetWidgetClassStatic()
{
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/Item_System_UMG/Inventory_WB.Inventory_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

void UInventory_WS::NativeConstruct()
{
	Super::NativeConstruct();

	UWidgetAnimation** InitAnimation = AnimationList.Find(FName(TEXT("InitLocation")));
	if (nullptr != InitAnimation)
	{
		PlayAnimation(*InitAnimation);
	}
}

bool UInventory_WS::Initialize()
{
	Super::Initialize();

	// 8���� ���ִ°� CDO�����̶� �����ִ� ���� ������...
	InventorySlots.Empty();

	//������� ���÷��� ����� �̿��ؼ� �Ʒ�ó�� ó���� ����
	 //���� Ŭ������ ���Ե� �Ӽ����� ����Ʈ�� �ݺ��ڸ� ������.
	 //�Ļ� Ŭ������ BP Ŭ�������� ������ ������ �������� ������ �̷��� ������ �ʿ���.
	for (TFieldIterator<FObjectProperty> PropIt(GetClass()); PropIt; ++PropIt)
	{
		// �ݺ��ڷκ��� �����͸� �޾Ƽ� �ٽ� ��������. �� �ϸ� �ڵ尡 ������� �̷�����.)
		FObjectProperty* Property = *PropIt;

		// isvalid ��� �Ұ�. // �Ӽ��� ��ȿ�ϴٸ�
		if (Property != nullptr)
		{
			// ���� �Ӽ� ��ü�� �ִ� ������  ���� �޾ƿ�
			UObject* TestSlotObject = Property->GetObjectPropertyValue_InContainer(this);
			// ���� �ʿ��� �Ӽ����� ����ȯ . �����ϸ� ���ϴ� �Ӽ��� �ƴ� ��. �ƹ��� ���� �����ܰ迡�� ����
			UInventorySlot_WS* WidgetSlot = Cast<UInventorySlot_WS>(TestSlotObject);

			// isvalid ��� �Ұ�. ������ �����ϸ� ���� nullptr�� �ƴϰ� ��ȿ��.
			if (WidgetSlot != nullptr)
			{
				// �κ��丮�� ���Թ迭�� ������ �߰���.
				InventorySlots.Add(WidgetSlot);
			}
		}
	}
// ���� Ŭ������ ���Ե� ���� �Ӽ����� �̸����� ã�ƿͼ� �ݺ������� �Ʒ� ó���� ��ü �� �� ����.
//	UObjectProperty* ObjProperty = FindField<UObjectProperty>(GetClass(), FName(TEXT("[������]")));
// �ݺ��� ���� �׳� �ϵ��ڵ���.
	//InventorySlots.Add(InventorySlot_0);
	//InventorySlot_0->SlotIndex = 0;

	//InventorySlots.Add(InventorySlot_1);
	//InventorySlot_1->SlotIndex = 1;

	//InventorySlots.Add(InventorySlot_2);
	//InventorySlot_2->SlotIndex = 2;

	//InventorySlots.Add(InventorySlot_3);
	//InventorySlot_3->SlotIndex = 3;

	//InventorySlots.Add(InventorySlot_4);
	//InventorySlot_4->SlotIndex = 4;

	//InventorySlots.Add(InventorySlot_5);
	//InventorySlot_5->SlotIndex = 5;

	//InventorySlots.Add(InventorySlot_6);
	//InventorySlot_6->SlotIndex = 6;

	//InventorySlots.Add(InventorySlot_7);
	//InventorySlot_7->SlotIndex = 7;

	return true;
}

void UInventory_WS::ToggleInventory()
{
	// �κ��丮 ���� �ݱ�
	Showing = !Showing;

	// ���� �ݱ� �ִϸ��̼� ó��
	UWidgetAnimation** ShowAnimation = AnimationList.Find(FName(TEXT("Showing")));
	if (nullptr == ShowAnimation)
		return;

	if (Showing)
	{
		if (IsPlayingAnimation())
		{
			ReverseAnimation(*ShowAnimation);
		}
		else
		{
			PlayAnimation(*ShowAnimation);
		}
	}
	else
	{
		if (IsPlayingAnimation())
		{
			ReverseAnimation(*ShowAnimation);
		}
		else
		{
			PlayAnimation(*ShowAnimation,0.0f,1,EUMGSequencePlayMode::Reverse);
		}
	}
}

UInventorySlot_WS* UInventory_WS::GetEmptySlot()
{
	UInventorySlot_WS* FindSlot = nullptr;

	// ���� �κ��丮�� �ҼӵǾ� �ִ�  �����߿��� ����ִ� ������ ������.(�翬�� ����ִ� �߿� �ε����� ���� �������� ��)
	for (auto slot : InventorySlots)
	{
		if (slot->GetIsEmpty())
		{
			FindSlot = slot;
			break;
		}
	}

	return FindSlot;
}

UInventorySlot_WS* UInventory_WS::GetSlot(int index)
{
	// TODO : �κ��丮 ���Թ��� ����ȭ �ؾ���. �κ��丮 ����ũ�⸦ �Ļ�Ŭ�������� �޾ƿ��� �ʱ�ȭ ������ �߰��� ��.
	// �䱸���� �κ��丮 ������ �ε����� ��ȿ�ϴٸ� �ش� �κ��丮 ������ �����͸� ��ȯ�Ѵ�.
	if (index < 0 || index >= 8)
		return nullptr;

	return InventorySlots[index];
}

int UInventory_WS::GetSlotIndex(UInventorySlot_WS* InventorySlot)
{
	// ���� ������ �����ͷ� ������ �ε����� ��ȯ��.
	// TArray������ �ε�����. 
	return InventorySlots.Find(InventorySlot);
}