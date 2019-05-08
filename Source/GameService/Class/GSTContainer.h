#pragma once

#include "CoreMinimal.h"
#include "GameService.h"
#include <typeinfo>

//----------------------------------------------------------------
// 매니징 클래스 기본형(Array)
// TArray, TMap 를 감싼이유는 new delete로 생성/소멸하는 객체들을 통일화된 규격으로 관리하기 위함입니다.
// 제대로 소멸되지 않았을때 로드출력을 추가했으며 내부에서 TShardedPtr를 통해관리함으로 가져다 쓰는쪽에서는 Ref카운트를 주의해주세요 (다쓰고 NULL만 호출하면됨)
//----------------------------------------------------------------

template< typename T >
class GSArray
{
	TArray<TSharedPtr<T>>	_container;
public:
	//소멸처리
	virtual ~GSArray()
	{
		// 소멸자에서는 절대 버추얼 함수를 불려서는 안됩니다
		//https://ljh131.tistory.com/16
		Clear();
	}
	// ----------------------------------------------------------------
	// 접근자 추가
	TSharedRef<T> operator[](int index)
	{
		return _container.GetData()[i].ToSharedRef();
	}

	//----------------------------------------------------------------
	// 소멸시 전체 소멸 
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
	// 내부 인스턴스 생성
	virtual TSharedRef<T> MakeInstance()
	{
		T* instance = new T();
		GSCHECK(instance);
		_container.Add(MakeShareable(instance));
		return _container.Last().ToSharedRef();
	}
	//----------------------------------------------------------------
	// 외부 인스턴스 입력
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
// T2형 할당자(고정 객체가 아닐수 있기에 할당자를 지정해야함
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
// 매니징 클래스 기본형(Map)
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


