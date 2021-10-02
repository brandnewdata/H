#pragma once
#include "CoreMinimal.h"
#include "UMG/H1UMGWidget.h"
#include "DraggedItem_WS.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class H1_API UDraggedItem_WS : public UH1UMGWidget
{
	GENERATED_BODY()

	virtual UClass* GetWidgetClass() override { return GetWidgetClassStatic(); }

public:
	static UClass* GetWidgetClassStatic();

	// 아이템 이미지 설정
	void SetItemImage(const FSoftObjectPath& ItemPathRef);

public:
	UPROPERTY(meta = (Bindwidget))
	UImage* ItemImage;
};
