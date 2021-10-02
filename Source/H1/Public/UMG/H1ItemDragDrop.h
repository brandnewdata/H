#pragma once
#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "H1ItemDragDrop.generated.h"

class UH1InventoryItem;
class UInventorySlot_WS;
/**
 * 
 */
UCLASS()
class H1_API UH1ItemDragDrop : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UH1InventoryItem* InventoryItemRef;
	UPROPERTY()
	UInventorySlot_WS* FromInventorySlotRef;

	// 쉬프트 키를 누른 상태로 아이템을 드래그 드롭하면 전체 개수의 1/2만 드래그 드롭 대상으로 한다.
	UPROPERTY()
	bool Split = false;
};

