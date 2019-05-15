// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GsStateBase.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

/**
 * 
 */
class GAMESERVICE_API FGsFSMManager
{
    DECLARE_DELEGATE_OneParam(FGsStateChangeFailed, int32/* State Id*/);

public:
	FORCEINLINE IGsStateBase* CurrentState() const { return Current; }
	FORCEINLINE IGsStateBase* PrevState() const	{ return Prev; }
	FORCEINLINE bool IsState(int StateID) const
	{
		return (nullptr != Current && Current->GetStateID() == StateID);
	}

	//region
	void Initialize(UGsGameObjectBase* Owner);
	void DeInitialize();

	template <class Char>
	void Update(Char* Owner, float Delta);
	template <class State>
	bool ChangeState(FGsStateChangeFailed const& FailDelegate = nullptr);
	//FTimerManager를 통한 이벤트 처리 일정 시간후 스테이트 전환
	template <class State>
	void ChangeDelayState(float Time);
	void ChangeDelayState(IGsStateBase* State, float Time);
	void ChangeDelayPrevState(float Time);

private:
	bool ChangeState(IGsStateBase* State, FGsStateChangeFailed const& FailDelegate = NULL);
	void CallbakChangeState(IGsStateBase* State);

private:
	UGsGameObjectBase* Owner;
	IGsStateBase* Current;
	IGsStateBase* Prev;
};

//[Todo]
//나중에 Update만이라도 캐스팅을 피하게 작업해보기....ㅠ
template <class Char>
void FGsFSMManager::Update(Char* Owner, float Delta)
{
	if (nullptr != Current)
	{
		Current->Update(Owner, Delta);
	}
}

template <class State>
bool FGsFSMManager::ChangeState(FGsStateChangeFailed const& FailDelegate)
{
	State* state = State::GetInstance();
	return ChangeState(state, FailDelegate);
}

template <class State>
void FGsFSMManager::ChangeDelayState(float Time)
{
	State* state = State::GetInstance();
	ChangeDelayState(state, Time);
}