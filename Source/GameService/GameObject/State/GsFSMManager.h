﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

class IGsStateBase;

/**
 * 상태 관리 매니져 클래스
 */
class GAMESERVICE_API FGsFSMManager
{
	//ChangeState() 메서드 접근 허용 클래스(유일)
	friend class IGsStateBase;

public:
	DECLARE_DELEGATE_OneParam(FGsStateChangeFailed, uint8/* State Id*/);

	FORCEINLINE IGsStateBase* CurrentState() const	{ return Current; }
	FORCEINLINE IGsStateBase* PrevState() const		{ return Prev; }
	bool IsState(int StateID) const;

	template <class State>
	void Initialize(UGsGameObjectBase* Owner);		//시작 State정의
	void Finalize();

	//[Todo]
	//나중에 Update만이라도 캐스팅을 피하게 작업해보기....ㅠ
	//현재는 의미없는 템플릿..
	//template <class tChar>
	void Update(UGsGameObjectBase* Owner, float Delta);

	//스테이트 타입값 기반 상태 제어 처리
	//ChangeState() 호출은 상태 클래스 내에서만 가능하도록 Private 선언
	template <typename tStateType>
	void ProcessEvent(tStateType StateID, FGsStateChangeFailed const& FailDelegate = NULL);

private:
	template <class tState>
	bool ChangeState();
	bool ChangeState(IGsStateBase* State);

	//FTimerManager를 통한 이벤트 처리 일정 시간후 스테이트 전환
	template <class tState>
	void ChangeDelayState(float Time);
	void ChangeDelayState(IGsStateBase* State, float Time);
	void ChangeDelayPrevState(float Time);
	
	void CallbakChangeState(IGsStateBase* State);

private:
	UGsGameObjectBase* Owner;
	IGsStateBase* Current;
	IGsStateBase* Prev;
};

template <class tState>
void FGsFSMManager::Initialize(UGsGameObjectBase* owner)
{
	Owner = owner;
	Prev = nullptr;
	Current = tState::GetInstance();
	ChangeState(Current);
}

template <typename tStateType>
void FGsFSMManager::ProcessEvent(tStateType StateID, FGsStateChangeFailed const& FailDelegate)
{
	if (Current)
	{
		Current->ProcessEvent(Owner, static_cast<uint8>(StateID), FailDelegate);
	}
}

template <class tState>
bool FGsFSMManager::ChangeState()
{
	auto state = tState::GetInstance();
	return ChangeState(state);
}

template <class tState>
void FGsFSMManager::ChangeDelayState(float Time)
{
	auto state = tState::GetInstance();
	ChangeDelayState(state, Time);
}