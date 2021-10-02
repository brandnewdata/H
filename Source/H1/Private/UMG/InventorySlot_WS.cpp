// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/InventorySlot_WS.h"

#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Components/MenuAnchor.h"
#include "Runtime/UMG/Public/Animation/WidgetAnimation.h"
#include "Runtime/UMG/Public/Blueprint/SlateBlueprintLibrary.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/SlateCore/Public/Input/Reply.h"

#include "CoreExtentsion/H1GameInstance.h"
#include "CoreExtentsion/H1TableManager.h"

#include "Item/H1InventoryItem.h"
#include "Item/H1ItemTableDef.h"
#include "../Source/H1/H1PlayerController.h"

#include "UMG/H1InventorySlotContextMenu_WS.h"
#include "UMG/H1ItemDesc_WS.h"
#include "UMG/H1HUDContainer_WS.h"
#include "UMG/H1ItemDragDrop.h"
#include "UMG/DraggedItem_WS.h"

// ������ġ�� ��
#include "Runtime/UMG/Public/Components/Button.h"

UClass* UInventorySlot_WS::GetWidgetClassStatic()
{
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/Item_System_UMG/InventorySlot_WB.InventorySlot_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

bool UInventorySlot_WS::Initialize()
{
	Super::Initialize();


	// �޴� ��Ŀ�� open() �Լ��� ������ ��� �˾�UI ���������� ���� �Լ��� ���ε���.
	// 
	if (IsValid(ContextMenuAnchor))
	{
		// defrecated
		// ContextMenuAnchor->OnGetMenuContentEvent.BindUFunction(this, FName("OnGetMenuContentEvent"));
		// 4.26 ��ü //
		// https://docs.unrealengine.com/4.27/ko/InteractiveExperiences/UMG/UserGuide/WidgetTypeReference/MenuAnchor/
		ContextMenuAnchor->OnGetUserMenuContentEvent.BindUFunction(this, FName("OnGetMenuContentEvent"));
	}

	// ���� �����̳� �����͸� �÷��̾� ��Ʈ�ѷ��� ������ ���� �̰� ȣ��Ǹ� ũ����.
	SetItemInfo(0);

	return true;
}

void UInventorySlot_WS::SetItemInfo(UH1InventoryItem* NewInventoryItemRef)
{
	InventoryItemRef = NewInventoryItemRef;
	if (nullptr == NewInventoryItemRef)
	{
		CleanUp();
		return;
	}

	const FItemTableRow* ItemTable = InventoryItemRef->GetItemTableRow();

	if (nullptr == ItemTable)
	{
		CleanUp();
		return;
	}

	IsEmpty = false;
	ItemImage->SetBrushFromTexture(Cast<UTexture2D>(ItemTable->ItemThumb.TryLoad()), true);
	ItemImage->SetVisibility(ESlateVisibility::Visible);
}

void UInventorySlot_WS::ShowAnimation()
{
	UWidgetAnimation** WidgetAnimation = AnimationList.Find(FName(TEXT("ShowingItemImage")));
	if (nullptr != WidgetAnimation)
	{
		PlayAnimation(*WidgetAnimation);
	}
}

void UInventorySlot_WS::CleanUp()
{
	ClearItemDesc();
	OnCliekedInventorySlot.Clear();
	OnCliekedInventoryContext.Clear();	// ���Կ� ����� ���ؽ�Ʈ �޴��� �����.
	IsEmpty = true;
	ItemImage->SetBrushFromTexture(nullptr);
	ItemImage->SetVisibility(ESlateVisibility::Collapsed);
	ItemStateText->SetText(FText::FromString(TEXT("")));
	InventoryItemRef = nullptr;
}

void UInventorySlot_WS::ClearItemDesc()
{
	AH1PlayerController* H1PlayerController = Cast<AH1PlayerController>(GetOwningPlayer());
	if (!IsValid(H1PlayerController))
		return;

	if (H1PlayerController->WidgetMainContainer == nullptr)
	{
		H1LOG(Warning, TEXT("Main Is Nullptr"));
		return;
	}

	UH1ItemDesc_WS* H1ItemDesc_WS = H1PlayerController->WidgetMainContainer->ItemDesc;
	if (IsValid(H1ItemDesc_WS))
	{
		H1ItemDesc_WS->SetVisibility(ESlateVisibility::Collapsed);
	}

	FTimerManager& TimerManager = H1PlayerController->GetWorldTimerManager();
	if (TimerManager.TimerExists(TimerHandle) && TimerHandle.IsValid())
	{
		TimerManager.ClearTimer(TimerHandle);
	}
}

void UInventorySlot_WS::UpdateSlot()
{
	if (!IsValid(InventoryItemRef))
	{
		CleanUp();
		return;
	}

	const FItemTableRow* ItemTableRow = InventoryItemRef->GetItemTableRow();
	if (nullptr == ItemTableRow)
		return;

	if (ItemTableRow->ItemType == EItemType::Consume)
	{
		if (InventoryItemRef->StackCount <= 1)
		{
			ItemStateText->SetText(FText::FromString(TEXT("")));
		}
		else
		{
			ItemStateText->SetText(FText::FromString(FString::FromInt(InventoryItemRef->StackCount)));
		}
	}
	else
	{
		if (InventoryItemRef->Equipped)
		{
			FString EquipText(TEXT("Equiped"));
			ItemStateText->SetText(FText::FromString(EquipText));
		}
		else
		{
			FString EquipText(TEXT(""));
			ItemStateText->SetText(FText::FromString(EquipText));
		}
	}
}

FReply UInventorySlot_WS::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	return FReply::Unhandled();
}

