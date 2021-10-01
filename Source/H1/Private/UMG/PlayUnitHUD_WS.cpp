// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/PlayUnitHUD_WS.h"
#include "Runtime/UMG/Public/Components/ProgressBar.h"

UClass* UPlayUnitHUD_WS::GetWidgetClassStatic()
{
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/Status_System_UMG/PlayUnitHUD_WB.PlayUnitHUD_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

void UPlayUnitHUD_WS::SetHPRate(float HpRate)
{
	// 0�� 1���̷� ������. 
	HpRate = FMath::Clamp<float>(HpRate, 0.0f, 1.0f);

	// ü�¹� ����
	HealthBar->SetPercent(HpRate);
}
