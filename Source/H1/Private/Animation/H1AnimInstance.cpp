// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/H1AnimInstance.h"

UH1AnimInstance::UH1AnimInstance()
{
	CurrentPawnSpeed = 0.f;
	bInAir = false;

	// 몽타주 애셋 로드.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/H1/PlayerAnims/H1_PC_AttackMontage.H1_PC_AttackMontage"));
	if(ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UH1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TryGEtPawnOwner 현재 애니메이션 BP를 사용중인 Pawn의 포인터를 얻어온다.
	auto Pawn = TryGetPawnOwner();
	if(::IsValid(Pawn)) // Pawn 유효하다면
	{
		// Pawn으로부터 속도 -> 속력을 얻어온다.
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if(Character)
		{
			// 캐릭터로 부터 공중인지 여부를 받아온다.
			bInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UH1AnimInstance::playAttackMontage()
{
	// 이 함수 Character의 IsAttacking == false일 때만 호출 됩니다.	
	Montage_Play(AttackMontage, 1.0f);
}

void UH1AnimInstance::AnimNotify_AttackHitCheck()
{
	H1LOG_S(Warning);
}
