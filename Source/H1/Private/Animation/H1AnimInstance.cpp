// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/H1AnimInstance.h"

UH1AnimInstance::UH1AnimInstance()
{
	CurrentPawnSpeed = 0.f;
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
	}
}
