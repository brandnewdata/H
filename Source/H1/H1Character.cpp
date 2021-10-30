// Copyright Epic Games, Inc. All Rights Reserved.

#include "H1Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"

#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"
#include "Item/H1InventoryItem.h"
#include "ITem/H1ItemTableDef.h"

#include "Animation/H1AnimInstance.h"

AH1Character::AH1Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// 아이템의 바운드리 스피어와 충돌하면 이 태그를 가졌는지 물어본다 그래서 필요함
	// 지금은 딱히 필요 없는 것 같기도한데..? 아마 적 AI들도 pawn이니까 플레이어 구분하는 용도 인 듯.
	// 플레이어 전용 충돌 채널을 고려해보자.
	GetCapsuleComponent()->ComponentTags.Add(FName("ItemPickingBoundary"));

	// Skeletal Mesh 설정하기
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90, 0.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ROBO(TEXT("/Game/H1/PC_CharM_Robo/SK_CharM_Robo.SK_CharM_Robo"));
	if (SK_ROBO.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_ROBO.Object);
	}

	// Skeletal Mesh에 애니메이션 블루프린트 세팅하기.
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> ROBO_ANIM(TEXT("/Game/H1/PlayerAnims/AMBP_PC.AMBP_PC_C"));
	if (SK_ROBO.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(ROBO_ANIM.Class);
	}

	WeaponBack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponBack"));;
	WeaponBack->SetupAttachment(GetMesh(), FName(TEXT("WeaponBack")));

	WeaponRightHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponRightHand"));;
	WeaponRightHand->SetupAttachment(GetMesh(), FName(TEXT("WeaponRightHand")));

	WeaponLeftHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponLeftHand"));;
	WeaponLeftHand->SetupAttachment(GetMesh(), FName(TEXT("WeaponLeftHand")));

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

															 // Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// 컨트롤 모드 설정 >> 초기값: GTA -> CDO : 디아블로
	SetControlMode(EPlayerControlMode::GTA);

	// 점프 높이를 설정함.
	GetCharacterMovement()->JumpZVelocity = 500.0f;

	// 카메라 시점변환 장식성 코드
	ArmLengthSpeed = 3.f;		// 틱당 location이 3씩 이동함
	ArmRotationSpeed = 10.f;	// 틱당 각도가 10도씩 돌아감

	// combo attack initialization
	FinalComboIndex = 4;
	AttackEndComboState();

	// 전용 Collision preset으로 설정 합니다.
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("H1Character"));
}

void AH1Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// TODO : 더 성능좋은 방식 찾아서 검증하기 ... 매틱마다 업데이트 vs 매 틱마다 float값 확인(float용 equal 비교함수 사용...)
	// 매틱마다 목표 위치까지 카메라 위치가 이동한다.(카메라 막대기가 길어짐. 최종 위치에 도달해고 계속 같은 값으로 덮어쓰게되는 구조)
	// 어차피 0,1인데 그냥 if쓰는게 나을지도. 다른 시점도 추가할 생각인가?
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, ArmLengthTo, DeltaSeconds, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EPlayerControlMode::DIABLO:
		CameraBoom->SetRelativeRotation(FMath::RInterpTo(CameraBoom->GetRelativeRotation(), ArmRotationTo, DeltaSeconds, ArmRotationSpeed));
		break;
	}


	// 현재 모드에 따라서 다른처리
	switch (CurrentControlMode)
	{
	case EPlayerControlMode::DIABLO:
		// 사이즈 스쿼드 이거는 내적인데??
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			// 컨트롤러의 회전을 저장해둔 회전값으로 설정한다.
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());

			// TODO : 그런데 이거 대각선 누르면 속도가 올라가는거 아닌가?
			// 키 이벤트로 입력받은 방향키 값만큼 캐릭터를 이동한다.
			AddMovementInput(DirectionToMove);
		}
		break;
	}

	// 커서 위치 업데이트를 매 틱마다 처리함.
	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}


