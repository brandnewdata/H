// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/H1Item.h"
#include "Runtime/Engine/Classes/Components/SphereComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Engine/DataTable.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Item/H1ItemTableDef.h"
#include "CoreExtentsion/H1_singleton.h"
#include "CoreExtentsion/H1GameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "CoreExtentsion/H1TableManager.h"
#include "UMG/ItemName_WS.h"
#include "..\Source\H1\H1PlayerController.h"
#include "..\Source\H1\H1Character.h"


// Sets default values
AH1Item::AH1Item()
{
	// �浹ü �����ϱ�
	ItemSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSphere"));
	ItemSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ItemSphere->SetSphereRadius(150.0f);
	ItemSphere->OnComponentBeginOverlap.AddDynamic(this, &AH1Item::OnSphereBeginOverlap);
	ItemSphere->OnComponentEndOverlap.AddDynamic(this, &AH1Item::OnSphereEndOverlap);
	SetRootComponent(ItemSphere);

	// TODO : ������(�ݱ��) ���ο� ������Ʈ �̰Ÿ� ������ ��� ������ �𸣰ڴµ� ..??
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);


	// Item Mesh~ �̹� �浹�� ���Ǿ ó���ϴϱ�. pawn�ϰ� �浹ó���� �ʿ���� �ϸ� ������ ���ɱ��� ������.
	// ���߿� ��ư���� �������� �Դ� ��� ����ٸ� ������ ���ɼ� ����
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	ItemMesh->SetRelativeScale3D(FVector(1.5f));
	ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// ������ Ŭ���� ID �⺻���� 0�̴�.
	ItemClassID = 0;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AH1Item::BeginPlay()
{
	Super::BeginPlay();
	
	// ������ �����ϸ� �̸�ǥ ������ �����Ѵ�.
	ItemNameWidget = CreateWidget<UItemName_WS>(UGameplayStatics::GetPlayerController(GetWorld(), 0), UItemName_WS::GetWidgetClassStatic());
	// �̸�ǥ ������ ��ư�� ������ �������� �����ϴ� �������� ���� �Լ��� �̸�ǥ�� ��� ��������Ʈ�� ���ε��Ѵ�.
	ItemNameWidget->OnItemNameButtonClicked.AddDynamic(this, &AH1Item::OnItemWidgetButtonClicked);

	// �����Ϳ��� ������ ������ �������� �����Ѵ�. (DT���� �ҷ���)
	SetItemInfo(ItemClassID, StackCount);
}

// Called every frame
void AH1Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �������� �ʿ��� �̵��ϴ� ���� ���� �̸�ǥ�� �̵��Ѵ�.
	AH1PlayerController* H1PlayerController = Cast<AH1PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(ItemNameWidget) && ItemNameWidget->GetIsVisible()/*->GetVisibility() == ESlateVisibility::Visible*/)
	{
		FVector2D ScreenPos;
		
		// 3D ���� ������ ��ġ�κ��� ��ũ���󿡼� ������ ��ġ�� ���´�. 
		if (H1PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenPos))
		{
			// �̸�ǥ ������ ��ġ�� ���� ��ũ������ ������ ��ġ�� ������Ʈ �Ѵ�.
			ItemNameWidget->SetPositionInViewport(ScreenPos);
		}
	}

}
#if WITH_EDITOR
void AH1Item::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// �Ӽ��� ��ȿ�ϸ� �Ӽ����κ��� �Ӽ� �̸��� ��� �ɴϴ�.
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AH1Item, ItemClassID))
	{
		SetItemInfo(ItemClassID, StackCount);
	}

}
#endif

bool AH1Item::SetItemInfo(int DataTableID, int NewStackCount)
{
	const FItemTableRow* ItemTable = nullptr;

	UH1TableManager* TableManager = UH1GameInstance::GetTableManager();
	if (!IsValid(TableManager))
		return false;

	if (0 == DataTableID)
		ItemTable = nullptr;
	else
		ItemTable = TableManager->GetTable<FItemTableRow>(DataTableID);

	if (nullptr == ItemTable)
		return false;

	ItemClassID = ItemTable->ItemClassID;
	ItemMesh->SetStaticMesh(Cast<UStaticMesh>(ItemTable->ItemModel.TryLoad()));

	ItemClassID = DataTableID;
	StackCount = NewStackCount;

	if (IsValid(ItemNameWidget))
	{
		FString ItemName;
		ItemName = ItemTable->ItemName;
		if (StackCount > 1)
		{
			ItemName += TEXT("(");
			ItemName += FString::FromInt(StackCount);
			ItemName += TEXT(")");
		}

		ItemNameWidget->SetItemName(ItemName);
		AH1PlayerController* H1PlayerController = Cast<AH1PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		ItemNameWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	}

	return true;
}


void AH1Item::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �� ������ �ȿ� ������ �������� �̸��� ǥ���ϴ� UI�� ���ϴ�.
	if (OtherComp->ComponentHasTag(FName(TEXT("ItemPickingBoundary"))) == false)
		return;

	if (Cast<AH1Character>(OtherActor))
	{
		if (IsValid(ItemNameWidget))
		{
			if (!ItemNameWidget->GetIsVisible())
			{
				ItemNameWidget->AddToViewport();
			}
			ItemNameWidget->Showing();
		}
	}
}


void AH1Item::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// ���� ������ ������ ������ �̸� UI�� ����ϴ�.
	if (OtherComp->ComponentHasTag(FName(TEXT("ItemPickingBoundary"))) == false)
		return;

	if (Cast<AH1Character>(OtherActor))
	{
		if (IsValid(ItemNameWidget))
		{
			ItemNameWidget->Hiding();
		}
	}
}

void AH1Item::OnItemWidgetButtonClicked()
{
	AH1PlayerController* H1PlayerController = Cast<AH1PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(H1PlayerController))
	{
		if (H1PlayerController->TryPickingItem(ItemClassID, StackCount) == true)
		{
			Destroy();
		}
	}
}

