#pragma once
#include "../GSFStageMode.h"
#include "../../../Class/GSTState.h"

//------------------------------------------------------------------------------
// �ΰ��� ���������� �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
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
