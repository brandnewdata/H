// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/H1ItemDesc_WS.h"
#include "Runtime/Engine/Classes/Kismet/KismetTextLibrary.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/SlateCore/Public/Input/Reply.h"
#include "Item/H1InventoryItem.h"
#include "Item/H1ItemTableDef.h"


UClass* UH1ItemDesc_WS::GetWidgetClassStatic()
{
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/Item_System_UMG/H1ItemDesc_WB.H1ItemDesc_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

bool UH1ItemDesc_WS::Initialize()
{
	Super::Initialize();

	return true;
}

void UH1ItemDesc_WS::PlayShowAnimation()
{
	UWidgetAnimation** ShowAnimation = AnimationList.Find(FName(TEXT("Show")));
	if (nullptr != ShowAnimation)
	{
		PlayAnimation(*ShowAnimation);
	}
}

void UH1ItemDesc_WS::SetDesc(UH1InventoryItem* InventoryItem)
{
	if (!IsValid(InventoryItem))
		return;

	const FItemTableRow* ItemTableRow = InventoryItem->GetItemTableRow();
	if (nullptr == ItemTableRow)
		return;

	ThumbName->SetBrushFromTexture(Cast<UTexture2D>(ItemTableRow->ItemThumb.TryLoad()));

	ItemName->SetText(UKismetTextLibrary::Conv_StringToText(ItemTableRow->ItemName));
	ItemDesc->SetText(FText::AsCultureInvariant(ItemTableRow->ItemDesc));
}