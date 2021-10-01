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

	// ���� ������ �����ؼ� ����Ʈ�� �׸���.
	WidgetMainContainer = CreateWidget<UH1HUDContainer_WS>(this, UH1HUDContainer_WS::GetWidgetClassStatic());
	if (IsValid(WidgetMainContainer) && IsLocalPlayerController())
	{
		WidgetMainContainer->AddToViewport(0);

	}

	// ü�°� �����ۿ� ����  ������.-----------------------------------------------------------
	
	// �κ��丮 �������� �� �迭�� ����� �̸� ��Ƴ��´�.(�κ��丮 ũ��� �̹� �����������Ƿ� �ѹ��� ��°� �̵�)
	// ������ �ø��� ũ�� ���δ�.
	//https://docs.unrealengine.com/4.27/ko/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/TArrays/ 
	InventoryItems.SetNumZeroed(MaxInventorySlotCount);

	FInputModeGameAndUI InputMode;
	// �÷��̾� �Է� ��� ���� �̰� ������ �̺�Ʈ�� ��� ��������!
	SetInputMode(InputMode);

	// ���е�
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

	// keep updating the destination every tick while desired // Ŀ���� �̵�ó�� ��ƽ����..
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AH1PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// ���⼭ ��ư���� �����ϴ� ���� ������ true false toggle �ٸ����� ������ �����̴� �ڵ尡 �۵���.
	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AH1PlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AH1PlayerController::OnSetDestinationReleased);

	//// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AH1PlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AH1PlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AH1PlayerController::OnResetVR);

	// ���� UI�� ���� �Է�ó�� �߰�.
	InputComponent->BindKey(EKeys::X, EInputEvent::IE_Pressed, this, &AH1PlayerController::IP_Esc);
	// �κ��丮 ���
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
				// ���⸦ ��Ȱ��ȭ�ϸ� �ڵ����� �̵��޽��� �߼��� �����
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

		// �κ��丮 �������� ��ĭ�̰ų� ��ȿ�ϸ� ���� ĭ�� �˻��ϵ��� �ݺ����� ó������ ���ư��� ���� ��Ҹ� �˻��Ѵ�.
		if (nullptr == CheckInventoryItem || !IsValid(CheckInventoryItem))
			continue;

		// ���� ��ȿ�ϴٸ� ID�� ���ϴ� �Ͱ� ��ġ�ϴ��� �˻��Ѵ�.
		if (CheckInventoryItem->ItemClassID == ItemClassID)
		{
			// ��ġ�Ѵٸ� ���� �������� �����͸� ����� �Ű������� �����ϰ�, ���� �ε����� ��ȯ�Ѵ�
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
		// �κ��丮�� ��ĭ�� �ִ��� �˻��ϰ�
		if (nullptr == InventoryItems[i])
		{
			// ��ĭ�� �ִٸ� ������ ���ο� �������� �� �ڸ��� ����
			UH1InventoryItem* NewInventoryItem = NewObject<UH1InventoryItem>(this, UH1InventoryItem::StaticClass());
			InventoryItems[i] = NewInventoryItem;
			// ���� ���� �ڸ��� �ּҸ� ��ȯ��.
			FoundSlot = NewInventoryItem;
			// �κ��丮 �ε����� ��ȯ��.
			return i;
		}
	}
	return INDEX_NONE;
}


