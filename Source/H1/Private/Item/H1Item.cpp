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
	// 충돌체 설정하기
	ItemSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSphere"));
	ItemSphere->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ItemSphere->SetSphereRadius(150.0f);
	ItemSphere->OnComponentBeginOverlap.AddDynamic(this, &AH1Item::OnSphereBeginOverlap);
	ItemSphere->OnComponentEndOverlap.AddDynamic(this, &AH1Item::OnSphereEndOverlap);
	SetRootComponent(ItemSphere);

	// TODO : 아이템(줍기용) 에로우 컴포넌트 이거먼 봐서는 어디 쓰느지 모르겠는데 ..??
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);


	// Item Mesh~ 이미 충돌은 스피어가 처리하니까. pawn하고 충돌처리가 필요없고 하면 오히려 성능까지 낭비임.
	// 나중에 버튼없이 지나가면 먹는 기능 만든다면 변경할 가능성 있음
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);
	ItemMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	ItemMesh->SetRelativeScale3D(FVector(1.5f));
	ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// 아이템 클래스 ID 기본값은 0이다.
	ItemClassID = 0;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AH1Item::BeginPlay()
{
	Super::BeginPlay();
	
	// 게임이 시작하면 이름표 위젯을 생성한다.
	ItemNameWidget = CreateWidget<UItemName_WS>(UGameplayStatics::GetPlayerController(GetWorld(), 0), UItemName_WS::GetWidgetClassStatic());
	// 이름표 위젯의 버튼이 눌리면 아이템이 수행하는 로직들을 담은 함수를 이름표에 멤버 델리게이트에 바인드한다.
	ItemNameWidget->OnItemNameButtonClicked.AddDynamic(this, &AH1Item::OnItemWidgetButtonClicked);

	// 에디터에서 지정한 값으로 아이템을 설정한다. (DT에서 불러옴)
	SetItemInfo(ItemClassID, StackCount);
}

// Called every frame
void AH1Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 아이템이 맵에서 이동하는 것을 따라서 이름표도 이동한다.
	AH1PlayerController* H1PlayerController = Cast<AH1PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(ItemNameWidget) && ItemNameWidget->GetIsVisible()/*->GetVisibility() == ESlateVisibility::Visible*/)
	{
		FVector2D ScreenPos;
		
		// 3D 상의 아이템 위치로부터 스크린상에서 아이템 위치를 얻어온다. 
		if (H1PlayerController->ProjectWorldLocationToScreen(GetActorLocation(), ScreenPos))
		{
			// 이름표 위젯의 위치를 얻어온 스크린상의 아이템 위치로 업데이트 한다.
			ItemNameWidget->SetPositionInViewport(ScreenPos);
		}
	}

}
#if WITH_EDITOR
void AH1Item::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 속성이 유효하면 속성으로부터 속성 이름을 얻어 옵니다.
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
	// 이 범위에 안에 들어오면 아이템의 이름을 표시하는 UI를 띄웁니다.
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
	// 범위 밖으로 나가면 아이템 이름 UI를 지웁니다.
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

