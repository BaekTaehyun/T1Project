// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "T1AnimInstance.generated.h"

/*
  몽타주에 선언된 NextAttackCheck 애니메이션 노티파이가 발생할때마가 T1Character에 전달할 델리게이트 선언
*/
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);


/**
 * 커스터 마이징된 애니메이션 인스턴스 처리예
   UAnimInstance 클래스는 애니메이션 블루프린트와 매칭되는 언리얼 클래스입니다.
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
		몽타주에 정의된 커스텀 노티파이인 경우에는 애님인스턴스 클래스의 'AnimNotify_노티파이명'이라는 이름의 멤버 함수를 찾아서 호출
		또한 불려지는 함수는 언리얼 런타임이 찾을수 있도록 반드시 UFUNCTION 매크로가 지정되어야 한다.
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
		UPROPERTY에 설정하는 키워드인 EditAnyWhere와 VisibleAnywhere에 있는 Anywhere 키워드는 에디터와의
		연동 방법에 따라 DefaultOnly와 InstacneOnly로 세분화될 수 있다 DefaultOnly는 클래스의 기본값을
		담당값을 담당하는 블루프린트 편집 화면에서만 보여지고 InstanceOnly는 인스턴스의 속성을 보여주는 에디터 뷰포트에서만
		보여진다. 이번세팅은 애니메이션 블루프린트 클래스 설계도에서만 보여지도록 VisibleDefaultsOnly로 지정
	*/
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	bool IsDead;
};

/* p.239 쪽 참고(리타겟 방법) 
  언리얼 엔진은 두 캐릭터가 같은 구조의 스켈레톤을 사용하지만 비율이 다를 때와 두 캐릭터가 서로다른
  스켈레톤을 사용할 때라는 두가지 상황에 대해 각기 다른 리타겟 방법을 제공한다. 현재 예제에서는 마네킹과
  워리어의 두케릭터는 동일한 본 이름과 스켈레톤 구조를 가지지만, 각자 독립적인 스켈레톤 애셋을 
  가지고 있기 때문에 다른 스켈레톤을 사용할때 시도하는 리테겟 방법을 사용했다.    
*/