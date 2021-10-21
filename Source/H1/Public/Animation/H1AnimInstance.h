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
	// @ 수행할 애니메이션 세트를 결정하기 위해서 애니메이션 관련 속성들을 업데이트 합니다.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void playAttackMontage();



	// State를 바꾸는데 필요한 character property 복사본
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool bInAir;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		UAnimMontage* AttackMontage;
};
