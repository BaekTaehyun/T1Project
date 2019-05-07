#include "GSFStageLobby_CharacterCreate.h"
#include "GameService.h"

GSFStageLobby_CharacterCreate::GSFStageLobby_CharacterCreate() : GSFStageLobbyBase(GSFStageMode::Lobby::CHARACTER_CREATE)
{
}

GSFStageLobby_CharacterCreate::~GSFStageLobby_CharacterCreate()
{
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterCreate : ~GSFStageLobby_CharacterCreate"));
}

void GSFStageLobby_CharacterCreate::Enter()
{
	GSFStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterCreate : Enter"));
}

void GSFStageLobby_CharacterCreate::Exit()
{
	GSFStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterCreate : Exit"));
}

void GSFStageLobby_CharacterCreate::Update()
{
	GSFStageLobbyBase::Update();
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterCreate : Update"));
}