#pragma once
#include "GameService.h"
//------------------------------------------------------------------------------
// 이벤트를 전달하기위한 기본객체클래스
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class GSTMessage
{
	T1 _id;
	T2 _data;

public:
	GSTMessage(GSTMessage&& other) : _id(std::move(other.id)), _data(std::move(other._data))
	{
		GSLOG(Warning, TEXT("GSTMessage : GSTMessage(GSTMessage&& other)"));
	}
	GSTMessage(T1&& id, T2&& data) : _id(std::move(id)), _data(std::move(data))
	{
		GSLOG(Warning, TEXT("GSTMessage : GSTMessage(T1&& id, T2&& data)"));
	}
	GSTMessage(T1& id, T2& data) : _id(id), _data(data)
	{
		GSLOG(Warning, TEXT("GSTMessage : GSTMessage(T1& id, T2& data)"));
	}

	GSTMessage& operator = (GSTMessage&& other)
	{
		GSLOG(Warning, TEXT("GSTMessage& operator = (GSTMessage&& other)"));

		_id = std::move(other._id);
		_data = std::move(other._data);
		return *this;
	}

	GSTMessage& operator = (GSTMessage& other)
	{
		GSLOG(Warning, TEXT("CSTMessage& operator = (GSTMessage& other)"));
		_id = other._id;
		_data = other._data;
		return *this;
	}

	virtual ~GSTMessage() {}
	const T1& GetId()	{ return _id; }
	const T2& GetData() { return _data; }
};	

//------------------------------------------------------------------------------
// 메세지 온리 처리를 위한 클래스
//------------------------------------------------------------------------------
class GTSMessageNone
{
public:
	GTSMessageNone()
	{
		GSLOG(Warning, TEXT("GTSMessageNone"));
	};
	GTSMessageNone(GTSMessageNone& other)
	{
		GSLOG(Warning, TEXT("GTSMessageNone(GTSMessageNone& other)"));
	}
	GTSMessageNone(GTSMessageNone&& other)
	{
		GSLOG(Warning, TEXT("GTSMessageNone(GTSMessageNone&& other)"));
	}
	GTSMessageNone& operator = (GTSMessageNone&& other)
	{
		GSLOG(Warning, TEXT("GTSMessageNone& operator = (GTSMessageNone&& other)"));
		return *this;
	}
	GTSMessageNone& operator = (GTSMessageNone& other)
	{
		GSLOG(Warning, TEXT("GTSMessageNone& operator = (GTSMessageNone& other)"));
		return *this;
	}
};

