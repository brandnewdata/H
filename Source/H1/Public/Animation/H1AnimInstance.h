// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Source\H1\H1.h"
#include "Animation/AnimInstance.h"
#include "H1AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class H1_API UH1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UH1AnimInstance();
	// @ ������ �ִϸ��̼� ��Ʈ�� �����ϱ� ���ؼ� �ִϸ��̼� ���� �Ӽ����� ������Ʈ �մϴ�.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void playAttackMontage();



	// State�� �ٲٴµ� �ʿ��� character property ���纻
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool bInAir;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage;
};
