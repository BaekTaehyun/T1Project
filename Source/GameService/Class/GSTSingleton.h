#pragma once

// 동적생성 싱글톤을 쓰지 않는다.
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

