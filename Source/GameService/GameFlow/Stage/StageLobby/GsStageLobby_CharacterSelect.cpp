#include "GsStageLobby_CharacterSelect.h"
#include "GameService.h"
#include "Message/GsMessageManager.h"


FGsStageLobby_CharacterSelect::FGsStageLobby_CharacterSelect() : FGsStageLobbyBase(FGsStageMode::Lobby::CAHRACTER_SELECT)
{
}

FGsStageLobby_CharacterSelect::~FGsStageLobby_CharacterSelect()
{
	GSLOG(Warning, TEXT("FGsStageLobby_CharacterSelect : ~GSFStageLobby_Intro"));
}

void FGsStageLobby_CharacterSelect::Enter()
{
	FGsStageLobbyBase::Enter();
	GSLOG(Warning, TEXT("GSFStageLobby_CharacterSelect : Enter"));

	GMessage()->GetStage().SendMessage(MessageLobby::Stage::ENTER_CHARACTERSELECT_STAGE);
}

void FGsStageLobby_CharacterSelect::Exit()
{
	FGsStageLobbyBase::Exit();
	GSLOG(Warning, TEXT("FGsStageLobby_CharacterSelect : Exit"));
}

void FGsStageLobby_CharacterSelect::Update()
{
	FGsStageLobbyBase::Update();
	GSLOG(Warning, TEXT("FGsStageLobby_CharacterSelect : Update"));
}