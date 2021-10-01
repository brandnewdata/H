// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/H1InventorySlotContextMenu_WS.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/MenuAnchor.h"
#include "UMG/InventorySlot_WS.h"


UClass* UH1InventorySlotContextMenu_WS::GetWidgetClassStatic()
{
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/Item_System_UMG/InvenContextMenu_WB.InvenContextMenu_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

bool UH1InventorySlotContextMenu_WS::Initialize()
{
	Super::Initialize();

	if (IsValid(UseButton))
		UseButton->OnClicked.AddDynamic(this, &UH1InventorySlotContextMenu_WS::OnClickedUseButton);

	if (IsValid(ThrowButton))
		ThrowButton->OnClicked.AddDynamic(this, &UH1InventorySlotContextMenu_WS::OnClickedThrowButton);

	if (IsValid(CloseButton))
		CloseButton->OnClicked.AddDynamic(this, &UH1InventorySlotContextMenu_WS::OnClickedCloseButton);

	return true;
}

void UH1InventorySlotContextMenu_WS::OnClickedUseButton()
{
	if (IsValid(MenuAnchor))
		MenuAnchor->Close();

	InventorySlot_WS->OnContextUse();
}

void UH1InventorySlotContextMenu_WS::OnClickedThrowButton()
{
	if (IsValid(MenuAnchor))
		MenuAnchor->Close();

	InventorySlot_WS->OnContextThrow();
}

void UH1InventorySlotContextMenu_WS::OnClickedCloseButton()
{
	if (IsValid(MenuAnchor))
		MenuAnchor->Close();

	InventorySlot_WS->OnContextClose();
}
