#pragma once
#include "GameService.h"
//------------------------------------------------------------------------------
// 이벤트를 전달하기위한 기본객체클래스
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class TGsMessage
{
	T1 _id;
	T2 _data;

public:
	TGsMessage(TGsMessage&& other) : _id(Forward<T1>(other._id)), _data(Forward<T2>(other._data))
	{
		GSLOG(Warning, TEXT("TGsMessage : TGsMessage(GSTMessage&& other)"));
	}
	TGsMessage(const TGsMessage<T1, T2>& other) : _id(other._id), _data(other._data)
	{
		GSLOG(Warning, TEXT("TGsMessage : TGsMessage(const GSTMessage<T1, T2>& other)"));
	}
	TGsMessage(T1&& id, T2&& data) : _id(MoveTemp<T1>(id)), _data(MoveTemp<T2>(data))
	{
		GSLOG(Warning, TEXT("TGsMessage : TGsMessage(T1&& id, T2&& data)"));
	}
	TGsMessage(T1& id, T2& data) : _id(id), _data(data)
	{
		GSLOG(Warning, TEXT("TGsMessage : TGsMessage(T1& id, T2& data)"));
	}

	virtual ~TGsMessage() {}
	T1& GetId()	{ return _id; }
	T2& GetData() { return _data; }
};	

