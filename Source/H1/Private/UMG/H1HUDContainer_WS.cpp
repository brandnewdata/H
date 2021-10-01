// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/H1HUDContainer_WS.h"
#include "Runtime/UMG/Public/Components/CanvasPanel.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Blueprint/SlateBlueprintLibrary.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"
#include "Runtime/Core/Public/Misc/MessageDialog.h"
#include "UMG/Exit_WS.h"
#include "UMG/Inventory_WS.h"
#include "UMG/H1ItemDesc_WS.h"
//#include "UMG/WidgetDragDrop.h"
//#include "UMG/ItemDragDrop.h"
#include "UMG/InventorySlot_WS.h"
#include "Item/H1InventoryItem.h"
#include "../Source/H1/H1PlayerController.h"

UClass* UH1HUDContainer_WS::GetWidgetClassStatic()
{
	//생성!
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/H1HUDContainer_WB.H1HUDContainer_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;

	// 다른 방식
	//FSoftClassPath WidgetClassLoader(TEXT("WidgetBlueprint'/Game/UMG/MainContainer_WB.MainContainer_WB_C'"));
	//UClass* WidgetClass = WidgetClassLoader.TryLoadClass<UUserWidget>();
	//return WidgetClass;
}

bool UH1HUDContainer_WS::Initialize()
{
	Super::Initialize();

	if (IsValid(ItemDesc))
	{
		ItemDesc->SetVisibility(ESlateVisibility::Collapsed);
		UCanvasPanelSlot* ItemDescCanvasSlot = Cast<UCanvasPanelSlot>(ItemDesc->Slot);
		if (IsValid(ItemDescCanvasSlot))
		{
			ItemDescCanvasSlot->SetZOrder(255);
		}
	}

	// 메인 HUD는 당연히 시작부터 보이게함.
	SetVisibility(ESlateVisibility::Visible);

	return true;
}

bool UH1HUDContainer_WS::OpenWidget(UUserWidget* Widget, FAnchors Anchors, FVector2D Alignment, FVector2D Position)
{
	// 이 함수는 메인HUD에 포함되어 있는 하위 위젯을 열때 호출된다.
	// 첫번째 인자로 받은 위젯을 적절한 위치를 잡아준다. (위젯은 컨트롤러에서 생성되서 온다.)
	// 이 함수는 controller에서 호출한다.

	if (!IsValid(MainUIContainer) || !IsValid(Widget))
	{
		return false;
	}

	// 생성된 위젯이 메인HUD에 부착함.
	UCanvasPanelSlot* CanvasPanelSlot = MainUIContainer->AddChildToCanvas(Widget);
	if (!IsValid(CanvasPanelSlot))
		return false;

	// 열린 위젯의 위치를 설정한다.
	CanvasPanelSlot->SetAutoSize(true);
	CanvasPanelSlot->SetAlignment(Alignment);
	CanvasPanelSlot->SetAnchors(Anchors);
	CanvasPanelSlot->SetPosition(Position);

	return true;
}

bool UH1HUDContainer_WS::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// 이 함수는 아이템 드래그N드롭 구현할 때에 사용한다.
	
	//Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//UItemDragDrop* ItemDragDropOperation = Cast<UItemDragDrop>(InOperation);
	//if (!IsValid(ItemDragDropOperation))
	//	return false;

	//Apractice_umgPlayerController* practice_umgPlayerController = Cast<Apractice_umgPlayerController>(GetOwningPlayer());
	//if (!IsValid(practice_umgPlayerController))
	//	return false;

	//practice_umgPlayerController->OnclieckedContextThrow(ItemDragDropOperation->InventoryItemRef, ItemDragDropOperation->FromInventorySlotRef);

	return true;
}
