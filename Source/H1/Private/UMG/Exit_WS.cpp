// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/Exit_WS.h"
#include "Runtime/UMG/Public/Components/Button.h"
#include "Runtime/Engine/Classes/Engine/ObjectLibrary.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/UMG/Public/Animation/WidgetAnimation.h"

void UExit_WS::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기화할때 부모클래스에서 만들어진 애니메이션 리스트에서 showing을 찾아서 실행한다.
	UWidgetAnimation** FIndAnimation = AnimationList.Find(FName(TEXT("Showing")));
	if (nullptr != FIndAnimation)
	{
		PlayAnimation(*FIndAnimation);
	}
}


UClass* UExit_WS::GetWidgetClassStatic()
{
	// BP애셋으로부터 위젯을 생성한다.
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
	// TODO : 정체를 밝혀라  UKismetSystemLibrary::QuitGame 4번째 매개변수.
	// @마지막 매개변수는 정체가 모호하다. 설명은 아래와 같은데 특정 플랫폼마다 다른 값이 필요한듯.
	// Ignores and best-practices based on platform (e.g PS4 games should never quit). Non-shipping only
	// FGenericPlatformMisc::RequestExit(bool)이라는 함수도 true면 강제종료라 문제가 발생함.
	// 이러한 이유로 false로 설정해두었다.
	UKismetSystemLibrary::QuitGame(GetOwningPlayer()->GetWorld(), GetOwningPlayer(), EQuitPreference::Type::Quit, false);
}

void UExit_WS::OnClickedNoButton()
{
	// 게임 종료안하니까 창 삭제. 
	// 좀 더 우우한 방법이 있을지도?
	// 한번은 계산량이 많지만 게임 전체에 비용을 소모하는 않는 방식이 좋다. 
	// 게임 종료 팝업은 진짜 게임 종료를 하려는 상황이나 실수로만 만들어지니까.
	RemoveFromParent();
}

