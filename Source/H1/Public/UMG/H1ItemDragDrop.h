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

	// ����Ʈ Ű�� ���� ���·� �������� �巡�� ����ϸ� ��ü ������ 1/2�� �巡�� ��� ������� �Ѵ�.
	UPROPERTY()
	bool Split = false;
};

