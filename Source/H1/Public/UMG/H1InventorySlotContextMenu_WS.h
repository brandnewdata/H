// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "H1InventorySlotContextMenu_WS.generated.h"


class UButton;
class UTextBlock;
class UMenuAnchor;
class UInventorySlot_WS;
/**
 * 
 */
UCLASS()
class H1_API UH1InventorySlotContextMenu_WS : public UH1UMGWidget
{
	GENERATED_BODY()
	
public:
	static UClass* GetWidgetClassStatic();
	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

	virtual bool Initialize() override;

	// slot���� Use ��ư�� ó���� �����մϴ�.
	UFUNCTION()
	void OnClickedUseButton();

	// slot���� Throw ��ư�� ó���� �����մϴ�.
	UFUNCTION()
	void OnClickedThrowButton();

	// slot���� Close ��ư�� ó���� �����մϴ�.
	UFUNCTION()
	void OnClickedCloseButton();

public:
	UPROPERTY()
	UMenuAnchor* MenuAnchor;

	UPROPERTY()
	UInventorySlot_WS* InventorySlot_WS;

public:
	UPROPERTY(meta = (Bindwidget))
	UTextBlock* UseText;

private:
	UPROPERTY(meta = (Bindwidget))
	UButton* UseButton;

	UPROPERTY(meta = (Bindwidget))
	UButton* ThrowButton;

	UPROPERTY(meta = (Bindwidget))
	UButton* CloseButton;

};
