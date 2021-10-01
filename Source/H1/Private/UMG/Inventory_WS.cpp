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

	// 8개가 차있는거 CDO생성이랑 연관있는 버그 같은데...
	InventorySlots.Empty();

	//변수명과 리플렉션 기능을 이용해서 아래처럼 처리도 가능
	 //현재 클래스에 포함된 속성들을 리스트의 반복자를 가져옴.
	 //파생 클래스인 BP 클래스에서 슬롯의 개수가 정해지기 때문에 이러한 과정이 필요함.
	for (TFieldIterator<FObjectProperty> PropIt(GetClass()); PropIt; ++PropIt)
	{
		// 반복자로부터 포인터를 받아서 다시 포인팅함. 안 하면 코드가 길어져서 이렇게함.)
		FObjectProperty* Property = *PropIt;

		// isvalid 사용 불가. // 속성이 유효하다면
		if (Property != nullptr)
		{
			// 실제 속성 객체가 있는 포인터  값을 받아옴
			UObject* TestSlotObject = Property->GetObjectPropertyValue_InContainer(this);
			// 지금 필요한 속성으로 형변환 . 실패하면 원하던 속성이 아닌 것. 아무일 없이 검증단계에서 종료
			UInventorySlot_WS* WidgetSlot = Cast<UInventorySlot_WS>(TestSlotObject);

			// isvalid 사용 불가. 검증이 성공하면 값이 nullptr이 아니고 유효함.
			if (WidgetSlot != nullptr)
			{
				// 인벤토리의 슬롯배열에 슬롯을 추가함.
				InventorySlots.Add(WidgetSlot);
			}
		}
	}
// 현재 클래스에 포함된 하위 속성들을 이름으로 찾아와서 반복문으로 아래 처리를 대체 할 수 있음.
//	UObjectProperty* ObjProperty = FindField<UObjectProperty>(GetClass(), FName(TEXT("[변수명]")));
// 반복문 없이 그냥 하드코딩함.
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
	// 인벤토리 열고 닫기
	Showing = !Showing;

	// 열고 닫기 애니메이션 처리
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

	// 지금 인벤토리에 소속되어 있는  슬롯중에서 비어있는 슬롯을 가져옴.(당연히 비어있는 중에 인덱스가 가장 빠른것이 옴)
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
	// TODO : 인벤토리 슬롯범위 변수화 해야함. 인벤토리 슬롯크기를 파생클래스에서 받아오는 초기화 구문을 추가할 것.
	// 요구받은 인벤토리 슬롯의 인덱스가 유효하다면 해당 인벤토리 슬롯의 포인터를 반환한다.
	if (index < 0 || index >= 8)
		return nullptr;

	return InventorySlots[index];
}

int UInventory_WS::GetSlotIndex(UInventorySlot_WS* InventorySlot)
{
	// 현재 슬롯의 포인터로 슬롯의 인덱스를 반환함.
	// TArray에서의 인덱스임. 
	return InventorySlots.Find(InventorySlot);
}