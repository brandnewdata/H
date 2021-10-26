// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/H1AnimInstance.h"

UH1AnimInstance::UH1AnimInstance()
{
	CurrentPawnSpeed = 0.f;
	bInAir = false;

	// ��Ÿ�� �ּ� �ε�.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/H1/PlayerAnims/H1_PC_AttackMontage.H1_PC_AttackMontage"));
	if(ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UH1AnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// TryGEtPawnOwner ���� �ִϸ��̼� BP�� ������� Pawn�� �����͸� ���´�.
	auto Pawn = TryGetPawnOwner();
	if(::IsValid(Pawn)) // Pawn ��ȿ�ϴٸ�
	{
		// Pawn���κ��� �ӵ� -> �ӷ��� ���´�.
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<ACharacter>(Pawn);
		if(Character)
		{
			// ĳ���ͷ� ���� �������� ���θ� �޾ƿ´�.
			bInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UH1AnimInstance::playAttackMontage()
{
	// �� �Լ� Character�� IsAttacking == false�� ���� ȣ�� �˴ϴ�.	
	Montage_Play(AttackMontage, 1.0f);
}

void UH1AnimInstance::JumpToOtherAttackMontageSection(int32 NewSection)
{
	H1CHECK(Montage_IsPlaying(AttackMontage)); // ��Ÿ�� �÷����߿��� ��Ÿ�� ���� ������ ������.
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
}

void UH1AnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UH1AnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UH1AnimInstance::GetAttackMontageSectionName(int32 Section)
{
	H1CHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
