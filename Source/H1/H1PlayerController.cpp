// Copyright Epic Games, Inc. All Rights Reserved.

#include "H1PlayerController.h"
#include "CoreExtentsion/H1GameInstance.h"
#include "CoreExtentsion/H1TableManager.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "H1Character.h"
#include "Engine/World.h"

#include "UMG/H1HUDContainer_WS.h"
#include "UMG/Exit_WS.h"
#include "UMG/Inventory_WS.h"
#include "UMG/InventorySlot_WS.h"
#include "UMG/PlayUnitHUD_WS.h"
#include "Item/H1InventoryItem.h"
#include "Item/H1ItemTableDef.h"
#include "Item/H1Item.h"

AH1PlayerController::AH1PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}

void AH1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 메인 위젯을 생성해서 뷰포트에 그린다.
	WidgetMainContainer = CreateWidget<UH1HUDContainer_WS>(this, UH1HUDContainer_WS::GetWidgetClassStatic());
	if (IsValid(WidgetMainContainer) && IsLocalPlayerController())
	{
		WidgetMainContainer->AddToViewport(0);

	}

	// 체력과 아이템에 대한  설정들.-----------------------------------------------------------
	
	// 인벤토리 아이템이 들어갈 배열을 사이즈를 미리 잡아놓는다.(인벤토리 크기는 이미 정해져있으므로 한번에 잡는게 이득)
	// 작으면 늘리고 크면 줄인다.
	//https://docs.unrealengine.com/4.27/ko/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/TArrays/ 
	InventoryItems.SetNumZeroed(MaxInventorySlotCount);

	FInputModeGameAndUI InputMode;
	// 플레이어 입력 모드 조정 이거 때문에 이벤트가 없어서 기고생혔네!
	SetInputMode(InputMode);

	// 실패들
	//SetInputMode(FInputModeGameAndUI());
	//FInputModeGameOnly InputMode;
	//SetInputMode(InputMode);

	AH1Character* H1Character = Cast<AH1Character>(GetPawn());
	if (IsValid(H1Character))
	{
		UPlayUnitHUD_WS* PlayUnitHUD_WS = WidgetMainContainer->PlayUnitHUD;
		if (IsValid(PlayUnitHUD_WS))
		{
			PlayUnitHUD_WS->SetHPRate(H1Character->GetCurrentHPRate());
		}
	}
}


void AH1PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired // 커서로 이동처리 매틱마다..
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AH1PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// 여기서 버튼으로 제어하는 것은 움직임 true false toggle 다른데서 실제로 움직이는 코드가 작동함.
	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AH1PlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AH1PlayerController::OnSetDestinationReleased);

	//// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AH1PlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AH1PlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AH1PlayerController::OnResetVR);

	// 종료 UI에 대한 입력처리 추가.
	InputComponent->BindKey(EKeys::X, EInputEvent::IE_Pressed, this, &AH1PlayerController::IP_Esc);
	// 인벤토리 토글
	InputComponent->BindKey(EKeys::I, EInputEvent::IE_Pressed, this, &AH1PlayerController::IP_Inventory);
}

void AH1PlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AH1PlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (AH1Character* MyPawn = Cast<AH1Character>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				// 여기를 비활성화하면 자동으로 이동메시지 발송이 사라짐
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void AH1PlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AH1PlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AH1PlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AH1PlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void AH1PlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	H1LOG_S(Warning);
}

void AH1PlayerController::OnPossess(APawn* aPawn)
{
	H1LOG_S(Warning);
	Super::OnPossess(aPawn);
}


void AH1PlayerController::IP_Esc()
{
	UExit_WS* Exit_WS = CreateWidget<UExit_WS>(this, UExit_WS::GetWidgetClassStatic());
	if (IsValid(Exit_WS))
	{
		WidgetMainContainer->OpenWidget(Exit_WS, FAnchors(0.5f, 0.5f, 0.5f, 0.5f), FVector2D(0.5f, 0.5f), FVector2D(0.0f, 0.0f));
	}
}

void AH1PlayerController::IP_Inventory()
{
	UInventory_WS* Inventory = WidgetMainContainer->Inventory;
	if (!IsValid(Inventory))
		return;

	Inventory->ToggleInventory();
}

