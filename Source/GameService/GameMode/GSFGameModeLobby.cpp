#include "GSFGameModeLobby.h"
#include "GameService.h"
	
GSFGameModeLobby::GSFGameModeLobby() : GSFGameModeBase(GSFGameMode::Mode::LOBBY)
{
}

GSFGameModeLobby::~GSFGameModeLobby()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : ~GSFGameModeLobby"));
}

void GSFGameModeLobby::Enter()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : Enter"));

	_stageManager = TUniquePtr<GSStageManagerLobby>(new GSStageManagerLobby());
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->InitState();
	}
}

void GSFGameModeLobby::Exit()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : Exit"));
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->RemoveAll();
	}
}

void GSFGameModeLobby::Update()
{
	GSLOG(Warning, TEXT("GSFGameModeLobby : Update"));
}