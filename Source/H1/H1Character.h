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

	// �ʱ�ȭ �Լ���
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

	// ��� �Լ���
public:
	// ü���� ä��ų� ��´�.
	UFUNCTION()
	float Heal(float AmountOfHeal);

	// ���⸦ �����ϴ�.
	UFUNCTION()
	void EquipWeapon(UH1InventoryItem* InventoryItem);

	// ���� ������ �����Ѵ�.
	UFUNCTION()
	void ClearWeapon(UH1InventoryItem* InventoryItem);

	UFUNCTION()
	FVector GetFootLocation();

protected:

	// ����Ű �̵��� ���� �Լ� �߰�
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
	
	// �⺻ ��Ʈ�� ���� GTA
	EPlayerControlMode CurrentControlMode = EPlayerControlMode::GTA;

	// �÷��̾ �Է��� ���Ⱚ�� �����ؼ� �̵��ϴ� �������� ����Ѵ� (��ƺ�� ���)
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

	// ī�޶� ������ȯ�� �ʿ��� �Ӽ���
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

