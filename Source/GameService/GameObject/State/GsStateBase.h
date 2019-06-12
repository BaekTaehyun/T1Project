// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <mutex>

#include "Util/GsText.h"
#include "GsFSMManager.h"
#include "GameObject/Component/GsAnimInstanceState.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

/**
 * State 객체의 기본이 되는 Abstruct클래스 
 */
class GAMESERVICE_API IGsStateBase
{
public:
	IGsStateBase();
	virtual ~IGsStateBase();

	virtual uint8 GetStateID()		{ return 0; }
	virtual FString Name() = 0;

	//애니메이션 랜덤 값 관련 처리이지만 테스트용 프로퍼티에 가깝다.(삭제될 메서드)
	virtual int GetAniRandomCount() { return 0; }

	//체인지 가능 or 불가능(Black List | White List) 상태 정의
	//ProcessEvent에서 처리로 변경하여 삭제
	//virtual bool IsChange(int StateID);
	virtual bool IsSameState(uint8 StateID);		

	//State의 전환은 이 메서드를 통해서만 전환
	virtual bool ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID) { return false; }

	virtual void Enter(UGsGameObjectBase* Owner)				{}
	virtual void ReEnter(UGsGameObjectBase* Owner)				{}
	virtual void Update(UGsGameObjectBase* Owner, float Delta)	{}
	virtual void Exit(UGsGameObjectBase* Owner)					{}

	//FSM상태 전환 접근 메서드
protected:
	template <class tState>
	bool ChangeState(FGsFSMManager* Fsm)
	{
		if (Fsm) { return Fsm->ChangeState<tState>(); }
		return false;
	}
	
	template <class tState>
	void ChangeDelayState(FGsFSMManager* Fsm, float Time)
	{
		if (Fsm) { Fsm->ChangeDelayState<tState>(Time); }
	}

	void ChangeDelayPrevState(FGsFSMManager* Fsm, float Time);
};

template <class T>
class GAMESERVICE_API TGsStateSingleton
{
protected:
	TGsStateSingleton() = default;
	//StateSingleton(const StateSingleton<T>& rhs) = delete;
	//StateSingleton<T>& operator = (const StateSingleton<T>& rhs) = delete;

private:
	static TUniquePtr<T> _instance;
	//static std::once_flag _flag1;

public:
	static T* GetInstance() {
		if(false == _instance.IsValid())
		{
			_instance = TUniquePtr<T>(new T());
		}
		return _instance.Get();
	}
};

template <typename T> TUniquePtr<T> TGsStateSingleton<T>::_instance;