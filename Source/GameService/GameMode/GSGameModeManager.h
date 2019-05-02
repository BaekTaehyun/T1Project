#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "GSFGameMode.h"
#include "GSFGameModeLobby.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
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
// 게임모드를 관리
//------------------------------------------------------------------------------
class GSGameModeManager : public GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>
{
public:
	virtual ~GSGameModeManager();
	virtual void RemoveAll() override;
	virtual void InitState();
};