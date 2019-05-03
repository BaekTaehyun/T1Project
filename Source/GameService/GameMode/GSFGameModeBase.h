#pragma once

#include "GSFGameMode.h"
#include "../Class/GSTState.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
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