#pragma once

// �������� �̱����� ���� �ʴ´�.
template<typename T>
class TGsSingleton
{
public:
	virtual ~TGsSingleton() {}
	static T* Instance;
protected:
	static void InitInstance(T* inThis)
	{
		Instance = inThis;
	}
	static void RemoveInstance()
	{
		Instance = NULL;
	}
};

