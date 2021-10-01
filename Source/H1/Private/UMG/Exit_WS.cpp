// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Exit_WS.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/UMG/Public/Animation/WidgetAnimation.h"

void UExit_WS::NativeConstruct()
{
	Super::NativeConstruct();

	// �ʱ�ȭ�Ҷ� �θ�Ŭ�������� ������� �ִϸ��̼� ����Ʈ���� showing�� ã�Ƽ� �����Ѵ�.
	UWidgetAnimation** FIndAnimation = AnimationList.Find(FName(TEXT("Showing")));
	if (nullptr != FIndAnimation)
	{
		PlayAnimation(*FIndAnimation);
	}
}


UClass* UExit_WS::GetWidgetClassStatic()
{
	// BP�ּ����κ��� ������ �����Ѵ�.
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/AppCore_UMG/Exit_WB.Exit_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

bool UExit_WS::Initialize()
{
	Super::Initialize();

	YesButton->OnClicked.AddDynamic(this, &UExit_WS::OnClickedYesButton);
	NoButton->OnClicked.AddDynamic(this, &UExit_WS::OnClickedNoButton);

	return true;
}

void UExit_WS::OnClickedYesButton()
{
	// TODO : ��ü�� ������  UKismetSystemLibrary::QuitGame 4��° �Ű�����.
	// @������ �Ű������� ��ü�� ��ȣ�ϴ�. ������ �Ʒ��� ������ Ư�� �÷������� �ٸ� ���� �ʿ��ѵ�.
	// Ignores and best-practices based on platform (e.g PS4 games should never quit). Non-shipping only
	// FGenericPlatformMisc::RequestExit(bool)�̶�� �Լ��� true�� ��������� ������ �߻���.
	// �̷��� ������ false�� �����صξ���.
	UKismetSystemLibrary::QuitGame(GetOwningPlayer()->GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit, false);
}

void UExit_WS::OnClickedNoButton()
{
	// ���� ������ϴϱ� â ����. 
	// �� �� ����� ����� ��������?
	// �ѹ��� ��귮�� ������ ���� ��ü�� ����� �Ҹ��ϴ� �ʴ� ����� ����. 
	// ���� ���� �˾��� ��¥ ���� ���Ḧ �Ϸ��� ��Ȳ�̳� �Ǽ��θ� ��������ϱ�.
	RemoveFromParent();
}

