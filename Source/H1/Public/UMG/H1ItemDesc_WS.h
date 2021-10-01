// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "H1ItemDesc_WS.generated.h"



class UH1InventoryItem;
/**
 * 
 */
UCLASS()
class H1_API UH1ItemDesc_WS : public UH1UMGWidget
{
	GENERATED_BODY()

public:
	static UClass* GetWidgetClassStatic();
	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

	virtual bool Initialize() override;

	void PlayShowAnimation();
	void SetDesc(UH1InventoryItem* InventoryItem);

private:
	UPROPERTY(meta = (Bindwidget))
	class UImage* ThumbName;

	UPROPERTY(meta = (Bindwidget))
	class UTextBlock* ItemName;

	UPROPERTY(meta = (Bindwidget))
	class UTextBlock* ItemDesc;

};
