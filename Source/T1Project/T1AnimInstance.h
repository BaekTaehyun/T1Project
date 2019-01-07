// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "T1AnimInstance.generated.h"

/*
  ��Ÿ�ֿ� ����� NextAttackCheck �ִϸ��̼� ��Ƽ���̰� �߻��Ҷ����� T1Character�� ������ ��������Ʈ ����
*/
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);


/**
 * Ŀ���� ����¡�� �ִϸ��̼� �ν��Ͻ� ó����
   UAnimInstance Ŭ������ �ִϸ��̼� �������Ʈ�� ��Ī�Ǵ� �𸮾� Ŭ�����Դϴ�.
 */
UCLASS()
class T1PROJECT_API UT1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UT1AnimInstance();
	virtual void NativeUpdateAnimation(float deltaSecond) override;

	void PlayAttackMontage();
	void JumpToAttackMontageSection(int32 newSection);
	void SetDeadAnim() { IsDead = true; }

public:
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnAttackHitCheckDelegate OnAttackHitCheck;

private:
	/* 
		��Ÿ�ֿ� ���ǵ� Ŀ���� ��Ƽ������ ��쿡�� �ִ��ν��Ͻ� Ŭ������ 'AnimNotify_��Ƽ���̸�'�̶�� �̸��� ��� �Լ��� ã�Ƽ� ȣ��
		���� �ҷ����� �Լ��� �𸮾� ��Ÿ���� ã���� �ֵ��� �ݵ�� UFUNCTION ��ũ�ΰ� �����Ǿ�� �Ѵ�.
	*/
	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	FName GetAttackMontageSectionName(int section);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsAir;
	/*
		UPROPERTY�� �����ϴ� Ű������ EditAnyWhere�� VisibleAnywhere�� �ִ� Anywhere Ű����� �����Ϳ���
		���� ����� ���� DefaultOnly�� InstacneOnly�� ����ȭ�� �� �ִ� DefaultOnly�� Ŭ������ �⺻����
		��簪�� ����ϴ� �������Ʈ ���� ȭ�鿡���� �������� InstanceOnly�� �ν��Ͻ��� �Ӽ��� �����ִ� ������ ����Ʈ������
		��������. �̹������� �ִϸ��̼� �������Ʈ Ŭ���� ���赵������ ���������� VisibleDefaultsOnly�� ����
	*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};

/* p.239 �� ����(��Ÿ�� ���) 
  �𸮾� ������ �� ĳ���Ͱ� ���� ������ ���̷����� ��������� ������ �ٸ� ���� �� ĳ���Ͱ� ���δٸ�
  ���̷����� ����� ����� �ΰ��� ��Ȳ�� ���� ���� �ٸ� ��Ÿ�� ����� �����Ѵ�. ���� ���������� ����ŷ��
  �������� ���ɸ��ʹ� ������ �� �̸��� ���̷��� ������ ��������, ���� �������� ���̷��� �ּ��� 
  ������ �ֱ� ������ �ٸ� ���̷����� ����Ҷ� �õ��ϴ� ���װ� ����� ����ߴ�.    
*/