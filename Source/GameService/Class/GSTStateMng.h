#pragma once

#include "GSTContainer.h"

//----------------------------------------------------------------
// 상태를 가진 구조를 관리하기위해 만들어진 클래스
// GSTMapAllocator 활용하면 여러가지 관리 객체를 구성할 수있다 
// ex) 검색 GSGameModeManager
//----------------------------------------------------------------

template<typename T1, class  GSTState, class GSFGameModeAllocator>
class GSTStateMng : public GSMap<T1, GSTState, GSFGameModeAllocator>
{
	TSharedPtr<GSTState>	_currentState = NULL;
public:
	GSTStateMng<T1, GSTState, GSFGameModeAllocator>() : GSMap<T1, GSTState, GSFGameModeAllocator>() {};
	virtual ~GSTStateMng<T1, GSTState, GSFGameModeAllocator>() {};

	//------------------------------------------------------------------------------
	virtual void RemoveAll()
	{ 
		if (_currentState.IsValid())
		{
			_currentState = NULL;
		}
		
		Clear();
	};

	//------------------------------------------------------------------------------
	// 추가
	virtual void InitState() = 0;

	//------------------------------------------------------------------------------
	void ChangeState(T1 inState)
	{
		auto instance = Find(inState);

		if (_currentState.IsValid())
		{
			_currentState.Get()->Exit();
			GSLOG(Warning, TEXT("GSTStateMng : ChangeState [%d] Exit"), _currentState.Get()->GetType());
		}

		_currentState = instance;

		if (_currentState.IsValid())
		{
			_currentState.Get()->Enter();
			GSLOG(Warning, TEXT("`GSTStateMng : ChangeState [%d] Enter"), _currentState.Get()->GetType());
		}
	}
};