void UInventorySlot_WS::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	// �κ��丮�� �������� ������ Desc�� ����.
	if (true == IsEmpty)
		return;

	FTimerDelegate TimerCallback;
	TimerCallback.BindLambda([=]
	{
		AH1PlayerController* H1PlayerController = Cast<AH1PlayerController>(GetOwningPlayer());
		if (!IsValid(H1PlayerController))
			return;

		UH1ItemDesc_WS* ItemDesc_WS = H1PlayerController->WidgetMainContainer->ItemDesc;
		if (IsValid(ItemDesc_WS))
		{
			ItemDesc_WS->SetVisibility(ESlateVisibility::HitTestInvisible/* ::Visible*/);

			FVector2D ViewportPixelPos;
			FVector2D ViewportPos;
			USlateBlueprintLibrary::AbsoluteToViewport(H1PlayerController->GetWorld(), InMouseEvent.GetScreenSpacePosition(), ViewportPixelPos, ViewportPos);

			UCanvasPanelSlot* ItemDescSlot = Cast<UCanvasPanelSlot>(ItemDesc_WS->Slot);
			if (IsValid(ItemDescSlot))
			{
				ItemDescSlot->SetAlignment(FVector2D(0.5f, 1.0f));
				ViewportPos.Y += -10.0f;
				ItemDescSlot->SetPosition(ViewportPos);
				ItemDesc_WS->SetDesc(InventoryItemRef);
				ItemDesc_WS->PlayShowAnimation();
			}
		}
	});

	// Ÿ�̸� ������ ���ͼ� 
	FTimerManager& TimerManager = GetOwningPlayer()->GetWorldTimerManager();
	if (TimerManager.TimerExists(TimerHandle))
	{
		TimerManager.ClearTimer(TimerHandle);
	}

	TimerManager.SetTimer(TimerHandle, TimerCallback, 0.5f, false);
}

void UInventorySlot_WS::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	ClearItemDesc();
}


FReply UInventorySlot_WS::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// ���콼 ���� ��ư�� ���ȳ�?
	LButtonDown = InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	// ���콺 ���� ��ư�� �� ���Ȱ� �����ʸ� ���ȳ�?
	RButtonDown = (false == LButtonDown) && InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton);

	// ���ʸ� ���ȴٸ� 
	if (LButtonDown)
	{
		// �巡�׸� �����մϴ�.
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return FReply::Handled();
}

