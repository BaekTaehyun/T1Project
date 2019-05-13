#include "GsGameModeLobby.h"
#include "GameService.h"
#include "./Stage/StageLobby/GsStageManagerLobby.h"
FGsGameModeLobby::FGsGameModeLobby() : FGsGameModeBase(FGsGameMode::Mode::LOBBY)
{
}

FGsGameModeLobby::~FGsGameModeLobby()
{
	GSLOG(Warning, TEXT("FGsGameModeLobby : ~FGsGameModeLobby"));
}

void FGsGameModeLobby::Enter()
{
	GSLOG(Warning, TEXT("FGsGameModeLobby : Enter"));

	_stageManager = TUniquePtr<FGsStageManagerLobby>(new FGsStageManagerLobby());
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->InitState();
	}
}

void FGsGameModeLobby::Exit()
{
	GSLOG(Warning, TEXT("FGsGameModeLobby : Exit"));
	if (_stageManager.IsValid())
	{
		_stageManager.Get()->RemoveAll();
	}
}

void FGsGameModeLobby::Update()
{
	GSLOG(Warning, TEXT("FGsGameModeLobby : Update"));
}

void FGsGameModeLobby::OnReconnectionStart()
{
	GSLOG(Warning, TEXT("FGsGameModeLobby : OnReconectionStart"));
}

void FGsGameModeLobby::OnReconnectionEnd()
{
	GSLOG(Warning, TEXT("FGsGameModeLobby : OnReconectionEnd"));
}