bool AH1PlayerController::TryPickingItem(int ItemClassID, int StackCount)
{
	// ������ ���̺��� �ҷ��´�.
	UH1TableManager* TableManager = UH1GameInstance::GetTableManager();
	if (!IsValid(TableManager) || 0 == ItemClassID)
	{
		return false;
	}

	// �Ⱦ��� �õ��� �������� ����(��)�� ���̺��� �ε��Ѵ�.
	const FItemTableRow* ItemTableRow = TableManager->GetTable<FItemTableRow>(ItemClassID);
	if (nullptr == ItemTableRow)
		return false;

	// �Һ��� �������̶��. ��ġ�Ⱑ �����ؼ� �ٸ���. ó����.
	if (EItemType::Consume == ItemTableRow->ItemType)
	{
		UH1InventoryItem* TargetItem = nullptr;
		int TargetItemIndex = INDEX_NONE;
		bool AddNewSlot = true; // �⺻������ �κ��丮�� �����۰� ���� ������ �������� ���� ��ĭ�� �ִ� ��찡 true�̴�.

		// �Լ� ����θ� �����ϸ� �Լ� ������ �ڼ��ϰ� �� �ִ�.
		TargetItemIndex = FindInventoryItemByItemID(ItemClassID, TargetItem);

		// FindIbventoryItemByItemID�� ������ ���� ��ȯ�ߴٸ� �ش� �����۰� ���� ������ �̹� �κ��丮�� �����ϴ� ���̴�.
		if (INDEX_NONE != TargetItemIndex && IsValid(TargetItem))
			AddNewSlot = false; // ���� ���ο�  ������ ���� �ʿ� ����.
		// �κ��丮�� ���� ������ �������� ������ �κ��丮�� ��ĭ�� Ȯ���ϰ� ��ĭ�� ����ϵ����Ѵ�.
		else if (INDEX_NONE != (TargetItemIndex = GetEmptyInvenItem(TargetItem)) && IsValid(TargetItem))
			AddNewSlot = true;
		else
			return false; // ���� ������ �������� �κ��丮�� ���� ��ĭ�� ����.

		// �κ��丮�� ���ٸ� ��ȿ
		UInventory_WS* Inventory = WidgetMainContainer->Inventory;
		if (!IsValid(Inventory))
			return false;

		// ��Ʈ�ѷ��� ������ �ִ� �κ��丮 �ε����� ������ �ش��ϴ� // �κ��丮 ������ ��� ��ȿ 
		UInventorySlot_WS* InventorySlot = Inventory->GetSlot(TargetItemIndex);
		if (!IsValid(InventorySlot))
			return false;

		// ���� ���Կ� ���� ������ �������� ������ ��ĭ�� �־��ٸ� 
		if (AddNewSlot)
		{
			// �Ҵ���� �κ��丮 ĭ�� ������ �����Ѵ�. (��Ʈ�ѷ� �Ҽ��̴�.)
			TargetItem->SetItemInfo(ItemClassID, StackCount);
			InventorySlot->CleanUp();	// �κ��丮 ������ �ʱ�ȭ�ϰ�
			InventorySlot->SetItemInfo(TargetItem); // ���ο� �������� ������ �κ��丮�� �����Ѵ�.
			InventorySlot->UpdateSlot();	// �κ��丮 ������ view�� ���ο� �������� �����Ѵ�.
			// ������ ������ �ϴ� ó�� �Լ� ���
			InventorySlot->OnCliekedInventorySlot.AddDynamic(this, &AH1PlayerController::OnclieckedInventorySlot);
			// ������ ���ؽ�Ʈ �޴� ó���Լ� ���
			InventorySlot->OnCliekedInventoryContext.AddDynamic(this, &AH1PlayerController::OnclieckedContextThrow);

			if (Inventory->RenderOpacity == 1.0f)
			{
				InventorySlot->ShowAnimation();
			}
		}
		else // �Һ����̰� ���� ������ �������� �̹� ���� �Ѵٸ�. �⺻�� ĭ�� stack�� �״´�.
		{
			TargetItem->StackCount += StackCount;
			// ��Ʈ�ѷ��� ���� �κ��丮 ������� �κ��丮 ���� UI�� ������ �����Ѵ�.
			InventorySlot->SetItemInfo(TargetItem);
			InventorySlot->UpdateSlot();
		}
	}
	else // ������ �������� �Һ��簡 �ƴ϶� �ױ� ������ �Ұ����� ����� ó��
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
		// ������ ������ �ϴ� ó�� �Լ� ���, ������ ���ؽ�Ʈ �޴� ó���Լ� ���
		InventorySlot->OnCliekedInventorySlot.AddDynamic(this, &AH1PlayerController::OnclieckedInventorySlot);
		InventorySlot->OnCliekedInventoryContext.AddDynamic(this, &AH1PlayerController::OnclieckedContextThrow);

		// ������Ʈ�� �κ��丮 ���¸� UI�� �׸�.
		if (Inventory->RenderOpacity == 1.0f)
		{
			InventorySlot->ShowAnimation();
		}
	}

	return true;
}


