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

	// 상속 받은 함수들
public:
	virtual void PostInitializeComponents() override;

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

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// H1Character GamePlay Methods
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

	// Bound to Anim Instance to 'process End Of Attack Anim Montage.'
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	// 방향키 이동을 위한 함수 추가
private:
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	void Turn(float NewAxisValue);
	void LookUp(float NewAxisValue);

	// Combo Attack Methods
	void AttackStartComboState();
	void AttackEndComboState();
	void AttackCheck(); // @ 공격범위에 타격 대상이 있는지 체크하고 데미지를 적용한다.

	// Unreal Damage Framework
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	enum class EPlayerControlMode
	{
		GTA,
		DIABLO
	};

	void ViewChange(); 
	void SetControlMode(EPlayerControlMode NewControlMode);
	void Attack(); // 인풋에 바인드 되어 있는 함수

	// 기본 컨트롤 모드는 GTA
	EPlayerControlMode CurrentControlMode = EPlayerControlMode::GTA;

	// 플레이어가 입력한 방향값을 저장해서 이동하는 로직에서 사용한다 (디아블로 방식)
	FVector DirectionToMove = FVector::ZeroVector;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Character Components
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
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
	
	// Equipmentable Weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* m_Weapon;

	// Static Mesh 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponBack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponRightHand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponLeftHand;
	
	// Skeletal Mesh Weapon
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponBack_SK;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponRightHand_SK;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipmentable, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* WeaponLeftHand_SK;

	UPROPERTY()
	class UH1AnimInstance* H1AnimInst;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Character Utils
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// 카메라 시점변환에 필요한 속성들
	float ArmLengthTo = 0.f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.f;
	float ArmRotationSpeed = 0.f;
;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Character Status
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Todo : private로 만드세요. // Game Play Property // Character Status
public:
	UPROPERTY()
	class UArrowComponent* FootLocate;
	UPROPERTY()
	float HP = 500.0f;
	UPROPERTY()
	float MaxHP = 1000.0f;
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Character States
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool bCanNextCombo;
	// 다음 콤보를 위한 플레이어의 입력이 있었는가? // 없다면 콤보가 끊긴다. // 이어지는 콤보어택은 OnNextAttackCheck에서 발동시킵니다.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool bIsComboInputOn;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentComboIndex;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 FinalComboIndex;
};

