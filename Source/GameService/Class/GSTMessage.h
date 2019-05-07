#pragma once
//------------------------------------------------------------------------------
// 이벤트를 전달하기위한 기본객체클래스
//------------------------------------------------------------------------------
template<typename T1, typename T2>
class GSTMessage<T1, T2>
{
	T1 _id;
	T2 _data;

public:
	virtual ~GSTMessage() {}
	const T1& GetId()	{ return _id; }
	const T2& GetData() { return _data; }
};	

//------------------------------------------------------------------------------
class GTSMessageNone
{
};

//------------------------------------------------------------------------------
// 데이터가 없는 메시지만 던지기위한 구조
//------------------------------------------------------------------------------
template<typename T1>
class GSTMessageOnly : GSTMessage<T1, GTSMessageNone>
{
public :
	virtual ~GSTMessageOnly() {}
};