void AH1PlayerController::OnclieckedInventorySlot(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot)
{
	// ������ ��ȿ�Ѱ�? 
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
			// �ϳ� �Ҹ���
			TargetInventoryItem->StackCount--;
			if (TargetInventoryItem->StackCount <= 0)
			{
				// �Ҹ��ؼ� ���� 0���ϸ� �����Ƿ� ������Ʈ
				InventorySlot->CleanUp();
				InventoryItems[TargetItemIndex] = nullptr;
			}
			else // ���� ���� ���� ������Ʈ
			{
				InventorySlot->UpdateSlot();
			}
		}
	}
	break;
	case EItemType::Equip:	// �������� �����
	{
		// ĳ���� �����͸� �޾ƿ��� nullptr�̸� �ٷ� �Լ� ��ȯ
		AH1Character* H1Character = Cast<AH1Character>(GetPawn());
		if (!IsValid(H1Character))
			break;

		// ���� ������ ���������� �����Ǿ� �ִ� �����̸� ������ �����Ѵ�.
		if (true == TargetInventoryItem->Equipped)
		{
			// ��������
			TargetInventoryItem->Equipped = false;
			InventorySlot->UpdateSlot();
			H1Character->ClearWeapon(TargetInventoryItem);
		}
		else // ���� ������ ������ ���°� �ƴϸ� ������ �õ��Ѵ�. ���� ���Կ� �ٸ� �������� �������̶�� �����ϰ� ���� ���������� ��ü�Ѵ�.
		{
			// ���� �������� �������� �ִٸ� �������� ���´�.
			UH1InventoryItem* EquipedInventoryItem = nullptr;
			UInventorySlot_WS* EquipedInventorySlot = nullptr;;

			// ���� ���� �������� '�κ��丮 ������'�� '�κ��丮 ����'�� ���´�.
			GetEquipedInventoryItem(TargetInventoryItem->GetItemEquipSlot(), EquipedInventoryItem, EquipedInventorySlot);
			
			// ���� �������� �������� �ִٸ� �����Ѵ�.
			if (IsValid(EquipedInventoryItem))
				EquipedInventoryItem->Equipped = false;

			// ���� �������� �������� ������ �������� ���·� ������Ʈ�Ѵ�.
			if (IsValid(EquipedInventorySlot))
				EquipedInventorySlot->UpdateSlot();

			// ������� �κ��丮 �������� �������·� ����
			TargetInventoryItem->Equipped = true;
			// ������� �κ��丮 ������ ������Ʈ
			InventorySlot->UpdateSlot();
			// �ش� �������� ĳ���Ϳ� ����
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
		// �κ��丮 �������� ����ִ� ĭ�� ã�� �ƴϸ� �ٽ� �ٸ� ĭ�� ã�´�.
		UH1InventoryItem* CheckedItem = InventoryItems[i];
		if (IsValid(CheckedItem) == false)
			continue;

		// ã�� �κ��丮 �������� ������ �������̰� &&
		// �κ��丮 �������� ���� ����(����)�� ItemEquipSlot(����)�� ��ġ�Ѵٸ� ���ÿ� ���� �Ұ����ϹǷ�
		// ���� �������� �������� �������� �ϱ����ؼ� �� ������ ��ȯ�Ѵ�.
		if (true == CheckedItem->Equipped && CheckedItem->GetItemEquipSlot() == ItemEquipSlot)
		{
			// ���� ��ǥ ���Կ� �������� �κ��丮 �������� �ƿ��Ű������� �����Ѵ�.
			InventoryItem = CheckedItem;

			// �κ��丮�� �����Ѵ�.
			UInventory_WS* Inventory = WidgetMainContainer->Inventory;
			if (IsValid(Inventory))
			{
				// ���� ��ǥ ���Կ� �������� '�κ��丮 ������'�� '�ε���'�� '�κ��丮 ������ �ּ�'�� �� '��¸Ű�����'�� �����Ѵ�.
				InventorySlot = Inventory->GetSlot(i);
			}
			break;
		}
	}
}

void AH1PlayerController::OnclieckedContextThrow(UH1InventoryItem* InventoryItem, UInventorySlot_WS* InventorySlot)
{
	// ���԰� ���Կ� ����ִ� �������� ��� ��ȿ�ϸ�
	if (!IsValid(InventorySlot) || !IsValid(InventorySlot->InventoryItemRef))
		return;

	UH1InventoryItem* TargetInventoryItem = InventorySlot->InventoryItemRef;
	const FItemTableRow* ItemTable = TargetInventoryItem->GetItemTableRow();

	// ���� ����� �������� �κ��丮 �ε����� �����´�.
	int TargetItemIndex = InventoryItems.Find(TargetInventoryItem);
	if (INDEX_NONE == TargetItemIndex)
		return;


	if (TargetInventoryItem->Equipped) 	// ���� �����Ǿ� �ִٸ� �������� �ϰ� �������� ��Ʈ�ѷ��� ���Կ��� �����Ѵ�.
	{
		AH1Character* H1Character = Cast<AH1Character>(GetPawn());
		if (IsValid(H1Character))
		{
			// �������� ���� �����ϰ�
			H1Character->ClearWeapon(TargetInventoryItem);
			// ��Ʈ�ѷ��� �������ִ� �������� �����
			InventoryItems[TargetItemIndex] = nullptr;
			// ������ �����
			InventorySlot->CleanUp();
			// ���� ������ ������Ʈ
			InventorySlot->UpdateSlot();
		}
	}
	else // �������� �ƴϸ� �ٷ� �������� ��Ʈ�ѷ��� ���Կ��� �����Ѵ�.
	{
		InventoryItems[TargetItemIndex] = nullptr;
		InventorySlot->CleanUp();
		InventorySlot->UpdateSlot();
	}

	// ĳ���� ��ġ�� ������� �������� �ٴڿ� ������.
	AH1Character* h1Character = Cast<AH1Character>(GetPawn());
	if (IsValid(h1Character))
	{
		FTransform SpawnTransform;
		FVector RandomAmount = UKismetMathLibrary::RandomUnitVector();
		RandomAmount *= UKismetMathLibrary::RandomFloatInRange(100.0f, 250.0f);
		RandomAmount.Z = 0.0f;

		SpawnTransform.SetLocation(h1Character->GetFootLocation() + RandomAmount);
		// ���� ���� ���� �������� �ٴڿ� �ٰ� ��.
		SpawnTransform.SetLocation(FVector(SpawnTransform.GetLocation().X, SpawnTransform.GetLocation().Y, 0.f));
		SpawnTransform.SetRotation(FRotator(0.0f, UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f), 0.0f).Quaternion());

		// �� ������ ���͸� ������ �����Ѵ�.
		AH1Item* ThrowedItem = GetWorld()->SpawnActor<AH1Item>(AH1Item::StaticClass(), SpawnTransform);
		// ������ ������ �������� �����Ѵ�.
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









