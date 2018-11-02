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
		TEXT("/Game/InfinityBladeWarriors/Character/Animation/SK_WarriorMontage.SK_WarriorMontage"));
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
	Montage_Play(AttackMontage, 1.0f);
}

void UT1AnimInstance::JumpToAttackMontageSection(int32 newSection)
{
	T1CHECK(Montage_IsPlaying(AttackMontage));
	Montage_JumpToSection(GetAttackMontageSectionName(newSection), AttackMontage);
}

void UT1AnimInstance::AnimNotify_AttackHitCheck()
{
	T1LOG_S(Log);
	//OnAttackHitCheck.Broadcast();
}

void UT1AnimInstance::AnimNotify_NextAttackCheck()
{
	T1LOG_S(Log);
	OnNextAttackCheck.Broadcast();
}

FName UT1AnimInstance::GetAttackMontageSectionName(int32 section)
{
	T1CHECK(FMath::IsWithinInclusive<int32>(section, 1, 4), NAME_None);
	return FName(*FString::Printf(TEXT("Attack%d"), section));
}

