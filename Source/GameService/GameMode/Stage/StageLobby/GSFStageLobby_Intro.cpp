#include "GSFStageLobby_Intro.h"
#include "GameService.h"

GSFStageLobby_Intro::GSFStageLobby_Intro() : GSFStageLobbyBase(GSFStageMode::Lobby::INTRO)
{
}

GSFStageLobby_Intro::~GSFStageLobby_Intro()
{
	GSLOG(Warning, TEXT("GSFStageLobby_Intro : ~GSFStageLobby_Intro"));
}

void GSFStageLobby_Intro::Enter()
{
	GSFStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("GSFStageLobby_Intro : Enter"));
}

void GSFStageLobby_Intro::Exit()
{
	GSFStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("GSFStageLobby_Intro : Exit"));
}

void GSFStageLobby_Intro::Update()
{
	GSFStageLobbyBase::Update();
	GSLOG(Warning, TEXT("GSFStageLobby_Intro : Update"));
}