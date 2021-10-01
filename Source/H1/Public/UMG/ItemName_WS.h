// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "ItemName_WS.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemNameButtonClicked);

UCLASS()
class H1_API UItemName_WS : public UH1UMGWidget
{
	GENERATED_BODY()
	
public:
	static UClass* GetWidgetClassStatic();
	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

	virtual bool Initialize() override;

	void SetItemName(FString NewItemName);

	void Showing();
	void Hiding();

	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

	UFUNCTION()
	void OnClickedButton();

	UFUNCTION()
	void OnHoveredButton();

	UFUNCTION()
	void OnUnHoveredButton();

public:
	UPROPERTY(meta = (Bindwidget))
	class UButton* ItemButton;

	UPROPERTY(meta = (Bindwidget))
	class UTextBlock* ItemName;

	UPROPERTY(meta = (Bindwidget))
	class UImage* BackImage;

	UPROPERTY()
	FOnItemNameButtonClicked OnItemNameButtonClicked;
};
