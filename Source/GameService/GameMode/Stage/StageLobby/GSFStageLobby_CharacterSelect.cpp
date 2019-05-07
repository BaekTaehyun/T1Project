#include "GSFStageLobby_CharacterSelect.h"
#include "GameService.h"

GSFStageLobby_CharacterSelect::GSFStageLobby_CharacterSelect() : GSFStageLobbyBase(GSFStageMode::Lobby::CAHRACTER_SELECT)
{
}

GSFStageLobby_CharacterSelect::~GSFStageLobby_CharacterSelect()
{
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterSelect : ~GSFStageLobby_Intro"));
}

void GSFStageLobby_CharacterSelect::Enter()
{
	GSFStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterSelect : Enter"));
}

void GSFStageLobby_CharacterSelect::Exit()
{
	GSFStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterSelect : Exit"));
}

void GSFStageLobby_CharacterSelect::Update()
{
	GSFStageLobbyBase::Update();
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterSelect : Update"));
}