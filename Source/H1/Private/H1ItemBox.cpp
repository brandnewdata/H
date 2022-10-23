// Fill out your copyright notice in the Description page of Project Settings.
#include "H1ItemBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AH1ItemBox::AH1ItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOXMESH"));

	RootComponent = Trigger;
	BoxMesh->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(40.f, 42.f, 30.f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/H1/Item/DungeonItem/SM_Env_DungeonItemBox.SM_Env_DungeonItemBox"));

	if (SM_BOX.Succeeded())
	{
		BoxMesh->SetStaticMesh(SM_BOX.Object);
	}

	BoxMesh->SetRelativeLocation(FVector(0.f, -3.5f, -30.f));
}

// Called when the game starts or when spawned
void AH1ItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AH1ItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

