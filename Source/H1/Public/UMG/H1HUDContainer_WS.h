// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "Runtime/Slate/Public/Widgets/Layout/Anchors.h"
#include "H1HUDContainer_WS.generated.h"

class UButton;
class UInventory_WS;
class UPlayUnitHUD_WS;
class UH1ItemDesc_WS;

/**
 * 
 */
UCLASS()
class H1_API UH1HUDContainer_WS : public UH1UMGWidget
{
	GENERATED_BODY()

public:
	static UClass* GetWidgetClassStatic();
	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

	virtual bool Initialize() override;

	bool OpenWidget(UUserWidget* Widget, FAnchors Anchors, FVector2D Alignment, FVector2D Position);

protected:
	// 인벤토리 외부로 아이템을 드랍하면 처리하는 함수.
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
	UPROPERTY(meta = (Bindwidget))
	class UCanvasPanel* MainUIContainer;

	UPROPERTY(meta = (Bindwidget))
	UInventory_WS* Inventory;

	UPROPERTY(meta = (Bindwidget))
	UPlayUnitHUD_WS* PlayUnitHUD;

	UPROPERTY(meta = (Bindwidget))
	UH1ItemDesc_WS* ItemDesc;

};
