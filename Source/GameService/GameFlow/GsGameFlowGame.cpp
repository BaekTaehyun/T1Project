#include "GsGameFlowGame.h"
#include "GameService.h"
#include "./Stage/StageGame/GsStageManagerGame.h"

FGsGameFlowGame::FGsGameFlowGame() : FGsGameFlowBase(FGsGameFlow::Mode::GAME)
{
}

FGsGameFlowGame::~FGsGameFlowGame()
{
	GSLOG(Warning, TEXT("FGsGameFlowGame : ~FGsGameFlowGame"));
}

void FGsGameFlowGame::Enter()
{
	GSLOG(Warning, TEXT("FGsGameFlowGame : Enter"));
	_stageManager = TUniquePtr<FGsStageManagerGame>(new FGsStageManagerGame());
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->InitState();
	}

}

void FGsGameFlowGame::Exit()
{
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->RemoveAll();
	}

	GSLOG(Warning, TEXT("FGsGameFlowGame : Exit"));
}

void FGsGameFlowGame::Update()
{
	GSLOG(Warning, TEXT("FGsGameFlowGame : Update"));
}

void FGsGameFlowGame::OnReconnectionStart()
{
	GSLOG(Warning, TEXT("FGsGameFlowGame : OnReconectionStart"));
}

void FGsGameFlowGame::OnReconnectionEnd()
{
	GSLOG(Warning, TEXT("FGsGameFlowGame : OnReconectionEnd"));
}