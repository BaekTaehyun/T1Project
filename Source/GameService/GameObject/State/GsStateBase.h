// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <mutex>
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"
#include "GameObject/Component/Animation/GsAnimInstanceState.h"

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
	virtual bool IsChange(int StateID);
	virtual bool IsSameState(int StateID);

	virtual void Enter(UGsGameObjectBase* Owner) {}
	virtual void ReEnter(UGsGameObjectBase* Owner) {}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) {}
	virtual void Exit(UGsGameObjectBase* Owner) {}
};

template <class T>
class TGsStateSingleton : public IGsStateBase
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
template <class T, class U>
class GAMESERVICE_API FGsStateTargetBase : public TGsStateSingleton<U>
{
public:
	virtual void Enter(UGsGameObjectBase* Owner) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnEnter(Cast<T>(Owner));
	}
	virtual void ReEnter(UGsGameObjectBase* Owner) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnReEnter(Cast<T>(Owner));
	}
	virtual void Update(UGsGameObjectBase* Owner, float Delta) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnUpdate(Cast<T>(Owner), Delta);
	}
	virtual void Exit(UGsGameObjectBase* Owner) override
	{
		//T* castOwner = static_cast<T*>(Owner->cast());
		OnExit(Cast<T>(Owner));
	}

protected:
	virtual void OnEnter(T* Owner) = 0;
	virtual void OnReEnter(T* Owner) = 0;
	virtual void OnUpdate(T* Owner, float Delta) = 0;
	virtual void OnExit(T* Owner) = 0;
};


