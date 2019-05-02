#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "GSFGameMode.h"
#include "GSFGameModeLobby.h"



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


class GSGameModeManager : public GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>
{
public:
	virtual ~GSGameModeManager();
	virtual void RemoveAll() override;
	virtual void InitState();
};