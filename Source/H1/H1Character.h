// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "H1.h"
#include "GameFramework/Character.h"
#include "H1Character.generated.h"

class UH1InventoryItem;

UCLASS(Blueprintable)
class AH1Character : public ACharacter
{
	GENERATED_BODY()

	// 초기화 함수들
public:
	AH1Character();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* playerInputComponent) override;

	// Getter, Setter
public:
	// 
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }
	UFUNCTION()
	float GetCurrentHPRate() { return HP / MaxHP; }

	// 기능 함수들
public:
	// 체력을 채우거나 깎는다.
	UFUNCTION()
	float Heal(float AmountOfHeal);

	// 무기를 장착하다.
	UFUNCTION()
	void EquipWeapon(UH1InventoryItem* InventoryItem);

	// 무기 장착을 해제한다.
	UFUNCTION()
	void ClearWeapon(UH1InventoryItem* InventoryItem);

	UFUNCTION()
	FVector GetFootLocation();

protected:

	// 방향키 이동을 위한 함수 추가
private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);


protected:
	enum class EPlayerControlMode
	{
		GTA,
		DIABLO
	};

	
	void ViewChange(); 
	void SetControlMode(EPlayerControlMode NewControlMode);
	
	// 기본 컨트롤 모드는 GTA
	EPlayerControlMode CurrentControlMode = EPlayerControlMode::GTA;

	// 플레이어가 입력한 방향값을 저장해서 이동하는 로직에서 사용한다 (디아블로 방식)
	FVector DirectionToMove = FVector::ZeroVector;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;
	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
	/**  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* m_Weapon;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* WeaponBack;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* WeaponRightHand;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* WeaponLeftHand;

	// 카메라 시점변환에 필요한 속성들
	float ArmLengthTo = 0.f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.f;
	float ArmRotationSpeed = 0.f;

	UPROPERTY()
	float HP = 500.0f;

	UPROPERTY()
	float MaxHP = 1000.0f;

public:
	UPROPERTY()
	class UArrowComponent* FootLocate;
};