void AH1Character::ViewChange()
{
	switch (CurrentControlMode)
	{
	case EPlayerControlMode::GTA:
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EPlayerControlMode::DIABLO);
		break;
	case EPlayerControlMode::DIABLO:
		GetController()->SetControlRotation(CameraBoom->GetRelativeRotation());
		SetControlMode(EPlayerControlMode::GTA);
		break;
	}
}

void AH1Character::SetControlMode(EPlayerControlMode NewControlMode)
{
	// 현재 컨트롤 모드 변경
	CurrentControlMode = NewControlMode;

	// 컨트롤 모드에 따라서 다르게 셋팅함
	switch (CurrentControlMode)
	{
	case EPlayerControlMode::GTA:
		// 이제 카메라 위치는 매 틱마다 업데이트 됩니다. 
		// CameraBoom->TargetArmLength = 500.f;
		// CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
		ArmLengthTo = 450.f;
		// 카메라 각도는 원래 자유각도라서 무의미

		// 카메라가 컨트로러에 의해서 회전한다.
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bInheritPitch = true;
		CameraBoom->bInheritRoll = true;
		CameraBoom->bInheritYaw = true;

		// 카메라가 유동적이면 안되니까 충돌처리함
		CameraBoom->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;

		// 캐릭터가 회전하는 속도를 사용함
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// TODO : 이거 뭔데? 캐릭터 회전 천천히 돌릴지 빨리 돌릴지랑 연관 있는 듯. 
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		// 캐릭터가 초당 720도 회전 가능함
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
		break;
	case EPlayerControlMode::DIABLO:
		// CameraBoom->TargetArmLength = 800.f;
		// CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
		ArmLengthTo = 800.f;
		ArmRotationTo = FRotator(-45.f, 0.f, 0.f);

		// Don't rotate character to camera direction
		CameraBoom->bUsePawnControlRotation = false;
		CameraBoom->bInheritPitch = false;
		CameraBoom->bInheritRoll = false;
		CameraBoom->bInheritYaw = false;

		// Don't want to pull camera in when it collides with level
		// 카메라가 고정이라서 충돌처리 ㄴㄴ
		CameraBoom->bDoCollisionTest = false;
		// 컨트롤러의 회전으로 카메라가 아니라 캐릭터가 움직임.(마우스 이동과 일치함)
		bUseControllerRotationYaw = false;

		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);

		break;
	}
	
}

void AH1Character::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	playerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AH1Character::ViewChange);

	playerInputComponent->BindAxis(TEXT("UpDown"), this, &AH1Character::UpDown);
	playerInputComponent->BindAxis(TEXT("LeftRight"), this, &AH1Character::LeftRight);

	// GTA식 플레이어 캐릭터 조작일 경우에는 마우스 XY로 시점 변환 
	playerInputComponent->BindAxis(TEXT("Turn"), this, &AH1Character::Turn);
	playerInputComponent->BindAxis(TEXT("LookUp"), this, &AH1Character::LookUp);

	// 점프키 처리 함수 바인드, 상속받은 함수를 그대로 사용함.
	playerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	// Attack키에 처리 함수 바인드
	playerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AH1Character::Attack);
}

void AH1Character::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		// GTA 모드인 경우에만 카메라 시점이 움직인다. 
	case EPlayerControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	}
}

