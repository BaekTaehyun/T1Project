#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "../Message/GSFMessageHandlerSystem.h"
#include "GSFGameMode.h"
#include "GSFGameModeBase.h"
//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFGameModeAllocator : GSTMapAllocator<GSFGameMode::Mode, GSFGameModeBase>
{
public:
	GSFGameModeAllocator() {}
	virtual ~GSFGameModeAllocator() {}
	virtual GSFGameModeBase* Alloc(GSFGameMode::Mode inMode) override;	
};

//------------------------------------------------------------------------------
// 게임모드를 관리
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