int AH1PlayerController::FindInventoryItemByItemID(int ItemClassID, UH1InventoryItem*& FoundSlot/* = nullptr*/)
{
	for (int i = 0; i < MaxInventorySlotCount; i++)
	{
		UH1InventoryItem* CheckInventoryItem = InventoryItems[i];

		// 인벤토리 아이템이 빈칸이거나 무효하면 다음 칸을 검사하도록 반복문의 처음으로 돌아가서 다음 요소를 검사한다.
		if (nullptr == CheckInventoryItem || !IsValid(CheckInventoryItem))
			continue;

		// 값이 유효하다면 ID가 원하는 것과 일치하는지 검사한다.
		if (CheckInventoryItem->ItemClassID == ItemClassID)
		{
			// 일치한다면 현재 아이템의 포인터를 입출력 매개변수에 저장하고, 현재 인덱스를 반환한다
			FoundSlot = CheckInventoryItem;
			return i;
		}
	}

	return INDEX_NONE;
}

int AH1PlayerController::GetEmptyInvenItem(UH1InventoryItem*& FoundSlot)
{
	for (int i = 0; i < MaxInventorySlotCount; i++)
	{
		// 인벤토리에 빈칸이 있는지 검사하고
		if (nullptr == InventoryItems[i])
		{
			// 빈칸이 있다면 슬롯을 새로운 아이템이 들어갈 자릴르 만들어서
			UH1InventoryItem* NewInventoryItem = NewObject<UH1InventoryItem>(this, UH1InventoryItem::StaticClass());
			InventoryItems[i] = NewInventoryItem;
			// 새로 만든 자리의 주소를 반환함.
			FoundSlot = NewInventoryItem;
			// 인벤토리 인덱스도 반환함.
			return i;
		}
	}
	return INDEX_NONE;
}


