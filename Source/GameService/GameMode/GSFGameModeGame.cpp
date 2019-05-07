#include "GSFGameModeGame.h"
#include "GameService.h"

GSFGameModeGame::GSFGameModeGame() : GSFGameModeBase(GSFGameMode::Mode::GAME)
{
}

GSFGameModeGame::~GSFGameModeGame()
{
	GSLOG(Warning, TEXT("GSFGameModeGame : ~GSFGameModeGame"));
}

void GSFGameModeGame::Enter()
{
	GSLOG(Warning, TEXT("GSFGameModeGame : Enter"));
	_stageManager = TUniquePtr<GSStageManagerGame>(new GSStageManagerGame());
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->InitState();
	}

}

void GSFGameModeGame::Exit()
{
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->RemoveAll();
	}

	GSLOG(Warning, TEXT("GSFGameModeGame : Exit"));
}

void GSFGameModeGame::Update()
{
	GSLOG(Warning, TEXT("GSFGameModeGame : Update"));
}