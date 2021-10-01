// Fill out your copyright notice in the Description page of Project Settings.


#include "UMG/H1UMGWidget.h"
#include "Runtime/UMG/Public/Animation/WidgetAnimation.h"

// TODO : 재확인 대상 코드
void UH1UMGWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 어차피 비어 있을거 같긴한데..
	// 이 함수가 특이하게 애니메이션 
	AnimationList.Empty();

	// 현재 Widget의 Class를 알아온다.(실형식을 가져옴. 가상함수이고 현재 클래스에는 그냥 nullptr 반환함.)
	UClass* WidgetClass = GetWidgetClass();

	// 현재 클래스가 유효하면
	if (WidgetClass == nullptr)
		return;

	
	for (TFieldIterator<FObjectProperty> PropIt(WidgetClass); PropIt; ++PropIt)
	{
		FObjectProperty* Property = *PropIt;

		// TODO : IsValid 함수로 IsPendingKill 체크하던거 못하게 됨 대안필요함. 이러한 연유로 nullptr만 체크함.
		if (Property != nullptr)
		{
			UObject* TestObj = Property->GetObjectPropertyValue_InContainer(this);
			UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(TestObj);
			if (IsValid(WidgetAnimation))
			{
				AnimationList.Add(Property->GetFName(), WidgetAnimation);
			}
		}
	}
	
	// 기본형에서 가장 많이 파생된(most-drived to base, OOP 실형식과 같은 표현) Class의 UProperty 리스트를에 대한 포인터를 받아옴. 
	// 동작보니까 진짜 노드로된 리스트 같음.
	//FProperty* prop = WidgetClass->PropertyLink;

	//// Run through all properties of this class to find any widget animations
	//while (prop != nullptr)
	//{
	//	// Only interested in object properties
	//	if (prop->GetClass() == UObjectProperty::StaticClass())
	//	{
	//		UObjectProperty* objectProp = Cast<UObjectProperty>(prop);

	//		// Only want the properties that are widget animations
	//		if (objectProp->PropertyClass == UWidgetAnimation::StaticClass())
	//		{
	//			UObject* object = objectProp->GetObjectPropertyValue_InContainer(this);

	//			UWidgetAnimation* widgetAnim = Cast<UWidgetAnimation>(object);

	//			if (widgetAnim != nullptr)
	//			{
	//				AnimationList.Add(objectProp->GetFName(), widgetAnim);

	//				// DO SOMETHING TO STORE OFF THE ANIM PTR HERE!
	//				// E.g. add to a TArray of some struct that holds info for each anim
	//			}
	//		}
	//	}

	//	// 
	//	prop = prop->PropertyLinkNext;
	//}
		 
}

	
