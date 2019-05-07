#pragma once
#include "GSTMessage.h"
#include "CoreMinimal.h"

//------------------------------------------------------------------------------
// 이벤트를 전달하기위한 기본객체클래스
// GSTMessage 와 같이 쓰인다.
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
	// 메세지 등록
	virtual void InsertMessage() {};
	TMap<T1, MessageType>& GetDeliveryAddress() { return _delieveryAddr; }

	//-------------------------------------------------------------------------
	// 메세지 전송
	virtual void SendMessage(GSTMessage<T1, T2>& inMessage)
	{
		auto delegateFunc = _delieveryAddr.FindRef(inMessage.GetId());
		delegateFunc.Broadcast(inMessage.GetData());
	}		
};