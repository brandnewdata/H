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

// 내가고치는 그
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


	// 메뉴 앵커의 open() 함수를 받으면 어떻게 팝업UI 구성할지를 가진 함수를 바인딩함.
	// 
	if (IsValid(ContextMenuAnchor))
	{
		// defrecated
		// ContextMenuAnchor->OnGetMenuContentEvent.BindUFunction(this, FName("OnGetMenuContentEvent"));
		// 4.26 대체 //
		// https://docs.unrealengine.com/4.27/ko/InteractiveExperiences/UMG/UserGuide/WidgetTypeReference/MenuAnchor/
		ContextMenuAnchor->OnGetUserMenuContentEvent.BindUFunction(this, FName("OnGetMenuContentEvent"));
	}

	// 메인 컨테이너 포인터를 플레이어 컨트롤러가 가지기 전에 이게 호출되면 크래쉬.
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
	OnCliekedInventoryContext.Clear();	// 슬롯에 연결된 컨텍스트 메뉴를 지운다.
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

	// 인벤토리에 아이템이 없으면 Desc도 없다.
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

	// 타이머 참조를 얻어와서 
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

	// 마우슨 왼쪽 버튼이 눌렸나?
	LButtonDown = InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	// 마우스 왼쪽 버튼이 안 눌렸고 오른쪽만 눌렸냐?
	RButtonDown = (false == LButtonDown) && InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton);

	// 왼쪽만 눌렸다면 
	if (LButtonDown)
	{
		// 드래그를 시작합니다.
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
	// 바인드된 소모 및 장착용 함수들 수행
	if (OnCliekedInventorySlot.IsBound())
	{
		OnCliekedInventorySlot.Broadcast(InventoryItemRef, this);
	}
}

void UInventorySlot_WS::OnRButtonClick()
{
	// 슬롯에 아이템이 있으면
	if (true == IsEmpty)
		return;
	
	// 컨텍스트 메뉴를 활성화
	if (IsValid(ContextMenuAnchor))
	{
		ContextMenuAnchor->Open(true);
	}
}

UWidget* UInventorySlot_WS::OnGetMenuContentEvent()
{
	// 슬롯과 연결된 인벤토리 아이템이 유효하면! (컨트롤러 소유중)
	if (!IsValid(InventoryItemRef))
		return nullptr;

	// 리플렉션 기능활용해서~ 위젯을 생성하고 포인터 얻어옴.
	UH1InventorySlotContextMenu_WS* Contextmenu = CreateWidget<UH1InventorySlotContextMenu_WS>(GetOwningPlayer(), UH1InventorySlotContextMenu_WS::GetWidgetClassStatic());

	// 생성한 팝업 메뉴창의 앵커를 슬롯의 멤버로 가지고 있는메뉴 앵커로 지정한다.
	Contextmenu->MenuAnchor = ContextMenuAnchor;
	// 현재  슬롯은 생성한 팝업 메뉴의 아웃터이고 처리가 연관되어 있어서 포인터를 넘김. // 사용하다가 다른방법이 생겨서 필요 없으면 나중에 뺄 것.
	Contextmenu->InventorySlot_WS = this;

	// 슬롯에 들어있는 아이템의 테이블행 포인터 가져옴.
	const FItemTableRow* ItemTableRow = InventoryItemRef->GetItemTableRow();
	// 테이블행이 유효하다면
	if (ItemTableRow)
	{
		// 아이템의 타입에 따라서 UI구성을 다르게 설정함. // 소모성은 컨슘, 장비는 장착으로 use를 설정함.
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
			UseString = TEXT("Use"); // 이건 사실상 오류...
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

	// 드래그 드롭 연산 생성
	UH1ItemDragDrop* ItemDragDropOperation = Cast<UH1ItemDragDrop>(UWidgetBlueprintLibrary::CreateDragDropOperation(UH1ItemDragDrop::StaticClass()));
	if (!IsValid(ItemDragDropOperation))
		return;

	// TODO : 아이템 슬롯에 아이템이 없어도 드래그 드롭이 발동 됩니다.
	// 여기보다 먼저 시점에서 이 것을 체크해서 드래그 드롭을 막아야 합니다.
	if (InventoryItemRef == nullptr)
		return;

	// 드래그로 이동하는 아이템 이미지 생성 
	UDraggedItem_WS* DragingItemWidget = CreateWidget<UDraggedItem_WS>(GetOwningPlayer(), UDraggedItem_WS::GetWidgetClassStatic());
	if (IsValid(DragingItemWidget))
	{
		const FItemTableRow* ItemTableRow = InventoryItemRef->GetItemTableRow();
		if (nullptr != ItemTableRow)
		{
			DragingItemWidget->SetItemImage(ItemTableRow->ItemThumb);
		}

		// 드래그 연산의 대상으로 이 위젯을 지정함.
		ItemDragDropOperation->DefaultDragVisual = DragingItemWidget;
	}

	ItemDragDropOperation->Split = InMouseEvent.IsShiftDown(); // 절반은 원래 슬롯에 있고 1/2만 드래그 아이템으로 만들어 진다.
	ItemDragDropOperation->FromInventorySlotRef = this;	// 출발지 슬롯 위치 설정
	ItemDragDropOperation->InventoryItemRef = InventoryItemRef; // 드래그하는 인벤토리 아이템의 포인터
	ItemDragDropOperation->Pivot = EDragPivot::MouseDown; // 드래그드롭하는 위젯이 마우스 보다 위냐 아래냐 z오더 문제로 보임.

	OutOperation = ItemDragDropOperation; // 출력 매개변수

	// 마우스 버튼업시에 클릭이나 버튼업에 묶인 이벤트가 발생하지 않게 처리함.
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


