#include "GsGameFlowLobby.h"
#include "GameService.h"
#include "./Stage/StageLobby/GsStageManagerLobby.h"
FGsGameFlowLobby::FGsGameFlowLobby() : FGsGameFlowBase(FGsGameFlow::Mode::LOBBY)
{
}

FGsGameFlowLobby::~FGsGameFlowLobby()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : ~FGsGameModeLobby"));
}

void FGsGameFlowLobby::Enter()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : Enter"));

	_stageManager = TUniquePtr<FGsStageManagerLobby>(new FGsStageManagerLobby());
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->InitState();
	}
}

void FGsGameFlowLobby::Exit()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : Exit"));
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->RemoveAll();
	}
}

void FGsGameFlowLobby::Update()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : Update"));
}

void FGsGameFlowLobby::OnReconnectionStart()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnReconectionStart"));
}

void FGsGameFlowLobby::OnReconnectionEnd()
{
	GSLOG(Warning, TEXT("FGsGameFlowLobby : OnReconectionEnd"));
}