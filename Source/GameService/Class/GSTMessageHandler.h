#pragma once
#include "GSTMessage.h"
#include "CoreMinimal.h"
#include "DelegateInstanceInterface.h"

//------------------------------------------------------------------------------
// �̺�Ʈ�� �����ϱ����� �⺻��üŬ����
// GSTMessage �� ���� ���δ�.
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class GSTMessageHandler
{
public:
	virtual ~GSTMessageHandler() {}
	DECLARE_EVENT_OneParam(GSTMessageHandler, MessageType, const T2&)

private:
	TMap<T1, MessageType>	_delieveryAddr;	
public:
	void RemoveAll() { _delieveryAddr.Empty(); }
	//-------------------------------------------------------------------------
	// �޼��� ���
	virtual void InsertMessage() {};
	TMap<T1, MessageType>& GetDeliveryAddress() { return _delieveryAddr; }

	// �޽��� �ܺε��
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
	// �޼��� ����
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