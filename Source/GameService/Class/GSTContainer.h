#pragma once

#include "CoreMinimal.h"
#include "GameService.h"
#include <typeinfo>

//----------------------------------------------------------------
// �Ŵ�¡ Ŭ���� �⺻��(Array)
// TArray, TMap �� ���������� new delete�� ����/�Ҹ��ϴ� ��ü���� ����ȭ�� �԰����� �����ϱ� �����Դϴ�.
// ����� �Ҹ���� �ʾ����� �ε������ �߰������� ���ο��� TShardedPtr�� ���ذ��������� ������ �����ʿ����� Refī��Ʈ�� �������ּ��� (�پ��� NULL�� ȣ���ϸ��)
//----------------------------------------------------------------

template< typename T >
class GSArray
{
	TArray<TSharedPtr<T>>	_container;
public:
	//�Ҹ�ó��
	virtual ~GSArray()
	{
		// �Ҹ��ڿ����� ���� ���߾� �Լ��� �ҷ����� �ȵ˴ϴ�
		//https://ljh131.tistory.com/16
		Clear();
	}
	// ----------------------------------------------------------------
	// ������ �߰�
	TSharedRef<T> operator[](int index)
	{
		return _container.GetData()[i].ToSharedRef();
	}

	//----------------------------------------------------------------
	// �Ҹ�� ��ü �Ҹ� 
	void Clear()
	{
		for (auto& a : _container)
		{
			if (a.IsUnique() && a.IsValid())
			{
				a = NULL;
			}
			else
			{
				GSLOG(Error, TEXT("GSNArray : Ref Count Not 1, Reset Call %s"), typeid(T).name());
			}
		}
		_container.Empty();
	}
	//----------------------------------------------------------------
	// ���� �ν��Ͻ� ����
	virtual TSharedRef<T> MakeInstance()
	{
		T* instance = new T();
		GSCHECK(instance);
		_container.Add(MakeShareable(instance));
		return _container.Last().ToSharedRef();
	}
	//----------------------------------------------------------------
	// �ܺ� �ν��Ͻ� �Է�
	virtual void InsertInstance(const T* instance)
	{
		GSCHECK(instance);
		_container.Add(MakeShareable(instance));
	}

	//----------------------------------------------------------------
	virtual void Remove(TSharedPtr<T>)
	{
		_container.Remove(instance);
		if (instance.IsUnique() && a.IsValid())
		{
			instance = NULL;
		}
		else
		{
			GSLOG(Error, TEXT("GSTArray : Ref Count Not 1, Reset Call %s"), typeid(T).name());
		}
	}
};

//----------------------------------------------------------------
// T2�� �Ҵ���(���� ��ü�� �ƴҼ� �ֱ⿡ �Ҵ��ڸ� �����ؾ���
//----------------------------------------------------------------
template<typename T1, typename T2>
class GSTMapAllocator
{
public:
	GSTMapAllocator() {}
	virtual ~GSTMapAllocator() {}
	//----------------------------------------------------------------
	virtual T2* Alloc(T1 type)
	{
		return new T2();
	}
};

//----------------------------------------------------------------
// �Ŵ�¡ Ŭ���� �⺻��(Map)
//----------------------------------------------------------------
template< typename T1, typename T2, class Alloc = GSTMapAllocator<T1, T2>>
class GSMap
{
	TSharedPtr<Alloc>	_allocator;
	TMap<T1, TSharedPtr<T2>> _container;
public:

	GSMap() { _allocator = TSharedPtr<Alloc>(new Alloc()); }
	virtual ~GSMap() { Clear(); }
	//----------------------------------------------------------------
	void Clear()
	{
		if (_allocator.IsValid())
		{
			_allocator = NULL;
		}

		for (auto& a : _container)
		{
			if (a.Value.IsUnique() && a.Value.IsValid())
			{
				a.Value = NULL;
			}
			else
			{
				GSLOG(Error, TEXT("GSTMap : Ref Count Not 1, Reset Call Key[%s] Value[%s]"), typeid(T1).name(), typeid(T2).name());
			}
		}
		_container.Empty();
	}

	//----------------------------------------------------------------
	virtual TSharedRef<T2> MakeInstance(T1 inKey)
	{
		if (_allocator.IsValid())
		{
			T2* instance = _allocator.Get()->Alloc(inKey);
			if (nullptr == instance)
			{
				GSLOG(Error, TEXT("GSTMap : _allocator MakeInstance NULL, Call Key[%s] Value[%s]"), typeid(T1).name(), typeid(T2).name());
				return TSharedRef<T2>();
			}
			TSharedPtr<T2> shard_instance = MakeShareable(instance);
			_container.Add(inKey, shard_instance);
			return shard_instance.ToSharedRef();
		}
		return TSharedRef<T2>();
	}
		
	//----------------------------------------------------------------
	virtual void Remove(T1 inKey)
	{
		auto instance = _container.FindAndRemoveChecked(inKey);
		if (instance.IsUnique() && instance.IsValid())
		{
			instance = NULL;
		}
		else
		{
			GSLOG(Error, TEXT("GSTMap : Ref Count Not 1, Reset Call Key[%s] Value[%s]"), typeid(T1).name(), typeid(T2).name());
		}
	}
	//----------------------------------------------------------------
	virtual TSharedRef<T2> Find(T1 inKey)
	{
		auto instance = _container.Find(inKey);
		return instance->ToSharedRef();
	}
};


