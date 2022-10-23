// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "..\Source\H1\H1.h"
#include "Item/H1InventoryItem.h"
#include "GameFramework/PlayerController.h"
#include "H1PlayerController.generated.h"

enum class EItemEquipSlot : uint8;


class UH1InventoryItem; // 필요없음
class UInventorySlot_WS;


UCLASS()
class AH1PlayerController : public APlayerController
{
	GENERATED_BODY()

		// 시작 및 초기화 함수들
public:
	AH1PlayerController();
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;

protected:
	// 에디터에서 시뮬레이션을 시작하면 포커스가 실행화면이 아니라서 화면을 한번 눌러야 입력이 가능하다.
	// 인풋모드를 게임으로 설정해서 테스트를 편하게함.
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
	// 종료 팝업 함수 
	UFUNCTION()
	void IP_Esc();

	// 인벤토리 팝업 함수
	UFUNCTION()
	void IP_Inventory();

	// 아이템 픽킹 함수, 아이템 객체에서 호출하며 이 호출 스택은 이름표 버튼 이벤트이다.
	bool TryPickingItem(int ItemClassID, int StackCount);

	// 인벤토리에 ID가 일치하는 아이템이 있는지 확인하고 존재하는경우에는 반환값으로는 그 인덱스를 없으면 none을 반환한다.
	// 두번째 매개변수는 포인터에 대한 참조로 해석된다.
	UFUNCTION()
	int FindInventoryItemByItemID(int ItemClassID, UH1InventoryItem*& FoundSlot);

	// TODO : 참조에 대한 포인터 형식이 특별한 점이 있는지 알아보기
	// 인벤토리에 빈칸이 있는지 확인하는 함수. 빈칸이 있다면 빈칸의 주소를 반환함.
	UFUNCTION()
	int GetEmptyInvenItem(UH1InventoryItem*& FoundSlot);

	// 슬롯에 클릭이 일어났을 때에 controller의 처리 (클릭은 아이템 소모 로직임.)
	UFUNCTION()
	void OnclieckedInventorySlot(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot);

	// 
	UFUNCTION()
	void GetEquipedInventoryItem(EItemEquipSlot ItemEquipSlot, UH1InventoryItem*& InventoryItem, UInventorySlot_WS*& InventorySlot);

	// 컨텍스트 메뉴의 버리기 처리
	UFUNCTION()
	void OnclieckedContextThrow(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot);

	// 서로 다른 아이템 슬롯 간의 내용물 교체
	void ExchangeInventorySlot(UH1InventoryItem* SrcInventoryItem, UInventorySlot_WS* SrcInventorySlot, UH1InventoryItem* DestInventoryItem, UInventorySlot_WS* DestInventorySlot);
	// 슬롯의 아이템이 1개 이상이면 절반만 드래그 아이템으로 생성. 
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


