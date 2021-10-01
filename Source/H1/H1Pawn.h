// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "..\Source\H1\H1.h"
#include "GameFramework/Pawn.h"
#include "H1Pawn.generated.h"

UCLASS()
class H1_API AH1Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AH1Pawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, Category=Collision)
	class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere, Category=Visual)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category =Movement)
	class UFloatingPawnMovement* Movement; 

	UPROPERTY(VisibleAnywhere, Category =Camera)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category =Camera)
	class UCameraComponent* Camera;

private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);


	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* m_Weapon;

	// Getter,  Setter
public:
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
};
