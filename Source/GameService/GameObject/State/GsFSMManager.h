// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameObject/Define/GsGameObjectDefine.h"

/**
 * 상태 관리 매니져 클래스
 */
class UGsGameObjectBase;
class IGsStateBase;

class GAMESERVICE_API FGsFSMManager
{
	//ChangeState() 메서드 접근 허용 클래스(유일)
	friend IGsStateBase;

public:
	DECLARE_DELEGATE_OneParam(FGsStateChangeFailed, uint8/* State Id*/);

	FORCEINLINE IGsStateBase* CurrentState() const	{ return Current; }
	FORCEINLINE IGsStateBase* PrevState() const		{ return Prev; }
	bool IsState(uint8 StateID) const;
	bool IsState(EGsStateBase StateType) const;
	bool IsState(EGsStateUpperBase StateType) const;

	template <class tState>//시작 State정의
	void Initialize(UGsGameObjectBase* owner)
	{
		Owner = owner;
		Prev = nullptr;
		Current = tState::GetInstance();
		ChangeState(Current);
	}

	void Finalize();
	void Update(UGsGameObjectBase* Owner, float Delta);

	//스테이트 타입값 기반 상태 제어 처리
	//ChangeState() 호출은 상태 클래스 내에서만 가능하도록 Private 선언
	template <typename tStateType>
	void ProcessEvent(tStateType StateID, FGsStateChangeFailed const& FailDelegate = NULL)
	{
		ProcessEvent(static_cast<uint8>(StateID), FailDelegate);
	}

private:
	template <class tState>
	bool ChangeState()
	{
		auto state = tState::GetInstance();
		return ChangeState(state);
	}
	bool ChangeState(IGsStateBase* State);

	//FTimerManager를 통한 이벤트 처리 일정 시간후 스테이트 전환
	template <class tState>
	void ChangeDelayState(float Time)
	{
		auto state = tState::GetInstance();
		ChangeDelayState(state, Time);
	}
	void ChangeDelayState(IGsStateBase* State, float Time);
	void ChangeDelayPrevState(float Time);
	
	void CallbakChangeState(IGsStateBase* State);

	//IGsStateBase 선언에 대한 문제를 고민해본다.
	//현재는 ProcessEvent 템플릿 함수의 호출용으로 사용
	void ProcessEvent(uint8 StateID, FGsStateChangeFailed const& FailDelegate = NULL);

private:
	UGsGameObjectBase* Owner;
	IGsStateBase* Current;
	IGsStateBase* Prev;
};
