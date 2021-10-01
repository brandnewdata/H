// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "Exit_WS.generated.h"

/**
 * 
 */
UCLASS()
class H1_API UExit_WS : public UH1UMGWidget
{
	GENERATED_BODY()

public:
	static UClass* GetWidgetClassStatic();
	virtual bool Initialize() override;
	UFUNCTION()
	void OnClickedYesButton();

	UFUNCTION()
	void OnClickedNoButton();

protected:
	virtual void NativeConstruct() override;
	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

//-------------------------- properties ----------------------------------------------------
public:
	UPROPERTY(meta = (Bindwidget))
	class UButton* NoButton;

	UPROPERTY(meta = (Bindwidget))
	class UButton* YesButton;
};