bool AH1PlayerController::TryPickingItem(int ItemClassID, int StackCount)
{
	// 아이템 테이블을 불러온다.
	UH1TableManager* TableManager = UH1GameInstance::GetTableManager();
	if (!IsValid(TableManager) || 0 == ItemClassID)
	{
		return false;
	}

	// 픽업을 시도한 아이템의 정보(행)를 테이블에서 로드한다.
	const FItemTableRow* ItemTableRow = TableManager->GetTable<FItemTableRow>(ItemClassID);
	if (nullptr == ItemTableRow)
		return false;

	// 소비재 아이템이라면. 겹치기가 가능해서 다르다. 처리가.
	if (EItemType::Consume == ItemTableRow->ItemType)
	{
		UH1InventoryItem* TargetItem = nullptr;
		int TargetItemIndex = INDEX_NONE;
		bool AddNewSlot = true; // 기본적으로 인벤토리에 아이템과 같은 유형의 아이템이 없고 빈칸이 있는 경우가 true이다.

		// 함수 선언부를 참조하면 함수 역할이 자세하게 써 있다.
		TargetItemIndex = FindInventoryItemByItemID(ItemClassID, TargetItem);

		// FindIbventoryItemByItemID가 유요한 값을 반환했다면 해당 아이템과 같은 유형이 이미 인벤토리에 존재하는 것이다.
		if (INDEX_NONE != TargetItemIndex && IsValid(TargetItem))
			AddNewSlot = false; // 따라서 새로운  슬롯을 만들 필요 없다.
		// 인벤토리에 같은 유형의 아이템이 없으면 인벤토리에 빈칸을 확인하고 빈칸을 사용하도록한다.
		else if (INDEX_NONE != (TargetItemIndex = GetEmptyInvenItem(TargetItem)) && IsValid(TargetItem))
			AddNewSlot = true;
		else
			return false; // 같은 유형의 아이템이 인벤토리에 없고 빈칸도 없다.

		// 인벤토리가 없다면 무효
		UInventory_WS* Inventory = WidgetMainContainer->Inventory;
		if (!IsValid(Inventory))
			return false;

		// 컨트롤러가 가지고 있는 인벤토리 인덱스를 가지고 해당하는 // 인벤토리 슬롯이 없어도 무효 
		UInventorySlot_WS* InventorySlot = Inventory->GetSlot(TargetItemIndex);
		if (!IsValid(InventorySlot))
			return false;

		// 기존 슬롯에 같은 종류의 아이템이 없었고 빈칸이 있었다면 
		if (AddNewSlot)
		{
			// 할당받은 인벤토리 칸에 정보를 누적한다. (컨트롤러 소속이다.)
			TargetItem->SetItemInfo(ItemClassID, StackCount);
			InventorySlot->CleanUp();	// 인벤토리 슬롯을 초기화하고
			InventorySlot->SetItemInfo(TargetItem); // 새로운 아이템의 값으로 인벤토리를 설정한다.
			InventorySlot->UpdateSlot();	// 인벤토리 슬롯의 view를 새로운 스펙으로 갱신한다.
			// 슬롯이 눌리면 하는 처리 함수 등록
			InventorySlot->OnCliekedInventorySlot.AddDynamic(this, &AH1PlayerController::OnclieckedInventorySlot);
			// 슬롯의 컨텍스트 메뉴 처리함수 등록
			InventorySlot->OnCliekedInventoryContext.AddDynamic(this, &AH1PlayerController::OnclieckedContextThrow);

			if (Inventory->RenderOpacity == 1.0f)
			{
				InventorySlot->ShowAnimation();
			}
		}
		else // 소비재이고 같은 종류의 아이템이 이미 존재 한다면. 기본의 칸에 stack을 쌓는다.
		{
			TargetItem->StackCount += StackCount;
			// 컨트롤러가 가진 인벤토리 정보대로 인벤토리 슬롯 UI의 정보도 갱신한다.
			InventorySlot->SetItemInfo(TargetItem);
			InventorySlot->UpdateSlot();
		}
	}
	else // 습득한 아이템이 소비재가 아니라서 쌓기 보관이 불가능한 경우의 처리
	{
		UH1InventoryItem* TargetItem = nullptr;
		int TargetItemIndex = INDEX_NONE;

		TargetItemIndex = GetEmptyInvenItem(TargetItem);
		if (INDEX_NONE == TargetItemIndex /*|| !IsValid(TargetItem)*/)
			return false;

		TargetItem->SetItemInfo(ItemClassID, StackCount);

		UInventory_WS* Inventory = WidgetMainContainer->Inventory;
		if (!IsValid(Inventory))
			return false;

		UInventorySlot_WS* InventorySlot = Inventory->GetSlot(TargetItemIndex);
		if (!IsValid(InventorySlot))
			return false;

		InventorySlot->CleanUp();
		InventorySlot->SetItemInfo(TargetItem);
		InventorySlot->UpdateSlot();
		// 슬롯이 눌리면 하는 처리 함수 등록, 슬롯의 컨텍스트 메뉴 처리함수 등록
		InventorySlot->OnCliekedInventorySlot.AddDynamic(this, &AH1PlayerController::OnclieckedInventorySlot);
		InventorySlot->OnCliekedInventoryContext.AddDynamic(this, &AH1PlayerController::OnclieckedContextThrow);

		// 업데이트된 인벤토리 상태를 UI로 그림.
		if (Inventory->RenderOpacity == 1.0f)
		{
			InventorySlot->ShowAnimation();
		}
	}

	return true;
}


