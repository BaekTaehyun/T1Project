#pragma once
#include "GsMessage.h"
#include "CoreMinimal.h"
#include "DelegateInstanceInterface.h"

//------------------------------------------------------------------------------
// 이벤트를 전달하기위한 기본객체클래스
// 가변인자를 지원하는듯 보이지만 델리게이트의 인자분기를 중첩구성할수 없음
//------------------------------------------------------------------------------
template<typename T1, typename... ParamTypes>
class TGsMessageHandler
{
public:
	virtual ~TGsMessageHandler() { RemoveAll(); }
	DECLARE_EVENT(TGsMessageHandler, MessageType)

private:
	TMap<T1, MessageType>	_delieveryAddr;
	/** A critical section for settings access */
	mutable FCriticalSection CriticalSection;
	TArray<T1>				_AsyncMessage;
public:
	void RemoveAll() { _delieveryAddr.Empty(); }
	//-------------------------------------------------------------------------
	// 메세지 등록
	virtual void InsertMessage() {};
	TMap<T1, MessageType>& GetDeliveryAddress() { return _delieveryAddr; }

	// 메시지 외부등록
	// ex) GSFMessageSingle::Instance->GetSystem().AddRaw(MessageSystem::ID::RECONNECT_END, this, &FGsGameModeManager::OnReconnectionEnd);
	template <typename UserClass, typename... VarTypes>
	FDelegateHandle AddRaw(T1 Message, UserClass* InUserObject,
		typename TMemFunPtrType<false, UserClass, void(ParamTypes..., VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TGsMessageHandler<T1>::MessageType delFunc;
		auto Result = delFunc.AddRaw(InUserObject, InFunc);
		_delieveryAddr.Add(Message, delFunc);
		return Result;
	}

	//-------------------------------------------------------------------------
	// 메세지 전송(Sync 용)
	virtual void SendMessage(const T1& id)
	{
		auto delegateFunc = _delieveryAddr.FindRef(id);
		delegateFunc.Broadcast();
	}

	virtual void SendMessage(T1&& id)
	{
		auto delegateFunc = _delieveryAddr.FindRef(id);
		delegateFunc.Broadcast();
	}
	//-------------------------------------------------------------------------
	// 메세지 전송(ASync 용) : 다른 Thread에서 메인쓰레드로 요청할때( Hive, 다운로드, ThreadJob등)
	// 큐에 메시지를 담고 메인루프에서 꺼낸다음 일괄호출하는 방식사용
	virtual void SendMessageAsync(T1& id)
	{
		FScopeLock InsertLock(&CriticalSection);
		_AsyncMessage.Add(id);
	}

	virtual void SendMessageAsync(T1&& id)
	{
		FScopeLock InsertLock(&CriticalSection);
		_AsyncMessage.Add(std::move(id));
	}

	virtual void Update()
	{
		TArray<T1> copyQue;
		{
			FScopeLock InsertLock(&CriticalSection);
			copyQue = _AsyncMessage;
			_AsyncMessage.Empty();
		}

		for (auto & message : copyQue)
		{
			SendMessage(message);
		}
	}
};


//------------------------------------------------------------------------------
// 이벤트를 전달하기위한 기본객체클래스
// GSTMessage 와 같이 쓰인다.
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class TGsMessageHandlerOneParam
{
public:
	virtual ~TGsMessageHandlerOneParam() { RemoveAll(); }
	DECLARE_EVENT_OneParam(TGsMessageHandlerOneParam, MessageType, const T2&)

private:
	TMap<T1, MessageType>	_delieveryAddr;
	/** A critical section for settings access */
	mutable FCriticalSection CriticalSection;
	TArray<TGsMessage<T1, T2>> _AsyncMessage;
public:
	void RemoveAll() { _delieveryAddr.Empty(); }
	//-------------------------------------------------------------------------
	// 메세지 등록
	virtual void InsertMessage() {};
	TMap<T1, MessageType>& GetDeliveryAddress() { return _delieveryAddr; }

	// 메시지 외부등록
	// ex) _messagehandler.AddRaw(MessageSystem::ID::RECONNECT_END, this, &GSFGameModeManager::OnReconnectionEnd);
	template <typename UserClass, typename... VarTypes>
	FDelegateHandle AddRaw(T1 Message, UserClass* InUserObject,
		typename TMemFunPtrType<false, UserClass, void(const T2&, VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TGsMessageHandlerOneParam<T1, T2>::MessageType delFunc;
		auto Result = delFunc.AddRaw(InUserObject, InFunc);
		_delieveryAddr.Add(Message, delFunc);
		return Result;
	}

	//-------------------------------------------------------------------------
	// 메세지 전송(Sync 용)
	virtual void SendMessage(const T1& id, const T2& inData)
	{
		auto delegateFunc = _delieveryAddr.FindRef(id);
		delegateFunc.Broadcast(inData);
	}

	virtual void SendMessage(T1&& id, T2&& inData)
	{
		auto delegateFunc = _delieveryAddr.FindRef(id);
		delegateFunc.Broadcast(inData);
	}
	//-------------------------------------------------------------------------
	// 메세지 전송(ASync 용) : 다른 Thread에서 메인쓰레드로 요청할때( Hive, 다운로드, ThreadJob등)
	// 큐에 메시지를 담고 메인루프에서 꺼낸다음 일괄호출하는 방식사용
	virtual void SendMessageAsync(T1& id, const T2& inData)
	{
		FScopeLock InsertLock(&CriticalSection);
		_AsyncMessage.Add(TGsMessage<T1, T2>(id, inData));
	}

	virtual void SendMessageAsync(T1&& id, T2&& inData)
	{
		FScopeLock InsertLock(&CriticalSection);
		_AsyncMessage.Add(TGsMessage<T1, T2>(std::move(id), std::move(inData)));
	}

	virtual void Update()
	{
		TArray<TGsMessage<T1, T2>> copyQue;
		{
			FScopeLock InsertLock(&CriticalSection);
			copyQue = _AsyncMessage;
			_AsyncMessage.Empty();
		}

		for (auto & message : copyQue)
		{
			SendMessage(message.GetId(), message.GetData());
		}
	}
};