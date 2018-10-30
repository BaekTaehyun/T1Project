// Fill out your copyright notice in the Description page of Project Settings.

#include "T1AnimInstance.h"
#include "T1Player.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "ConstructorHelpers.h"

UT1AnimInstance::UT1AnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsAir = false;

	//�ִϸ��̼� ��Ÿ�ַε�
	/*
		��Ÿ�ֶ� ���� �ִϸ��̼��� ��ġ�� ���̰ų� �ø��µ� ���������� ���� 1���� �̸����� �����Ǵ� �ִϸ��̼� ���̶󺸸��
	*/
	// ConstructorHelpers �ش�迭�� �Լ��� static���� �����ѰͿ� ����(������������ �迭 Ŭ������ �ν��Ͻ̵ɶ����� ȣ���ϴ°��� ����)
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(
		TEXT("/Game/InfinityBladeWarriors/Character/Animation/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}

}

void UT1AnimInstance::NativeUpdateAnimation(float deltaSecond)
{
	Super::NativeUpdateAnimation(deltaSecond);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		auto Character = Cast<AT1Player>(Pawn);
		if (Character)
		{
			IsAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UT1AnimInstance::PlayAttackMontage()
{	
	// AT1Player Ŭ�������� ��������Ʈ ó���� ���� �÷���üũ�� �ϱ⶧���� �ּ�
	// Montage_IsPlaying ���� �Լ��� ������ �����ϱ�ٶ��.
	//if (false == Montage_IsPlaying(AttackMontage))
	{
		Montage_Play(AttackMontage, 1.0f);
	}
}