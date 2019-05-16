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
	TGsMessage(TGsMessage&& other) : _id(std::move(other._id)), _data(std::move(other._data))
	{
		GSLOG(Warning, TEXT("TGsMessage : TGsMessage(GSTMessage&& other)"));
	}
	TGsMessage(const TGsMessage<T1, T2>& other) : _id(other._id), _data(other._data)
	{
		GSLOG(Warning, TEXT("TGsMessage : TGsMessage(const GSTMessage<T1, T2>& other)"));
	}
	TGsMessage(T1&& id, T2&& data) : _id(std::move(id)), _data(std::move(data))
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

//------------------------------------------------------------------------------
// 메세지 온리 처리를 위한 클래스
//------------------------------------------------------------------------------
class FGsMessageNone
{
public:
	FGsMessageNone()
	{
		GSLOG(Warning, TEXT("FGsMessageNone"));
	};
	
	FGsMessageNone(const FGsMessageNone& other)
	{
		GSLOG(Warning, TEXT("FGsMessageNone(FGsMessageNone& other)"));
	}
	FGsMessageNone(FGsMessageNone&& other)
	{
		GSLOG(Warning, TEXT("FGsMessageNone(FGsMessageNone&& other)"));
	}	
};

