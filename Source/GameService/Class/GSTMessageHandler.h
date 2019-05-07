#pragma once
#include "GSTMessage.h"
#include "CoreMinimal.h"

//------------------------------------------------------------------------------
// �̺�Ʈ�� �����ϱ����� �⺻��üŬ����
// GSTMessage �� ���� ���δ�.
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class GSTMessageHandler<T1, T2>
{
	DECLARE_DELEGATE_OneParam(MessageType, T2&)
	TMap<T1, MessageType>	_delieveryAddr;
public:
	virtual void InsertMessage() = 0;
	virtual void SendMessage(GSTMessage<T1, T2>& inMessage)
	{
		auto delegateFunc = _delieveryAddr.Find(inMessage.GetId());
		delegateFunc->(inMessage.GetData());
	}
	TMap<T1, MessageType>& GetDeliveryAddress() { return _delieveryAddr; }
	void RemoveAll() { _delieveryAddr.Clear(); }
};