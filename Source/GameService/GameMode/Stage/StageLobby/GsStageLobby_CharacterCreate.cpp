#include "GsStageLobby_CharacterCreate.h"
#include "GameService.h"

FGsStageLobby_CharacterCreate::FGsStageLobby_CharacterCreate() : FGsStageLobbyBase(FGsStageMode::Lobby::CHARACTER_CREATE)
{
}

FGsStageLobby_CharacterCreate::~FGsStageLobby_CharacterCreate()
{
	GSLOG(Warning, TEXT("FGsStageLobby_CharacterCreate : ~FGsStageLobby_CharacterCreate"));
}

void FGsStageLobby_CharacterCreate::Enter()
{
	FGsStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("FGsStageLobby_CharacterCreate : Enter"));
}

void FGsStageLobby_CharacterCreate::Exit()
{
	FGsStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("FGsStageLobby_CharacterCreate : Exit"));
}

void FGsStageLobby_CharacterCreate::Update()
{
	FGsStageLobbyBase::Update();
	GSLOG(Warning, TEXT("FGsStageLobby_CharacterCreate : Update"));
}