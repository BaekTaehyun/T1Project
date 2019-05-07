#pragma once
#include "../GSFStageMode.h"
#include "../../../Class/GSTState.h"

//------------------------------------------------------------------------------
// 인게임 스테이지를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFStageGameBase : public GSTState<GSFStageMode::Game>
{
public:
	GSFStageGameBase() : GSTState<GSFStageMode::Game>(GSFStageMode::Game::GAME_MAX) {}
	GSFStageGameBase(GSFStageMode::Game inMode) : GSTState<GSFStageMode::Game>(inMode) {}
	virtual ~GSFStageGameBase() {}
	virtual void Enter() override {};
	virtual void Exit() override {};
	virtual void Update() override {};
};
