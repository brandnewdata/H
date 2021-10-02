#include "UMG/DraggedItem_WS.h"
#include "Runtime/UMG/Public/Components/Image.h"

UClass* UDraggedItem_WS::GetWidgetClassStatic()
{
	FSoftObjectPath AssetLoader(TEXT("WidgetBlueprint'/Game/UMG/Item_System_UMG/DraggedItem_WB.DraggedItem_WB_C'"));
	UClass* Object = Cast<UClass>(AssetLoader.TryLoad());
	if (!IsValid(Object))
		return nullptr;

	return Object;
}

void UDraggedItem_WS::SetItemImage(const FSoftObjectPath& ItemPathRef)
{
	if (IsValid(ItemImage))
	{
		ItemImage->SetBrushFromTexture(Cast<UTexture2D>(ItemPathRef.TryLoad()));
	}
}
