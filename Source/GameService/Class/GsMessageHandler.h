#pragma once
#include "GsMessage.h"
#include "CoreMinimal.h"
#include "DelegateInstanceInterface.h"

//------------------------------------------------------------------------------
// �̺�Ʈ�� �����ϱ����� �⺻��üŬ����
// GSTMessage �� ���� ���δ�.
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class TGsMessageHandler
{
public:
	virtual ~TGsMessageHandler() { RemoveAll(); }
	DECLARE_EVENT_OneParam(TGsMessageHandler, MessageType, const T2&)

private:
	TMap<T1, MessageType>	_delieveryAddr;
	/** A critical section for settings access */
	mutable FCriticalSection CriticalSection;
	TArray<TGsMessage<T1, T2>> _AsyncMessage;
public:
	void RemoveAll() { _delieveryAddr.Empty(); }
	//-------------------------------------------------------------------------
	// �޼��� ���
	virtual void InsertMessage() {};
	TMap<T1, MessageType>& GetDeliveryAddress() { return _delieveryAddr; }

	// �޽��� �ܺε��
	// ex) _messagehandler.AddRaw(MessageSystem::ID::RECONNECT_END, this, &GSFGameModeManager::OnReconnectionEnd);
	template <typename UserClass, typename... VarTypes>
	FDelegateHandle AddRaw(T1 Message, UserClass* InUserObject,
		typename TMemFunPtrType<false, UserClass, void(const T2&, VarTypes...)>::Type InFunc, VarTypes... Vars)
	{
		TGsMessageHandler<T1, T2>::MessageType delFunc;
		auto Result = delFunc.AddRaw(InUserObject, InFunc);
		_delieveryAddr.Add(Message, delFunc);
		return Result;
	}

	//-------------------------------------------------------------------------
	// �޼��� ����(Sync ��)
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
	// �޼��� ����(ASync ��) : �ٸ� Thread���� ���ξ������ ��û�Ҷ�( Hive, �ٿ�ε�, ThreadJob��)
	// ť�� �޽����� ��� ���η������� �������� �ϰ�ȣ���ϴ� ��Ļ��
	virtual void SendMessageAsync(T1& id, T2& inData)
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