// Fill out your copyright notice in the Description page of Project Settings.


#include "TestNResearch/FounTain.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
AFounTain::AFounTain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;	// 더 이상 회전에 Actor Tick 사용하지 않아서 비활성화

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
	Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
	Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));

	RootComponent = Body;
	Water->SetupAttachment(Body);

	Water->SetRelativeLocation(FVector(0.f, 0.f, 135.f));
	Light->SetRelativeLocation(FVector(0.f, 0.f, 195.f));
	Splash->SetRelativeLocation(FVector(0.f, 0.f, 195.f));

	RotatingMovementComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));

	RotateSpeed = 30.f;

	RotatingMovementComp->RotationRate = FRotator(0.f, RotateSpeed, 0.f);

}

// Called when the game starts or when spawned
void AFounTain::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(H1_Custom, Warning, TEXT("Actor Name : %s, Location X : %3f"), *GetName(), GetActorLocation().X);

	H1LOG_S(Warning);
	H1LOG(Warning, TEXT("Actor Name : %s, Location X : %.3f"), *GetName(), GetActorLocation().X);
}

void AFounTain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	H1LOG_S(Warning);
}

void AFounTain::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	H1LOG_S(Warning);
}

// Called every frame
void AFounTain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 회전처리를 actor tick에서 하는경우 (대부분 movement Component를 사용함.)
	// AddActorLocalRotation(FRotator(0.f, RotateSpeed * DeltaTime, 0.f));
}

