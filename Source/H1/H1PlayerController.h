// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "..\Source\H1\H1.h"
#include "Item/H1InventoryItem.h"
#include "GameFramework/PlayerController.h"
#include "H1PlayerController.generated.h"

enum class EItemEquipSlot : uint8;


class UH1InventoryItem; // �ʿ����
class UInventorySlot_WS;


UCLASS()
class AH1PlayerController : public APlayerController
{
	GENERATED_BODY()

		// ���� �� �ʱ�ȭ �Լ���
public:
	AH1PlayerController();
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;

protected:
	// �����Ϳ��� �ùķ��̼��� �����ϸ� ��Ŀ���� ����ȭ���� �ƴ϶� ȭ���� �ѹ� ������ �Է��� �����ϴ�.
	// ��ǲ��带 �������� �����ؼ� �׽�Ʈ�� ���ϰ���.
	virtual void BeginPlay() override;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	///** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	///** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	void SetNewMoveDestination(const FVector DestLocation);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

// H1
public:
	// ���� �˾� �Լ� 
	UFUNCTION()
	void IP_Esc();

	// �κ��丮 �˾� �Լ�
	UFUNCTION()
	void IP_Inventory();

	// ������ ��ŷ �Լ�, ������ ��ü���� ȣ���ϸ� �� ȣ�� ������ �̸�ǥ ��ư �̺�Ʈ�̴�.
	bool TryPickingItem(int ItemClassID, int StackCount);

	// �κ��丮�� ID�� ��ġ�ϴ� �������� �ִ��� Ȯ���ϰ� �����ϴ°�쿡�� ��ȯ�����δ� �� �ε����� ������ none�� ��ȯ�Ѵ�.
	// �ι�° �Ű������� �����Ϳ� ���� ������ �ؼ��ȴ�.
	UFUNCTION()
	int FindInventoryItemByItemID(int ItemClassID, UH1InventoryItem*& FoundSlot);

	// TODO : ������ ���� ������ ������ Ư���� ���� �ִ��� �˾ƺ���
	// �κ��丮�� ��ĭ�� �ִ��� Ȯ���ϴ� �Լ�. ��ĭ�� �ִٸ� ��ĭ�� �ּҸ� ��ȯ��.
	UFUNCTION()
	int GetEmptyInvenItem(UH1InventoryItem*& FoundSlot);

	// ���Կ� Ŭ���� �Ͼ�� ���� controller�� ó�� (Ŭ���� ������ �Ҹ� ������.)
	UFUNCTION()
	void OnclieckedInventorySlot(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot);

	// 
	UFUNCTION()
	void GetEquipedInventoryItem(EItemEquipSlot ItemEquipSlot, UH1InventoryItem*& InventoryItem, UInventorySlot_WS*& InventorySlot);

	// ���ؽ�Ʈ �޴��� ������ ó��
	UFUNCTION()
	void OnclieckedContextThrow(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot);

	// ���� �ٸ� ������ ���� ���� ���빰 ��ü
	void ExchangeInventorySlot(UH1InventoryItem* SrcInventoryItem, UInventorySlot_WS* SrcInventorySlot, UH1InventoryItem* DestInventoryItem, UInventorySlot_WS* DestInventorySlot);
	// ������ �������� 1�� �̻��̸� ���ݸ� �巡�� ���������� ����. 
	void SplitInventoryItem(UH1InventoryItem* SrcInventoryItem, UInventorySlot_WS* SrcInventorySlot, UH1InventoryItem* DestInventoryItem, UInventorySlot_WS* DestInventorySlot);


//------------------------------------------------------------------------------------------------------------
// Properties
//-----------------------------------------------------------------------------------------------------------

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

public:
	UPROPERTY()
	class UH1HUDContainer_WS* WidgetMainContainer;

	UPROPERTY()
	TArray<UH1InventoryItem*> InventoryItems;

private:
	//
	const int MaxInventorySlotCount = 8;
};


