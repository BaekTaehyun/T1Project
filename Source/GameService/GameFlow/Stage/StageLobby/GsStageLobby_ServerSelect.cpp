#include "GsStageLobby_ServerSelect.h"
#include "GameService.h"
#include "UI/GsUIManager.h"
#include "BPFunction/GSBluePrintEnumLobby.h"
#include "GameMode/GsGameModeLobby.h"


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

	AGsGameModeLobby* GameModeLobby = AGsGameModeLobby::GetGameModeLobby();
	if (nullptr != GameModeLobby)
	{
		AGsUIManager* UIManager = GameModeLobby->GetUIManager();
		if (nullptr != UIManager)
		{
			auto WidgetClass = GameModeLobby->GetWidgetClass(EGS_LOBBY_WIDGET_Enum::GS_LOBBY_WIDGET_SERVER_SELECT);
			if (nullptr != WidgetClass)
			{
				UIManager->Push(WidgetClass);
			}
		}
	}
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