#pragma once

#include "GsContainer.h"

//----------------------------------------------------------------
// ���¸� ���� ������ �����ϱ����� ������� Ŭ����
// TGsMapAllocator Ȱ���ϸ� �������� ���� ��ü�� ������ ���ִ� 
// ex) �˻� GSGameModeManager
//----------------------------------------------------------------

template<typename T1, class  TGsState, class GSFGameModeAllocator>
class TGsStateMng : public GSTMap<T1, TGsState, GSFGameModeAllocator>
{
	TSharedPtr<TGsState>	_currentState = NULL;

	DECLARE_EVENT(GSTStateMng, MainEvent)
	DECLARE_EVENT_OneParam(GSTStateMng, StateEvent, T1)

	MainEvent				_onInit;
	MainEvent				_onRemoveAll;

	StateEvent				_onEnterState;
	StateEvent				_onLeaveState;

protected:
	TSharedRef<TGsState>	GetState()
	{
		return _currentState.ToSharedRef();
	}
public:
	TGsStateMng<T1, TGsState, GSFGameModeAllocator>() : GSTMap<T1, TGsState, GSFGameModeAllocator>() {};
	virtual ~TGsStateMng<T1, TGsState, GSFGameModeAllocator>() {};

	MainEvent&	OnInit() const {	return _onInit;	}
	MainEvent&	OnRemoveAll() const { return _onRemoveAll; }
	StateEvent& OnEnterState() const { return _onEnterState; }
	StateEvent& OnLeaveState() const { return _onLeaveState; }
	//------------------------------------------------------------------------------
	virtual void RemoveAll()
	{ 
		_onRemoveAll.Broadcast();

		if (_currentState.IsValid())
		{
			_currentState.Get()->Exit();
			_currentState = NULL;
		}
		
		GSTMap<T1, TGsState, GSFGameModeAllocator>::Clear();
	};

	//------------------------------------------------------------------------------
	// �߰�(����°� �ִ��� ��ӿ��� �����ؼ� �߰����ش�.)
	virtual void InitState()
	{
		_onInit.Broadcast();
	}

	//------------------------------------------------------------------------------
	void ChangeState(T1 inState)
	{
		auto instance = Find(inState);

		if (_currentState.IsValid())
		{
			_currentState.Get()->Exit();
			_onLeaveState.Broadcast(_currentState.Get()->GetType());
			GSLOG(Warning, TEXT("GSTStateMng : ChangeState [%d] Exit"), _currentState.Get()->GetType());
		}

		_currentState = instance;

		if (_currentState.IsValid())
		{
			_currentState.Get()->Enter();
			_onEnterState.Broadcast(_currentState.Get()->GetType());
			GSLOG(Warning, TEXT("`GSTStateMng : ChangeState [%d] Enter"), _currentState.Get()->GetType());
		}
	}
};
