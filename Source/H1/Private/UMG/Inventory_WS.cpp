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

	//// 8개가 차있는거 CDO생성이랑 연관있는 버그 같은데...
	//InventorySlots.Empty();

	////변수명과 리플렉션 기능을 이용해서 아래처럼 처리도 가능
	// //현재 클래스에 포함된 속성들을 리스트의 반복자를 가져옴.
	// //파생 클래스인 BP 클래스에서 슬롯의 개수가 정해지기 때문에 이러한 과정이 필요함.
	//for (TFieldIterator<FObjectProperty> PropIt(GetClass()); PropIt; ++PropIt)
	//{
	//	// 반복자로부터 포인터를 받아서 다시 포인팅함. 안 하면 코드가 길어져서 이렇게함.)
	//	FObjectProperty* Property = *PropIt;

	//	// isvalid 사용 불가. // 속성이 유효하다면
	//	if (Property != nullptr)
	//	{
	//		// 실제 속성 객체가 있는 포인터  값을 받아옴
	//		UObject* TestSlotObject = Property->GetObjectPropertyValue_InContainer(this);
	//		// 지금 필요한 속성으로 형변환 . 실패하면 원하던 속성이 아닌 것. 아무일 없이 검증단계에서 종료
	//		UInventorySlot_WS* WidgetSlot = Cast<UInventorySlot_WS>(TestSlotObject);

	//		// isvalid 사용 불가. 검증이 성공하면 값이 nullptr이 아니고 유효함.
	//		if (WidgetSlot != nullptr)
	//		{
	//			// 인벤토리의 슬롯배열에 슬롯을 추가함.
	//			InventorySlots.Add(WidgetSlot);
	//		}
	//	}
	//}

// 현재 클래스에 포함된 하위 속성들을 이름으로 찾아와서 반복문으로 아래 처리를 대체 할 수 있음.
//	UObjectProperty* ObjProperty = FindField<UObjectProperty>(GetClass(), FName(TEXT("[변수명]")));
// 반복문 없이 그냥 하드코딩함.
// TODO : 알수 없는 버그 도대체 이거 CDO에서 불리는거 아니면 설명이 안됨 여기 말고 이 배열을 건드리는 코드가 없는데 정확하게 8개가 먼저 들어가 있음..;;
// 심지어 배열 이름 앞에 array_를 붙이니까 8개로 잡히는 기현상이 사라짐 
// 심각함.
// CDO를 파일로 만들어서 가지고 있다가 여러 엔진 버젼에서 공유하거나 하는 기상천외한 경우가 아니고는 CDO에서 이 함수가 호출되서 발생한다고 생각하는 것이..
// 그런것 치고는 flag값은 여전히 이 함수가 1번만 호출되고 있음.
// 결국 최악의 경우인 어디서인가 메모리가 누수되어서 오버플로우로 메모리가 덮어씌어 졌고 우연히 인벤토리 사이즈인 8로 덮인 것...?
// 이 경우는 메모리 디버거 살펴봐야 할 것 같다.
	//array_InventorySlots[0] = InventorySlot_0;
	//array_InventorySlots[1] = InventorySlot_1;
	//array_InventorySlots[2] = InventorySlot_2;
	//array_InventorySlots[3] = InventorySlot_3;
	//array_InventorySlots[4] = InventorySlot_4;
	//array_InventorySlots[5] = InventorySlot_5;
	//array_InventorySlots[6] = InventorySlot_6;
	//array_InventorySlots[7] = InventorySlot_7;

	flag += 2;
	array_InventorySlots.Add(InventorySlot_0);
	array_InventorySlots.Add(InventorySlot_1);
	array_InventorySlots.Add(InventorySlot_2);
	array_InventorySlots.Add(InventorySlot_3);
	array_InventorySlots.Add(InventorySlot_4);
	array_InventorySlots.Add(InventorySlot_5);
	array_InventorySlots.Add(InventorySlot_6);
	array_InventorySlots.Add(InventorySlot_7);
	
	// 인벤토리의 빈 슬롯으로 아이템을 이동 시키려면 빈 슬롯의 인덱스가 필요함.
	InventorySlot_0->SlotIndex = 0;
	InventorySlot_1->SlotIndex = 1;
	InventorySlot_2->SlotIndex = 2;
	InventorySlot_3->SlotIndex = 3;
	InventorySlot_4->SlotIndex = 4;
	InventorySlot_5->SlotIndex = 5;
	InventorySlot_6->SlotIndex = 6;
	InventorySlot_7->SlotIndex = 7;

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
	for (auto slot : array_InventorySlots)
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

	return array_InventorySlots[index];
}

int UInventory_WS::GetSlotIndex(UInventorySlot_WS* InventorySlot)
{
	// 현재 슬롯의 포인터로 슬롯의 인덱스를 반환함.
	// TArray에서의 인덱스임. 
	return array_InventorySlots.Find(InventorySlot);
}