void AH1PlayerController::OnclieckedInventorySlot(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot)
{
	// 슬롯이 유효한가? 
	if (!IsValid(InventorySlot) || !IsValid(InventorySlot->InventoryItemRef))
		return;

	int TargetItemIndex = InventoryItems.Find(InventorySlot->InventoryItemRef);
	if (INDEX_NONE == TargetItemIndex)
		return;

	UH1InventoryItem* TargetInventoryItem = InventoryItems[TargetItemIndex];
	const FItemTableRow* ItemTable = TargetInventoryItem->GetItemTableRow();
	if (nullptr == ItemTable)
		return;

	switch (ItemTable->ItemType)
	{
	case EItemType::Consume:
	{
		AH1Character* H1Character = Cast<AH1Character>(GetPawn());
		if (IsValid(H1Character))
		{
			float HealthRatio = H1Character->Heal(ItemTable->ItemFloat1);

			UPlayUnitHUD_WS* PlayUnitHUD_WS = WidgetMainContainer->PlayUnitHUD;
			if (IsValid(PlayUnitHUD_WS))
			{
				PlayUnitHUD_WS->SetHPRate(HealthRatio);
			}
			// 하나 소모함
			TargetInventoryItem->StackCount--;
			if (TargetInventoryItem->StackCount <= 0)
			{
				// 소모해서 값이 0이하면 없으므로 업데이트
				InventorySlot->CleanUp();
				InventoryItems[TargetItemIndex] = nullptr;
			}
			else // 변한 슬롯 정보 업데이트
			{
				InventorySlot->UpdateSlot();
			}
		}
	}
	break;
	case EItemType::Equip:	// 아이템이 장비라면
	{
		// 캐릭터 포인터를 받아오고 nullptr이면 바로 함수 반환
		AH1Character* H1Character = Cast<AH1Character>(GetPawn());
		if (!IsValid(H1Character))
			break;

		// 눌린 슬롯의 장비아이템이 장착되어 있는 상태이면 장착을 해제한다.
		if (true == TargetInventoryItem->Equipped)
		{
			// 장착해제
			TargetInventoryItem->Equipped = false;
			InventorySlot->UpdateSlot();
			H1Character->ClearWeapon(TargetInventoryItem);
		}
		else // 눌린 슬롯이 장착중 상태가 아니면 장착을 시도한다. 같은 슬롯에 다른 아이템이 장착중이라면 해제하고 현재 아이템으로 교체한다.
		{
			// 현재 장착중인 아이템이 있다면 그정보를 얻어온다.
			UH1InventoryItem* EquipedInventoryItem = nullptr;
			UInventorySlot_WS* EquipedInventorySlot = nullptr;;

			// 장착 중인 아이템의 '인벤토리 아이템'과 '인벤토리 슬롯'을 얻어온다.
			GetEquipedInventoryItem(TargetInventoryItem->GetItemEquipSlot(), EquipedInventoryItem, EquipedInventorySlot);
			
			// 현재 장착중인 아이템이 있다면 해제한다.
			if (IsValid(EquipedInventoryItem))
				EquipedInventoryItem->Equipped = false;

			// 현재 장착중인 아이템의 슬롯을 장착해제 상태로 업데이트한다.
			if (IsValid(EquipedInventorySlot))
				EquipedInventorySlot->UpdateSlot();

			// 장착대상 인벤토리 아이템을 장착상태로 만듬
			TargetInventoryItem->Equipped = true;
			// 장착대상 인벤토리 슬롯을 업데이트
			InventorySlot->UpdateSlot();
			// 해당 아이템을 캐릭터에 장착
			H1Character->EquipWeapon(TargetInventoryItem);
		}
	}
	break;
	}
}

void AH1PlayerController::GetEquipedInventoryItem(EItemEquipSlot ItemEquipSlot, UH1InventoryItem*& InventoryItem, UInventorySlot_WS*& InventorySlot)
{
	InventoryItem = nullptr;
	InventorySlot = nullptr;

	for (int i = 0; i < 8; i++)
	{
		// 인벤토리 아이템이 들어있는 칸을 찾고 아니면 다시 다른 칸을 찾는다.
		UH1InventoryItem* CheckedItem = InventoryItems[i];
		if (IsValid(CheckedItem) == false)
			continue;

		// 찾은 인벤토리 아이템이 장착용 아이템이고 &&
		// 인벤토리 아이템의 장착 소켓(슬롯)이 ItemEquipSlot(소켓)과 일치한다면 동시에 장착 불가능하므로
		// 현재 장착중인 아이템을 장착해제 하기위해서 그 정보를 반환한다.
		if (true == CheckedItem->Equipped && CheckedItem->GetItemEquipSlot() == ItemEquipSlot)
		{
			// 현재 목표 슬롯에 장착중인 인벤토리 아이템을 아웃매개변수에 저장한다.
			InventoryItem = CheckedItem;

			// 인벤토리에 접근한다.
			UInventory_WS* Inventory = WidgetMainContainer->Inventory;
			if (IsValid(Inventory))
			{
				// 현재 목표 슬롯에 장착중인 '인벤토리 아이템'의 '인덱스'로 '인벤토리 슬롯의 주소'를 얻어서 '출력매개변수'에 저장한다.
				InventorySlot = Inventory->GetSlot(i);
			}
			break;
		}
	}
}

