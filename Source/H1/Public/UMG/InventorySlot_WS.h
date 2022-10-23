// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "InventorySlot_WS.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UButton;
class UMenuAnchor;
class UH1InventoryItem;
class UH1ItemDesc_WS;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCliekedInventorySlot, UH1InventoryItem*, InventoryItem, UInventorySlot_WS*, InventorySlot);

UCLASS()
class H1_API UInventorySlot_WS : public UH1UMGWidget
{
	GENERATED_BODY()
	
public:
	static UClass* GetWidgetClassStatic();
	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

	virtual bool Initialize() override;

	// 플레이어 컨트롤러의 인벤토리와 인벤토리 UI의 인벤토리 슬롯을 동기화하는 역할을한다.
	// 플레이어 컨트롤러가 인벤토리 아이템의 정보를 넘기면 그 정보로 인벤토리 슬롯을 설정한다.
	void SetItemInfo(UH1InventoryItem* NewInventoryItemRef);
	void ShowAnimation();
	bool GetIsEmpty() { return IsEmpty; }

	// 슬롯 초기화
	UFUNCTION()
	void CleanUp();

	UFUNCTION()
	void UpdateSlot();

	UFUNCTION()
	void OnLButtonClick();

	UFUNCTION()
	void OnRButtonClick();

	// Use는 슬롯이 눌렸을 때랑 같은 함수로 동작합니다. 소모 or 장착 //
	UFUNCTION()
	void OnContextUse();

	// 바닥에 버리는 함수가 바인드 되어 있습니다.
	UFUNCTION()
	void OnContextThrow();

	// 메뉴 자체를 삭제합니다. 아직 별다른 처리 없음 혼자 
	UFUNCTION()
	void OnContextClose();

	// 이렇게 하면 UMenuAnchor->open()함수 호출-> OnGetUserMenuContentEvent 델리게이트에 바인드된 함수가 호출되고
	// 함수 내부에서 컨텍스트 메뉴 UI의 컨텐츠(UI Asset)를 실제로 생성해서 그 포인터를 반환합니다.
	UFUNCTION()
	UWidget* OnGetMenuContentEvent();

protected:
	// Click Functions
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// Mouse over Functions
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	// DragNDrop Functions
	//
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	// 드래깅이 취소되면 호출되는 함수 인데 아직 그런 경우가 없음.
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	// 현재는 슬롯위로 다른 슬롯의 아이템이 드래그 상태로 들어오면 현재 지나고 있는 슬롯의 투명도가 0.5가됨
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	// 슬롯 위로 지나가던 드래그가 끝나면 슬롯의 투명도를 원래 1.0으로 되돌린다.
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	void ClearItemDesc();

public:
	UPROPERTY(meta=(Bindwidget))
	UImage* ItemImage;
	UPROPERTY(meta=(Bindwidget))
	UTextBlock* ItemStateText;
	UPROPERTY(meta=(Bindwidget))
	UMenuAnchor* ContextMenuAnchor;

	// 슬롯에 저장된 '인벤토리 아이템'에 대한 포인터
	UPROPERTY()
	UH1InventoryItem* InventoryItemRef;


	// 슬롯이 클리받으면 L이면 Consume OR Equip // R이면 Context Menu 활성
	UPROPERTY()
	FOnCliekedInventorySlot OnCliekedInventorySlot; 

	// 컨텍스트 메뉴의 버튼이 눌리면 실행되는 이벤트들.
	UPROPERTY()
	FOnCliekedInventorySlot OnCliekedInventoryContext;

	FTimerHandle TimerHandle;

public:
	// 인벤토리의 빈 슬롯으로 아이템을 이동 시키려면 빈 슬롯의 인덱스가 필요함.
	int SlotIndex = INDEX_NONE;

private:
	bool IsEmpty = true;
	bool LButtonDown = false;
	bool RButtonDown = false;
};