FReply UInventorySlot_WS::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (LButtonDown)
	{
		OnLButtonClick();
	}
	else if (RButtonDown)
	{
		OnRButtonClick();
	}
	else
	{
		return FReply::Unhandled();
	}

	LButtonDown = false;
	RButtonDown = false;

	return FReply::Handled();
}

FReply UInventorySlot_WS::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);

	return FReply::Handled();
}


void UInventorySlot_WS::OnLButtonClick()
{
	// ���ε�� �Ҹ� �� ������ �Լ��� ����
	if (OnCliekedInventorySlot.IsBound())
	{
		OnCliekedInventorySlot.Broadcast(InventoryItemRef, this);
	}
}

void UInventorySlot_WS::OnRButtonClick()
{
	// ���Կ� �������� ������
	if (true == IsEmpty)
		return;
	
	// ���ؽ�Ʈ �޴��� Ȱ��ȭ
	if (IsValid(ContextMenuAnchor))
	{
		ContextMenuAnchor->Open(true);
	}
}

UWidget* UInventorySlot_WS::OnGetMenuContentEvent()
{
	// ���԰� ����� �κ��丮 �������� ��ȿ�ϸ�! (��Ʈ�ѷ� ������)
	if (!IsValid(InventoryItemRef))
		return nullptr;

	// ���÷��� ���Ȱ���ؼ�~ ������ �����ϰ� ������ ����.
	UH1InventorySlotContextMenu_WS* Contextmenu = CreateWidget<UH1InventorySlotContextMenu_WS>(GetOwningPlayer(), UH1InventorySlotContextMenu_WS::GetWidgetClassStatic());

	// ������ �˾� �޴�â�� ��Ŀ�� ������ ����� ������ �ִ¸޴� ��Ŀ�� �����Ѵ�.
	Contextmenu->MenuAnchor = ContextMenuAnchor;
	// ����  ������ ������ �˾� �޴��� �ƿ����̰� ó���� �����Ǿ� �־ �����͸� �ѱ�. // ����ϴٰ� �ٸ������ ���ܼ� �ʿ� ������ ���߿� �� ��.
	Contextmenu->InventorySlot_WS = this;

	// ���Կ� ����ִ� �������� ���̺��� ������ ������.
	const FItemTableRow* ItemTableRow = InventoryItemRef->GetItemTableRow();
	// ���̺����� ��ȿ�ϴٸ�
	if (ItemTableRow)
	{
		// �������� Ÿ�Կ� ���� UI������ �ٸ��� ������. // �Ҹ��� ����, ���� �������� use�� ������.
		FString UseString;
		switch (ItemTableRow->ItemType)
		{
		case EItemType::Consume:
			UseString = TEXT("Consume");
			Contextmenu->UseText->SetText(FText::FromString(UseString));
			break;

		case EItemType::Equip:
			if (InventoryItemRef->Equipped)
			{
				UseString = TEXT("UnEquip");
				Contextmenu->UseText->SetText(FText::FromString(UseString));
			}
			else
			{
				UseString = TEXT("Equip");
				Contextmenu->UseText->SetText(FText::FromString(UseString));
			}
			break;

		default:
			UseString = TEXT("Use"); // �̰� ��ǻ� ����...
			Contextmenu->UseText->SetText(FText::FromString(UseString));
			break;
		}
	}

	return Contextmenu;
}


void UInventorySlot_WS::OnContextUse()
{
	if (OnCliekedInventorySlot.IsBound())
	{
		OnCliekedInventorySlot.Broadcast(InventoryItemRef, this);
	}
}

void UInventorySlot_WS::OnContextThrow()
{
	if (OnCliekedInventoryContext.IsBound())
	{
		OnCliekedInventoryContext.Broadcast(InventoryItemRef, this);
	}
}

void UInventorySlot_WS::OnContextClose()
{

}

