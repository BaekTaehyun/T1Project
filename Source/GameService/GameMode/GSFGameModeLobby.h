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
	virtual void Enter() override
	{
		GSLOG(Warning, TEXT("GSFGameModeBase : Enter"));
	};
	virtual void Exit() override {};
	virtual void Update() override {};
};

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
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
// ���Ӹ�带 �����Ҷ� �ʿ��� �޸� �Ҵ������(��ӱ����� �����ϱ� ����)
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