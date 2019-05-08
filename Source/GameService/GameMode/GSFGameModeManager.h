#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "../Message/GSFMessageHandlerSystem.h"
#include "GSFGameMode.h"
#include "GSFGameModeBase.h"
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFGameModeAllocator : GSTMapAllocator<GSFGameMode::Mode, GSFGameModeBase>
{
public:
	GSFGameModeAllocator() {}
	virtual ~GSFGameModeAllocator() {}
	virtual GSFGameModeBase* Alloc(GSFGameMode::Mode inMode) override;	
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
class GSFGameModeManager : public GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>
{
	typedef GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator> Super;
	GSFMessageHandlerSystem		_messagehandler;
public:
	virtual ~GSFGameModeManager() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;

	void OnReconnectionStart(const GTSMessageNone& message);
	void OnReconnectionEnd(const GTSMessageNone& message);
};