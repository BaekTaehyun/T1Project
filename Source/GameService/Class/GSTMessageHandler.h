#pragma once
#include "GSTMessage.h"
#include "CoreMinimal.h"
#include "DelegateInstanceInterface.h"

//------------------------------------------------------------------------------
// 이벤트를 전달하기위한 기본객체클래스
// GSTMessage 와 같이 쓰인다.
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class GSTMessageHandler
{
public:
	virtual ~GSTMessageHandler() { RemoveAll(); }
	DECLARE_EVENT_OneParam(GSTMessageHandler, MessageType, const T2&)

private:
	TMap<T1, MessageType>	_delieveryAddr;	
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
		GSTMessageHandler<T1, T2>::MessageType delFunc;
		auto Result = delFunc.AddRaw(InUserObject, InFunc);
		_delieveryAddr.Add(Message, delFunc);
		return Result;
	}

	//-------------------------------------------------------------------------
	// 메세지 전송
	virtual void SendMessage(GSTMessage<T1, T2>& inMessage)
	{
		auto delegateFunc = _delieveryAddr.FindRef(inMessage.GetId());
		delegateFunc.Broadcast(inMessage.GetData());
	}
	virtual void SendMessageC(GSTMessage<T1, T2> inMessage)
	{
		auto delegateFunc = _delieveryAddr.FindRef(inMessage.GetId());
		delegateFunc.Broadcast(inMessage.GetData());
	}

	virtual void SendMessage(T1& id, T2& inData)
	{
		auto delegateFunc = _delieveryAddr.FindRef(id);
		delegateFunc.Broadcast(inData);
	}

	virtual void SendMessage(T1&& id, T2&& inData)
	{
		auto delegateFunc = _delieveryAddr.FindRef(id);
		delegateFunc.Broadcast(inData);
	}
};