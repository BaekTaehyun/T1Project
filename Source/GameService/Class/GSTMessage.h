#pragma once
//------------------------------------------------------------------------------
// �̺�Ʈ�� �����ϱ����� �⺻��üŬ����
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
// �����Ͱ� ���� �޽����� ���������� ����
//------------------------------------------------------------------------------
template<typename T1>
class GSTMessageOnly : GSTMessage<T1, GTSMessageNone>
{
public :
	virtual ~GSTMessageOnly() {}
};