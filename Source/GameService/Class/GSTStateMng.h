#pragma once

#include "GSTContainer.h"

template<typename T1, class  GSTState, class GSFGameModeAllocator>
class GSTStateMng : public GSMap<T1, GSTState, GSFGameModeAllocator>
{
	TSharedPtr<GSTState>	_currentState = NULL;
public:
	GSTStateMng<T1, GSTState, GSFGameModeAllocator>() : GSMap<T1, GSTState, GSFGameModeAllocator>() {};
	virtual ~GSTStateMng<T1, GSTState, GSFGameModeAllocator>() {};

	virtual void RemoveAll()
	{ 
		if (_currentState.IsValid())
		{
			_currentState = NULL;
		}
		
		Clear();
	};
	// Ãß°¡
	virtual void InitState() = 0;

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
