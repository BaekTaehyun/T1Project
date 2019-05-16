#pragma once

//------------------------------------------------------------------------------
// ���¸� ��Ÿ���� �⺻Ŭ����
// GSTStateMng �� �Բ� ����
//------------------------------------------------------------------------------
template<typename T1>
class TGsState
{
private:
	T1	_type;

public:
	TGsState(T1 inType) : _type(inType) {}
	virtual ~TGsState() {}

	T1 GetType()
	{
		return _type;
	}
	
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update() = 0;
};