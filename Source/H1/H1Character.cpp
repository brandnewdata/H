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
	// �������� �ٿ�帮 ���Ǿ�� �浹�ϸ� �� �±׸� �������� ����� �׷��� �ʿ���
	// ������ ���� �ʿ� ���� �� ���⵵�ѵ�..? �Ƹ� �� AI�鵵 pawn�̴ϱ� �÷��̾� �����ϴ� �뵵 �� ��.
	// �÷��̾� ���� �浹 ä���� ����غ���.
	GetCapsuleComponent()->ComponentTags.Add(FName("ItemPickingBoundary"));

	// Skeletal Mesh �����ϱ�
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90, 0.f));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_ROBO(TEXT("/Game/H1/PC_CharM_Robo/SK_CharM_Robo.SK_CharM_Robo"));
	if (SK_ROBO.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_ROBO.Object);
	}

	// Skeletal Mesh�� �ִϸ��̼� �������Ʈ �����ϱ�.
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

	// ��Ʈ�� ��� ���� >> �ʱⰪ: GTA -> CDO : ��ƺ��
	SetControlMode(EPlayerControlMode::GTA);

	// ���� ���̸� ������.
	GetCharacterMovement()->JumpZVelocity = 500.0f;

	// ī�޶� ������ȯ ��ļ� �ڵ�
	ArmLengthSpeed = 3.f;		// ƽ�� location�� 3�� �̵���
	ArmRotationSpeed = 10.f;	// ƽ�� ������ 10���� ���ư�
}

void AH1Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// TODO : �� �������� ��� ã�Ƽ� �����ϱ� ... ��ƽ���� ������Ʈ vs �� ƽ���� float�� Ȯ��(float�� equal ���Լ� ���...)
	// ��ƽ���� ��ǥ ��ġ���� ī�޶� ��ġ�� �̵��Ѵ�.(ī�޶� ����Ⱑ �����. ���� ��ġ�� �����ذ� ��� ���� ������ ����ԵǴ� ����)
	// ������ 0,1�ε� �׳� if���°� ��������. �ٸ� ������ �߰��� �����ΰ�?
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, ArmLengthTo, DeltaSeconds, ArmLengthSpeed);

	switch (CurrentControlMode)
	{
	case EPlayerControlMode::DIABLO:
		CameraBoom->SetRelativeRotation(FMath::RInterpTo(CameraBoom->GetRelativeRotation(), ArmRotationTo, DeltaSeconds, ArmRotationSpeed));
		break;
	}


	// ���� ��忡 ���� �ٸ�ó��
	switch (CurrentControlMode)
	{
	case EPlayerControlMode::DIABLO:
		// ������ ������ �̰Ŵ� �����ε�??
		if (DirectionToMove.SizeSquared() > 0.f)
		{
			// ��Ʈ�ѷ��� ȸ���� �����ص� ȸ�������� �����Ѵ�.
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());

			// TODO : �׷��� �̰� �밢�� ������ �ӵ��� �ö󰡴°� �ƴѰ�?
			// Ű �̺�Ʈ�� �Է¹��� ����Ű ����ŭ ĳ���͸� �̵��Ѵ�.
			AddMovementInput(DirectionToMove);
		}
		break;
	}

	// Ŀ�� ��ġ ������Ʈ�� �� ƽ���� ó����.
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
	// ���� ��Ʈ�� ��� ����
	CurrentControlMode = NewControlMode;

	// ��Ʈ�� ��忡 ���� �ٸ��� ������
	switch (CurrentControlMode)
	{
	case EPlayerControlMode::GTA:
		// ���� ī�޶� ��ġ�� �� ƽ���� ������Ʈ �˴ϴ�. 
		// CameraBoom->TargetArmLength = 500.f;
		// CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
		ArmLengthTo = 450.f;
		// ī�޶� ������ ���� ���������� ���ǹ�

		// ī�޶� ��Ʈ�η��� ���ؼ� ȸ���Ѵ�.
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bInheritPitch = true;
		CameraBoom->bInheritRoll = true;
		CameraBoom->bInheritYaw = true;

		// ī�޶� �������̸� �ȵǴϱ� �浹ó����
		CameraBoom->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;

		// ĳ���Ͱ� ȸ���ϴ� �ӵ��� �����
		GetCharacterMovement()->bOrientRotationToMovement = true;
		// TODO : �̰� ����? ĳ���� ȸ�� õõ�� ������ ���� �������� ���� �ִ� ��. 
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		// ĳ���Ͱ� �ʴ� 720�� ȸ�� ������
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
		// ī�޶� �����̶� �浹ó�� ����
		CameraBoom->bDoCollisionTest = false;
		// ��Ʈ�ѷ��� ȸ������ ī�޶� �ƴ϶� ĳ���Ͱ� ������.(���콺 �̵��� ��ġ��)
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

	// GTA�� �÷��̾� ĳ���� ������ ��쿡�� ���콺 XY�� ���� ��ȯ 
	playerInputComponent->BindAxis(TEXT("Turn"), this, &AH1Character::Turn);
	playerInputComponent->BindAxis(TEXT("LookUp"), this, &AH1Character::LookUp);

	// ����Ű ó�� �Լ� ���ε�, ��ӹ��� �Լ��� �״�� �����.
	playerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	// AttackŰ�� ó�� �Լ� ���ε�
	playerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AH1Character::Attack);
}

