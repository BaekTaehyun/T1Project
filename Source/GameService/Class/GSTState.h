#pragma once

template<typename T1>
class GSTState
{
private:
	T1	_type;

public:
	GSTState(T1 inType) : _type(inType) {}
	virtual ~GSTState() {}

	T1 GetType()
	{
		return _type;
	}
	
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update() = 0;
};