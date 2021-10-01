// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "PlayUnitHUD_WS.generated.h"


class UProgressBar;
/**
 * 
 */
UCLASS()
class H1_API UPlayUnitHUD_WS : public UH1UMGWidget
{
	GENERATED_BODY()
	

public:
	static UClass* GetWidgetClassStatic();
	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

	void SetHPRate(float HpRate);

private:
	UPROPERTY(meta = (Bindwidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (Bindwidget))
	UProgressBar* ManaBar;
	
	UPROPERTY(meta = (Bindwidget))
	UProgressBar* StaminaBar;
};
