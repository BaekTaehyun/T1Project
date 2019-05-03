#pragma once

#include "GSFGameMode.h"
#include "../Class/GSTState.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFGameModeBase : public GSTState<GSFGameMode::Mode>
{
public:
	GSFGameModeBase() : GSTState<GSFGameMode::Mode>(GSFGameMode::Mode::MAX) {}
	GSFGameModeBase(GSFGameMode::Mode inMode) : GSTState<GSFGameMode::Mode>(inMode) {}
	virtual ~GSFGameModeBase() {}
	virtual void Enter() override{};
	virtual void Exit() override {};
	virtual void Update() override {};
};