void AH1PlayerController::OnclieckedContextThrow(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot)
{
	// 슬롯과 슬롯에 들어있는 아이템이 모두 유효하면
	if (!IsValid(InventorySlot) || !IsValid(InventorySlot->InventoryItemRef))
		return;

	UH1InventoryItem* TargetInventoryItem = InventorySlot->InventoryItemRef;
	const FItemTableRow* ItemTable = TargetInventoryItem->GetItemTableRow();

	// 버릴 대상인 아이템의 인벤토리 인덱스를 가져온다.
	int TargetItemIndex = InventoryItems.Find(TargetInventoryItem);
	if (INDEX_NONE == TargetItemIndex)
		return;


	if (TargetInventoryItem->Equipped) 	// 만약 장착되어 있다면 해제부터 하고 아이템을 컨트롤러와 슬롯에서 제거한다.
	{
		AH1Character* H1Character = Cast<AH1Character>(GetPawn());
		if (IsValid(H1Character))
		{
			// 아이템을 장착 해제하고
			H1Character->ClearWeapon(TargetInventoryItem);
			// 컨트롤러가 가지고있던 아이템을 지우고
			InventoryItems[TargetItemIndex] = nullptr;
			// 슬롯을 지우고
			InventorySlot->CleanUp();
			// 슬롯 정보를 업데이트
			InventorySlot->UpdateSlot();
		}
	}
	else // 장착중이 아니면 바로 아이템을 컨트롤러와 슬롯에서 제거한다.
	{
		InventoryItems[TargetItemIndex] = nullptr;
		InventorySlot->CleanUp();
		InventorySlot->UpdateSlot();
	}

	// 캐릭터 위치를 기반으로 아이템을 바닥에 버린다.
	AH1Character* h1Character = Cast<AH1Character>(GetPawn());
	if (IsValid(h1Character))
	{
		FTransform SpawnTransform;
		FVector RandomAmount = UKismetMathLibrary::RandomUnitVector();
		RandomAmount *= UKismetMathLibrary::RandomFloatInRange(100.0f, 250.0f);
		RandomAmount.Z = 0.0f;

		SpawnTransform.SetLocation(h1Character->GetFootLocation() + RandomAmount);
		// 날름 버그 수정 아이템이 바닥에 붙게 함.
		SpawnTransform.SetLocation(FVector(SpawnTransform.GetLocation().X, SpawnTransform.GetLocation().Y, 0.f));
		SpawnTransform.SetRotation(FRotator(0.0f, UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f), 0.0f).Quaternion());

		// 빈 아이템 엑터를 레벨에 스폰한다.
		AH1Item* ThrowedItem = GetWorld()->SpawnActor<AH1Item>(AH1Item::StaticClass(), SpawnTransform);
		// 레벨에 스폰된 아이템을 설정한다.
		ThrowedItem->SetItemInfo(TargetInventoryItem->ItemClassID, TargetInventoryItem->StackCount);
	}
}


