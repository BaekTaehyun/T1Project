// Fill out your copyright notice in the Description page of Project Settings.

#include "T1AnimInstance.h"
#include "T1Player.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "ConstructorHelpers.h"

UT1AnimInstance::UT1AnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsAir = false;

	//애니메이션 몽타주로드
	/*
		몽타주란 여러 애니메이션을 합치고 줄이거나 늘리는등 편집과정을 거쳐 1개의 이름으로 관리되는 애니메이션 셋이라보면됨
	*/
	// ConstructorHelpers 해당계열의 함수를 static으로 선언한것에 유의(생성자임으로 계열 클래스가 인스턴싱될때마다 호출하는것을 방지)
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
	// AT1Player 클래스에서 델리게이트 처리를 통해 플레이체크를 하기때문에 주석
	// Montage_IsPlaying 같은 함수도 있음을 인지하길바라며.
	//if (false == Montage_IsPlaying(AttackMontage))
	{
		Montage_Play(AttackMontage, 1.0f);
	}
}