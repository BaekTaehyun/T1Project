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
	virtual void Enter() override
	{
		GSLOG(Warning, TEXT("GSFGameModeBase : Enter"));
	};
	virtual void Exit() override {};
	virtual void Update() override {};
};

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFGameModeLobby : public GSFGameModeBase
{
public:
	GSFGameModeLobby() : GSFGameModeBase(GSFGameMode::Mode::LOBBY)
	{
		GSLOG(Warning, TEXT("GSFGameModeLobby : GSFGameModeLobby"));
	}
	virtual ~GSFGameModeLobby()
	{
		GSLOG(Warning, TEXT("GSFGameModeLobby : ~GSFGameModeLobby"));
	}
	virtual void Enter() override 
	{
		GSLOG(Warning, TEXT("GSFGameModeLobby : Enter"));
	}
	virtual void Exit() override 
	{
		GSLOG(Warning, TEXT("GSFGameModeLobby : Enter"));
	}
	virtual void Update() override {}
};

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 메모리 할당관리자(상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFGameModeGame : public GSFGameModeBase
{
public:
	GSFGameModeGame() : GSFGameModeBase(GSFGameMode::Mode::GAME)
	{
		GSLOG(Warning, TEXT("GSFGameModeGame : GSFGameModeGame"));
	}
	virtual ~GSFGameModeGame()
	{
		GSLOG(Warning, TEXT("GSFGameModeGame : ~GSFGameModeGame"));
	}
	virtual void Enter() override {}
	virtual void Exit() override {}
	virtual void Update() override {}
};