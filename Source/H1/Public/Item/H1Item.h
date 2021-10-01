// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "H1Item.generated.h"

UCLASS()
class H1_API AH1Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AH1Item();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 에디터 디테일 패널에서 값을 수정하면 이 함수가 호출된다.
	// 에디터에서 입력한 값으로 데이터 테이블에서 값을 가져온다.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	bool SetItemInfo(int DataTableID, int NewStackCount);

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnItemWidgetButtonClicked();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UArrowComponent* Arrow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USphereComponent* ItemSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int ItemClassID;
	UPROPERTY()
	class UItemName_WS* ItemNameWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int StackCount = 1;
};
