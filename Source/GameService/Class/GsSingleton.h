#pragma once

// �������� �̱����� ���� �ʴ´�.
template<typename T>
class GAMESERVICE_API TGsSingleton
{
protected:
	static T* _instance;
	virtual ~TGsSingleton() {}
	static void InitInstance(T* inThis)
	{
		_instance = inThis;
	}
	static void RemoveInstance()
	{
		_instance = NULL;
	}
public:
	static T* Instance()
	{
		return _instance;
	}
};

// Ǯ�� ��ü�� ���� �̱��� ����.. �ٸ���ó�δ� ���� �ʱ�
// �Ҹ��� �� ȣ�����ֱ�
template<typename T>
class GAMESERVICE_API TGsPoolSingle
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

