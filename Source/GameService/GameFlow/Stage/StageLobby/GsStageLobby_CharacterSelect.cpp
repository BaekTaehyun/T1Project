#include "GsStageLobby_CharacterSelect.h"
#include "GameService.h"
#include "UI/GsUIManager.h"
#include "BPFunction/GSBluePrintEnumLobby.h"
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
			auto WidgetClass = GameModeLobby->GetWidgetClass(EGS_LOBBY_WIDGET_Enum::GS_LOBBY_WIDGET_CHARACTER_SELECT);
			if (nullptr != WidgetClass)
			{
				UIManager->Push(WidgetClass);
			}
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