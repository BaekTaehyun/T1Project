#pragma once

// �������� �̱����� ���� �ʴ´�.
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

// Ǯ�� ��ü�� ���� �̱��� ����.. �ٸ���ó�δ� ���� �ʱ�
// �Ҹ��� �� ȣ�����ֱ�
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
