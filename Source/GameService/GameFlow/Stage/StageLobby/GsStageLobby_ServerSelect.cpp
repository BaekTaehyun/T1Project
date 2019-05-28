#include "GsStageLobby_ServerSelect.h"
#include "GameService.h"
#include "Message/GsMessageManager.h"


FGsStageLobby_ServerSelect::FGsStageLobby_ServerSelect() : FGsStageLobbyBase(FGsStageMode::Lobby::SERVER_SELECT)
{
}

FGsStageLobby_ServerSelect::~FGsStageLobby_ServerSelect()
{
	GSLOG(Warning, TEXT("FGsStageLobby_ServerSelect : ~GSFStageLobby_ServerSelect"));
}

void FGsStageLobby_ServerSelect::Enter()
{
	FGsStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("FGsStageLobby_ServerSelect : Enter"));

	GMessage()->GetStage().SendMessage(MessageLobby::Stage::ENTER_SERVERSELECT_STAGE);
}

void FGsStageLobby_ServerSelect::Exit()
{
	FGsStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("FGsStageLobby_ServerSelect : Exit"));
}

void FGsStageLobby_ServerSelect::Update()
{
	FGsStageLobbyBase::Update();
	GSLOG(Warning, TEXT("FGsStageLobby_ServerSelect : Update"));
}