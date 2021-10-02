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

	// �÷��̾� ��Ʈ�ѷ��� �κ��丮�� �κ��丮 UI�� �κ��丮 ������ ����ȭ�ϴ� �������Ѵ�.
	// �÷��̾� ��Ʈ�ѷ��� �κ��丮 �������� ������ �ѱ�� �� ������ �κ��丮 ������ �����Ѵ�.
	void SetItemInfo(UH1InventoryItem* NewInventoryItemRef);
	void ShowAnimation();
	bool GetIsEmpty() { return IsEmpty; }

	// ���� �ʱ�ȭ
	UFUNCTION()
	void CleanUp();

	UFUNCTION()
	void UpdateSlot();

	UFUNCTION()
	void OnLButtonClick();

	UFUNCTION()
	void OnRButtonClick();

	// Use�� ������ ������ ���� ���� �Լ��� �����մϴ�. �Ҹ� or ���� //
	UFUNCTION()
	void OnContextUse();

	// �ٴڿ� ������ �Լ��� ���ε� �Ǿ� �ֽ��ϴ�.
	UFUNCTION()
	void OnContextThrow();

	// �޴� ��ü�� �����մϴ�. ���� ���ٸ� ó�� ���� ȥ�� 
	UFUNCTION()
	void OnContextClose();

	// �̷��� �ϸ� UMenuAnchor->open()�Լ� ȣ��-> OnGetUserMenuContentEvent ��������Ʈ�� ���ε�� �Լ��� ȣ��ǰ�
	// �Լ� ���ο��� ���ؽ�Ʈ �޴� UI�� ������(UI Asset)�� ������ �����ؼ� �� �����͸� ��ȯ�մϴ�.
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
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
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

	// ���Կ� ����� '�κ��丮 ������'�� ���� ������
	UPROPERTY()
	UH1InventoryItem* InventoryItemRef;


	// ������ Ŭ�������� L�̸� Consume OR Equip // R�̸� Context Menu Ȱ��
	UPROPERTY()
	FOnCliekedInventorySlot OnCliekedInventorySlot; 

	// ���ؽ�Ʈ �޴��� ��ư�� ������ ����Ǵ� �̺�Ʈ��.
	UPROPERTY()
	FOnCliekedInventorySlot OnCliekedInventoryContext;

	FTimerHandle TimerHandle;

public:
	int SlotIndex = INDEX_NONE;

private:
	bool IsEmpty = true;
	bool LButtonDown = false;
	bool RButtonDown = false;
};
