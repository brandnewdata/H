// Fill out your copyright notice in the Description page of Project Settings.


#include "H1Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AH1Pawn::AH1Pawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �ν�Ʈ ��ũ ó�� ���� ���� 
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ������Ʈ ����
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	// ��Ʈ�� ��Ӱ��� ����
	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	// ĸ�� �Ӽ� ����
	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(34.f);

	// �θ��� ��ǥ�踦 �������� ��ġ�� ȸ���� ���� �մϴ�.
	Mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	SpringArm->TargetArmLength = 750.f;
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PLAYERUNIT(TEXT("/Game/H1/PC_CharM_Robo/SK_CharM_Robo.SK_CharM_Robo"));
	
	if (SK_PLAYERUNIT.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_PLAYERUNIT.Object);
	}

	// http://egloos.zum.com/sweeper/v/3208657
	// CDO ���鶧 ����ϴ� �����  static ConstructorHelpers::FClassFinder
	// AnimBlueprint'/Game/H1/Animations/AMBP_PC.AMBP_PC'
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// BP Class �����ö��� ���� _C ���̸��� ������.. 
	static ConstructorHelpers::FClassFinder<UAnimInstance> ROBO_ANIM(TEXT("/Game/H1/Animations/AMBP_PC.AMBP_PC_C"));

	// Ŭ������ ã�Ҵٸ�
	if (ROBO_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(ROBO_ANIM.Class);
	}
}

// Called when the game starts or when spawned
void AH1Pawn::BeginPlay()
{
	Super::BeginPlay();	
	//// �ִϸ��̼��� �ִϸ��̼� BP�� ���� ��Ÿ�ַ� ���� �������� ���� �׷��� ���
	//Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//// ���ҽ��� �ε�, TODO : outer�� nullptr���ϸ� world�� outer�ΰ�?? 
	//UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, 
	//	TEXT("/Game/H1/Animations/PC_Anims/WarriorRun.WarriorRun"));

	//// �ε带 �����ߴٸ� �ִϸ��̼� �÷��� ����
	//if (AnimAsset != nullptr)
	//{
	//	Mesh->PlayAnimation(AnimAsset, true);
	//}
}

// Called every frame
void AH1Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AH1Pawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	H1LOG_S(Warning);
}

void AH1Pawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	H1LOG_S(Warning);
}

// Called to bind functionality to input
void AH1Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AH1Pawn::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AH1Pawn::LeftRight);
}

void AH1Pawn::UpDown(float NewAxisValue)
{

	// TODO : 0üũ�� �α״� Ȯ�ο��̹Ƿ� ���߿� ��������.
	// if������ 0���� üũ�ϴ� �ͺ��� �׳� 0��ŭ �̵��ϴ� ���� �� ������ ����. 
	if (FMath::IsNearlyZero(NewAxisValue) == false)
	{
		AddMovementInput(GetActorForwardVector(), NewAxisValue);
		H1LOG(Warning, TEXT("%f"), NewAxisValue);
	}
}

void AH1Pawn::LeftRight(float NewAxisValue)
{
	// TODO : 0üũ�� �α״� Ȯ�ο��̹Ƿ� ���߿� ��������.
	// if������ 0���� üũ�ϴ� �ͺ��� �׳� 0��ŭ �̵��ϴ� ���� �� ������ ����. 
	if (FMath::IsNearlyZero(NewAxisValue) == false)
	{
		AddMovementInput(GetActorRightVector(), NewAxisValue);
		H1LOG(Warning, TEXT("%f"), NewAxisValue);
	}
}