void AH1Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	auto AnimInst = Cast<UH1AnimInstance>(GetMesh()->GetAnimInstance());
	AnimInst->OnMontageEnded.AddDynamic(this, &AH1Character::OnAttackMontageEnded); // �̷��� �ϸ� ��� ��Ÿ�ְ� ���������� ȣ���
	// ��Ÿ�ְ� �������� �Ǹ� ó�� �Լ����� �б�ó���� �ʿ��ϴ�.
}

void AH1Character::Attack()
{
	if(IsAttacking) return; // �������̸� �� �Լ��� �ٷ� ��ȯ�Ѵ�.

	// �ִϸ��̼� �������Ʈ �����͸� ����
	auto AnimInst = Cast<UH1AnimInstance>(GetMesh()->GetAnimInstance());
	if (nullptr == AnimInst) return;

	// �ִϸ��̼�BP �����ͷ� ��Ÿ�� ���.
	AnimInst->playAttackMontage();

	IsAttacking = true; // ���� �ִϸ��̼��� ���� �ȴ�.
}


float AH1Character::Heal(float AmountOfHeal)
{
	HP += AmountOfHeal;
	// ü���� �ִ� ü�� ���ϰ� �Ǿ���.
	HP = FMath::Clamp(HP, 0.0f, MaxHP);

	// ü�� UI ǥ�ø� ���ؼ� ���ۼ�Ʈ�� ���ִ��� ��ȯ��.
	return HP / MaxHP;
}

void AH1Character::EquipWeapon(UH1InventoryItem* InventoryItem)
{
	if (!IsValid(InventoryItem))
		return;

	// �κ��丮 �������� �����ϴµ� ����� DT ���� �����͸� �޾ƿ´�.
	const FItemTableRow* ItemTableRow = InventoryItem->GetItemTableRow();
	// nullptr�̸� ��ȿ
	if (nullptr == ItemTableRow)
		return;

	// ������ ���̺� ������ ���� ��� �޽��� �����Ѵ�.
	UStaticMesh* ItemMesh = Cast<UStaticMesh>(ItemTableRow->ItemModel.TryLoad());
	if (!IsValid(ItemMesh))
		return;

	// ��� �������� ���� ��ġ�� ���� ĳ���Ϳ� �����Ǿ��ִ� �޽� ������Ʈ�� �޽��� �����Ѵ�.
	// �޽� ������Ʈ�� �̹� ������ġ�� �����Ǿ� �ִ�.
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

	// �ش��ϴ� �޽� ������Ʈ�� �޽��� �����Ѵ�.
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
	// �������� ����� �߾Ʒ� ������Ʈ�� ��ȿ�ϴٸ� �߾Ʒ� ������Ʈ���� ����� ��ġ�� �����´�.
	if (IsValid(FootLocate))
	{
		return FootLocate->GetComponentTransform().GetLocation();
	}

	// �߾Ʒ� ������Ʈ�� ���ٸ� ���� ĳ������ ��ġ�� ��ȯ�ؼ� �� ��ġ�� �������� ����Ѵ�.
	return GetActorLocation();
}

void AH1Character::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// IsAttacking�� false�� ���·� �� �Լ��� ȣ��Ǹ� �и��� ���װ� �ִ� ��.
	H1CHECK(IsAttacking);
	// Attack Montage�� ���� ���� Character�� property set�� apply
	IsAttacking = false;
}

void AH1Character::UpDown(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	// ��Ʈ�ѷ��κ��� ȸ������ �������� ȸ������ X��ǥ���� ���� ���´�. �� �������� �Է¹��� �� ��ŭ �̵��Ѵ�.
	case EPlayerControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X), NewAxisValue);
		break;
	// X����⿡ ���߰� �� ������ Tick���� �̵�ó����.
	case EPlayerControlMode::DIABLO:
		DirectionToMove.X = NewAxisValue;
		break;
	}
}

void AH1Character::LeftRight(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		// ��Ʈ�ѷ��κ��� ȸ������ �������� ȸ������ Y��ǥ���� ���� ���´�. �� �������� �Է¹��� �� ��ŭ �̵��Ѵ�.
	case EPlayerControlMode::GTA:
		AddMovementInput(FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y), NewAxisValue);
		break;
		// Y����⿡ ���߰� �� ������ Tick���� �̵�ó����.
	case EPlayerControlMode::DIABLO:
		DirectionToMove.Y = NewAxisValue;
		break;
	}
}

// ī�޶� ȸ���� ĳ���� ȸ���� �и� �Ǿ� ����
void AH1Character::LookUp(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
	// GTA ����� ��쿡�� ī�޶� ������ �����δ�. 
	case EPlayerControlMode::GTA:
		AddControllerPitchInput(NewAxisValue);
		break;
	}
}

void AH1Character::Turn(float NewAxisValue)
{
	switch (CurrentControlMode)
	{
		// GTA ����� ��쿡�� ī�޶� ������ �����δ�. 
	case EPlayerControlMode::GTA:
		AddControllerYawInput(NewAxisValue);
		break;
	}
}

