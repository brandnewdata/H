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

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta=(AllowPrivateAccess=true))
	float CurrentPawnSpeed;
};
