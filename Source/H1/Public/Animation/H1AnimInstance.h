// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Source\H1\H1.h"
#include "Animation/AnimInstance.h"
#include "H1AnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
/**
 * 
 */
UCLASS()
class H1_API UH1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	// 
public:
	UH1AnimInstance();
	// @ ������ �ִϸ��̼� ��Ʈ�� �����ϱ� ���ؼ� �ִϸ��̼� ���� �Ӽ����� ������Ʈ �մϴ�.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void playAttackMontage();
	void JumpToOtherAttackMontageSection(int32 NewSection);


private:
	// Anim Notify Check Methods
	UFUNCTION()
	void AnimNotify_AttackHitCheck();
	UFUNCTION()
	void AnimNotify_NextAttackCheck();
	FName GetAttackMontageSectionName(int32 Section);

	// Delegates
public:
	// Combo Attack Delegate
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

	// State�� �ٲٴµ� �ʿ��� character property ���纻
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool bInAir;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;
};
