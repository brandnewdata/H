// Fill out your copyright notice in the Description page of Project Settings.


#include "H1Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
AH1Pawn::AH1Pawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 로스트 아크 처럼 시점 고정 
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 컴포넌트 생성
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MOVEMENT"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	// 루트와 상속관계 설정
	RootComponent = Capsule;
	Mesh->SetupAttachment(Capsule);
	SpringArm->SetupAttachment(Capsule);
	Camera->SetupAttachment(SpringArm);

	// 캡슐 속성 설정
	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(34.f);

	// 부모의 좌표계를 기준으로 위치와 회전을 설정 합니다.
	Mesh->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.f, 0.f));

	SpringArm->TargetArmLength = 750.f;
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_PLAYERUNIT(TEXT("/Game/H1/PC_CharM_Robo/SK_CharM_Robo.SK_CharM_Robo"));
	
	if (SK_PLAYERUNIT.Succeeded())
	{
		Mesh->SetSkeletalMesh(SK_PLAYERUNIT.Object);
	}

	// http://egloos.zum.com/sweeper/v/3208657
	// CDO 만들때 사용하는 방법은  static ConstructorHelpers::FClassFinder
	// AnimBlueprint'/Game/H1/Animations/AMBP_PC.AMBP_PC'
	Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// BP Class 가져올때는 끝에 _C 잊이말고 붙이자.. 
	static ConstructorHelpers::FClassFinder<UAnimInstance> ROBO_ANIM(TEXT("/Game/H1/Animations/AMBP_PC.AMBP_PC_C"));

	// 클래스를 찾았다면
	if (ROBO_ANIM.Succeeded())
	{
		Mesh->SetAnimInstanceClass(ROBO_ANIM.Class);
	}
}

// Called when the game starts or when spawned
void AH1Pawn::BeginPlay()
{
	Super::BeginPlay();	
	//// 애니메이션을 애니메이션 BP로 할지 몽타주로 할지 시퀀스로 할지 그런거 모드
	//Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//// 리소스를 로드, TODO : outer를 nullptr로하면 world가 outer인가?? 
	//UAnimationAsset* AnimAsset = LoadObject<UAnimationAsset>(nullptr, 
	//	TEXT("/Game/H1/Animations/PC_Anims/WarriorRun.WarriorRun"));

	//// 로드를 성공했다면 애니메이션 플레이 시작
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

	// TODO : 0체크랑 로그는 확인용이므로 나중에 제거하자.
	// if문으로 0인지 체크하는 것보다 그냥 0마큼 이동하는 쪽이 더 빠를것 같음. 
	if (FMath::IsNearlyZero(NewAxisValue) == false)
	{
		AddMovementInput(GetActorForwardVector(), NewAxisValue);
		H1LOG(Warning, TEXT("%f"), NewAxisValue);
	}
}

void AH1Pawn::LeftRight(float NewAxisValue)
{
	// TODO : 0체크랑 로그는 확인용이므로 나중에 제거하자.
	// if문으로 0인지 체크하는 것보다 그냥 0마큼 이동하는 쪽이 더 빠를것 같음. 
	if (FMath::IsNearlyZero(NewAxisValue) == false)
	{
		AddMovementInput(GetActorRightVector(), NewAxisValue);
		H1LOG(Warning, TEXT("%f"), NewAxisValue);
	}
}

