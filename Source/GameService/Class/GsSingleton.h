#pragma once

// 동적생성 싱글톤을 쓰지 않는다.
template<typename T>
class GAMESERVICE_API TGsSingleton
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

// 풀링 객체를 위한 싱글톤 생성.. 다른용처로는 쓰지 않기
// 소멸은 꼭 호출해주기
template<typename T>
class TGsPoolSingle
{
	static T* _instance;
public:
	static T* GetInstance()
	{
		if (nullptr == _instance)
		{
			_instance = new T();
		}
		return _instance;
	}
	static void RemoveInstance()
	{
		if (nullptr != _instance)
		{
			delete _instance;
			_instance = nullptr;
		}
	}
};