void AH1Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	H1AnimInst = Cast<UH1AnimInstance>(GetMesh()->GetAnimInstance());

	H1CHECK(H1AnimInst != nullptr);

	// 1. 이 람다 함수는 AnimMontage Notify_NextAttaCheck에 연결된 델리게이트를 통해서 호출 됩니다.
	// 2. 몽타주 재생 중에  다음 콤보 입력이 있었는지 확인하고 있다면 다음 몽타주 섹션을 재생하도록 합니다.
	// 3. 마지막 콤보어택에서는 NextAttackCheck 노티파이가 없습니다. 그래서 마지막 어택 후에는 애니메이션 종료까지 어택 입력이 무시됩니다.
	H1AnimInst->OnNextAttackCheck.AddLambda([this]()->void
	{
		H1LOG(Warning, TEXT("OnNextAttackCheck"));
		bCanNextCombo = false; // 콤보어택 시작

		if (bIsComboInputOn) 
		{
			AttackStartComboState();
			H1AnimInst->JumpToOtherAttackMontageSection(CurrentComboIndex);
		}
	}
	);

	H1AnimInst->OnMontageEnded.AddDynamic(this, &AH1Character::OnAttackMontageEnded); // 이렇게 하면 모든 몽타주가 끝날때마다 호출됨
	// 몽타주가 여러개가 되면 처리 함수에서 분기처리가 필요하다.

	// 애니메이션의 노티파이가 타격 지점에 도달하면 대리자에 등록한 함수가 호출된다.  
	H1AnimInst->OnAttackHitCheck.AddUObject(this, &AH1Character::AttackCheck);
}

float AH1Character::Heal(float AmountOfHeal)
{
	HP += AmountOfHeal;
	// 체력이 최대 체력 이하가 되야함.
	HP = FMath::Clamp(HP, 0.0f, MaxHP);

	// 체력 UI 표시를 위해서 몇퍼센트가 차있는지 반환함.
	return HP / MaxHP;
}

void AH1Character::EquipWeapon(UH1InventoryItem* InventoryItem)
{
	if (!IsValid(InventoryItem))
		return;

	// 인벤토리 아이템을 생성하는데 사용한 DT 행의 포인터를 받아온다.
	const FItemTableRow* ItemTableRow = InventoryItem->GetItemTableRow();
	// nullptr이면 무효
	if (nullptr == ItemTableRow)
		return;

	// 데이터 테이블 정보로 부터 장비 메쉬를 생성한다.
	UStaticMesh* ItemMesh = Cast<UStaticMesh>(ItemTableRow->ItemModel.TryLoad());
	if (!IsValid(ItemMesh))
		return;

	// 장비 아이템의 소켓 위치에 따라서 캐릭터에 생성되어있는 메쉬 컴포넌트에 메쉬로 설정한다.
	// 메쉬 컴포넌트는 이미 소켓위치에 부착되어 있다.
	switch (ItemTableRow->ItemEquipSlot)
	{
	case EItemEquipSlot::WeaponBack:
		WeaponBack->SetStaticMesh(ItemMesh);
		break;
	case EItemEquipSlot::WeaponLeftHand:
		WeaponRightHand->SetStaticMesh(ItemMesh);
		break;
	case EItemEquipSlot::WeaponRightHand:
		WeaponLeftHand->SetStaticMesh(ItemMesh);
		break;
	}
}

void AH1Character::ClearWeapon(UH1InventoryItem* InventoryItem)
{
	if (!IsValid(InventoryItem))
		return;

	const FItemTableRow* ItemTable = InventoryItem->GetItemTableRow();
	if (nullptr == ItemTable)
		return;

	// 해당하는 메쉬 컴포넌트의 메쉬를 제거한다.
	switch (ItemTable->ItemEquipSlot)
	{
	case EItemEquipSlot::WeaponBack:
		WeaponBack->SetStaticMesh(nullptr);
		break;
	case EItemEquipSlot::WeaponLeftHand:
		WeaponRightHand->SetStaticMesh(nullptr);
		break;
	case EItemEquipSlot::WeaponRightHand:
		WeaponLeftHand->SetStaticMesh(nullptr);
		break;
	}
}

FVector AH1Character::GetFootLocation()
{
	// 아이템을 드랍할 발아래 오브젝트가 유효하다면 발아래 오브젝트에서 드랍할 위치를 가져온다.
	if (IsValid(FootLocate))
	{
		return FootLocate->GetComponentTransform().GetLocation();
	}

	// 발아래 오브젝트가 없다면 현재 캐릭터의 위치를 반환해서 그 위치에 아이템을 드랍한다.
	return GetActorLocation();
}

