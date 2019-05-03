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
}

void GSFGameModeGame::Exit()
{
	GSLOG(Warning, TEXT("GSFGameModeGame : Exit"));
}

void GSFGameModeGame::Update()
{
	GSLOG(Warning, TEXT("GSFGameModeGame : Update"));
}