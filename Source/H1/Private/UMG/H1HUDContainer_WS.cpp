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
	//����!
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/H1HUDContainer_WB.H1HUDContainer_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;

	// �ٸ� ���
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

	// ���� HUD�� �翬�� ���ۺ��� ���̰���.
	SetVisibility(ESlateVisibility::Visible);

	return true;
}

bool UH1HUDContainer_WS::OpenWidget(UUserWidget* Widget, FAnchors Anchors, FVector2D Alignment, FVector2D Position)
{
	// �� �Լ��� ����HUD�� ���ԵǾ� �ִ� ���� ������ ���� ȣ��ȴ�.
	// ù��° ���ڷ� ���� ������ ������ ��ġ�� ����ش�. (������ ��Ʈ�ѷ����� �����Ǽ� �´�.)
	// �� �Լ��� controller���� ȣ���Ѵ�.

	if (!IsValid(MainUIContainer) || !IsValid(Widget))
	{
		return false;
	}

	// ������ ������ ����HUD�� ������.
	UCanvasPanelSlot* CanvasPanelSlot = MainUIContainer->AddChildToCanvas(Widget);
	if (!IsValid(CanvasPanelSlot))
		return false;

	// ���� ������ ��ġ�� �����Ѵ�.
	CanvasPanelSlot->SetAutoSize(true);
	CanvasPanelSlot->SetAlignment(Alignment);
	CanvasPanelSlot->SetAnchors(Anchors);
	CanvasPanelSlot->SetPosition(Position);

	return true;
}

bool UH1HUDContainer_WS::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// �� �Լ��� ������ �巡��N��� ������ ���� ����Ѵ�.
	
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
