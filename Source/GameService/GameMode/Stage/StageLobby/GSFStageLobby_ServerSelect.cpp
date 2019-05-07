#include "GSFStageLobby_ServerSelect.h"
#include "GameService.h"

GSFStageLobby_ServerSelect::GSFStageLobby_ServerSelect() : GSFStageLobbyBase(GSFStageMode::Lobby::SERVER_SELECT)
{
}

GSFStageLobby_ServerSelect::~GSFStageLobby_ServerSelect()
{
	GSLOG(Warning, TEXT("GSFStageLobby_ServerSelect : ~GSFStageLobby_ServerSelect"));
}

void GSFStageLobby_ServerSelect::Enter()
{
	GSFStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("GSFStageLobby_ServerSelect : Enter"));
}

void GSFStageLobby_ServerSelect::Exit()
{
	GSFStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("GSFStageLobby_ServerSelect : Exit"));
}

void GSFStageLobby_ServerSelect::Update()
{
	GSFStageLobbyBase::Update();
	GSLOG(Warning, TEXT("GSFStageLobby_ServerSelect : Update"));
}