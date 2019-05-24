#include "GsStageLobby_CharacterSelect.h"
#include "GameService.h"
#include "UI/GsUIManager.h"
#include "GameMode/GsGameModeLobby.h"


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

	AGsGameModeLobby* GameModeLobby = AGsGameModeLobby::GetGameModeLobby();
	if (nullptr != GameModeLobby)
	{
		AGsUIManager* UIManager = GameModeLobby->GetUIManager();
		if (nullptr != UIManager)
		{
			UIManager->PushByKeyName(FName(TEXT("WindowCharacterSelect")));
		}
	}
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