void UInventorySlot_WS::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// �巡�� ��� ���� ����
	UH1ItemDragDrop* ItemDragDropOperation = Cast<UH1ItemDragDrop>(UWidgetBlueprintLibrary::CreateDragDropOperation(UH1ItemDragDrop::StaticClass()));
	if (!IsValid(ItemDragDropOperation))
		return;

	// TODO : ������ ���Կ� �������� ��� �巡�� ����� �ߵ� �˴ϴ�.
	// ���⺸�� ���� �������� �� ���� üũ�ؼ� �巡�� ����� ���ƾ� �մϴ�.
	if (InventoryItemRef == nullptr)
		return;

	// �巡�׷� �̵��ϴ� ������ �̹��� ���� 
	UDraggedItem_WS* DragingItemWidget = CreateWidget<UDraggedItem_WS>(GetOwningPlayer(), UDraggedItem_WS::GetWidgetClassStatic());
	if (IsValid(DragingItemWidget))
	{
		const FItemTableRow* ItemTableRow = InventoryItemRef->GetItemTableRow();
		if (nullptr != ItemTableRow)
		{
			DragingItemWidget->SetItemImage(ItemTableRow->ItemThumb);
		}

		// �巡�� ������ ������� �� ������ ������.
		ItemDragDropOperation->DefaultDragVisual = DragingItemWidget;
	}

	ItemDragDropOperation->Split = InMouseEvent.IsShiftDown(); // ������ ���� ���Կ� �ְ� 1/2�� �巡�� ���������� ����� ����.
	ItemDragDropOperation->FromInventorySlotRef = this;	// ����� ���� ��ġ ����
	ItemDragDropOperation->InventoryItemRef = InventoryItemRef; // �巡���ϴ� �κ��丮 �������� ������
	ItemDragDropOperation->Pivot = EDragPivot::MouseDown; // �巡�׵���ϴ� ������ ���콺 ���� ���� �Ʒ��� z���� ������ ����.

	OutOperation = ItemDragDropOperation; // ��� �Ű�����

	// ���콺 ��ư���ÿ� Ŭ���̳� ��ư���� ���� �̺�Ʈ�� �߻����� �ʰ� ó����.
	LButtonDown = false;
	RButtonDown = false;
}

void UInventorySlot_WS::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	//LButtonDown = false;
	//RButtonDown = false;
}

bool UInventorySlot_WS::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	//UH1ItemDragDrop* ItemDragDropOperation = Cast<UH1ItemDragDrop>(InOperation);
	//if (!IsValid(ItemDragDropOperation))
	//	return false;

	//if (this == ItemDragDropOperation->FromInventorySlotRef)
	//{
	//	return true;
	//}

	//AH1PlayerController* H1PlayerController = Cast<AH1PlayerController>(GetOwningPlayer());
	//if (!IsValid(H1PlayerController))
	//	return false;

	//if (ItemDragDropOperation->Split)
	//{
	//	H1PlayerController->SplitInventoryItem(ItemDragDropOperation->InventoryItemRef, ItemDragDropOperation->FromInventorySlotRef, InventoryItemRef, this);
	//}
	//else
	//{
	//	H1PlayerController->ExchangeInventorySlot(ItemDragDropOperation->InventoryItemRef, ItemDragDropOperation->FromInventorySlotRef, InventoryItemRef, this);
	//}

	//ItemImage->SetOpacity(1.0f);
	return true;
}

void UInventorySlot_WS::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	//UItemDragDrop* ItemDragDropOperation = Cast<UItemDragDrop>(InOperation);
	//if (!IsValid(ItemDragDropOperation))
	//	return;

	//if (false == IsEmpty && this != ItemDragDropOperation->FromInventorySlotRef)
	//	ItemImage->SetOpacity(0.5f);
}

void UInventorySlot_WS::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);

	//UItemDragDrop* ItemDragDropOperation = Cast<UItemDragDrop>(InOperation);
	//if (!IsValid(ItemDragDropOperation))
	//	return;

	//ItemImage->SetOpacity(1.0f);
}


