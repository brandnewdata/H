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
	// @ 수행할 애니메이션 세트를 결정하기 위해서 애니메이션 관련 속성들을 업데이트 합니다.
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

// Getter, Setter
public:
	FORCEINLINE void SetDeadAnim() { IsDead = true; } // 애니메이션 상태를 죽음으로 만든다 // 사망 애니메이션이 재상된다.

	// Delegates
public:
	// Combo Attack Delegate
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

// State를 바꾸는데 필요한 character property 복사본들
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	// 중첩가능한 동작상태들.
 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool bInAir;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};
