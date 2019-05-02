#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "GSFGameMode.h"
#include "GSFGameModeLobby.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFGameModeAllocator : GSTMapAllocator<GSFGameMode::Mode, GSFGameModeBase>
{
public:
	GSFGameModeAllocator() {}
	virtual ~GSFGameModeAllocator() {}
	virtual GSFGameModeBase* Alloc(GSFGameMode::Mode inMode) override
	{
		if (inMode == GSFGameMode::Mode::LOBBY)
		{
			return new GSFGameModeLobby();
		}
		else if (inMode == GSFGameMode::Mode::GAME)
		{
			return new GSFGameModeGame();
		}
		return NULL;
	}
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
class GSGameModeManager : public GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>
{
public:
	virtual ~GSGameModeManager() {};
	virtual void RemoveAll() override;
	virtual void InitState();
};