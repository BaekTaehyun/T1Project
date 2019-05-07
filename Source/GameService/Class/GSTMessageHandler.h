#pragma once
#include "GSTMessage.h"
#include "CoreMinimal.h"

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

	//-------------------------------------------------------------------------
	// �޼��� ����
	virtual void SendMessage(GSTMessage<T1, T2>& inMessage)
	{
		auto delegateFunc = _delieveryAddr.FindRef(inMessage.GetId());
		delegateFunc.Broadcast(inMessage.GetData());
	}		
};