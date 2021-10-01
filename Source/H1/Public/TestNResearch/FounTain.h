// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Source\H1\H1.h"
#include "GameFramework/Actor.h"
#include "FounTain.generated.h"

UCLASS()
class AFounTain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFounTain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category=Stat, Meta = (AllowPrivateAccess = true))
	float RotateSpeed;

	// 전방선언ㄱㄱ
	UPROPERTY(VisibleAnywhere)
	class URotatingMovementComponent* RotatingMovementComp;


	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Body;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Water;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* Light;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* Splash;


};
