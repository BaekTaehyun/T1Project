#include "GsGameModeGame.h"
#include "GameService.h"
#include "./Stage/StageGame/GsStageManagerGame.h"

FGsGameModeGame::FGsGameModeGame() : FGsGameModeBase(FGsGameMode::Mode::GAME)
{
}

FGsGameModeGame::~FGsGameModeGame()
{
	GSLOG(Warning, TEXT("FGsGameModeGame : ~FGsGameModeGame"));
}

void FGsGameModeGame::Enter()
{
	GSLOG(Warning, TEXT("FGsGameModeGame : Enter"));
	_stageManager = TUniquePtr<FGsStageManagerGame>(new FGsStageManagerGame());
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->InitState();
	}

}

void FGsGameModeGame::Exit()
{
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->RemoveAll();
	}

	GSLOG(Warning, TEXT("FGsGameModeGame : Exit"));
}

void FGsGameModeGame::Update()
{
	GSLOG(Warning, TEXT("FGsGameModeGame : Update"));
}

void FGsGameModeGame::OnReconnectionStart()
{
	GSLOG(Warning, TEXT("FGsGameModeGame : OnReconectionStart"));
}

void FGsGameModeGame::OnReconnectionEnd()
{
	GSLOG(Warning, TEXT("FGsGameModeGame : OnReconectionEnd"));
}