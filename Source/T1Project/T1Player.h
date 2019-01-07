// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "T1Project.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "DataActorComponent.h"

#include "T1Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UT1AnimInstance;


UCLASS()
class T1PROJECT_API AT1Player : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AT1Player();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ī�޶��带 ����
	enum class ECameraControlMode
	{
		FOLLOW,
		DIABLO,
	};
	
	ECameraControlMode CurrentCameraControlMode = ECameraControlMode::FOLLOW;
	FVector DirectionToMove = FVector::ZeroVector;

	// ī�޶� ������
	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// �ݸ� ������ �ľ� �ϱ� ���� �������̵�	
	virtual void PostInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;

	// �𸮾� ���� ������ ó����
	virtual float TakeDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = CharInfo)
	class UDataActorComponent* CharInfoData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	
private:
	// ������Ʈ �ɼǿ� ��ǲ�� ��Ī�Ǵ� �Լ�
	void UpDown(float newAxisValue);
	void LeftRight(float newAxisValue);
	void Turn(float newAxisValue);
	void LookUp(float newAxisValue);

	void CameraViewChange();
	void SetCameraControlMode(ECameraControlMode inControlMode);

	void Attack();
	void AttackStartComboState();
	void AttackEndComboState();

	// ������ �浹������ Ȯ���ϱ������Լ�
	void AttackCheck();

	/*
		�𸮾󿡼� ��������Ʈ�� C++��ü������ ����ϴ� ��������Ʈ�� C++, �������Ʈ ��ü�� ��� ����Ҽ� �ִ� ��������Ʈ�� ������.
		�������Ʈ ������Ʈ�� �ɹ� �Լ��� ���� ������ �����ϰ� �ε��ϴ� ����ȭ(Serialization) ��Ŀ������ �� �ֱ⋚����
		�Ϲ� C++ �� �����ϴ� ������� �ɹ��Լ��� ���� �Ҽ� ����. �׷��� �������Ʈ�� ���ä��� C++�Լ��� ��� UFUNCTION ��ũ�θ�
		����ؾ��Ѵ�. �̷��� �������Ʈ ��ü�͵� �����Ǵ� ��������Ʈ�� �𸮾���������� ���̳��� ��������Ʈ(Dynamic Delegate)��� �Ѵ�.
	*/
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentComboIndex;

	UPROPERTY(VisibleInstanceOnly, BluePrintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int MaxComboCount;

	UPROPERTY()
	class UT1AnimInstance* T1Anim;
};
