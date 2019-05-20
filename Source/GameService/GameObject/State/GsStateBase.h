// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <mutex>

#include "Util/GsText.h"
#include "GsFSMManager.h"
#include "GameObject/Component/Animation/GsAnimInstanceState.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

/**
 *
 */
class GAMESERVICE_API IGsStateBase
{
public:
	IGsStateBase();
	virtual ~IGsStateBase();

	virtual int GetStateID() = 0;
	virtual FString Name() = 0;
	virtual int GetAniRandomCount() { return 0; }

	//체인지 가능 or 불가능(Black List | White List) 상태 정의
	//ProcessEvent에서 처리로 변경
	//virtual bool IsChange(int StateID);
	virtual bool IsSameState(int StateID);

	//State의 전환은 이 메서드를 통해서만 전환
	virtual void ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID, FGsFSMManager::FGsStateChangeFailed const& FailDelegate = NULL) {}

	virtual void Enter(UGsGameObjectBase* Owner) {}
	virtual void ReEnter(UGsGameObjectBase* Owner) {}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) {}
	virtual void Exit(UGsGameObjectBase* Owner) {}

	//FSM상태 전환 접근 메서드
protected:
	template <class tState>
	bool ChangeState(FGsFSMManager* Fsm)
	{
		if (Fsm) { Fsm->ChangeState<tState>(); }
	}

	template <class tState>
	void ChangeDelayState(FGsFSMManager* Fsm, float Time)
	{
		if (Fsm) { Fsm->ChangeDelayState<tState>(Time); }
	}

	void ChangeDelayPrevState(FGsFSMManager* Fsm, float Time)
	{
		if (Fsm && Fsm->Prev) { Fsm->ChangeDelayPrevState(Time); }
	}

//타이핑 실수 방지 내부 사용 메크로 정의
#define ObjectBaseStateChange(State) ChangeState<State>(Owner->GetBaseFSM())
#define ObjectUpperStateChange(State) ChangeState<State>(Owner->GetUpperFSM())
};

template <class T>
class GAMESERVICE_API TGsStateSingleton : public IGsStateBase
{
protected:
	TGsStateSingleton() = default;
	//StateSingleton(const StateSingleton<T>& rhs) = delete;
	//StateSingleton<T>& operator = (const StateSingleton<T>& rhs) = delete;

private:
	static std::unique_ptr<T> _instance;
	static std::once_flag _flag1;

public:
	static T* GetInstance() {
		std::call_once(_flag1, []() {
			_instance.reset(new T);
		});

		return _instance.get();
	}
};

template <typename T> std::unique_ptr<T> TGsStateSingleton<T>::_instance;
template <typename T> std::once_flag TGsStateSingleton<T>::_flag1;

/**
* [Todo] 
* FSM의 상태 클래스들은 자주 호출되고, 싱글톤 객체이므로 다중상속에 의한 캐스팅보다(Owner->cast())
* 추후 템플릿 구현으로 수정
*/
template <class tObject, class tState, typename tStateType>
class GAMESERVICE_API FGsStateTargetBase : public TGsStateSingleton<tState>
{
public:
	virtual void ProcessEvent(UGsGameObjectBase* Owner, uint8 StateID, FGsFSMManager::FGsStateChangeFailed const& FailDelegate = NULL) override
	{
		auto stateType = static_cast<tStateType>(StateID);
		if (!OnProcessEvent(Cast<tObject>(Owner), stateType))
		{
			if (FailDelegate.IsBound())
			{
				FailDelegate.Execute(GetStateID());
			}
#if WITH_EDITOR
			//스테이트 전환 실패 로그 메세지
			FString stateName = (StateID > static_cast<uint8>(EGsStateUpperBase::None)) ?
				FGsTextUtil::GetEnumValueAsString<tStateType>(TEXT("EGsStateUpperBase"), stateType) :
				FGsTextUtil::GetEnumValueAsString<tStateType>(TEXT("EGsStateBase"), stateType);
			UE_LOG(LogTemp, Warning, TEXT("[%s] State Changed failed!  Current State : [%s]"), *stateName, *Name());
#endif
		}
	}
	virtual void Enter(UGsGameObjectBase* Owner) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnEnter(Cast<tObject>(Owner));
	}
	virtual void ReEnter(UGsGameObjectBase* Owner) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnReEnter(Cast<tObject>(Owner));
	}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnUpdate(Cast<tObject>(Owner), Delta);
	}
	virtual void Exit(UGsGameObjectBase* Owner) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnExit(Cast<tObject>(Owner));
	}

protected:
	virtual bool OnProcessEvent(tObject* Owner, tStateType StateID) = 0;

	virtual void OnEnter(tObject* Owner) = 0;
	virtual void OnReEnter(tObject* Owner) = 0;
	virtual void OnUpdate(tObject* Owner, float Delta) = 0;
	virtual void OnExit(tObject* Owner) = 0;
};


