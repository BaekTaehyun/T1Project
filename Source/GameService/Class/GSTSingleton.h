#pragma once

// �������� �̱����� ���� �ʴ´�.
template<typename T>
class GSTSingleton
{
public:
	virtual ~GSTSingleton() {}
	static T* Instance;
	static void InitInstance(T* inThis)
	{
		Instance = inThis;
	}
	static void RemoveInstance()
	{
		Instance = NULL;
	}
};

