#include "GSFStageLobby_CharacterPreview.h"
#include "GameService.h"

GSFStageLobby_CharecterPreview::GSFStageLobby_CharecterPreview() : GSFStageLobbyBase(GSFStageMode::Lobby::CHARACTER_PREVIEW)
{
}

GSFStageLobby_CharecterPreview::~GSFStageLobby_CharecterPreview()
{
	GSLOG(Warning, TEXT("GSFStageLobby_CharecterPreview : ~GSFStageLobby_CharecterPreview"));
}

void GSFStageLobby_CharecterPreview::Enter()
{
	GSFStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("GSFStageLobby_Intro : Enter"));
}

void GSFStageLobby_CharecterPreview::Exit()
{
	GSFStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("GSFStageLobby_Intro : Exit"));
}

void GSFStageLobby_CharecterPreview::Update()
{
	GSFStageLobbyBase::Update();
	GSLOG(Warning, TEXT("GSFStageLobby_Intro : Update"));
}