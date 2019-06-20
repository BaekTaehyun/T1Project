// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GsAnimInstanceState.generated.h"

/**
* 외부 노출용 Enum정의
* int형으로 전달할수 있지만 외부에서 타입에 대한 명확성을 주기 위해 재정의
* EGsState... 타입들이 추가/삭제 될때마다 동기화 시켜줘야 한다....
* 더 좋은 방법에 대한 고민이 필요
* (실제 로직에서 사용되는 StateType을 기능에 따라 세분화할 필요가 있는지 확인이 필요)
*
UENUM()
enum class EBPAnimState : uint8
{
	//--------------------
	//EGsStateBase
	//--------------------
	None = 0,
	Spawn,
	Idle,
	ForwardWalk,
	BackwardWalk,
	SideWalk,
	Run,

	Attack,
	Beaten,
	Die,

	//--------------------
	//EGsStatePlayer
	//--------------------
	None2,
	Ride,
	StatePlayerMax,
};
*/

/**
 * 애님 블루프린트 동기화용 클래스
 * 애님 블루프린트의 상태머신은 BaseStateType, UpperStateType 설정값 체크로만 변환(transition)을 일으켜야한다.
 */
UCLASS()
class GAMESERVICE_API UGsAnimInstanceState : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	//상태 정보
	//FSMManager에서 직접 참조 하고 싶은데 나이스한 방법을 아직 잘모르겠음 ㅠ
	EGsStateBase StateType;

protected:
	bool Moving;
	//랜덤값 사용 캐시 정보
	//추후EStateBase(Enum) 타입과 함께 구조체로 만들어 사용해야할듯...
	int RandomIndex;
	//변환뒤 지속 시간 체크
	float Timer, UpperTimer;

public:
	//애님 플루프린트 FSM동기화 참조 함수
	UFUNCTION(BlueprintPure, Category = "MyAnimation", meta = (BlueprintThreadSafe))
	bool IsState(EGsStateBase State);
	UFUNCTION(BlueprintPure, Category = "MyAnimation", meta = (BlueprintThreadSafe))
	bool IsStates(const TArray<EGsStateBase>& States);
	UFUNCTION(BlueprintPure, Category = "MyAnimation", meta = (BlueprintThreadSafe))
	bool IsUpperBlend();
	UFUNCTION(BlueprintPure, Category = "MyAnimation", meta = (BlueprintThreadSafe))
	bool IsMoveState();
	UFUNCTION(BlueprintPure, Category = "MyAnimation", meta = (BlueprintThreadSafe))
	int GetRandomIndex();
	UFUNCTION(BlueprintPure, Category = "MyAnimation", meta = (BlueprintThreadSafe))
	float GetTimer();
	UFUNCTION(BlueprintPure, Category = "MyAnimation", meta = (BlueprintThreadSafe))
	float GetUpperTimer();

public:
	UGsAnimInstanceState();
	virtual ~UGsAnimInstanceState();

    //[Todo]
	//FSMManager 애님 블루프린트 FSM 파라미터 동기화
	//FSMManager에서 직접 참조하고 싶지만.. 아직 잘모르겠음..
	//문제점1. AMyCharacter 객체 참조를 하려해도 에디터상 문제가 생김(CDO)
	//문제점2. 애님 블루프린트 상태 파라미터 검사시 쓰레드 세이프 하지 않음(AMyCharacter 형변환 노드 사용시)
	void ChangeState(uint8 State, int Min = 0, int Max = 0);
	void SetMoving(bool IsMove);

	void PlayUpperAni(UAnimMontage* Res);
	void StopUpperAni(UAnimMontage* Res);
};
