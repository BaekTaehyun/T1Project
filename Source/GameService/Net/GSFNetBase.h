#pragma once
#include "GSFNet.h"
#include "../Class/GSTState.h"
#include "../Class/GSTMessageHandler.h"

#include "./Network/Session.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFNetModeBase : public GSTState<GSFNet::Mode>
{
	std::shared_ptr<Session> session_;  // 서버 구조에 맞춤
public:
	GSFNetModeBase() : GSTState<GSFNet::Mode>(GSFNet::Mode::MAX) {}
	GSFNetModeBase(GSFNet::Mode inMode) : GSTState<GSFNet::Mode>(inMode) {}
	virtual ~GSFNetModeBase() {}
	virtual void Enter() override {};
	virtual void Exit() override {};
	virtual void Update() override {};
};