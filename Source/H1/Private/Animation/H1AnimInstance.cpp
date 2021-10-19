// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/H1AnimInstance.h"

UH1AnimInstance::UH1AnimInstance()
{
	CurrentPawnSpeed = 0.f;
	bInAir = false;
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