//void Apractice_umgPlayerController::ExchangeInventorySlot(UInventoryItem* SrcInventoryItem, UInventorySlot_WS* SrcInventorySlot, UInventoryItem* DestInventoryItem, UInventorySlot_WS* DestInventorySlot)
//{
//	if (!IsValid(SrcInventoryItem))
//		return;
//
//	int32 srcInvenIndex = InventoryItems.Find(SrcInventoryItem);
//	if (INDEX_NONE == srcInvenIndex)
//		return;
//
//	int32 DestInvenIndex = INDEX_NONE;
//
//	if (!IsValid(DestInventoryItem))
//	{
//		DestInvenIndex = DestInventorySlot->SlotIndex;
//		if (INDEX_NONE == DestInvenIndex)
//			return;
//
//		InventoryItems[DestInvenIndex] = SrcInventoryItem;
//		InventoryItems[srcInvenIndex] = nullptr;
//
//		SrcInventorySlot->CleanUp();
//
//		DestInventorySlot->CleanUp();
//		DestInventorySlot->SetItemInfo(SrcInventoryItem);
//		DestInventorySlot->UpdateSlot();
//		DestInventorySlot->OnCliekedInventorySlot.AddDynamic(this, &Apractice_umgPlayerController::OnclieckedInventorySlot);
//		DestInventorySlot->OnCliekedInventoryContext.AddDynamic(this, &Apractice_umgPlayerController::OnclieckedContextThrow);
//	}
//	else
//	{
//		if (SrcInventoryItem->ItemClassID == DestInventoryItem->ItemClassID && SrcInventoryItem->GetItemTableRow()->ItemType == EItemType::Consume)
//		{
//			DestInvenIndex = InventoryItems.Find(DestInventoryItem);
//			if (INDEX_NONE == DestInvenIndex)
//				return;
//
//			DestInventoryItem->StackCount += SrcInventoryItem->StackCount;
//			InventoryItems[srcInvenIndex] = nullptr;
//
//			SrcInventorySlot->CleanUp();
//			DestInventorySlot->UpdateSlot();
//		}
//		else
//		{
//			DestInvenIndex = InventoryItems.Find(DestInventoryItem);
//			if (INDEX_NONE == DestInvenIndex)
//				return;
//
//			InventoryItems[DestInvenIndex] = SrcInventoryItem;
//			InventoryItems[srcInvenIndex] = DestInventoryItem;
//
//			SrcInventorySlot->SetItemInfo(DestInventoryItem);
//			SrcInventorySlot->UpdateSlot();
//
//			DestInventorySlot->SetItemInfo(SrcInventoryItem);
//			DestInventorySlot->UpdateSlot();
//		}
//	}
//}

//void Apractice_umgPlayerController::SplitInventoryItem(UInventoryItem* SrcInventoryItem, UInventorySlot_WS* SrcInventorySlot, UInventoryItem* DestInventoryItem, UInventorySlot_WS* DestInventorySlot)
//{
//	if (!IsValid(SrcInventoryItem))
//		return;
//
//	int32 srcInvenIndex = InventoryItems.Find(SrcInventoryItem);
//	if (INDEX_NONE == srcInvenIndex)
//		return;
//
//	int HalfStackCount = SrcInventoryItem->StackCount / 2.0f;
//	if (0 == HalfStackCount)
//		return;
//
//	if (!IsValid(DestInventoryItem))
//	{
//		int DestInvenSlotIndex = DestInventorySlot->SlotIndex;
//		if (INDEX_NONE == DestInvenSlotIndex)
//			return;
//
//		SrcInventoryItem->StackCount -= HalfStackCount;
//		SrcInventorySlot->UpdateSlot();
//
//		UInventoryItem* NewInventoryItem = NewObject<UInventoryItem>();
//		NewInventoryItem->SetItemInfo(SrcInventoryItem->ItemClassID, HalfStackCount);
//		InventoryItems[DestInvenSlotIndex] = NewInventoryItem;
//
//		DestInventorySlot->CleanUp();
//		DestInventorySlot->SetItemInfo(NewInventoryItem);
//		DestInventorySlot->UpdateSlot();
//		DestInventorySlot->OnCliekedInventorySlot.AddDynamic(this, &Apractice_umgPlayerController::OnclieckedInventorySlot);
//		DestInventorySlot->OnCliekedInventoryContext.AddDynamic(this, &Apractice_umgPlayerController::OnclieckedContextThrow);
//	}
//	else
//	{
//		if (SrcInventoryItem->ItemClassID == DestInventoryItem->ItemClassID)
//		{
//			SrcInventoryItem->StackCount -= HalfStackCount;
//			SrcInventorySlot->UpdateSlot();
//
//			DestInventoryItem->StackCount += HalfStackCount;
//			DestInventorySlot->UpdateSlot();
//		}
//		else
//		{
//			ExchangeInventorySlot(SrcInventoryItem, SrcInventorySlot, DestInventoryItem, DestInventorySlot);
//		}
//	}
//}