void AH1Character::UpDown(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	// 컨트롤러로부터 회전값을 가져오고 회전값의 X좌표방향 값을 얻어온다. 그 방향으로 입력받은 값 만큼 이동한다.
	case EPlayerControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	// X축방향에 값추가 이 값으로 Tick에서 이동처리함.
	case EPlayerControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue;
		break;
	}
}

void AH1Character::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		// 컨트롤러로부터 회전값을 가져오고 회전값의 Y좌표방향 값을 얻어온다. 그 방향으로 입력받은 값 만큼 이동한다.
	case EPlayerControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
		// Y축방향에 값추가 이 값으로 Tick에서 이동처리함.
	case EPlayerControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue;
		break;
	}
}

// 카메라 회전과 캐릭터 회전은 분리 되어 있음
void AH1Character::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	// GTA 모드인 경우에만 카메라 시점이 움직인다. 
	case EPlayerControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	}
}

void AH1Character::Attack()
{
	// 애니메이션BP nullptr 체크
	if (H1AnimInst == nullptr) return;

	if (IsAttacking)
	{
		H1CHECK(FMath::IsWithinInclusive<int32>(CurrentComboIndex, 1, FinalComboIndex));
		if (bCanNextCombo) // 콤보어택이 이미 진행 중이면 다음 어택을 허용해주는 로직을 실행합니다.
		{
			bIsComboInputOn = true;  
		}
	}
	else
	{
		H1CHECK(CurrentComboIndex == 0);		// 새로운 콤보를 시작하는데 이미 콤보 인덱스가 늘어나 있으면 버그 입니다.
		AttackStartComboState();				// 콤보 상태를 콤보 시작으로 설정 합니다.
		H1AnimInst->playAttackMontage();		// 몽타주를 재생합니다.
		H1AnimInst->JumpToOtherAttackMontageSection(CurrentComboIndex); // 지금 인덱스는 0 이므로 섹션 1번으로 점프합니다. (0번은 없음..)
		IsAttacking = true;						// 이제부터 콤보 공격이 진행 중 입니다.
	}
}

void AH1Character::AttackStartComboState()
{
	// UTF8
	bCanNextCombo = true; // 콤보 입력 받을 수 있고
	bIsComboInputOn = false; // 콤보 입력 

	// FMath::IsWithinInclusive()
	// 판별 대상 값이 특정 범위안에 속하는지 판정해서 bool 반환
	// @param1 판별대상
	// @param2 최소값
	// @param3 최댓값
	H1CHECK(FMath::IsWithinInclusive<int32>(CurrentComboIndex, 0, FinalComboIndex - 1));
	// 콤보 인덱스를 +1하고 1~4로 값을 제한한다. // 5부터는 애니메이션이 없는 무효한 값이기 때문에.
	CurrentComboIndex = FMath::Clamp<int32>(CurrentComboIndex + 1, 1, FinalComboIndex);
}

void AH1Character::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 공격을 시작하지 않고도 이 함수가 호출되면 분명히 버그가 있는 것.
	H1CHECK(IsAttacking);
	H1CHECK(CurrentComboIndex > 0);
	// Attack Montage가 끝난 것을 Character의 property set에 apply
	IsAttacking = false;
	AttackEndComboState();
}

void AH1Character::AttackEndComboState()
{
	bIsComboInputOn = false;
	bCanNextCombo = false;
	CurrentComboIndex = 0; 
}

void AH1Character::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this); // 자신은 충돌검사 대상에서 제외한다, 단순 충돌체만 검사한다.
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 200.f,
		FQuat::Identity, // 회전값 없음.
		ECollisionChannel::ECC_EngineTraceChannel2,
		FCollisionShape::MakeSphere(50.f),
		Params);

	if(bResult)
	{
		if(HitResult.Actor.IsValid())
		{
			H1LOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());
		}
	}
}



