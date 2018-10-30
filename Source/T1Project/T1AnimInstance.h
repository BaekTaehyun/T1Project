// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "T1AnimInstance.generated.h"

/**
 * Ŀ���� ����¡�� �ִϸ��̼� �ν��Ͻ� ó����
 */
UCLASS()
class T1PROJECT_API UT1AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UT1AnimInstance();
	virtual void NativeUpdateAnimation(float deltaSecond) override;

	void PlayAttackMontage();

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
};

/* p.239 �� ����(��Ÿ�� ���) 
  �𸮾� ������ �� ĳ���Ͱ� ���� ������ ���̷����� ��������� ������ �ٸ� ���� �� ĳ���Ͱ� ���δٸ�
  ���̷����� ����� ����� �ΰ��� ��Ȳ�� ���� ���� �ٸ� ��Ÿ�� ����� �����Ѵ�. ���� ���������� ����ŷ��
  �������� ���ɸ��ʹ� ������ �� �̸��� ���̷��� ������ ��������, ���� �������� ���̷��� �ּ��� 
  ������ �ֱ� ������ �ٸ� ���̷����� ����Ҷ� �õ��ϴ� ���װ� ����� ����ߴ�.    
*/