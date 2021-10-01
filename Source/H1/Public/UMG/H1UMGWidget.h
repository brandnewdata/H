// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Source\H1\H1.h"
#include "Blueprint/UserWidget.h"
#include "H1UMGWidget.generated.h"

/**
 * 
 */
class UWidgetAnimation;

UCLASS()
class H1_API UH1UMGWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	virtual UClass* GetWidgetClass() { return nullptr; }

protected:
	UPROPERTY()
	TMap<FName, UWidgetAnimation*> AnimationList;
};
