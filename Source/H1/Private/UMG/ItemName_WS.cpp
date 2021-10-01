// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/ItemName_WS.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/UMG/Public/Components/Image.h"
#include "Runtime/UMG/Public/Animation/WidgetAnimation.h"

UClass* UItemName_WS::GetWidgetClassStatic()
{
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/Item_System_UMG/ItemName_WB.ItemName_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

bool UItemName_WS::Initialize()
{
	Super::Initialize();

	ItemButton->OnClicked.AddDynamic(this, &UItemName_WS::OnClickedButton);
	ItemButton->OnHovered.AddDynamic(this, &UItemName_WS::OnHoveredButton);
	ItemButton->OnUnhovered.AddDynamic(this, &UItemName_WS::OnUnHoveredButton);

	return true;
}

void UItemName_WS::SetItemName(FString NewItemName)
{
	//ItemName->SetText(FText::AsCultureInvariant(NewItemName));
	ItemName->SetText(FText::FromString(NewItemName));
}

void UItemName_WS::Showing()
{
	UWidgetAnimation** WidgetAnimation = AnimationList.Find(FName(TEXT("Showing")));
	if (nullptr != WidgetAnimation)
	{
		PlayAnimation(*WidgetAnimation);
	}
}

void UItemName_WS::Hiding()
{
	UWidgetAnimation** WidgetAnimation = AnimationList.Find(FName(TEXT("Hiding")));
	if (nullptr != WidgetAnimation)
	{
		PlayAnimation(*WidgetAnimation);
	}
}

void UItemName_WS::OnClickedButton()
{
	// 버튼이 눌리면 델리게이트에 바인드되어 있는 작업들을 수행한다.
	if (OnItemNameButtonClicked.IsBound())
	{
		OnItemNameButtonClicked.Broadcast();
	}
}

void UItemName_WS::OnHoveredButton()
{
	BackImage->SetOpacity(0.8f);
}

void UItemName_WS::OnUnHoveredButton()
{
	BackImage->SetOpacity(0.5f);
}

void UItemName_WS::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (IsValid(Animation) && Animation->GetFName() == FName(TEXT("Hiding")))
	{
		RemoveFromParent();
	}
}